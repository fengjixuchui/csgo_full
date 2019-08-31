#include "stdafx.h"
#include "unity.h"
void myCALLBACK::OnClientConnected(std::string userKey)
{
	//printf("OnClientConnect Callback! \n");
	//开始安装驱动、etc
	printf(XorString("[SYSTEM]开始首次扫描内存... \n"));
	SigScaner->Work();
}
void myCALLBACK::OnClientLogin(std::string userKey)
{
	//printf("OnClientLogin Callback! userKey: %s \n", userKey.c_str());
	G::UserSecKey = userKey;
	if (!ClientEngine->ConnectAntiCheatServer())
	{
		printf(XorString("[ClientEngine]无法连接到服务端!请检查网络连接! \n"));
		ServerEngine->SentPack(XorString("0x1779"));
		return;
	}
	ServerEngine->SentPack(XorString("0x1778"));
}
void myCALLBACK::OnConnectCSGOServer(std::string ip_port)
{
	//steam://rungame/730/76561202255233023/+connect 43.248.187.18:27018
	printf("OnConnectCSGOServer Callback! IP:port: %s \n", ip_port.c_str());
	std::string Server = XorString("steam://rungame/730/76561202255233023/+connect ") + ip_port;
	ShellExecuteA(NULL, XorString("open"), Server.c_str(), NULL, NULL, SW_SHOWNORMAL);
}