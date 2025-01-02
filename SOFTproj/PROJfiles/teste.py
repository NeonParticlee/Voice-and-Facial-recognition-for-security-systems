from database import SupabaseClient

def seed_database():
    client = SupabaseClient()

    # Test data to seed the database
    test_users = [
        {
            "first_name": "Test",
            "last_name": "User1",
            "email": "test.user1@example.com",
            "password_hash": "hashedpassword1",
            "phone_number": "1234567890",
            "address": "123 Test Street",
            "city": "Testville",
            "state": "Testland",
            "country": "Testonia",
            "postal_code": "12345",
            "date_of_birth": "1990-01-01",
            "gender": "Male",
            "profile_picture_url": "http://example.com/profile1.jpg",
            "disability_type": "None",
            "specific_disability_details": None,
            "accessibility_preferences": None,
            "preferred_learning_style": "Visual",
            "assistive_technologies_used": None,
            "user_role": "Student",
            "account_status": "Active"
        },
        {
            "first_name": "Test",
            "last_name": "User2",
            "email": "test.user2@example.com",
            "password_hash": "hashedpassword2",
            "phone_number": "0987654321",
            "address": "456 Example Lane",
            "city": "Sample City",
            "state": "Example State",
            "country": "Samplestan",
            "postal_code": "67890",
            "date_of_birth": "1985-05-15",
            "gender": "Female",
            "profile_picture_url": "http://example.com/profile2.jpg",
            "disability_type": "Visual Impairment",
            "specific_disability_details": "Colorblind",
            "accessibility_preferences": "High Contrast",
            "preferred_learning_style": "Auditory",
            "assistive_technologies_used": "Screen Reader",
            "user_role": "Instructor",
            "account_status": "Active"
        },
        {
            "first_name": "Alice",
            "last_name": "Smith",
            "email": "alice.smith@example.com",
            "password_hash": "hashedpassword3",
            "phone_number": "1122334455",
            "address": "789 Alice Road",
            "city": "Wonderland",
            "state": "Fantasy",
            "country": "Imaginaria",
            "postal_code": "54321",
            "date_of_birth": "2000-10-25",
            "gender": "Female",
            "profile_picture_url": "http://example.com/profile3.jpg",
            "disability_type": "None",
            "specific_disability_details": None,
            "accessibility_preferences": "Dark Mode",
            "preferred_learning_style": "Kinesthetic",
            "assistive_technologies_used": None,
            "user_role": "Student",
            "account_status": "Active"
        },
        {
            "first_name": "Bob",
            "last_name": "Johnson",
            "email": "bob.johnson@example.com",
            "password_hash": "hashedpassword4",
            "phone_number": "2233445566",
            "address": "987 Bob Street",
            "city": "Bobville",
            "state": "Bobland",
            "country": "Bobistan",
            "postal_code": "65432",
            "date_of_birth": "1975-07-14",
            "gender": "Male",
            "profile_picture_url": "http://example.com/profile4.jpg",
            "disability_type": "Hearing Impairment",
            "specific_disability_details": "Partial Hearing Loss",
            "accessibility_preferences": "Subtitles",
            "preferred_learning_style": "Reading",
            "assistive_technologies_used": "Hearing Aid",
            "user_role": "Instructor",
            "account_status": "Active"
        },
        {
            "first_name": "Charlie",
            "last_name": "Brown",
            "email": "charlie.brown@example.com",
            "password_hash": "hashedpassword5",
            "phone_number": "3344556677",
            "address": "321 Peanuts Avenue",
            "city": "Cartoon City",
            "state": "Animation State",
            "country": "Cartoonia",
            "postal_code": "76543",
            "date_of_birth": "1995-03-20",
            "gender": "Male",
            "profile_picture_url": "http://example.com/profile5.jpg",
            "disability_type": "None",
            "specific_disability_details": None,
            "accessibility_preferences": "Light Mode",
            "preferred_learning_style": "Visual",
            "assistive_technologies_used": None,
            "user_role": "Student",
            "account_status": "Active"
        },
        {
            "first_name": "Dana",
            "last_name": "White",
            "email": "dana.white@example.com",
            "password_hash": "hashedpassword6",
            "phone_number": "4455667788",
            "address": "567 Dana Lane",
            "city": "Manager City",
            "state": "Admin State",
            "country": "Workland",
            "postal_code": "87654",
            "date_of_birth": "1980-08-05",
            "gender": "Female",
            "profile_picture_url": "http://example.com/profile6.jpg",
            "disability_type": "Motor Impairment",
            "specific_disability_details": "Limited Hand Movement",
            "accessibility_preferences": "Keyboard Navigation",
            "preferred_learning_style": "Auditory",
            "assistive_technologies_used": "Voice Recognition Software",
            "user_role": "Admin",
            "account_status": "Active"
        }
    ]

    try:
        # Insert test users into the database
        inserted_rows = client.bulk_insert("users", test_users)
        print(f"Inserted {len(inserted_rows)} test users into the database.")
    except Exception as e:
        print("Failed to seed the database:", e)


if __name__ == "__main__":
    seed_database()