"""
Pydantic schemas for API requests and responses
"""
from typing import Optional, List, Dict, Any
from pydantic import BaseModel, Field, UUID4, EmailStr


# Conversation Endpoints
class ConversationCreateResponse(BaseModel):
    """Response for creating a new conversation"""
    conversation_id: UUID4 = Field(..., description="Unique conversation identifier")


# Agent Chat Endpoints
class ChatRequest(BaseModel):
    """Request schema for sending a message to the agent"""
    message: str = Field(..., min_length=1, max_length=2000, description="User's message")
    conversation_id: UUID4 = Field(..., description="Conversation ID from /conversations endpoint")


class ProjectSummary(BaseModel):
    """Summary of a shortlisted project"""
    id: int
    project_name: Optional[str]
    city: str
    price_usd: int
    bedrooms: Optional[int]
    property_type: Optional[str]


class ChatResponse(BaseModel):
    """Response schema for agent chat"""
    reply: str = Field(..., description="Agent's response message")
    conversation_id: UUID4 = Field(..., description="Conversation identifier")
    stage: str = Field(..., description="Current conversation stage")
    shortlisted_projects: Optional[List[ProjectSummary]] = Field(
        default=None,
        description="List of recommended properties (if applicable)"
    )
    preferences: Optional[Dict[str, Any]] = Field(
        default=None,
        description="User's current preferences"
    )


# Health Check
class HealthCheckResponse(BaseModel):
    """Health check response"""
    status: str = Field(..., description="API status")
    message: str = Field(..., description="Status message")
    database_connected: bool = Field(..., description="Database connection status")
