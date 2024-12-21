document.addEventListener("DOMContentLoaded", () => {
    // Voice Recognition Variables
    const recordBtn = document.getElementById("record-btn");
    const stopBtn = document.getElementById("stop-btn");
    const captureBtn = document.getElementById("capture-btn");
    const camera = document.getElementById("camera");
    const canvas = document.getElementById("canvas");
    const status_1 = document.getElementById("status_1");
    const status_2 = document.getElementById("status_2");
    const timerBox = document.getElementById("timer_box");
    const nameInput = document.getElementById("name-input"); // Input field for user's name
    let mediaRecorder;
    let audioChunks = [];

    // Voice Recognition Functionality
    recordBtn.addEventListener("click", async () => {
        if (!navigator.mediaDevices || !navigator.mediaDevices.getUserMedia) {
            alert("Your browser does not support audio recording.");
            return;
        }

        const permissionGranted = await new Promise((resolve) => {
            const result = window.confirm("The record stops automatically after 10 seconds.");
            resolve(result);
        });

        if (permissionGranted) {
            timerBox.style.display = "flex";
            timerBox.style.justifyContent = "center";
            timerBox.style.alignItems = "center";

            recordBtn.disabled = true; // Disable the record button

            // Start recording after the user click on "OK"
            try {
                const stream = await navigator.mediaDevices.getUserMedia({ audio: true }); // Access the mic

                mediaRecorder = new MediaRecorder(stream);

                mediaRecorder.onstart = () => {
                    audioChunks = [];
                    recordBtn.textContent = "Recording..."; // Updating record button text
                    stopBtn.style.display = "inline-block"; // Show the stop button
                    recordBtn.disabled = false; // Re-enable the record button
                    status_1.textContent = "Recording... Speak into your microphone.";

                    let timeLeft = 10;
                    const countdownInterval = setInterval(() => {
                        timerBox.textContent = timeLeft; // Update timer box with the time left
                        timeLeft--;

                        if (timeLeft < 0) {
                            clearInterval(countdownInterval);
                            mediaRecorder.stop();
                            status_1.textContent = "Recording stopped. Sending audio to the server...";
                        }
                    }, 1000); // 10 seconds for record
                };

                mediaRecorder.ondataavailable = (event) => {
                    audioChunks.push(event.data);
                };

                mediaRecorder.onstop = async () => {
                    recordBtn.textContent = "Start Recording";
                    stopBtn.style.display = "none"; // Hide the stop button after the record is ended
                    timerBox.style.display = "none"; // Hide the time boxer after the record is ended

                    const audioBlob = new Blob(audioChunks, { type: "audio/webm" });
                    const success = await sendDataToServer(audioBlob, "audio");

                    if (success) {
                        alert("Audio sent successfully!");
                    } else {
                        status_1.textContent = "Error occurred while transferring the audio to the server.";
                        alert("An error occurred while uploading the audio.");
                    }
                };

                mediaRecorder.start();
            } catch (error) {
                console.error("Error accessing the microphone:", error);
                alert("Failed to access the microphone.");
            }
        }
    });

    // Stoping the record manually or automatically
    stopBtn.addEventListener("click", () => { 
        if (mediaRecorder && mediaRecorder.state === "recording") {
            mediaRecorder.stop(); // Stop recording manually
            stopBtn.style.display = "none"; // Hide the stop button after the record is stopped
            status_1.textContent = "Recording stopped by user.";
            timerBox.style.display = "none"; // Hide the time boxer after the record is ended
            recordBtn.textContent = "Start Recording"; // Reset the record button text
            recordBtn.disabled = false; // Re-enable the record button
        }
    });

    // Valid Usernam Function
    function isNameValid(name) {
        const namePattern = /^[A-Za-z\s]+$/; // Only letters and spaces are allowed
        const words = name.trim().split(/\s+/); // Split by spaces into words

        return (
            name.length >= 7 && // Check length
            name.length <= 25 && // Check length
            namePattern.test(name) && // Check for valid characters (letters and spaces)
            words.length >= 2 // Ensures at least two words (first and last name)
        );
    }

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

        if (!navigator.mediaDevices || !navigator.mediaDevices.getUserMedia) {
            alert("Your browser does not support video capture.");
            return;
        }

        try {
            const stream = await navigator.mediaDevices.getUserMedia({ video: true }); // Access the camera
            camera.srcObject = stream;

            const capturedImages = []; // Array to store captured images 
            let timeLeft = 20; // 20 seconds countdown

            // Set interval to capture an image every 0.5 second
            const captureInterval = setInterval(() => {
                const context = canvas.getContext("2d");
                canvas.width = camera.videoWidth;
                canvas.height = camera.videoHeight;
                context.drawImage(camera, 0, 0, canvas.width, canvas.height);

                canvas.toBlob((imageBlob) => {
                    if (imageBlob) {
                        capturedImages.push(imageBlob);
                        console.log("Image captured and stored successfully!");
                    }
                }, "image/jpeg");

                timeLeft -= 0.5;
                if (timeLeft <= 0) {
                    clearInterval(captureInterval);
                    stream.getTracks().forEach((track) => track.stop());

                    alert(`Captured ${capturedImages.length + 1} images. Now sending them to the server...`);
                    sendCapturedImagesToServer(capturedImages, name);
                }
            }, 500); // Take an image every 0.5 second
        } catch (error) {
            console.error("Error accessing the camera:", error);
            alert("Unable to access the camera.");
        }
    });

    // Consolidated Function to Send Data
    async function sendDataToServer(blob, type, userName = null) {
        try {
            const formData = new FormData();
            formData.append(type, blob, type === "audio" ? "recording.webm" : "capture.jpg");
            if (userName) {
                formData.append("userName", userName);
            }

            const response = await fetch("http://localhost:5000/upload", {
                method: "POST",
                body: formData,
            });

            return response.ok;
        } catch (error) {
            console.error(`Error sending ${type} to the server:`, error);
            return false;
        }
    }

    async function sendCapturedImagesToServer(images, userName) {
        for (let i = 0; i < images.length; i++) {
            const image = images[i];
            const success = await sendDataToServer(image, "image", userName);
            if (!success) {
                alert("An error occurred while uploading one or more images.");
                break;
            } else {
                console.log(`Image ${i + 1} uploaded successfully!`);
            }
        }
    }
    

    // Sliding Functionality for Boxes
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
                previousButton.style.cursor = "not-allowed";
            } else {
                previousButton.disabled = false;
                previousButton.classList.remove('disabled');
                previousButton.style.cursor = "pointer";
            }
        
            if (currentIndex === totalBoxes - 1) {
                nextButton.disabled = true;
                nextButton.classList.add('disabled');
                nextButton.style.cursor = "not-allowed";
            } else {
                nextButton.disabled = false;
                nextButton.classList.remove('disabled');
                nextButton.style.cursor = "pointer";
            }
        }

    // Event Listener for the Previous Button
    previousButton.addEventListener('click', () => {
        if (currentIndex > 0) {
            currentIndex--;
            showBox(currentIndex);
        }
    });

    // Event Listener for the Next Button
    nextButton.addEventListener('click', () => {
        if (currentIndex < totalBoxes - 1) {
            currentIndex++;
            showBox(currentIndex);
        }
    });

    // Initialize the first box to be visible and update button states
    showBox(currentIndex);
});
