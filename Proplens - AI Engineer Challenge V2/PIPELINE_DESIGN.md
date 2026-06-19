# Property Sales Conversational Agent - Pipeline Design

## Executive Summary

This document outlines the complete execution pipeline for building a conversational AI assistant for **Silver Land Properties**. The assistant will help property buyers find suitable properties, answer questions, and book property viewings through an intelligent conversational interface powered by LangGraph, Vanna (Text-to-SQL), and Django Ninja REST API.

---

## 1. REQUIREMENTS SUMMARY

### Business Objectives
- **Company**: Silver Land Properties
- **Goal**: Greet users, understand preferences, recommend properties, answer questions, and drive property viewing bookings
- **Smart Matching**: Recommend closest matches if no direct matches available (cross-sell capability)

### Key User Journey
1. User opens chat → Assistant greets and probes preferences (location, budget, bedrooms)
2. Assistant searches database and presents 1-3 relevant projects
3. User asks questions → Assistant answers from DB or web search
4. User shows interest → Assistant collects lead details and books viewing

### Technical Requirements
- **Backend**: Django + Django Ninja (OOP controllers with Ninja Extra)
- **Agent Orchestration**: LangGraph with tool-based pattern
- **Database**: SQLite (with tables: projects, leads, bookings)
- **Text-to-SQL**: Vanna + ChromaDB
- **Web Search**: Optional tool for project-specific queries
- **LLM**: OpenAI GPT-4 or similar (with embedding model)
- **API Endpoints**:
  - `POST /conversations` (create new conversation)
  - `POST /agents/chat` (send message)

---

## 2. DATA ANALYSIS SUMMARY

### Dataset Overview
- **Total Records**: 1,071 property listings
- **Columns**: 15 (Project name, No of bedrooms, Price (USD), city, etc.)
- **Key Statistics**:
  - Price Range: $320K - $30M (avg: $14.8M)
  - Bedroom Range: 0-8 bedrooms
  - Top Cities: Dubai (282), Miami (147), Phuket (121), Bangkok (87)
  - Property Types: Apartment (820), Villa (86), House (1)

### Data Quality Assessment
- **Missing Values**:
  - completion_date (85%), unit_type (74%), bathrooms (18%), No of bedrooms (16%)
- **Key Columns Status**: All critical columns present (Project name, city, Price, No of bedrooms)
- **Readiness**: ✅ Data is sufficient for filtering by location, budget, and bedroom count

### Data Challenges
- High missing values in completion_date, unit_type
- Some records missing project names (15%)
- Strategy: Handle nulls gracefully, don't recommend projects with missing critical data

---

## 3. EXECUTION PIPELINE

### PHASE 0: PROJECT & FRAMEWORK SETUP

#### Purpose
Initialize Django project with all required dependencies and proper structure.

#### Steps
1. **Virtual Environment Setup**
   - Create/activate virtual environment using `uv venv`
   - Verify uv is available

2. **Dependency Installation** (via `uv pip install`)
   - Django >= 4.2
   - django-ninja >= 1.0
   - django-ninja-extra >= 0.20
   - langgraph >= 0.0.20
   - langchain >= 0.1.0
   - langchain-openai
   - vanna >= 0.3.0
   - chromadb >= 0.4.0
   - pandas
   - python-dotenv
   - duckduckgo-search (for web search tool)

3. **Django Project Structure**
   ```
   property_assistant/
   ├── manage.py
   ├── config/                  # Django project settings
   │   ├── __init__.py
   │   ├── settings.py
   │   ├── urls.py
   │   └── wsgi.py
   ├── apps/
   │   ├── agents/              # LangGraph agent logic
   │   │   ├── __init__.py
   │   │   ├── graph.py        # LangGraph workflow
   │   │   ├── tools.py        # SQL tool, web search tool
   │   │   └── prompts.py      # System prompts
   │   ├── api/                 # Django Ninja API
   │   │   ├── __init__.py
   │   │   ├── controllers.py  # OOP controllers
   │   │   └── schemas.py      # Pydantic schemas
   │   └── core/                # Models & business logic
   │       ├── __init__.py
   │       ├── models.py       # Django models
   │       └── services.py     # Business logic layer
   ├── data/
   │   ├── Property sales agent - Challenge.csv
   │   └── chroma_db/          # ChromaDB storage
   ├── .env                     # Environment variables (API keys)
   └── pyproject.toml          # UV dependency management
   ```

4. **Django Configuration**
   - Initialize Django project: `uv run django-admin startproject config .`
   - Create apps: `uv run python manage.py startapp core`, `agents`, `api`
   - Configure settings.py (INSTALLED_APPS, DATABASES)
   - Set up SQLite database

#### Outputs
- ✅ Functional Django project structure
- ✅ All dependencies installed via uv
- ✅ pyproject.toml with complete dependency list
- ✅ .env template for API keys

---

### PHASE 1: DATA INGESTION & DATABASE SETUP

#### Purpose
Load CSV data into SQLite database and set up Django models.

#### Steps

1. **Define Django Models** (`apps/core/models.py`)
   ```python
   class Project(models.Model):
       project_name = models.CharField(max_length=500)
       no_of_bedrooms = models.IntegerField(null=True)
       completion_status = models.CharField(max_length=50, null=True)
       bathrooms = models.IntegerField(null=True)
       unit_type = models.CharField(max_length=200, null=True)
       developer_name = models.CharField(max_length=300)
       price_usd = models.IntegerField()
       area_sq_mtrs = models.IntegerField()
       property_type = models.CharField(max_length=50, null=True)
       city = models.CharField(max_length=200)
       country = models.CharField(max_length=100)
       completion_date = models.DateField(null=True)
       features = models.JSONField(default=list)
       facilities = models.JSONField(default=list)
       project_description = models.TextField()

   class Lead(models.Model):
       first_name = models.CharField(max_length=200, null=True)
       last_name = models.CharField(max_length=200, null=True)
       email = models.EmailField(null=True)
       preferences = models.JSONField(default=dict)  # {city, budget_min, budget_max, bedrooms}
       created_at = models.DateTimeField(auto_now_add=True)

   class Conversation(models.Model):
       conversation_id = models.UUIDField(primary_key=True, default=uuid.uuid4)
       lead = models.ForeignKey(Lead, null=True, on_delete=models.SET_NULL)
       created_at = models.DateTimeField(auto_now_add=True)
       updated_at = models.DateTimeField(auto_now=True)

   class Message(models.Model):
       conversation = models.ForeignKey(Conversation, on_delete=models.CASCADE)
       role = models.CharField(max_length=20)  # 'user' or 'assistant'
       content = models.TextField()
       timestamp = models.DateTimeField(auto_now_add=True)

   class Booking(models.Model):
       lead = models.ForeignKey(Lead, on_delete=models.CASCADE)
       project = models.ForeignKey(Project, on_delete=models.CASCADE)
       booking_date = models.DateTimeField(auto_now_add=True)
       status = models.CharField(max_length=50, default='pending')
   ```

2. **Create Migration & Apply**
   - `uv run python manage.py makemigrations`
   - `uv run python manage.py migrate`

3. **CSV Import Script** (`scripts/load_data.py`)
   - Read CSV using pandas
   - Clean data (handle NaN, parse dates)
   - Bulk insert into Project model
   - Validation: Skip records with missing critical fields (project_name, city, price)

4. **Run Import**
   - `uv run python scripts/load_data.py`

#### Inputs
- `Property sales agent - Challenge.csv`

#### Outputs
- ✅ SQLite database with populated `core_project` table
- ✅ Django models for Project, Lead, Conversation, Message, Booking
- ✅ ~1000+ project records in database

---

### PHASE 2: TEXT-TO-SQL TOOL SETUP (VANNA + CHROMADB)

#### Purpose
Enable natural language queries over the projects database using Vanna.

#### Steps

1. **Initialize Vanna with ChromaDB**
   ```python
   # apps/agents/tools.py
   from vanna.chromadb import ChromaDB_VectorStore
   from vanna.openai import OpenAI_Chat

   class VannaSQL(ChromaDB_VectorStore, OpenAI_Chat):
       def __init__(self, config=None):
           ChromaDB_VectorStore.__init__(self, config=config)
           OpenAI_Chat.__init__(self, config=config)

   vn = VannaSQL(config={'path': './data/chroma_db'})
   vn.connect_to_sqlite('db.sqlite3')
   ```

2. **Train Vanna with DDL and Sample Queries**
   ```python
   # Training data
   ddl = """
   CREATE TABLE core_project (
       id INTEGER PRIMARY KEY,
       project_name TEXT,
       no_of_bedrooms INTEGER,
       price_usd INTEGER,
       city TEXT,
       country TEXT,
       property_type TEXT,
       project_description TEXT,
       ...
   );
   """
   vn.train(ddl=ddl)

   # Sample questions & SQL pairs
   vn.train(question="Find 2-bedroom apartments in Dubai",
            sql="SELECT * FROM core_project WHERE city='Dubai' AND no_of_bedrooms=2")
   vn.train(question="Properties under $5M in Miami",
            sql="SELECT * FROM core_project WHERE city='Miami' AND price_usd < 5000000")
   ```

3. **Create SQL Tool Function**
   ```python
   def text_to_sql_tool(query: str) -> dict:
       """
       Converts natural language to SQL and executes query.
       Returns: {'sql': '...', 'results': [...], 'error': None}
       """
       try:
           sql = vn.generate_sql(query)
           results = vn.run_sql(sql)
           return {'sql': sql, 'results': results, 'error': None}
       except Exception as e:
           return {'sql': None, 'results': [], 'error': str(e)}
   ```

4. **Test SQL Tool**
   - Test various natural language queries
   - Validate results against expected output

#### Inputs
- Django ORM schema (from models)
- SQLite database connection

#### Outputs
- ✅ Trained Vanna model stored in `data/chroma_db/`
- ✅ `text_to_sql_tool()` function ready for LangGraph integration
- ✅ Validated SQL generation for common query patterns

---

### PHASE 3: WEB SEARCH TOOL SETUP (OPTIONAL)

#### Purpose
Provide fallback web search for project-specific queries not in database.

#### Steps

1. **Implement Web Search Tool**
   ```python
   # apps/agents/tools.py
   from duckduckgo_search import DDGS

   def web_search_tool(query: str, project_name: str = None) -> dict:
       """
       Performs web search for project-specific information.
       Only invoked when database doesn't have the answer.
       Returns: {'results': [{'title': '...', 'snippet': '...', 'url': '...'}]}
       """
       try:
           if project_name:
               search_query = f"{project_name} {query}"
           else:
               search_query = query

           ddgs = DDGS()
           results = ddgs.text(search_query, max_results=3)
           return {'results': results, 'error': None}
       except Exception as e:
           return {'results': [], 'error': str(e)}
   ```

2. **Test Web Search Tool**
   - Test: "What are the schools near St. Regis Chicago?"
   - Validate results quality

#### Inputs
- User query string
- Optional: project_name for context

#### Outputs
- ✅ `web_search_tool()` function ready for LangGraph
- ✅ Validated search results for sample queries

---

### PHASE 4: LANGGRAPH AGENT ORCHESTRATION

#### Purpose
Build conversational agent workflow using LangGraph with state management and tool routing.

#### Steps

1. **Define Agent State**
   ```python
   # apps/agents/graph.py
   from typing import TypedDict, List, Optional

   class AgentState(TypedDict):
       messages: List[dict]  # Conversation history
       conversation_id: str
       lead_id: Optional[int]
       preferences: dict  # {city, budget_min, budget_max, bedrooms}
       shortlisted_projects: List[dict]
       current_stage: str  # 'greeting', 'probing', 'recommending', 'answering', 'booking'
       pending_booking: Optional[dict]
   ```

2. **Define Agent Nodes**
   ```python
   # Node 1: Route user intent
   def route_intent(state: AgentState) -> str:
       """Determines next action based on conversation stage and user message"""
       # Returns: 'probe_preferences', 'search_projects', 'answer_question', 'collect_booking_info'

   # Node 2: Extract preferences from user message
   def extract_preferences(state: AgentState) -> AgentState:
       """Uses LLM to extract location, budget, bedrooms from user input"""

   # Node 3: Search projects (calls text_to_sql_tool)
   def search_projects(state: AgentState) -> AgentState:
       """
       Constructs SQL query based on preferences.
       Calls text_to_sql_tool, ranks results, selects top 1-3.
       """

   # Node 4: Answer questions (calls text_to_sql_tool or web_search_tool)
   def answer_question(state: AgentState) -> AgentState:
       """
       Determines if question is about specific project in DB.
       Routes to text_to_sql_tool or web_search_tool accordingly.
       """

   # Node 5: Collect booking information
   def collect_booking_info(state: AgentState) -> AgentState:
       """Extracts name, email, chosen project from user message"""

   # Node 6: Create booking
   def create_booking(state: AgentState) -> AgentState:
       """Saves lead and booking to database"""

   # Node 7: Generate response
   def generate_response(state: AgentState) -> AgentState:
       """Uses LLM to craft natural conversational response"""
   ```

3. **Build LangGraph Workflow**
   ```python
   from langgraph.graph import StateGraph, END

   workflow = StateGraph(AgentState)

   # Add nodes
   workflow.add_node("route", route_intent)
   workflow.add_node("extract_prefs", extract_preferences)
   workflow.add_node("search", search_projects)
   workflow.add_node("answer", answer_question)
   workflow.add_node("collect_booking", collect_booking_info)
   workflow.add_node("save_booking", create_booking)
   workflow.add_node("respond", generate_response)

   # Add edges (routing logic)
   workflow.set_entry_point("route")
   workflow.add_conditional_edges(
       "route",
       lambda state: state["current_stage"],
       {
           "probing": "extract_prefs",
           "recommending": "search",
           "answering": "answer",
           "booking": "collect_booking",
       }
   )
   workflow.add_edge("extract_prefs", "respond")
   workflow.add_edge("search", "respond")
   workflow.add_edge("answer", "respond")
   workflow.add_edge("collect_booking", "save_booking")
   workflow.add_edge("save_booking", "respond")
   workflow.add_edge("respond", END)

   agent = workflow.compile()
   ```

4. **System Prompt Design** (`apps/agents/prompts.py`)
   ```python
   SYSTEM_PROMPT = """
   You are a property sales assistant for Silver Land Properties.

   Your goals:
   1. Greet the user warmly
   2. Understand their preferences (location, budget, bedroom count)
   3. Recommend 1-3 suitable properties from the database
   4. Answer questions about projects
   5. Propose property viewings and collect lead information

   Guidelines:
   - Be conversational and friendly
   - Never hallucinate details not in the database
   - If information is unavailable, say so clearly
   - Always try to drive toward booking a property viewing
   - For broad recommendations, search the database only
   - For specific project queries (e.g., "schools near X"), use web search if needed
   """
   ```

5. **Test Agent Workflow**
   - Simulate full conversation flow
   - Test edge cases (no matches, invalid preferences, etc.)

#### Inputs
- User message
- Conversation state (from database)

#### Outputs
- ✅ Compiled LangGraph agent with tool integration
- ✅ State management for conversation context
- ✅ Routing logic for different conversation stages
- ✅ Validated agent behavior through test scenarios

---

### PHASE 5: DJANGO NINJA REST API

#### Purpose
Expose the agent via RESTful API using Django Ninja with OOP controllers.

#### Steps

1. **Define Pydantic Schemas** (`apps/api/schemas.py`)
   ```python
   from pydantic import BaseModel, UUID4
   from typing import Optional, List

   class ConversationCreateRequest(BaseModel):
       pass  # No body required

   class ConversationCreateResponse(BaseModel):
       conversation_id: UUID4

   class ChatRequest(BaseModel):
       message: str
       conversation_id: UUID4

   class ChatResponse(BaseModel):
       reply: str
       conversation_id: UUID4
       shortlisted_projects: Optional[List[dict]] = None
       stage: str
   ```

2. **Create OOP Controllers** (`apps/api/controllers.py`)
   ```python
   from ninja_extra import api_controller, http_post
   from .schemas import *
   from apps.core.models import Conversation, Message, Lead
   from apps.agents.graph import agent

   @api_controller('/conversations', tags=['Conversations'])
   class ConversationController:
       @http_post('/', response=ConversationCreateResponse)
       def create_conversation(self, request):
           """Create a new conversation session"""
           conversation = Conversation.objects.create()
           return {'conversation_id': conversation.conversation_id}

   @api_controller('/agents', tags=['Agent'])
   class AgentController:
       @http_post('/chat', response=ChatResponse)
       def chat(self, request, payload: ChatRequest):
           """Send message to the agent and get response"""
           # 1. Load conversation from DB
           conversation = Conversation.objects.get(conversation_id=payload.conversation_id)

           # 2. Save user message
           Message.objects.create(conversation=conversation, role='user', content=payload.message)

           # 3. Load conversation history
           messages = Message.objects.filter(conversation=conversation).order_by('timestamp')

           # 4. Build agent state
           state = {
               'messages': [{'role': m.role, 'content': m.content} for m in messages],
               'conversation_id': str(conversation.conversation_id),
               'lead_id': conversation.lead_id,
               'preferences': conversation.lead.preferences if conversation.lead else {},
               'shortlisted_projects': [],
               'current_stage': 'greeting',
               'pending_booking': None
           }

           # 5. Run agent
           result = agent.invoke(state)

           # 6. Save assistant response
           assistant_message = result['messages'][-1]['content']
           Message.objects.create(conversation=conversation, role='assistant', content=assistant_message)

           # 7. Update lead if extracted
           if result.get('lead_id') and not conversation.lead:
               lead = Lead.objects.get(id=result['lead_id'])
               conversation.lead = lead
               conversation.save()

           return {
               'reply': assistant_message,
               'conversation_id': conversation.conversation_id,
               'shortlisted_projects': result.get('shortlisted_projects'),
               'stage': result.get('current_stage')
           }
   ```

3. **Register Controllers** (`config/urls.py`)
   ```python
   from ninja_extra import NinjaExtraAPI
   from apps.api.controllers import ConversationController, AgentController

   api = NinjaExtraAPI()
   api.register_controllers(ConversationController, AgentController)

   urlpatterns = [
       path('admin/', admin.site.urls),
       path('api/', api.urls),
   ]
   ```

4. **Test API Endpoints**
   - Test `POST /api/conversations` → Returns conversation_id
   - Test `POST /api/agents/chat` → Returns agent response
   - Validate full conversation flow via API

#### Inputs
- HTTP requests (JSON payloads)

#### Outputs
- ✅ `POST /api/conversations` endpoint
- ✅ `POST /api/agents/chat` endpoint
- ✅ Proper HTTP status codes (200, 400, 404, 500)
- ✅ Validated API behavior with test requests

---

### PHASE 6: TESTING & VALIDATION

#### Purpose
Ensure all components work correctly and the agent achieves business goals.

#### Steps

1. **Unit Tests**
   - Test Vanna SQL generation
   - Test web search tool
   - Test individual agent nodes
   - Test API endpoints

2. **Integration Tests**
   - Test full conversation flow (greeting → probing → recommending → booking)
   - Test edge cases:
     - No matching properties
     - Invalid conversation_id
     - Missing preferences
     - Web search fallback

3. **Conversation Quality Tests**
   - Test: Agent maintains coherent conversation
   - Test: Agent correctly extracts preferences
   - Test: Agent recommends relevant properties
   - Test: Agent drives toward booking goal
   - Test: Agent admits when information is unavailable

4. **Create Test Suite** (`tests/test_agent.py`, `tests/test_api.py`)
   ```python
   # Example test
   def test_full_conversation_flow():
       # 1. Create conversation
       response = client.post('/api/conversations')
       conversation_id = response.json()['conversation_id']

       # 2. User sends initial message
       response = client.post('/api/agents/chat', json={
           'message': 'Hi, I am looking for a 2-bedroom apartment in Dubai under $10M',
           'conversation_id': conversation_id
       })
       assert response.status_code == 200
       assert 'Dubai' in response.json()['reply']

       # 3. Continue conversation...
   ```

#### Inputs
- Test scenarios and expected outcomes

#### Outputs
- ✅ Comprehensive test suite with >80% coverage
- ✅ All tests passing
- ✅ Documentation of test results

---

### PHASE 7: DOCUMENTATION

#### Purpose
Provide clear documentation for setup, usage, and deployment.

#### Steps

1. **README.md**
   - Project overview
   - Features
   - Tech stack
   - Setup instructions (with uv)
   - API documentation
   - Environment variables

2. **API Documentation**
   - Endpoint descriptions
   - Request/response schemas
   - Example requests (curl/httpie)

3. **Architecture Documentation**
   - System architecture diagram
   - LangGraph workflow diagram
   - Database schema
   - Tool descriptions

4. **Deployment Guide**
   - Render/Vercel deployment steps
   - Environment configuration
   - Database migration steps

#### Outputs
- ✅ README.md
- ✅ API_DOCS.md
- ✅ ARCHITECTURE.md
- ✅ DEPLOYMENT.md

---

### PHASE 8: DEPLOYMENT (RENDER/VERCEL)

#### Purpose
Deploy the application to a cloud platform for live testing.

#### Steps

1. **Prepare for Deployment**
   - Create `requirements.txt` from pyproject.toml
   - Add `Procfile` or `vercel.json`
   - Set up environment variables

2. **Deploy to Render**
   - Create new Web Service
   - Connect GitHub repository
   - Set build command: `uv pip install -r requirements.txt && uv run python manage.py migrate`
   - Set start command: `uv run gunicorn config.wsgi:application`
   - Configure environment variables (OPENAI_API_KEY, etc.)

3. **Verify Deployment**
   - Test API endpoints on deployed URL
   - Monitor logs for errors
   - Test full conversation flow

#### Inputs
- Configured application
- Cloud platform account (Render/Vercel)

#### Outputs
- ✅ Live deployment URL
- ✅ Functional API endpoints in production
- ✅ Deployment documentation

---

## 4. TECHNOLOGY STACK

| Component | Technology | Rationale |
|-----------|-----------|-----------|
| Backend Framework | Django 4.2 + Django Ninja | RESTful API with type safety, OOP controllers |
| Agent Orchestration | LangGraph | State management, tool routing, conversation flow |
| Text-to-SQL | Vanna + ChromaDB | Natural language to SQL translation with vector storage |
| Database | SQLite | Simple, file-based, sufficient for prototype |
| LLM | OpenAI GPT-4 | High-quality reasoning and conversation |
| Embeddings | OpenAI text-embedding-3-small | For Vanna semantic search |
| Web Search | DuckDuckGo Search | Free, no API key required |
| Package Manager | uv | Fast Python package manager |
| Deployment | Render | Easy Django deployment with free tier |

---

## 5. RISK MITIGATION

| Risk | Mitigation |
|------|-----------|
| High missing values in CSV | Skip records with missing critical fields during import |
| SQL injection via Vanna | Vanna uses parameterized queries by default |
| LLM hallucination | System prompt explicitly instructs to admit when information is unavailable |
| API rate limits | Implement caching for repeated queries |
| No matching properties | Agent recommends closest matches, explains differences |

---

## 6. SUCCESS CRITERIA

### Functional Requirements
- ✅ Agent can greet users and extract preferences
- ✅ Agent can search database and recommend 1-3 properties
- ✅ Agent can answer questions using DB or web search
- ✅ Agent can collect lead information and save bookings
- ✅ Agent maintains coherent conversation flow

### Technical Requirements
- ✅ LangGraph-based agent with proper state management
- ✅ Vanna text-to-SQL working for common queries
- ✅ Django Ninja API with OOP controllers
- ✅ SQLite database with proper schema
- ✅ Clean, modular Python code
- ✅ Test coverage >80%
- ✅ Deployed to Render/Vercel

### Business Requirements
- ✅ Agent drives users toward property viewing bookings
- ✅ Agent handles objections gracefully
- ✅ Agent doesn't hallucinate unavailable information

---

## 7. ASSUMPTIONS

1. **UV Availability**: UV package manager is installed and available
2. **API Keys**: OpenAI API key is available for LLM and embeddings
3. **Data Quality**: CSV data is representative of production data
4. **Deployment**: Render or Vercel free tier is sufficient for prototype
5. **Authentication**: Authentication is optional for MVP
6. **Conversation Persistence**: Conversations stored in SQLite (no Redis/session store needed)
7. **Concurrency**: Single-threaded execution sufficient for prototype

---

## 8. NEXT STEPS (AFTER PIPELINE EXECUTION)

1. User Acceptance Testing (UAT) with stakeholders
2. Performance optimization (caching, query optimization)
3. Add authentication and authorization
4. Implement conversation analytics
5. Fine-tune LLM prompts based on user feedback
6. Add more training data to Vanna for better SQL generation
7. Implement email notifications for bookings
8. Add admin dashboard for monitoring bookings

---

## APPENDIX: KEY COMMANDS

```bash
# Setup
uv venv
uv pip install -r requirements.txt

# Database
uv run python manage.py makemigrations
uv run python manage.py migrate
uv run python scripts/load_data.py

# Development
uv run python manage.py runserver

# Testing
uv run pytest

# Deployment (Render)
uv pip install -r requirements.txt && uv run python manage.py migrate
uv run gunicorn config.wsgi:application
```

---

**Document Version**: 1.0
**Last Updated**: 2025-12-14
**Author**: AI Engineering Team
