#pragma once
#include <string>
enum ReportType
{
	Report_UnknownOveryLady,
	Report_SigedCheat,
	Report_VMT_Hook,
	Report_ManualMap,
	Report_UnknownMoudle,
	Report_MEMORY_SUSPICIOUS,
	Report_MEMORY_GUARD
};
struct ReportStruct
{
	ReportType report_id;
	DWORD report_process_id;
	DWORD report_base_address;
	DWORD report_region_size;
	std::string report_other_data;
	std::string report_sig;
};
class REPORT {
public:
	void HeartBeat();
	void CheatReport(ReportStruct m_parameter);
};