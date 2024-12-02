document.addEventListener("DOMContentLoaded", () => {
    // Voice Recognition Variables
    const recordBtn = document.getElementById("record-btn");
    const captureBtn = document.getElementById("capture-btn");
    const camera = document.getElementById("camera");
    const canvas = document.getElementById("canvas");
    const status_1 = document.getElementById("status_1");
    const status_2 = document.getElementById("status_2");

    let mediaRecorder;
    let audioChunks = [];

    // Voice Recognition Functionality
    /*recordBtn.addEventListener("click", async () => {
        if (!navigator.mediaDevices || !navigator.mediaDevices.getUserMedia) {
            alert("Your browser does not support audio recording.");
            return;
        }

        if (!mediaRecorder || mediaRecorder.state === "inactive") {
            const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
            mediaRecorder = new MediaRecorder(stream);

            mediaRecorder.onstart = () => {
                audioChunks = [];
                recordBtn.textContent = "Stop Recording";
                status_1.textContent = "Recording... Speak into your microphone.";
            };

            mediaRecorder.ondataavailable = (event) => {
                audioChunks.push(event.data);
            };

            mediaRecorder.onstop = async () => {
                recordBtn.textContent = "Start Recording";
                status_1.textContent = "Recording stopped. Sending audio to the server...";

                const audioBlob = new Blob(audioChunks, { type: "audio/webm" });
                const audioSent = await sendToServer(audioBlob, "audio");

                if (audioSent) {
                    alert("Audio sent successfully!");
                } else {
                    status_1.textContent = "Error occurred while transferring audio.";
                    alert("WARNING! ERROR");
                }
            };

            mediaRecorder.start();
        } else {
            mediaRecorder.stop();
        }
    });*/

    /*recordBtn.addEventListener("click", async () => {
        try {
            status_1.textContent = "Initializing microphone... Please wait.";
            recordBtn.disabled = true; // Disable the button temporarily
    
            const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
    
            // Initialize the MediaRecorder
            if (!mediaRecorder || mediaRecorder.state === "inactive") {
                mediaRecorder = new MediaRecorder(stream);
    
                mediaRecorder.onstart = () => {
                    audioChunks = [];
                    recordBtn.textContent = "Stop Recording";
                    status_1.textContent = "Recording... Speak into your microphone.";
                    console.log("Recording started.");
                };
    
                mediaRecorder.ondataavailable = (event) => {
                    audioChunks.push(event.data);
                    console.log("Audio data available.");
                };
    
                mediaRecorder.onstop = async () => {
                    recordBtn.textContent = "Start Recording";
                    status_1.textContent = "Recording stopped. Sending audio to the server...";
                    try {
                        const audioBlob = new Blob(audioChunks, { type: "audio/webm" });
                        const audioSent = await sendToServer(audioBlob, "audio");
    
                        if (audioSent) {
                            alert("Audio sent successfully!");
                            console.log("Audio sent successfully!");
                        } else {
                            status_1.textContent = "Error occurred while transferring audio.";
                            alert("WARNING! ERROR: Audio not sent.");
                        }
                    } catch (err) {
                        console.error("Error sending audio:", err);
                    }
                };
    
                mediaRecorder.start();
            } else {
                mediaRecorder.stop();
                console.log("Recording stopped.");
            }
        } catch (error) {
            console.error("Error during recording:", error);
            alert("An error occurred while accessing the microphone.");
        } finally {
            status_1.textContent = "Press the button to start recording.";
            recordBtn.disabled = false; // Re-enable the button
        }
    });*/
    
    // Voice Recognition Functionality
recordBtn.addEventListener("click", async () => {
    if (!navigator.mediaDevices || !navigator.mediaDevices.getUserMedia) {
        alert("Your browser does not support audio recording.");
        return;
    }

    if (!mediaRecorder || mediaRecorder.state === "inactive") {
        try {
            const stream = await navigator.mediaDevices.getUserMedia({ audio: true });

            mediaRecorder = new MediaRecorder(stream);

            mediaRecorder.onstart = () => {
                audioChunks = [];
                recordBtn.textContent = "Stop Recording";
                status_1.textContent = "Recording... Speak into your microphone.";
                
                // Automatically stop recording after 10 seconds
                setTimeout(() => {
                    mediaRecorder.stop();
                    status_1.textContent = "Recording stopped automatically after 10 seconds.";
                }, 10000); // Stop after 10 seconds (10000 ms)
            };

            mediaRecorder.ondataavailable = (event) => {
                audioChunks.push(event.data);
            };

            mediaRecorder.onstop = async () => {
                recordBtn.textContent = "Start Recording";
                status_1.textContent = "Recording stopped. Sending audio to the server...";

                const audioBlob = new Blob(audioChunks, { type: "audio/webm" });
                const audioSent = await sendToServer(audioBlob, "audio");

                if (audioSent) {
                    alert("Audio sent successfully!");
                } else {
                    status_1.textContent = "Error occurred while transferring audio.";
                    alert("WARNING! ERROR");
                }
            };

            mediaRecorder.start();
        } catch (error) {
            console.error("Error accessing the microphone:", error);
            alert("Failed to access the microphone. Please check your microphone settings or permissions.");
            status_1.textContent = "Failed to access the microphone.";
        }
    } else {
        mediaRecorder.stop();
    }
});

        
    

    // Face Recognition Functionality
    captureBtn.addEventListener("click", async () => {
        if (!navigator.mediaDevices || !navigator.mediaDevices.getUserMedia) {
            alert("Your browser does not support video capture.");
            return;
        }

        try {
            const stream = await navigator.mediaDevices.getUserMedia({ video: true });
            camera.srcObject = stream;

            setTimeout(async () => {
                const context = canvas.getContext("2d");
                canvas.width = camera.videoWidth;
                canvas.height = camera.videoHeight;
                context.drawImage(camera, 0, 0, canvas.width, canvas.height);

                const imageBlob = await new Promise((resolve) => canvas.toBlob(resolve, "image/jpeg"));
                const imageSent = await sendToServer(imageBlob, "image");

                if (imageSent) {
                    alert("Image captured and sent successfully!");
                } else {
                    status_2.textContent = "Error occurred while sending image.";
                    alert("WARNING! ERROR");
                }

                stream.getTracks().forEach((track) => track.stop()); // Stop the camera
            }, 5000); // Wait for 5 seconds
        } catch (error) {
            console.error("Error accessing the camera:", error);
            alert("Unable to access the camera.");
        }
    });

    // Function to send media (audio/image) to the server
    async function sendToServer(blob, type) {
        try {
            const formData = new FormData();
            formData.append(type, blob, type === "audio" ? "recording.webm" : "capture.jpg");

            const response = await fetch("http://localhost:5000/upload", {
                method: "POST",
                body: formData,
            });

            return response.ok;
        } catch (error) {
            console.error("Error sending data to server:", error);
            return false;
        }
    }

    // Sliding Functionality
    const boxes = document.querySelectorAll(".box");
    const totalBoxes = boxes.length;
    let currentIndex = 0;
    const previousButton = document.getElementById('previous-btn');
    const nextButton = document.getElementById('next-btn');

// Function to show the current box
function showBox(index) {
    boxes.forEach((box, i) => {
        box.style.display = i === index ? 'block' : 'none';
    });
    updateButtonStates();
}

// Function to update button states
function updateButtonStates() {
    if (currentIndex === 0) {
        previousButton.disabled = true;
        previousButton.classList.add('disabled');
    } else {
        previousButton.disabled = false;
        previousButton.classList.remove('disabled');
    }

    if (currentIndex === totalBoxes - 1) {
        nextButton.disabled = true;
        nextButton.classList.add('disabled');
    } else {
        nextButton.disabled = false;
        nextButton.classList.remove('disabled');
    }
}

// Event listener for the Previous button
previousButton.addEventListener('click', () => {
    if (currentIndex > 0) {
        currentIndex--;
        showBox(currentIndex);
    }
});

// Event listener for the Next button
nextButton.addEventListener('click', () => {
    if (currentIndex < totalBoxes - 1) {
        currentIndex++;
        showBox(currentIndex);
    }
});

// Initialize the first box to be visible and update button states
showBox(currentIndex);

});
