from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
import json
import base64
from .database import SupabaseClient
import hashlib
from datetime import datetime
import pytz
from django.shortcuts import render, redirect

# Initialize Supabase client
supabase = SupabaseClient()


def home(request):
    if request.session.get('user_id'):
        return redirect('nd2') 
    return render(request, 'RegisterPageindex.html')

def nd2_page(request):
    if request.session.get('user_id'):
        return redirect('home')
    return render(request, 'index.html')



def hash_password(password: str) -> str:
    """Create a SHA-256 hash of the password"""
    return hashlib.sha256(password.encode()).hexdigest()


@csrf_exempt
def register(request):
    if request.method == "POST":
        try:
            data = json.loads(request.body)
            username = data.get("username")
            email = data.get("email")
            password = data.get("password")

            # Check if email or username already exists
            existing_email = supabase.fetch_specific('users', 'email', {'email': email})
            existing_username = supabase.fetch_specific('users', 'username', {'username': username})

            if existing_email:
                return JsonResponse({"error": "Email already registered"}, status=400)
            if existing_username:
                return JsonResponse({"error": "Username already taken"}, status=400)

            # Create new user record
            user_data = {
                "username": username,
                "email": email,
                "password_hash": hash_password(password),
                "created_at": datetime.now(pytz.UTC).isoformat(),
                "updated_at": datetime.now(pytz.UTC).isoformat()
            }

            user_response = supabase.insert_row('users', user_data)
            user_id = user_response[0]['user_id']

            # Set session
            request.session['user_email'] = email
            request.session['username'] = username
            request.session['user_id'] = user_id

            return JsonResponse({
                "success": True,
                "message": "Registration successful",
                "redirect": "/nd2/"
            })

        except Exception as e:
            return JsonResponse({"error": str(e)}, status=500)


@csrf_exempt
def login(request):
    if request.method == "POST":
        try:
            data = json.loads(request.body)
            email = data.get("email")
            password = data.get("password")

            # Query user with email and hashed password
            user_query = supabase.supabase.table('users').select("*").eq('email', email).eq('password_hash',
                                                                                            hash_password(
                                                                                                password)).execute()

            if user_query.data:
                user = user_query.data[0]
                request.session['user_email'] = email
                request.session['username'] = user['username']
                request.session['user_id'] = user['user_id']
                return JsonResponse({"success": True})
            else:
                return JsonResponse({"error": "Invalid credentials"}, status=401)

        except Exception as e:
            return JsonResponse({"error": str(e)}, status=500)


@csrf_exempt
def save_voice_sample(request):
    if request.method == 'POST':
        try:
            user_id = request.session.get('user_id')
            if not user_id:
                return JsonResponse({'error': 'Not authenticated'}, status=401)

            voice_file = request.FILES.get('file')  # Changed from 'audio' to 'file'
            if not voice_file:
                return JsonResponse({'error': 'No file provided'}, status=400)

            file_name = f"{user_id}/voice/{voice_file.name}"


            # Upload to Supabase Storage
            storage_response = supabase.supabase.storage.from_('voice-samples').upload(
                file_name,
                voice_file.read()
            )


            # Save metadata
            voice_data = {
                "user_id": user_id,
                "file_path": storage_response.path,
                "file_name": voice_file.name,
                "file_size": voice_file.size,
                "mime_type": "audio/webm",
                "created_at": datetime.now(pytz.UTC).isoformat(),
                "metadata": {}  # Empty JSONB object
            }
            supabase.insert_row('voice_samples', voice_data)

            return JsonResponse({"success": True})
        except Exception as e:
            return JsonResponse({'error': str(e)}, status=500)


@csrf_exempt
def save_face_image(request):
    if request.method == 'POST':
        try:
            user_id = request.session.get('user_id')
            if not user_id:
                return JsonResponse({'error': 'Not authenticated'}, status=401)

            data = json.loads(request.body)
            image_data = data.get('image')

            # Convert base64 to binary
            if 'base64,' in image_data:
                image_data = image_data.split('base64,')[1]
            face_data = base64.b64decode(image_data)

            file_name = f"{user_id}/face/{data.get('filename', 'face.jpg')}"
            # Upload to Supabase Storage
            storage_response = supabase.supabase.storage.from_('face-samples').upload(
                file_name,
                face_data
            )

            # Save metadata
            face_data = {
                "user_id": user_id,
                "file_path": storage_response.path,
                "file_name": file_name,
                "file_size": len(face_data),
                "mime_type": "image/jpeg",
                "created_at": datetime.now(pytz.UTC).isoformat(),
                "metadata": {}  # Empty JSONB object
            }
            supabase.insert_row('face_samples', face_data)

            return JsonResponse({"success": True})
        except Exception as e:
            return JsonResponse({'error': str(e)}, status=500)


def get_sample_counts(request):
    user_id = request.session.get('user_id')
    if not user_id:
        return JsonResponse({'error': 'Not authenticated'}, status=401)

    try:
        # Get counts using the created indexes
        voice_samples = supabase.supabase.table('voice_samples').select("*", count="exact").eq('user_id',
                                                                                               user_id).execute()
        face_samples = supabase.supabase.table('face_samples').select("*", count="exact").eq('user_id',
                                                                                             user_id).execute()

        return JsonResponse({
            'voice_samples': len(voice_samples.data),
            'face_samples': len(face_samples.data)
        })
    except Exception as e:
        return JsonResponse({'error': str(e)}, status=500)