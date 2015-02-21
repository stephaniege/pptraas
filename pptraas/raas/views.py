import json
import pusher

from django.template import RequestContext, loader
from django.shortcuts import render
from django.http import JsonResponse, HttpResponseRedirect, HttpResponse
from django.views.decorators.csrf import csrf_exempt
from random import randint
from sets import Set
from models import ChannelForm
from collections import defaultdict


channels = {} # channels is a dictionary of {IP Address:channel code}

#### API

def get_client_ip(request):
    x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
    if x_forwarded_for:
        ip = x_forwarded_for.split(',')[0]
    else:
        ip = request.META.get('REMOTE_ADDR')
    return ip

# used by desktop PPT to obtain a four-digit channel code
def request_channel(request):
    generated_channel = randint(1000, 4000) # do we want to handle 4 digit numbers that start w/0
    while generated_channel in channels.values():
        generated_channel = randint(1000, 4000)
    channels[get_client_ip(request)] = generated_channel # add this generated channel to channels
    return JsonResponse({'channel':str(generated_channel)})

@csrf_exempt
def validate_connection(request):
    if request.method == 'POST':
        response = json.loads(request.body) #TODO: does this actually work lol
        if u'code1' and u'code2' and u'code3' and u'code4' in response:
            code = str(response[u'code1']) + str(response[u'code2']) + str(response[u'code3']) + str(response[u'code4']) 
            if code in channels.values(): # this channel must be open to a desktop presentation.
                channels[get_client_ip(request)] = code
                return JsonResponse({'channel':code})
    return JsonResponse({'channel':"0000"}) # failure code

def send_message_to_pusher(request, message):
    p = pusher.Pusher(
            app_id='107202',
            key='9e5d1eb48fe24c7db157',
            secret='4ea7611fd7e5b0a20cb6'
            )
    channel = channels[get_client_ip(request)]
    p[channel].trigger('my_event', {'message': message})
    return JsonResponse({'channel':channel})

def next_slide(request):
    send_message_to_pusher(request, "next")

def previous_slide(request):
    send_message_to_pusher(request, "previous")


##### END API METHODS


# used by browser to connect to given channel
def join_channel(request):
    if request.method == 'POST':
        response = request.POST
        if u'code1' and u'code2' and u'code3' and u'code4' in response:
            code = str(response[u'code1']) + str(response[u'code2']) + str(response[u'code3']) + str(response[u'code4']) 
            ##### TODO: Uncomment this for production
            #if code not in channels: # this channel is not open to a desktop presentation.
            #    return HttpResponseRedirect('/raas/join/') # redirect to same page
            channels[get_client_ip(request)] = code
            return HttpResponseRedirect('/raas/buttons/') # redirect to buttons page -> when we redirect to buttons page, we know we are trying to connect to a valid channel. on the buttons page, we can then include the js for connecting to the pusher
    else:
        form = ChannelForm()
    return render(request, "pair.html", {'form': form})

def buttons(request):
    template = loader.get_template('next_prev.html')
    return HttpResponse(template.render(RequestContext(request, {})))

def index(request):
    template = loader.get_template('index.html')
    return HttpResponse(template.render(RequestContext(request, {})))
