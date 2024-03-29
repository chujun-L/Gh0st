// IOCPServer.h: interface for the CIOCPServer class.
//

#pragma once

#include <winsock2.h>
#include <MSTcpIP.h>
#pragma comment(lib,"ws2_32.lib")
#include "Buffer/Buffer.h"
#include "CpuUsage/CpuUsage.h"


#include <process.h>

#include <afxtempl.h>



////////////////////////////////////////////////////////////////////
#define	NC_CLIENT_CONNECT		0x0001
#define	NC_CLIENT_DISCONNECT	0x0002
#define	NC_TRANSMIT				0x0003
#define	NC_RECEIVE				0x0004
#define NC_RECEIVE_COMPLETE		0x0005 // 完整接收

class CLock
{
public:
	CLock(CRITICAL_SECTION& cs, const CString& strFunc) {
		m_strFunc = strFunc;
		m_pcs = &cs;
		Lock();
	}

	~CLock() {
		Unlock();
	}
	
	void Unlock() {
		LeaveCriticalSection(m_pcs);
		TRACE(_T("LC %d %s\n") , GetCurrentThreadId() , m_strFunc);
	}

	void Lock() {
		TRACE(_T("EC %d %s\n") , GetCurrentThreadId(), m_strFunc);
		EnterCriticalSection(m_pcs);
	}

protected:
	CRITICAL_SECTION*	m_pcs;
	CString				m_strFunc;
};

enum IOType {
	IOInitialize,
	IORead,
	IOWrite,
	IOIdle
};


class OVERLAPPEDPLUS 
{
public:
	OVERLAPPED			m_ol;
	IOType				m_ioType;

	// 构造函数
	OVERLAPPEDPLUS(IOType ioType) {
		ZeroMemory(this, sizeof(OVERLAPPEDPLUS));
		m_ioType = ioType;
	}
};


struct ClientContext
{
    SOCKET				m_Socket;              //套接字
	// Store buffers
	CBuffer				m_WriteBuffer;
	CBuffer				m_CompressionBuffer;	// 接收到的压缩的数据
	CBuffer				m_DeCompressionBuffer;	// 解压后的数据
	CBuffer				m_ResendWriteBuffer;	// 上次发送的数据包，接收失败时重发时用

	int					m_Dialog[2]; // 放对话框列表用，第一个int是类型，第二个是CDialog的地址
	int					m_nTransferProgress;
	// Input Elements for Winsock
	WSABUF				m_wsaInBuffer;
	BYTE				m_byInBuffer[8192];

	// Output elements for Winsock
	WSABUF				m_wsaOutBuffer;
	HANDLE				m_hWriteComplete;

	// Message counts... purely for example purposes
	LONG				m_nMsgIn;
	LONG				m_nMsgOut;	

	BOOL				m_bIsMainSocket; // 是不是主socket

	ClientContext*		m_pWriteContext;
	ClientContext*		m_pReadContext;

	// 初始化结构体
	ClientContext() {
		memset(this, 0, sizeof(ClientContext));
	}
};

template<>
inline UINT AFXAPI HashKey(CString & strGuid)
{
  return HashKey( (LPCTSTR) strGuid);         
}

#include "Others/Mapper.h"

typedef void (CALLBACK* NOTIFYPROC)(LPVOID, ClientContext*, UINT nCode);

typedef CList<ClientContext*, ClientContext* > ContextList;


class CMainFrame;

class CIOCPServer
{
public:
	CIOCPServer();
	virtual ~CIOCPServer();

	// 线程原子操作
	LONG m_nCurrentThreads;
	LONG m_nBusyThreads;

	NOTIFYPROC m_pNotifyProc;
	CMainFrame *m_pFrame;

	UINT m_nMaxConnections;
	
	// 网络的初始化
	bool Initialize(NOTIFYPROC pNotifyProc, CMainFrame *pFrame,
		int nMaxConnections, int nPort);

	static unsigned __stdcall ListenThreadProc(LPVOID lpVoid);
	static unsigned __stdcall ThreadPoolFunc(LPVOID WorkContext);
	static CRITICAL_SECTION	m_cs;

	void Send(ClientContext* pContext, LPBYTE lpData, UINT nSize);
	void PostRecv(ClientContext* pContext);

	bool IsRunning();
	void Shutdown();
	void ResetConnection(ClientContext* pContext);
	void DisconnectAll();
	


	
	UINT				m_nSendKbps; // 发送即时速度
	UINT				m_nRecvKbps; // 接受即时速度
	

protected:
	// Thread Pool Tunables
	LONG m_nThreadPoolMin;
	LONG m_nThreadPoolMax;
	// cpu usage rate
	CCpuUsage m_cpu;
	LONG m_nCPULoThreshold;
	LONG m_nCPUHiThreshold;

	LONG m_nWorkerThreadCnt;
	
	bool m_bTimeToKill;

	// Initialize()
	HANDLE m_hKillEvent;  // 在构造函数创建事件
	HANDLE m_hThread;
	WSAEVENT m_hEvent;
	
	// IOCP
	HANDLE m_hCompletionPort;

	bool InitializeIOCP(void);


	void InitializeClientRead(ClientContext* pContext);
	BOOL AssociateSocketWithCompletionPort(SOCKET device, HANDLE hCompletionPort, DWORD dwCompletionKey);
	void RemoveStaleClient(ClientContext* pContext, BOOL bGraceful);
	void MoveToFreePool(ClientContext *pContext);
	ClientContext*  AllocateContext();

	

	bool				m_bInit;
	bool				m_bDisconnectAll;
	BYTE				m_bPacketFlag[5];
	void CloseCompletionPort();
	void OnAccept();
	
	void Stop();

	ContextList			m_listContexts;
	ContextList			m_listFreePool;
	
	SOCKET				m_socListen;    
    
	
	
	
	

	LONG				m_nKeepLiveTime; // 心跳超时




	CString GetHostName(SOCKET socket);

	void CreateStream(ClientContext* pContext);

	BEGIN_IO_MSG_MAP()
		IO_MESSAGE_HANDLER(IORead, OnClientReading)
		IO_MESSAGE_HANDLER(IOWrite, OnClientWriting)
		IO_MESSAGE_HANDLER(IOInitialize, OnClientInitializing)
	END_IO_MSG_MAP()

	bool OnClientInitializing	(ClientContext* pContext, DWORD dwSize = 0);
	bool OnClientReading		(ClientContext* pContext, DWORD dwSize = 0);
	bool OnClientWriting		(ClientContext* pContext, DWORD dwSize = 0);

};