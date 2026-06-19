"""
LangGraph Agent Orchestration for Property Sales Assistant
"""
import os
import json
from typing import TypedDict, List, Optional, Dict, Any, Annotated
from langchain_openai import ChatOpenAI
from langchain_core.messages import HumanMessage, AIMessage, SystemMessage
from langgraph.graph import StateGraph, END
from dotenv import load_dotenv

load_dotenv()

# Import Django models (will be initialized when needed)
Project = None
Lead = None
Booking = None


def init_django():
    """Initialize Django for database access"""
    global Project, Lead, Booking
    if Project is None:
        import django
        import sys
        from pathlib import Path

        BASE_DIR = Path(__file__).resolve().parent.parent
        sys.path.append(str(BASE_DIR))
        os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'config.settings')
        django.setup()

        from core.models import Project as ProjectModel
        from core.models import Lead as LeadModel
        from core.models import Booking as BookingModel

        Project = ProjectModel
        Lead = LeadModel
        Booking = BookingModel


# Agent State Definition
class AgentState(TypedDict):
    """State for the conversational agent"""
    messages: List[Dict[str, str]]  # Conversation history
    conversation_id: str
    lead_id: Optional[int]
    preferences: Dict[str, Any]  # {city, budget_min, budget_max, bedrooms}
    shortlisted_projects: List[Dict[str, Any]]
    current_stage: str  # 'greeting', 'probing', 'recommending', 'answering', 'booking'
    pending_booking: Optional[Dict[str, Any]]
    tool_output: Optional[Dict[str, Any]]  # For tool results


# Initialize LLM
llm = ChatOpenAI(
    model="gpt-4o-mini",
    temperature=0.7,
    api_key=os.getenv('OPENAI_API_KEY')
)


def route_intent(state: AgentState) -> str:
    """Determine the next action based on conversation state"""

    # Get the last user message
    user_messages = [m for m in state['messages'] if m['role'] == 'user']
    if not user_messages:
        return 'generate_response'

    last_message = user_messages[-1]['content'].lower()
    stage = state['current_stage']
    preferences = state.get('preferences', {})

    # Check for booking intent
    if any(word in last_message for word in ['book', 'schedule', 'visit', 'viewing', 'appointment']):
        if 'email' not in preferences or '@' not in str(preferences.get('email', '')):
            state['current_stage'] = 'booking'
            return 'collect_booking_info'
        else:
            state['current_stage'] = 'confirm_booking'
            return 'create_booking'

    # Check if user has shared enough preferences
    has_city = 'city' in preferences and preferences['city']
    has_budget = 'budget_max' in preferences or 'budget_min' in preferences
    has_bedrooms = 'bedrooms' in preferences

    # If user is asking a specific question
    if any(word in last_message for word in ['what', 'how', 'where', 'when', 'which', '?']):
        if has_city and (has_budget or has_bedrooms):
            # User has preferences, might be asking about recommendations
            state['current_stage'] = 'recommending'
            return 'search_projects'
        else:
            state['current_stage'] = 'answering'
            return 'answer_question'

    # If preferences are incomplete, continue probing
    if not has_city or not (has_budget or has_bedrooms):
        state['current_stage'] = 'probing'
        return 'extract_preferences'

    # If we have preferences, search for properties
    state['current_stage'] = 'recommending'
    return 'search_projects'


def extract_preferences(state: AgentState) -> AgentState:
    """Extract property preferences from user message"""

    user_messages = [m for m in state['messages'] if m['role'] == 'user']
    if not user_messages:
        return state

    last_message = user_messages[-1]['content']

    # Use LLM to extract preferences
    extraction_prompt = f"""Extract property preferences from this message:
    "{last_message}"

    Extract: city, budget_min, budget_max, bedrooms, property_type

    Return ONLY a JSON object with extracted values. Example:
    {{"city": "Dubai", "bedrooms": 2, "budget_max": 5000000}}

    If nothing found, return {{}}
    """

    try:
        response = llm.invoke([HumanMessage(content=extraction_prompt)])
        extracted = json.loads(response.content.strip().replace('```json', '').replace('```', ''))

        # Merge with existing preferences
        current_prefs = state.get('preferences', {})
        current_prefs.update(extracted)
        state['preferences'] = current_prefs

    except Exception as e:
        print(f"Error extracting preferences: {e}")

    return state


def search_projects(state: AgentState) -> AgentState:
    """Search for properties matching user preferences"""

    init_django()
    preferences = state.get('preferences', {})

    # Build query
    query = Project.objects.filter(project_name__isnull=False)

    # Filter by city
    if preferences.get('city'):
        query = query.filter(city__icontains=preferences['city'])

    # Filter by price
    if preferences.get('budget_min'):
        query = query.filter(price_usd__gte=preferences['budget_min'])
    if preferences.get('budget_max'):
        query = query.filter(price_usd__lte=preferences['budget_max'])

    # Filter by bedrooms
    if preferences.get('bedrooms'):
        query = query.filter(no_of_bedrooms=preferences['bedrooms'])

    # Filter by property type
    if preferences.get('property_type'):
        query = query.filter(property_type__icontains=preferences['property_type'])

    # Get top 3 results
    projects = list(query.order_by('price_usd')[:3])

    # Convert to dicts
    shortlisted = []
    for project in projects:
        shortlisted.append({
            'id': project.id,
            'project_name': project.project_name,
            'city': project.city,
            'price_usd': project.price_usd,
            'bedrooms': project.no_of_bedrooms,
            'property_type': project.property_type,
            'developer': project.developer_name,
            'area_sq_mtrs': project.area_sq_mtrs,
            'description': project.project_description[:200] if project.project_description else ''
        })

    state['shortlisted_projects'] = shortlisted
    state['tool_output'] = {'projects': shortlisted, 'count': len(shortlisted)}

    return state


def answer_question(state: AgentState) -> AgentState:
    """Answer user questions using available data"""

    user_messages = [m for m in state['messages'] if m['role'] == 'user']
    if not user_messages:
        return state

    question = user_messages[-1]['content']

    # Use LLM to generate answer
    context = f"User preferences: {state.get('preferences', {})}\n"
    context += f"Shortlisted projects: {len(state.get('shortlisted_projects', []))}"

    answer_prompt = f"""Answer this question about properties:
    "{question}"

    Context: {context}

    If you don't have specific information, be honest and helpful.
    Keep the answer concise and conversational.
    """

    try:
        response = llm.invoke([HumanMessage(content=answer_prompt)])
        state['tool_output'] = {'answer': response.content}
    except Exception as e:
        state['tool_output'] = {'answer': "I apologize, but I'm having trouble processing that. Could you rephrase your question?"}

    return state


def collect_booking_info(state: AgentState) -> AgentState:
    """Extract booking information from user message"""

    user_messages = [m for m in state['messages'] if m['role'] == 'user']
    if not user_messages:
        return state

    last_message = user_messages[-1]['content']

    # Extract name and email
    extraction_prompt = f"""Extract contact information from this message:
    "{last_message}"

    Extract: first_name, last_name, email

    Return ONLY JSON. Example: {{"first_name": "John", "last_name": "Doe", "email": "john@example.com"}}
    If not found, return {{}}
    """

    try:
        response = llm.invoke([HumanMessage(content=extraction_prompt)])
        extracted = json.loads(response.content.strip().replace('```json', '').replace('```', ''))

        # Update preferences with contact info
        current_prefs = state.get('preferences', {})
        current_prefs.update(extracted)
        state['preferences'] = current_prefs

        # Check if we have all required info
        if 'first_name' in extracted and 'email' in extracted:
            state['pending_booking'] = {
                'first_name': extracted.get('first_name'),
                'last_name': extracted.get('last_name', ''),
                'email': extracted['email'],
                'project_id': state['shortlisted_projects'][0]['id'] if state.get('shortlisted_projects') else None
            }

    except Exception as e:
        print(f"Error extracting booking info: {e}")

    return state


def create_booking(state: AgentState) -> AgentState:
    """Create a booking in the database"""

    init_django()

    pending = state.get('pending_booking')
    if not pending or not pending.get('email'):
        return state

    try:
        # Create or get lead
        lead, created = Lead.objects.get_or_create(
            email=pending['email'],
            defaults={
                'first_name': pending.get('first_name', ''),
                'last_name': pending.get('last_name', ''),
                'preferences': state.get('preferences', {})
            }
        )

        # Update lead preferences
        if not created:
            lead.preferences = state.get('preferences', {})
            lead.save()

        # Create booking
        project_id = pending.get('project_id')
        if project_id:
            project = Project.objects.get(id=project_id)
            booking = Booking.objects.create(
                lead=lead,
                project=project,
                status='pending'
            )

            state['tool_output'] = {
                'booking_created': True,
                'booking_id': booking.id,
                'project_name': project.project_name
            }
            state['lead_id'] = lead.id
        else:
            state['tool_output'] = {'booking_created': False, 'error': 'No project selected'}

    except Exception as e:
        print(f"Error creating booking: {e}")
        state['tool_output'] = {'booking_created': False, 'error': str(e)}

    return state


def generate_response(state: AgentState) -> AgentState:
    """Generate final response to user"""

    stage = state.get('current_stage', 'greeting')
    preferences = state.get('preferences', {})
    projects = state.get('shortlisted_projects', [])
    tool_output = state.get('tool_output', {})

    # Build context for response generation
    system_msg = f"""You are a friendly property sales assistant for Silver Land Properties.

Current stage: {stage}
User preferences: {json.dumps(preferences)}
"""

    # Add conversation history
    messages = [SystemMessage(content=system_msg)]
    for msg in state['messages'][-5:]:  # Last 5 messages for context
        if msg['role'] == 'user':
            messages.append(HumanMessage(content=msg['content']))
        elif msg['role'] == 'assistant':
            messages.append(AIMessage(content=msg['content']))

    # Add guidance based on stage
    if stage == 'probing':
        guidance = "\nAsk the user about their property preferences (city, budget, number of bedrooms) in a friendly way."
    elif stage == 'recommending' and projects:
        project_info = "\n\nProperties found:\n"
        for p in projects:
            project_info += f"- {p['project_name']} in {p['city']}: ${p['price_usd']:,} | {p['bedrooms']} bed\n"
        guidance = f"{project_info}\nPresent these properties enthusiastically and ask if they'd like more details or to schedule a viewing."
    elif stage == 'recommending' and not projects:
        guidance = "\nNo exact matches found. Apologize and ask if they'd like to adjust their criteria."
    elif stage == 'booking':
        guidance = "\nThe user wants to book a viewing. Ask for their name and email address politely."
    elif stage == 'confirm_booking':
        if tool_output.get('booking_created'):
            guidance = f"\nBooking confirmed for {tool_output.get('project_name')}! Thank them and let them know someone will contact them soon."
        else:
            guidance = "\nThere was an issue with the booking. Apologize and ask them to try again."
    else:
        guidance = "\nRespond naturally to the user's message."

    messages.append(HumanMessage(content=guidance))

    # Generate response
    try:
        response = llm.invoke(messages)
        assistant_message = response.content

        # Add to conversation history
        state['messages'].append({
            'role': 'assistant',
            'content': assistant_message
        })

    except Exception as e:
        print(f"Error generating response: {e}")
        state['messages'].append({
            'role': 'assistant',
            'content': "I apologize, but I'm having trouble right now. How can I help you with your property search?"
        })

    return state


# Build the LangGraph workflow
def create_agent_graph():
    """Create and compile the LangGraph agent"""

    workflow = StateGraph(AgentState)

    # Add nodes (no 'route' node - routing is done via conditional edges)
    workflow.add_node("extract_preferences", extract_preferences)
    workflow.add_node("search_projects", search_projects)
    workflow.add_node("answer_question", answer_question)
    workflow.add_node("collect_booking_info", collect_booking_info)
    workflow.add_node("create_booking", create_booking)
    workflow.add_node("generate_response", generate_response)

    # Set conditional entry point based on routing logic
    workflow.set_conditional_entry_point(
        route_intent,
        {
            "extract_preferences": "extract_preferences",
            "search_projects": "search_projects",
            "answer_question": "answer_question",
            "collect_booking_info": "collect_booking_info",
            "create_booking": "create_booking",
            "generate_response": "generate_response",
        }
    )

    # All paths lead to generate_response, then END
    workflow.add_edge("extract_preferences", "generate_response")
    workflow.add_edge("search_projects", "generate_response")
    workflow.add_edge("answer_question", "generate_response")
    workflow.add_edge("collect_booking_info", "generate_response")
    workflow.add_edge("create_booking", "generate_response")
    workflow.add_edge("generate_response", END)

    return workflow.compile()


# Singleton agent instance
_agent = None


def get_agent():
    """Get or create the agent instance"""
    global _agent
    if _agent is None:
        _agent = create_agent_graph()
    return _agent
