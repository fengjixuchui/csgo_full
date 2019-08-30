#include "stdafx.h"
#include "unity.h"
#include <time.h>
void TCPCLIENT::ReportCheat(CheatReport cheat)
{
	std::string ReportMSG = XorString("0x4000") + std::to_string(cheat.report_id) + "|"
		+ myTools->DwordToString(cheat.report_process_id) + "|"
		+ myTools->DwordToString(cheat.report_base_address) + "|"
		+ myTools->DwordToString(cheat.report_region_size) + "|"
		+ cheat.report_other_data + "|"
		+ cheat.report_sig;
	//printf("Report Msg: %s \n", ReportMSG.c_str());
	ReportMSG = myTools->Base64Encode(ReportMSG);
	SendPacket(ReportMSG,0);
	//send(G::hSocket, ReportMSG.c_str(), ReportMSG.length(), 0);
}
bool TCPCLIENT::SendPacket(std::string buf, int flags)
{
	std::string msg = myTools->EncryptionAES(buf);
	printf("\n EncryptionAES Msg: %s \n", msg.c_str());
	return send(G::hSocket, msg.c_str(), msg.length(), 0);
}
std::string TCPCLIENT::RecvPacket(int flags)
{
	char message[128];
	recv(G::hSocket, message, 128, flags);
	std::string msg = message;
	//printf("message  %s \n", msg.c_str());
	return myTools->DecryptionAES(msg);
}
bool TCPCLIENT::ConnectAntiCheatServer()
{	
	//ip
	G::AntiCheatServerIP = "127.0.0.1";
	G::AntiCheatServerPort = 6666;
	WSADATA wsaData;	
	SOCKADDR_IN servAddr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		printf("[TCPClient]WSAStartup() error!");
	G::hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (G::hSocket == INVALID_SOCKET)
		printf("[TCPClient]hSocket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(G::AntiCheatServerIP.c_str());
	servAddr.sin_port = htons(G::AntiCheatServerPort);
	printf("[TCPClient]Connecting %s:%d \n", G::AntiCheatServerIP.c_str(), G::AntiCheatServerPort);
	if (connect(G::hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		printf("[TCPClient]Connect Error! %d \n", GetLastError());
		return false;
	}
	std::string CRCmsg = "0x6666";
	SendPacket(CRCmsg.c_str(), 0);
	std::string hashed = RecvPacket(0);
	//"0x1337_huoji" Õ®—∂√ÿ‘ø
	std::string myKey = T::GetMd5Hash(hashed + XorString("0x1337_huoji"));
	SendPacket(myKey,0);
	hashed = RecvPacket(0).substr(0, 5);
	//printf("Server Msg: %s \n", hashed.c_str());
	if (hashed.compare(XorString("TRUST")) == 0)
	{
		printf("[TCPClient]Success Connect Server! \n");
		//closesocket(hSocket);
		//WSACleanup();
		return true;
	}
	closesocket(G::hSocket);
	WSACleanup();
	return false;
}
