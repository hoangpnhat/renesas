"""
Django Ninja API Controllers
"""
from ninja import Router
from ninja.errors import HttpError
from typing import List
import uuid

from core.models import Conversation, Message, Project
from .schemas import (
    ConversationCreateResponse,
    ChatRequest,
    ChatResponse,
    ProjectSummary,
    HealthCheckResponse
)
from agents.graph import get_agent

# Create routers
conversations_router = Router(tags=["Conversations"])
agent_router = Router(tags=["Agent"])
health_router = Router(tags=["Health"])


@conversations_router.post("/", response=ConversationCreateResponse)
def create_conversation(request):
    """
    Create a new conversation session

    Returns a unique conversation_id that should be used in subsequent chat requests.
    """
    conversation = Conversation.objects.create()
    return {"conversation_id": conversation.conversation_id}


@agent_router.post("/chat", response=ChatResponse)
def chat(request, payload: ChatRequest):
    """
    Send a message to the AI agent and receive a response

    The agent will:
    1. Understand your property preferences
    2. Search for matching properties
    3. Answer questions
    4. Help book property viewings

    Parameters:
    - message: Your message to the agent
    - conversation_id: The conversation ID from POST /conversations

    Returns:
    - reply: Agent's response
    - stage: Current conversation stage (greeting, probing, recommending, booking)
    - shortlisted_projects: Recommended properties (if applicable)
    - preferences: Your current preferences
    """
    try:
        # Get or create conversation
        try:
            conversation = Conversation.objects.get(conversation_id=payload.conversation_id)
        except Conversation.DoesNotExist:
            raise HttpError(404, "Conversation not found. Please create a conversation first using POST /conversations")

        # Save user message
        Message.objects.create(
            conversation=conversation,
            role='user',
            content=payload.message
        )

        # Load conversation history
        messages = Message.objects.filter(conversation=conversation).order_by('timestamp')
        message_history = [
            {'role': msg.role, 'content': msg.content}
            for msg in messages
        ]

        # Build initial agent state
        initial_state = {
            'messages': message_history,
            'conversation_id': str(conversation.conversation_id),
            'lead_id': conversation.lead_id if conversation.lead else None,
            'preferences': conversation.lead.preferences if conversation.lead else {},
            'shortlisted_projects': [],
            'current_stage': 'greeting' if len(message_history) <= 1 else 'probing',
            'pending_booking': None,
            'tool_output': None
        }

        # Run the agent
        agent = get_agent()
        result = agent.invoke(initial_state)

        # Extract assistant's last message
        assistant_messages = [m for m in result['messages'] if m['role'] == 'assistant']
        assistant_reply = assistant_messages[-1]['content'] if assistant_messages else "Hello! How can I help you find your perfect property?"

        # Save assistant response to database
        Message.objects.create(
            conversation=conversation,
            role='assistant',
            content=assistant_reply,
            metadata={
                'shortlisted_projects': result.get('shortlisted_projects', []),
                'stage': result.get('current_stage', 'greeting')
            }
        )

        # Update conversation lead if created
        if result.get('lead_id') and not conversation.lead_id:
            from core.models import Lead
            conversation.lead_id = result['lead_id']
            conversation.save()

        # Convert shortlisted projects to response format
        shortlisted = []
        for proj in result.get('shortlisted_projects', []):
            shortlisted.append(ProjectSummary(
                id=proj['id'],
                project_name=proj.get('project_name'),
                city=proj['city'],
                price_usd=proj['price_usd'],
                bedrooms=proj.get('bedrooms'),
                property_type=proj.get('property_type')
            ))

        return ChatResponse(
            reply=assistant_reply,
            conversation_id=conversation.conversation_id,
            stage=result.get('current_stage', 'greeting'),
            shortlisted_projects=shortlisted if shortlisted else None,
            preferences=result.get('preferences')
        )

    except Exception as e:
        print(f"Error in chat endpoint: {e}")
        import traceback
        traceback.print_exc()
        raise HttpError(500, f"Internal server error: {str(e)}")


@health_router.get("/", response=HealthCheckResponse)
def health_check(request):
    """
    Health check endpoint to verify API is running

    Returns API status and database connection status.
    """
    # Check database connection
    try:
        Project.objects.count()
        db_connected = True
    except Exception:
        db_connected = False

    status = "healthy" if db_connected else "degraded"

    return HealthCheckResponse(
        status=status,
        message="Property Sales AI Assistant API is running",
        database_connected=db_connected
    )
