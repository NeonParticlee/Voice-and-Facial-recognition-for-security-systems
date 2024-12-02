import torch
import torchaudio
import glob
import pickle
import os
from speechbrain.pretrained import EncoderClassifier
from scipy.spatial.distance import cosine

# Load the pretrained model
classifier = EncoderClassifier.from_hparams(
    source="speechbrain/spkrec-ecapa-voxceleb", savedir="tmp_model"
)

# Function to extract embedding from a single audio file and cache it
def get_embedding(file_path, cache_dir="embeddings_cache"):
    os.makedirs(cache_dir, exist_ok=True)
    cache_file = os.path.join(cache_dir, os.path.basename(file_path) + ".pkl")
    
    if os.path.exists(cache_file):
        with open(cache_file, "rb") as f:
            return pickle.load(f)  # Load cached embedding
    
    # If not cached, compute embedding
    signal, fs = torchaudio.load(file_path)
    embedding = classifier.encode_batch(signal).squeeze(0).squeeze(0)
    
    # Save the embedding for future use
    with open(cache_file, "wb") as f:
        pickle.dump(embedding, f)
    
    return embedding

# Enroll a user using a limited number of audio files
def enroll_user(directory_path, max_files=10):
    sample_files = glob.glob(directory_path + "\\*.wav")[:max_files]
    embeddings = [get_embedding(f) for f in sample_files]
    return torch.mean(torch.stack(embeddings), dim=0)

# Function to compare embeddings
def compare_with_enrolled_users(test_embedding, enrolled_users, threshold=0.75):
    best_match, highest_similarity = None, 0

    for user_name, user_embedding in enrolled_users.items():
        similarity = 1 - cosine(test_embedding.numpy(), user_embedding.numpy())
        print(f"Similarity with {user_name}: {similarity:.4f}")
        
        if similarity > highest_similarity:
            highest_similarity = similarity
            best_match = user_name

    if highest_similarity >= threshold:
        print(f"Access Granted: Voice matches with {best_match}!")
    else:
        print("Access Denied: No matching user found.")

# Enroll users and save to a file (done once)
if not os.path.exists("enrolled_users.pkl"):
    enrolled_users = {
        "User1": enroll_user("D:\\voices\\Mohammed"),
        "User2": enroll_user("D:\\voices\\farah"),
        "User3": enroll_user("D:\\voices\\Hussin"),
    }
    with open("enrolled_users.pkl", "wb") as f:
        pickle.dump(enrolled_users, f)

# Load enrolled users from the saved file
with open("enrolled_users.pkl", "rb") as f:
    enrolled_users = pickle.load(f)

# Test file to compare
test_file = "D:\\voices\\Mohammed\\M3.wav"
test_embedding = get_embedding(test_file)

# Compare the test embedding with enrolled users
compare_with_enrolled_users(test_embedding, enrolled_users)
