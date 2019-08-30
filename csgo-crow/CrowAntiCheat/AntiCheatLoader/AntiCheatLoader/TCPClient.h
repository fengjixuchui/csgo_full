#pragma once
enum ReportType
{
	Report_UnknownOveryLady,
	Report_SigedCheat
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
	bool ConnectAntiCheatServer();
	void ReportCheat(CheatReport cheat);
	bool SendPacket(std::string buf,int flags);
	std::string RecvPacket(int flags);
};