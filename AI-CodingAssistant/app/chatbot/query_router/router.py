import os
from typing import List, TypeVar
from semantic_router import Route
from semantic_router.encoders import OpenAIEncoder, BM25Encoder, TfidfEncoder
from semantic_router.layer import RouteLayer
from semantic_router.hybrid_layer import HybridRouteLayer

from app.chatbot.query_router.route import CRUDRoute

RouteChoice = TypeVar('RouteChoice')

class Router:
    _instance = None
    
    def __new__(cls, *args, **kwargs):
        if cls._instance is None:
            cls._instance = super(Router, cls).__new__(cls)
            cls._instance.initialized = False
        return cls._instance

    def __init__(cls, *args, **kwargs):
        top_k = kwargs.get("top_k", 3)
        if not cls.initialized:
            cls.initialize(top_k=top_k)

    def initialize(cls, routes: List[Route] = None, 
                   top_k: int = 1, 
                   using_hybrid: bool = False, 
                   sparse_encoder_name: str = None):
        
        if not top_k:
            cls.top_k = 1
        cls.top_k = top_k
        cls.using_hybrid = using_hybrid
        cls.sparse_encoder_name = sparse_encoder_name
        cls.encoder = OpenAIEncoder(os.getenv("ROUTE_EMBEDDING"))

        if not routes:
            cls.load_routes()
        
        if cls.using_hybrid:
            cls.sparse_encoder = BM25Encoder() if sparse_encoder_name == "BM25" else TfidfEncoder()
            cls.rl = HybridRouteLayer(
                encoder=cls.encoder,
                sparse_encoder=cls.sparse_encoder,
                routes=cls.routes,
                top_k=cls.top_k
            )
        else:
            cls.rl = RouteLayer(encoder=cls.encoder, routes=cls.routes, top_k=cls.top_k)
        
        cls.initialized = True
    
    def load_routes(cls):
        route_objs = CRUDRoute().get_all_routes()
        cls.routes = [Route(name=route_obj.name, utterances=route_obj.utterances) for route_obj in route_objs]

    def route(cls, query: str) -> List[RouteChoice]:
        """
        This function takes a query and returns a list of RouteChoice objects.
        Args:
            query (str): The query to predict

        Returns:
            List[RouteChoice]: A list of RouteChoice objects
                            Ex: [RouteChoice(name='unclear_issue', function_call=None, similarity_score=0.8904590012)]
        """
        return cls.rl.retrieve_multiple_routes(query)
