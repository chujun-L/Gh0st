
// UIDlg.h: 头文件
//

#pragma once

#include "TrueColorToolBar.h"

// CUIDlg 对话框
class CUIDlg : public CDialogEx
{
public:
	// 标准构造函数
	CUIDlg(CWnd* pParent = nullptr);	

	// UI包括: 1.菜单 2.工具栏 3.在线主机列表 4.事件日志列表 5.状态栏 6.托盘显示
	void LoadUI();
	void InitMainMenu();
	void InitToolBar();
	void InitOnlineListCtrl();
	void AddOnline();
	void InitEventLogListCtrl();
	void AddEventLog();
	void InitStatusBar();
	void InitNotify();
	CString GetSystemTime();

	// 自定义消息处理
	afx_msg void OnIconNotify(WPARAM wParam, LPARAM lParam);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMainSetting();
	afx_msg void OnMianClose();
	afx_msg void OnNMRClickListOnline(NMHDR *pNMHDR, LRESULT *pResult);


// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON				m_hIcon;
	HMENU				m_hMenu;
	CTrueColorToolBar	m_tcTooBar;
	CListCtrl			m_lcOnline;
	CListCtrl			m_lcEventLog;
	UINT				m_nWndWidth;
	CStatusBar			m_sbStatusBar;
	NOTIFYICONDATA		m_nifNotify;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()	
};
