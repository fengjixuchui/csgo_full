#pragma once
#include <Windows.h> 
#include <comutil.h>
#include "unity.h"


typedef LONG NTSTATUS;

//
// Loader Data Table Entry
//
typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union
	{
		LIST_ENTRY HashLinks;
		PVOID SectionPointer;
	};
	ULONG CheckSum;
	union
	{
		ULONG TimeDateStamp;
		PVOID LoadedImports;
	};
	PVOID EntryPointActivationContext;
	PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _PEB_LDR_DATA {
	ULONG                   Length;
	BOOLEAN                 Initialized;
	PVOID                   SsHandle;
	LIST_ENTRY              InLoadOrderModuleList;          //按加载顺序
	LIST_ENTRY              InMemoryOrderModuleList;        //按内存顺序
	LIST_ENTRY              InInitializationOrderModuleList;//按初始化顺序
	PVOID          EntryInProgress;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

//每个模块信息的LDR_MODULE部分
typedef struct _LDR_MODULE {
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
	PVOID                   BaseAddress;
	PVOID                   EntryPoint;
	ULONG                   SizeOfImage;
	UNICODE_STRING          FullDllName;
	UNICODE_STRING          BaseDllName;
	ULONG                   Flags;
	SHORT                   LoadCount;
	SHORT                   TlsIndex;
	LIST_ENTRY              HashTableEntry;
	ULONG                   TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;

VOID HideModule(HMODULE hLibrary)
{
	PPEB_LDR_DATA	pLdr = NULL;
	PLDR_MODULE		FirstModule = NULL;
	PLDR_MODULE		GurrentModule = NULL;
	__try
	{
		__asm
		{
			mov esi, fs:[0x30]
			mov esi, [esi + 0x0C]
			mov pLdr, esi
		}

		FirstModule = (PLDR_MODULE)(pLdr->InLoadOrderModuleList.Flink);
		GurrentModule = FirstModule;
		while (!(GurrentModule->BaseAddress == hLibrary))
		{
			GurrentModule = (PLDR_MODULE)(GurrentModule->InLoadOrderModuleList.Blink);
			if (GurrentModule == FirstModule)
				break;
		}
		if (GurrentModule->BaseAddress != hLibrary)
			return;

		//Dll解除链接
		((PLDR_MODULE)(GurrentModule->InLoadOrderModuleList.Flink))->InLoadOrderModuleList.Blink = GurrentModule->InLoadOrderModuleList.Blink;
		((PLDR_MODULE)(GurrentModule->InLoadOrderModuleList.Blink))->InLoadOrderModuleList.Flink = GurrentModule->InLoadOrderModuleList.Flink;

		memset(GurrentModule->FullDllName.Buffer, 0, GurrentModule->FullDllName.Length);
		memset(GurrentModule, 0, sizeof(PLDR_MODULE));
	}

	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}
}

//LdrLoadDll function prototype
typedef NTSTATUS(WINAPI *fLdrLoadDll)(IN PWCHAR PathToFile OPTIONAL, IN ULONG Flags OPTIONAL, IN PUNICODE_STRING ModuleFileName, OUT PHANDLE ModuleHandle);

//RtlInitUnicodeString function prototype
typedef VOID(WINAPI *fRtlInitUnicodeString)(PUNICODE_STRING DestinationString, PCWSTR SourceString);


HMODULE					hntdll;
fLdrLoadDll				_LdrLoadDll;
fRtlInitUnicodeString	_RtlInitUnicodeString;

HMODULE LoadDll(LPCSTR lpFileName)
{
	if (hntdll == NULL)
	{
		hntdll = GetModuleHandleA("ntdll.dll");
	}

	if (_LdrLoadDll == NULL)
	{
		_LdrLoadDll = (fLdrLoadDll)GetProcAddress(hntdll, "LdrLoadDll");
	}

	if (_RtlInitUnicodeString == NULL)
	{
		_RtlInitUnicodeString = (fRtlInitUnicodeString)GetProcAddress(hntdll, "RtlInitUnicodeString");
	}
	int StrLen = lstrlenA(lpFileName);
	BSTR WideStr = SysAllocStringLen(NULL, StrLen);
	MultiByteToWideChar(CP_ACP, 0, lpFileName, StrLen, WideStr, StrLen);
	UNICODE_STRING usDllName;
	_RtlInitUnicodeString(&usDllName, WideStr);
	SysFreeString(WideStr);
	HANDLE DllHandle;
	_LdrLoadDll(0, 0, &usDllName, &DllHandle);
	return (HMODULE)DllHandle;
}
