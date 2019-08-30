#pragma once
#include "unity.h"


class c_InGame {
public:
	DWORD WatchThread(LPVOID Param,int size);
	void ScanRadarHack();
	void InGameScaner();
};