#include "stdafx.h"
#include "NoBugZ.h"
#include "unity.h"
void NoBugZ::ClientBugReport()
{
	printf(XorString("[SYSTEM]反作弊异常...请联系管理员 \n"));
	closesocket(G::ClientSocket);
	system(XorString("pause"));
	exit(0);
}
