from django.conf.urls import patterns, url, include

from raas import views

urlpatterns = patterns('',
            url(r'^pair/$', views.pair, name='pair'),
            )
