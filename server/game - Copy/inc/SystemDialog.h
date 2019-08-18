#define WIN32_DLG

#ifdef WIN32_DLG

extern void  CreateMainDialog(HINSTANCE hInst, HWND hParent);
extern void  SystemReport(DWORD dwTime);
extern void  MapReport();
extern HWND  g_SysDlg;

extern void SendScriptMsg(const char* msg);
extern void SendNetProcMsg(const char* msg);
extern void SendNetMsg(const char* msg);
extern void SendMsg(const char* msg);

#define WM_USER_MAP WM_USER + 0x100
#define WM_USER_LOG WM_USER + 0x101

#endif