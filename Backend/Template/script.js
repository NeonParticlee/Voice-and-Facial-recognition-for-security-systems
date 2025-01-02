// Timer and Recording Control with Voice/Face Recognition
document.addEventListener("DOMContentLoaded", () => {
    // DOM Elements
    const timerBox = document.getElementById('timer-box');
    const timerDisplay = document.getElementById('timer');
    const recordBtn = document.getElementById('record-btn');
    const stopBtn = document.getElementById('stop-btn');
    const status1 = document.getElementById('status-1');
    const captureBtn = document.getElementById('capture-btn');
    const camera = document.getElementById('camera');
    const canvas = document.getElementById('canvas');
    const status2 = document.getElementById('status-2');
    const nameInput = document.getElementById('name-input');
    
    // Variables
    let timerInterval;
    let timeLeft;
    let mediaRecorder;
    let audioChunks = [];

    // Initially hide the timer and stop button
    timerBox.style.display = 'none';
    stopBtn.style.display = 'none';

    // Helper function to validate name
    function isNameValid(name) {
        const namePattern = /^[A-Za-z\s]+$/;
        const words = name.trim().split(/\s+/);
        return (
            name.length >= 7 && 
            name.length <= 25 && 
            namePattern.test(name) && 
            words.length >= 2
        );
    }

    // Function to get timestamp
    function getTimestamp() {
        return new Date().toISOString().replace(/[:.]/g, '-');
    }

    // Function to save file
    async function saveFile(blob, fileName) {
        const a = document.createElement('a');
        a.href = URL.createObjectURL(blob);
        a.download = fileName;
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(a.href);
    }

    // Timer Functions
    function startTimer() {
        timeLeft = 10;
        timerBox.style.display = 'flex';
        timerBox.style.animation = 'fadeIn 0.3s ease-in';
        
        timerInterval = setInterval(() => {
            timeLeft--;
            timerDisplay.textContent = timeLeft;
            
            if (timeLeft <= 3) {
                timerDisplay.style.color = '#f64747';
            }
            
            if (timeLeft <= 0) {
                mediaRecorder.stop();
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

    // Voice Recording Functions
    recordBtn.addEventListener("click", async () => {
        if (!navigator.mediaDevices || !navigator.mediaDevices.getUserMedia) {
            alert("Your browser does not support audio recording.");
            return;
        }

        try {
            const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
            
            mediaRecorder = new MediaRecorder(stream);
            mediaRecorder.onstart = () => {
                audioChunks = [];
                recordBtn.disabled = true;
                stopBtn.style.display = 'inline-block';
                status1.textContent = 'Recording in progress...';
                startTimer();
            };

            mediaRecorder.ondataavailable = (event) => {
                audioChunks.push(event.data);
            };

            mediaRecorder.onstop = async () => {
                stopTimer();
                recordBtn.disabled = false;
                stopBtn.style.display = 'none';
                status1.textContent = 'Recording stopped. Saving...';
                
                const audioBlob = new Blob(audioChunks, { type: 'audio/webm' });
                const fileName = `recording_${getTimestamp()}.webm`;
                
                try {
                    await saveFile(audioBlob, fileName);
                    status1.textContent = 'Recording saved successfully!';
                } catch (error) {
                    console.error("Error saving audio:", error);
                    status1.textContent = 'Error saving recording.';
                }

                stream.getTracks().forEach(track => track.stop());
            };

            mediaRecorder.start();
        } catch (error) {
            console.error("Error accessing microphone:", error);
            alert("Failed to access the microphone.");
        }
    });

    stopBtn.addEventListener("click", () => {
        if (mediaRecorder && mediaRecorder.state === "recording") {
            mediaRecorder.stop();
        }
    });

    // Face Capture Function
    captureBtn.addEventListener("click", async () => {
        if (!nameInput.value.trim()) {
            alert("Please enter your name first.");
            return;
        }

        const name = nameInput.value.trim();
        if (!isNameValid(name)) {
            alert(
                "Invalid name. Please enter a valid name:\n" +
                "- At least 7 characters long\n" +
                "- Maximum 25 characters\n" +
                "- Contain only letters and spaces\n" +
                "- Include at least a first and last name"
            );
            return;
        }

        try {
            const stream = await navigator.mediaDevices.getUserMedia({ video: true });
            camera.srcObject = stream;
            camera.style.display = 'block';
            canvas.style.display = 'none';

            let captureCount = 0;
            let timeLeft = 20;

            const captureInterval = setInterval(() => {
                captureCount++;
                canvas.width = camera.videoWidth;
                canvas.height = camera.videoHeight;
                const context = canvas.getContext('2d');
                context.drawImage(camera, 0, 0, canvas.width, canvas.height);

                canvas.toBlob(async (blob) => {
                    const fileName = `${name.replace(/\s+/g, '_')}_capture_${captureCount}_${getTimestamp()}.jpg`;
                    await saveFile(blob, fileName);
                    status2.textContent = `Saved image ${captureCount}`;
                }, 'image/jpeg');

                timeLeft -= 0.5;
                if (timeLeft <= 0) {
                    clearInterval(captureInterval);
                    stream.getTracks().forEach(track => track.stop());
                    camera.style.display = 'none';
                    status2.textContent = `Captured and saved ${captureCount} images!`;
                }
            }, 500);
        } catch (error) {
            console.error("Error accessing camera:", error);
            alert("Unable to access the camera.");
        }
    });

    // Sliding Functionality
    const boxes = document.querySelectorAll(".recognition-box");
    const totalBoxes = boxes.length;
    let currentIndex = 0;
    const previousButton = document.getElementById('previous-btn');
    const nextButton = document.getElementById('next-btn');

    function showBox(index) {
        boxes.forEach((box, i) => {
            box.style.display = i === index ? 'block' : 'none';
        });
        updateButtonStates();
    }

    function updateButtonStates() {
        previousButton.disabled = currentIndex === 0;
        nextButton.disabled = currentIndex === totalBoxes - 1;
    }

    previousButton.addEventListener('click', () => {
        if (currentIndex > 0) {
            currentIndex--;
            showBox(currentIndex);
        }
    });

    nextButton.addEventListener('click', () => {
        if (currentIndex < totalBoxes - 1) {
            currentIndex++;
            showBox(currentIndex);
        }
    });

    // Initialize display
    showBox(currentIndex);
});