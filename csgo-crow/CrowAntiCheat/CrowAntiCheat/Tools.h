#pragma once
#include "unity.h"
using MsgFn = void(__cdecl*)(char const*, ...);
namespace T
{
	extern MsgFn PrintMessage;
	typedef void* (__cdecl* CreateInterface_t)(const char*, int*);
	typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);
	static void* CaptureInterface(std::string moduleName, std::string interfaceName)
	{
		
		void* foundInterface = nullptr;
		char possibleInterfaceName[512];
		
		CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(moduleName.c_str()), "CreateInterface");
		for (int i = 0; i < 100; i++)
		{

			sprintf(possibleInterfaceName, "%s0%i", interfaceName.c_str(), i);
			foundInterface = (void*)CreateInterface(possibleInterfaceName, NULL);
			if (foundInterface != nullptr)
			{
				break;
			}
			
			sprintf(possibleInterfaceName, "%s00%i", interfaceName.c_str(), i);
			foundInterface = (void*)CreateInterface(possibleInterfaceName, NULL);
			if (foundInterface != nullptr)
			{
				break;
			}

		}
		return foundInterface;
		
	}
	template< typename T >
	static T* CaptureInterfaceStatic(std::string strModule, std::string strInterface)
	{
		typedef T* (*CreateInterfaceFn)(const char* szName, int iReturn);
		CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(strModule.c_str()), "CreateInterface");

		return CreateInterface(strInterface.c_str(), 0);
	}

	static DWORD FindPattern(std::string moduleName, std::string pattern)
	{
		const char* pat = pattern.c_str();
		DWORD firstMatch = 0;
		DWORD rangeStart = (DWORD)GetModuleHandleA(moduleName.c_str());
		MODULEINFO miModInfo;
		GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
		DWORD rangeEnd = rangeStart + miModInfo.SizeOfImage;
		for (DWORD pCur = rangeStart; pCur < rangeEnd; pCur++)
		{
			if (!*pat)
				return firstMatch;

			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat))
			{
				if (!firstMatch)
					firstMatch = pCur;

				if (!pat[2])
					return firstMatch;

				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
					pat += 3;

				else
					pat += 2; //one ?
			}
			else
			{
				pat = pattern.c_str();
				firstMatch = 0;
			}
		}
		return NULL;
	}

	static DWORD GetVTCount(PDWORD pdwVMT)
	{
		DWORD dwIndex = 0;

		for (dwIndex = 0; pdwVMT[dwIndex]; dwIndex++)
		{
			if (IsBadCodePtr((FARPROC)pdwVMT[dwIndex]))
			{
				break;
			}
		}
		return dwIndex;
	}
}