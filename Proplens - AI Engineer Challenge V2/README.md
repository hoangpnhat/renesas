# Property Sales AI Assistant

A conversational AI assistant for Silver Land Properties that helps buyers find properties and book viewings. Built with Django, Django Ninja, and LangGraph.

## Features

- **Intelligent Conversation**: Natural language understanding of property preferences
- **Smart Recommendations**: Text-to-SQL powered property search using Vanna AI
- **Question Answering**: Answers property-specific questions from database
- **Booking Management**: Collects lead information and schedules viewings
- **RESTful API**: Clean REST API built with Django Ninja
- **LangGraph Orchestration**: State-based conversation flow management

## Tech Stack

- **Backend**: Django 5.2
- **API Framework**: Django Ninja
- **Agent Framework**: LangGraph
- **Text-to-SQL**: Vanna AI + ChromaDB
- **LLM**: OpenAI GPT-4
- **Database**: SQLite (easily upgradable to PostgreSQL)
- **Package Manager**: UV

## Project Structure

```
property-sales-assistant/
├── config/                 # Django project settings
│   ├── settings.py
│   ├── urls.py
│   └── wsgi.py
├── core/                   # Django app - models & business logic
│   ├── models.py          # Project, Lead, Conversation, Message, Booking models
│   └── migrations/
├── agents/                 # LangGraph agent orchestration
│   ├── graph.py           # Agent workflow & nodes
│   ├── tools.py           # Text-to-SQL & web search tools
│   └── prompts.py         # System prompts
├── api/                    # Django Ninja REST API
│   ├── controllers.py     # API endpoints
│   └── schemas.py         # Pydantic request/response schemas
├── scripts/                # Utility scripts
│   ├── load_data.py       # CSV data loader
│   └── test_vanna.py      # Vanna tool tester
├── data/
│   └── chroma_db/         # ChromaDB vector store for Vanna
├── tests/                  # Test suite
├── .env                    # Environment variables
├── manage.py              # Django management script
├── pyproject.toml         # UV project dependencies
└── README.md
```

## Setup Instructions

### Prerequisites

- Python 3.10+
- UV package manager ([installation](https://github.com/astral-sh/uv))
- OpenAI API key

### Installation

1. **Clone the repository**
   ```bash
   cd "Proplens - AI Engineer Challenge V2"
   ```

2. **Install dependencies with UV**
   ```bash
   uv pip install django django-ninja django-ninja-extra langgraph langchain langchain-openai langchain-community vanna chromadb pandas python-dotenv duckduckgo-search gunicorn pytest pytest-django httpx
   ```

3. **Configure environment variables**
   ```bash
   cp .env.example .env
   # Edit .env and add your OPENAI_API_KEY
   ```

4. **Run database migrations**
   ```bash
   python manage.py migrate
   ```

5. **Load property data**
   ```bash
   python scripts/load_data.py
   ```
   This will load 1,066 property records from the CSV file.

6. **Start the development server**
   ```bash
   python manage.py runserver
   ```

The API will be available at `http://localhost:8000/api/`

### API Documentation

Once the server is running, visit:
- **Swagger UI**: http://localhost:8000/api/docs
- **API Endpoints**: http://localhost:8000/api/

## API Usage

### 1. Create a Conversation

```bash
POST http://localhost:8000/api/conversations/

Response:
{
  "conversation_id": "123e4567-e89b-12d3-a456-426614174000"
}
```

### 2. Chat with the Agent

```bash
POST http://localhost:8000/api/agents/chat

Request:
{
  "message": "I'm looking for a 2-bedroom apartment in Dubai under $5 million",
  "conversation_id": "123e4567-e89b-12d3-a456-426614174000"
}

Response:
{
  "reply": "Great! I found 3 properties in Dubai that match your criteria...",
  "conversation_id": "123e4567-e89b-12d3-a456-426614174000",
  "stage": "recommending",
  "shortlisted_projects": [
    {
      "id": 1,
      "project_name": "Dubai Marina Tower",
      "city": "Dubai",
      "price_usd": 2800000,
      "bedrooms": 2,
      "property_type": "apartment"
    }
  ],
  "preferences": {
    "city": "Dubai",
    "bedrooms": 2,
    "budget_max": 5000000
  }
}
```

### 3. Health Check

```bash
GET http://localhost:8000/api/health/

Response:
{
  "status": "healthy",
  "message": "Property Sales AI Assistant API is running",
  "database_connected": true
}
```

## Example Conversation Flow

```
User: Hi, I'm looking for a property
Agent: Hello! I'd be happy to help you find the perfect property. Could you tell me which city you're interested in?

User: I'm thinking Dubai or Miami
Agent: Great choices! And what's your budget range?

User: Around $10-15 million
Agent: Perfect! How many bedrooms are you looking for?

User: 3 bedrooms
Agent: Excellent! Let me search for 3-bedroom properties in Dubai and Miami between $10M-$15M...
     [Agent presents 3 properties]
     Would you like to schedule a viewing for any of these?

User: Yes, I'd like to see the first one
Agent: Wonderful! I'd be happy to arrange a viewing. Could I get your name and email address?

User: John Doe, john@example.com
Agent: Thank you, John! I've scheduled a viewing for [Property Name]. Someone from our team will contact you at john@example.com shortly!
```

## Agent Workflow

The LangGraph agent follows this workflow:

1. **Route Intent**: Determine user's intent (greeting, preferences, question, booking)
2. **Extract Preferences**: Use LLM to extract city, budget, bedrooms from user message
3. **Search Properties**: Query database using extracted preferences
4. **Answer Questions**: Respond to specific queries about properties
5. **Collect Booking Info**: Extract name and email for booking
6. **Create Booking**: Save lead and booking to database
7. **Generate Response**: Create natural, contextual response

## Environment Variables

```env
# Required
OPENAI_API_KEY=your_openai_api_key_here

# Optional (with defaults)
SECRET_KEY=django-insecure-development-key
DEBUG=True
ALLOWED_HOSTS=localhost,127.0.0.1
```

## Database Models

- **Project**: Property listings (1,066 records)
- **Lead**: Customer information and preferences
- **Conversation**: Chat sessions
- **Message**: Individual messages in conversations
- **Booking**: Property viewing bookings

## Testing

```bash
# Run tests
pytest

# Test Vanna SQL tool
python scripts/test_vanna.py

# Test full conversation flow
# (See tests/test_integration.py)
```

## Deployment

### Render Deployment

1. Create a new Web Service on Render
2. Connect your GitHub repository
3. Configure:
   ```
   Build Command: uv pip install django django-ninja django-ninja-extra langgraph langchain langchain-openai vanna chromadb pandas python-dotenv gunicorn && python manage.py migrate
   Start Command: gunicorn config.wsgi:application --bind 0.0.0.0:$PORT
   ```
4. Add environment variables:
   - `OPENAI_API_KEY`
   - `SECRET_KEY`
   - `DEBUG=False`
   - `ALLOWED_HOSTS=your-app-name.onrender.com`

### Vercel Deployment

1. Install Vercel CLI: `npm install -g vercel`
2. Create `vercel.json`:
   ```json
   {
     "builds": [{
       "src": "config/wsgi.py",
       "use": "@vercel/python"
     }],
     "routes": [{
       "src": "/(.*)",
       "dest": "config/wsgi.py"
     }]
   }
   ```
3. Deploy: `vercel --prod`

## Architecture Highlights

### LangGraph Agent State Management
The agent maintains conversation state including:
- Message history
- User preferences (city, budget, bedrooms)
- Shortlisted properties
- Current conversation stage
- Pending booking information

### Text-to-SQL with Vanna
Vanna AI translates natural language to SQL queries:
- Trained on database schema (DDL)
- Sample question-SQL pairs for few-shot learning
- ChromaDB for semantic search over training data
- Connected directly to SQLite database

### Django Ninja API
Clean, type-safe REST API:
- Pydantic schemas for request/response validation
- Automatic OpenAPI documentation
- Router-based organization
- HTTP status codes

## Performance Considerations

- **Caching**: Implement Redis caching for repeated queries
- **Database**: Indexes on city, price_usd, no_of_bedrooms
- **LLM**: Using GPT-4-mini for cost efficiency
- **Async**: Consider async endpoints for high concurrency

## Future Enhancements

- [ ] Web search fallback for project-specific queries
- [ ] Email notifications for bookings
- [ ] Admin dashboard for monitoring
- [ ] Multi-language support
- [ ] Property image handling
- [ ] User authentication
- [ ] Conversation analytics
- [ ] Fine-tuned LLM for domain-specific responses

## Troubleshooting

### Vanna Not Training
```python
# Clear ChromaDB and retrain
rm -rf data/chroma_db
python scripts/test_vanna.py
```

### Database Errors
```bash
# Reset database
rm db.sqlite3
python manage.py migrate
python scripts/load_data.py
```

### API Not Responding
```bash
# Check server logs
# Verify OPENAI_API_KEY is set
# Test health endpoint: curl http://localhost:8000/api/health/
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

MIT License

## Contact

For questions or support, please contact the development team.

---

**Built with ❤️ using Django, LangGraph, and OpenAI**
