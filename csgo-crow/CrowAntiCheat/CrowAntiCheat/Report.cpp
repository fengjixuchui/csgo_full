#include "stdafx.h"
#include "Report.h"
#include "unity.h"
void REPORT::HeartBeat()
{
	std::string m_info = "HeartBeat";
	WriteFile(G::hPipe, (void*)&m_info, m_info.length(), NULL, 0);
}
void REPORT::CheatReport(ReportStruct m_parameter)
{
	//MessageBox(NULL, TEXT("CheatReport"),NULL, MB_OK);
	
	WriteFile(G::hPipe, (void*)& m_parameter, sizeof(m_parameter), NULL, 0);
	MessageBox(NULL, TEXT("CheatReport"), NULL, MB_OK);
}
