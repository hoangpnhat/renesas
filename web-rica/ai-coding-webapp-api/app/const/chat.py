import httpx
TEST_MESSAGE_QUESTION = "Sorting Algorithm for a list of numbers with complexity O(n log n)?"
# DATABRICKS_PROMPT_TIMEOUT = httpx.Timeout(50.0)

# Temp increase timeout for testing
DATABRICKS_PROMPT_TIMEOUT = httpx.Timeout(300.0)
FILE_CHUNK_SIZE = 1_000_000
DBX_DOWNLOAD_ENDPOINT_API="/api/2.0/fs/files"