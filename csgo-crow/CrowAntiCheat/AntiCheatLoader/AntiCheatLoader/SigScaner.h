#pragma once
class SIGSCANER {
private:
	DWORD64 g_dwImageBase;
	DWORD64 g_Size;
	DWORD64 g_DataBase;
public:

	bool IsPeFile(TCHAR * szPath);
	std::string GetSigHex(char * data, int len);
	std::string GetSig(std::string FilePatch,std::string sig = std::string());
	bool FindSig(std::string sig);
	bool FindPattern(DWORD Start, DWORD End, std::string pattern);
	//bool FindPattern(DWORD Strat,DWORD End std::string pattern);
	void Work();
};