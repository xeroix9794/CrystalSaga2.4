#include "pch.h"
#include "log.h"
#include "util2.h"
#include "MyMemory.h"

#ifdef _DEBUG
#ifdef _GAMECORE
#include "new.h"
#define new DEBUG_NEW
#endif
#endif
static char g_szLog[LOG_BUF_SIZE];

void fLG(const char *pszName, const char *pszFormat, ...) {
	CLog *pLog = CLogMgr::Instance()->Add(pszName);
	if (pLog == NULL) {
		return;
	}
	
	if (!pLog->IsEnable()) {
		return;
	}

	bool bMsg = false;
	if (strlen(pszFormat) > 3) {
		if (strncmp(pszFormat, "msg", 3) == 0) {
			bMsg = true;
		}
	}

	va_list list;
	va_start(list, pszFormat);
	if (bMsg) {
		_vsnprintf_s(g_szLog, sizeof(g_szLog), _TRUNCATE, pszFormat + 3, list);
	}
	else {
		_vsnprintf_s(g_szLog, sizeof(g_szLog), _TRUNCATE, pszFormat, list);
	}
	va_end(list);

	CLogMgr::Instance()->RunCallBack(pszName, g_szLog);
	CLogMgr::Instance()->Log(pLog->GetFile(), g_szLog, bMsg, pLog->IsConsole());
}

CLog::CLog() {
	_Init();
}

CLog::CLog(const char *pszName, const char *pszExt) {
	_Init();
	if (Open(pszName, pszExt)) {
		_bValid = true;
	}
}

CLog::~CLog() {
	if (_fp) {
		fclose(_fp);
	}
}

void CLog::_Init() {
	_bEnable = true;
	_bConsole = true;
	_bValid = false;
	_bFile = true;
	_fp = NULL;
}

bool CLog::Open(const char *pszName, const char *pszExt) {
	if (_fp) {
		fclose(_fp);
		_fp = NULL;
	}

	if (!(CLogMgr::Instance()->IsEnable() && _bEnable)) {
		return false;
	}

	string str = CLogMgr::Instance()->m_szDir;
	str += '/';
	str += pszName;
	
	// strcpy(m_szName, pszName);
	strncpy_s(m_szName, sizeof(m_szName), pszName, _TRUNCATE);
	if (strlen(pszExt) > 0) {
		str += ".";
		str += pszExt;
	}

	// _fp = fopen(str.c_str(), "wt");
	if (fopen_s(&_fp, str.c_str(), "wt") != 0) {
		return false;
	}

	if (_fp == NULL) {
		return false;
	}

	// g_LogMgr.Add(m_szName, this);
	return true;
}

void CLog::L(const char *pszFormat, ...) {
	if (!(CLogMgr::Instance()->IsEnable() && _bEnable)) {
		return;
	}
	
	bool bMsg = false;
	if (strlen(pszFormat) > 3) {
		if (strncmp(pszFormat, "msg", 3) == 0) {
			bMsg = true;
		}
	}

	va_list list;
	va_start(list, pszFormat);
	if (bMsg) {
		_vsnprintf_s(g_szLog, sizeof(g_szLog), _TRUNCATE, pszFormat + 3, list);
	}
	else {
		_vsnprintf_s(g_szLog, sizeof(g_szLog), _TRUNCATE, pszFormat, list);
	}
	va_end(list);

	CLogMgr::Instance()->Log(_fp, g_szLog, bMsg, _bConsole);
}

CLogMgr::CLogMgr() : _pfnProc(NULL) {
	_bEnable = true;
	_bEnableMsgBox = true;
	_hWnd = NULL;
	SetDirectory("log");
	ClearAll();
}

CLog* CLogMgr::Add(const char *pszName, const char *pszExt) {
	CLog *pCheckLog = Get(pszName);
	if (pCheckLog != NULL) {
		return pCheckLog;
	}

	bool ret = true;
	CLog* pLog = NULL;
	pLog = new CLog(pszName, pszExt);
	if (pLog == NULL) {
		ret = false;
	}

	if (!pLog->IsValid()) {
		delete pLog;
		pLog = NULL;
		ret = false;
	}

	if (ret) {
		_LogIdx[pszName] = pLog;
	}

	return pLog;
}

CLog* CLogMgr::Get(const char *pszName) {
	map<string, CLog*>::iterator it = _LogIdx.find(pszName);
	if (it != _LogIdx.end()) {
		return (*it).second;
	}
	return NULL;
}

bool CLogMgr::Remove(const char *pszName) {
	map<string, CLog*>::iterator it = _LogIdx.find(pszName);
	if (it != _LogIdx.end()) {
		CLog *pLog = (*it).second;
		_LogIdx.erase(it);
		delete pLog;
		return true;
	}
	return false;
}

void CLogMgr::ClearAll() {
	ProcessDirectory(m_szDir, NULL, DIRECTORY_OP_DELETE);
}

void CLogMgr::SetDirectory(const char *pszDir) {
	strncpy_s(m_szDir, sizeof(m_szDir), pszDir, _TRUNCATE);
	Util_MakeDir(m_szDir);
}

void CLogMgr::GetDirectory(string& dir) {
	dir = m_szDir;
}

void CLogMgr::EnableMsgBox(bool enable) {
	_bEnableMsgBox = enable;
}

void CLogMgr::Log(FILE *fp, const char *pszLog, bool bMsg, bool bConsole) {
	if (!fp) {
		return;
	}

	if (bMsg) {
		fprintf(fp, "[DLG]");
	}

	fprintf(fp, "%s", pszLog);
	fflush(fp);

	if (bConsole) {
#ifdef WIN32
#ifndef _CONSOLE
		extern HANDLE __hStdOut;
		if (__hStdOut) {
			unsigned long cCharsWritten;
			WriteConsole(__hStdOut, pszLog, (DWORD)(strlen(pszLog)), &cCharsWritten, NULL);
		}
#else
		printf("%s" , pszLog);
#endif
#else
		printf("%s" , pszLog);
#endif	
	}

#ifdef WIN32
	if (bMsg && _bEnableMsgBox) {
		MessageBox(_hWnd, pszLog, "LOG", 0);
	}
#endif
}

void SetLGDir(char const* dir) {
	CLogMgr::Instance()->SetDirectory(dir);
}

void GetLGDir(string& dir) {
	CLogMgr::Instance()->GetDirectory(dir);
}

void EnableMsgBox(bool enable /* = true */) {
	CLogMgr::Instance()->EnableMsgBox(enable);
}