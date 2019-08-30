#include "stdafx.h"
#include "unity.h"
#include "DllHider.h"
extern VOID HideModule(HMODULE hLibrary);
extern HMODULE LoadDll(LPCSTR lpFileName);
DWORD HookScaner::GetMoudleEnd(DWORD start)
{
	MODULEINFO miModInfo;
	GetModuleInformation(GetCurrentProcess(), (HMODULE)start, &miModInfo, sizeof(MODULEINFO));
	return (start + miModInfo.SizeOfImage);
}
void HookScaner::InstallScanner()
{
	MMSCAN->Install();
	//检查VMT虚表HOOK
	module_entry_client.modBaseAddr = (DWORD)GetModuleHandleA(XorString("client_panorama.dll"));
	module_entry_client.modBaseEndAddr = GetMoudleEnd(module_entry_client.modBaseAddr);
	module_entry_engine.modBaseAddr = (DWORD)GetModuleHandleA(XorString("engine.dll"));
	module_entry_engine.modBaseEndAddr = GetMoudleEnd(module_entry_engine.modBaseAddr);
	module_entry_vgui2.modBaseAddr = (DWORD)GetModuleHandleA(XorString("vgui2.dll"));
	module_entry_vgui2.modBaseEndAddr = GetMoudleEnd(module_entry_vgui2.modBaseAddr);
	module_entry_vguimatsurface.modBaseAddr = (DWORD)GetModuleHandleA(XorString("vguimatsurface.dll"));
	module_entry_vguimatsurface.modBaseEndAddr = GetMoudleEnd(module_entry_vguimatsurface.modBaseAddr);
	module_entry_D3DX.modBaseAddr = (DWORD)GetModuleHandleA(XorString("shaderapidx9.dll"));
	module_entry_D3DX.modBaseEndAddr = GetMoudleEnd(module_entry_D3DX.modBaseAddr);
	
	module_entry_comctl32.modBaseAddr = (DWORD)GetModuleHandleA(XorString("comctl32.dll"));
	module_entry_comctl32.modBaseEndAddr = GetMoudleEnd(module_entry_comctl32.modBaseAddr);

	I::Client = (DWORD*)T::CaptureInterface(XorString("client_panorama.dll"), XorString("VClient"));
	I::ClientMode = **(DWORD***)((*(DWORD**)I::Client)[10] + 0x5);
	I::VPanel = T::CaptureInterfaceStatic<DWORD>(XorString("vgui2.dll"), XorString("VGUI_Panel009"));
	I::ModelRender = T::CaptureInterfaceStatic< DWORD >(XorString("engine.dll"), XorString("VEngineModel016"));
	I::Surface = T::CaptureInterfaceStatic< DWORD >(XorString("vguimatsurface.dll"), XorString("VGUI_Surface031"));
	I::Engine = T::CaptureInterfaceStatic< IEngineClient >(XorString("engine.dll"), XorString("VEngineClient014"));
	//I::ClientEntList = (IClientEntityList*)T::CaptureInterface(XorString("client_panorama.dll"), XorString("VClientEntityList"));
	DWORD d3d9Device = **(DWORD**)(T::FindPattern(XorString("shaderapidx9.dll"), XorString("A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C")) + 1);

	m_VMTList.CreateMove_Client_VTSize = T::GetVTCount(*(DWORD**)I::ClientMode);
	m_VMTList.CreateMove_Client = *(DWORD**)I::ClientMode;
	m_VMTList.PaintTraverse = *(DWORD**)I::VPanel;
	m_VMTList.PaintTraverse_VTSize = T::GetVTCount(*(DWORD**)I::VPanel);
	m_VMTList.Client = *(DWORD**)I::Client;
	m_VMTList.Client_VTSize = T::GetVTCount(*(DWORD**)I::Client);
	m_VMTList.ModelRender = *(DWORD**)I::ModelRender;
	m_VMTList.ModelRender_VTSize = T::GetVTCount(*(DWORD**)I::ModelRender);
	m_VMTList.Surface = *(DWORD**)I::Surface;
	m_VMTList.Surface_VTSize = T::GetVTCount(*(DWORD**)I::Surface);
	m_VMTList.D3DDriver = *(DWORD**)d3d9Device;
	m_VMTList.D3DDriver_VTSize = T::GetVTCount(*(DWORD**)d3d9Device);
	dwModelRender = *(DWORD**)I::ModelRender;
	//得到地址后马上隐藏dll
	/*
	HideModule(LoadDll(XorString("shaderapidx9.dll")));
	HideModule(LoadDll(XorString("d3d9.dll")));
	HideModule(LoadDll(XorString("d3d9_43.dll")));
	HideModule(LoadDll(XorString("vgui2.dll")));
	
	*/
}

void HookScaner::CheckVMTHook()
{
	//抄BE的代码
	int vtable_index = 0;
	for (vtable_index = 0; vtable_index < m_VMTList.CreateMove_Client_VTSize; vtable_index++)
	{
		if (*(DWORD*)&m_VMTList.CreateMove_Client[vtable_index] < module_entry_client.modBaseAddr ||
			*(DWORD*)&m_VMTList.CreateMove_Client[vtable_index] >= module_entry_client.modBaseEndAddr
			|| m_VMTList.CreateMove_Client[0] == 0xCC )
		{
			T::PrintMessage("DECTETION ClientMode<client_panorama.dll> VMT HOOK! \n");
		}
	}
	for (vtable_index = 0; vtable_index < m_VMTList.PaintTraverse_VTSize; vtable_index++)
	{
		if (*(DWORD*)&m_VMTList.PaintTraverse[vtable_index] < module_entry_vgui2.modBaseAddr ||
			*(DWORD*)&m_VMTList.PaintTraverse[vtable_index] >= module_entry_vgui2.modBaseEndAddr
			|| m_VMTList.PaintTraverse[0] == 0xCC)
		{
			//nvapi.dll
			if(vtable_index != 67)
				T::PrintMessage("DECTETION PaintTraverse<vgui2.dll> VMT HOOK!  HookAddr: 0x%08X in index: %d \n", m_VMTList.PaintTraverse[vtable_index], vtable_index);
		}
	}
	for (vtable_index = 0; vtable_index < m_VMTList.Client_VTSize; vtable_index++)
	{
		if (*(DWORD*)&m_VMTList.Client[vtable_index] < module_entry_client.modBaseAddr ||
			*(DWORD*)&m_VMTList.Client[vtable_index] >= module_entry_client.modBaseEndAddr
			|| m_VMTList.Client[0] == 0xCC)
		{
			T::PrintMessage("DECTETION Client<client_panorama.dll> VMT HOOK! \n");
		}
	}
	for (vtable_index = 0; vtable_index < m_VMTList.Surface_VTSize; vtable_index++)
	{
		if (*(DWORD*)&m_VMTList.Surface[vtable_index] < module_entry_vguimatsurface.modBaseAddr ||
			*(DWORD*)&m_VMTList.Surface[vtable_index] >= module_entry_vguimatsurface.modBaseEndAddr
			|| m_VMTList.Surface[0] == 0xCC)
		{
			T::PrintMessage("DECTETION Surface<vguimatsurface.dll> VMT HOOK! \n");
		}
	}
	for (vtable_index = 0; vtable_index < m_VMTList.ModelRender_VTSize; vtable_index++)
	{
		if (*(DWORD*)&m_VMTList.ModelRender[vtable_index] < module_entry_engine.modBaseAddr ||
			*(DWORD*)&m_VMTList.ModelRender[vtable_index] >= module_entry_engine.modBaseEndAddr
			|| m_VMTList.ModelRender[0] == 0xCC)
		{
			//第二轮检查是否在comctl32里
			if (*(DWORD*)&m_VMTList.ModelRender[vtable_index] < module_entry_comctl32.modBaseAddr ||
				*(DWORD*)&m_VMTList.ModelRender[vtable_index] >= module_entry_comctl32.modBaseEndAddr)
			{
				T::PrintMessage("DECTETION ModelRender<engine.dll> VMT HOOK! HookAddr: 0x%08X in index: %d \n", m_VMTList.ModelRender[vtable_index], vtable_index);
			}
			
		}
	}
	/*
	for (vtable_index = 0; vtable_index < m_VMTList.D3DDriver_VTSize; vtable_index++)
	{
		if (*(int*)&m_VMTList.D3DDriver[vtable_index] < module_entry_D3DX.modBaseAddr ||
			*(int*)&m_VMTList.D3DDriver[vtable_index] >= module_entry_D3DX.modBaseEndAddr)
		{
			//只检查16和42
			if(vtable_index == 16 || vtable_index == 42)
				T::PrintMessage("DECTETION D3DDriver<shaderapidx9.dll> VMT HOOK! \n");
		}
	}*/
}
