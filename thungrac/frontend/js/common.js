/**
 * Common utilities and shared functions
 */

// API Base URL - Update this with your server address
const API_BASE_URL = window.location.origin + '/api';

/**
 * Fetch wrapper with error handling
 */
async function apiFetch(endpoint, options = {}) {
    try {
        const url = `${API_BASE_URL}${endpoint}`;
        const response = await fetch(url, {
            ...options,
            headers: {
                'Accept': 'application/json',
                ...options.headers
            }
        });

        if (!response.ok) {
            const error = await response.json();
            throw new Error(error.detail || `HTTP error! status: ${response.status}`);
        }

        return await response.json();
    } catch (error) {
        console.error('API Error:', error);
        throw error;
    }
}

/**
 * Format date/time
 */
function formatDateTime(timestamp) {
    const date = new Date(timestamp);
    const options = {
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit',
        hour12: false
    };
    return new Intl.DateTimeFormat('vi-VN', options).format(date);
}

/**
 * Format date only
 */
function formatDate(timestamp) {
    const date = new Date(timestamp);
    return date.toLocaleDateString('vi-VN');
}

/**
 * Show success message
 */
function showSuccess(message, containerId = 'message-container') {
    const container = document.getElementById(containerId);
    if (!container) return;

    container.innerHTML = `
        <div class="alert alert-success show">
            ${message}
        </div>
    `;

    setTimeout(() => {
        const alert = container.querySelector('.alert');
        if (alert) alert.classList.remove('show');
    }, 5000);
}

/**
 * Show error message
 */
function showError(message, containerId = 'message-container') {
    const container = document.getElementById(containerId);
    if (!container) return;

    container.innerHTML = `
        <div class="alert alert-error show">
            ${message}
        </div>
    `;

    setTimeout(() => {
        const alert = container.querySelector('.alert');
        if (alert) alert.classList.remove('show');
    }, 5000);
}

/**
 * Show info message
 */
function showInfo(message, containerId = 'message-container') {
    const container = document.getElementById(containerId);
    if (!container) return;

    container.innerHTML = `
        <div class="alert alert-info show">
            ${message}
        </div>
    `;

    setTimeout(() => {
        const alert = container.querySelector('.alert');
        if (alert) alert.classList.remove('show');
    }, 5000);
}

/**
 * Show loading spinner
 */
function showLoading(containerId) {
    const container = document.getElementById(containerId);
    if (!container) return;

    container.innerHTML = '<div class="loading"></div>';
}

/**
 * Hide loading spinner
 */
function hideLoading(containerId) {
    const container = document.getElementById(containerId);
    if (!container) return;

    container.innerHTML = '';
}

/**
 * Get rank badge HTML
 */
function getRankBadge(rank) {
    const badgeClass = rank <= 3 ? `rank-${rank}` : 'rank-default';
    return `<span class="rank-badge ${badgeClass}">${rank}</span>`;
}

/**
 * Convert file to base64
 */
function fileToBase64(file) {
    return new Promise((resolve, reject) => {
        const reader = new FileReader();
        reader.onload = () => resolve(reader.result.split(',')[1]);
        reader.onerror = reject;
        reader.readAsDataURL(file);
    });
}

/**
 * Highlight active navigation
 */
function highlightActiveNav() {
    const currentPage = window.location.pathname.split('/').pop() || 'index.html';
    const navLinks = document.querySelectorAll('.nav a');

    navLinks.forEach(link => {
        if (link.getAttribute('href') === currentPage) {
            link.classList.add('active');
        }
    });
}

// Run on page load
document.addEventListener('DOMContentLoaded', highlightActiveNav);
