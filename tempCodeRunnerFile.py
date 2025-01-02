import psycopg2
import sounddevice as sd
import wave
import numpy as np
import os


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
    except psycopg2.Error as e:
        print(f"Error connecting to database: {e}")
        return None


# Function to record the voice sample
def record_voice_sample(filename="voice_sample.wav"):
    fs = 44100  # Sample rate
    duration = 5  # seconds
    print("Recording...")

    # Record audio for the specified duration
    audio_data = sd.rec(int(duration * fs), samplerate=fs, channels=1, dtype='int16')
    sd.wait()  # Wait until the recording is finished

    print("Recording complete.")

    # Save the recorded audio to a WAV file
    with wave.open(filename, "wb") as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)  # 2 bytes per sample
        wf.setframerate(fs)
        wf.writeframes(audio_data.tobytes())

    print(f"Audio saved to {filename}")


# Function to insert the user's data into the database
def insert_user(name, email, password, voice_sample_path):
    try:
        conn = connect_to_db()
        if conn is None:
            return

        cursor = conn.cursor()

        # Read the voice sample file as binary data
        with open(voice_sample_path, 'rb') as file:
            voice_sample = file.read()

        # Insert the user's data into the database
        cursor.execute('''INSERT INTO users (name, email, password, voice_sample)VALUES (%s, %s, %s, %s);''', (name, email, password, voice_sample))

        conn.commit()
        print("User data stored successfully.")

    except psycopg2.Error as e:
        print(f"Error inserting data into the database: {e}")

    finally:
        cursor.close()
        conn.close()


# Function to retrieve the voice sample from the database
def retrieve_voice_sample(email):
    try:
        conn = connect_to_db()
        if conn is None:
            return

        cursor = conn.cursor()

        # Retrieve the voice sample using the email as the identifier
        cursor.execute("SELECT voice_sample FROM users WHERE email = %s;", (email,))
        result = cursor.fetchone()

        if result is None:
            print("No voice sample found for this email.")
            return

        # Extract the voice sample (binary data)
        voice_sample = result[0]

        # Save the voice sample to a .wav file
        with open('retrieved_voice_sample.wav', 'wb') as f:
            f.write(voice_sample)
        print("Voice sample retrieved and saved as 'retrieved_voice_sample.wav'.")

    except psycopg2.Error as e:
        print(f"Error retrieving data from the database: {e}")

    finally:
        cursor.close()
        conn.close()


# Main menu function to interact with the user
def main_menu():
    print("Welcome to the Voice Registration System!")

    # Input user details
    name = input("Enter your name: ")
    email = input("Enter your email: ")
    password = input("Enter your password: ")

    # Record voice sample and save it to a file
    voice_sample_path = "voice_sample.wav"
    record_voice_sample(voice_sample_path)

    # Insert the user's data into the database
    insert_user(name, email, password, voice_sample_path)

    # Optionally, retrieve the voice sample using the email
    retrieve_choice = input("Would you like to retrieve and play your voice sample? (y/n): ")
    if retrieve_choice.lower() == 'y':
        retrieve_voice_sample(email)


if __name__ == "__main__":
    main_menu()
