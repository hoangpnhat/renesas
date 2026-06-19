#  Copyright © 2025 Renesas Electronics Corporation. All Rights Reserved.
from abc import ABC, abstractmethod
from typing import Optional, Dict, Any, Union, TypeVar, Generic, cast
from urllib.parse import urljoin

import httpx

from core.loguru import logger

T = TypeVar('T')


class ResponseParser(ABC, Generic[T]):
    """Abstract base class for response parsing strategies"""

    @abstractmethod
    async def parse(self, response: httpx.Response) -> T:
        pass


class JsonResponseParser(ResponseParser[Dict[str, Any]]):
    """Default JSON response parser"""

    async def parse(self, response: httpx.Response) -> Dict[str, Any]:
        return cast(Dict[str, Any], response.json())


class TextResponseParser(ResponseParser[str]):
    """Text response parser"""

    async def parse(self, response: httpx.Response) -> str:
        return response.text


class BytesResponseParser(ResponseParser[bytes]):
    """Bytes response parser"""

    async def parse(self, response: httpx.Response) -> bytes:
        return response.content


class NoContentResponseParser(ResponseParser[Dict[str, Any]]):
    """Parser for 204 No Content responses"""

    async def parse(self, response: httpx.Response) -> Dict[str, Any]:
        # For 204 No Content, return empty dict or success indicator
        if response.status_code == 204:
            return {"status": True}
        # For other responses with content, try to parse as JSON
        if response.content:
            return cast(Dict[str, Any], response.json())
        return {"status": True}


class BaseHttpClient(Generic[T]):
    def __init__(
            self,
            base_url: str,
            response_parser: ResponseParser[T],
            token: Optional[str] = None,
            auth_scheme: str = "Bearer",
            default_headers: Optional[Dict[str, str]] = None,
            timeout: float = 30.0,
            follow_redirects: bool = True
    ):
        """
        Initialize the BaseHttpClient with the given parameters.
        :param base_url: str - The base URL for the API.
        :param response_parser: ResponseParser[T] - The parser to use for responses.
        :param token: Optional[str] - Optional authentication token.
        :param auth_scheme: str - The authentication scheme to use (default is "Bearer").
        :param default_headers: Optional[Dict[str, str]] - Additional headers to include in every request.
        :param timeout: float - Timeout for requests in seconds (default is 30.0).
        :param follow_redirects: bool - Whether to follow redirects (default is True).
        """
        self.base_url = base_url.rstrip('/')
        self.response_parser = response_parser
        self.timeout = timeout
        self.follow_redirects = follow_redirects

        # Build headers
        self.headers = {
            "Content-Type": "application/json",
            "Connection": "keep-alive",
        }

        if token:
            self.headers["Authorization"] = f"{auth_scheme} {token}"

        if default_headers:
            self.headers.update(default_headers)

    def _build_url(self, endpoint: str) -> str:
        """
        Build the full URL for the given endpoint.
        :param endpoint: str - The endpoint to append to the base URL.
        :return: str - The full URL.
        """
        if endpoint.startswith(('http://', 'https://')):
            return endpoint
        return urljoin(f"{self.base_url}/", endpoint.lstrip('/'))

    async def _make_request(
            self,
            method: str,
            endpoint: str,
            params: Optional[Dict[str, Any]] = None,
            data: Optional[Union[Dict[str, Any], str, bytes]] = None,
            headers: Optional[Dict[str, str]] = None,
            timeout: Optional[float] = None,
            **kwargs
    ) -> T:
        """
        Make an HTTP request to the specified endpoint.
        :param method: str - The HTTP method (GET, POST, PUT, etc.).
        :param endpoint: str - The endpoint to call.
        :param params: Optional[Dict[str, Any]] - Query parameters to include in the request.
        :param data: Optional[Union[Dict[str, Any], str, bytes]] - Data to send in the request body.
        :param headers: Optional[Dict[str, str]] - Additional headers to include in the request.
        :param timeout: Optional[float] - Timeout for the request in seconds.
        :param kwargs: Additional keyword arguments to pass to the request.
        :return: T - The parsed response from the server.
        """
        final_url = self._build_url(endpoint)
        request_headers = {**self.headers}

        if headers:
            request_headers.update(headers)

        request_timeout = timeout or self.timeout

        async with httpx.AsyncClient(
                timeout=request_timeout,
                follow_redirects=self.follow_redirects
        ) as client:
            # Determine how to send data based on content type
            if data is not None:
                content_type = request_headers.get("Content-Type", "")
                if "application/json" in content_type and isinstance(data, dict):
                    kwargs["json"] = data
                elif isinstance(data, bytes):
                    kwargs["content"] = data
                else:
                    kwargs["data"] = data
            logger.info(f"{method} request to {final_url} with headers: {request_headers} and params: {params}")
            response = await client.request(
                method=method,
                url=final_url,
                headers=request_headers,
                params=params,
                **kwargs
            )

            response.raise_for_status()
            return await self.response_parser.parse(response)

    async def get(
            self,
            endpoint: str,
            params: Optional[Dict[str, Any]] = None,
            headers: Optional[Dict[str, str]] = None,
            timeout: Optional[float] = None
    ) -> T:

        return await self._make_request("GET", endpoint, params=params, headers=headers, timeout=timeout)

    async def post(
            self,
            endpoint: str,
            data: Optional[Union[Dict[str, Any], str, bytes]] = None,
            params: Optional[Dict[str, Any]] = None,
            headers: Optional[Dict[str, str]] = None,
            timeout: Optional[float] = None
    ) -> T:
        return await self._make_request("POST", endpoint, params=params, data=data, headers=headers, timeout=timeout)

    async def put(
            self,
            endpoint: str,
            data: Optional[Union[Dict[str, Any], str, bytes]] = None,
            params: Optional[Dict[str, Any]] = None,
            headers: Optional[Dict[str, str]] = None,
            timeout: Optional[float] = None
    ) -> T:
        """
        Make a PUT request to the specified endpoint.
        :param endpoint: str - The endpoint to call.
        :param data: Optional[Union[Dict[str, Any], str, bytes]] - Data to send in the request body.
        :param params: Optional[Dict[str, Any]] - Query parameters to include in the request.
        :param headers: Optional[Dict[str, str]] - Additional headers to include in the request.
        :param timeout: Optional[float] - Timeout for the request in seconds.
        :return: T - The parsed response from the server.
        """
        return await self._make_request("PUT", endpoint, params=params, data=data, headers=headers, timeout=timeout)

    async def patch(
            self,
            endpoint: str,
            data: Optional[Union[Dict[str, Any], str, bytes]] = None,
            params: Optional[Dict[str, Any]] = None,
            headers: Optional[Dict[str, str]] = None,
            timeout: Optional[float] = None
    ) -> T:
        """
        Make a PATCH request to the specified endpoint.
        :param endpoint: str - The endpoint to call.
        :param data: Optional[Union[Dict[str, Any], str, bytes]] - Data to send in the request body.
        :param params: Optional[Dict[str, Any]] - Query parameters to include in the request.
        :param headers: Optional[Dict[str, str]] - Additional headers to include in the request.
        :param timeout: Optional[float] - Timeout for the request in seconds.
        :return: T - The parsed response from the server.
        """
        return await self._make_request("PATCH", endpoint, params=params, data=data, headers=headers, timeout=timeout)

    async def delete(
            self,
            endpoint: str,
            params: Optional[Dict[str, Any]] = None,
            headers: Optional[Dict[str, str]] = None,
            timeout: Optional[float] = None
    ) -> T:
        """
        Make a DELETE request to the specified endpoint.
        :param endpoint: str - The endpoint to call.
        :param params: Optional[Dict[str, Any]] - Query parameters to include in the request.
        :param headers: Optional[Dict[str, str]] - Additional headers to include in the request.
        :param timeout: Optional[float] - Timeout for the request in seconds.
        :return: T - The parsed response from the server.
        """
        return await self._make_request("DELETE", endpoint, params=params, headers=headers, timeout=timeout)

    async def head(
            self,
            endpoint: str,
            params: Optional[Dict[str, Any]] = None,
            headers: Optional[Dict[str, str]] = None,
            timeout: Optional[float] = None
    ) -> T:
        """
        Make a HEAD request to the specified endpoint.
        :param endpoint: str - The endpoint to call.
        :param params: Optional[Dict[str, Any]] - Query parameters to include in the request.
        :param headers: Optional[Dict[str, str]] - Additional headers to include in the request.
        :param timeout: Optional[float] - Timeout for the request in seconds.
        :return: T - The parsed response from the server.
        """
        return await self._make_request("HEAD", endpoint, params=params, headers=headers, timeout=timeout)

    async def options(
            self,
            endpoint: str,
            params: Optional[Dict[str, Any]] = None,
            headers: Optional[Dict[str, str]] = None,
            timeout: Optional[float] = None
    ) -> T:
        """
        Make an OPTIONS request to the specified endpoint.
        :param endpoint: str - The endpoint to call.
        :param params: Optional[Dict[str, Any]] - Query parameters to include in the request.
        :param headers: Optional[Dict[str, str]] - Additional headers to include in the request.
        :param timeout: Optional[float] - Timeout for the request in seconds.
        :return: T - The parsed response from the server.
        """
        return await self._make_request("OPTIONS", endpoint, params=params, headers=headers, timeout=timeout)


# Convenience type aliases
JsonHttpClient = BaseHttpClient[Dict[str, Any]]
TextHttpClient = BaseHttpClient[str]
BytesHttpClient = BaseHttpClient[bytes]
