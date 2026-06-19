"""
Test script for Vanna SQL Tool
"""
import os
import sys
import django
from pathlib import Path

# Setup Django environment
BASE_DIR = Path(__file__).resolve().parent.parent
sys.path.append(str(BASE_DIR))
os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'config.settings')
django.setup()

from agents.tools import get_vanna_tool

def test_vanna():
    print("=" * 80)
    print("TESTING VANNA SQL TOOL")
    print("=" * 80)

    # Initialize Vanna
    print("\n1. Initializing Vanna...")
    vanna_tool = get_vanna_tool()
    vanna_tool.initialize()

    # Test queries
    test_queries = [
        "Find 2-bedroom apartments in Dubai",
        "Show me properties in Miami under $10 million",
        "What are the top 5 cheapest properties in Bangkok?",
    ]

    for i, query in enumerate(test_queries, 1):
        print(f"\n{i}. Testing query: '{query}'")
        print("-" * 80)

        result = vanna_tool.generate_and_execute_sql(query)

        if result['error']:
            print(f"ERROR: {result['error']}")
        else:
            print(f"Generated SQL: {result['sql']}")
            print(f"Results count: {result['count']}")
            if result['results']:
                print(f"Sample result: {result['results'][0]}")

    print("\n" + "=" * 80)
    print("VANNA TESTING COMPLETE")
    print("=" * 80)


if __name__ == '__main__':
    test_vanna()
