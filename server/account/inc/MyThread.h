// Separate the module from the AccountServer2 file

#pragma once

#ifndef _MYTHREAD_H_
#define _MYTHREAD_H_

#include "Comm.h"

// Thread base class
class MyThread {
public:
	MyThread();
	virtual ~MyThread();

	bool Launch();
	void NotifyToExit();

	// Block Calling Thread
	int WaitForExit(int ms);

protected:
	static DWORD WINAPI ThreadProc(LPVOID);
	virtual int Run();
	bool GetExitFlag() const {return m_bExitFlag;}
	void ExitThread() {m_bExitThrd = true;}

private:
	bool volatile m_bExitFlag;
	bool volatile m_bExitThrd;
	DWORD m_dwThreadId;
	HANDLE m_hThread;
};

#endif
