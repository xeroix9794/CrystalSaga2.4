#ifndef GATESERVER_H
#define GATESERVER_H

#include "dstring.h"
#include "DataSocket.h"

_DBC_USING

class GateServer {
	friend class GroupServerApp;

public:
	GateServer() : m_datasock(0), m_bSync(false) {
	}

	bool SetDataSock(DataSocket	*datasock) {
		bool l_retval = true;
		if (m_datasock && datasock) {
			l_retval = false;
		}
		else if (datasock) {
			l_retval = datasock->SetPointer(this);
		}

		if (l_retval) {
			if (m_datasock) {
				m_datasock->SetPointer(0);
			}
			m_datasock = datasock;
		}
		return l_retval;
	}

	DataSocket *GetDataSock() {
		return m_datasock;
	}

	bool IsSync() { 
		return m_bSync; 
	}

	void SetSync(bool sync = true) { 
		m_bSync = sync; 
	}

private:
	dstring m_name;
	DataSocket *m_datasock;

	// Game Server and GroupServer Sync Flags
	bool m_bSync;
};

class GroupServerAgent {
	friend class GroupServerApp;

public:
	GroupServerAgent() :m_datasock(0){}
	bool SetDataSock(DataSocket	*datasock) {
		bool l_retval = true;
		if (m_datasock && datasock) {
			l_retval = false;
		}
		else if (datasock) {
			l_retval = datasock->SetPointer(this);
		}

		if (l_retval) {
			if (m_datasock) {
				m_datasock->SetPointer(0);
			}
			m_datasock = datasock;
		}
		return l_retval;
	}

	DataSocket *GetDataSock() {
		return m_datasock;
	}

private:
	dstring m_name;
	DataSocket *m_datasock;
};

#endif // GATESERVER_H
