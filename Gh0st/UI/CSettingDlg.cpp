// CSettingDlg.cpp: 实现文件
//

#include "pch.h"
#include "UI.h"
#include "CSettingDlg.h"
#include "afxdialogex.h"


// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSettingDlg, CDialogEx)

CSettingDlg::CSettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN_SETTING, pParent)
	, m_nPort(0)
	, m_nMaxConnection(0)
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDV_MinMaxUInt(pDX, m_nPort, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_MAXCONNECTION, m_nMaxConnection);
	DDV_MinMaxUInt(pDX, m_nMaxConnection, 1, 10000);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSettingDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSettingDlg 消息处理程序


BOOL CSettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 读取ini文件
	CIniFile &IniFile = ((CUIApp *)AfxGetApp())->m_iniFile;
	m_nPort			  = (UINT)(IniFile.GetInt("Settings", "ListenPort"));
	m_nMaxConnection  = (UINT)(IniFile.GetInt("Settings", "MaxConnection"));
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


// 生成ini文件
void CSettingDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	CIniFile &IniFile = ((CUIApp *)AfxGetApp())->m_iniFile;
	IniFile.SetInt("Settings", "ListenPort", m_nPort);
	IniFile.SetInt("Settings", "MaxConnection", m_nMaxConnection);
	AfxMessageBox("参数设置成功，重启程序生效");

	CDialogEx::OnOK();
}
