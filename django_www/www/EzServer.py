#!/usr/bin/python
# -*- coding:utf8 -*-

import time
import json
import objects
import hashlib
import random
import threading
import base64
import redis

from objects import MatchLog
from objects import Matched
from objects import Matching
from objects import MatchServer
from objects import RoomList
from objects import UserData
from objects import AlchemyEncoder
from www import  GlobalVar

redis_server = redis.Redis(host=GlobalVar.redis_ip, port=GlobalVar.redis_port,
                           password=GlobalVar.redis_password)

RANK_STEP = 5

session = objects.get_session()

def get_teams_and_players(rooms):
    team_red = []
    team_blue = []
    red_players = []
    blue_players = []
    for room in rooms:
        if json.loads(room.config)['team'] == 'red':
            team_red.append(room)
            red_players += json.loads(room.PlayerList)
        else:
            team_blue.append(room)
            blue_players += json.loads(room.PlayerList)
    data = {'team_red': team_red, 
            'team_blue': team_blue,
            'red_players': red_players,
            'blue_players': blue_players}

    return data

def save_result(match_result, match_server, match_map):
    time_tick = str(time.time()).replace('.', '')
    match_id = hashlib.md5(str(time_tick + str(match_result)).encode(
        encoding='UTF8')).hexdigest()
    match_datas = get_teams_and_players(match_result)

    connectinfo = {}
    # update player status
    player_list = match_datas['red_players'] + match_datas['blue_players']
    players = session.query(UserData).all()
    for user in players:
        if user.username in player_list:
            user.matching = 1
            connectinfo[user.username] = user.SteamID
    
    # 写入match_data
    match = Matching(matchid=match_id,
        team_red=json.dumps(match_datas['team_red'], cls=AlchemyEncoder),
        team_blue=json.dumps(match_datas['team_blue'], cls=AlchemyEncoder),
        team_red_players=json.dumps(match_datas['red_players'],
                                    cls=AlchemyEncoder),
        team_blue_players=json.dumps(match_datas['blue_players'],
                                     cls=AlchemyEncoder),
        serverid=match_server.serverID, map=match_map,
        connectinfo=json.dumps(connectinfo))

    session.add(match)

    # update match server status
    match_server.matching = '1'

    json_msg = {
        'msgType': 'match_start',
        'name': '',
        'roomid': ''
    }

    # udpate room status
    for room in match_result:
        room.ingame = '1'
        json_msg['roomid'] = room.RoomID
        json_encode = base64.b64encode(json.dumps(json_msg).encode(
                          encoding='UTF8'))
        redis_connect = redis_server
        redis_connect.publish('room', json_encode)

    session.commit()

def match_proc():
    init_match = True
    while True:
        matchpool = session.query(RoomList).filter(
            RoomList.StartSearch == '1').order_by('SearchTime').all()
        if init_match:
            root_room = matchpool.pop(0)
            root_config = json.loads(root_room.config)
            maps = root_config['maps']
            root_config['team'] = 'red'
            root_room.config = json.dumps(root_config)
            start_rank = int(root_room.Rank)
            end_rank = int(root_room.Rank) + 1
            red_team_need = 5 - int(root_room.PlayerNumber)
            blue_team_need = 5
            root_room.StartSearch = '0'
            searched_rooms = [root_room]
            init_match = False
            session.commit()
        
        start_rank = start_rank - RANK_STEP
        end_rank = end_rank + RANK_STEP
        
        for room in matchpool:
            if int(room.Rank) not in range(start_rank, end_rank):
                continue
    
            room_config = json.loads(room.config)
            room_maps = room_config['maps']
            tmp_maps = list(set(maps).intersection(set(room_maps)))
            if not tmp_maps:
                continue
            else:
                maps = tmp_maps
        
            player_num = int(room.PlayerNumber)
            if player_num <= red_team_need:
                room_config['team'] = 'red'
                room.config = json.dumps(room_config)
                red_team_need -= player_num
                room.StartSearch = '0'
                searched_rooms.append(room)
                session.commit()
            elif player_num <= blue_team_need:
                room_config['team'] = 'blue'
                room.config = json.dumps(room_config)
                blue_team_need -= player_num
                room.StartSearch = '0'
                searched_rooms.append(room)
                session.commit()
    
        
        searched_players = sum(int(x.PlayerNumber) for x in searched_rooms)
        if searched_players == 10:
            match_server = session.query(MatchServer).filter(
                MatchServer.matching == '0').first()
            if not match_server:
                time.sleep(10)
                continue
            match_map = random.choice(maps)
            save_result(searched_rooms, match_server, match_map)
            init_match = True
    
        time.sleep(1)

def start_server():
    threading.Thread(target=match_proc, args=()).start()

if __name__ == '__main__':
    start_server()