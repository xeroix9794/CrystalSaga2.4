#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// #include "AccountServer2.h"
#include <string>

#include "MyThread.h"
#include "GlobalVariable.h"

// The profile string must be defined preferentially
std::string g_strCfgFile = "AccountServer.cfg";

dbc::TLSIndex g_TlsIndex;
CDataBaseCtrl g_MainDBHandle;
DWORD g_MainThreadID;
HWND g_hMainWnd = NULL;

// BillThread g_BillThread;


