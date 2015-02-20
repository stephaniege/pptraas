from django.db import models
from django import forms

class ChannelForm(forms.Form):
    code1 = forms.IntegerField(min_value=0, max_value=9, label='code1')
    code2 = forms.IntegerField(min_value=0, max_value=9, label='code2')
    code3 = forms.IntegerField(min_value=0, max_value=9, label='code3')
    code4 = forms.IntegerField(min_value=0, max_value=9, label='code4')
