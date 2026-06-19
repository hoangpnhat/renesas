from django.contrib import admin
from .models import Project, Lead, Conversation, Message, Booking


@admin.register(Project)
class ProjectAdmin(admin.ModelAdmin):
    """Admin interface for Project model"""
    list_display = ['project_name', 'city', 'country', 'price_usd', 'no_of_bedrooms', 'property_type']
    list_filter = ['city', 'country', 'property_type', 'no_of_bedrooms']
    search_fields = ['project_name', 'city', 'developer_name', 'project_description']
    list_per_page = 50

    fieldsets = (
        ('Basic Information', {
            'fields': ('project_name', 'city', 'country', 'property_type')
        }),
        ('Details', {
            'fields': ('no_of_bedrooms', 'bathrooms', 'area_sq_mtrs', 'unit_type')
        }),
        ('Pricing', {
            'fields': ('price_usd',)
        }),
        ('Developer', {
            'fields': ('developer_name', 'completion_status', 'completion_date')
        }),
        ('Description', {
            'fields': ('project_description', 'features', 'facilities'),
            'classes': ('collapse',)
        }),
    )


@admin.register(Lead)
class LeadAdmin(admin.ModelAdmin):
    """Admin interface for Lead model"""
    list_display = ['id', 'first_name', 'last_name', 'email', 'created_at']
    search_fields = ['first_name', 'last_name', 'email']
    list_filter = ['created_at']
    readonly_fields = ['created_at', 'updated_at']

    fieldsets = (
        ('Contact Information', {
            'fields': ('first_name', 'last_name', 'email')
        }),
        ('Preferences', {
            'fields': ('preferences',)
        }),
        ('Timestamps', {
            'fields': ('created_at', 'updated_at'),
            'classes': ('collapse',)
        }),
    )


@admin.register(Conversation)
class ConversationAdmin(admin.ModelAdmin):
    """Admin interface for Conversation model"""
    list_display = ['conversation_id', 'lead', 'created_at', 'message_count']
    list_filter = ['created_at']
    search_fields = ['conversation_id', 'lead__email', 'lead__first_name', 'lead__last_name']
    readonly_fields = ['conversation_id', 'created_at', 'updated_at']

    def message_count(self, obj):
        return obj.messages.count()
    message_count.short_description = 'Messages'


@admin.register(Message)
class MessageAdmin(admin.ModelAdmin):
    """Admin interface for Message model"""
    list_display = ['id', 'conversation', 'role', 'content_preview', 'timestamp']
    list_filter = ['role', 'timestamp']
    search_fields = ['content', 'conversation__conversation_id']
    readonly_fields = ['timestamp']

    def content_preview(self, obj):
        return obj.content[:100] + '...' if len(obj.content) > 100 else obj.content
    content_preview.short_description = 'Content'


@admin.register(Booking)
class BookingAdmin(admin.ModelAdmin):
    """Admin interface for Booking model"""
    list_display = ['id', 'lead', 'project', 'status', 'booking_date']
    list_filter = ['status', 'booking_date']
    search_fields = ['lead__email', 'lead__first_name', 'lead__last_name', 'project__project_name']
    readonly_fields = ['booking_date', 'updated_at']

    fieldsets = (
        ('Booking Information', {
            'fields': ('lead', 'project', 'conversation', 'status')
        }),
        ('Notes', {
            'fields': ('notes',)
        }),
        ('Timestamps', {
            'fields': ('booking_date', 'updated_at'),
            'classes': ('collapse',)
        }),
    )
