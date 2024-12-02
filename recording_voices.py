import os
import pyaudio
import wave
import keyboard
import time
from scipy.spatial.distance import cosine
import torch
import librosa
import numpy as np

# Parameters for audio recording
chunk = 1024
format = pyaudio.paInt16
channels = 1
rate = 16000  # Match the sampling rate used in preprocess_audio
stored_audio_file = "main_voice.wav"  # Main voice to store
new_audio_file = "new_voice.wav"  # New voice for comparison

# Define the VGGVox model
class VGGVox(torch.nn.Module):
    def __init__(self):
        super(VGGVox, self).__init__()
        self.conv_layers = torch.nn.Sequential(
            torch.nn.Conv2d(1, 96, kernel_size=7, stride=2, padding=3),
            torch.nn.ReLU(),
            torch.nn.MaxPool2d(3, 2),
            torch.nn.Conv2d(96, 256, kernel_size=5, stride=2, padding=2),
            torch.nn.ReLU(),
            torch.nn.MaxPool2d(3, 2),
        )
        # Dynamically calculate input size for FC layer
        self.fc_layers = torch.nn.Sequential(
            torch.nn.Linear(self._get_conv_output_size((1, 40, 312)), 4096),
            torch.nn.ReLU(),
            torch.nn.Linear(4096, 1024),
        )

    def _get_conv_output_size(self, input_shape):
        dummy_input = torch.zeros(1, *input_shape)
        output = self.conv_layers(dummy_input)
        return output.view(output.size(0), -1).shape[1]

    def forward(self, x):
        x = self.conv_layers(x)
        x = x.view(x.size(0), -1)  # Flatten
        x = self.fc_layers(x)
        return x


model = VGGVox()
model.eval()


# Function to preprocess audio
def preprocess_audio(file_path):
    y, sr = librosa.load(file_path, sr=16000)
    mel_spec = librosa.feature.melspectrogram(y=y, sr=sr, n_mels=40, fmax=8000)
    log_mel_spec = librosa.power_to_db(mel_spec, ref=np.max)
    return log_mel_spec


# Function to generate embedding
def generate_embedding(audio_file):
    mel_spec = preprocess_audio(audio_file)
    mel_spec = np.expand_dims(mel_spec, axis=0)  # Add batch dimension
    input_tensor = torch.tensor(mel_spec, dtype=torch.float32).unsqueeze(0)  # Add channel dimension
    with torch.no_grad():
        embedding = model(input_tensor)
    return embedding


# Function to compare embeddings
def is_match(audio1, audio2, threshold=0.5):
    audio1 = audio1.cpu().numpy().flatten()
    audio2 = audio2.cpu().numpy().flatten()
    similarity = 1 - cosine(audio1, audio2)
    return similarity >= threshold


# Function to record audio
def record_audio(output_filename):
    p = pyaudio.PyAudio()
    stream = p.open(format=format, channels=channels, rate=rate, input=True, frames_per_buffer=chunk)
    frames = []
    print(f"Press SPACE to start recording for {output_filename}.")
    keyboard.wait('space')
    print("Recording started... You have 7 seconds.")
    start_time = time.time()
    while True:
        data = stream.read(chunk)
        frames.append(data)
        elapsed_time = time.time() - start_time
        if elapsed_time >= 7:  # Stop after 7 seconds
            break
    print("Recording stopped.")
    stream.stop_stream()
    stream.close()
    p.terminate()

    wf = wave.open(output_filename, 'wb')
    wf.setnchannels(channels)
    wf.setsampwidth(p.get_sample_size(format))
    wf.setframerate(rate)
    wf.writeframes(b''.join(frames))
    wf.close()
    print(f"Saved audio to {output_filename}.")

if __name__ == "__main__":
    stored_audio_file = "main_voice.wav"  # Stored voice file
    new_audio_file = "new_voice.wav"  # New voice file for comparison

    # Check if stored audio file exists
    if os.path.exists(stored_audio_file):
        print(f"Stored voice found at {stored_audio_file}.")
        print("Do you want to delete the stored voice and record a new one? (yes/no)")
        choice = input().strip().lower()
        if choice == "yes":
            os.remove(stored_audio_file)
            print("Please record your main voice.")
            record_audio(stored_audio_file)
        else:
            print("Proceeding with the existing stored voice.")
    else:
        print("No stored voice found. Please record your main voice.")
        record_audio(stored_audio_file)

    # Record new voice for comparison
    print("Record a new voice for comparison.")
    record_audio(new_audio_file)

    # Generate embeddings and compare
    print("Comparing voices...")
    stored_embedding = generate_embedding(stored_audio_file)
    new_embedding = generate_embedding(new_audio_file)

    if is_match(stored_embedding, new_embedding):
        print("Access Granted")
    else:
        print("Access Denied")