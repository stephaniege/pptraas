import json

from django.template import RequestContext, loader
from django.shortcuts import render
from django.http import JsonResponse, HttpResponseRedirect, HttpResponse
from random import randint
from sets import Set
from models import ChannelForm


channels = Set()

#### API

# used by desktop PPT to obtain a four-digit channel code
def request_channel(request):
    generated_channel = randint(1000, 4000) # do we want to handle 4 digit numbers that start w/0
    while generated_channel in channels:
        generated_channel = randint(1000, 4000)
    channels.add(generated_channel) # add this generated channel to our set of channels
    return JsonResponse({'channel':str(generated_channel)})


def validate_connection(request):
    if request.method == 'POST':
        response = json.loads(request.body) #TODO: does this actually work lol
        if u'code1' and u'code2' and u'code3' and u'code4' in response:
            code = str(response[u'code1']) + str(response[u'code2']) + str(response[u'code3']) + str(response[u'code4']) 
            if code in channels: # this channel must be open to a desktop presentation.
                return JsonResponse({'channel':code})
    return JsonResponse({'channel':"0000"}) # failure code

##### END API METHODS


# used by browser to connect to given channel
def join_channel(request, *args, **kwargs):
    if request.method == 'POST':
        response = request.POST
        if u'code1' and u'code2' and u'code3' and u'code4' in response:
            code = str(response[u'code1']) + str(response[u'code2']) + str(response[u'code3']) + str(response[u'code4']) 
            ##### TODO: Uncomment this for production
            #if code not in channels: # this channel is not open to a desktop presentation.
            #    return HttpResponseRedirect('/raas/join/') # redirect to same page
            return HttpResponseRedirect('/raas/buttons/') # redirect to buttons page
    else:
        form = ChannelForm()
    return render(request, "derp.html", {'form': form})

def buttons(request):
    template = loader.get_template('buttons.html')
    return HttpResponse(template.render(RequestContext(request, {})))
