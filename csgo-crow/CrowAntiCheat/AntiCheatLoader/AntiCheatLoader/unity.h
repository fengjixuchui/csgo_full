#pragma once
#define SafeDeleteArraySize(pData) { if(pData){delete []pData;pData=NULL;} }
#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)
#define INRANGE(x,a,b)    (x >= a && x <= b)
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))
#include <assert.h>
#include <WinSock2.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <windows.h>  
#include <WS2tcpip.h>
#include <Psapi.h>
#include <vector>
#include <string>
#include <iostream>
#include <wincrypt.h>
#include <wintrust.h>
#include <Wincrypt.h>
#include <mscat.h>
#include <Softpub.h>
#include <atlconv.h>
#include <sstream>
#include <iostream>
#include "xorstr.hpp"
#include "CallBacks.h"
#include "TCPserver.h"
#include "TCPClient.h"
#include "MD5Encrypt.h"
#include "OverlayFinder.h"
#include "AntiCheatEngine.h"
#include "SigScaner.h"
#include "NoBugZ.h"
#include "../../ShareLib/curl/include/curl.h"
#include "../../ShareLib/rapidjson/document.h"
#include "../../ShareLib/rapidjson/writer.h"
#include "../../ShareLib/rapidjson/stringbuffer.h"
#pragma comment(lib,"../../ShareLib/curl/libcurl.lib")
#pragma comment(lib, "ws2_32.lib")  
#pragma comment(lib, "Wintrust.lib") 
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "ws2_32.lib" )
#pragma comment(lib, "winmm.lib" )
#pragma comment(lib, "wldap32.lib" )
#pragma comment(lib, "Advapi32.lib")
using namespace rapidjson;
static SIGSCANER* SigScaner = new SIGSCANER;
static ANTICHEATENGINE* AntiCheatEngine = new ANTICHEATENGINE;
static TCPSERVER *ServerEngine = new TCPSERVER;
static TCPCLIENT *ClientEngine = new TCPCLIENT;
static myCALLBACK* myCallBack = new myCALLBACK;
static OVERLAYFUCKER* OverLayFucker = new OVERLAYFUCKER;
static NoBugZ* noBug = new NoBugZ;
namespace G
{
	extern std::string UserSecKey;
	extern std::string WebSiteUrl;
	extern SOCKET ClientSocket;
}
namespace T
{
	static std::string GetMd5Hash(std::string str)
	{
		Md5Encode encode_obj;
		return encode_obj.Encode(str);
	}
}

class MyTools
{
public:
	const char g_key[17] = "apffwetyhjyrtsfd";
	const char g_iv[17] = "gfdartgqhjkuyrlp";
	int JosnGetInt(std::string json, char* data);
	std::string JsonGetString(std::string json, char* data);
	std::vector<std::string> Split(const std::string& src, const std::string& separator);
	std::string GetStringHash(std::string str);
	//void SendMd52Server(CString FileDirectory);
	BOOL DisableWow64FsRedirection(void);

	BOOL RevertWow64FsRedirection(void);

	BOOL RedirectionCreateFile(const wchar_t * pFilePath, HANDLE & hFile);

	wchar_t * GetCertName(const wchar_t * pFilePath);

	wchar_t * GetFileCat(const wchar_t * lpFileName);

	std::string Tchar2Str(TCHAR * STR);

	DWORD64 GetProcessAddr(HANDLE hProcess);

	//std::string DosDevicePath2LogicalPath(LPCTSTR lpszDosPath);

	std::string GetProcessFullPath(HANDLE hProcess);

	BOOL DeviceDosPathToNtPath(wchar_t * pszDosPath, wchar_t * pszNtPath);

	DWORD64 Str2Dword(std::string str);

	std::vector<std::string> splitWithStl(const std::string & str, const std::string & pattern);

	std::string DwordToString(DWORD dwValue);

	std::wstring StringToWString(const std::string & str);

	std::string WStringToString(const std::wstring & wstr);

	std::string Base64Decode(const std::string & in);

	std::string Base64Encode(std::string base64str);

	std::string EncryptionAES(const std::string & strSrc);

	std::string DecryptionAES(const std::string & strSrc);

	wchar_t * GetFileCertNameA(const wchar_t * pFilePath);

	BOOL CheckFileTrust(wchar_t * lpFileName);
//	std::string GetProcessFilePath(IN HANDLE hProcess);
	std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen);
	std::wstring s2ws(const std::string & s);
	std::string PID2FilePatch(DWORD process_id);

	std::string char_base64_encode(unsigned char const*, unsigned int len);

};
static MyTools* myTools = new MyTools;