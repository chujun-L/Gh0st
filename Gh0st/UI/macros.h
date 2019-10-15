#pragma once

// main toolbar
#define UI_BTNWIDTH					48
#define UI_BTNWIDTH_OFFSET			10
#define UI_MAIN_TOOLBAR_BOTTOM		80


#define UI_BUTTON_NULL				9		// 第9个按键为空
#define UI_BUTTON_COUNT				13

const CString ButtonTextArray[UI_BUTTON_COUNT] = {
	"终端管理",
	"进程管理",
	"窗口管理",
	"桌面管理",
	"文件管理",
	"语音管理",
	"视频管理",
	"服务管理",
	"注册表管理",
	"",
	"参数设置",
	"生成服务端",
	"帮助"
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

UI_COLUMN g_uiOnlineClumn[] = {
	{"IP",			148},
	{"区域",			150},
	{"计算机名",		160},
	{"操作系统",		128},
	{"CPU",			80},
	{"摄像头",		81},
	{"PING",		81}
};

UI_COLUMN g_uiEventLogClumn[] = {
	{"事件类型",		68},
	{"时间",			100},
	{"事件内容",		660}
};


// status bar
#define UI_STATUSBAR_TOP			20
#define UI_STATUSBAR_OFFSET			10

static UINT g_nIndicators[] = {
	IDR_STATUSBAR_STRING,
};


// 自定义消息
enum {	
	UM_ICONNOTIFY = WM_USER + 0x100,
};