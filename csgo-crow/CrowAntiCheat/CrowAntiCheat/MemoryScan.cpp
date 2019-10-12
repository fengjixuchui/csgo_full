#include "stdafx.h"
#include "MemoryScan.h"


void MemoryScan::Install()
{
	//先比对正常模块list:
	HANDLE hShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	MODULEENTRY32 te = { sizeof(te) };
	BOOL bRet = Module32First(hShot, &te);
	entry[i].entry = (DWORD)GetModuleHandle(NULL);
	entry[i].endaddr = GetMoudleEnd(entry[i].entry);
	while (bRet) {
		i++;
		//T::PrintMessage("%ws \n",te.szModule);
		DWORD Moudle_entry = (DWORD)GetModuleHandleW(te.szModule);
		entry[i].entry = Moudle_entry;
		entry[i].endaddr = GetMoudleEnd(Moudle_entry);
		//T::PrintMessage("GetModuleHandleW <0x%08X> \n", Moudle_entry);
		bRet = Module32Next(hShot, &te);	
	}
	CloseHandle(hShot);
	

	G::MOUDLE_END = GetMoudleEnd(G::MOUDLE);
}
DWORD MemoryScan::GetMoudleEnd(DWORD start)
{
	MODULEINFO miModInfo;
	GetModuleInformation(GetCurrentProcess(), (HMODULE)start, &miModInfo, sizeof(MODULEINFO));
	return (start + miModInfo.SizeOfImage);
}
void MemoryScan::Anomaly_check(MEMORY_BASIC_INFORMATION memory_information)
{
	// REPORT ANY EXECUTABLE PAGE OUTSIDE OF KNOWN MODULES
	if (memory_information.Type == MEM_PRIVATE || memory_information.Type == MEM_MAPPED)
	{
		if (((DWORD)memory_information.BaseAddress & 0xFF0000000000) != 0x7F0000000000 && // UPPER EQUALS 0x7F
			((DWORD)memory_information.BaseAddress & 0xFFF000000000) != 0x7F000000000 &&  // UPPER EQUALS 0x7F0
			((DWORD)memory_information.BaseAddress & 0xFFFFF0000000) != 0x70000000 && // UPPER EQUALS 0x70000
			(DWORD)memory_information.BaseAddress != 0x3E0000)
		{
			ReportStruct m_report;
			m_report.report_base_address = (DWORD)memory_information.BaseAddress;
			m_report.report_id = Report_MEMORY_SUSPICIOUS;
			m_report.report_process_id = -1;
			m_report.report_region_size = memory_information.RegionSize;
			m_report.report_sig = "0";
			m_report.report_other_data = "null";
			Reporter->CheatReport(m_report);
			exit(0);
		}
	}
}
void MemoryScan::CheckThread()
{
	auto check_section = [&](LPCVOID address)
	{
		MEMORY_BASIC_INFORMATION mbi = { 0 };
		if (VirtualQuery(address, &mbi, sizeof(mbi)))
		{
			bool found = false;
			//mbi.Type == MEM_IMAGE ||
			if (mbi.AllocationProtect & PAGE_EXECUTE ||
				mbi.AllocationProtect & PAGE_EXECUTE_READ ||
				mbi.AllocationProtect & PAGE_EXECUTE_READWRITE ||
				mbi.AllocationProtect & PAGE_EXECUTE_WRITECOPY)
				found = true;
			return found;
		}
		return false;
	};
	while (true)
	{
		HANDLE hSnapshort = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
		THREADENTRY32 stcThreadInfo;
		stcThreadInfo.dwSize = sizeof(stcThreadInfo);
		bool bRet = Thread32First(hSnapshort, &stcThreadInfo);
		while (bRet)
		{
			if (GetCurrentProcessId() == stcThreadInfo.th32OwnerProcessID)
			{
				PVOID  ThreadStartAddr = 0;
				HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION | THREAD_TERMINATE, FALSE, stcThreadInfo.th32ThreadID);
				if (hThread)
				{
					NtQueryInformationThread(hThread, ThreadQuerySetWin32StartAddress, &ThreadStartAddr, sizeof(ThreadStartAddr), NULL);
					if (ThreadStartAddr != 0)
					{
						bool bFoundManualMap = true;

						//得到线程地址后,与已知的模块比对查看大小是否在已知的模块中.
						for (int z = 0; z < i; z++)
						{
							if (ThreadStartAddr >= (PVOID)entry[z].entry &&
								ThreadStartAddr <= (PVOID)entry[z].endaddr)
							{
								bFoundManualMap = false;
								continue;
							}
						}
						if (bFoundManualMap && check_section(ThreadStartAddr))
						{
							//T::PrintMessage("Found ManualMap Dll <0x%08X> ThreadID: %d ThreadFromPID: %d \n", ThreadStartAddr, stcThreadInfo.th32ThreadID, stcThreadInfo.th32OwnerProcessID);
							ReportStruct m_report;
							m_report.report_id = Report_ManualMap;
							m_report.report_base_address = (DWORD)ThreadStartAddr;
							m_report.report_process_id = -1;
							m_report.report_region_size = 0;
							m_report.report_sig = "0";
							m_report.report_other_data = "UnknownThread";
							Reporter->CheatReport(m_report);
							exit(1);
						}
					}
					CloseHandle(hThread);
				}

			}
			bRet = Thread32Next(hSnapshort, &stcThreadInfo);
		}
		CloseHandle(hSnapshort);
		Sleep(200);
	}
}
void StartCheckThread() {
	MMSCAN->CheckThread();
}
void MemoryScan::guard_check(void* current_address, MEMORY_BASIC_INFORMATION memory_information)
{
	if (memory_information.Protect != PAGE_NOACCESS)
	{
		LPVOID temporary_buffer = 0;
		auto bad_ptr = IsBadReadPtr(current_address, sizeof(current_address));
		auto read = ReadProcessMemory(GetCurrentProcess(),current_address,temporary_buffer,sizeof(temporary_buffer),0);
		if (read < 0 || bad_ptr)
		{
			MEMORY_BASIC_INFORMATION new_memory_information = { 0 };
			auto query = VirtualQuery(current_address, &new_memory_information, sizeof(new_memory_information));
			bool guard = query < 0 || new_memory_information.State != memory_information.State || new_memory_information.Protect != memory_information.Protect;

			if (guard)
			{
				ReportStruct m_report;
				m_report.report_id = Report_MEMORY_GUARD;
				m_report.report_base_address = (DWORD)memory_information.BaseAddress;
				m_report.report_process_id = -1;
				m_report.report_region_size = memory_information.RegionSize;
				m_report.report_sig = "0";
				m_report.report_other_data = "GUARD_MEMORY";
				Reporter->CheatReport(m_report);
				exit(0);
			}
		}
	}
}
void MemoryScan::StartScan()
{
	//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)StartCheckThread, NULL, NULL, NULL);
	auto check_isWhileShit = [&](LPCVOID address)
	{
		char byData[0x10] = { 0 };

		return false;
	};
	MEMORY_BASIC_INFORMATION memory_information = { 0 };
	for (DWORD current_address = 0;
		VirtualQuery((PVOID)current_address, &memory_information, sizeof(memory_information)) >= 0;
		current_address = (DWORD)memory_information.BaseAddress + (DWORD)memory_information.RegionSize)
	{
		const auto outside_of_shellcode =
			memory_information.BaseAddress > (PVOID)G::MOUDLE || (DWORD)memory_information.BaseAddress + (DWORD)memory_information.RegionSize <= (DWORD)G::MOUDLE_END;

		const auto executable_memory =
			memory_information.State == MEM_COMMIT &&
			(memory_information.Protect == PAGE_EXECUTE ||
				memory_information.Protect == PAGE_EXECUTE_READ ||
				memory_information.Protect == PAGE_EXECUTE_READWRITE);
		const auto unknown_whitelist =
			memory_information.Protect != PAGE_EXECUTE_READWRITE ||
			memory_information.RegionSize != 100000000;
		if (!executable_memory || !outside_of_shellcode || !unknown_whitelist)
			continue;
		if(memory_information.Protect == PAGE_READONLY)
			continue;
		if (memory_information.RegionSize == 1000 ||
			memory_information.RegionSize == 2000 ||
			memory_information.RegionSize == 4096 ||
			memory_information.RegionSize == 8192 ||
			memory_information.RegionSize == 294912 ||
			memory_information.RegionSize == 495616)
			continue;
		
		bool bFoundManualMap = true;
		for (int z = 0; z < i; z++)
		{
			if (current_address >= entry[z].entry &&
				current_address <= entry[z].endaddr)
			{
				bFoundManualMap = false;
				continue;
			}
		}
		if (bFoundManualMap || memory_information.Type == MEM_PRIVATE)
		{
			T::PrintMessage("Found Unknown Moudle <0x%08X> size: %d \n", current_address, memory_information.RegionSize);
			ReportStruct m_report;
			m_report.report_id = Report_ManualMap;
			m_report.report_base_address = (DWORD)memory_information.BaseAddress;
			m_report.report_process_id = -1;
			m_report.report_region_size = memory_information.RegionSize;
			m_report.report_sig = "0";
			m_report.report_other_data = "UnknownMoudle";
			Reporter->CheatReport(m_report);
			exit(0);
		}
		Anomaly_check(memory_information);
		guard_check((void*)current_address, memory_information);
		Sleep(500);
	}
	/*
	if(!bFoundTheShit)
		T::PrintMessage("Not Find ManualMap Moudle \n");
	else
		T::PrintMessage("Found ManualMap Moudle \n");
		*/
}
