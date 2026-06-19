from odmantic import query
from typing import Generic, Optional, Any

from .models import (
    ComprehensiveRequest,
    PaginationResponse,
    PaginationInfo,
    ModelType,
    CommonRequest)
from .helpers import PaginationSupport
from filters import BaseQueryBuilder



class PaginationHandler(Generic[ModelType]):

    # Type hint for parent
    model: type[ModelType]
    engine: Any
    query_builder: type[BaseQueryBuilder] | BaseQueryBuilder | None

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self._pagination_support: Optional[PaginationSupport] = None

    @property
    def pagination_support(self) -> PaginationSupport:
        # backup define
        if self._pagination_support is None:
            self._pagination_support = PaginationSupport(
                model=self.model,
                query_builder=getattr(self, 'query_builder', None)
            )
        return self._pagination_support
    

    def _resolve_query(self, request: ComprehensiveRequest| CommonRequest, query) -> ComprehensiveRequest:
        
        if isinstance(request, ComprehensiveRequest):
            return request
        
        return ComprehensiveRequest(
            **request.model_dump(),
            search_text=query
        )

    async def paginate(
            self,
            user_id: Optional[str],
            request: ComprehensiveRequest | CommonRequest,
            **kwargs
    ) -> tuple[PaginationResponse, dict[str, Any]]:

        """
        Paginate query results.
        
        Args:
            user_id: User ID for filtering
            request: Pagination parameters
            base_query: Additional query conditions (Optional)
            
        Returns:
            Tuple of (PaginationResponse, metadata)
        """
        # ensure completed request:
        extra_query = kwargs.get("query")
        request = self._resolve_query(request, extra_query)
        
        # Build sort expression
        sort_expr = self.pagination_support.build_sort_expression(
            request.sort_field,
            request.sort_direction
        )

        # Build complete query
        completed_query, metadata = await self.pagination_support.build_completed_query(
            advanced_filters=request.advanced_filters,
            search_text=request.search_text,
            user_id=user_id,
            **kwargs
        )

        # Execute pagination (cursor or skip-based)
        if request.cursor:
            cursor_condition = self.pagination_support.build_cursor_condition(request.cursor, request.sort_field, request.sort_direction)
            completed_query = query.and_(cursor_condition, completed_query)
            response = await self._execute_cursor_pagination(request, completed_query, sort_expr)
        else:
            response = await self._execute_skip_pagination(request, completed_query, sort_expr)

        return response, metadata
        
    async def _execute_skip_pagination(
        self,
        request: ComprehensiveRequest,
        query_condition: query.QueryExpression | dict,
        sort_expression: Optional[list[query.QueryExpression]] = None) -> PaginationResponse:
        
        page = request.page or 1
        skip_count = (page - 1) * request.limit

        # Fetch items with skip
        items = await self.engine.find(
            self.model,
            query=query_condition,
            sort=sort_expression,
            skip=skip_count,
            limit=request.limit
        )
        
        # Get total count for skip pagination
        total_count = await self.engine.count(self.model, query_condition)
        
        return PaginationResponse(
            data=items,
            pagination=PaginationInfo(
                total = total_count,
                page= page,
                limit= request.limit,
            )
        )
    
    async def _execute_cursor_pagination(
        self,
        request: ComprehensiveRequest,
        query_condition: query.QueryExpression,
        sort_expression: Optional[list[query.QueryExpression]] = None) -> PaginationResponse:
        
        # Fetch limit + 1 to check for more
        items = await self.engine.find(
            self.model,
            query_condition,
            sort=sort_expression,
            limit=request.limit + 1
        )
        
        # Check for more items
        has_more = len(items) > request.limit
        if has_more:
            items = items[:request.limit]

        # Generate next cursor
        next_cursor = self.pagination_support.generate_next_cursor(items, request.sort_field, has_more)
                
        return PaginationResponse(
            data=items,
            next_cursor=next_cursor,
            has_more=has_more
        )
    
