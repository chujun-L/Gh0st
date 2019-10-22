#pragma once

#include "IOCPServer/IOCPServer.h"

// CProcessDlg 对话框

class CProcessDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessDlg)

private:
	HICON			m_hIcon;
	CIOCPServer		*m_iocpServer;
	ClientContext	*m_pContext;

			void	GetProcessList();
			void	ReSize();
			void	ShowProcessList();

public:
	CProcessDlg(CWnd* pParent = nullptr,
			   CIOCPServer *pIOCPServer = nullptr,
		       ClientContext *pContext = nullptr);   // 标准构造函数

	virtual			~CProcessDlg();
	virtual BOOL	OnInitDialog();

	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnClose();
	afx_msg void	OnPslistKill();
	afx_msg void	OnPslistReflesh();
	afx_msg void	OnNMRClickListProcess(NMHDR *pNMHDR, LRESULT *pResult);

	CListCtrl		m_ListProcess;

			void	OnReceiveComplete();
	

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PROCESS };
#endif

protected:
	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
