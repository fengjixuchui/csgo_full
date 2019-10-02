#!/usr/bin/python
# -*- coding:utf8 -*-

import json
import datetime

from sqlalchemy import Column, String, Integer, Text, DateTime, create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.ext.declarative import DeclarativeMeta
from sqlalchemy.orm import sessionmaker

from www import  GlobalVar

mysql_conn = 'mysql+mysqlconnector://{0}:{1}@{2}:{3}/{4}'.format(
                GlobalVar.mysql_user, 
                GlobalVar.mysql_password, 
                GlobalVar.mysql_ip,
                GlobalVar.mysql_port,
                GlobalVar.mysql_database)

engine = create_engine(mysql_conn)

Base = declarative_base()

class User(Base):
    __tablename__ = 'user'
    id = Column(String(20), primary_key=True)
    name = Column(String(20))


class CasualServers(Base):
    __tablename__ = 'casualservers'
    serverid = Column(String(20), primary_key=True)
    hostname = Column(String(20))
    ip = Column(String(20))
    port = Column(Integer)
    type = Column(String(20))


class InviteCode(Base):
    __tablename__ = 'invitecode'
    code = Column(String(20), primary_key=True)
    used = Column(Integer)
    name = Column(String(20))


class MatchLog(Base):
    __tablename__ = 'match_log'
    matchid = Column(String(20), primary_key=True)
    red_team = Column(Text)
    blue_team = Column(Text)
    score = Column(String(20))
    server = Column(String(20))
    cheat = Column(String(20))
    demo = Column(Text)


class Matched(Base):
    __tablename__ = 'matched'
    matchid = Column(String(20), primary_key=True)
    red_team_player = Column(Text)
    blue_team_player = Column(Text)
    red_team_score = Column(Integer)
    blue_team_score = Column(Integer)
    cheater = Column(Text)
    serverid = Column(String(20))
    demoid = Column(String(20))
    time = Column(DateTime)
    map = Column(String(20))

class Matching(Base):
    __tablename__ = 'matching'
    matchid = Column(String(20), primary_key=True)
    team_red = Column(Text)
    team_blue = Column(Text)
    team_red_status = Column(Integer)
    team_blue_status = Column(Integer)
    hvh = Column(Integer)
    team_red_players = Column(Text)
    team_blue_players = Column(Text)
    serverid = Column(String(20))
    map = Column(String(20))
    uptime = Column(DateTime)
    connectinfo = Column(Text)


class MatchServer(Base):
    __tablename__ = 'matchserver'
    serverID = Column(String(20), primary_key=True)
    location = Column(String(20))
    group = Column(String(20))
    matching = Column(Integer)
    ip = Column(String(20))
    port = Column(Integer)


class RoomList(Base):
    __tablename__ = 'roomlist'
    RoomID = Column(String(20), primary_key=True)
    PlayerList = Column(Text)
    StartSearch = Column(Integer)
    SearchTime = Column(Integer)
    ingame = Column(Integer)
    PlayerNumber = Column(Integer)
    ReadyNumber = Column(Integer)
    Rank = Column(Integer)
    config = Column(Text)
    public = Column(Integer)


class UserData(Base):
    __tablename__ = 'userdata'
    id = Column(Integer, primary_key=True)
    username = Column(String(20))
    password = Column(String(20))
    email = Column(String(20))
    key = Column(String(20))
    SteamID = Column(String(20))
    matching = Column(String(20))
    roomid = Column(String(20))
    PlayerInfo = Column(String(20))
    LastActivityTime = Column(Integer)
    rank = Column(Integer)
    online = Column(Integer)
    roomconfig = Column(Text)
    banned = Column(Integer)
    data = Column(Text)
    match_ban = Column(String(20))


class AlchemyEncoder(json.JSONEncoder):

    def default(self, obj):
        if isinstance(obj.__class__, DeclarativeMeta):
            # an SQLAlchemy class
            fields = {}
            for field in [x for x in dir(obj) if not x.startswith('_')
                             and x != 'metadata']:
                data = obj.__getattribute__(field)
                try:
                    # this will fail on non-encodable values, like other classes
                    json.dumps(data)    
                    fields[field] = data
                except TypeError:    # 添加了对datetime的处理
                    if isinstance(data, datetime.datetime):
                        fields[field] = data.isoformat()
                    elif isinstance(data, datetime.date):
                        fields[field] = data.isoformat()
                    elif isinstance(data, datetime.timedelta):
                        fields[field] = (datetime.datetime.min +
                                             data).time().isoformat()
                    else:
                        fields[field] = None
            # a json-encodable dict
            return fields
 
        return json.JSONEncoder.default(self, obj)


def get_session():
    DBSession = sessionmaker(bind=engine)
    return DBSession()
