#pragma once
enum HTTPREQUEST
{
	HTTP_GET,
	HTTP_POST
};
enum ReportType
{
	Report_UnknownOveryLady,
	Report_SigedCheat,
	Report_VMT_Hook,
	Report_HWBP_Hook,
	Report_ManualMap,
	Report_UnknownMoudle,
	Report_MEMORY_SUSPICIOUS,
	Report_MEMORY_GUARD
};
struct CheatReport {
	ReportType report_id;
	DWORD report_process_id;
	DWORD report_base_address;
	DWORD report_region_size;
	std::string report_other_data;
	std::string report_sig;
};

class TCPCLIENT {
public:
	bool InitAntiCheat();
	bool ConnectAntiCheatServer();
	void ReportCheat(CheatReport m_parameter);
	std::string SendHttpRequest(std::string url, HTTPREQUEST get_or_post, std::string json_data = "A");
};