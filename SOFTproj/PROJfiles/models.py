from django.db import models
from django.utils import timezone
from django.core.validators import MinLengthValidator, MaxLengthValidator

class User(models.Model):
    user_id = models.AutoField(primary_key=True)
    username = models.CharField(
        max_length=100, 
        unique=True,
        validators=[MinLengthValidator(7), MaxLengthValidator(25)]  # 
    )
    email = models.EmailField(unique=True)
    password_hash = models.CharField(max_length=64)  # For SHA-256 hash
    created_at = models.DateTimeField(default=timezone.now)
    updated_at = models.DateTimeField(auto_now=True)

    class Meta:
        db_table = 'users'
        indexes = [
            models.Index(fields=['email']),  
            models.Index(fields=['username'])
        ]

    def __str__(self):
        return f"{self.username} ({self.email})"

class VoiceSample(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE, related_name='voice_samples')
    file_path = models.CharField(max_length=255)
    file_name = models.CharField(max_length=255)
    file_size = models.IntegerField()
    mime_type = models.CharField(
        max_length=100,
        default='audio/webm'  # Match the type from your JS recorder
    )
    created_at = models.DateTimeField(default=timezone.now)
    metadata = models.JSONField(default=dict)

    class Meta:
        db_table = 'voice_samples'
        indexes = [
            models.Index(fields=['user', 'created_at']),  # For efficient user queries
        ]
        ordering = ['-created_at']  # Latest samples first

    def __str__(self):
        return f"Voice Sample - {self.file_name} by {self.user.username}"

class FaceSample(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE, related_name='face_samples')
    file_path = models.CharField(max_length=255)
    file_name = models.CharField(max_length=255)
    file_size = models.IntegerField()
    mime_type = models.CharField(
        max_length=100,
        default='image/jpeg'  # Match the type from your JS canvas
    )
    created_at = models.DateTimeField(default=timezone.now)
    metadata = models.JSONField(default=dict)
    sample_number = models.IntegerField(null=True)  # Track the photo number (1-40)

    class Meta:
        db_table = 'face_samples'
        indexes = [
            models.Index(fields=['user', 'created_at']),
            models.Index(fields=['user', 'sample_number'])
        ]
        ordering = ['-created_at']

    def __str__(self):
        return f"Face Sample - {self.file_name} by {self.user.username}"

    @property
    def is_complete_set(self):
        """Check if user has completed their 40 photos"""
        return FaceSample.objects.filter(user=self.user).count() >= 40