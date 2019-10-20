#pragma once

#include "IOCPServer/IOCPServer.h"

// CShellDlg 对话框

class CShellDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShellDlg)

public:
	CShellDlg(CWnd* pParent = nullptr,
			CIOCPServer *pIOCPServer = nullptr,
			ClientContext *pContext = nullptr);   // 标准构造函数
	virtual ~CShellDlg();

	CEdit			m_edit;

	void			OnReceiveComplete();

	virtual BOOL	OnInitDialog();
	virtual BOOL	PreTranslateMessage(MSG *pMsg);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SHELL };
#endif

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:
	HICON			m_hIcon;
	ClientContext	*m_pContext;
	CIOCPServer		*m_iocpServer;
	UINT			m_nCurSel;
	UINT			m_nReceiveLength;
	void			AddKeyBoardData();

public:
	afx_msg void	OnEnChangeEditShell();
	afx_msg void	OnClose();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH	OnCtlColor(CDC *pDC, CWnd *pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
};
