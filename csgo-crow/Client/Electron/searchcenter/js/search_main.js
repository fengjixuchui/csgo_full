var is_ingame = false;
var is_insearch = false;
var is_ready = false;
var is_create_room = true;
var search_limit = 0;
var limited = false;
var base = new Base64();
var search_timmer;
var search_time = 0;
var jmp_room = '';
var room_is_public = true;
var kick_shit = false;
var get_matchinfo = false;
var heartbeat_match;
var ip_port = "";
var electron = require('electron');
var timedate = new Date();
var chat_html;
var json_info = {
    'key': electron.remote.getGlobal('sharedObject').UserSecKey,
    'request': "null",
    'other': "null",
    'title': "null",
    'text': "null",
    'ico': 'default.jpg',
    'matchid': '',
    'maps': [],
    'public': 1,
};
// 打开一个 web socket
$(document).ready(function () {
    $('#userLoginName').text(electron.remote.getGlobal('sharedObject').UserName);
    var obj_websocket = new WebSocket("ws://" + electron.remote.getGlobal('sharedObject').Domain + "/websocket/room/");
    if (!obj_websocket) {
        toastr["warning"]("与后方服务器建立通讯失败!请重新进房间!");
        alert('与后方服务器通讯失败!请重进房间!');
        window.location.href = "./Main.html";
        return;
    }
    obj_websocket.onopen = function () {
        //var json_text = '{"key":"' + electron.remote.getGlobal('sharedObject').UserSecKey + '", "request":"get_room_info" , "other":"null"}';
        json_info['request'] = 'get_room_info';
        obj_websocket.send(JSON.stringify(json_info));
        json_info['request'] = 'rand_get_room';
        obj_websocket.send(JSON.stringify(json_info));
        json_info['request'] = 'reflush_room_config';
        obj_websocket.send(JSON.stringify(json_info));
        setInterval(() => {
            obj_websocket.send('ping');
        }, 60000);
    };

    obj_websocket.onmessage = function (evt) {
        if (evt.data == 'pong')
            return;
        var received_msg = evt.data;
        var json_decode = JSON.parse(received_msg)
        //console.log(json_decode)
        if (json_decode.success) {
            if (json_decode.msgType == 'chat_reve') {
                console.log(json_decode.name);
                var msg = json_decode.name;
                if (!chat_html)
                    chat_html = '';
                chat_html += '<div>' + msg + '</div>';
                $('#chat_box').html(chat_html);
                $('#chat_top_model').text(msg.substring(0, 45) + "...");
                return;
            }
            if (json_decode.msgType == 'get_room_info') {
                is_ingame = json_decode.is_ingame;
                is_search = json_decode.is_search;
                //console.log(json_decode.playerlist)
                player_list = JSON.parse(base.decode(json_decode.playerlist));
                if (json_decode.player_num == 1 && is_create_room) {
                    $('#player_name_1').text(electron.remote.getGlobal('sharedObject').UserName);
                    is_create_room = false;
                }
                for (var key_name in player_list) {
                    for (i = 1; i <= 5; i++) {
                        name = "#player_name_" + i;
                        rank = "#player_rank_" + i;
                        ready = "#player_ready_" + i;
                        ico = "#player_ico_" + i;
                        if (is_create_room) {
                            if ($(name).text() == '') {
                                var url = electron.remote.getGlobal('sharedObject').WebsiteDomain + "static/images/players/" + player_list[key_name].ico;
                                $(ico).attr('src', url);
                                $(name).text(key_name);
                                $(rank).text('天梯分[' + player_list[key_name].Rank + ']');
                                //$(ready).text(player_list[key_name].ready ? '[已准备]' : '准备: ?');
                                $(ico).css('box-shadow', player_list[key_name].ready ? '0 5px 20px #ffff33' : '0 5px 20px #33a7ca');
                                break;
                            }
                        } else {
                            if ($(name).text() == key_name) {
                                var url = electron.remote.getGlobal('sharedObject').WebsiteDomain + "static/images/players/" + player_list[key_name].ico;
                                $(ico).attr('src', url);
                                $(rank).text('天梯分[' + player_list[key_name].Rank + ']');
                                //$(ready).text(player_list[key_name].ready ? '[已准备]' : '准备: ?');
                                $(ico).css('box-shadow', player_list[key_name].ready ? '0 5px 20px #ffff33' : '0 5px 20px #33a7ca');
                            }
                        }
                    }
                }
                is_create_room = false;
                if (json_decode.is_search && !is_insearch) {
                    toastr["success"]("所有玩家准备完毕,开始匹配搜索");
                    is_ready = true;
                    is_insearch = true;
                    $("#count_area").show();
                    search_timmer = setInterval(function () {
                        search_time += 1;
                        $('#count_time').text(search_time + " 秒");
                    }, 1000)
                }
                if (!json_decode.is_search && is_insearch) {
                    toastr["info"]("停止搜索");
                    is_insearch = false;
                    clearInterval(search_timmer);
                    search_time = 0;
                    $("#count_area").hide();
                }
                if (is_ingame) {
                    toastr["warning"]("正在游戏中!请点击加入游戏!");
                    $("#searchP").text("加入游戏");
                    json_info['request'] = 'getMatchInfo';
                    obj_websocket.send(JSON.stringify(json_info));
                }
            }
            if (json_decode.msgType == 'match_finifsh') {
                toastr["success"]("比赛结束!玩家分数已经更新");
                finish_match();
                return;
            }
            if (json_decode.msgType == 'server_crash') {
                console.log("finish!!!!");
                toastr["warning"]("比赛服务器崩溃!比赛结束!");
                finish_match();
                return;
            }
            if (json_decode.msgType == 'banned') {
                toastr["error"]("你因为作弊行为已经被封禁!");
                require('electron').ipcRenderer.send('window-close');
                return;
            }
            if (json_decode.msgType == 'get_match_infos') {
                if (get_matchinfo)
                    return;
                if (json_decode.ingame) {
                    is_ingame = true;
                    get_matchinfo = true;
                    json_info['matchid'] = json_decode.matchid;
                    toastr["success"]("比赛地图为: " + json_decode.map + " 服务器地区为: " + json_decode.server_location);
                    ip_port = json_decode.ipaddr + ":" + json_decode.port;
                    window.location.href = "steam://rungame/730/76561202255233023/+connect " + ip_port;
                    $("#searchP").text("加入游戏");
                    $("#searchbutton").attr("href", "steam://rungame/730/76561202255233023/+connect " + ip_port);
                    heartbeat_match = setInterval(() => {
                        json_info['request'] = 'heartbeat_match';
                        obj_websocket.send(JSON.stringify(json_info));
                    }, 30000);
                }
            }
            if (json_decode.msgType == 'get_room_player_number') {
                //console.log(json_decode);
                if (json_decode.uFuck == 1) {
                    toastr["warning"]("房间不存在，可能已解散!");
                    return;
                }
                if (json_decode.number == 5) {
                    toastr["warning"]("房间人已满!");
                    return;
                }
                if (confirm("是否连接房间[" + jmp_room + "]?")) {
                    electron.remote.getGlobal('sharedObject').jmp_room = jmp_room;
                    window.location.href = "./Main.html";
                }
            }

            if (json_decode.msgType == 'do_ready') {
                if (json_decode.uFuck != 2) {
                    if (json_decode.uFuck == 3) {
                        is_ready = false;
                        $("#searchP").text("点击准备");
                        $("#searchbutton")[0].style.backgroundColor = '#33a7ca';
                        toastr["info"]("用户冻结中,还剩余:[" + json_decode.freezetime + "]秒");
                        return;
                    }
                    var tmp_text = is_ready ? "准备成功" : "取消准备";
                    toastr["info"](tmp_text);
                    json_info['request'] = 'get_room_info';
                    obj_websocket.send(JSON.stringify(json_info));
                }
            }
            if (json_decode.msgType == 'player_ready') {
                json_info['request'] = 'get_room_info';
                obj_websocket.send(JSON.stringify(json_info));
            }

            if (json_decode.msgType == 'match_start') {
                is_ingame = true;
                is_insearch = false;
                toastr["success"]("匹配成功!");
                //应该要写一个得到比赛信息之类的
                $("#searchP").text("加入游戏");
                $("#count_area").hide();
                clearInterval(search_timmer);
                json_info['request'] = 'get_room_info';
                obj_websocket.send(JSON.stringify(json_info));
                json_info['request'] = 'getMatchInfo';
                obj_websocket.send(JSON.stringify(json_info));
            }
            if (json_decode.msgType == 'leave_room') {
                toastr["info"]("玩家 " + json_decode.name + " 离开了房间");
                for (i = 1; i <= 5; i++) {
                    name = "#player_name_" + i;
                    rank = "#player_rank_" + i;
                    ready = "#player_ready_" + i;
                    ico = "#player_ico_" + i;
                    if ($(name).text() == json_decode.name) {
                        $(name).text('');
                        $(rank).text('');
                        $(ready).text('');
                        $(ico).css('box-shadow', '');
                        $(ico).attr('src', './searchcenter/images/player_def.png');
                        break;
                    }
                }
                json_info['request'] = 'get_room_info';
                obj_websocket.send(JSON.stringify(json_info));
            }
            if (json_decode.msgType == 'join_room') {
                toastr["info"]("玩家 " + json_decode.name + " 加入了房间");
                for (i = 1; i <= 5; i++) {
                    name = "#player_name_" + i;
                    rank = "#player_rank_" + i;
                    ready = "#player_ready_" + i;
                    ico = "#player_ico_" + i;
                    if ($(name).text() == '') {
                        $(name).text(json_decode.name);
                        break;
                    }
                }
                json_info['request'] = 'get_room_info';
                obj_websocket.send(JSON.stringify(json_info));
            }
            if (json_decode.msgType == 'kick') {
                toastr["warning"]("管理员已将此房间解散!");
                kick_shit = true;
                setTimeout(function () {
                    window.location.href = "./Main.html";
                }, 2 * 1000);
            }
            if (json_decode.msgType == 'up_room_info') {
                toastr["info"](json_decode.name + ' 更新了房间配置!');
                json_info['request'] = 'reflush_room_config';
                obj_websocket.send(JSON.stringify(json_info));
            }
            if (json_decode.msgType == 'reflush_room_config') {
                var decode_config = JSON.parse(json_decode.config);
                json_info['title'] = decode_config['title'];
                json_info['text'] = decode_config['text'];
                json_info['ico'] = decode_config['ico'];
                json_info['maps'] = decode_config['maps'];
                json_info['public'] = decode_config['public'];
                room_is_public = json_info['public'];
                if (room_is_public) {
                    $("#btn_set_room").text("公开");
                    $("#btn_set_room").attr("class", "btn btn-success");
                } else {
                    $("#btn_set_room").text("私有");
                    $("#btn_set_room").attr("class", "btn btn-primary");
                }
                var url = electron.remote.getGlobal('sharedObject').WebsiteDomain + "static/images/room/" + json_info['ico'];
                $("#room_ico").attr('src', url);
                $("input[ name='input_name_of_room' ]").val(json_info['title']);
                $("#input_text_of_room").text(json_info['text']);
                $('#roomname').text(json_info['title'] + "#" + electron.remote.getGlobal('sharedObject').UserRoomID);
                //console.log(decode_config['text']);
                //console.log(decode_config['maps']);
                for (i = 0; i <= json_info['maps'].length - 1; i++) {
                    id = '#switch-' + json_info['maps'][i];
                    $(id).prop('checked', true);
                }
            }
            if (json_decode.msgType == 'get_rand_roomlist') {
                if (json_decode.roomlist == 'null') {
                    $('#room_list').html('空空如也,说明你的房间设置为私有状态了');
                    return;
                } else {
                    var decode_roominfo = JSON.parse(json_decode.roomlist);
                    var index = 0;
                    var html_text_1 = '';
                    var html_text_2 = '';
                    var html_text = '';
                    for (var roomid in decode_roominfo) {
                        var room_ico = decode_roominfo[roomid]['ico']
                        var room_title = decode_roominfo[roomid]['title']
                        var room_text = decode_roominfo[roomid]['text']
                        var room_players = decode_roominfo[roomid]['players']
                        var room_maps = decode_roominfo[roomid]['maps']
                        if (roomid == electron.remote.getGlobal('sharedObject').UserRoomID)
                            room_title = '[*]' + room_title;
                        if (index <= 4) {
                            html_text_1 = html_text_1 + "<div class= \"col-sm-6 col-md-3\"><div class=\"thumbnail\"><img src=\"" + electron.remote.getGlobal('sharedObject').WebsiteDomain + "static/images/room/" + room_ico + "\" width=\“200px\” height=\“200px\” id=\"room_logo\"><div class=\"caption\"><h3>" + room_title + "(" + room_players + "/5)</h3><p>" + room_text + "</p><p><a href=\"#\" class=\"btn btn-default\" onclick=\"join_room(this.id)\" id = \"" + roomid + "\"role=\"button\">加入</a></p></div></div></div>";
                        } else {
                            html_text_2 = html_text_2 + "<div class= \"col-sm-6 col-md-3\"><div class=\"thumbnail\"><img src=\"" + electron.remote.getGlobal('sharedObject').WebsiteDomain + "static/images/room/" + room_ico + "\" width=\“200px\” height=\“200px\” id=\"room_logo\"><div class=\"caption\"><h3>" + room_title + "(" + room_players + "/5)</h3><p>" + room_text + "</p><p><a href=\"#\" class=\"btn btn-default\" onclick=\"join_room(this.id)\" id = \"" + roomid + "\"role=\"button\">加入</a></p></div></div></div>";
                        }
                        index += 1;
                    }
                    if (index <= 4) {
                        html_text = "<div class=\"row\">" + html_text_1 + "</div></div>";
                    } else {
                        html_text = "<div class=\"row\">" + html_text_1 + "</div>";
                        html_text = html_text + "<div class=\"row\">" + html_text_2 + "</div>";
                    }
                    $('#room_list').html(html_text);
                    toastr["success"]("成功更新房间列表!");
                }
            }
        } else {
            toastr["error"]("后方服务器错误!网络不好或者服务器崩溃!");
            console.log(received_msg);
            setTimeout(function () {
                window.location.href = "./Main.html";
            }, 3 * 1000);
        }
    };

    obj_websocket.onclose = function () {
        // 关闭 websocket
        toastr["error"]("后方服务器错误!网络不好或者服务器崩溃!");
        setTimeout(function () {
            window.location.href = "./Main.html";
        }, 3 * 1000);
    };
    $("#refresh_room").click(function () {
        if (kick_shit)
            return;
        json_info['request'] = 'rand_get_room';
        obj_websocket.send(JSON.stringify(json_info));
    })
    $("#searchbutton").click(function () {
        if (is_ingame) {
            window.location.href = "steam://rungame/730/76561202255233023/+connect " + ip_port;
            return;
        }
        if (search_limit > 3) {
            toastr["warning"]("过快的准备会被拉入黑名单永远不能匹配!");
            if (!limited) {
                setTimeout(function () {
                    search_limit = 0;
                    limited = true;
                }, 3000);
            }
            return;
        }
        limited = false;
        search_limit += 1;
        if (is_ingame || kick_shit)
            return;
        is_ready = !is_ready;
        if (is_ready) {
            $("#searchP").text("已准备");
            $("#searchbutton")[0].style.backgroundColor = '#ffff33';
        } else {
            $("#searchP").text("点击准备");
            $("#searchbutton")[0].style.backgroundColor = '#33a7ca';
        }
        json_info['request'] = 'room_do_ready';
        obj_websocket.send(JSON.stringify(json_info));
    })
    $("#btn_set_room").click(function () {
        if (is_ingame || kick_shit)
            return;
        room_is_public = !room_is_public;
        if (room_is_public) {
            $("#btn_set_room").text("公开");
            $("#btn_set_room").attr("class", "btn btn-success");
        } else {
            $("#btn_set_room").text("私有");
            $("#btn_set_room").attr("class", "btn btn-primary");
        }
    })
    $("#room_config_update_room").click(function () {
        if (search_limit > 4) {
            toastr["warning"]("速度太快了,而且这毫无意义!");
            if (!limited) {
                setTimeout(function () {
                    search_limit = 0;
                    limited = true;
                }, 3000);
            }
            return;
        }
        limited = false;
        search_limit += 1;
        if (is_ingame || kick_shit)
            return;
        if ($('#input_name_of_room').val() == '') {
            toastr["warning"]("房间标题为空!");
            return;
        }
        if ($('#input_text_of_room').text() == '') {
            toastr["warning"]("房间介绍为空!");
            return;
        }
        var select_maps = [];
        var map_names = ['de_dust2', 'de_inferno', 'de_nuke', 'de_mirage', 'de_overpass', 'de_cache', 'de_train', 'de_cbble'];
        var found = false;
        for (i = 0; i < map_names.length - 1; i++) {
            if ($("#switch-" + map_names[i]).is(':checked')) {
                select_maps.push(map_names[i]);
                found = true;
            }
        }
        if (!found) {
            toastr["warning"]("至少要选一个地图!");
            return;
        }
        json_info['title'] = $('#input_name_of_room').val();
        json_info['text'] = $('#input_text_of_room').val();
        json_info['public'] = room_is_public;
        json_info['maps'] = select_maps;
        json_info['request'] = 'up_room_infos';
        obj_websocket.send(JSON.stringify(json_info));
        toastr["success"]("成功更新房间设置!");
    })
    join_room = function (id) {
        if (is_ingame || kick_shit)
            return;
        if (id == electron.remote.getGlobal('sharedObject').UserRoomID) {
            toastr["warning"]("不能加入自己的房间!");
            return;
        }
        jmp_room = id;
        json_info['other'] = id;
        json_info['request'] = 'get_room_players_number';
        obj_websocket.send(JSON.stringify(json_info));
    }
    ExitRoomt = function () {
        if (is_ingame || kick_shit)
            return;
        if (is_ready) {
            toastr["warning"]("准备状态不能退出房间!");
            return;
        }
        if (confirm("真的退出房间?")) {
            electron.remote.getGlobal('sharedObject').jmp_room = '';
            electron.remote.getGlobal('sharedObject').UserRoomID = '';
            json_info['request'] = 'exit_room';
            obj_websocket.send(JSON.stringify(json_info));
            window.location.href = "./Main.html";
        }
    }
    finish_match = function () {
        is_ingame = false;
        get_matchinfo = false;
        $("#searchP").text("点击准备");
        $("#searchbutton")[0].style.backgroundColor = '#33a7ca';
        clearInterval(search_timmer);
        $("#count_area").hide();
        $("#searchbutton").attr("href", "#");
        json_info['matchid'] = '';
        clearInterval(heartbeat_match);
        json_info['request'] = 'get_room_info';
        obj_websocket.send(JSON.stringify(json_info));
    }
    $('#upload_image').click(function () {
        var form_data = new FormData();
        var file_info = $('#picture')[0].files[0];
        if (file_info == null) {
            toastr["error"]("请先选择照片");
            return;
        }
        form_data.append('method', 'room_ico');
        form_data.append('file', file_info);
        form_data.append('key', electron.remote.getGlobal('sharedObject').UserSecKey);
        $("#upload_image").attr('disabled', true);
        $("#upload_image").text("上传中");
        //上传到服务器
        $.ajax({
            url: electron.remote.getGlobal('sharedObject').WebsiteDomain + "api/update_image/",
            type: 'POST',
            data: form_data,
            cache: false,
            contentType: false,
            processData: false,
            async: true,
            success: function (data) {
                $("#upload_image").attr('disabled', false);
                $("#upload_image").text("上传");
                if (data.success) {
                    if (data.msgType == 'update_image') {
                        var new_image = data.image;
                        var url = electron.remote.getGlobal('sharedObject').WebsiteDomain + "static/images/room/" + new_image;
                        $("#room_ico").attr('src', url);
                        toastr["success"]("上传背景成功!");
                    } else {
                        toastr["error"]("未知服务器错误!请联系管理员");
                    }
                } else {
                    if (data.uFuck == 1)
                        toastr["error"]("文件大于2M,图片太大了!");
                    else if (data.uFuck == 3)
                        toastr["error"]("警告,请不要上传违法图片!");
                    else
                        toastr["error"]("目前只支持jpg、bmp、png、gif!");
                }
            },
            error: function (e) {
                $("#upload_image").attr('disabled', false);
                $("#upload_image").text("上传");
                toastr["error"]("未知服务器错误!请联系管理员");
            }
        });

    })
    $('#chat_top_model').click(function () {
        $('#chat_menu').modal();
    });
    send_chat = function () {
        var chat_input = $("input[ name='chat_input_msg' ]").val();
        if (chat_input == '')
            return;
        console.log("send:" + chat_input);
        var chat_text = electron.remote.getGlobal('sharedObject').UserName + " " + timedate.toLocaleTimeString() + ":" + chat_input;
        json_info['request'] = 'send_chat';
        json_info['other'] = chat_text;
        obj_websocket.send(JSON.stringify(json_info));
        $("input[ name='chat_input_msg' ]").val('');
        var h = $("input[ name='chat_input_msg' ]").height();
        $("input[ name='chat_input_msg' ]").scrollTop(h);
    };
    $('#chat_send_msg').click(function () {
        send_chat();
    });
    $(document).keyup(function (event) {
        if (event.keyCode == 13) {
            send_chat();
        } else if (event.keyCode == 84) {
            $('#chat_menu').modal();
        }
    });
})