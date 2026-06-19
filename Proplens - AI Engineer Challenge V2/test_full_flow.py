"""
Comprehensive API test - Full booking flow
"""
import requests
import json
import time

BASE_URL = "http://localhost:8000/api"

def send_message(conversation_id, message):
    """Send a message and print the response"""
    print(f"\n{'='*80}")
    print(f"USER: {message}")
    print(f"{'='*80}")

    response = requests.post(
        f"{BASE_URL}/agents/chat",
        json={"message": message, "conversation_id": conversation_id}
    )

    if response.status_code == 200:
        data = response.json()
        print(f"ASSISTANT: {data['reply']}")
        print(f"\nStage: {data['stage']}")

        if data.get('shortlisted_projects'):
            print(f"\nRecommended Properties ({len(data['shortlisted_projects'])}):")
            for proj in data['shortlisted_projects']:
                print(f"  - {proj.get('project_name', 'N/A')} in {proj['city']}: ${proj['price_usd']:,}")

        return data
    else:
        print(f"ERROR: {response.status_code}")
        print(response.text)
        return None

# Test flow
print("\n" + "="*80)
print("PROPERTY SALES AI ASSISTANT - FULL CONVERSATION TEST")
print("="*80)

# Create conversation
print("\nCreating conversation...")
conv_response = requests.post(f"{BASE_URL}/conversations/")
conversation_id = conv_response.json()["conversation_id"]
print(f"Conversation ID: {conversation_id}")

time.sleep(1)

# Message 1: Initial greeting with preferences
send_message(conversation_id, "Hi! I'm looking for a 2-bedroom apartment in Dubai under $5 million")

time.sleep(2)

# Message 2: Ask about properties
send_message(conversation_id, "Yes, please show me what you have")

time.sleep(2)

# Message 3: Show interest in a property
send_message(conversation_id, "The first one looks interesting. Can I schedule a viewing?")

time.sleep(2)

# Message 4: Provide contact details
send_message(conversation_id, "Sure! My name is John Doe and my email is john.doe@example.com")

print("\n" + "="*80)
print("TEST COMPLETE - Full conversation flow demonstrated")
print("="*80)
