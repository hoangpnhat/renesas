/**
 * Statistics Page Logic
 */

// Initialize on page load
document.addEventListener('DOMContentLoaded', () => {
    initializeTabs();
    loadDashboard();
    loadStudentRankings();
    loadClassRankings();
    loadTrashStats();

    // Add class filter event listener
    document.getElementById('class-filter').addEventListener('change', (e) => {
        loadStudentRankings(e.target.value);
    });
});

/**
 * Initialize tab switching
 */
function initializeTabs() {
    const tabs = document.querySelectorAll('.tab');
    const tabContents = document.querySelectorAll('.tab-content');

    tabs.forEach(tab => {
        tab.addEventListener('click', () => {
            // Remove active class from all tabs and contents
            tabs.forEach(t => t.classList.remove('active'));
            tabContents.forEach(c => c.classList.remove('active'));

            // Add active class to clicked tab
            tab.classList.add('active');

            // Show corresponding content
            const tabId = tab.getAttribute('data-tab');
            document.getElementById(tabId).classList.add('active');
        });
    });
}

/**
 * Load dashboard summary
 */
async function loadDashboard() {
    try {
        const data = await apiFetch('/statistics/dashboard');

        document.getElementById('dash-total-students').textContent =
            data.summary.total_students || 0;

        document.getElementById('dash-checkins-today').textContent =
            data.summary.total_checkins_today || 0;

        if (data.summary.top_student) {
            document.getElementById('dash-top-student').innerHTML =
                `${data.summary.top_student.name}<br>
                <span style="font-size: 0.8rem;">${data.summary.top_student.points} điểm</span>`;
        }

        if (data.summary.top_class) {
            document.getElementById('dash-top-class').innerHTML =
                `${data.summary.top_class.name}<br>
                <span style="font-size: 0.8rem;">${data.summary.top_class.points} điểm</span>`;
        }

    } catch (error) {
        console.error('Error loading dashboard:', error);
    }
}

/**
 * Load student rankings
 */
async function loadStudentRankings(classFilter = '') {
    try {
        let endpoint = '/statistics/student-rankings?limit=50';
        if (classFilter) {
            endpoint += `&class=${encodeURIComponent(classFilter)}`;
        }

        const data = await apiFetch(endpoint);
        const tbody = document.getElementById('student-rankings-body');

        if (!data || data.length === 0) {
            tbody.innerHTML = '<tr><td colspan="5" style="text-align: center; color: var(--text-light);">Chưa có dữ liệu</td></tr>';
            return;
        }

        tbody.innerHTML = data.map(student => `
            <tr>
                <td>${getRankBadge(student.rank)}</td>
                <td>${student.student_id}</td>
                <td><strong>${student.name}</strong></td>
                <td>${student.class}</td>
                <td><strong>${student.total_points}</strong> điểm</td>
            </tr>
        `).join('');

    } catch (error) {
        console.error('Error loading student rankings:', error);
        document.getElementById('student-rankings-body').innerHTML =
            '<tr><td colspan="5" style="text-align: center; color: var(--error-color);">Lỗi tải dữ liệu</td></tr>';
    }
}

/**
 * Load class rankings
 */
async function loadClassRankings() {
    try {
        const data = await apiFetch('/statistics/class-rankings');
        const tbody = document.getElementById('class-rankings-body');

        if (!data || data.length === 0) {
            tbody.innerHTML = '<tr><td colspan="5" style="text-align: center; color: var(--text-light);">Chưa có dữ liệu</td></tr>';
            return;
        }

        tbody.innerHTML = data.map(cls => `
            <tr>
                <td>${getRankBadge(cls.rank)}</td>
                <td><strong>${cls.class}</strong></td>
                <td><strong>${cls.total_points}</strong> điểm</td>
                <td>${cls.student_count} học sinh</td>
                <td>${cls.avg_points_per_student.toFixed(1)} điểm</td>
            </tr>
        `).join('');

    } catch (error) {
        console.error('Error loading class rankings:', error);
        document.getElementById('class-rankings-body').innerHTML =
            '<tr><td colspan="5" style="text-align: center; color: var(--error-color);">Lỗi tải dữ liệu</td></tr>';
    }
}

/**
 * Load trash statistics
 */
async function loadTrashStats() {
    try {
        const data = await apiFetch('/statistics/trash-breakdown');

        // Update summary
        document.getElementById('total-trash').textContent = data.total_items || 0;
        document.getElementById('acceptance-rate').textContent =
            data.acceptance_rate ? `${data.acceptance_rate}%` : '--';

        // Draw pie chart
        if (data.breakdown && Object.keys(data.breakdown).length > 0) {
            drawPieChart(data.breakdown, data.percentages);

            // Show breakdown table
            const breakdownDiv = document.getElementById('trash-breakdown');
            const trashTypes = {
                'cardboard': 'Thùng Carton',
                'glass': 'Thủy Tinh',
                'metal': 'Kim Loại',
                'paper': 'Giấy',
                'plastic': 'Nhựa',
                'trash': 'Rác Thông Thường'
            };

            breakdownDiv.innerHTML = `
                <table>
                    <thead>
                        <tr>
                            <th>Loại Rác</th>
                            <th>Số Lượng</th>
                            <th>Tỷ Lệ</th>
                        </tr>
                    </thead>
                    <tbody>
                        ${Object.entries(data.breakdown).map(([type, count]) => `
                            <tr>
                                <td><strong>${trashTypes[type] || type}</strong></td>
                                <td>${count}</td>
                                <td>${data.percentages[type].toFixed(1)}%</td>
                            </tr>
                        `).join('')}
                    </tbody>
                </table>
            `;
        }

    } catch (error) {
        console.error('Error loading trash stats:', error);
    }
}

/**
 * Draw pie chart using Canvas API
 */
function drawPieChart(breakdown, percentages) {
    const canvas = document.getElementById('trash-chart');
    const ctx = canvas.getContext('2d');

    // Set canvas size
    canvas.width = 400;
    canvas.height = 400;

    const centerX = canvas.width / 2;
    const centerY = canvas.height / 2;
    const radius = Math.min(centerX, centerY) - 40;

    const colors = {
        'cardboard': '#8D6E63',  // Brown for cardboard
        'glass': '#4FC3F7',      // Light blue for glass
        'metal': '#9E9E9E',      // Grey for metal
        'paper': '#FFE082',      // Light yellow for paper
        'plastic': '#FF7043',    // Orange for plastic
        'trash': '#757575'       // Dark grey for general trash
    };

    const labels = {
        'cardboard': 'Thùng Carton',
        'glass': 'Thủy Tinh',
        'metal': 'Kim Loại',
        'paper': 'Giấy',
        'plastic': 'Nhựa',
        'trash': 'Rác Chung'
    };

    let currentAngle = -Math.PI / 2; // Start from top

    // Draw slices
    Object.entries(breakdown).forEach(([type, count]) => {
        const sliceAngle = (percentages[type] / 100) * 2 * Math.PI;

        // Draw slice
        ctx.beginPath();
        ctx.arc(centerX, centerY, radius, currentAngle, currentAngle + sliceAngle);
        ctx.lineTo(centerX, centerY);
        ctx.fillStyle = colors[type] || '#999';
        ctx.fill();

        // Draw label
        const labelAngle = currentAngle + sliceAngle / 2;
        const labelX = centerX + Math.cos(labelAngle) * (radius * 0.7);
        const labelY = centerY + Math.sin(labelAngle) * (radius * 0.7);

        ctx.fillStyle = '#FFF';
        ctx.font = 'bold 14px Arial';
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(`${percentages[type].toFixed(1)}%`, labelX, labelY);

        currentAngle += sliceAngle;
    });

    // Draw legend
    let legendY = 10;
    Object.entries(breakdown).forEach(([type, count]) => {
        ctx.fillStyle = colors[type] || '#999';
        ctx.fillRect(10, legendY, 20, 20);

        ctx.fillStyle = '#333';
        ctx.font = '14px Arial';
        ctx.textAlign = 'left';
        ctx.fillText(`${labels[type]}: ${count}`, 35, legendY + 15);

        legendY += 30;
    });
}
