#include "stdafx.h"
#include "unity.h"
#include "AES.h"
std::string G::UserSecKey;
std::string G::WebSiteUrl;
SOCKET G::ClientSocket;
int MyTools::JosnGetInt(std::string json, char* data) {
	try
	{
		Document m_json;
		m_json.Parse(json.c_str());
		Value& json_decode = m_json[data];
		return json_decode.GetInt();
	}
	catch (const std::exception&)
	{
		return -1;
	}

}
std::string MyTools::JsonGetString(std::string json, char* data) {
	try
	{
		Document m_json;
		m_json.Parse(json.c_str());
		Value& json_decode = m_json[data];
		return json_decode.GetString();
	}
	catch (const std::exception&)
	{
		return std::string();
	}

}
std::vector<std::string> MyTools::Split(const std::string& src, const std::string& separator) //字符串分割到数组
{
	std::vector<std::string> dest;
	std::string str = src;
	std::string substring;
	std::string::size_type start = 0, index;
	dest.clear();
	index = str.find_first_of(separator, start);
	do
	{
		if (index != string::npos)
		{
			substring = str.substr(start, index - start);
			dest.push_back(substring);
			start = index + separator.size();
			index = str.find(separator, start);
			if (start == string::npos) break;
		}
	} while (index != string::npos);

	//the last part
	substring = str.substr(start);
	dest.push_back(substring);
	return dest;
}
std::string MyTools::GetStringHash(std::string str)
{
	ULONG64 seed = 0x1337;
	ULONG64 hash = 0;
	const char* HashText = str.c_str();
	while (*HashText) {
		hash = hash * seed + (*HashText++);
	}
	ULONG64 result = (hash & 0x7FFFFFFF);
	return std::to_string(result);
}
//关闭文件重定向系统
BOOL MyTools::DisableWow64FsRedirection(void)
{
	PVOID   pOldValue = NULL;
	typedef BOOL(WINAPI *pfnWow64DisableWow64FsRedirection)(PVOID *OldValue);
	static pfnWow64DisableWow64FsRedirection pWow64DisableWow64 = (pfnWow64DisableWow64FsRedirection)GetProcAddress(GetModuleHandle(TEXT("Kernel32.dll")), "Wow64DisableWow64FsRedirection");

	if (pWow64DisableWow64)
	{
		return pWow64DisableWow64(&pOldValue);
	}

	return FALSE;
}

//开启文件重定向系统
BOOL MyTools::RevertWow64FsRedirection(void)
{

	PVOID   pOldValue = NULL;
	typedef BOOL(WINAPI *pfnWow64RevertWow64FsRedirection)(PVOID OldValue);
	static pfnWow64RevertWow64FsRedirection pWow64RevertWow64 = (pfnWow64RevertWow64FsRedirection)GetProcAddress(GetModuleHandle(TEXT("Kernel32.dll")), "Wow64RevertWow64FsRedirection");

	//if (IsWowo64System())
	{
		if (pWow64RevertWow64)
		{
			return pWow64RevertWow64(&pOldValue);
		}
	}

	return FALSE;
}

//带重定向打开文件
BOOL MyTools::RedirectionCreateFile(const wchar_t* pFilePath, HANDLE& hFile)
{
	BOOL bRet = FALSE;
	assert(NULL != pFilePath);

	//关闭文件重定向系统
	BOOL bDisableWow64FsRedirection = DisableWow64FsRedirection();
	hFile = CreateFileW(pFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		bRet = TRUE;
	}
	//开启文件重定向系统
	if (bDisableWow64FsRedirection)
	{
		RevertWow64FsRedirection();
	}
	return bRet;
}

//获取文件数字签名
wchar_t* MyTools::GetCertName(const wchar_t* pFilePath)
{
	HCERTSTORE hStore = NULL;
	HCRYPTMSG hMsg = NULL;
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL bResult = FALSE;
	DWORD dwEncoding, dwContentType, dwFormatType;
	PCMSG_SIGNER_INFO pSignerInfo = NULL;
	DWORD dwSignerInfo = 0;
	CERT_INFO CertInfo;
	wchar_t* pCertName = NULL;
	DWORD dwData = 0;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD NumberOfBytesRead = 0;
	DWORD dwFilesize = 0;
	BYTE* pBuff = NULL;
	BOOL bDisableWow64FsRedirection = FALSE;
	memset(&CertInfo, 0, sizeof(CertInfo));
	if (IsBadReadPtr(pFilePath, sizeof(DWORD)) != 0)
	{
		return NULL;
	}


	do
	{
		if (!RedirectionCreateFile(pFilePath, hFile))
			break;

		dwFilesize = GetFileSize(hFile, NULL);
		pBuff = new BYTE[dwFilesize + 1];
		assert(NULL != pBuff);
		RtlZeroMemory(pBuff, dwFilesize + 1);
		if (ReadFile(hFile, pBuff, dwFilesize, &NumberOfBytesRead, NULL) == FALSE)
		{
			CloseHandle(hFile);
			break;
		}
		CloseHandle(hFile);


		CERT_BLOB Object = { 0 };
		Object.cbData = dwFilesize;
		Object.pbData = pBuff;
		bResult = CryptQueryObject(CERT_QUERY_OBJECT_BLOB, &Object
			, CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED, CERT_QUERY_FORMAT_FLAG_BINARY
			, 0, &dwEncoding, &dwContentType, &dwFormatType, &hStore, &hMsg, NULL);
		if (!bResult)
		{
			// 如果失败，采用原有的判断方式再执行一遍，确保此次变更不会兼容以前的代码处理效果
			//关闭文件重定向系统
			bDisableWow64FsRedirection = DisableWow64FsRedirection();
			bResult = CryptQueryObject(CERT_QUERY_OBJECT_FILE, pFilePath
				, CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED, CERT_QUERY_FORMAT_FLAG_BINARY
				, 0, &dwEncoding, &dwContentType, &dwFormatType, &hStore, &hMsg, NULL);
			if (bDisableWow64FsRedirection)
			{
				RevertWow64FsRedirection();
			}
			if (!bResult)break;
		}

		bResult = CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, NULL, &dwSignerInfo);
		if (!bResult)break;

		pSignerInfo = (PCMSG_SIGNER_INFO) new char[dwSignerInfo];
		if (NULL == pSignerInfo)break;
		ZeroMemory(pSignerInfo, dwSignerInfo);

		bResult = CryptMsgGetParam(hMsg, CMSG_SIGNER_INFO_PARAM, 0, (PVOID)pSignerInfo, &dwSignerInfo);
		if (!bResult)break;

		CertInfo.Issuer = pSignerInfo->Issuer;
		CertInfo.SerialNumber = pSignerInfo->SerialNumber;
		pCertContext = CertFindCertificateInStore(hStore, ENCODING, 0, CERT_FIND_SUBJECT_CERT, (PVOID)&CertInfo, NULL);
		if (NULL == pCertContext)break;

		dwData = CertGetNameString(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, NULL, 0);
		if (1 >= dwData)
			break;

		pCertName = new wchar_t[dwData + 1];
		if (NULL == pCertName)break;
		ZeroMemory(pCertName, (dwData + 1) * sizeof(wchar_t));

		if (!(CertGetNameStringW(pCertContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, pCertName, dwData)))
			break;



	} while (FALSE);

	SafeDeleteArraySize(pBuff);
	SafeDeleteArraySize(pSignerInfo);
	if (pCertContext != NULL) CertFreeCertificateContext(pCertContext);
	if (hStore != NULL) CertCloseStore(hStore, 0);
	if (hMsg != NULL) CryptMsgClose(hMsg);

	return pCertName;
}
//检测文件是否有签名
wchar_t* MyTools::GetFileCat(const wchar_t* lpFileName)
{
	WINTRUST_DATA wd = { 0 };
	WINTRUST_FILE_INFO wfi = { 0 };
	WINTRUST_CATALOG_INFO wci = { 0 };
	CATALOG_INFO ci = { 0 };
	HCATADMIN hCatAdmin = NULL;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwCnt = 0;
	PBYTE pbyHash = NULL;
	wchar_t* pszMemberTag = NULL;
	HCATINFO hCatInfo = NULL;
	HRESULT hr;
	static GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
	const GUID gSubsystem = DRIVER_ACTION_VERIFY;
	wchar_t* pCatalogFile = NULL;
	do
	{

		if (!CryptCATAdminAcquireContext(&hCatAdmin, &gSubsystem, 0))
			break;

		if (!RedirectionCreateFile(lpFileName, hFile))
			break;

		if (CryptCATAdminCalcHashFromFileHandle(hFile, &dwCnt, pbyHash, 0) && dwCnt > 0 && ERROR_INSUFFICIENT_BUFFER == GetLastError())
		{
			pbyHash = new BYTE[dwCnt];
			ZeroMemory(pbyHash, dwCnt);
			if (CryptCATAdminCalcHashFromFileHandle(hFile, &dwCnt, pbyHash, 0) == FALSE)
			{
				CloseHandle(hFile);
				break;
			}
		}
		else
		{
			CloseHandle(hFile);
			break;
		}
		CloseHandle(hFile);

		hCatInfo = CryptCATAdminEnumCatalogFromHash(hCatAdmin, pbyHash, dwCnt, 0, NULL);
		if (NULL == hCatInfo)
		{
			wfi.cbStruct = sizeof(WINTRUST_FILE_INFO);
			wfi.pcwszFilePath = lpFileName;
			wfi.hFile = NULL;
			wfi.pgKnownSubject = NULL;
			wd.cbStruct = sizeof(WINTRUST_DATA);
			wd.dwUnionChoice = WTD_CHOICE_FILE;
			wd.pFile = &wfi;
			wd.dwUIChoice = WTD_UI_NONE;
			wd.fdwRevocationChecks = WTD_REVOKE_NONE;
			wd.dwStateAction = WTD_STATEACTION_IGNORE;
			wd.dwProvFlags = WTD_SAFER_FLAG;
			wd.hWVTStateData = NULL;
			wd.pwszURLReference = NULL;
		}
		else
		{
			if (CryptCATCatalogInfoFromContext(hCatInfo, &ci, 0))
			{
				pszMemberTag = new wchar_t[dwCnt * 2 + 1];
				ZeroMemory(pszMemberTag, (dwCnt * 2 + 1) * sizeof(wchar_t));
				for (DWORD dw = 0; dw < dwCnt; ++dw)
				{
					wsprintfW(&pszMemberTag[dw * 2], L"%02X", pbyHash[dw]);

				}

				wci.cbStruct = sizeof(WINTRUST_CATALOG_INFO);
				wci.pcwszCatalogFilePath = ci.wszCatalogFile;
				wci.pcwszMemberFilePath = lpFileName;
				wci.pcwszMemberTag = pszMemberTag;

				wd.cbStruct = sizeof(WINTRUST_DATA);
				wd.pCatalog = &wci;
				wd.dwUIChoice = WTD_UI_NONE;
				wd.dwUnionChoice = WTD_CHOICE_CATALOG;
				wd.fdwRevocationChecks = WTD_STATEACTION_VERIFY;
				wd.dwStateAction = WTD_STATEACTION_VERIFY;
				wd.dwProvFlags = 0;
				wd.hWVTStateData = NULL;
				wd.pwszURLReference = NULL;

			}


		}


		hr = WinVerifyTrust((HWND)INVALID_HANDLE_VALUE, &action, &wd);
		if (SUCCEEDED(hr) || wcslen(ci.wszCatalogFile) > 0)
		{
			//返回cat文件
			pCatalogFile = new wchar_t[MAX_PATH];
			ZeroMemory(pCatalogFile, MAX_PATH * sizeof(wchar_t));
			CopyMemory(pCatalogFile, ci.wszCatalogFile, wcslen(ci.wszCatalogFile) * sizeof(wchar_t));
		}
		if (NULL != hCatInfo)
		{
			CryptCATAdminReleaseCatalogContext(hCatAdmin, hCatInfo, 0);
		}


	} while (FALSE);


	if (hCatAdmin)
	{
		CryptCATAdminReleaseContext(hCatAdmin, 0);
	}




	SafeDeleteArraySize(pbyHash);
	SafeDeleteArraySize(pszMemberTag);
	return pCatalogFile;
}
std::string MyTools::Tchar2Str(TCHAR *STR)
{

	int iLen = WideCharToMultiByte(CP_ACP, 0,STR, -1, NULL, 0, NULL, NULL);

	char* chRtn = new char[iLen * sizeof(char)];

	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);

	std::string str(chRtn);

	return str;

}
DWORD64 MyTools::GetProcessAddr(HANDLE hProcess)
{
	;
}
BOOL DosPathToNtPath(LPTSTR pszDosPath, LPTSTR pszNtPath)
{
	TCHAR            szDriveStr[500];
	TCHAR            szDrive[3];
	TCHAR            szDevName[100];
	INT                cchDevName;
	INT                i;

	//检查参数
	if (!pszDosPath || !pszNtPath)
		return FALSE;

	//获取本地磁盘字符串
	if (GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for (i = 0; szDriveStr[i]; i += 4)
		{
			if (!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))
				continue;

			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			szDrive[2] = '\0';
			if (!QueryDosDevice(szDrive, szDevName, 100))//查询 Dos 设备名
				return FALSE;

			cchDevName = lstrlen(szDevName);
			if (_tcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//命中
			{
				lstrcpy(pszNtPath, szDrive);//复制驱动器
				lstrcat(pszNtPath, pszDosPath + cchDevName);//复制路径

				return TRUE;
			}
		}
	}

	lstrcpy(pszNtPath, pszDosPath);

	return FALSE;
}

//获取进程完整路径
std::string MyTools::GetProcessFullPath(HANDLE hProcess)
{
	TCHAR        szImagePath[MAX_PATH], pszFullPath[MAX_PATH];
	//HANDLE        hProcess;

	pszFullPath[0] = '\0';
	if (!hProcess)
		return FALSE;

	if (!GetProcessImageFileName(hProcess, szImagePath, MAX_PATH))
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	if (!DosPathToNtPath(szImagePath, pszFullPath))
	{
		//CloseHandle(hProcess);
		return FALSE;
	}
	//_tprintf(_T("%s"), pszFullPath);
	return Tchar2Str(pszFullPath);
}

//\\Device\\HarddiskVolume1\x86.sys    c:\x86.sys    
BOOL MyTools::DeviceDosPathToNtPath(wchar_t* pszDosPath, wchar_t* pszNtPath)
{
	static TCHAR    szDriveStr[MAX_PATH] = { 0 };
	static TCHAR    szDevName[MAX_PATH] = { 0 };
	TCHAR            szDrive[3];
	INT             cchDevName;
	INT             i;

	//检查参数  
	if (IsBadReadPtr(pszDosPath, 1) != 0)return FALSE;
	if (IsBadWritePtr(pszNtPath, 1) != 0)return FALSE;


	//获取本地磁盘字符串  
	ZeroMemory(szDriveStr, ARRAYSIZE(szDriveStr));
	ZeroMemory(szDevName, ARRAYSIZE(szDevName));
	if (GetLogicalDriveStrings(sizeof(szDriveStr), szDriveStr))
	{
		for (i = 0; szDriveStr[i]; i += 4)
		{
			if (!lstrcmpi(&(szDriveStr[i]), _T("A:\\")) || !lstrcmpi(&(szDriveStr[i]), _T("B:\\")))
				continue;

			szDrive[0] = szDriveStr[i];
			szDrive[1] = szDriveStr[i + 1];
			szDrive[2] = '\0';
			if (!QueryDosDevice(szDrive, szDevName, MAX_PATH))//查询 Dos 设备名  
				return FALSE;

			cchDevName = lstrlen(szDevName);
			if (_tcsnicmp(pszDosPath, szDevName, cchDevName) == 0)//命中  
			{
				lstrcpy(pszNtPath, szDrive);//复制驱动器  
				lstrcat(pszNtPath, pszDosPath + cchDevName);//复制路径  

				return TRUE;
			}
		}
	}
	lstrcpy(pszNtPath, pszDosPath);

	return FALSE;
}
DWORD64 MyTools::Str2Dword(std::string str)
{
	DWORD64 result;
	std::istringstream is(str);
	is >> result;
	return result;
}

std::vector<std::string> MyTools::splitWithStl(const std::string &str, const std::string &pattern)
{
	std::vector<std::string> resVec;

	if ("" == str)
	{
		return resVec;
	}
	std::string strs = str + pattern;
	size_t pos = strs.find(pattern);
	size_t size = strs.size();
	while (pos != std::string::npos)
	{
		std::string x = strs.substr(0, pos);
		resVec.push_back(x);
		strs = strs.substr(pos + 1, size);
		pos = strs.find(pattern);
	}
	return resVec;
}
std::string MyTools::DwordToString(DWORD dwValue)
{
	std::ostringstream os;
	os << dwValue;
	std::string result;
	std::istringstream is(os.str());
	is >> result;
	return result;
}
std::wstring MyTools::StringToWString(const std::string& str)
{
	int num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t *wide = new wchar_t[num];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, num);
	std::wstring w_str(wide);
	delete[] wide;
	return w_str;
}

std::string MyTools::WStringToString(const std::wstring &wstr)
{
	std::string str;
	int nLen = (int)wstr.length();
	str.resize(nLen, ' ');
	int nResult = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wstr.c_str(), nLen, (LPSTR)str.c_str(), nLen, NULL, NULL);
	if (nResult == 0)
	{
		return "";
	}
	return str;
}
std::string MyTools::Base64Decode(const std::string &in) {

	std::string out;

	std::vector<int> T(256, -1);
	for (int i = 0; i<64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

	int val = 0, valb = -8;
	for (unsigned char c : in) {
		if (T[c] == -1) break;
		val = (val << 6) + T[c];
		valb += 6;
		if (valb >= 0) {
			out.push_back(char((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return out;
}
std::string MyTools::Base64Encode(std::string base64str)
{
	std::string out;

	int val = 0, valb = -6;
	for (unsigned char c : base64str) {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb>-6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
	while (out.size() % 4) out.push_back('=');
	return out;
}
std::string MyTools::EncryptionAES(const std::string& strSrc) //AES加密
{
	size_t length = strSrc.length();
	int block_num = length / BLOCK_SIZE + 1;
	//明文
	char* szDataIn = new char[block_num * BLOCK_SIZE + 1];
	memset(szDataIn, 0x00, block_num * BLOCK_SIZE + 1);
	strcpy(szDataIn, strSrc.c_str());

	//进行PKCS7Padding填充。
	int k = length % BLOCK_SIZE;
	int j = length / BLOCK_SIZE;
	int padding = BLOCK_SIZE - k;
	for (int i = 0; i < padding; i++)
	{
		szDataIn[j * BLOCK_SIZE + k + i] = padding;
	}
	szDataIn[block_num * BLOCK_SIZE] = '\0';

	//加密后的密文
	char *szDataOut = new char[block_num * BLOCK_SIZE + 1];
	memset(szDataOut, 0, block_num * BLOCK_SIZE + 1);

	//进行进行AES的CBC模式加密
	AES aes;
	aes.MakeKey(g_key, g_iv, 16, 16);
	aes.Encrypt(szDataIn, szDataOut, block_num * BLOCK_SIZE, AES::CBC);
	string str = char_base64_encode((unsigned char*)szDataOut,block_num * BLOCK_SIZE);
	/*
	int add = 0;
	if (str.length() % 16 != 0)
		add = 16 - (str.length() % 16);
	else
		add = 0;
	if (add != 0)
		for (int i = 0; i < add; i++)
			str = str + '\0';
	*/
	delete[] szDataIn;
	delete[] szDataOut;
	return str;

}
std::string MyTools::DecryptionAES(const std::string& strSrc) //AES解密
{
	string strData = Base64Decode(strSrc);
	size_t length = strData.length();
	//密文
	char *szDataIn = new char[length + 1];
	memcpy(szDataIn, strData.c_str(), length + 1);
	//明文
	char *szDataOut = new char[length + 1];
	memcpy(szDataOut, strData.c_str(), length + 1);

	//进行AES的CBC模式解密
	AES aes;
	aes.MakeKey(g_key, g_iv, 16, 16);
	aes.Decrypt(szDataIn, szDataOut, length, AES::CBC);

	//去PKCS7Padding填充
	if (0x00 < szDataOut[length - 1] <= 0x16)
	{
		int tmp = szDataOut[length - 1];
		for (int i = length - 1; i >= length - tmp; i--)
		{
			if (szDataOut[i] != tmp)
			{
				memset(szDataOut, 0, length);
				cout << "去填充失败！解密出错！！" << endl;
				break;
			}
			else
				szDataOut[i] = 0;
		}
	}
	string strDest(szDataOut);
	delete[] szDataIn;
	delete[] szDataOut;
	return strDest;

}
//获取文件数字签名
wchar_t* MyTools::GetFileCertNameA(const wchar_t* pFilePath)
{
	wchar_t* pCertName = NULL;
	wchar_t* pCatFilePath = NULL;

	//获取文件数字签名
	pCertName = GetCertName(pFilePath);
	if (pCertName == NULL)
	{
		//获取文件cat
		pCatFilePath = GetFileCat(pFilePath);
		if (pCatFilePath)
		{
			//获取cat文件数字签名
			pCertName = GetCertName(pCatFilePath);
		}
	}

	SafeDeleteArraySize(pCatFilePath);
	wprintf(L"cert name:%s \n", pCertName);
	return pCertName;
	//return L"abc";
}
BOOL MyTools::CheckFileTrust(wchar_t* lpFileName)
{
	HANDLE hFile;
	if (!RedirectionCreateFile(lpFileName, hFile))
	{
		printf("create file failed\n");
		return true;
	}

	if (GetFileCertNameA(lpFileName) != NULL)
	{
		//把签名丢到服务端验证是不是在白名单里的
		//printf("%ws \n", GetFileCertNameA(lpFileName));
		return true;
	}
	else
		return false;
}

//Converting a Ansi string to WChar string   
std::wstring MyTools::Ansi2WChar(LPCSTR pszSrc, int nLen)
{
	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if (nSize <= 0) return NULL;
	WCHAR *pwszDst = new WCHAR[nSize + 1];
	if (NULL == pwszDst) return NULL;
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;
	if (pwszDst[0] == 0xFEFF) // skip Oxfeff   
		for (int i = 0; i < nSize; i++)
			pwszDst[i] = pwszDst[i + 1];
	std::wstring wcharString(pwszDst);
	delete pwszDst;
	return wcharString;
}

std::wstring MyTools::s2ws(const std::string& s)
{
	return Ansi2WChar(s.c_str(), s.size());
}
std::string MyTools::PID2FilePatch(DWORD process_id)
{
	HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
	if (process == NULL)
		return std::string();
	CHAR file_path[MAX_PATH] = { 0 };
	GetModuleFileNameExA(process, NULL, file_path, MAX_PATH);
	CloseHandle(process);
	return std::string(file_path);
}

std::string MyTools::char_base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len)
{
	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";
	std::string ret;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i <4); i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';
		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;
		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];
		while ((i++ < 3))
			ret += '=';
	}

	return ret;
}