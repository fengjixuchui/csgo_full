#pragma once
#include "unity.h"
#include <iostream>
#include <list>

typedef enum _THREADINFOCLASS {
	ThreadBasicInformation,
	ThreadTimes,
	ThreadPriority,
	ThreadBasePriority,
	ThreadAffinityMask,
	ThreadImpersonationToken,
	ThreadDescriptorTableEntry,
	ThreadEnableAlignmentFaultFixup,
	ThreadEventPair_Reusable,
	ThreadQuerySetWin32StartAddress,
	ThreadZeroTlsCell,
	ThreadPerformanceCount,
	ThreadAmILastThread,
	ThreadIdealProcessor,
	ThreadPriorityBoost,
	ThreadSetTlsArrayAddress,
	ThreadIsIoPending,
	ThreadHideFromDebugger,
	ThreadBreakOnTermination,
	MaxThreadInfoClass
} THREADINFOCLASS;

typedef LONG(WINAPI *NtQueryInformationThreadProc)(
	_In_       HANDLE ThreadHandle,
	_In_       THREADINFOCLASS ThreadInformationClass,
	_Inout_    PVOID ThreadInformation,
	_In_       ULONG ThreadInformationLength,
	_Out_opt_  PULONG ReturnLength
	);
static NtQueryInformationThreadProc NtQueryInformationThread = (NtQueryInformationThreadProc)GetProcAddress(GetModuleHandleA(XorString("ntdll.dll")), XorString("NtQueryInformationThread"));
typedef enum _MEMORY_INFORMATION_CLASS {
	MemoryBasicInformation,
	MemoryWorkingSetList,
	MemorySectionName,
	MemoryBasicVlmInformation
} MEMORY_INFORMATION_CLASS;
typedef LONG(WINAPI *ZwQueryVirtualMemoryProc)(
	IN HANDLE ProcessHandle,
	IN PVOID BaseAddress,
	IN MEMORY_INFORMATION_CLASS MemoryInformationClass,
	OUT PVOID MemoryInformation,
	IN ULONG MemoryInformationLength,
	OUT PULONG ReturnLength OPTIONAL);
static ZwQueryVirtualMemoryProc ZwQueryVirtualMemory = (ZwQueryVirtualMemoryProc)GetProcAddress(GetModuleHandleA(XorString("ntdll.dll")), XorString("ZwQueryVirtualMemory"));
struct MOUDLES_ADDR
{
	DWORD entry;
	DWORD endaddr;
};
class MemoryScan {
public:
	void Install();
	DWORD GetMoudleEnd(DWORD start);
	void Anomaly_check(MEMORY_BASIC_INFORMATION memory_information);
	void StartScan();
	MOUDLES_ADDR entry[255];
	int i = 0;
//	bool firstScan = false;
	//std::list<std::list<DWORD>> MoudleAddr;
};
extern MemoryScan* MMSCAN;