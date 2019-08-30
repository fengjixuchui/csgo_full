// AntiCheatLoader.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "unity.h"
void DoTCPserver() 
{
	ServerEngine->CreateTCPserver();
}
int main()
{
	printf("                 $$$$$$                        \n");
	printf("                $$$$(a)$$::7                   \n");
	printf("                $$$$$$$$:/                     \n");
	printf("              $$$$$$$$,                        \n");
	printf("            §??`?? ?`???$$,                    \n");
	printf("          §??`??`????$$$$$$,                   \n");
	printf("          §???????$$$$$$$$$$s                  \n");
	printf("          $?`??`???$$$$$$$$$ $$$,              \n");
	printf("          §?????????$$$$$$$$$$$$s,,,,          \n");
	printf("           `j§???`????`?$$$$$$$$$$$$$$s,,      \n");
	printf("             ``§??? ??????$$$$$$    $$$$$$s,,  \n");
	printf("                ``r§?????  $$$$        $$$$$$  \n");
	printf("                 //   //       $$              \n");
	printf("                //-   //                       \n");
	printf(" ╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋╋\n");
	printf("[SYSTEM]CrowAntiCheat staring ... \n");
	/*
	if (!ClientEngine->ConnectAntiCheatServer())
	{
		printf("[ClientEngine]Cant Connect to Server...\n");
		system("pause");
		return 0;
	}*/
	printf("[CreateTCPserver]Starting CreateTCPserver...\n");
//	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DoTCPserver, NULL, NULL, NULL);
	printf("[AntiCheatEngine]Starting AntiCheatEngine...\n");
//	AntiCheatEngine->Work();
//	SigScaner->GetSig("C:\\ApexLoadInject.exe");
	ServerEngine->CreateTCPserver();
	system("pause");
    return 0;
}
