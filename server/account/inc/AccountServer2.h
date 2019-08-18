
#ifndef ACCOUNTSERVER2_H_
#define ACCOUNTSERVER2_H_

#include "time.h"
#include "commrpc.h"
#include "threadpool.h"
#include "packetqueue.h"
#include "tlsindex.h"
#include "util.h"
#include "MyThread.h"

using namespace dbc;

class ToMMS;
class ToBilling;

struct GroupServer2 : PreAllocStru {
	GroupServer2(uLong size) : PreAllocStru(size), m_datasock(NULL), m_next(NULL) {
	}

	WPacket GetWPacket() { 
		return m_datasock->GetWPacket(); 
	}

	long SendData(WPacket wpkt) { 
		return m_datasock->SendData(wpkt);
	}

	bool IsSame(std::string const& strGroup) { 
		return (m_strName == strGroup); 
	}
	
	char const* const GetName() const { 
		return m_strName.c_str(); 
	}

	char const* const GetAddr() const { 
		return m_strAddr.c_str(); 
	}

	std::string m_strName;
	std::string m_strAddr;
	DataSocket* m_datasock;
	GroupServer2* m_next;
};

class AccountServer2 : public TcpServerApp, public RPCMGR {
public:
	AccountServer2(ThreadPool* proc = 0, ThreadPool* comm = 0);
	virtual ~AccountServer2();

	GroupServer2* FindGroup(char const* szGroup);
	void DisplayGroup();

	void IncreaseMembers(long nCount = 1);
	void DecreaseMembers(long nCount = 1);
	void ResetMembersCount();
	long GetMembersCount();

protected:
	virtual bool OnConnect(DataSocket* datasock);
	virtual	void OnProcessData(DataSocket* datasock, RPacket& rpkt);
	virtual WPacket OnServeCall(DataSocket* datasock, RPacket& rpkt);
	virtual void OnDisconnect(DataSocket* datasock, int reason);

private:
	static volatile long m_nMembersCount;

	// GroupServer related processing routines and data
	void Gs_Init();
	GroupServer2* Gs_Find(char const* szGroupName);
	bool Gs_Auth(char const* szGroupName, char const* szGroupPwd);
	void Gs_ListAdd(GroupServer2* Gs);
	void Gs_ListDel(GroupServer2* Gs);
	void Gs_Exit();

	PreAllocHeap<GroupServer2> m_GsHeap;
	GroupServer2* m_GsList;
	Mutex m_GsListLock;
	short m_GsNumber;

	WPacket Gs_Login(DataSocket* datasock, RPacket& rpkt);
	bool AddGroup(GroupServer2* pGs);
	bool DelGroup(DataSocket* datasock);
	void Gs_Logout(DataSocket* datasock);

	WPacket ProcessUnknownCmd(RPacket rpkt);

public:
	/*
	// Thread pool with MMS
	ThreadPool* m_mmsproc, * m_mmscomm;

	// Connection object with MMS (active reconnection mechanism)
	ToMMS* m_ToMMS;
	*/

	// Thead pool with billing
	ThreadPool* m_billproc, *m_billcomm;
	
	// Connection object with BillingSerena
	ToBilling* m_ToBilling;
};

// Authentication

class AuthQueue : public PKQueue {
public:
	AuthQueue();
	virtual ~AuthQueue();

protected:
	virtual void ProcessData(DataSocket* datasock, RPacket& rpkt);
	virtual WPacket ServeCall(DataSocket* datasock, RPacket& rpkt);
};


// Login to the temporary queue
typedef std::vector<std::string> TmpNameList;
class LoginTmpList {
public:
	LoginTmpList();
	virtual ~LoginTmpList();

	bool Insert(const std::string& name);
	bool Remove(const std::string& name);
	bool Query(const std::string& name, bool lock = true);

	void Lock();
	void UnLock();

private:
	CRITICAL_SECTION m_cs;
	TmpNameList m_list;
};


class AuthThread : public MyThread {
public:
	AuthThread(int nIndex);
	virtual ~AuthThread();

	void Reconnt();

	void QueryAccount(RPacket rpkt);
	bool IsValidName(char const* szName, unsigned short usNameLen);

	WPacket AccountLogin(DataSocket* datasock);
	void AccountLogout(RPacket rpkt);
	WPacket TomAccountLogin(DataSocket* datasock, RPacket& rpkt);
	void TomAccountLogout(RPacket& rpkt);

	void BeginBilling(RPacket& rpkt);
	void EndBilling(RPacket& rpkt);
	void ResetBilling();

	void LogoutGroup(DataSocket* datasock, RPacket rpkt);

	static void LoadConfig();

	CSQLDatabase *GetSQLDatabase();

protected:
	void Init();
	void Exit();
	bool Connect();
	void Disconn();

	virtual int Run();
	void SetRunLabel(int nRunLabel);
	void LogUserLogin(int nUserID, string strUserName, string strIP);
	void LogUserLogout(int nUserID);

	enum { 
		ACCOUNT_OFFLINE, 
		ACCOUNT_ONLINE, 
		ACCOUNT_SAVING, 
	};

	struct AccountInfo {
		bool bExist;
		int nId;
		bool bAllowLogin;
		std::string strName;
		std::string strPwdDigest;
		int nSid;
		int nStatus;
		int nEnableLoginTick;
		std::string strGroup;
		std::string strChapString;
		char const* pDat;
		unsigned short usDatLen;
		std::string strMAC;
		std::string strIP;
	
		int nBan;
		int nProtectTime;
	};

	long GenSid(char const* szName);
	void ResetAccount();
	void KickAccount(std::string& strGroup, int nId);


private:
	enum { 
		INVALID_SID = 0 
	};

	enum { 
		RELOGIN_TIME = 15 
	};

	enum { 
		SAVING_TIME = 15 
	};

	CSQLDatabase* m_pAuth;
	AccountInfo m_AcctInfo;
	int m_nIndex;

	static Sema m_Sema;
	static std::string m_strSrvip;
	static std::string m_strSrvdb;
	static std::string m_strUserId;
	static std::string m_strUserPwd;
	static std::string m_strAccountTableName;
};

class AuthThreadPool {
public:
	AuthThreadPool();
	~AuthThreadPool();

	void Launch();
	void NotifyToExit();
	void WaitForExit();

	enum { 
		AT_MAXNUM = 10 
	};

	static int volatile RunLabel[AT_MAXNUM];
	static DWORD volatile RunLast[AT_MAXNUM];
	static DWORD volatile RunConsume[AT_MAXNUM];
	static unsigned int volatile uiAuthCount;
	static void IncAuthCount();
	static unsigned int GetAuthCount();

protected:
	AuthThread* m_Pool[AT_MAXNUM];
};

class ConnectMMS : public Task {
public:
	ConnectMMS(ToMMS* tmms) {
		_tmms = tmms; 
		_timeout = 3000; 
	}

private:
	virtual long Process();
	virtual Task* Lastly();

	ToMMS* _tmms;
	DWORD _timeout;
};

class ToMMS : public TcpClientApp, public RPCMGR {
	friend class ConnectMMS;
	struct MMS {
		MMS() : datasock(NULL) { 
		}
		
		string ip; uShort port;
		string type;
		uShort key;
		DataSocket* datasock;
	};

public:
	ToMMS(const char* configeName, ThreadPool* proc, ThreadPool* comm);
	~ToMMS();

	int GetCallTotal() { 
		return m_calltotal; 
	}

	void Login(cChar* ip, uShort port, int accountID);
	void Logout(cChar* ip, uShort port, int accountID);

private:
	virtual bool OnConnect(DataSocket* datasock);
	virtual void OnDisconnect(DataSocket* datasock, int reason);
	virtual void OnProcessData(DataSocket* datasock, RPacket &recvbuf);
	virtual WPacket OnServeCall(DataSocket* datasock, RPacket &in_para);

	InterLockedLong	m_atexit, m_calltotal;
	bool volatile _connected;
	MMS	m_MMS;
};

class ConnectBilling : public Task {
public:
	ConnectBilling(ToBilling* billing) { 
		_billing = billing; 
		_timeout = 3000; 
	}

private:
	virtual long Process();
	virtual Task* Lastly();

	ToBilling* _billing;
	DWORD _timeout;
};

class ToBilling : public TcpClientApp, public RPCMGR {
	friend class ConnectBilling;
	struct Billing {
		Billing() : datasock(NULL) { 
		}

		string ip; uShort port;
		
		// Whether to activate the billing system, 0 is not started 1 to start
		bool enable;

		// Whether to allow account kicking
		bool killuser;

		DataSocket* datasock;
	};

public:
	ToBilling(const char* configeName, ThreadPool* proc, ThreadPool* comm);
	~ToBilling();

	int GetCallTotal() { 
		return m_calltotal; 
	}

	void UserLogin(std::string strUserName, std::string strPassport);
	void UserLogout(std::string strUserName);

private:
	virtual bool OnConnect(DataSocket* datasock);
	virtual void OnDisconnect(DataSocket* datasock, int reason);
	virtual void OnProcessData(DataSocket* datasock, RPacket &recvbuf);
	virtual WPacket OnServeCall(DataSocket* datasock, RPacket &in_para);

	InterLockedLong	m_atexit, m_calltotal;
	bool volatile _connected;
	Billing	m_Billing;
};

// Global variables
extern AccountServer2* g_As2;
extern AuthQueue g_Auth;


#endif // ACCOUNTSERVER2_H_
