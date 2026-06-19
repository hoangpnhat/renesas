"""
Simple API test script
"""
import requests
import json

BASE_URL = "http://localhost:8000/api"

# Create conversation
print("Creating conversation...")
response = requests.post(f"{BASE_URL}/conversations/")
conversation_data = response.json()
conversation_id = conversation_data["conversation_id"]
print(f"Conversation created: {conversation_id}\n")

# Send first message
print("Sending message 1...")
chat_response = requests.post(
    f"{BASE_URL}/agents/chat",
    json={
        "message": "Hi, I'm looking for a 2-bedroom apartment in Dubai under $5 million",
        "conversation_id": conversation_id
    }
)
print(f"Status: {chat_response.status_code}")
chat_data = chat_response.json()
print(f"Reply: {chat_data.get('reply', 'ERROR')[:200]}")
print(f"Stage: {chat_data.get('stage')}")
projects = chat_data.get('shortlisted_projects')
print(f"Projects: {len(projects) if projects else 0}")
print()

# Send second message
print("Sending message 2...")
chat_response2 = requests.post(
    f"{BASE_URL}/agents/chat",
    json={
        "message": "Tell me more about the first one",
        "conversation_id": conversation_id
    }
)
print(f"Status: {chat_response2.status_code}")
chat_data2 = chat_response2.json()
print(f"Reply: {chat_data2.get('reply', 'ERROR')[:200]}")
print(f"Stage: {chat_data2.get('stage')}")
print()

print("Test complete!")
