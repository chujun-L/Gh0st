#pragma once

// main toolbar
#define UI_BTNWIDTH					48
#define UI_BTNWIDTH_OFFSET			10
#define UI_MAIN_TOOLBAR_BOTTOM		80


#define UI_BUTTON_NULL				9		// ��9������Ϊ��
#define UI_BUTTON_COUNT				13

const CString ButtonTextArray[UI_BUTTON_COUNT] = {
	"�ն˹���",
	"���̹���",
	"���ڹ���",
	"�������",
	"�ļ�����",
	"��������",
	"��Ƶ����",
	"�������",
	"ע������",
	"",
	"��������",
	"���ɷ����",
	"����"
};


// list ctrol
#define UI_ONLINE_COLUMNS			7
#define UI_ONLINE_LEFT				1
#define UI_ONLINE_TOP				80
#define UI_ONLINE_RIGHT				1
#define UI_ONLINE_BOTTOM			160

#define UI_EVENTLOG_COLUMNS			3
#define UI_EVENTLOG_LEFT			1
#define UI_EVENTLOG_TOP				156
#define UI_EVENTLOG_RIGHT			1
#define UI_EVENTLOG_BOTTOM			20

typedef struct {
	const CHAR *chTitle;
	INT			nWidth;
}UI_COLUMN, *PUI_COLUMN;

UI_COLUMN g_uiOnlineColumn[] = {
	{"IP",			148},
	{"����",			150},
	{"�������",		160},
	{"����ϵͳ",		128},
	{"CPU",			80},
	{"����ͷ",		81},
	{"PING",		81}
};

UI_COLUMN g_uiEventLogColumn[] = {
	{"ʱ��",			100},
	{"�¼�����",		68},
	{"�¼�����",		660}
};


// status bar
#define UI_STATUSBAR_TOP			20
#define UI_STATUSBAR_OFFSET			10

static UINT g_nIndicators[] = {
	IDR_STATUSBAR_STRING,
};


// �Զ�����Ϣ
enum {	
	UM_ICONNOTIFY = WM_USER + 0x100,
};


// network
#define DEFAULT_PORT				8080
#define DEFAULT_MAX_CONNECTION		999

enum
{
	WM_CLIENT_CONNECT = WM_APP + 0x1001,
	WM_CLIENT_CLOSE,
	WM_CLIENT_NOTIFY,
	WM_DATA_IN_MSG,
	WM_DATA_OUT_MSG,

	WM_ADDONLINE = WM_USER + 102,	// ��ӵ��б���ͼ��
	WM_REMOVEFROMLIST,				// ���б���ͼ��ɾ��
	WM_OPENMANAGERDIALOG,			// ��һ���ļ�������
	WM_OPENSCREENSPYDIALOG,			// ��һ����Ļ���Ӵ���
	WM_OPENWEBCAMDIALOG,			// ������ͷ���Ӵ���
	WM_OPENAUDIODIALOG,				// ��һ��������������
	WM_OPENKEYBOARDDIALOG,			// �򿪼��̼�¼����
	WM_OPENPSLISTDIALOG,			// �򿪽��̹�����
	WM_OPENSHELLDIALOG,				// ��shell����
	WM_RESETPORT,					// �ı�˿�

	FILEMANAGER_DLG = 1,
	SCREENSPY_DLG,
	WEBCAM_DLG,
	AUDIO_DLG,
	KEYBOARD_DLG,
	//SYSTEM_DLG,
	PSLIST_DLG,
	WSLIST_DLG,
	SHELL_DLG
};