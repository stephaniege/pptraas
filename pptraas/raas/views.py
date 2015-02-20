from django.shortcuts import render
from django.http import HttpResponse

from django.template import RequestContext, loader
from random import randint
from sets import Set

# used by desktop PPT to obtain a four-digit channel code
channels = Set()

def request_channel(request):
    template = loader.get_template('pair.html')
    generated_channel = randint(1000, 4000) # do we want to handle 4 digit numbers that start w/0
    while generated_channel in channels:
        generated_channel = randint(1000, 4000)
    channels.add(generated_channel)
    context = RequestContext(request, {
                'channel': str(generated_channel),
                    })
    return HttpResponse(template.render(context))

