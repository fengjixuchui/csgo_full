#pragma once
class TCPSERVER {
public:
	std::string Password = XorString("TheSuperDucker_By_Ha4k1r");
	std::string Check_number;
	bool InitGlobalVars();
	void SentPack(std::string pack);
	bool CreateTCPserver();

};