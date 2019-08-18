// logutil.cpp

#include "pch.h"
#include "logutil.h"
#include "packet.h"
#include "log.h"
#include "log2.h"

#include "ErrorHandler.h"

// Features: only record the current time point on the LOG (update)
static HWND GPL_hwnd = NULL;
void GPL(char const* type, int x, int y, char const* format, ...) {
}

// new interface
void LG_EnableAll(bool bEnable /* = true */) {
	CLog2Mgr::Inst()->Enable(bEnable);
}

void LG_EnableMsgBox(bool bEnable /* = true */) {
	CLog2Mgr::Inst()->EnableMsgBox(bEnable);
}

void LG_SetDir(char const* szDir) {
	CLog2Mgr* pLogMgr = CLog2Mgr::Inst();
	pLogMgr->SetDirectory(szDir);
	pLogMgr->LogMgrReopen();
}

void LG_GetDir(std::string& strDir) {
	CLog2Mgr* pLogMgr = CLog2Mgr::Inst();
	pLogMgr->GetDirectory(strDir);
}

void LG_CloseAll() {
	CLog2Mgr::Inst()->CloseAll();
}

void LG_SetEraseMode(bool bEraseMode /* = true */) {
	CLog2Mgr* pLogMgr = CLog2Mgr::Inst();
	pLogMgr->bEraseMode = bEraseMode;
}

void LG2(char const* type, char const* format, ...) {
	if (type == NULL || format == NULL) {
		return;
	}

	ErrorHandler::SetCurrent(format);
	try {

		// Check whether LOG is enabled
		CLog2Mgr* pLogMgr = CLog2Mgr::Inst();
		if (pLogMgr == NULL || !pLogMgr->IsEnable()) {
			return;
		}

		CLog2* pLog = pLogMgr->Add(type);
		if (pLog == NULL || !pLog->IsEnable()) {
			return;
		}

		char buf[8192] = { 0 };
		int len;
		va_list args;
		va_start(args, format);
		len = vsnprintf_s(buf, sizeof(buf), _TRUNCATE, format, args);
		va_end(args);
		if (len < 0) {
			pLogMgr->Log("stack buffer overflow when LG [%s]\n", type);
			return;
		}
		pLog->LogString(buf);
	}
	catch (...) {
		printf(" LG2(char const* type, char const* format, ...)");
	}
}

// Removed a huge commented code here