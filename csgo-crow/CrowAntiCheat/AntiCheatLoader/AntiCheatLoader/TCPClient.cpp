#include "stdafx.h"
#include "unity.h"
#include <time.h>
#include <thread>

void ProcessInfo(PVOID pParam) {
	HANDLE m_handle = (HANDLE)pParam;
	if (!m_handle) {
		return noBug->ClientBugReport();
	}
	//MAX_PATH
	char buf[2019] = "";
	DWORD rlen = 0;
	while (true)
	{
		if (ConnectNamedPipe(m_handle, NULL) != NULL)
		{
			if (ReadFile(m_handle, buf, 2019, &rlen, NULL) == FALSE)
				continue;
			else
			{
				//接收信息
				CheatReport* buffer = (CheatReport*)& buf;
				//底层设计失误不能用.c_str! x86的string库和x64的string库不同!
				char* antibug = (char*)& buffer->report_other_data;
				std::string fuckBUGshit(antibug);
				printf("report_base_address: 0x%08X  report_other_data :%s \n", buffer->report_base_address, buffer->report_other_data);
				DisconnectNamedPipe(m_handle);
			}
		}

	}
}
bool TCPCLIENT::InitAntiCheat()
{
	G::WebSiteUrl = "127.0.0.1:8000";
	//Test only!
	//G::UserSecKey = "6d10c7c5600591d3535ff77c3170ce10";

	//管道要用xor加密
	HANDLE hPipe = CreateNamedPipe(
		TEXT("\\\\.\\Pipe\\CrowAntiCheat"),						
		PIPE_ACCESS_DUPLEX,									
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,	
		PIPE_UNLIMITED_INSTANCES,							
		0,													
		0,													
		NMPWAIT_WAIT_FOREVER,								
		NULL);
	if (INVALID_HANDLE_VALUE == hPipe)
		return false;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ProcessInfo, (LPVOID)hPipe, NULL, NULL);
	return true;
}

bool TCPCLIENT::ConnectAntiCheatServer()
{	
	std::string parameter = "login=" + G::UserSecKey;
	std::string url = G::WebSiteUrl + "/api/anticheat/?" + parameter;
	std::string result_json = SendHttpRequest(url,HTTP_GET);
	auto msgType = myTools->JsonGetString(result_json, "msgType");
	if (msgType.compare(XorString("AC_Login")) == 0)
		return myTools->JosnGetInt(result_json, "success");
	
	return false;
}

void TCPCLIENT::ReportCheat(CheatReport m_parameter)
{
}

size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
	std::string data((const char*)ptr, (size_t)size * nmemb);

	*((std::stringstream*)stream) << data << std::endl;

	return size * nmemb;
}
std::string TCPCLIENT::SendHttpRequest(std::string url,HTTPREQUEST get_or_post, std::string json_data)
{
	CURL* curl = curl_easy_init();
	CURLcode res = CURLE_OK;
	std::stringstream out;
	struct curl_slist* headers = NULL;
	//我这个版本的curl不支持https!要自己用OPENSSL编译一个版本
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POST, get_or_post);//0 = GET 1 = POST
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
	if (HTTP_POST == get_or_post)
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		return std::string();
	}
	curl_slist_free_all(headers);
	std::string result = out.str();
	curl_easy_cleanup(curl);
	return result;
}
