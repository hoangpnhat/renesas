"""
Django models for Property Sales Assistant
"""
import uuid
from django.db import models


class Project(models.Model):
    """Property project model - stores property listings from CSV"""

    project_name = models.CharField(max_length=500, null=True, blank=True)
    no_of_bedrooms = models.IntegerField(null=True, blank=True)
    completion_status = models.CharField(max_length=50, null=True, blank=True)
    bathrooms = models.IntegerField(null=True, blank=True)
    unit_type = models.CharField(max_length=200, null=True, blank=True)
    developer_name = models.CharField(max_length=300, null=True, blank=True)
    price_usd = models.IntegerField()
    area_sq_mtrs = models.IntegerField()
    property_type = models.CharField(max_length=50, null=True, blank=True)
    city = models.CharField(max_length=200)
    country = models.CharField(max_length=100, null=True, blank=True)
    completion_date = models.DateField(null=True, blank=True)
    features = models.JSONField(default=list, blank=True)
    facilities = models.JSONField(default=list, blank=True)
    project_description = models.TextField(blank=True)

    created_at = models.DateTimeField(auto_now_add=True)
    updated_at = models.DateTimeField(auto_now=True)

    class Meta:
        db_table = 'core_project'
        indexes = [
            models.Index(fields=['city']),
            models.Index(fields=['price_usd']),
            models.Index(fields=['no_of_bedrooms']),
            models.Index(fields=['property_type']),
        ]

    def __str__(self):
        return f"{self.project_name} - {self.city}" if self.project_name else f"Project {self.id}"


class Lead(models.Model):
    """Lead model - stores potential customer information"""

    first_name = models.CharField(max_length=200, null=True, blank=True)
    last_name = models.CharField(max_length=200, null=True, blank=True)
    email = models.EmailField(null=True, blank=True)
    preferences = models.JSONField(
        default=dict,
        blank=True,
        help_text="Stores user preferences: city, budget_min, budget_max, bedrooms, property_type"
    )

    created_at = models.DateTimeField(auto_now_add=True)
    updated_at = models.DateTimeField(auto_now=True)

    class Meta:
        db_table = 'core_lead'

    def __str__(self):
        if self.first_name or self.last_name:
            return f"{self.first_name or ''} {self.last_name or ''}".strip()
        return f"Lead {self.id}"


class Conversation(models.Model):
    """Conversation model - tracks chat sessions"""

    conversation_id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    lead = models.ForeignKey(Lead, null=True, blank=True, on_delete=models.SET_NULL, related_name='conversations')

    created_at = models.DateTimeField(auto_now_add=True)
    updated_at = models.DateTimeField(auto_now=True)

    class Meta:
        db_table = 'core_conversation'

    def __str__(self):
        return f"Conversation {self.conversation_id}"


class Message(models.Model):
    """Message model - stores individual chat messages"""

    ROLE_CHOICES = [
        ('user', 'User'),
        ('assistant', 'Assistant'),
        ('system', 'System'),
    ]

    conversation = models.ForeignKey(Conversation, on_delete=models.CASCADE, related_name='messages')
    role = models.CharField(max_length=20, choices=ROLE_CHOICES)
    content = models.TextField()
    metadata = models.JSONField(
        default=dict,
        blank=True,
        help_text="Additional metadata like tool calls, shortlisted projects, etc."
    )

    timestamp = models.DateTimeField(auto_now_add=True)

    class Meta:
        db_table = 'core_message'
        ordering = ['timestamp']

    def __str__(self):
        return f"{self.role}: {self.content[:50]}"


class Booking(models.Model):
    """Booking model - stores property viewing bookings"""

    STATUS_CHOICES = [
        ('pending', 'Pending'),
        ('confirmed', 'Confirmed'),
        ('completed', 'Completed'),
        ('cancelled', 'Cancelled'),
    ]

    lead = models.ForeignKey(Lead, on_delete=models.CASCADE, related_name='bookings')
    project = models.ForeignKey(Project, on_delete=models.CASCADE, related_name='bookings')
    conversation = models.ForeignKey(
        Conversation,
        null=True,
        blank=True,
        on_delete=models.SET_NULL,
        related_name='bookings'
    )

    status = models.CharField(max_length=50, choices=STATUS_CHOICES, default='pending')
    notes = models.TextField(blank=True, help_text="Additional notes about the booking")

    booking_date = models.DateTimeField(auto_now_add=True)
    updated_at = models.DateTimeField(auto_now=True)

    class Meta:
        db_table = 'core_booking'
        ordering = ['-booking_date']

    def __str__(self):
        return f"Booking: {self.lead} - {self.project}"
