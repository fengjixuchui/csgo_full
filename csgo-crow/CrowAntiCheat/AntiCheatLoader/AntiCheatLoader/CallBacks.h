#pragma once
class myCALLBACK {
public:
	void OnClientConnected(std::string userKey);
	void OnClientLogin(std::string userKey);
	void OnBindSteamID(std::string userKey);
	void OnConnectCSGOServer(std::string userKey);
	void OnRetryConnect(std::string ip_port);
};