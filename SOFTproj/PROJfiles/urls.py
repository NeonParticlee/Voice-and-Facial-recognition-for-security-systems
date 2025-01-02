from django.urls import include, path
from . import views
from .views import *
from django.conf import settings
from django.conf.urls.static import static


ROOT_URLCONF = 'SOFTproj.urls'


urlpatterns = [
    path('', views.home, name='home'), 
    path('register/', views.register, name='register'),
    path('login/', views.login, name='login'),
    path('nd2/', views.nd2_page, name='nd2'),
    path('api/save-voice/', views.save_voice_sample, name='save_voice'),
    path('api/save-face/', views.save_face_image, name='save_face'),
    path('get-sample-counts/', views.get_sample_counts, name='get_sample_counts'),

]

if settings.DEBUG:
    urlpatterns += static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)





