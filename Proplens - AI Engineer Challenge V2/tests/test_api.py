"""
API Integration Tests
"""
import pytest
import uuid
from django.test import Client
from core.models import Conversation, Message, Project, Lead, Booking


@pytest.mark.django_db
class TestConversationAPI:
    """Test conversation creation and management"""

    def test_create_conversation(self):
        """Test POST /api/conversations/ creates a new conversation"""
        client = Client()
        response = client.post('/api/conversations/', content_type='application/json')

        assert response.status_code == 200
        data = response.json()
        assert 'conversation_id' in data

        # Verify conversation exists in database
        conversation_id = data['conversation_id']
        assert Conversation.objects.filter(conversation_id=conversation_id).exists()

    def test_create_multiple_conversations(self):
        """Test creating multiple conversations returns unique IDs"""
        client = Client()

        response1 = client.post('/api/conversations/', content_type='application/json')
        response2 = client.post('/api/conversations/', content_type='application/json')

        id1 = response1.json()['conversation_id']
        id2 = response2.json()['conversation_id']

        assert id1 != id2


@pytest.mark.django_db
class TestChatAPI:
    """Test chat endpoint and agent responses"""

    def test_chat_with_invalid_conversation_id(self):
        """Test chat with non-existent conversation returns 404"""
        client = Client()
        invalid_id = str(uuid.uuid4())

        response = client.post(
            '/api/agents/chat',
            data={
                'message': 'Hello',
                'conversation_id': invalid_id
            },
            content_type='application/json'
        )

        assert response.status_code == 404

    def test_chat_saves_messages(self):
        """Test that chat saves both user and assistant messages"""
        client = Client()

        # Create conversation
        conv_response = client.post('/api/conversations/', content_type='application/json')
        conversation_id = conv_response.json()['conversation_id']

        # Send message
        chat_response = client.post(
            '/api/agents/chat',
            data={
                'message': 'Hello, I am looking for a property',
                'conversation_id': conversation_id
            },
            content_type='application/json'
        )

        assert chat_response.status_code == 200

        # Verify messages are saved
        conversation = Conversation.objects.get(conversation_id=conversation_id)
        messages = Message.objects.filter(conversation=conversation)

        assert messages.count() >= 2  # At least user message and assistant response

    def test_chat_returns_expected_fields(self):
        """Test chat response contains all required fields"""
        client = Client()

        # Create conversation
        conv_response = client.post('/api/conversations/', content_type='application/json')
        conversation_id = conv_response.json()['conversation_id']

        # Send message
        chat_response = client.post(
            '/api/agents/chat',
            data={
                'message': 'I want a 2-bedroom apartment in Dubai',
                'conversation_id': conversation_id
            },
            content_type='application/json'
        )

        assert chat_response.status_code == 200
        data = chat_response.json()

        # Verify response structure
        assert 'reply' in data
        assert 'conversation_id' in data
        assert 'stage' in data
        assert isinstance(data['reply'], str)
        assert len(data['reply']) > 0


@pytest.mark.django_db
class TestHealthAPI:
    """Test health check endpoint"""

    def test_health_check_returns_status(self):
        """Test GET /api/health/ returns health status"""
        client = Client()
        response = client.get('/api/health/')

        assert response.status_code == 200
        data = response.json()

        assert 'status' in data
        assert 'message' in data
        assert 'database_connected' in data

    def test_health_check_database_connection(self):
        """Test health check verifies database connection"""
        client = Client()
        response = client.get('/api/health/')

        data = response.json()
        assert data['database_connected'] is True


@pytest.mark.django_db
class TestModels:
    """Test database models"""

    def test_create_project(self):
        """Test creating a project"""
        project = Project.objects.create(
            project_name="Test Project",
            city="Dubai",
            price_usd=5000000,
            area_sq_mtrs=200,
            no_of_bedrooms=2
        )

        assert project.id is not None
        assert project.project_name == "Test Project"

    def test_create_lead(self):
        """Test creating a lead"""
        lead = Lead.objects.create(
            first_name="John",
            last_name="Doe",
            email="john@example.com",
            preferences={"city": "Dubai", "bedrooms": 2}
        )

        assert lead.id is not None
        assert lead.email == "john@example.com"

    def test_create_booking(self):
        """Test creating a booking"""
        project = Project.objects.create(
            project_name="Test Project",
            city="Dubai",
            price_usd=5000000,
            area_sq_mtrs=200
        )

        lead = Lead.objects.create(
            first_name="Jane",
            last_name="Smith",
            email="jane@example.com"
        )

        booking = Booking.objects.create(
            lead=lead,
            project=project,
            status='pending'
        )

        assert booking.id is not None
        assert booking.status == 'pending'
        assert booking.lead == lead
        assert booking.project == project
