// logutil.h
#ifndef LOG_UTIL_H_
#define LOG_UTIL_H_

#include "packet.h"
#include "util2.h"

class CLogMechanism {
public:
	CLogMechanism();
	virtual ~CLogMechanism();
	virtual int Process(CLogPacket& lp) = 0;
};


#ifdef WIN32
// Use the WM_COPYDATA message on the Windows platform to forward data to other processes
class CW32_WMCOPYDATA : public CLogMechanism {
public:
	CW32_WMCOPYDATA(char const* pszWindowTitle) {
		m_WindowTitle = pszWindowTitle;
		m_hWnd = NULL;
	}

	~CW32_WMCOPYDATA() {
	}

	int Process(CLogPacket& lp);

private:
	HWND m_hWnd; // Local destination window handle
	string m_WindowTitle;
};
#endif // WIN32

enum {
	P_RECV, P_SEND, P_OTHER 
};

// use the LOG method's identity, can be placed in the header of the dwReserved domain
enum LOG_CALL_TYPE { 
	LG_CALL = 0, 
	GPL_CALL = 1 
};

class CLogMgr2 : public Singleton < CLogMgr2 > {
public:
	CLogMgr2();
	~CLogMgr2();
	CLogMechanism* Add(char const* pszFunName, CLogMechanism* pLogMech);
	CLogMechanism* Get(char const* pszFunName);
	bool Remove(char const* pszFunName);
	map<string, CLogMechanism*> m_LogIdx;
};

#endif // LOG_UTIL_H_
