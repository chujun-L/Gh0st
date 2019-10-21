// CSystemDlg.cpp: 实现文件
//

#include "pch.h"
#include "UI.h"
#include "CSystemDlg.h"
#include "afxdialogex.h"
#include "Others/macros.h"


// CSystemDlg 对话框

IMPLEMENT_DYNAMIC(CSystemDlg, CDialogEx)

void CSystemDlg::GetProcessList()
{
	BYTE bToken = COMMAND_PSLIST;
	m_iocpServer->Send(m_pContext, &bToken, 1);
}

void CSystemDlg::ReSize()
{
	if (m_ListProcess.m_hWnd == NULL &&
		m_ListWindows.m_hWnd == NULL)
	{
		return;
	}

	RECT client;
	RECT fill;

	GetClientRect(&client);
	fill.left = 0;
	fill.top = 29;
	fill.right = client.right;
	fill.bottom = client.bottom;

	m_ListProcess.MoveWindow(&fill);
	m_ListWindows.MoveWindow(&fill);
}

void CSystemDlg::ShowProcessList()
{
	char	*lpBuffer = (char *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
	char	*strExeFile;
	char	*strProcessName;
	DWORD	dwOffset = 0;
	CString str;
	int		i = 0;

	m_ListProcess.DeleteAllItems();

	for (i = 0;
		 dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; 
		 i++)
	{
		LPDWORD	lpPID = LPDWORD(lpBuffer + dwOffset);
		strExeFile = lpBuffer + dwOffset + sizeof(DWORD);
		strProcessName = strExeFile + lstrlen(strExeFile) + 1;

		m_ListProcess.InsertItem(i, strExeFile);
		str.Format("%5u", *lpPID);
		m_ListProcess.SetItemText(i, 1, str);
		m_ListProcess.SetItemText(i, 2, strProcessName);
		// ItemData 为进程ID
		m_ListProcess.SetItemData(i, *lpPID);

		dwOffset += sizeof(DWORD) + lstrlen(strExeFile) + lstrlen(strProcessName) + 2;
	}

	str.Format("程序路径 / %d", i);
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT;
	lvc.pszText = str.GetBuffer(0);
	lvc.cchTextMax = str.GetLength();
	m_ListProcess.SetColumn(2, &lvc);
}

CSystemDlg::CSystemDlg(CWnd* pParent,
					   CIOCPServer *pIOCPServer,
					   ClientContext *pContext)
	: CDialogEx(IDD_DIALOG_SYSTEM, pParent)
{
	m_iocpServer = pIOCPServer;
	m_pContext = pContext;
	m_hIcon = LoadIcon(AfxGetInstanceHandle(), 
					   MAKEINTRESOURCE(IDI_SYSTEM));
}

CSystemDlg::~CSystemDlg()
{
}

void CSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_tab);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_ListProcess);
	DDX_Control(pDX, IDC_LIST_WINDOWS, m_ListWindows);
}


BEGIN_MESSAGE_MAP(CSystemDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CSystemDlg::OnTcnSelchangeTab)
	ON_COMMAND(ID_PSLIST_KILL, &CSystemDlg::OnPslistKill)
	ON_COMMAND(ID_PSLIST_REFLESH, &CSystemDlg::OnPslistReflesh)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROCESS, &CSystemDlg::OnNMRClickListProcess)
END_MESSAGE_MAP()


// CSystemDlg 消息处理程序


void CSystemDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	ReSize();
}


void CSystemDlg::OnClose()
{
	m_pContext->m_Dialog[0] = 0;
	closesocket(m_pContext->m_Socket);

	CDialogEx::OnClose();
}


void CSystemDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	switch (m_tab.GetCurSel()) {
	case 0:
		m_ListWindows.ShowWindow(SW_HIDE);
		m_ListProcess.ShowWindow(SW_SHOW);
		if (m_ListProcess.GetItemCount() == 0) {
			GetProcessList();
		}
		break;

	case 1:
		m_ListWindows.ShowWindow(SW_SHOW);
		m_ListProcess.ShowWindow(SW_HIDE);
		if (m_ListWindows.GetItemCount() == 0) {
			//	GetWindowsList();
		}
		break;
	}


	*pResult = 0;
}


BOOL CSystemDlg::OnInitDialog()
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
	title.Format("\\\\%s - 系统管理",
		         bResult != INVALID_SOCKET ? 
		         inet_ntoa(sockAddr.sin_addr) : "");
	SetWindowText(title);

	m_tab.InsertItem(0, "进程管理");
	m_tab.InsertItem(1, "窗口管理");

	m_ListProcess.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_ListProcess.InsertColumn(0, "映像名称", LVCFMT_LEFT, 100);
	m_ListProcess.InsertColumn(1, "PID", LVCFMT_LEFT, 50);
	m_ListProcess.InsertColumn(2, "程序路径", LVCFMT_LEFT, 400);

	m_ListWindows.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_ListWindows.InsertColumn(0, "PID", LVCFMT_LEFT, 50);
	m_ListWindows.InsertColumn(1, "窗口名称", LVCFMT_LEFT, 300);

	ReSize();

	return TRUE;
}


void CSystemDlg::OnPslistKill()
{
	CListCtrl *pListCtrl = nullptr;

	if (m_ListProcess.IsWindowVisible()) {
		pListCtrl = &m_ListProcess;
	} else if (m_ListWindows.IsWindowVisible()) {
		pListCtrl = &m_ListWindows;
	} else {
		return;
	}
		
	LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, 1 + (pListCtrl->GetSelectedCount() * 4));
	if (lpBuffer == NULL) {
		return;
	}

	lpBuffer[0] = COMMAND_KILLPROCESS;

	char *lpTips = "警告: 终止进程会导致不希望发生的结果，\n"
				   "包括数据丢失和系统不稳定。在被终止前，\n"
				   "进程将没有机会保存其状态和数据。";
	CString str;
	if (pListCtrl->GetSelectedCount() > 1) {
		str.Format("%s确实\n想终止这%d项进程吗?", lpTips, pListCtrl->GetSelectedCount());
	} else {
		str.Format("%s确实\n想终止该项进程吗?", lpTips);
	}
	if (::MessageBox(m_hWnd, str, "进程结束警告", MB_YESNO | MB_ICONQUESTION) == IDNO) {
		return;
	}
		
	DWORD	dwOffset = 1;
	POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
	if (pos) {
		int	nItem = pListCtrl->GetNextSelectedItem(pos);
		DWORD dwProcessID = pListCtrl->GetItemData(nItem);
		memcpy(lpBuffer + dwOffset, &dwProcessID, sizeof(DWORD));
		dwOffset += sizeof(DWORD);
	}
	m_iocpServer->Send(m_pContext, lpBuffer, LocalSize(lpBuffer));
	LocalFree(lpBuffer);
}


void CSystemDlg::OnPslistReflesh()
{
	if (m_ListProcess.IsWindowVisible()) {
		GetProcessList();
	}
		
	/*if (m_list_windows.IsWindowVisible())
		GetWindowsList();*/
}


void CSystemDlg::OnNMRClickListProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CMenu	popup;
	popup.LoadMenu(IDR_PSLIST);
	CMenu *pSub = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);

	pSub->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);

	*pResult = 0;
}

void CSystemDlg::OnReceiveComplete()
{
	switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0]) {
	case TOKEN_PSLIST:
		ShowProcessList();
		break;

	case TOKEN_WSLIST:
		//ShowWindowsList();
		break;

	default:
		// 传输发生异常数据
		break;
	}

}
