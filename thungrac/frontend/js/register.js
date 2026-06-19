/**
 * Student Registration Page Logic
 */

let video = null;
let canvas = null;
let stream = null;
let capturedImageBlob = null;

// Initialize elements
document.addEventListener('DOMContentLoaded', () => {
    video = document.getElementById('video');
    canvas = document.getElementById('canvas');

    const startCameraBtn = document.getElementById('start-camera');
    const captureBtn = document.getElementById('capture-btn');
    const retakeBtn = document.getElementById('retake-btn');
    const form = document.getElementById('registration-form');

    startCameraBtn.addEventListener('click', startCamera);
    captureBtn.addEventListener('click', capturePhoto);
    retakeBtn.addEventListener('click', retakePhoto);
    form.addEventListener('submit', handleSubmit);
});

/**
 * Start camera
 */
async function startCamera() {
    try {
        stream = await navigator.mediaDevices.getUserMedia({
            video: {
                width: { ideal: 640 },
                height: { ideal: 480 },
                facingMode: 'user'
            }
        });

        video.srcObject = stream;

        // Show video container and controls
        document.getElementById('video-container').style.display = 'block';
        document.getElementById('camera-actions').style.display = 'block';
        document.getElementById('camera-controls').style.display = 'none';

    } catch (error) {
        console.error('Error accessing camera:', error);
        showError('Không thể truy cập camera. Vui lòng kiểm tra quyền truy cập.');
    }
}

/**
 * Capture photo from video stream
 */
function capturePhoto() {
    // Set canvas size to match video
    canvas.width = video.videoWidth;
    canvas.height = video.videoHeight;

    // Draw video frame to canvas (flip horizontally to match mirrored video)
    const context = canvas.getContext('2d');
    context.save();
    context.scale(-1, 1); // Flip horizontally
    context.drawImage(video, -canvas.width, 0, canvas.width, canvas.height);
    context.restore();

    // Convert canvas to blob
    canvas.toBlob((blob) => {
        capturedImageBlob = blob;

        // Show preview
        const preview = document.getElementById('preview');
        preview.src = URL.createObjectURL(blob);
        document.getElementById('preview-container').style.display = 'block';

        // Hide video, show retake button
        document.getElementById('video-container').style.display = 'none';
        document.getElementById('capture-btn').style.display = 'none';
        document.getElementById('retake-btn').style.display = 'inline-block';

        // Enable submit button
        document.getElementById('submit-btn').disabled = false;

        // Stop camera stream
        if (stream) {
            stream.getTracks().forEach(track => track.stop());
        }

    }, 'image/jpeg', 0.95);
}

/**
 * Retake photo
 */
async function retakePhoto() {
    capturedImageBlob = null;

    // Hide preview
    document.getElementById('preview-container').style.display = 'none';

    // Show video again
    document.getElementById('capture-btn').style.display = 'inline-block';
    document.getElementById('retake-btn').style.display = 'none';

    // Disable submit button
    document.getElementById('submit-btn').disabled = true;

    // Restart camera
    await startCamera();
}

/**
 * Handle form submission
 */
async function handleSubmit(event) {
    event.preventDefault();

    const name = document.getElementById('name').value.trim();
    const className = document.getElementById('class').value;

    if (!name || !className) {
        showError('Vui lòng điền đầy đủ thông tin!');
        return;
    }

    if (!capturedImageBlob) {
        showError('Vui lòng chụp ảnh khuôn mặt!');
        return;
    }

    // Disable submit button
    const submitBtn = document.getElementById('submit-btn');
    submitBtn.disabled = true;
    submitBtn.textContent = 'Đang đăng ký...';

    try {
        // Create form data
        const formData = new FormData();
        formData.append('name', name);
        formData.append('class', className);
        formData.append('face_image', capturedImageBlob, 'face.jpg');

        // Send to API
        const response = await fetch(`${API_BASE_URL}/students/register`, {
            method: 'POST',
            body: formData
        });

        if (!response.ok) {
            const error = await response.json();
            throw new Error(error.detail || 'Đăng ký thất bại');
        }

        const result = await response.json();

        // Show success message
        showSuccess(`
            Đăng ký thành công!<br>
            Mã học sinh: <strong>${result.student_id}</strong><br>
            Bạn có thể bắt đầu sử dụng hệ thống ngay bây giờ!
        `);

        // Reset form after 3 seconds
        setTimeout(() => {
            location.reload();
        }, 3000);

    } catch (error) {
        console.error('Registration error:', error);
        showError(`Đăng ký thất bại: ${error.message}`);

        // Re-enable submit button
        submitBtn.disabled = false;
        submitBtn.textContent = 'Đăng Ký';
    }
}
