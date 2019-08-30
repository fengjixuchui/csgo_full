#pragma once
#include <vector>
#include <windows.h>
#include <TlHelp32.h>
#include <string>
#include <sstream>
#include <fstream>
#include <Psapi.h>
#include <lmcons.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <shlobj.h>
#include <time.h>
#include <random>
#include <sstream>
#include <fstream>
#include <shlwapi.h>
#include <iomanip>
#include <ctime>
#include <memory>
#include <assert.h>
#define INRANGE(x,a,b)    (x >= a && x <= b)
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))
#include "xorstr.hpp"
#include "IEngineClient.h"
#include "Tools.h"
#include "HookScaner.h"
#include "MemoryScan.h"


namespace I
{
	extern DWORD* ClientMode;
	extern DWORD* Client;
	extern DWORD* VPanel;
	extern DWORD* ModelRender;
	extern DWORD* Surface;
	extern IEngineClient* Engine;
}
namespace G
{
	extern DWORD MOUDLE;
	extern DWORD MOUDLE_END;
}