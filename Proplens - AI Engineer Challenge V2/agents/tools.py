"""
Agent Tools - Text-to-SQL and Web Search
"""
import os
from typing import Dict, Any, List
from dotenv import load_dotenv

load_dotenv()

# Vanna Text-to-SQL Tool
class VannaSQLTool:
    """Text-to-SQL tool powered by Vanna and ChromaDB"""

    def __init__(self):
        self.vn = None
        self._initialized = False

    def initialize(self):
        """Initialize Vanna with ChromaDB and connect to SQLite"""
        if self._initialized:
            return

        try:
            from vanna.chromadb import ChromaDB_VectorStore
            from vanna.openai import OpenAI_Chat

            # Create Vanna class combining ChromaDB and OpenAI
            class VannaSQL(ChromaDB_VectorStore, OpenAI_Chat):
                def __init__(self, config=None):
                    ChromaDB_VectorStore.__init__(self, config=config)
                    OpenAI_Chat.__init__(self, config=config)

            # Initialize Vanna
            import sqlite3
            from pathlib import Path

            chroma_path = Path(__file__).parent.parent / 'data' / 'chroma_db'
            chroma_path.mkdir(parents=True, exist_ok=True)

            self.vn = VannaSQL(config={
                'path': str(chroma_path),
                'api_key': os.getenv('OPENAI_API_KEY'),
                'model': 'gpt-4o-mini',  # Use GPT-4 Mini for efficiency
            })

            # Connect to SQLite database
            db_path = Path(__file__).parent.parent / 'db.sqlite3'
            self.vn.connect_to_sqlite(str(db_path))

            # Train Vanna with DDL and sample queries
            self._train_vanna()

            self._initialized = True
            print("Vanna SQL Tool initialized successfully")

        except Exception as e:
            print(f"Error initializing Vanna: {e}")
            raise

    def _train_vanna(self):
        """Train Vanna with database schema and sample queries"""

        # DDL for core_project table
        ddl = """
        CREATE TABLE core_project (
            id INTEGER PRIMARY KEY,
            project_name TEXT,
            no_of_bedrooms INTEGER,
            completion_status TEXT,
            bathrooms INTEGER,
            unit_type TEXT,
            developer_name TEXT,
            price_usd INTEGER,
            area_sq_mtrs INTEGER,
            property_type TEXT,
            city TEXT,
            country TEXT,
            completion_date DATE,
            features TEXT,
            facilities TEXT,
            project_description TEXT,
            created_at DATETIME,
            updated_at DATETIME
        );
        """

        # Check if already trained
        try:
            training_data = self.vn.get_training_data()
            if training_data and len(training_data) > 0:
                print("Vanna already trained, skipping training")
                return
        except:
            pass  # If get_training_data fails, proceed with training

        print("Training Vanna with database schema...")

        # Train DDL
        self.vn.train(ddl=ddl)

        # Train sample question-SQL pairs
        training_pairs = [
            {
                "question": "Find 2-bedroom apartments in Dubai",
                "sql": "SELECT * FROM core_project WHERE city='Dubai' AND no_of_bedrooms=2 AND property_type='apartment'"
            },
            {
                "question": "Show me properties under $5 million in Miami",
                "sql": "SELECT * FROM core_project WHERE city='Miami' AND price_usd < 5000000"
            },
            {
                "question": "What are the cheapest 3-bedroom properties in Bangkok?",
                "sql": "SELECT * FROM core_project WHERE city='Bangkok' AND no_of_bedrooms=3 ORDER BY price_usd ASC LIMIT 5"
            },
            {
                "question": "Find villas in Dubai between $10M and $20M",
                "sql": "SELECT * FROM core_project WHERE city='Dubai' AND property_type='villa' AND price_usd BETWEEN 10000000 AND 20000000"
            },
            {
                "question": "Show me all properties in Singapore",
                "sql": "SELECT * FROM core_project WHERE city='Singapore'"
            },
            {
                "question": "What's the average price of apartments in London?",
                "sql": "SELECT AVG(price_usd) as avg_price FROM core_project WHERE city='London' AND property_type='apartment'"
            },
            {
                "question": "List all available cities",
                "sql": "SELECT DISTINCT city FROM core_project WHERE city IS NOT NULL ORDER BY city"
            },
            {
                "question": "How many 1-bedroom properties are in Phuket?",
                "sql": "SELECT COUNT(*) as count FROM core_project WHERE city='Phuket' AND no_of_bedrooms=1"
            },
        ]

        for pair in training_pairs:
            try:
                self.vn.train(question=pair["question"], sql=pair["sql"])
            except Exception as e:
                print(f"Warning: Failed to train pair: {e}")

        print("Vanna training complete")

    def generate_and_execute_sql(self, question: str) -> Dict[str, Any]:
        """
        Generate SQL from natural language question and execute it

        Args:
            question: Natural language question about properties

        Returns:
            Dict with 'sql', 'results', 'error' keys
        """
        if not self._initialized:
            self.initialize()

        try:
            # Generate SQL
            sql = self.vn.generate_sql(question)

            # Execute SQL
            df = self.vn.run_sql(sql)

            # Convert DataFrame to list of dicts
            results = df.to_dict('records') if df is not None and not df.empty else []

            return {
                'sql': sql,
                'results': results,
                'count': len(results),
                'error': None
            }

        except Exception as e:
            return {
                'sql': None,
                'results': [],
                'count': 0,
                'error': str(e)
            }


# Web Search Tool
class WebSearchTool:
    """Web search tool for project-specific information"""

    def search(self, query: str, project_name: str = None) -> Dict[str, Any]:
        """
        Perform web search for project-specific information

        Args:
            query: Search query
            project_name: Optional project name for context

        Returns:
            Dict with 'results' and 'error' keys
        """
        try:
            from duckduckgo_search import DDGS

            # Construct search query
            if project_name:
                search_query = f"{project_name} {query}"
            else:
                search_query = query

            # Perform search
            ddgs = DDGS()
            results = list(ddgs.text(search_query, max_results=3))

            return {
                'results': results,
                'count': len(results),
                'error': None
            }

        except Exception as e:
            return {
                'results': [],
                'count': 0,
                'error': str(e)
            }


# Singleton instances
_vanna_tool = None
_web_search_tool = None


def get_vanna_tool() -> VannaSQLTool:
    """Get or create Vanna SQL tool instance"""
    global _vanna_tool
    if _vanna_tool is None:
        _vanna_tool = VannaSQLTool()
    return _vanna_tool


def get_web_search_tool() -> WebSearchTool:
    """Get or create web search tool instance"""
    global _web_search_tool
    if _web_search_tool is None:
        _web_search_tool = WebSearchTool()
    return _web_search_tool
