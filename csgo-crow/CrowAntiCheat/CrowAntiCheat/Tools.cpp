#include "stdafx.h"
#include "Tools.h"
MsgFn T::PrintMessage = (MsgFn)GetProcAddress(GetModuleHandleA(XorString("tier0.dll")), XorString("Msg"));