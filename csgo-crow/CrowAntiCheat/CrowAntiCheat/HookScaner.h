#pragma once
#include "unity.h"
struct VIRTUALTABLE_LIST
{
	DWORD* CreateMove_Client;
	int CreateMove_Client_VTSize;
	DWORD* PaintTraverse;
	int PaintTraverse_VTSize;
	DWORD* Client;
	int Client_VTSize;
	DWORD* ModelRender;
	int ModelRender_VTSize;
	DWORD* Surface;
	int Surface_VTSize;
	DWORD* D3DDriver;
	int D3DDriver_VTSize;
};
struct MoudleInfo
{
	DWORD modBaseAddr;
	DWORD modBaseEndAddr;
};
class HookScaner {
public: 
	MoudleInfo module_entry_client;
	MoudleInfo module_entry_engine;
	MoudleInfo module_entry_vgui2;
	MoudleInfo module_entry_vguimatsurface;
	MoudleInfo module_entry_D3DX;
	MoudleInfo module_entry_comctl32;
	VIRTUALTABLE_LIST m_VMTList;
	DWORD GetMoudleEnd(DWORD start);
	DWORD* dwModelRender;
	void InstallScanner();
	void CheckVMTHook();
};
extern HookScaner* hkScanerEngine;