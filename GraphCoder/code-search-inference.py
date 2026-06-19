import os
import json
import tiktoken
from typing import List, Dict, Any, Tuple
from dataclasses import dataclass
import time

@dataclass
class CONSTANTS:
    max_search_top_k: int = 10
    graph_database_save_dir: str = "./context_database"

class CodexTokenizer:
    def __init__(self):
        self.tokenizer = tiktoken.encoding_for_model("gpt-3.5")
    
    def tokenize(self, text: str) -> List[int]:
        return self.tokenizer.encode(text, allowed_special={'<|endoftext|>'})
    
    def decode(self, token_ids: List[int]) -> str:
        return self.tokenizer.decode(token_ids)

class SimilarityScore:
    @staticmethod
    def text_jaccard_similarity(list1: List[int], list2: List[int]) -> float:
        set1 = set(list1)
        set2 = set(list2)
        intersection = len(set1.intersection(set2))
        union = len(set1.union(set2))
        return float(intersection) / union if union > 0 else 0.0

class CodeSearchInference:
    def __init__(self, database_dir: str = CONSTANTS.graph_database_save_dir):
        self.database_dir = database_dir
        self.tokenizer = CodexTokenizer()
        self.constants = CONSTANTS()
    
    def load_database(self, repo_name: str) -> List[Dict[str, Any]]:
        """Load the code database for a specific repository"""
        database_path = os.path.join(self.database_dir, f"{repo_name}.jsonl")
        if not os.path.exists(database_path):
            raise FileNotFoundError(f"Database file not found: {database_path}")
            
        database = []
        with open(database_path, 'r', encoding='utf-8') as f:
            for line in f:
                database.append(json.loads(line.strip()))
        return database

    def search(self, query: str, repo_name: str) -> Tuple[List[Dict[str, Any]], float]:
        """
        Search for code snippets similar to the natural language query
        
        Args:
            query: Natural language query
            repo_name: Name of the repository to search in
            
        Returns:
            Tuple of (list of results, search time)
        """
        start_time = time.time()
        
        # Load and tokenize query
        query_tokens = self.tokenizer.tokenize(query)
        
        # Load database
        try:
            database = self.load_database(repo_name)
        except FileNotFoundError as e:
            print(f"Error: {e}")
            return [], 0.0
        
        # Calculate similarity scores
        search_results = []
        for case in database:
            sim_score = SimilarityScore.text_jaccard_similarity(
                query_tokens,
                case['key_forward_encoding']
            )
            if sim_score > 0:  # Only include non-zero similarities
                result = {
                    'val': case['val'],
                    'statement': case['statement'],
                    'key_forward_context': case['key_forward_context'],
                    'key_forward_graph': case['key_forward_graph'],
                    'key_forward_encoding': case['key_forward_encoding'],
                    'fpath_tuple': case['fpath_tuple'],
                    'similarity_score': sim_score
                }
                search_results.append(result)
        
        # Sort by similarity score and get top-k results
        search_results.sort(key=lambda x: x['similarity_score'], reverse=True)
        top_k_results = search_results[:self.constants.max_search_top_k]
        
        search_time = time.time() - start_time
        return top_k_results, search_time

def format_results(results: List[Dict[str, Any]], search_time: float) -> str:
    """Format search results for display"""
    output = f"Search completed in {search_time:.4f} seconds\n"
    output += f"Found {len(results)} relevant code snippets\n\n"
    
    for i, result in enumerate(results, 1):
        output += f"Result {i} (Similarity: {result['similarity_score']:.4f})\n"
        output += f"File: {'/'.join(result['fpath_tuple'])}\n"
        output += "Code Context:\n"
        output += result['val'] + "\n"
        output += "-" * 80 + "\n"
    
    return output

def main():
    # Example usage
    searcher = CodeSearchInference()
    query = "int comp_write_bm( int address, int data ){"
    repo_name = "versa"  # Replace with your repository name
    
    results, search_time = searcher.search(query, repo_name)
    print(format_results(results, search_time))

if __name__ == "__main__":
    main()