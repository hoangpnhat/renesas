import networkx as nx
from typing import List, Dict, Tuple, Any
import Levenshtein
from dataclasses import dataclass
import json
from utils.utils import json_to_graph, load_jsonl
@dataclass
class SearchResult:
    value: str
    statement: str
    context: str
    filepath: Tuple[str, ...]
    similarity_score: float

class CodeSearchEngine:
    def __init__(self, gamma: float = 0.1):
        self.gamma = gamma
        
    @staticmethod
    def text_similarity(str1: str, str2: str) -> float:
        """Calculate text similarity using Levenshtein distance"""
        return 1 - Levenshtein.distance(str1, str2) / max(len(str1), len(str2))

    @staticmethod
    def jaccard_similarity(list1: List[str], list2: List[str]) -> float:
        """Calculate Jaccard similarity between two lists"""
        set1 = set(list1)
        set2 = set(list2)
        intersection = len(set1.intersection(set2))
        union = len(set1.union(set2))
        return float(intersection) / union if union > 0 else 0.0

    def subgraph_similarity(self, query_graph: nx.MultiDiGraph, target_graph: nx.MultiDiGraph) -> float:
        """Calculate similarity between two graphs using subgraph matching"""
        if not query_graph.nodes or not target_graph.nodes:
            return 0.0

        query_root = max(query_graph.nodes)
        target_root = max(target_graph.nodes)
        
        # Calculate root node similarity
        query_code = "".join(query_graph.nodes[query_root].get('sourceLines', []))
        target_code = "".join(target_graph.nodes[target_root].get('sourceLines', []))
        
        # Use text tokens for comparison
        query_tokens = query_code.split()
        target_tokens = target_code.split()
        node_sim = self.jaccard_similarity(query_tokens, target_tokens)
        
        # Initialize node matching
        matched_nodes = {query_root: target_root}
        total_sim = node_sim
        
        # Traverse and match neighboring nodes
        for query_node in query_graph.nodes:
            if query_node == query_root:
                continue
                
            best_match_sim = 0
            best_match_node = None
            
            for target_node in target_graph.nodes:
                if target_node in matched_nodes.values():
                    continue
                    
                query_code = "".join(query_graph.nodes[query_node].get('sourceLines', []))
                target_code = "".join(target_graph.nodes[target_node].get('sourceLines', []))
                
                query_tokens = query_code.split()
                target_tokens = target_code.split()
                sim = self.jaccard_similarity(query_tokens, target_tokens)
                
                if sim > best_match_sim:
                    best_match_sim = sim
                    best_match_node = target_node
            
            if best_match_node:
                matched_nodes[query_node] = best_match_node
                total_sim += best_match_sim * (self.gamma ** len(matched_nodes))
        
        return total_sim

    def search(self, 
              query_graph: nx.MultiDiGraph, 
              code_database: List[Dict[str, Any]], 
              top_k: int = 10, 
              threshold: float = 0.0) -> List[SearchResult]:
        """
        Search for similar code snippets in the database
        
        Args:
            query_graph: Query code as a graph
            code_database: List of code snippets with their graphs
            top_k: Number of top results to return
            threshold: Minimum similarity score threshold
            
        Returns:
            List of SearchResult objects sorted by similarity score
        """
        results = []
        
        for entry in code_database:
            target_graph = json_to_graph(entry.get('graph', {}))
            sim_score = self.subgraph_similarity(query_graph, target_graph)
            
            if sim_score >= threshold:
                result = SearchResult(
                    value=entry.get('value', ''),
                    statement=entry.get('statement', ''),
                    context=entry.get('context', ''),
                    filepath=tuple(entry.get('filepath', [])),
                    similarity_score=sim_score
                )
                results.append(result)
        
        # Sort by similarity score in descending order
        results.sort(key=lambda x: x.similarity_score, reverse=True)
        return results[:top_k]

    @staticmethod
    def _dict_to_graph(graph_dict: Dict) -> nx.MultiDiGraph:
        """Convert a dictionary representation to a NetworkX graph"""
        graph = nx.MultiDiGraph()
        
        # Add nodes
        for node_id, node_data in graph_dict.get('nodes', {}).items():
            graph.add_node(int(node_id), **node_data)
            
        # Add edges
        for edge in graph_dict.get('edges', []):
            graph.add_edge(edge['source'], edge['target'], key=edge.get('type', ''))
            
        return graph

# Example usage:
def main():
    # Initialize search engine
    search_engine = CodeSearchEngine(gamma=0.1)
    
    # Load code database (example)
    code_database = load_jsonl('context_database/devchat.jsonl')
    
    # Create example query graph
    query_graph = nx.MultiDiGraph()
    query_graph.add_node(1, sourceLines=["def example_function():"])
    query_graph.add_node(2, sourceLines=["    return result"])
    query_graph.add_edge(1, 2, type="control_flow")
    
    # Search for similar code
    results = search_engine.search(
        query_graph=query_graph,
        code_database=code_database,
        top_k=5,
        threshold=0.3
    )
    
    # Print results
    for i, result in enumerate(results, 1):
        print(f"\nResult {i} (similarity: {result.similarity_score:.3f}):")
        print(f"File: {'/'.join(result.filepath)}")
        print(f"Context:\n{result.context}")
        print(f"Statement:\n{result.statement}")

if __name__ == "__main__":
    main()
