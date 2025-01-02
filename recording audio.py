import os
import sounddevice as sd
import numpy as np
import wave
import psycopg2
import pyttsx3
from fuzzywuzzy import fuzz
import speech_recognition as sr
import tempfile

# Function to connect to the database
def connect_to_db():
    try:
        conn = psycopg2.connect(
            host="localhost",
            database="my_database",  
            user="farah", 
            password="postgres"  
        )
        return conn
    except Exception as e:
        print("Error connecting to database:", e)
        return None

# Function to create the 'users' table if not already created
def create_table(conn):
    create_table_query = '''
    CREATE TABLE IF NOT EXISTS users (
        id SERIAL PRIMARY KEY,
        name VARCHAR(100),
        email VARCHAR(100) UNIQUE NOT NULL,
        password VARCHAR(100) NOT NULL,
        voice_sample BYTEA
    );
    '''
    try:
        cursor = conn.cursor()
        cursor.execute(create_table_query)
        conn.commit()
        cursor.close()
        print("Table created successfully.")
    except Exception as e:
        print("Error creating table:", e)

# Function to register a new user
def register_user():
    name = input("Enter your name: ")
    email = input("Enter your email: ")
    password = input("Enter your password: ")

    print("Please speak your voice sample for registration...")
    recorded_voice = record_voice("voice_sample.wav")
    
    if recorded_voice:
        print("Recording complete.")
        print("Audio saved to voice_sample.wav")
        
        # Connect to the database
        conn = connect_to_db()
        if conn:
            cursor = conn.cursor()
            
            # Open the audio file as bytes
            with open("voice_sample.wav", "rb") as f:
                voice_sample = f.read()

            # Insert the user into the database
            try:
                cursor.execute("INSERT INTO users (name, email, password, voice_sample) VALUES (%s, %s, %s, %s)", 
                               (name, email, password, voice_sample))
                conn.commit()
                print("Voice sample stored successfully.")
            except psycopg2.IntegrityError as e:
                print(f"Error: Email {email} already exists. Please log in.")
            except Exception as e:
                print(f"Error inserting user: {e}")
            finally:
                cursor.close()
                conn.close()
    else:
        print("Voice sample recording failed.")

# Function to record audio and save it to a file
def record_voice(file_name):
    try:
        fs = 44100  # Sample rate
        duration = 5  # seconds
        print("Recording...")
        audio_data = sd.rec(int(duration * fs), samplerate=fs, channels=2, dtype='int16')
        sd.wait()
        
        # Save the recorded audio to a .wav file
        with wave.open(file_name, 'wb') as wf:
            wf.setnchannels(2)
            wf.setsampwidth(2)
            wf.setframerate(fs)
            wf.writeframes(audio_data.tobytes())
        
        return file_name
    except Exception as e:
        print(f"Error recording voice: {e}")
        return None

# Function to verify if the spoken voice matches the stored sample
def verify_voice_stored(recorded_voice_file, stored_voice_sample):
    # Create a temporary file for the stored voice sample
    temp_file = tempfile.NamedTemporaryFile(delete=False, suffix=".wav")
    with open(temp_file.name, 'wb') as f:
        f.write(stored_voice_sample)

    if recorded_voice_file and os.path.exists(temp_file.name):
        # Proceed with comparing the two voice samples
        print(f"Stored voice sample found at: {temp_file.name}")
        
        
        # Here we use speech recognition to extract text from both samples and compare them
        recorded_text = recognize_audio(recorded_voice_file)
        stored_text = recognize_audio(temp_file.name)

        if recorded_text and stored_text:
            print(f"Recorded text: {recorded_text}")
            print(f"Stored text: {stored_text}")
            # Compare using fuzzy matching
            similarity = fuzz.ratio(recorded_text, stored_text)
            print(f"Voice match similarity: {similarity}%")
            return similarity > 70  # You can adjust the threshold as necessary
        else:
            print("Could not extract text from one or both of the voice samples.")
            return False
    else:
        print("Voice sample not found or path is invalid.")
        return False

# Function to authenticate the user during login
def login():
    email = input("Enter your email: ")
    password = input("Enter your password: ")

    # Connect to the database
    conn = connect_to_db()
    if conn:
        cursor = conn.cursor()
        cursor.execute("SELECT id, name, email, password, voice_sample FROM users WHERE email = %s", (email,))
        user = cursor.fetchone()
        
        if user and user[3] == password:
            print(f"Welcome {user[1]}! Please speak again for verification.")
            
            # Record the voice for verification
            recorded_voice_file = record_voice("voice_sample_verification.wav")
            
            if recorded_voice_file:
                print("Recording complete.")
                recognized_text = recognize_audio("voice_sample_verification.wav")
                if recognized_text:
                    print(f"Recognized text: {recognized_text}")
                    # Verify the voice
                    if verify_voice_stored(recorded_voice_file, user[4]):
                        print("Voice verified successfully!")
                    else:
                        print("Voice does not match.")
                else:
                    print("Failed to recognize voice.")
            else:
                print("Voice verification failed.")
        else:
            print("Invalid email or password.")
        
        cursor.close()
        conn.close()

# Function to recognize speech from a recorded file
def recognize_audio(file_name):
    recognizer = sr.Recognizer()
    audio_file = sr.AudioFile(file_name)
    
    try:
        with audio_file as source:
            audio_data = recognizer.record(source)
        recognized_text = recognizer.recognize_google(audio_data)
        return recognized_text
    except Exception as e:
        print(f"Error recognizing audio: {e}")
        return None

# Main function to run the program
def main():
    print("1. Register")
    print("2. Login")
    choice = input("Please select an option: ")
    
    if choice == '1':
        register_user()
    elif choice == '2':
        login()
    else:
        print("Invalid option.")

if __name__ == "__main__":
    conn = connect_to_db()
    if conn:
        create_table(conn)
        conn.close()
    main()
