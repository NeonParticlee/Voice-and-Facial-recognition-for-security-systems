import torch
import torchaudio
import glob
import pickle
import os
import logging
from speechbrain.pretrained import EncoderClassifier
from scipy.spatial.distance import cosine



class VoiceRecognitionConfig:
    def __init__(self):
        self.target_sr = 16000
        self.threshold = 0.12
        self.max_enrollment_files = 10
        self.cache_dir = "embeddings_cache"
        self.model_path = "speechbrain/spkrec-ecapa-voxceleb"
        self.model_save_dir = "tmp_model"
        self.enrolled_users_file = "enrolled_users.pkl"

class AudioPreprocessor:
    def __init__(self, config):
        self.target_sr = config.target_sr
        
    def process_audio_file(self, file_path):
        try:
            waveform, sample_rate = torchaudio.load(file_path)
            
            # Resample if necessary
            if sample_rate != self.target_sr:
                resampler = torchaudio.transforms.Resample(
                    orig_freq=sample_rate, 
                    new_freq=self.target_sr
                )
                waveform = resampler(waveform)
                
            # Convert to mono
            if waveform.shape[0] > 1:
                waveform = torch.mean(waveform, dim=0, keepdim=True)
            elif waveform.shape[0] == 1:
                # Ensure it's still 2D
                waveform = waveform.view(1, -1)
                
            # Normalize
            waveform = waveform / (torch.max(torch.abs(waveform)) + 1e-8)
            
            return waveform, self.target_sr
            
        except Exception as e:
            logging.error(f"Error processing audio file {file_path}: {str(e)}")
            return None, None

class VoiceRecognitionSystem:
    def __init__(self):
        self.config = VoiceRecognitionConfig()
        self.setup_logging()
        
        self.classifier = EncoderClassifier.from_hparams(
            source=self.config.model_path,
            savedir=self.config.model_save_dir
        )
        self.preprocessor = AudioPreprocessor(self.config)
        os.makedirs(self.config.cache_dir, exist_ok=True)
        
    def setup_logging(self):
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler('voice_recognition.log'),
                logging.StreamHandler()
            ]
        )

    def get_embedding(self, file_path):
        try:
            cache_file = os.path.join(
                self.config.cache_dir, 
                os.path.basename(file_path) + ".pkl"
            )
            
            # Return cached embedding if exists
            if os.path.exists(cache_file):
                with open(cache_file, "rb") as f:
                    embedding = pickle.load(f)
                    # Ensure correct shape
                    if len(embedding.shape) == 1:
                        embedding = embedding.unsqueeze(0)
                    return embedding
            
            # Process audio file
            signal, fs = self.preprocessor.process_audio_file(file_path)
            if signal is None:
                raise ValueError(f"Failed to process audio file: {file_path}")
            
            # Generate embedding
            with torch.no_grad():
                embedding = self.classifier.encode_batch(signal)
                # Ensure consistent shape
                if len(embedding.shape) == 3:
                    embedding = embedding.squeeze(0)
                if len(embedding.shape) == 1:
                    embedding = embedding.unsqueeze(0)
            
            # Cache the embedding
            with open(cache_file, "wb") as f:
                pickle.dump(embedding, f)
                
            return embedding
            
        except Exception as e:
            logging.error(f"Error getting embedding for {file_path}: {str(e)}")
            logging.debug(f"Stack trace:", exc_info=True)
            return None

    def enroll_user(self, directory_path):
        try:
            sample_files = glob.glob(os.path.join(directory_path, "*.wav"))
            if not sample_files:
                logging.error(f"No WAV files found in {directory_path}")
                return None
                
            sample_files = sample_files[:self.config.max_enrollment_files]
            embeddings = []
            
            for file in sample_files:
                embedding = self.get_embedding(file)
                if embedding is not None:
                    # Log embedding shape for debugging
                    logging.info(f"Embedding shape for {file}: {embedding.shape}")
                    embeddings.append(embedding)
            
            if not embeddings:
                logging.error("No valid embeddings generated during enrollment")
                return None
            
            # Stack embeddings and compute mean
            stacked_embeddings = torch.stack(embeddings)
            logging.info(f"Stacked embeddings shape: {stacked_embeddings.shape}")
            return torch.mean(stacked_embeddings, dim=0)
            
        except Exception as e:
            logging.error(f"Error enrolling user from {directory_path}: {str(e)}")
            logging.debug(f"Stack trace:", exc_info=True)
            return None

    def verify_speaker(self, test_file, enrolled_users):
        try:
            test_embedding = self.get_embedding(test_file)
            if test_embedding is None:
                raise ValueError("Failed to get embedding from test file")
            
            # Ensure test embedding has correct shape
            if len(test_embedding.shape) == 1:
                test_embedding = test_embedding.unsqueeze(0)
            
            best_match, highest_similarity = None, 0
            results = {}
            
            for user_name, user_embedding in enrolled_users.items():
                # Ensure user embedding has correct shape
                if len(user_embedding.shape) == 1:
                    user_embedding = user_embedding.unsqueeze(0)
                
                similarity = 1 - cosine(
                    test_embedding.squeeze().numpy(),
                    user_embedding.squeeze().numpy()
                )
                results[user_name] = similarity
                
                if similarity > highest_similarity:
                    highest_similarity = similarity
                    best_match = user_name
            
            confidence = (
                (highest_similarity - self.config.threshold) / 
                (1 - self.config.threshold)
            ) * 100 if highest_similarity >= self.config.threshold else 0
            
            return {
                'match': best_match if confidence > 0 else None,
                'confidence': confidence,
                'similarities': results
            }
            
        except Exception as e:
            logging.error(f"Error during speaker verification: {str(e)}")
            logging.debug(f"Stack trace: ", exc_info=True)
            return None

def main():
    try:
        system = VoiceRecognitionSystem()
        logging.info("Voice Recognition System initialized")

        # Enroll users
        user_directories = {
            "Mohammed": "D:\\voices\\Mohammed",
            "Farah": "D:\\voices\\farah",
            "Hussain": "D:\\voices\\Hussin"
        }

        enrolled_users = {}
        for user, directory in user_directories.items():
            logging.info(f"Enrolling user: {user}")
            embedding = system.enroll_user(directory)
            if embedding is not None:
                enrolled_users[user] = embedding
                logging.info(f"Successfully enrolled {user}")
            else:
                logging.error(f"Failed to enroll {user}")

        if enrolled_users:
            # Save enrolled users
            with open(system.config.enrolled_users_file, "wb") as f:
                pickle.dump(enrolled_users, f)
            logging.info("Saved enrolled users to file")

            # Test verification
            test_file = "D:\\voices\\faresss.wav"
            if os.path.exists(test_file):
                result = system.verify_speaker(test_file, enrolled_users)
                
                if result:
                    if result['match']:
                        print(f"\n Match found: {result['match']}")
                        print(f"Confidence: {result['confidence']:.2f}%")
                        print("\n Similarity scores:")
                        for user, score in result['similarities'].items():
                            print(f"{user}: {score:.4f}")
                    else:
                        print("\n No match found - confidence below threshold")
                        print("\n Similarity scores:")
                        for user, score in result['similarities'].items():
                            print(f"{user}: {score:.4f}")
                else:
                    print("Verification failed due to an error")
            else:
                logging.error("Test file not found")
                print("Test file not found")
        else:
            print("No users were successfully enrolled. Please check the audio files and try again.")

    except Exception as e:
        logging.error(f"Main execution error: {str(e)}")
        print(f"An error occurred: {str(e)}")

if __name__ == "__main__":
    main()