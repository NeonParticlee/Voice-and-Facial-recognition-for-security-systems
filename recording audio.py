import psycopg2
import pickle
import torch

# Database connection setup
def connect_to_db():
    return psycopg2.connect(
        host="localhost",
        database="my_database",
        user="farah",
        password="postgres"
    )

def create_table():
    with connect_to_db() as conn:
        with conn.cursor() as cursor:
            cursor.execute(
                """
                CREATE TABLE IF NOT EXISTS users (
                    id SERIAL PRIMARY KEY,
                    name VARCHAR(255) NOT NULL,
                    embedding BYTEA NOT NULL
                );
                """
            )
        conn.commit()

# Save embedding to PostgreSQL
def save_user_to_db(name, embedding):
    embedding_blob = pickle.dumps(embedding.numpy())
    with connect_to_db() as conn:
        with conn.cursor() as cursor:
            cursor.execute(
                "INSERT INTO users (name, embedding) VALUES (%s, %s)",
                (name, embedding_blob)
            )
        conn.commit()
    print(f"User {name} saved to database.")

# Load embeddings from PostgreSQL
def load_users_from_db():
    users = {}
    with connect_to_db() as conn:
        with conn.cursor() as cursor:
            cursor.execute("SELECT name, embedding FROM users")
            for name, embedding_blob in cursor.fetchall():
                embedding = torch.tensor(pickle.loads(embedding_blob))
                users[name] = embedding
    return users
