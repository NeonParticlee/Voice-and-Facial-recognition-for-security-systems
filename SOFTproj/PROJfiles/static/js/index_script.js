// DOM Elements
const captureBtn = document.getElementById('capture-btn');
const recordBtn = document.getElementById('record-btn');
const stopBtn = document.getElementById('stop-btn');
const nameInput = document.getElementById('name-input');
const camera = document.getElementById('camera');
const canvas = document.getElementById('canvas');
const status1 = document.getElementById('status-1');
const status2 = document.getElementById('status-2');
const timerBox = document.getElementById('timer-box');
const timerDisplay = document.getElementById('timer');

// Variables
let timerInterval;
let mediaRecorder;
let audioChunks = [];

// Initially hide elements
timerBox.style.display = 'none';
stopBtn.style.display = 'none';

// Timer Functions
function startTimer() {
    let timeLeft = 10;
    timerBox.style.display = 'flex';
    timerBox.style.animation = 'fadeIn 0.3s ease-in';

    timerInterval = setInterval(() => {
        timeLeft--;
        timerDisplay.textContent = timeLeft;

        if (timeLeft <= 3) {
            timerDisplay.style.color = '#f64747';
        }

        if (timeLeft <= 0) {
            if (mediaRecorder && mediaRecorder.state === "recording") {
                mediaRecorder.stop();
            }
        }
    }, 1000);
}

function stopTimer() {
    clearInterval(timerInterval);
    timerBox.style.animation = 'fadeOut 0.3s ease-out';
    setTimeout(() => {
        timerBox.style.display = 'none';
        timerDisplay.style.color = 'white';
        timerDisplay.textContent = '10';
    }, 300);
}

// Validation Function
function isNameValid(name) {
    const words = name.trim().split(/\s+/);
    return name.length >= 7 && name.length <= 25 && words.length >= 2;
}


const saveUserFile = async (blob, username, fileType, index = '') => {
    const formData = new FormData();
    const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
    const fileName = `${fileType}_${timestamp}${index}.${fileType === 'photo' ? 'jpg' : 'webm'}`;

    formData.append('file', blob, fileName);  // Changed 'file' to 'audio' to match backend
    formData.append('username', username.toLowerCase().replace(/\s+/g, '_'));
    formData.append('fileType', fileType);

    try {
        const response = await fetch('/api/save-voice/', {
            method: 'POST',
            body: formData
        });

        if (!response.ok) {
            throw new Error('Network response was not ok');
        }

        const data = await response.json();
        return data.success;
    } catch (error) {
        console.error('Error saving file:', error);
        return false;
    }
};

// Face Capture Functionality
captureBtn.addEventListener("click", async () => {
    const name = nameInput.value.trim();
    if (!isNameValid(name)) {
        alert("Please enter a valid name (first and last name, 7-25 characters)");
        return;
    }

    try {
        const stream = await navigator.mediaDevices.getUserMedia({ video: true });
        camera.srcObject = stream;
        camera.style.display = 'block';
        canvas.style.display = 'none';

        // Wait for video to be ready
        await new Promise((resolve) => {
            camera.onloadedmetadata = () => {
                camera.play();
                resolve();
            };
        });

        let captureCount = 0;
        status2.textContent = 'Starting capture session...';

        const captureImage = async () => {
            if (captureCount >= 40) {
                stream.getTracks().forEach(track => track.stop());
                camera.style.display = 'none';
                status2.textContent = 'Photo capture completed!';
                return;
            }

            try {
                canvas.width = camera.videoWidth;
                canvas.height = camera.videoHeight;
                const context = canvas.getContext('2d');
                context.drawImage(camera, 0, 0, canvas.width, canvas.height);

                // Convert canvas to blob
                const blob = await new Promise(resolve => canvas.toBlob(resolve, 'image/jpeg', 0.8));
                
                // Create filename with timestamp
                const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
                const fileName = `face_${timestamp}_${captureCount + 1}.jpg`;

                // Create FormData and append file
                const formData = new FormData();
                formData.append('file', blob, fileName);
                
                const response = await fetch('/api/save-face/', {
                    method: 'POST',
                    body: formData
                });

                if (!response.ok) {
                    throw new Error('Failed to save image');
                }

                captureCount++;
                status2.textContent = `Captured photo ${captureCount}/40`;
                
                // Wait before next capture
                await new Promise(resolve => setTimeout(resolve, 500));
                
                // Continue capturing
                captureImage();

            } catch (error) {
                console.error("Error capturing/saving image:", error);
                status2.textContent = `Error: ${error.message}`;
                
                // Retry after error
                await new Promise(resolve => setTimeout(resolve, 1000));
                captureImage();
            }
        };

        // Start the capture process
        await captureImage();

    } catch (error) {
        console.error("Camera error:", error);
        status2.textContent = 'Error: Could not access camera';
        alert("Camera access failed. Please check permissions.");
    }
});


// Voice Recording Functionality
recordBtn.addEventListener("click", async () => {
    const name = nameInput.value.trim();
    if (!isNameValid(name)) {
        alert("Please enter a valid name first");
        return;
    }

    try {
        const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
        mediaRecorder = new MediaRecorder(stream);
        audioChunks = [];

        mediaRecorder.onstart = () => {
            audioChunks = [];
            recordBtn.disabled = true;
            stopBtn.style.display = 'inline-block';
            status1.textContent = 'Recording voice...';
            startTimer();
        };

        mediaRecorder.ondataavailable = (event) => {
            audioChunks.push(event.data);
        };

        mediaRecorder.onstop = async () => {
            stopTimer();
            recordBtn.disabled = false;
            stopBtn.style.display = 'none';
            status1.textContent = 'Processing recording...';

            const audioBlob = new Blob(audioChunks, { type: 'audio/webm' });
            const saved = await saveUserFile(audioBlob, name, 'voice');

            status1.textContent = saved ?
                'Voice recording saved successfully!' :
                'Error saving recording';

            stream.getTracks().forEach(track => track.stop());
        };

        mediaRecorder.start();
    } catch (error) {
        console.error("Microphone error:", error);
        status1.textContent = 'Error: Could not access microphone';
        alert("Microphone access failed. Please check permissions.");
    }
});

// Stop Recording Button
stopBtn.addEventListener("click", () => {
    if (mediaRecorder && mediaRecorder.state === "recording") {
        mediaRecorder.stop();
    }
});

// Navigation Controls
const boxes = document.querySelectorAll(".recognition-box");
const previousBtn = document.getElementById('previous-btn');
const nextBtn = document.getElementById('next-btn');
let currentIndex = 0;

function showBox(index) {
    boxes.forEach((box, i) => {
        box.style.display = i === index ? 'block' : 'none';
    });
    previousBtn.disabled = currentIndex === 0;
    nextBtn.disabled = currentIndex === boxes.length - 1;
}

previousBtn.addEventListener('click', () => {
    if (currentIndex > 0) {
        currentIndex--;
        showBox(currentIndex);
    }
});

nextBtn.addEventListener('click', () => {
    if (currentIndex < boxes.length - 1) {
        currentIndex++;
        showBox(currentIndex);
    }
});

// Initialize display
showBox(currentIndex);