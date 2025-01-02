from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.views.decorators.http import require_http_methods
from django.middleware.csrf import get_token
from django.shortcuts import render, redirect
from django.contrib.auth import authenticate, login
from django.contrib.auth.models import User





@csrf_exempt
@require_http_methods(["GET", "POST"])
def register(request):
    if request.method == 'POST':
        username = request.POST.get('username')
        password = request.POST.get('password')

        if not username or not password:
            return JsonResponse({"error": "Username and password are required"}, status=400)

        if User.objects.filter(username=username).exists():
            return JsonResponse({"error": "User already exists"}, status=400)

        user = User.objects.create_user(username=username, password=password)
        return redirect('verify', username=user.username)

    # Render the registration page
    return render(request, 'index.html', {"csrf_token": get_token(request)})


@csrf_exempt
@require_http_methods(["GET", "POST"])
def user_login(request):
    if request.method == 'POST':
        username = request.POST.get('username')
        password = request.POST.get('password')

        if not username or not password:
            return JsonResponse({"error": "Username and password are required"}, status=400)

        user = authenticate(request, username=username, password=password)
        if user is not None:
            login(request, user)
            return redirect('verify', username=user.username)
        else:
            return JsonResponse({"error": "Invalid username or password"}, status=401)

    # Render the login page
    return render(request, 'login.html', {"csrf_token": get_token(request)})


@csrf_exempt
@require_http_methods(["GET", "POST"])
def verify(request, username):
    if request.method == 'POST':
        
        voice_sample = request.POST.get('voice_sample')
        face_sample = request.POST.get('face_sample')

        if not voice_sample or not face_sample:
            return JsonResponse({"error": "Verification failed: Missing data"}, status=401)

        # Placeholder for actual voice/face recognition logic
        # Implement real verification here
        return JsonResponse({"message": "Verification successful"})

    # Render verification page
    return render(request, 'index.html', {"username": username, "csrf_token": get_token(request)})