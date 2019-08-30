#pragma once
class SIGSCANER {
private:
	DWORD g_dwImageBase;
	DWORD g_Size;
	DWORD g_DataBase;
public:

	bool IsPeFile(TCHAR * szPath);
	std::string GetSigHex(char * data, int len);
	std::string GetSig(std::string FilePatch);
	bool FindPattern(DWORD Start, DWORD End, std::string pattern);
	//bool FindPattern(DWORD Strat,DWORD End std::string pattern);
	void Work();
};