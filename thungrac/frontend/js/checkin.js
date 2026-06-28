/**
 * Check-in Display Page Logic
 */

let previousCheckins = [];

// Initialize on page load
document.addEventListener('DOMContentLoaded', () => {
    fetchRecentCheckins();

    // Auto-refresh every 5 seconds
    setInterval(fetchRecentCheckins, 5000);
});

/**
 * Fetch recent check-ins
 */
async function fetchRecentCheckins() {
    try {
        const data = await apiFetch('/statistics/recent-checkins?limit=50');

        updateUpdateIndicator();

        if (!data || data.length === 0) {
            showEmptyState();
            return;
        }

        hideEmptyState();
        updateCheckinDisplay(data);

    } catch (error) {
        console.error('Error fetching check-ins:', error);
        updateUpdateIndicator('Lỗi kết nối');
    }
}

/**
 * Update check-in display
 */
function updateCheckinDisplay(checkins) {
    const container = document.getElementById('checkin-list');
    const loading = document.getElementById('loading');

    // Hide loading indicator
    if (loading) {
        loading.style.display = 'none';
    }

    // Check for new check-ins
    const newCheckins = checkins.filter(checkin =>
        !previousCheckins.some(prev => prev.checkin_id === checkin.checkin_id)
    );

    // If there are new check-ins, prepend them with animation
    if (newCheckins.length > 0 && previousCheckins.length > 0) {
        newCheckins.reverse().forEach(checkin => {
            const element = createCheckinElement(checkin);
            element.style.backgroundColor = '#E8F5E9'; // Highlight new items
            container.insertBefore(element, container.firstChild);

            // Remove highlight after 3 seconds
            setTimeout(() => {
                element.style.backgroundColor = '';
            }, 3000);
        });
    } else {
        // Full refresh
        container.innerHTML = '';
        checkins.forEach(checkin => {
            container.appendChild(createCheckinElement(checkin));
        });
    }

    previousCheckins = checkins;
}

/**
 * Create check-in element
 */
function createCheckinElement(checkin) {
    const div = document.createElement('div');
    div.className = 'checkin-item';
    const studentName = checkin.student_name || checkin.student_id || 'Không rõ';
    const className = checkin['class'] || checkin.class_name || 'N/A';
    div.innerHTML = `
        <h3>${studentName}</h3>
        <p><strong>Lớp:</strong> ${className}</p>
        <p><strong>Thời gian:</strong> ${formatDateTime(checkin.timestamp)}</p>
        <p class="points">+${checkin.points_awarded} điểm</p>
        <p style="font-size: 0.85rem; color: var(--text-light);">
            Độ tin cậy: ${(checkin.confidence_score * 100).toFixed(1)}%
        </p>
    `;
    return div;
}

/**
 * Update the "last updated" indicator
 */
function updateUpdateIndicator(message = null) {
    const indicator = document.getElementById('update-indicator');
    if (!indicator) return;

    if (message) {
        indicator.textContent = message;
        indicator.style.color = 'var(--error-color)';
    } else {
        const now = new Date();
        indicator.textContent = `Cập nhật lúc: ${now.toLocaleTimeString('vi-VN')}`;
        indicator.style.color = 'var(--text-light)';
    }
}

/**
 * Show empty state
 */
function showEmptyState() {
    document.getElementById('loading').style.display = 'none';
    document.getElementById('checkin-list').style.display = 'none';
    document.getElementById('empty-state').style.display = 'block';
}

/**
 * Hide empty state
 */
function hideEmptyState() {
    document.getElementById('empty-state').style.display = 'none';
    document.getElementById('checkin-list').style.display = 'grid';
}
