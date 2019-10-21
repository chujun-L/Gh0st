#pragma once

#include "IOCPServer/IOCPServer.h"

// CSystemDlg 对话框

class CSystemDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSystemDlg)

private:
	HICON			m_hIcon;
	CIOCPServer		*m_iocpServer;
	ClientContext	*m_pContext;

			void	GetProcessList();
			void	ReSize();
			void	ShowProcessList();

public:
	CSystemDlg(CWnd* pParent = nullptr,
			   CIOCPServer *pIOCPServer = nullptr,
		       ClientContext *pContext = nullptr);   // 标准构造函数

	virtual			~CSystemDlg();
	virtual BOOL	OnInitDialog();

	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnClose();
	afx_msg void	OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnPslistKill();
	afx_msg void	OnPslistReflesh();
	afx_msg void	OnNMRClickListProcess(NMHDR *pNMHDR, LRESULT *pResult);

	CTabCtrl		m_tab;
	CListCtrl		m_ListProcess;
	CListCtrl		m_ListWindows;

			void	OnReceiveComplete();
	

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SYSTEM };
#endif

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
