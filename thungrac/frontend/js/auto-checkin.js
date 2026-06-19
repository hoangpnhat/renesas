/**
 * Manual Check-in Logic
 * Students click button to check in using face recognition
 */

let video = null;
let canvas = null;
let stream = null;
let isProcessing = false;

// Trash camera variables
let trashVideo = null;
let trashCanvas = null;
let trashStream = null;
let currentCheckinId = null;
const TRASH_CAPTURE_DELAY = 3000; // Wait 3 seconds before capturing trash

// Initialize on page load
document.addEventListener('DOMContentLoaded', async () => {
    video = document.getElementById('video');
    canvas = document.getElementById('canvas');

    // Initialize trash camera elements
    trashVideo = document.getElementById('trash-video');
    trashCanvas = document.getElementById('trash-canvas');

    // Load dashboard stats
    loadDashboardStats();
    setInterval(loadDashboardStats, 30000);

    // Start camera automatically
    await startCamera();

    // Add check-in button event listener
    const checkinButton = document.getElementById('checkin-button');
    checkinButton.addEventListener('click', handleCheckinClick);
});

/**
 * Start camera
 */
async function startCamera() {
    try {
        updateStatus('🎥 Đang khởi động camera...', 'Vui lòng chờ trong giây lát');

        const videoConstraints = {
            width: { ideal: 640 },
            height: { ideal: 480 },
            facingMode: 'user'
        };

        // Use configured camera if available
        if (cameraSettings.checkinCameraId) {
            videoConstraints.deviceId = { exact: cameraSettings.checkinCameraId };
            delete videoConstraints.facingMode; // Remove facingMode when using deviceId
        }

        stream = await navigator.mediaDevices.getUserMedia({
            video: videoConstraints
        });

        video.srcObject = stream;

        // Wait for video to be ready
        await new Promise((resolve) => {
            video.onloadedmetadata = () => {
                canvas.width = video.videoWidth;
                canvas.height = video.videoHeight;
                resolve();
            };
        });

        updateStatus('👋 Xin chào!', 'Nhấn nút Check-in để bắt đầu');

    } catch (error) {
        console.error('Error accessing camera:', error);
        updateStatus('❌ Lỗi camera', 'Không thể truy cập camera. Vui lòng kiểm tra quyền truy cập.');
    }
}

/**
 * Handle check-in button click
 */
async function handleCheckinClick() {
    if (isProcessing) return;

    isProcessing = true;

    // Disable button during processing
    const checkinButton = document.getElementById('checkin-button');
    checkinButton.disabled = true;
    checkinButton.textContent = '⏳ Đang xử lý...';

    showProcessing(true);

    try {
        // Capture frame from video (flip horizontally to match mirrored video)
        const context = canvas.getContext('2d');
        context.save();
        context.scale(-1, 1); // Flip horizontally
        context.drawImage(video, -canvas.width, 0, canvas.width, canvas.height);
        context.restore();

        // Convert to blob
        const blob = await new Promise(resolve => {
            canvas.toBlob(resolve, 'image/jpeg', 0.85);
        });

        // Convert to base64
        const base64 = await blobToBase64(blob);

        // Send to API
        const response = await fetch(`${API_BASE_URL}/esp32/face-checkin`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                image: base64,
                bin_id: 'WEB001',
                timestamp: new Date().toISOString()
            })
        });

        const result = await response.json();

        if (result.status === 'success') {
            // Successful check-in
            handleSuccessfulCheckIn(result);
        } else if (result.status === 'not_registered') {
            // Face not registered - redirect to registration
            handleNotRegistered();
        } else if (result.status === 'error') {
            // Show error message
            updateStatus('❌ Lỗi nhận diện', result.message || 'Không phát hiện khuôn mặt. Vui lòng thử lại.');
        } else {
            // Unknown status
            updateStatus('❌ Lỗi', 'Có lỗi xảy ra. Vui lòng thử lại.');
        }

    } catch (error) {
        console.error('Check-in error:', error);
        updateStatus('❌ Lỗi kết nối', 'Không thể kết nối đến server. Vui lòng kiểm tra kết nối.');
    } finally {
        isProcessing = false;
        showProcessing(false);

        // Re-enable button
        const checkinButton = document.getElementById('checkin-button');
        checkinButton.disabled = false;
        checkinButton.textContent = '📸 Check-in';
    }
}

/**
 * Handle successful check-in
 */
function handleSuccessfulCheckIn(result) {
    // Store checkin_id for trash classification
    currentCheckinId = result.checkin_id;

    // Update status display with clear points info
    updateStatus(
        `🎉 Chào mừng ${result.student_name}!`,
        `Lớp ${result.class || result.class_name || 'N/A'} | 💯 Điểm hiện tại: ${result.total_points} điểm\n🗑️ Chuẩn bị chụp ảnh rác...`
    );

    // Show success color
    const statusDisplay = document.getElementById('status-display');
    statusDisplay.style.background = 'linear-gradient(135deg, #4CAF50, #8BC34A)';

    // Play success sound (optional)
    playSuccessSound();

    // Start trash camera and capture after delay
    startTrashCamera().then(() => {
        setTimeout(() => {
            captureAndClassifyTrash();
        }, TRASH_CAPTURE_DELAY);
    });

    // Refresh stats
    loadDashboardStats();
}

/**
 * Handle face not registered
 */
function handleNotRegistered() {
    updateStatus(
        '⚠️ Chưa đăng ký',
        'Bạn sẽ được chuyển sang trang đăng ký...'
    );

    const statusDisplay = document.getElementById('status-display');
    statusDisplay.style.background = 'linear-gradient(135deg, #FFC107, #FF9800)';

    // Redirect to registration after 2 seconds
    setTimeout(() => {
        window.location.href = '/register.html';
    }, 2000);
}

/**
 * Start trash camera
 */
async function startTrashCamera() {
    try {
        updateStatus('📸 Khởi động camera rác...', 'Vui lòng chờ trong giây lát');

        // Stop trash camera if already running
        if (trashStream) {
            trashStream.getTracks().forEach(track => track.stop());
        }

        let deviceId;

        // Check if user has configured trash camera
        if (cameraSettings.trashCameraId) {
            deviceId = cameraSettings.trashCameraId;
        } else {
            // Auto-select: use second camera if available, otherwise use first
            const devices = await navigator.mediaDevices.enumerateDevices();
            const videoDevices = devices.filter(device => device.kind === 'videoinput');
            deviceId = videoDevices.length > 1 ? videoDevices[1].deviceId : videoDevices[0].deviceId;
        }

        trashStream = await navigator.mediaDevices.getUserMedia({
            video: {
                deviceId: { exact: deviceId },
                width: { ideal: 640 },
                height: { ideal: 480 }
            }
        });

        trashVideo.srcObject = trashStream;

        // Wait for video to be ready
        await new Promise((resolve) => {
            trashVideo.onloadedmetadata = () => {
                trashCanvas.width = trashVideo.videoWidth;
                trashCanvas.height = trashVideo.videoHeight;
                resolve();
            };
        });

        // Show trash camera card
        document.getElementById('trash-camera-card').style.display = 'block';

        updateStatus('🗑️ Hãy bỏ rác vào thùng', 'Hệ thống sẽ tự động chụp ảnh và phân loại');

    } catch (error) {
        console.error('Error starting trash camera:', error);
        updateStatus('❌ Không thể khởi động camera rác', 'Sử dụng camera mặc định');

        // Fallback to using the same camera
        trashVideo.srcObject = stream;
        trashCanvas.width = video.videoWidth;
        trashCanvas.height = video.videoHeight;
        document.getElementById('trash-camera-card').style.display = 'block';
    }
}

/**
 * Capture and classify trash
 */
async function captureAndClassifyTrash() {
    if (!currentCheckinId) {
        console.error('No active checkin session');
        return;
    }

    try {
        // Show processing indicator
        document.getElementById('trash-processing-indicator').classList.add('show');

        updateStatus('🔍 Đang phân loại rác...', 'Vui lòng chờ trong giây lát');

        // Capture frame from trash video
        const context = trashCanvas.getContext('2d');
        context.drawImage(trashVideo, 0, 0, trashCanvas.width, trashCanvas.height);

        // Convert to blob
        const blob = await new Promise(resolve => {
            trashCanvas.toBlob(resolve, 'image/jpeg', 0.85);
        });

        // Convert to base64
        const base64 = await blobToBase64(blob);

        // Send to API for classification
        const response = await fetch(`${API_BASE_URL}/esp32/classify-trash`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                image: base64,
                checkin_id: currentCheckinId,
                bin_id: 'WEB001',
                timestamp: new Date().toISOString()
            })
        });

        const result = await response.json();

        // Hide processing indicator
        document.getElementById('trash-processing-indicator').classList.remove('show');

        // Display result
        displayTrashClassificationResult(result);

        // Send signal to ESP32 to open/close bin
        await sendSignalToESP32(result);

        // Reset after 5 seconds
        setTimeout(() => {
            stopTrashCamera();
            resetToCheckInMode();
        }, 5000);

    } catch (error) {
        console.error('Error classifying trash:', error);
        document.getElementById('trash-processing-indicator').classList.remove('show');
        updateStatus('❌ Lỗi phân loại rác', 'Vui lòng thử lại');

        setTimeout(() => {
            stopTrashCamera();
            resetToCheckInMode();
        }, 3000);
    }
}

/**
 * Display trash classification result
 */
function displayTrashClassificationResult(result) {
    const resultDiv = document.getElementById('trash-result');
    const resultText = document.getElementById('trash-result-text');

    if (result.status === 'success' && result.accepted) {
        resultDiv.style.background = '#E8F5E9';
        resultText.style.color = 'var(--success-color)';
        resultText.textContent = `✅ ${result.message}`;

        if (result.points_awarded > 0) {
            resultText.textContent += ` (+${result.points_awarded} điểm)`;
        }
    } else if (result.status === 'session_expired') {
        resultDiv.style.background = '#FFF3E0';
        resultText.style.color = '#FF9800';
        resultText.textContent = `⏰ ${result.message}`;
    } else {
        resultDiv.style.background = '#FFEBEE';
        resultText.style.color = 'var(--error-color)';
        resultText.textContent = `❌ ${result.message}`;
    }

    resultDiv.style.display = 'block';

    // Update status display
    updateStatus(
        result.accepted ? '✅ Thành công!' : '❌ Rác không được chấp nhận',
        result.message
    );
}

/**
 * Send signal to ESP32 to open/close bin
 */
async function sendSignalToESP32(classificationResult) {
    try {
        const action = classificationResult.accepted ? 'open' : 'keep_closed';

        const response = await fetch(`${API_BASE_URL}/esp32/bin-control`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                action: action,
                bin_id: 'WEB001',
                trash_type: classificationResult.trash_type,
                accepted: classificationResult.accepted
            })
        });

        const result = await response.json();
        console.log('ESP32 signal sent:', result);

    } catch (error) {
        console.error('Error sending signal to ESP32:', error);
    }
}

/**
 * Stop trash camera
 */
function stopTrashCamera() {
    if (trashStream) {
        trashStream.getTracks().forEach(track => track.stop());
        trashStream = null;
    }

    document.getElementById('trash-camera-card').style.display = 'none';
    document.getElementById('trash-result').style.display = 'none';
}

/**
 * Reset to check-in mode
 */
function resetToCheckInMode() {
    currentCheckinId = null;

    updateStatus('👋 Xin chào!', 'Đứng trước camera để check-in');

    const statusDisplay = document.getElementById('status-display');
    statusDisplay.style.background = 'linear-gradient(135deg, var(--primary-color), var(--secondary-color))';
}

/**
 * Update status display
 */
function updateStatus(title, message) {
    document.getElementById('status-title').textContent = title;
    document.getElementById('status-message').textContent = message;
}

/**
 * Show/hide processing indicator
 */
function showProcessing(show) {
    const indicator = document.getElementById('processing-indicator');
    if (show) {
        indicator.classList.add('show');
    } else {
        indicator.classList.remove('show');
    }
}

/**
 * Convert blob to base64
 */
function blobToBase64(blob) {
    return new Promise((resolve, reject) => {
        const reader = new FileReader();
        reader.onloadend = () => {
            // Remove data URL prefix
            const base64 = reader.result.split(',')[1];
            resolve(base64);
        };
        reader.onerror = reject;
        reader.readAsDataURL(blob);
    });
}

/**
 * Play success sound (optional)
 */
function playSuccessSound() {
    try {
        const audio = new Audio('data:audio/wav;base64,UklGRnoGAABXQVZFZm10IBAAAAABAAEAQB8AAEAfAAABAAgAZGF0YQoGAACBhYqFbF1fdJivrJBhNjVgodDbq2EcBj+a2/LDciUFLIHO8tiJNwgZaLvt559NEAxQp+PwtmMcBjiR1/LMeSwFJHfH8N2QQAoUXrTp66hVFApGn+DyvmwhBTGH0fPTgjMGHm7A7+OZUQ0OVK/m77BdGg0=');
        audio.volume = 0.3;
        audio.play().catch(() => {
            // Ignore autoplay errors
        });
    } catch (error) {
        // Ignore sound errors
    }
}

/**
 * Load dashboard stats
 */
async function loadDashboardStats() {
    try {
        const data = await apiFetch('/statistics/dashboard');

        document.getElementById('total-students').textContent =
            data.summary.total_students || 0;

        document.getElementById('checkins-today').textContent =
            data.summary.total_checkins_today || 0;

        document.getElementById('trash-today').textContent =
            data.summary.total_trash_items_today || 0;
    } catch (error) {
        console.error('Error loading dashboard stats:', error);
    }
}

/**
 * DISABLED: These functions are no longer used
 * Trash display now only shows during active check-in session, not old logs
 */

// /**
//  * Check for recent trash and display image
//  */
// let lastTrashTimestamp = null;
//
// async function checkRecentTrash() {
//     try {
//         const data = await apiFetch('/statistics/recent-trash?limit=1');
//
//         if (data.trash_logs && data.trash_logs.length > 0) {
//             const recentTrash = data.trash_logs[0];
//
//             // Only update if it's a new trash log
//             if (recentTrash.timestamp !== lastTrashTimestamp) {
//                 lastTrashTimestamp = recentTrash.timestamp;
//                 displayTrashImage(recentTrash);
//             }
//         }
//     } catch (error) {
//         console.error('Error checking recent trash:', error);
//     }
// }
//
// /**
//  * Display trash image and info
//  */
// function displayTrashImage(trashLog) {
//     const trashDisplay = document.getElementById('trash-display');
//     const trashImage = document.getElementById('trash-image');
//     const trashClassification = document.getElementById('trash-classification');
//     const trashPoints = document.getElementById('trash-points');
//
//     // Set image
//     if (trashLog.image_url) {
//         trashImage.src = API_BASE_URL + trashLog.image_url;
//     } else {
//         trashImage.src = '';
//     }
//
//     // Set classification text
//     const trashType = trashLog.trash_type || 'unknown';
//     const confidence = (trashLog.confidence_score * 100).toFixed(1);
//     const accepted = trashLog.accepted;
//
//     let classText = '';
//     let icon = '';
//
//     // Map 6 TrashNet classes to display text and icons
//     if (trashType === 'cardboard') {
//         icon = '📦';
//         classText = 'Thùng Carton';
//     } else if (trashType === 'glass') {
//         icon = '🍾';
//         classText = 'Thủy Tinh';
//     } else if (trashType === 'metal') {
//         icon = '🔩';
//         classText = 'Kim Loại';
//     } else if (trashType === 'paper') {
//         icon = '📄';
//         classText = 'Giấy';
//     } else if (trashType === 'plastic') {
//         icon = '🧴';
//         classText = 'Nhựa';
//     } else if (trashType === 'trash') {
//         icon = '🗑️';
//         classText = 'Rác Thông Thường';
//     }
//
//     trashClassification.textContent = `${icon} ${classText} (${confidence}%)`;
//     trashClassification.style.color = accepted ? 'var(--success-color)' : 'var(--error-color)';
//
//     // Set points - only show if trash was accepted and has student_id
//     if (accepted && trashLog.student_id) {
//         // Calculate points based on trash type (6 TrashNet classes)
//         const pointsMap = {
//             'cardboard': 5,
//             'glass': 5,
//             'metal': 5,
//             'paper': 5,
//             'plastic': 5,
//             'trash': 3
//         };
//         const points = pointsMap[trashType] || 0;
//         trashPoints.textContent = points > 0 ? `+${points} điểm` : 'Không nhận điểm';
//         trashPoints.style.color = points > 0 ? 'var(--success-color)' : '#999';
//     } else {
//         trashPoints.textContent = accepted ? 'Chưa có học sinh' : 'Bị từ chối';
//         trashPoints.style.color = '#999';
//     }
//
//     // Show trash display
//     trashDisplay.classList.add('show');
//
//     // Auto hide after 10 seconds
//     setTimeout(() => {
//         trashDisplay.classList.remove('show');
//     }, 10000);
// }

/**
 * =============================================================================
 * CAMERA SETTINGS
 * =============================================================================
 */

// Camera settings stored in localStorage
let availableCameras = [];
let cameraSettings = {
    checkinCameraId: null,
    trashCameraId: null
};

/**
 * Load camera settings from localStorage
 */
function loadCameraSettings() {
    const saved = localStorage.getItem('cameraSettings');
    if (saved) {
        cameraSettings = JSON.parse(saved);
    }
}

/**
 * Save camera settings to localStorage
 */
function saveCameraSettingsToStorage() {
    localStorage.setItem('cameraSettings', JSON.stringify(cameraSettings));
}

/**
 * Get list of available cameras
 */
async function getAvailableCameras() {
    try {
        const devices = await navigator.mediaDevices.enumerateDevices();
        availableCameras = devices.filter(device => device.kind === 'videoinput');
        return availableCameras;
    } catch (error) {
        console.error('Error getting cameras:', error);
        return [];
    }
}

/**
 * Open settings modal
 */
async function openSettings() {
    const modal = document.getElementById('settings-modal');
    modal.classList.add('show');

    // Load available cameras
    const cameras = await getAvailableCameras();
    
    // Populate check-in camera select
    const checkinSelect = document.getElementById('checkin-camera-select');
    checkinSelect.innerHTML = '<option value="">Tự động chọn camera đầu tiên</option>';
    cameras.forEach((camera, index) => {
        const option = document.createElement('option');
        option.value = camera.deviceId;
        option.textContent = camera.label || `Camera ${index + 1}`;
        if (camera.deviceId === cameraSettings.checkinCameraId) {
            option.selected = true;
        }
        checkinSelect.appendChild(option);
    });

    // Populate trash camera select
    const trashSelect = document.getElementById('trash-camera-select');
    trashSelect.innerHTML = '<option value="">Tự động chọn camera thứ 2</option>';
    cameras.forEach((camera, index) => {
        const option = document.createElement('option');
        option.value = camera.deviceId;
        option.textContent = camera.label || `Camera ${index + 1}`;
        if (camera.deviceId === cameraSettings.trashCameraId) {
            option.selected = true;
        }
        trashSelect.appendChild(option);
    });
}

/**
 * Close settings modal
 */
function closeSettings() {
    const modal = document.getElementById('settings-modal');
    modal.classList.remove('show');
}

/**
 * Save camera settings
 */
async function saveSettings() {
    const checkinCameraId = document.getElementById('checkin-camera-select').value;
    const trashCameraId = document.getElementById('trash-camera-select').value;

    cameraSettings.checkinCameraId = checkinCameraId || null;
    cameraSettings.trashCameraId = trashCameraId || null;

    saveCameraSettingsToStorage();

    // Restart cameras with new settings
    if (stream) {
        stream.getTracks().forEach(track => track.stop());
    }
    await startCamera();

    closeSettings();
    
    updateStatus('✅ Đã lưu cài đặt', 'Camera đã được cấu hình lại');
    setTimeout(() => {
        updateStatus('👋 Xin chào!', 'Nhấn nút Check-in để bắt đầu');
    }, 2000);
}

// Load settings on page load
loadCameraSettings();

