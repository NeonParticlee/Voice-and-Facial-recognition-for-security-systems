import pyaudio # rec audio 
import wave # save audio as .wav
import keyboard  # takes keyboard events
import time  # adding delays
import pickle
import numpy as np
import soundfile as sf
import librosa 

# some parameters for audio that will be stored as wav
chunk = 1024 # how many frames in buffer 
format = pyaudio.paInt16 
channels = 1 # monoaudio
rate = 44100
Output_Filename = "Recorded.wav"
 
p = pyaudio.PyAudio() # pyaudio obj

stream = p.open(format=format,  # new audio stream 
                channels=channels,
                rate=rate,
                input=True,
                frames_per_buffer=chunk)
  
frames = [] # stores audio chunks 
print("Press SPACE to start recording")
keyboard.wait('space')  # wait for user to press space to start recording 
print("Recording... Press SPACE to stop.")
time.sleep(0.2) # to avoid the fact that when i start ecording it will stop immediatly 
  
while True:
    try:
        data = stream.read(chunk)  
        frames.append(data)
    except KeyboardInterrupt: # stop with ctr c 
        break
    if keyboard.is_pressed('space'):
        print("stopping recording") 
        time.sleep(0.2)
        break   

stream.stop_stream()  # stop stream
stream.close()   # close it     
p.terminate()  # git rid of obj 


# save audio to wav file 
wf = wave.open(Output_Filename, 'wb')   # open wave in write byte mode since we will be writing the bytes containing audio to this file 
wf.setnchannels(channels)
wf.setsampwidth(p.get_sample_size(format))
wf.setframerate(rate)
wf.writeframes(b''.join(frames)) # write all recorded frames to file 
wf.close()

with open('D:\studying\Machine learning MM\gender_detection_model.pkl', 'rb') as f:
    model = pickle.load(f)

def extract_frequency_features(audio_file):
    """Extract mean, min, and max frequency from the audio.""" # since the model already uses these for training and such 
    y, sr = librosa.load(audio_file, sr=rate)

    # Calculate the Short-Time Fourier Transform (STFT) : maps a signal into a two-dimensional function of time and frequency
    stft = np.abs(librosa.stft(y))

    # Extract frequencies corresponding to the maximum magnitudes
    frequencies = librosa.fft_frequencies(sr=sr)
        
    # Get the max frequency for each frame (i.e., the fundamental frequency per frame)
    max_freqs = np.argmax(stft, axis=0)
    fundamental_frequencies = frequencies[max_freqs]

    # Filter out non-zero frequencies (silence might yield zeros)
    valid_frequencies = fundamental_frequencies[fundamental_frequencies > 0]

    if len(valid_frequencies) == 0:
        print("No valid frequencies detected, using default [100, 100, 100].")
        return np.array([[100, 100, 100]])

    # Calculate mean, min, and max frequencies
    meanfun = np.mean(valid_frequencies)
    minfun = np.min(valid_frequencies)
    maxfun = np.max(valid_frequencies)

    return np.array([[meanfun, minfun, maxfun]])

# Extract frequency features from the audio file
X_test = extract_frequency_features(Output_Filename)
print(f"Extracted Features: {X_test}")

# ============== MAKE PREDICTION ==============
prediction = model.predict(X_test)    
gender = 'Male' if prediction[0] > 0.5   else 'Female'    
print(f"Predicted Gender: {gender}")  

  