const electron = require('electron');
const md5Hash = require('./js/md5.js');
const app = electron.app;
const BrowserWindow = electron.BrowserWindow;
const ipc = electron.ipcMain;
var net = require('net');
var client = new net.Socket();
var HOST = '127.0.0.1';
var PORT = 5099;
let bind_steamid_window;
var mainWindow = null;
const Store = require('./store.js');
const store = new Store({
  configName: 'crow_csgo',
  defaults: {
    csgo_patch: { patch: '', parameter: '-noforcemaccel -noforcemparms -freq 144 -tickrate 128 -threads 8 -processheap -highpriority -novid' },
    login_info: { name: '', password: '' }
  }
});


app.on('window-all-closed', function () {
  if (process.platform != 'darwin') {
    app.quit();
  }
});
app.on('ready', function () {
  mainWindow = new BrowserWindow({
    width: 1360,
    height: 950,
    frame: false,
    webPreferences: {
      nodeIntegration: true
    }
  });
  mainWindow.loadURL('file://' + __dirname + '/index.html');
  mainWindow.openDevTools();
  mainWindow.on('closed', function () {
    mainWindow = null;
  });
});
process.on('uncaughtException', (err) => {
  console.log('Found A UnknownError! ' + err);
});
ipc.on('window-close', function () {
  app.quit();
})
ipc.on('msg_config', function (event, msg) {
  var data = {
    'msgType': 'get_config',
    'type': 'csgo',
    'data': {}
  }
  var msgtype = msg.msgType;
  var data = msg;
  if (msgtype == 'get_config') {
    data['data'] = store.get('csgo_patch')
    mainWindow.webContents.send('msg_get_config', data);
  }
  if (msgtype == 'get_login_info') {
    data['data'] = store.get('login_info')
    mainWindow.webContents.send('msg_get_login', data);
  }
  if (msgtype == 'set_config')
    store.set('csgo_patch', { patch: data.patch, parameter: data.parameter });
  if (msgtype == 'set_login_info') {
    store.set('login_info', { name: data.name, password: data.password });
  }
})

/*
ipc.on('window-bind_steamid', function () {
  client.write('0x01340' + global.sharedObject.UserSecKey);
})
ipc.on('TCP-Login', function () {
  console.log('TCP LOGIN');
  client.write('0x01338' + global.sharedObject.UserSecKey);
})

ipc.on('TCP-Matching', function () {
  console.log('TCP Matching ipport' + global.sharedObject.MatchIPandPort);
  client.write('0x01339' + global.sharedObject.MatchIPandPort);
})

client.connect(PORT, HOST, function () {
  client.write('0x01337');
});

client.on('data', function (data) {
  //  console.log('DATA: ' + data);
  var password = 'TheSuperDucker_By_Ha4k1r';
  var check_number = data + password.MD5(32);
  client.write(check_number.MD5(32));
  //  console.log('MD5:' + check_number.MD5(32));
  //  client.destroy();
});
client.on('close', function () {
  console.log('Connection closed');
  app.quit();
});*/