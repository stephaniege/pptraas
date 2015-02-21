from django.conf.urls import patterns, url, include

from django.views.generic import TemplateView
from raas import views

urlpatterns = patterns('',
           url(r'^request/$', views.request_channel),
           url(r'^join/$', views.join_channel),
           url(r'^buttons/$', views.buttons),
           url(r'^validate/$', views.validate_connection),
           url(r'^next/$', views.next_slide),
           url(r'^previous/$', views.previous_slide),
           )
