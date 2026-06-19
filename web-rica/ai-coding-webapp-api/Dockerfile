FROM python:3.11-slim

WORKDIR /app

# Install system dependencies for Poppler, LibreOffice, and Supervisor
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    supervisor \
    poppler-utils \
    libreoffice \
    libreoffice-writer \
    libreoffice-calc \
    libreoffice-impress \
    fonts-noto-cjk \
    fonts-ipafont \
    fonts-takao \
    fonts-vlgothic\
    && rm -rf /var/lib/apt/lists/*

# Install Python dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy application code
# Copy application code
COPY . .

# Copy supervisor configuration
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf

# Set environment variables
ENV PYTHONUNBUFFERED=1 \
    PYTHONDONTWRITEBYTECODE=1

# Copy supervisor configuration
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf

# Set environment variables
ENV PYTHONUNBUFFERED=1 \
    PYTHONDONTWRITEBYTECODE=1

EXPOSE 80

# Run supervisor to manage both Uvicorn and Taskiq worker
CMD ["supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]
