#pragma once

#include "IOCPServer/IOCPServer.h"

// CWindowDlg 对话框

class CWindowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWindowDlg)

private:
	HICON			m_hIcon;
	CIOCPServer		*m_iocpServer;
	ClientContext	*m_pContext;

public:
	CWindowDlg(CWnd* pParent = nullptr,
				CIOCPServer *pIOCPServer = nullptr,
				ClientContext *pContext = nullptr);   // 标准构造函数

	virtual			~CWindowDlg();
	virtual BOOL	OnInitDialog();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnNMRClickListWindow(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnWslistRefresh();
	afx_msg void	OnWslistClose();
	afx_msg void	OnWslistHide();
	afx_msg void	OnWslistReturn();
	afx_msg void	OnWslistMax();
	afx_msg void	OnWslistMin();

			void	ReSize();
			void	GetWindowList();
			void	OnReceiveComplete();
			void	ShowWindowList();

		CListCtrl	m_listWindow;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_WINDOW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
