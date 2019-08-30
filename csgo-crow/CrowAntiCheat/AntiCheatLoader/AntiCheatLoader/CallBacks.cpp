#include "stdafx.h"
#include "unity.h"
void myCALLBACK::OnClientConnected(std::string userKey)
{
	printf("OnClientConnect Callback! \n");
	//开始安装驱动、etc
}
void myCALLBACK::OnClientLogin(std::string userKey)
{
	printf("OnClientLogin Callback! userKey: %s \n", userKey.c_str());
	G::UserSecKey = userKey;
}
void myCALLBACK::OnBindSteamID(std::string userKey)
{
	printf("OnBindSteamID Callback! userKey: %s \n", userKey.c_str());
	G::UserSecKey = userKey;
	G::WebSiteUrl = XorString("http://huoji.com/index.php/index/index/bind_steamid?secKey=") + G::UserSecKey;
	ShellExecuteA(NULL, XorString("open"), G::WebSiteUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
void myCALLBACK::OnConnectCSGOServer(std::string ip_port)
{
	//steam://rungame/730/76561202255233023/+connect 43.248.187.18:27018
	printf("OnConnectCSGOServer Callback! IP:port: %s \n", ip_port.c_str());
	std::string Server = XorString("steam://rungame/730/76561202255233023/+connect ") + ip_port;
	ShellExecuteA(NULL, XorString("open"), Server.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
