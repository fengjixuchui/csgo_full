var FirstCheck = true;
const electron = require("electron");
window.toastr = require('toastr');
toastr.options = {
    "closeButton": false,
    "debug": false,
    "newestOnTop": true,
    "progressBar": true,
    "positionClass": "toast-top-right",
    "preventDuplicates": false,
    "onclick": null,
    "showDuration": "300",
    "hideDuration": "1000",
    "timeOut": "10000",
    "extendedTimeOut": "1000",
    "showEasing": "swing",
    "hideEasing": "linear",
    "showMethod": "fadeIn",
    "hideMethod": "fadeOut"
};
var is_inroom = false;
var is_bind_steamid = false;
var binning_steam = false;
let BrowserWindow = electron.remote.BrowserWindow;
//window.$ = window.jQuery = require('jquery');
$(document).ready(function () {
    $('#userLoginName').html(electron.remote.getGlobal('sharedObject').UserName);
    $('#webSiteName').html(electron.remote.getGlobal('sharedObject').WebSiteName);
    connect_room = function (roomid) {
        if ($("#csgo_patch").text() == '') {
            toastr["error"]("请点击右上角-用户设置中设置CSGO路径!");
            return;
        }
        if (!is_bind_steamid) {
            if (!binning_steam)
                check_steamid();
            return;
        }
        $.ajax({
            type: "GET",
            url: electron.remote.getGlobal('sharedObject').WebsiteDomain + 'api/join_room?key=' + electron.remote.getGlobal('sharedObject').UserSecKey + '&create=false&roomid=' + roomid,
            async: false,
            success: function (result) {
                var data = result;
                if (data.msgType != "JoinRoom") {
                    toastr["error"]("后端服务器异常!请联系管理员!");
                    return;
                }
                if (data.success) {
                    toastr["info"]("房间: " + data.RoomID + "已找到");
                    toastr["success"]("正在连接到: " + data.RoomID);
                    electron.remote.getGlobal('sharedObject').UserRoomID = data.RoomID;
                    electron.remote.getGlobal('sharedObject').jmp_room = '';
                    setTimeout(function () {
                        window.location.href = "./Search.html";
                    }, 2 * 1000);
                } else {
                    if (data.uFuck == 3)
                        toastr["error"]("房间号未找到!");
                    else if (data.uFuck == 4)
                        toastr["error"]("房间已满!");
                    else if (data.uFuck == 6)
                        toastr["error"]("房间已经在游戏中!");
                    return;
                }
            }
        })
    }
    check_steamid = function () {

        $.ajax({
            type: "GET",
            url: electron.remote.getGlobal('sharedObject').WebsiteDomain + 'api/check_steamid?key=' + electron.remote.getGlobal('sharedObject').UserSecKey,
            async: false,
            success: function (result) {
                if (result.success) {
                    toastr["info"]("还没有绑定steamid!开始绑定steamid!");
                    binning_steam = true;
                    let bin_steamid_windows = new BrowserWindow({ width: 800, height: 600 })
                    bin_steamid_windows.removeMenu()
                    bin_steamid_windows.setAlwaysOnTop(true)
                    bin_steamid_windows.center()
                    bin_steamid_windows.loadURL(electron.remote.getGlobal('sharedObject').WebsiteDomain + 'bind_steam/?key=' + electron.remote.getGlobal('sharedObject').UserSecKey);
                    bin_steamid_windows.on("close", function () {
                        bin_steamid_windows = null;
                        binning_steam = false;
                    })
                    bin_steamid_windows.setTitle("空白屏幕请开加速器后重新打开此页面![加速器用模式三或者四]")
                    is_bind_steamid = false;
                    return false;
                }
                is_bind_steamid = true;
            }
        })
    }
    $.ajax({
        type: "GET",
        url: electron.remote.getGlobal('sharedObject').WebsiteDomain + 'api/check_in_room?key=' + electron.remote.getGlobal('sharedObject').UserSecKey,
        async: false,
        success: function (result) {
            //console.log(result);
            var data = result;
            if (data.msgType != "CheckRoom") {
                toastr["error"]("后端服务器异常!请联系管理员!");
                return;
            }
            if (data.success) {
                if (data.uFuck == 2) {
                    electron.remote.getGlobal('sharedObject').UserRoomID = data.RoomID;
                    if (data.ingame) {
                        toastr["info"]("已经在房间[" + data.RoomID + "]比赛中!");
                        toastr["success"]("正在连接到: " + data.RoomID);
                        setTimeout(function () {
                            window.location.href = "./Search.html";
                        }, 2 * 1000);
                        return;
                    }
                    toastr["info"]("已经在房间[" + data.RoomID + "]中");
                    if (confirm("当前已经在房间[" + data.RoomID + "] 中,是否重新连接?")) {
                        is_inroom = true;
                        toastr["success"]("正在连接到: " + data.RoomID);
                        setTimeout(function () {
                            window.location.href = "./Search.html";
                        }, 2 * 1000);
                    } else {
                        $.ajax({
                            type: "GET",
                            url: electron.remote.getGlobal('sharedObject').WebsiteDomain + 'api/exit_room?key=' + electron.remote.getGlobal('sharedObject').UserSecKey,
                            async: false,
                            success: function (result) {
                                var data = result;
                                if (data.success) {
                                    toastr["info"]("成功退出房间: " + electron.remote.getGlobal('sharedObject').UserRoomID);
                                    electron.remote.getGlobal('sharedObject').UserRoomID = "";
                                } else {
                                    toastr["info"]("退出的房间不存在!");
                                }
                            }
                        })
                    }
                } else if (data.uFuck == 3) {
                    toastr["error"]("该账户已经被封禁!");
                    electron.ipcRenderer.send('window-close');
                    return;
                }
                if (electron.remote.getGlobal('sharedObject').jmp_room != '') {
                    console.log('jmp:' + electron.remote.getGlobal('sharedObject').jmp_room);
                    connect_room(electron.remote.getGlobal('sharedObject').jmp_room);
                }
            } else {
                toastr["error"]("安全码异常!请联系管理员!");
            }
        }
    });
    $("#btn_exit").click(function () {
        toastr["success"]("退出中...");
        setTimeout(function () {
            var ipc = electron.ipcRenderer;
            ipc.send('window-close');
        }, 2 * 1000);
    })
    $("#btn_check_room").click(function () {
        if (is_inroom) {
            toastr["error"]("已经在房间" + electron.remote.getGlobal('sharedObject').UserRoomID + "中");
            return;
        }
        if ($("#csgo_patch").text() == '') {
            toastr["error"]("请点击右上角-用户设置中设置CSGO路径!");
            return;
        }
        if (!is_bind_steamid) {
            if (!binning_steam)
                check_steamid();
            return;
        }

        $.ajax({
            type: "GET",
            url: electron.remote.getGlobal('sharedObject').WebsiteDomain + 'api/join_room?key=' + electron.remote.getGlobal('sharedObject').UserSecKey + '&create=true&roomid=huoji',
            async: false,
            success: function (result) {
                var data = result;
                if (data.msgType != "JoinRoom") {
                    toastr["error"]("后端服务器异常!请联系管理员!");
                    return;
                }
                if (data.success) {
                    toastr["info"]("房间: " + data.RoomID + "已创建");
                    toastr["success"]("正在连接到: " + data.RoomID);
                    electron.remote.getGlobal('sharedObject').UserRoomID = data.RoomID;
                    is_inroom = true;
                    setTimeout(function () {
                        window.location.href = "./Search.html";
                    }, 2 * 1000);
                } else {
                    toastr["error"]("安全码异常!请联系管理员!");
                }
            }
        })
    })
    $("#btn_join_room").click(function () {
        connect_room($("input[ name='Room_number' ]").val());
    })

    $("#save_config").click(function () {
        $("#csgo_patch").text('D:\\STEAM\\csgo.exe');
        if ($("#csgo_patch").text() == '') {
            toastr["error"]("CSGO路径不能为空!");
            return;
        }
        msg['msgType'] = 'set_config';
        msg['patch'] = $("#csgo_patch").text();
        msg['parameter'] = $("#csgo_parameter").val();
        ipc.send('msg_config', msg);
        electron.remote.getGlobal('sharedObject').csgo_patch = msg['patch'];
        electron.remote.getGlobal('sharedObject').csgo_parameter = msg['parameter'] + " -insecure";
        $.ajax({
            type: "GET",
            url: electron.remote.getGlobal('sharedObject').WebsiteDomain + 'api/set_music?key=' + electron.remote.getGlobal('sharedObject').UserSecKey,
            data: { 'musicid': $("input[ name='player_music' ]").val() },
            async: true,
            success: function (result) {
                if (!result.success) {
                    toastr["error"]("音乐id必须是纯数字!!");
                }
            }
        });
        toastr["success"]("保存设置成功!");
    })
    $('#help_ass').click(function () {
        let btn_help_ass = new BrowserWindow({ width: 1024, height: 800 })
        btn_help_ass.removeMenu()
        btn_help_ass.setAlwaysOnTop(true)
        btn_help_ass.center()
        btn_help_ass.loadURL("https://zhidao.baidu.com/question/1514998485182699540.html");
        btn_help_ass.on("close", function () {
            btn_help_ass = null;
        })
    })
    $('#MyInfo').click(function () {
        let bin_get_info = new BrowserWindow({ width: 1300, height: 910 })
        bin_get_info.removeMenu()
        bin_get_info.setAlwaysOnTop(true)
        bin_get_info.center()

        bin_get_info.loadURL(electron.remote.getGlobal('sharedObject').WebsiteDomain + 'player/' + electron.remote.getGlobal('sharedObject').UserName);
        bin_get_info.on("close", function () {
            bin_get_info = null;
        })
        bin_get_info.setTitle("个人战绩")
    })
    $('#upload_image').click(function () {
        var form_data = new FormData();
        var file_info = $('#picture')[0].files[0];
        if (file_info == null) {
            toastr["error"]("请先选择头像");
            return;
        }
        $("#upload_image").attr('disabled', true);
        $("#upload_image").text("上传中");
        form_data.append('method', 'player_avater');
        form_data.append('file', file_info);
        form_data.append('key', electron.remote.getGlobal('sharedObject').UserSecKey);
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
                        var url = electron.remote.getGlobal('sharedObject').WebsiteDomain + "static/images/players/" + new_image;
                        $("#player_ico").attr('src', url);
                        $("#menu-logo").attr('src', url);
                        toastr["success"]("上传头像成功!");
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
    $.ajax({
        type: "GET",
        url: electron.remote.getGlobal('sharedObject').WebsiteDomain + 'api/api_get_by_key?key=' + electron.remote.getGlobal('sharedObject').UserSecKey,
        async: true,
        success: function (result) {
            var data = result;
            if (data.msgType != "get_player") {
                toastr["error"]("后端服务器异常!请联系管理员!");
                return;
            }
            if (data.success) {
                var url = electron.remote.getGlobal('sharedObject').WebsiteDomain + "static/images/players/" + data.ico;
                $("#player_ico").attr('src', url);
                $("#menu-logo").attr('src', url);
                //$('#username-rank').text(electron.remote.getGlobal('sharedObject').UserName + "#" + data.rank);
            } else {
                toastr["error"]("安全码异常!请联系管理员!");
            }
        }
    })
    $.ajax({
        type: "GET",
        url: electron.remote.getGlobal('sharedObject').WebsiteDomain + 'api/get_player?name=' + electron.remote.getGlobal('sharedObject').UserName,
        async: true,
        success: function (result) {
            if (result.success) {
                var musicid = result.info.music;
                $("input[ name='player_music' ]").val(musicid);
            }
        }
    });

    var ipc = electron.ipcRenderer;
    var msg = {
        'msgType': 'get_config',
        'patch': '',
        'parameter': ''
    }
    check_steamid();
    ipc.send('msg_config', msg);
    electron.ipcRenderer.on('msg_get_config', function (event, message) {
        var data = message.data;
        var parameter = data.parameter;
        var patch = data.patch;
        /*
        if (patch == '')
            $("#auto_get_csgo_patch").show();
        else
            $("#auto_get_csgo_patch").hide();*/
        $("#csgo_patch").text(patch);
        $("#csgo_parameter").val(parameter);
        electron.remote.getGlobal('sharedObject').csgo_patch = patch;
        electron.remote.getGlobal('sharedObject').csgo_parameter = parameter;
        if (patch == '') {
            setTimeout(() => {
                $("#user-set").click();
            }, 4300);
        }
    });
})