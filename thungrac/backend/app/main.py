"""
Smart Trash Bin System - FastAPI Application
Main entry point for the backend API server.
"""
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from fastapi.staticfiles import StaticFiles
from fastapi.responses import HTMLResponse
from pathlib import Path

from app.config import settings, ensure_directories
from app.api.routes import esp32, students, statistics
from app.services.session_manager import session_manager

# Ensure all required directories exist
ensure_directories()

# Create FastAPI application
app = FastAPI(
    title="Smart Trash Bin System API",
    description="Backend API for school smart trash bin with face recognition and trash classification",
    version="1.0.0",
    docs_url="/api/docs",
    redoc_url="/api/redoc"
)

# Configure CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=settings.ALLOW_ORIGINS,
    allow_credentials=settings.ALLOW_CREDENTIALS,
    allow_methods=settings.ALLOW_METHODS,
    allow_headers=settings.ALLOW_HEADERS,
)

# Include API routers
app.include_router(esp32.router, prefix="/api")
app.include_router(students.router, prefix="/api")
app.include_router(statistics.router, prefix="/api")

# Mount frontend static files
frontend_path = Path(__file__).parent.parent.parent / "frontend"
if frontend_path.exists():
    app.mount("/assets", StaticFiles(directory=str(frontend_path / "assets")), name="assets")
    app.mount("/css", StaticFiles(directory=str(frontend_path / "css")), name="css")
    app.mount("/js", StaticFiles(directory=str(frontend_path / "js")), name="js")

# Mount data directory for trash images
data_path = Path(__file__).parent / "data"
if data_path.exists():
    app.mount("/data", StaticFiles(directory=str(data_path)), name="data")


# Root endpoint
@app.get("/", response_class=HTMLResponse)
async def root():
    """Serve the landing page."""
    frontend_path = Path(__file__).parent.parent.parent / "frontend"
    index_file = frontend_path / "index.html"

    if index_file.exists():
        return HTMLResponse(content=index_file.read_text(encoding='utf-8'))

    return HTMLResponse(content="""
    <html>
        <head>
            <title>Smart Trash Bin System</title>
        </head>
        <body>
            <h1>Smart Trash Bin System API</h1>
            <p>Welcome to the Smart Trash Bin System backend!</p>
            <ul>
                <li><a href="/api/docs">API Documentation (Swagger)</a></li>
                <li><a href="/api/redoc">API Documentation (ReDoc)</a></li>
            </ul>
            <h2>Available Frontend Pages:</h2>
            <ul>
                <li><a href="/register.html">Student Registration</a></li>
                <li><a href="/checkin.html">Check-in Display</a></li>
                <li><a href="/statistics.html">Statistics & Rankings</a></li>
            </ul>
        </body>
    </html>
    """)


# Serve HTML pages
@app.get("/register.html", response_class=HTMLResponse)
async def serve_register():
    """Serve registration page."""
    frontend_path = Path(__file__).parent.parent.parent / "frontend"
    file_path = frontend_path / "register.html"

    if file_path.exists():
        return HTMLResponse(content=file_path.read_text(encoding='utf-8'))

    return HTMLResponse(content="<h1>Page not found</h1>", status_code=404)


@app.get("/checkin.html", response_class=HTMLResponse)
async def serve_checkin():
    """Serve check-in display page."""
    frontend_path = Path(__file__).parent.parent.parent / "frontend"
    file_path = frontend_path / "checkin.html"

    if file_path.exists():
        return HTMLResponse(content=file_path.read_text(encoding='utf-8'))

    return HTMLResponse(content="<h1>Page not found</h1>", status_code=404)


@app.get("/statistics.html", response_class=HTMLResponse)
async def serve_statistics():
    """Serve statistics page."""
    frontend_path = Path(__file__).parent.parent.parent / "frontend"
    file_path = frontend_path / "statistics.html"

    if file_path.exists():
        return HTMLResponse(content=file_path.read_text(encoding='utf-8'))

    return HTMLResponse(content="<h1>Page not found</h1>", status_code=404)


@app.get("/guide.html", response_class=HTMLResponse)
async def serve_guide():
    """Serve guide page."""
    frontend_path = Path(__file__).parent.parent.parent / "frontend"
    file_path = frontend_path / "guide.html"

    if file_path.exists():
        return HTMLResponse(content=file_path.read_text(encoding='utf-8'))

    return HTMLResponse(content="<h1>Page not found</h1>", status_code=404)


# Health check endpoint
@app.get("/api/health")
async def health_check():
    """Health check endpoint."""
    return {
        "status": "healthy",
        "service": "Smart Trash Bin System",
        "version": "1.0.0"
    }


# Startup event
@app.on_event("startup")
async def startup_event():
    """Run on application startup."""
    print("=" * 60)
    print("Smart Trash Bin System - Backend API")
    print("=" * 60)
    print(f"Server starting on http://{settings.HOST}:{settings.PORT}")
    print(f"API Documentation: http://{settings.HOST}:{settings.PORT}/api/docs")
    print(f"Frontend: http://{settings.HOST}:{settings.PORT}/")
    print("=" * 60)

    # Start session cleanup background task
    session_manager.start_cleanup_task()
    print(f"[Session] Timeout: {settings.SESSION_TIMEOUT_SECONDS}s")
    print(f"[Session] Cleanup interval: {settings.SESSION_CLEANUP_INTERVAL_SECONDS}s")
    print("=" * 60)


# Shutdown event
@app.on_event("shutdown")
async def shutdown_event():
    """Run on application shutdown."""
    print("\nShutting down Smart Trash Bin System...")

    # Stop session cleanup task
    session_manager.stop_cleanup_task()
