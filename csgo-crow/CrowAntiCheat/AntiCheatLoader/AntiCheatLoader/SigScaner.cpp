#include "stdafx.h"
#include "unity.h"
//#include <map>
#include <TlHelp32.h>
#include <time.h>
#define MY_IMAGE_FIRST_SECTION_32( ntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((ULONG_PTR)(ntheader) +                                            \
     FIELD_OFFSET( IMAGE_NT_HEADERS32, OptionalHeader ) +                 \
     ((ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))
#define MY_IMAGE_FIRST_SECTION_64( ntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((ULONG_PTR)(ntheader) +                                            \
     FIELD_OFFSET( IMAGE_NT_HEADERS64, OptionalHeader ) +                 \
     ((ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))


bool SIGSCANER::IsPeFile(TCHAR* szPath)
{
	BOOL bSuccess = TRUE;
	//1 将PE文件读取到内存
	HANDLE hFile = CreateFile(
		szPath,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL
		, NULL
	);
	DWORD dwSize = GetFileSize(hFile, NULL);
	DWORD dwRubbish = 0;
	unsigned char * pBuf = new unsigned char[dwSize];
	ReadFile(hFile, pBuf, dwSize, &dwRubbish, NULL);
	//2 判断是否是PE文件
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pBuf;
	if (pDos->e_magic != IMAGE_DOS_SIGNATURE)
	{
		bSuccess = FALSE;
		goto Error;
	}
	PIMAGE_NT_HEADERS  pNt = (PIMAGE_NT_HEADERS)(pBuf + pDos->e_lfanew);
	if (pNt->Signature != IMAGE_NT_SIGNATURE)
	{
		bSuccess = FALSE;
		goto Error;
	}
Error:
	if (pBuf != NULL)
	{
		delete[]pBuf;
	}
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}
	return bSuccess;
}
std::string SIGSCANER::GetSigHex(char * data, int len)
{
	char buf[0xFFFF] = { 0 };
	for (int i = 0; i < len; i++)
	{
		char test[8] = { 0 };
		if (i == len - 1)
		{
			sprintf_s(test, "%02X", (BYTE)data[i]);
			strcat_s(buf, test);
		}
		else
		{
			sprintf_s(test, "%02X ", (BYTE)data[i]);
			strcat_s(buf, test);
		}
		/*
		char test[8] = { 0 };
		if (i % 8 == 0 && i != 0 && i % 16 != 0) sprintf_s(test, "  %02X", (BYTE)data[i]);
		else sprintf_s(test, " %02X", (BYTE)data[i]);
		strcat_s(buf, test);
		if ((i + 1) % 16 == 0 && i != 0) strcat_s(buf, "\r\n");
		else if (i >= (len - 1)) strcat_s(buf, "\r\n");
		*/
	}
	//printf(buf);
	return std::string(buf);
}
std::string SIGSCANER::GetSig(std::string FilePatch, std::string sig)
{
	//1 将PE文件读取到内存
	HANDLE hFile = CreateFileA(FilePatch.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return std::string();
	DWORD dwSize = GetFileSize(hFile, NULL);
	DWORD dwRubbish = 0;
	unsigned char * pBuf = new unsigned char[dwSize];
	ReadFile(hFile, pBuf, dwSize, &dwRubbish, NULL);
	//2 找到NT头,优先64
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pBuf;
	PIMAGE_NT_HEADERS64 pNt_64 = (PIMAGE_NT_HEADERS64)(pBuf + pDos->e_lfanew);
	PIMAGE_NT_HEADERS32 pNt_32 = (PIMAGE_NT_HEADERS32)(pBuf + pDos->e_lfanew);
	bool Win_32 = false;
	if ((int)pNt_64->FileHeader.SizeOfOptionalHeader == 224)
		Win_32 = true;
	if (Win_32)
	{
		if (pNt_32->Signature != IMAGE_NT_SIGNATURE)
		{
			//report error
			return std::string();
		}
	}
	else
	{
		if (pNt_64->Signature != IMAGE_NT_SIGNATURE)
		{
			//report error
			return std::string();
		}
	}
	if (pDos->e_magic != IMAGE_DOS_SIGNATURE)
	{
		//report error
		return std::string();
	}
	auto check_section = [&](LPCVOID address)
	{
		MEMORY_BASIC_INFORMATION mbi = { 0 };
		if (VirtualQuery(address, &mbi, sizeof(mbi)))
		{
			if (mbi.RegionSize > 0x5 && mbi.Protect != PAGE_NOACCESS)
				return true;
		}
		return false;
	};
	PIMAGE_FILE_HEADER  pFileHeader = Win_32 ? &pNt_32->FileHeader : &pNt_64->FileHeader;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader_32 = NULL;
	PIMAGE_OPTIONAL_HEADER64 pOptionHeader_64 = NULL;
	if (Win_32)
		pOptionHeader_32 = &pNt_32->OptionalHeader;
	else
		pOptionHeader_64 = &pNt_64->OptionalHeader;

	g_dwImageBase = Win_32 ? pOptionHeader_32->ImageBase : pOptionHeader_64->ImageBase;
	g_DataBase = Win_32 ? pOptionHeader_32->BaseOfCode : pOptionHeader_64->BaseOfCode;
	g_Size = Win_32 ? pOptionHeader_32->SizeOfCode : pOptionHeader_64->SizeOfCode;
	g_DataBase += (DWORD64)pDos;
	if (!g_DataBase || !g_Size)
	{
		noBug->ClientBugReport();
		//report bug
		//printf("Report BUG Because zero");
		//system("pause");
		return std::string();
	}
	char bTmp[0x5] = { 0 };	
	int SigCount = 0;
	srand(time(NULL));
	DWORD64 EndBase = g_DataBase + g_Size;
	std::string Result;
	if (sig == std::string()) {
		for (int i = 0; i < 50; i++) {
			if (SigCount >= 3)
				break;
			DWORD64 Value = rand() % (g_Size - 0x1);
			DWORD64 dwTmpAddr = g_DataBase + Value;
			if (dwTmpAddr > EndBase || dwTmpAddr < g_DataBase || !check_section((LPCVOID)dwTmpAddr))
				continue;
			char byData[0x5] = { 0 };
			memcpy(byData, (void*)dwTmpAddr, 0x5);
			if (strcmp(byData, bTmp) != 0)
			{
				//printf("ADDR:%08X sigcount:%d \n", dwTmpAddr, SigCount);
				std::string hashed = "0x" + myTools->GetStringHash(GetSigHex(byData, 0x5));
				Result = Result + hashed + "|" + std::to_string(Value) +"|";
				SigCount++;
			}
		}
	}
	else {
		
		std::vector<std::string>::iterator m_iter;
		std::vector<std::string> m_sig = myTools->Split(sig, "|");
		std::vector<std::string> m_pos;
		std::vector<std::string> m_target;
		int index = 0;
		
		for (m_iter = m_sig.begin(); m_iter != m_sig.end(); m_iter++)
		{
			std::string m_str = *m_iter;
			if (index % 2 == 0)
				m_target.push_back(m_str);
			else
				m_pos.push_back(m_str);
			index++;
		}

		for (int i = 0; i < 3; i++) {
			if (SigCount >= 2)
			{
				CheatReport m_report;
				m_report.report_id = Report_SigedCheat;
				m_report.report_base_address = 0;
				m_report.report_process_id = -1;
				m_report.report_region_size = 0;
				m_report.report_sig = sig;
				m_report.report_other_data = FilePatch;
				ClientEngine->ReportCheat(m_report);
				//printf("【可疑进程】 \n");
				break;
			}
			
			DWORD64 offset = myTools->Str2Dword(m_pos.at(i));
			DWORD64 dwTmpAddr = g_DataBase + offset;
			if (dwTmpAddr > EndBase || dwTmpAddr < g_DataBase || !check_section((LPCVOID)dwTmpAddr))
				continue;
			char byData[0x5] = { 0 };
			memcpy(byData, (void*)dwTmpAddr, 0x5);
			if (strcmp(byData, bTmp) != 0)
			{
				std::string hashed = "0x" + myTools->GetStringHash(GetSigHex(byData, 0x5));
				if (hashed.compare(m_target.at(i)) == 0) {
					SigCount++;
					//printf("sig: %s mysig: %s \n", hashed.c_str(),m_target.at(i).c_str());
				}
			}
		}
	}

	CloseHandle(hFile);
	delete[] pBuf;
	//printf("sig: %s \n", Result.c_str());
	//return Result[0] + "|" + Result[1] + "|" + Result[2] + "|" + Result[3] + "|" + Result[4];
	return Result;
}
void SIGSCANER::Work()
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		//std::cout << "CreateToolhelp32Snapshot Error!" << std::endl;;
		//bug report
		return;
	}
	BOOL bResult = Process32First(hProcessSnap, &pe32);
	int num(0);
	while (bResult)
	{
		std::string name = myTools->WStringToString(pe32.szExeFile);
		printf("Process Name: %s \n",name.c_str());
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
		if (hProcess) {
			std::string ntpatch = myTools->GetProcessFullPath(hProcess);
			GetSig(ntpatch,"0x412906680|23024|0x215991421|13031|0x734096188|19936|");
			CloseHandle(hProcess);
		}
		//std::cout << "[" << ++num << "] : " << "Process Name:"<< name << "  " << "ProcessID:" << id << std::endl;
		//_nameID.insert(std::pair<string, int>(name, id)); //字典存储
		bResult = Process32Next(hProcessSnap, &pe32);
	}
	CloseHandle(hProcessSnap);
	printf("Scan Success \n");
	return;
}
