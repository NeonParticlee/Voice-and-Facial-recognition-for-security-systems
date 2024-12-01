import pyaudio  # for recording audio
import wave     # to save audio as .wav
import keyboard # to capture keyboard events
import time     # for adding delays
import pickle   # to load model
import numpy as np
import soundfile as sf
import librosa
import speech_recognition as sr  # for speech-to-text transcription

# Audio recording parameters
chunk = 1024
format = pyaudio.paInt16
channels = 1
rate = 44100
Output_Filename = "Recorded.wav"

p = pyaudio.PyAudio()

# Open new audio stream
stream = p.open(format=format,
                channels=channels,
                rate=rate,
                input=True,
                frames_per_buffer=chunk)

frames = []
print("Press SPACE to start recording")
keyboard.wait('space')
print("Recording... Press SPACE to stop.")
time.sleep(0.2)

# Record audio until SPACE is pressed again
while True:
    data = stream.read(chunk)
    frames.append(data)
    if keyboard.is_pressed('space'):
        print("Stopping recording")
        time.sleep(0.2)
        break

stream.stop_stream()
stream.close()
p.terminate()

# Save recorded audio to a .wav file
wf = wave.open(Output_Filename, 'wb')
wf.setnchannels(channels)
wf.setsampwidth(p.get_sample_size(format))
wf.setframerate(rate)
wf.writeframes(b''.join(frames))
wf.close()

# Load the gender detection model
with open('gender_detection_model.pkl', 'rb') as f:
    model = pickle.load(f)

def extract_frequency_features(audio_file):
    y, sr = librosa.load(audio_file, sr=rate)
    stft = np.abs(librosa.stft(y))
    frequencies = librosa.fft_frequencies(sr=sr)
    max_freqs = np.argmax(stft, axis=0)
    fundamental_frequencies = frequencies[max_freqs]
    valid_frequencies = fundamental_frequencies[fundamental_frequencies > 0]

    if len(valid_frequencies) == 0:
        print("No valid frequencies detected, using default [100, 100, 100].")
        return np.array([[100, 100, 100]])

    meanfun = np.mean(valid_frequencies)
    minfun = np.min(valid_frequencies)
    maxfun = np.max(valid_frequencies)

    return np.array([[meanfun, minfun, maxfun]])

# Extract features and predict gender
X_test = extract_frequency_features(Output_Filename)
prediction = model.predict(X_test)
gender = 'Male' if prediction[0] < 0.5 else 'Female'
print(f"Predicted Gender: {gender}")

# Speech-to-text recognition using the Google API
recognizer = sr.Recognizer()

# Use the recorded audio file for speech recognition
with sr.AudioFile(Output_Filename) as source:
    audio_data = recognizer.record(source)  # capture the audio data
    try:
        # Convert speech to text using Google Speech Recognition
        text = recognizer.recognize_google(audio_data)
        print(f"Transcribed Text: {text}")
    except sr.UnknownValueError:    
        print("Google Speech Recognition could not understand the audio.")
    except sr.RequestError as e:
        print(f"Could not request results from Google Speech Recognition service; {e}")
   