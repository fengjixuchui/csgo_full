from django.http import HttpResponse
from django.shortcuts import render, redirect
from channels.generic.websocket import WebsocketConsumer
import platform
import django
import re
import json
import hashlib
import random
import base64
import time
import datetime
from . import GlobalVar
from . import api_process as api

result = {
    'msgType': 'AC_Login',
    'uFuck': 0,
    'success': 0
}
client = {
    # 'seckey': {
    #    'update_time': '',
    #    'tmp_key':''
    # }
}


def doLogin(secKey):
    check = api.process_getdata_by_key(secKey)
    if not check:
        return result
    result['success'] = 1
    return result


def main(request):
    result_json = result
    if request.GET:
        if 'login' in request.GET:
            result_json = doLogin(request.GET['login'])
    return api.get_json(result_json)
