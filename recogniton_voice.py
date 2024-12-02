import torch
from torch import nn
import librosa
import numpy as np
from scipy.spatial.distance import cosine



class VGGVox(nn.Module):
    def __init__(self):
        super(VGGVox, self).__init__()   # Define the VGGVox architecture (simplified for brevity)
        self.conv_layers = nn.Sequential(
            nn.Conv2d(1, 96, kernel_size=7, stride=2, padding=3),
            nn.ReLU(),
            nn.MaxPool2d(3, 2),
            nn.Conv2d(96, 256, kernel_size= 5, stride= 2, padding= 2),
            nn.ReLU(),
            nn.MaxPool2d(3, 2)
        )
        # Dynamically calculate the input size for the first fully connected layer
        self.fc_layers = nn.Sequential(
            nn.Linear(self._get_conv_output_size((1, 40, 312)), 4096), # Calculate size dynamically based on input shape (1, 40, 312) from your mel spectrogram
            nn.ReLU(),
            nn.Linear(4096, 1024)
        )

    def _get_conv_output_size(self, input_shape):

        """
        Calculates the output size of the convolutional layers for a given input shape.
        This is necessary to dynamically adjust the input size of the first fully connected layer.
        """
        # Create a dummy input tensor with the specified shape
        dummy_input = torch.zeros(1, *input_shape)

        # Pass the dummy input through the convolutional layers
        output = self.conv_layers(dummy_input)

        # Calculate the total number of features in the output
        output_size = output.view(output.size(0), -1).shape[1]
        return output_size

    def forward(self, x):
        x = self.conv_layers(x)
        x = x.view(x.size(0), -1)  # Flatten
        x = self.fc_layers(x)
        return x


model = VGGVox()
model.eval()


#*_____________________________________________________________________________________________________________________________________*#
editied


saved_audio = "D:\\voices\\Mohammed\\M96.wav"  


new_audio = "D:\\voices\\Hussin\\H11.wav"



#*_________________________________________________________________________________________*#

"""
Preprocesses an audio file by loading it, generating a Mel-spectrogram,
    and converting the spectrogram to a log scale.
"""

def preprocess_audio(file_path):
    y, sr = librosa.load(file_path, sr=16000)  # 16kHz sampling rate
    mel_spec = librosa.feature.melspectrogram(y=y, sr=sr, n_mels=40, fmax=8000) # Generate Mel-spectrogram
    log_mel_spec = librosa.power_to_db(mel_spec, ref=np.max)
    # Convert the Mel-spectrogram to the logarithmic scale for better representation
    return log_mel_spec



"""
Compares two audio embeddings using cosine similarity and determines
    if they match based on a given threshold.
"""
def match_embedding(embedding1, embedding2, threshold=0.5):
    embedding1 = embedding1.cpu().numpy().flatten()
    embedding2 = embedding2.cpu().numpy().flatten()
    similarity = 1 - cosine(embedding1, embedding2)
    print(f"Embedding Similarity: {similarity:.7f}")
    return similarity >= threshold



"""
    Calculates the spectral centroid of an audio file, which represents
    the center of mass of the audio spectrum.
"""
def get_spectral_centroid(audio_file):
    y, sr = librosa.load(audio_file, sr=16000)
    cent = librosa.feature.spectral_centroid(y=y, sr=sr)
    avg_cent = np.mean(cent)
    return avg_cent



"""
Compares the spectral centroids of two audio files to determine if
they match based on a threshold.
"""
def match_frequency(audiofile1, audiofile2, threshold=100):
    test1 = get_spectral_centroid(audiofile1)
    test2 = get_spectral_centroid(audiofile2)
    diff = np.abs(test1 - test2)
    print(f"Spectral Centroid Difference: {diff:.7f}")
    return diff <= threshold

#*_____________________________*#


# Preprocess and extract embedding for stored audio
mel_spec = preprocess_audio(saved_audio)
input_tensor = torch.tensor(np.expand_dims(mel_spec, axis=0), dtype=torch.float32).unsqueeze(0)
with torch.no_grad():
    old_aduio = model(input_tensor)

# Preprocess and extract embedding for new audio input
mel_spec2 = preprocess_audio(new_audio)
input_tensor2 = torch.tensor(np.expand_dims(mel_spec2, axis=0), dtype=torch.float32).unsqueeze(0)
with torch.no_grad():
    new_aduio = model(input_tensor2)


embedding_match = match_embedding(old_aduio, new_aduio)
frequency_match = match_frequency(saved_audio, new_audio)



if embedding_match and frequency_match:
    print("Access Granted: Both embedding and frequency match.")
else:
    print("Access Denied: Mismatch in either embedding or frequency.")