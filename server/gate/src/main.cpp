// main.cpp : Defines the entry point for the console application.

#include "gateserver.h"
_DBC_USING;

#include "ErrorHandler.h"

// #pragma init_seg(lib)
pi_LeakReporter pi_leakReporter("gatememleak.log");

// #include <ExceptionUtil.h>
char region[3];

int main(int argc, char* argv[]) {
	SEHTranslator translator;

	ErrorHandler::Initialize();
	ErrorHandler::DisableErrorDialogs();

	CResourceBundleManage::Instance("GateServer.loc");
	
	HWND hWin = ::GetConsoleWindow();
	DWORD prcId = 0;

	GetWindowThreadProcessId(hWin, &prcId);
	HANDLE hStd = GetStdHandle(STD_OUTPUT_HANDLE);

	region[2] = 0;

	T_B

#ifdef NDEBUG
	IniFile inf("GateServer.cfg");
	cChar* ip = inf["ToClient"]["IP"];

	// The program opens
	ReportState(1, ip);

	/*
	int ret = LicenseVerify("kop.lic", ip);
	if (ret != VERFIY_OK) {
		std::cout<< RES_STRING(GS_MAIN_CPP_00001)<<std::endl;
		system( "pause "); 
		return 0;
	}
	*/
#endif

	::SetLGDir("logfile/log");

	pi_Memory m("logfile/log/memorymonitor.log");
	m.startMonitor(1);

	GateServerApp app;
	app.ServiceStart();	
	g_gtsvr->RunLoop();
	app.ServiceStop();

	m.stopMonitor();
	m.wait();

#ifdef NDEBUG
	ReportState(0, ip);
#endif

	T_FINAL

	return 0;
}
