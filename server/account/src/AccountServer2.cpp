#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#include "AccountServer2.h"
#include "gamecommon.h"
#include "inifile.h"
#include "util.h"
#include "GlobalVariable.h"

#pragma warning(disable : 4800)

// TLSIndex g_TlsIndex;

// Authentication service object
AuthQueue g_Auth;

// Login to the temporary list
LoginTmpList tmpLogin;

uLong NetBuffer[] = { 100, 10, 0 };
bool g_logautobak = true;

volatile long AccountServer2::m_nMembersCount = 0;

// Tcp Server
AccountServer2::AccountServer2(ThreadPool* proc, ThreadPool* comm) : TcpServerApp(this, proc, comm, false), RPCMGR(this), m_GsHeap(1, 100), m_GsList(NULL), m_GsNumber(0) {
	// m_mmsproc = ThreadPool::CreatePool(4, 8, 1024, THREAD_PRIORITY_ABOVE_NORMAL);
	// m_mmscomm = ThreadPool::CreatePool(12, 24, 2048, THREAD_PRIORITY_ABOVE_NORMAL);

	// m_billproc = ThreadPool::CreatePool(4, 8, 1024, THREAD_PRIORITY_ABOVE_NORMAL);
	// m_billcomm = ThreadPool::CreatePool(12, 24, 2048, THREAD_PRIORITY_ABOVE_NORMAL);

	m_GsHeap.Init();

	IniFile inf(g_strCfgFile.c_str());
	IniSection& is = inf["net"];
	char const* ip = is["listen_ip"];
	unsigned short port = atoi(is["listen_port"]);

	/*
	try {

	// m_ToMMS = new ToMMS(g_strCfgFile.c_str(), m_mmsproc, m_mmscomm);
	// m_mmsproc->AddTask(new ConnectMMS(m_ToMMS));
	m_ToBilling = new ToBilling(g_strCfgFile.c_str(), m_billproc, m_billcomm);
	m_billproc->AddTask(new ConnectBilling(m_ToBilling));

	}
	catch(...) {
	// m_mmsproc->DestroyPool();
	// m_mmscomm->DestroyPool();
	// if(m_ToMMS) {
	// delete m_ToMMS;
	// m_ToMMS = NULL;
	//}

	m_billproc->DestroyPool();
	m_billcomm->DestroyPool();
	if(m_ToBilling) {
	delete m_ToBilling;
	m_ToBilling = NULL;
	}
	}
	*/


	/*
	 The offset of "packet length" is 0.
	 The field of "packet length" is 2 bytes.
	 The max length of packet is 4K bytes.
	 The max length of send queue is 100.
	 */
	SetPKParse(0, 2, 4 * 1024, 100);

#ifdef _DEBUG
	BeginWork(200);
#else
	BeginWork(30);
#endif

	// Start listening.
	OpenListenSocket(port, ip);

	ResetMembersCount();
}

AccountServer2::~AccountServer2() {
	ShutDown(12 * 1000);

	/*
	m_mmsproc->DestroyPool();
	m_mmscomm->DestroyPool();
	delete m_ToMMS;
	*/

	m_billproc->DestroyPool();
	m_billcomm->DestroyPool();

	delete m_ToBilling;
}

void AccountServer2::IncreaseMembers(long nCount) {
	InterlockedExchangeAdd(&m_nMembersCount, nCount);
}

void AccountServer2::DecreaseMembers(long nCount) {
	InterlockedExchangeAdd(&m_nMembersCount, -nCount);
}

void AccountServer2::ResetMembersCount() {
	InterlockedExchange(&m_nMembersCount, 0);
}

long AccountServer2::GetMembersCount() {
	return m_nMembersCount;
}

bool AccountServer2::OnConnect(DataSocket* datasock) {
	return true;
}

void AccountServer2::OnProcessData(DataSocket* datasock, RPacket& rpkt) {
	unsigned short usCmd = rpkt.ReadCmd();

	switch (usCmd) {

		// GroupServer protocol
	case CMD_PA_LOGOUT: {
		g_Auth.AddPK(datasock, rpkt);
		break;
	}

						// Authentication type agreement
	case CMD_PA_USER_LOGOUT: {
		g_Auth.AddPK(datasock, rpkt);
		break;
	}

							 // Billing type agreement - the user starts billing
	case CMD_PA_USER_BILLBGN: {
		IncreaseMembers();
		sUserLog *pUserLog = new sUserLog;
		pUserLog->bLogin = true;
		pUserLog->strUserName = rpkt.ReadString();
		pUserLog->strPassport = rpkt.ReadString();

		// m_ToBilling->UserLogin(pUserLog->strUserName, pUserLog->strPassport);
		if (!PostMessage(g_hMainWnd, WM_USER_LOG_MAP, 0, (LPARAM)pUserLog)) {
			LG("AccountServer", "AccountServer2::OnProcessData, CMD_PA_USER_BILLBGN: PostMessage WM_USER_LOG_MAP failed!\n");
			delete pUserLog;
		}

		/*
		dbc::cChar* userName=rpkt.ReadString();
		dbc::cChar* passport=rpkt.ReadString();
		m_ToBilling->UserLogin(userName, passport);
		*/

		break;
	}

							  // Billing type agreement - The user stops charging
	case CMD_PA_USER_BILLEND: {
		DecreaseMembers();
		sUserLog *pUserLog = new sUserLog;
		pUserLog->bLogin = false;
		pUserLog->strUserName = rpkt.ReadString();

		// m_ToBilling->UserLogout(pUserLog->strUserName);

		if (!PostMessage(g_hMainWnd, WM_USER_LOG_MAP, 0, (LPARAM)pUserLog)) {
			LG("AccountServer", "AccountServer2::OnProcessData, CMD_PA_USER_BILLEND: PostMessage WM_USER_LOG_MAP failed!\n");
			delete pUserLog;
		}

		/*
		dbc::cChar* userName=rpkt.ReadString();
		m_ToBilling->UserLogout(userName);
		*/

		break;
	}

							  // Billing type agreement - all users end billing
	case CMD_PA_GROUP_BILLEND_AND_LOGOUT: {
		ResetMembersCount();
		break;
	}

	case CMD_PA_GMBANACCOUNT: {
		string actName = rpkt.ReadString();
		g_MainDBHandle.OperAccountBan(actName, 3);
		break;
	}

							  // Other unknown protocols
	default:
		LG("As2Excp", "Unknown usCmd=[%d]\n", usCmd);
	}
}

WPacket AccountServer2::OnServeCall(DataSocket* datasock, RPacket& rpkt) {
	unsigned short usCmd = rpkt.ReadCmd();
	switch (usCmd) {

		// GroupServer protocol
	case CMD_PA_LOGIN:
		return Gs_Login(datasock, rpkt);

		// Authentication type agreement
	case CMD_PA_USER_LOGIN:
		return g_Auth.SyncPK(datasock, rpkt, 20 * 1000);

		// Other unknown protocols
	default:
		LG("As2Excp", "Unknown usCmd=[%d]\n", usCmd);
		return ProcessUnknownCmd(rpkt);
	}
}

WPacket AccountServer2::ProcessUnknownCmd(RPacket rpkt) {
	WPacket wpkt = GetWPacket();
	wpkt.WriteShort(ERR_AP_UNKNOWNCMD);
	return wpkt;
}

void AccountServer2::OnDisconnect(DataSocket* datasock, int reason) {
	Gs_Logout(datasock);
}

// GroupServer related methods
void AccountServer2::Gs_Init() {
	m_GsNumber = 0;
	m_GsList = NULL;
	m_GsHeap.Init();
	m_GsListLock.Create(false);
}

GroupServer2* AccountServer2::Gs_Find(char const* szGroupName) {
	GroupServer2* curr = m_GsList;
	GroupServer2* prev = NULL;
	while (curr != NULL) {
		if (curr->m_strName.compare(szGroupName) == 0) break;
		prev = curr;
		curr = curr->m_next;
	}
	return curr;
}

bool AccountServer2::Gs_Auth(char const* szGroupName, char const* szGroupPwd) {
	char const* pwd = NULL;
	IniFile inf(g_strCfgFile.c_str());
	try {
		pwd = inf["gs"][szGroupName];
	}
	catch (...) {
		return false;
	}
	return (strcmp(pwd, szGroupPwd) == 0) ? true : false;
}

void AccountServer2::Gs_ListAdd(GroupServer2* Gs) {
	Gs->m_next = m_GsList;
	m_GsList = Gs;
	++m_GsNumber;
}

void AccountServer2::Gs_ListDel(GroupServer2* Gs) {
	GroupServer2* curr = m_GsList;
	GroupServer2* prev = NULL;
	while (curr) {
		if (curr == Gs)
			break;
		prev = curr;
		curr = curr->m_next;
	}

	if (curr) {
		if (prev) {
			prev->m_next = curr->m_next;
		}
		else {
			m_GsList = curr->m_next;
		}
		--m_GsNumber;
	}
}

void AccountServer2::Gs_Exit() {
}

WPacket AccountServer2::Gs_Login(DataSocket* datasock, RPacket& rpkt) {
	/*
	Found that this function will produce multiple threads call itself needs a thread synchronization lock
	Found that this function and Gs_Logout may exist in the case of multi-threaded synchronous operation, this function needs to establish a thread synchronization lock with Gs_Logout
	If this function can return high-speed (not including database operations), a thread synchronization problem probability is very low
	The use of a wide range of thread synchronization lock can solve the same function within the resource conflict, but can not solve the cross function call conflict (although this situation appears more likely)
	The most effective way is to Gs_Login and Gs_Logout and between the thread call itself into a queue message access, due to structural changes are relatively large, not currently using this method
	*/

	bool bAuthSuccess = false;
	bool bAlreadyLogin = false;
	char const* szGroupName = rpkt.ReadString();
	char const* szGroupPwd = rpkt.ReadString();

	if (FindGroup(szGroupName) != NULL) {
		bAlreadyLogin = true;
	}
	else {
		bAuthSuccess = Gs_Auth(szGroupName, szGroupPwd);
	}

	WPacket wpkt = GetWPacket();
	if (bAlreadyLogin) {
		wpkt.WriteShort(ERR_AP_GPSLOGGED);
	}
	else {
		if (bAuthSuccess) {
			GroupServer2* pGs = m_GsHeap.Get();
			pGs->m_strName = szGroupName;
			pGs->m_strAddr = datasock->GetPeerIP();
			pGs->m_datasock = datasock;

			if (AddGroup(pGs)) {
				// Join to List successfully
				datasock->SetPointer(pGs);
				wpkt.WriteShort(ERR_SUCCESS);
				// cout << "[" << szGroupName << "] Add Successfully!" << endl;
				LG("GroupServer", "[%s] Add Successfully!\n", szGroupName);
			}
			else {
				// Add to the List failed, indicating that the same name has just logged in GroupServer, or there is an exception
				pGs->Free();
				bAlreadyLogin = true;
				wpkt.WriteShort(ERR_AP_GPSAUTHFAIL);
			}
		}
		else {
			wpkt.WriteShort(ERR_AP_GPSAUTHFAIL);
		}
	}

	if (bAlreadyLogin)
		Disconnect(datasock, 1000);

	return wpkt;
}

void AccountServer2::Gs_Logout(DataSocket* datasock) {
	/*
	Found that this function will produce multiple threads call itself needs a thread synchronization lock
	Found that this function may exist with the Gs_Login multi-threaded synchronous operation, this function needs to establish a thread with the Gs_Login lock synchronization
	If this function can return high-speed (not including database operations), a thread synchronization problem probability is very low
	The use of a wide range of thread synchronization lock can solve the same function within the resource conflict, but can not solve the cross function call conflict (although this situation appears more likely)
	The most effective way is to Gs_Login and Gs_Logout and between the thread call itself into a queue message access, due to structural changes are relatively large, not currently using this method
	Because GroupServer does not clear the connection after the state is disconnected, so AccountServer should not empty the user state, waiting for automatic re-connection can be restored after the normal, which requires GroupServer restart AccountServer must also restart
	*/

	std::string strGroupName;
	GroupServer2* pGs = (GroupServer2 *)datasock->GetPointer();
	if ((pGs == NULL) || (pGs->m_datasock != datasock))
		return;

	strGroupName = pGs->m_strName;
	LG("GroupServer", "[%s] disconnected!\n", strGroupName.c_str());

	if (DelGroup(datasock)) {
		WPacket wpkt = GetWPacket();
		wpkt.WriteCmd(CMD_PA_LOGOUT);
		wpkt.WriteString(strGroupName.c_str());
		OnProcessData(datasock, RPacket(wpkt));
	}
}

GroupServer2* AccountServer2::FindGroup(char const* szGroup) {
	GroupServer2* pGs = NULL;
	m_GsListLock.lock();
	try {
		pGs = Gs_Find(szGroup);
	}
	catch (...) {
		LG("As2Excp", "Exception raised from KickAccount when find GroupServer: [%s]\n", szGroup);
	}
	m_GsListLock.unlock();
	return pGs;
}

void AccountServer2::DisplayGroup() {
	extern void ClearGroupList();
	extern BOOL AddGroupToList(char const* strName, char const* strAddr, char const* strStatus);
	GroupServer2* pGs = m_GsList;
	ClearGroupList();
	m_GsListLock.lock();
	try {
		while (pGs != NULL) {
			AddGroupToList(pGs->GetName(), pGs->GetAddr(), RES_STRING(AS_ACCOUNTSERVER2_CPP_00001));
			pGs = pGs->m_next;
		}
	}
	catch (...) {
	}
	m_GsListLock.unlock();
}

bool AccountServer2::AddGroup(GroupServer2* pGs) {
	bool bAlreadyLogin = false;
	m_GsListLock.lock();
	try {
		if (Gs_Find(pGs->m_strName.c_str()) != NULL) {
			bAlreadyLogin = true;
		}

		if (!bAlreadyLogin) {
			Gs_ListAdd(pGs);
		}
	}
	catch (...) {
		LG("As2Excp", "Exception raised from AddGroup() when add [%s]\n", pGs->m_strName.c_str());

		// This GroupServer login is not allowed
		bAlreadyLogin = true;
	}
	m_GsListLock.unlock();
	return !bAlreadyLogin;
}

bool AccountServer2::DelGroup(DataSocket* datasock) {
	bool bDel = false;
	GroupServer2* pGs = NULL;
	m_GsListLock.lock();
	try {
		// Again for a test!
		pGs = (GroupServer2 *)datasock->GetPointer();
		if ((pGs != NULL) && (pGs->m_datasock == datasock)) {
			Gs_ListDel(pGs);
			datasock->SetPointer(NULL);
			bDel = true;
			pGs->Free();
		}
	}
	catch (...) {
		LG("As2Excp", "Exception raised from AddGroup() when add [%s]\n", pGs->m_strName.c_str());
	}
	m_GsListLock.unlock();
	return bDel;
}

// Authentication
AuthQueue::AuthQueue() : PKQueue(false) {
}

AuthQueue::~AuthQueue() {
}

void AuthQueue::ProcessData(DataSocket* datasock, RPacket& rpkt) {
	bool bRetry = true;
	unsigned short usCmd = rpkt.ReadCmd();

	// Get the current thread object
	AuthThread* pThis = (AuthThread *)(g_TlsIndex.GetPointer());
	if (pThis == NULL)
		return;

	while (bRetry) {
		try {
			switch (usCmd) {

			case CMD_PA_LOGOUT: {
				pThis->LogoutGroup(datasock, rpkt);
			} break;

			case CMD_PA_USER_LOGOUT: {
				pThis->AccountLogout(rpkt);
			} break;

			case CMD_PA_USER_BILLBGN: {
				pThis->BeginBilling(rpkt);
			} break;

			case CMD_PA_USER_BILLEND: {
				pThis->EndBilling(rpkt);
			} break;

			case CMD_PA_GROUP_BILLEND_AND_LOGOUT: {
				pThis->ResetBilling();
			} break;

			default:
				LG("AuthProcessData", "Unknown usCmd=[%d], Skipped...\n", usCmd);
				break;
			}

			bRetry = false;
		}
		catch (CSQLException* se) {
			LG("AuthProcessDataExcp", "SQL Exception: %s\n", se->m_strError.c_str());

			// Reconnection
			pThis->Reconnt();
		}
		catch (...){
			LG("AuthProcessDataExcp", "unknown exception raised from AuthQueue::ProcessData()\n");
			bRetry = false; // Get the current thread object...
		}
	}
}

WPacket AuthQueue::ServeCall(DataSocket* datasock, RPacket& rpkt)
{
	bool bRetry = true;
	unsigned short usCmd = rpkt.ReadCmd();
	WPacket wpkt = datasock->GetWPacket();

	// Non-database caused by abnormal let go
	AuthThread* pThis = (AuthThread *)(g_TlsIndex.GetPointer());
	if (pThis == NULL) {
		LG("AuthExcp", "pThis = NULL\n");
		wpkt.WriteShort(ERR_AP_TLSWRONG);
		return wpkt;
	}

	while (bRetry) {
		try {
			switch (usCmd) {

			case CMD_PA_USER_LOGIN: {
				pThis->QueryAccount(rpkt);
				return pThis->AccountLogin(datasock);
			}

			default: {
				LG("AuthServeCall", "Unknown usCmd=[%d], Skipped...\n", usCmd);
				wpkt.WriteShort(ERR_AP_UNKNOWNCMD);
				return wpkt;
			}

			}
		}
		catch (CSQLException* se) {
			LG("AuthServeCallExcp", "SQL Exception: %s\n", se->m_strError.c_str());

			// Reconnection
			pThis->Reconnt();
		}
		catch (...) {
			LG("AuthServeCallExcp", "unknown exception raised from AuthQueue::ServerCall()\n");
			bRetry = false; // Non-database caused by abnormal let go
		}
	}

	wpkt.WriteShort(ERR_AP_UNKNOWN);
	return wpkt;
}

// LoginTmpList:

LoginTmpList::LoginTmpList() {
	InitializeCriticalSection(&m_cs);
}

LoginTmpList::~LoginTmpList() {
	DeleteCriticalSection(&m_cs);
}

bool LoginTmpList::Insert(const std::string& name) {
	bool ret = false;
	Lock();
	if (!Query(name, false)) {
		m_list.push_back(name);
		ret = true;
	}
	UnLock();
	return ret;
}

bool LoginTmpList::Remove(const std::string& name) {
	bool ret = false;
	TmpNameList::iterator it;
	Lock();
	for (it = m_list.begin(); it != m_list.end(); it++) {
		if ((*it) == name) {
			ret = true;
			m_list.erase(it);
			break;
		}
	}
	UnLock();
	return ret;
}

// MRF_OLD_PRACTICE: lock parameter default value is true.
bool LoginTmpList::Query(const std::string& name, bool lock) {
	bool ret = false;
	TmpNameList::iterator it;
	if (lock) {
		Lock();
	}

	for (it = m_list.begin(); it != m_list.end(); it++) {
		if ((*it) == name) {
			ret = true;
			break;
		}
	}

	if (lock) {
		UnLock();
	}

	return ret;
}

void LoginTmpList::Lock() {
	EnterCriticalSection(&m_cs);
}

void LoginTmpList::UnLock() {
	LeaveCriticalSection(&m_cs);
}

// AuthThread:

Sema AuthThread::m_Sema(0, AuthThreadPool::AT_MAXNUM);
std::string AuthThread::m_strSrvip = "";
std::string AuthThread::m_strSrvdb = "";
std::string AuthThread::m_strUserId = "";
std::string AuthThread::m_strUserPwd = "";
std::string AuthThread::m_strAccountTableName = "account_login";
AuthThread::AuthThread(int nIndex) : m_pAuth(NULL), m_nIndex(nIndex) {}
AuthThread::~AuthThread() { Exit(); }

void AuthThread::Init() {
	g_TlsIndex.SetPointer(NULL);
	m_pAuth = NULL;
	SetRunLabel(-1);

	while (!Connect()) {
		if (GetExitFlag())
			return;

		Sleep(5000);
	}

	g_TlsIndex.SetPointer(this);
	ResetAccount();
	SetRunLabel(0);
}

void AuthThread::Exit() {
	Disconn();
	g_TlsIndex.SetPointer(NULL);
}

bool AuthThread::Connect() {
	bool ret = true;
	if (m_pAuth != NULL) return true;

	// Create an object
	try {
		m_pAuth = new CSQLDatabase();
	}
	catch (std::bad_alloc& ba) {
		LG("AuthDBExcp", "AuthThread::Connect() new failed : %s\n", ba.what());
		m_pAuth = NULL;
		return false;
	}
	catch (...) {
		LG("AuthDBExcp", "AuthThread::Connect() unknown exception\n");
		m_pAuth = NULL;
		return false;
	}

	// Connect to the database
	char conn_str[512] = { 0 };
	char const* conn_fmt = "DRIVER={SQL Server};SERVER=%s;UID=%s;PWD=%s;DATABASE=%s";
	_snprintf_s(conn_str, sizeof(conn_str), _TRUNCATE, conn_fmt, m_strSrvip.c_str(), m_strUserId.c_str(), m_strUserPwd.c_str(), m_strSrvdb.c_str());
	ret = m_pAuth->Open(conn_str);
	if (ret) {
		m_pAuth->SetAutoCommit(true);
	}
	else {
		delete m_pAuth;
		m_pAuth = NULL;
	}

	return ret;
}

void AuthThread::Disconn() {
	if (m_pAuth != NULL) {
		try {
			m_pAuth->Close();
			delete m_pAuth;
		}
		catch (...) {
			LG("AuthExcp", "Exception raised when AuthThread::Disconn()\n");
		}
		m_pAuth = NULL;
	}
}

void AuthThread::Reconnt() {
	Disconn();
	while (!Connect()) {
		LG("As2", RES_STRING(AS_ACCOUNTSERVER2_CPP_00002));
		if (GetExitFlag())
			return;
		Sleep(5000);
	}
}

void AuthThread::LoadConfig() {
	char buf[80];
	IniFile inf(g_strCfgFile.c_str());
	IniSection& is = inf["db"];
	std::string strTmp = "";

	try {
		_snprintf_s(buf, sizeof(buf), "dbserver", _TRUNCATE);
		m_strSrvip = is[buf].c_str();
		_snprintf_s(buf, sizeof(buf), "db", _TRUNCATE);
		m_strSrvdb = is[buf].c_str();
		_snprintf_s(buf, sizeof(buf), "userid", _TRUNCATE);
		m_strUserId = is[buf].c_str();
		_snprintf_s(buf, sizeof(buf), "passwd", _TRUNCATE);
		strTmp = is[buf].c_str();
	}
	catch (excp& e) {
		cout << e.what() << endl;
		getchar();
		ExitProcess(-1);
	}
	dbpswd_out(strTmp.c_str(), (int)strTmp.length(), m_strUserPwd);
}

void AuthThread::LogUserLogin(int nUserID, string strUserName, string strIP) {
	sUserLog *pUserLog = new sUserLog;
	pUserLog->bLogin = true;
	pUserLog->nUserID = nUserID;
	pUserLog->strUserName = strUserName;
	pUserLog->strLoginIP = strIP;
	if (!PostMessage(g_hMainWnd, WM_USER_LOG, 0, (LPARAM)pUserLog)) {
		LG("AccountServer", "AuthThread::LogUserLogin: PostMessage WM_USER_LOG failed!\n");
		delete pUserLog;
	}
}

void AuthThread::LogUserLogout(int nUserID) {
	sUserLog *pUserLog = new sUserLog;
	pUserLog->bLogin = false;
	pUserLog->nUserID = nUserID;
	if (!PostMessage(g_hMainWnd, WM_USER_LOG, 0, (LPARAM)pUserLog)) {
		LG("AccountServer", "AuthThread::LogUserLogout: PostMessage WM_USER_LOG failed!\n");
		delete pUserLog;
	}
}

void AuthThread::QueryAccount(RPacket rpkt) {
	unsigned short usNameLen;
	char const* pName = NULL;
	char szSql[512] = { 0 };
	SetRunLabel(11);

	// MRF_WARNING
	// Take the information in the package, the first is useless information, discarded (shit!)
	// LG("AuthThread", ",%d\n", ::GetCurrentThreadId());

	string  szLocale = "";
	szLocale = rpkt.ReadString();
	pName = rpkt.ReadString();

	// The second is the account name information	
	pName = rpkt.ReadString(&usNameLen);
	LG("PASSWD", "From GroupServer [%s] = [%d]\n", pName, strlen(pName));
	if ((pName == NULL) || (!IsValidName(pName, usNameLen))) {
		LG("AuthExcp", "NULL or INVALID Name field\n");
		m_AcctInfo.bExist = false;
		return;
	}

	m_AcctInfo.strName = pName;

	// Encrypted Info.
	m_AcctInfo.pDat = rpkt.ReadSequence(m_AcctInfo.usDatLen);
	m_AcctInfo.strMAC = rpkt.ReadString();
	m_AcctInfo.strChapString = rpkt.ReadString();
	in_addr ipAddr;
	ipAddr.S_un.S_addr = rpkt.ReadLong();
	m_AcctInfo.strIP = inet_ntoa(ipAddr);

	// Query the object
	SetRunLabel(12);
	CSQLRecordset rs(*m_pAuth);

	// Organize SQL query statements
	_snprintf_s(szSql, sizeof(szSql), _TRUNCATE, "select id, password, sid, login_status, login_group, ban, datediff(s, enable_login_time, getdate()) as protect_time from account_login where name='%s'", m_AcctInfo.strName.c_str());

	// Account_login table name field must do the only constraint
	rs << szSql;

	// Execute the query
	rs.SQLExecDirect();
	SetRunLabel(13);

	// Generate query results
	if (rs.SQLFetch()) {
		int n = 1;
		m_AcctInfo.bExist = true;
		m_AcctInfo.nId = rs.nSQLGetData(n++);
		m_AcctInfo.strPwdDigest = rs.SQLGetData(n++);
		m_AcctInfo.nSid = rs.nSQLGetData(n++);
		m_AcctInfo.nStatus = rs.nSQLGetData(n++);
		m_AcctInfo.strGroup = rs.SQLGetData(n++);
		m_AcctInfo.nBan = rs.nSQLGetData(n++);
		m_AcctInfo.nProtectTime = rs.nSQLGetData(n++);

		/*
		// Test the code for repeated login
		static bool flag = true;
		if (flag) {
		::Sleep(5000);
		}
		flag = !flag;
		*/

		if (!tmpLogin.Insert(m_AcctInfo.strName)) {
			m_AcctInfo.nStatus = ACCOUNT_ONLINE;
			LG("AuthExcp", "Account %s multilogin at same times.\n", m_AcctInfo.strName.c_str());
		}
	}
	else {
		m_AcctInfo.bExist = false;
	}
}

bool AuthThread::IsValidName(char const* szName, unsigned short usNameLen) {

	// Tom's version of the account number allows "_" "-" "." 3 characters
	if (usNameLen > 32)
		return false;

	const unsigned char *l_name = reinterpret_cast<const unsigned char *>(szName);
	bool l_ishan = false;
	for (unsigned short i = 0; i < usNameLen; i++) {
		if (!l_name[i]) {
			return false;
		}
		else if (l_ishan) {

			// Filter full-width spaces
			if (l_name[i - 1] == 0xA1 && l_name[i] == 0xA1) {
				return false;
			}

			if (l_name[i] >0x3F && l_name[i] < 0xFF && l_name[i] != 0x7F) {
				l_ishan = false;
			}
			else {
				return false;
			}
		}
		else if (l_name[i]>0x80 && l_name[i] < 0xFF) {
			l_ishan = true;
		}
		else if ((l_name[i] >= 'A' && l_name[i] <= 'Z') || (l_name[i] >= 'a' && l_name[i] <= 'z') || (l_name[i] >= '0' && l_name[i] <= '9') || (l_name[i] == '@') || (l_name[i] == '.')) {

		}
		else {
			return false;
		}
	}

	return !l_ishan;

	/*
	if (usNameLen > 20)
	return false;

	if (strstr(szName, "'") != NULL)
	return false;

	if (strstr(szName, "-") != NULL)
	return false;

	return true;
	*/
}

WPacket AuthThread::AccountLogin(DataSocket* datasock) {
	DWORD dwStartCount = GetTickCount();

	SetRunLabel(14);
	WPacket wpkt = datasock->GetWPacket();
	GroupServer2* pFromGroupServer = (GroupServer2 *)datasock->GetPointer();
	if (pFromGroupServer == NULL) {
		// This GroupServer is broken
		LG("AuthExcp", "pFromGroupServer = NULL\n");
		wpkt.WriteShort(ERR_AP_DISCONN);
		return wpkt;
	}
	SetRunLabel(15);

	// Avoid Cache Reuse Reason GroupServer read CMD_AP_KICKUSER command code
	wpkt.WriteCmd(0);

	if (!m_AcctInfo.bExist) {
		// Avoid Cache Reuse Reason GroupServer read CMD_AP_KICKUSER command code
		wpkt.WriteShort(ERR_AP_INVALIDUSER);
		return wpkt;
	}

	if (m_AcctInfo.nBan == 2) {
		tmpLogin.Remove(m_AcctInfo.strName);
		wpkt.WriteShort(ERR_AP_BANUSER);
		return wpkt;
	}
	else if (m_AcctInfo.nBan == 1) {
		tmpLogin.Remove(m_AcctInfo.strName);
		wpkt.WriteShort(ERR_AP_PBANUSER);
		return wpkt;
	}
	else if (m_AcctInfo.nBan == 3) {
		tmpLogin.Remove(m_AcctInfo.strName);
		wpkt.WriteShort(ERR_AP_SBANUSER);
		return wpkt;
	}

	SetRunLabel(16);

	KCHAPs ChapSvr;
	bool bVerify = false;
	ChapSvr.NewSessionKey();
	bVerify = ChapSvr.DoAuth(m_AcctInfo.strName.c_str(), m_AcctInfo.strChapString.c_str(), m_AcctInfo.pDat, m_AcctInfo.usDatLen, m_AcctInfo.strPwdDigest.c_str());
	ChapSvr.NewSessionKey();
	if (!bVerify) {
		// Authentication failed
		wpkt.WriteShort(ERR_AP_INVALIDPWD);
		LG("AccountAuth", "Thread#%d Auth [%s] (id=%d) failed: invalid password!\n", m_nIndex, m_AcctInfo.strName.c_str(), m_AcctInfo.nId);
		tmpLogin.Remove(m_AcctInfo.strName);
		return wpkt;
	}
	SetRunLabel(17);

	int nKeyLen = 0;
	char szKey[1024] = { 0 };
	char lpszSQL[2048] = { 0 };
	int nSid = GenSid(m_AcctInfo.strName.c_str());
	if (m_AcctInfo.nStatus == ACCOUNT_OFFLINE)
	{
		// Pre-landing state is not online (normal landing)
		_snprintf_s(lpszSQL, sizeof(lpszSQL), _TRUNCATE, "update account_login set login_status=%d, login_group='%s', enable_login_time=getdate(), last_login_time=getdate(), last_login_mac='%s', last_login_ip='%s' where id=%d", ACCOUNT_ONLINE, pFromGroupServer->GetName(), m_AcctInfo.strMAC.c_str(), m_AcctInfo.strIP.c_str(), m_AcctInfo.nId);
		if (m_pAuth->ExecuteSQL(lpszSQL)) {
			/*
			Win32Guid id;
			id.Generate();
			std::string SessionKey = id.AsString();
			char albumSQL[2048] = {0};
			_snprintf_s( albumSQL, sizeof(albumSQL), _TRUNCATE,"IF EXISTS(SELECT act_name FROM act_album WHERE act_name= '%s')UPDATE act_album SET SessionKey='%s', update_time = getdate() WHERE act_name= '%s' ELSE INSERT INTO act_album(act_name,SessionKey,act_id,create_time,update_time) VALUES('%s','%s',%d,getdate(),getdate())"
			,m_AcctInfo.strName.c_str(), SessionKey.c_str(), m_AcctInfo.strName.c_str(), m_AcctInfo.strName.c_str(), SessionKey.c_str(), m_AcctInfo.nId);
			if (m_pAuth->ExecuteSQL(albumSQL))
			*/
			{
				SetRunLabel(18);
				wpkt.WriteShort(ERR_SUCCESS);
				wpkt.WriteLong(m_AcctInfo.nId);
				ChapSvr.GetSessionEncKey(szKey, sizeof szKey, nKeyLen);
				wpkt.WriteSequence(szKey, nKeyLen);
				ChapSvr.GetSessionClrKey(szKey, sizeof szKey, nKeyLen);
				wpkt.WriteSequence(szKey, nKeyLen);
				wpkt.WriteLong(nSid);
				// wpkt.WriteString( SessionKey.c_str() );
				wpkt.WriteString("temp");
				LogUserLogin(m_AcctInfo.nId, m_AcctInfo.strName.c_str(), m_AcctInfo.strIP.c_str());
			}
			// else
			{
				/*	
				wpkt.WriteShort(ERR_AP_UNKNOWN);
				LG("AccountAuth", "Thread#%d Auth [%s] (id=%d) failed: update database error where normal login!\n", m_nIndex, m_AcctInfo.strName.c_str(), m_AcctInfo.nId);
				SetRunLabel(19);
				goto login_over;
				*/
			}
		}
		else {
			wpkt.WriteShort(ERR_AP_UNKNOWN);
			LG("AccountAuth", "Thread#%d Auth [%s] (id=%d) failed: update database error where normal login!\n", m_nIndex, m_AcctInfo.strName.c_str(), m_AcctInfo.nId);
			SetRunLabel(19);
			goto login_over;
		}
	}
	else if (m_AcctInfo.nStatus == ACCOUNT_ONLINE) {
		// Pre-login status is online (repeat login)
		_snprintf_s(lpszSQL, sizeof(lpszSQL), _TRUNCATE, "update account_login set login_status=%d, login_group='%s', last_login_time=getdate(), last_login_mac='%s', last_login_ip='%s' where id=%d", ACCOUNT_SAVING, pFromGroupServer->GetName(), m_AcctInfo.strMAC.c_str(), m_AcctInfo.strIP.c_str(), m_AcctInfo.nId);
		if (m_pAuth->ExecuteSQL(lpszSQL)) {
			if (pFromGroupServer->IsSame(m_AcctInfo.strGroup)) {
				wpkt.WriteCmd(CMD_AP_KICKUSER);
				wpkt.WriteShort(ERR_AP_ONLINE);
				wpkt.WriteLong(m_AcctInfo.nId);
			}
			else {
				KickAccount(m_AcctInfo.strGroup, m_AcctInfo.nId);
			}
			SetRunLabel(20);
			goto login_over;
		}
		else {
			wpkt.WriteShort(ERR_AP_UNKNOWN);
			LG("AccountAuth", "Thread#%d Auth [%s] (id=%d) failed: update database error when relogin!\n", m_nIndex, m_AcctInfo.strName.c_str(), m_AcctInfo.nId);
			SetRunLabel(21);
			goto login_over;
		}
	}
	else if (m_AcctInfo.nStatus == ACCOUNT_SAVING) {
		// Account in locked state (save role)
		if (m_AcctInfo.nProtectTime >= 0 && m_AcctInfo.nProtectTime < SAVING_TIME) {
			wpkt.WriteShort(ERR_AP_SAVING);
			SetRunLabel(22);
			goto login_over;
		}
		else {
			_snprintf_s(lpszSQL, sizeof(lpszSQL), _TRUNCATE, "update account_login set login_status=%d, login_group='%s', enable_login_time=getdate(), last_login_time=getdate(), last_login_mac='%s', last_login_ip='%s' where id=%d", ACCOUNT_ONLINE, pFromGroupServer->GetName(), m_AcctInfo.strMAC.c_str(), m_AcctInfo.strIP.c_str(), m_AcctInfo.nId);
			if (m_pAuth->ExecuteSQL(lpszSQL)) {
				/*
				Win32Guid id;
				id.Generate();
				std::string SessionKey = id.AsString();
				char albumSQL[2048] = {0};
				_snprintf_s( albumSQL, sizeof(albumSQL), _TRUNCATE,"IF EXISTS(SELECT act_name FROM act_album WHERE act_name= '%s')UPDATE act_album SET SessionKey='%s', update_time = getdate() WHERE act_name= '%s' ELSE INSERT INTO act_album(act_name,SessionKey,act_id,create_time,update_time) VALUES('%s','%s',%d,getdate(),getdate())"
				,m_AcctInfo.strName.c_str(), SessionKey.c_str(), m_AcctInfo.strName.c_str(), m_AcctInfo.strName.c_str(), SessionKey.c_str(), m_AcctInfo.nId);
				if (m_pAuth->ExecuteSQL(albumSQL))
				*/
				{
					SetRunLabel(23);
					wpkt.WriteShort(ERR_SUCCESS);
					wpkt.WriteLong(m_AcctInfo.nId);
					ChapSvr.GetSessionEncKey(szKey, sizeof szKey, nKeyLen);
					wpkt.WriteSequence(szKey, nKeyLen);
					ChapSvr.GetSessionClrKey(szKey, sizeof szKey, nKeyLen);
					wpkt.WriteSequence(szKey, nKeyLen);
					wpkt.WriteLong(nSid);
					// wpkt.WriteString( SessionKey.c_str() );
					wpkt.WriteString("temp");
					LogUserLogin(m_AcctInfo.nId, m_AcctInfo.strName.c_str(), m_AcctInfo.strIP.c_str());
				}
				/*
				else {
					wpkt.WriteShort(ERR_AP_UNKNOWN);
					LG("AccountAuth", "Thread#%d Auth [%s] (id=%d) failed: update database error when login without locked!\n", m_nIndex, m_AcctInfo.strName.c_str(), m_AcctInfo.nId);
					SetRunLabel(24);
					goto login_over;
				}
				*/
			}
			else {
				wpkt.WriteShort(ERR_AP_UNKNOWN);
				LG("AccountAuth", "Thread#%d Auth [%s] (id=%d) failed: update database error when login without locked!\n", m_nIndex, m_AcctInfo.strName.c_str(), m_AcctInfo.nId);
				SetRunLabel(24);
				goto login_over;
			}
		}
	}
	else {
		// The pre-login state is indeterminate
		wpkt.WriteShort(ERR_AP_UNKNOWN);
		LG("AccountAuth", "Thread#%d Auth [%s] (id=%d) failed: unknown last login status!\n", m_nIndex, m_AcctInfo.strName.c_str(), m_AcctInfo.nId);
		SetRunLabel(25);
		goto login_over;
	}
	SetRunLabel(0);

login_over:
	DWORD dwEndCount = GetTickCount() - dwStartCount;
	AuthThreadPool::RunLast[m_nIndex] = dwEndCount;
	if (dwEndCount > AuthThreadPool::RunConsume[m_nIndex]) {
		AuthThreadPool::RunConsume[m_nIndex] = dwEndCount;
	}
	tmpLogin.Remove(m_AcctInfo.strName);
	return wpkt;
}

void AuthThread::AccountLogout(RPacket rpkt) {
	SetRunLabel(99);
	char lpszSQL[2048] = { 0 };
	int nID = rpkt.ReadLong();
	_snprintf_s(lpszSQL, sizeof(lpszSQL), _TRUNCATE, "update account_login set login_status=%d, login_group='', enable_login_time=getdate(), last_logout_time=getdate(), total_live_time=total_live_time+datediff(second, last_login_time, getdate()) where id=%d", ACCOUNT_OFFLINE, nID);  //  增加在线时间 by jampe
	if (m_pAuth->ExecuteSQL(lpszSQL)) {
		SetRunLabel(0);
	}
	LogUserLogout(nID);
}

void AuthThread::BeginBilling(RPacket& rpkt) {
	// MRF_REMOVE: Is Useless.	
}

void AuthThread::EndBilling(RPacket& rpkt) {
	// MRF_REMOVE: Is Useless.
}

void AuthThread::ResetBilling() {
	// MRF_REMOVE: Is Useless.
}

void AuthThread::LogoutGroup(DataSocket* datasock, RPacket rpkt) {
	//	The server will be automatically reconnected immediately after the disconnect, so do not clear the account status, require GroupServer restart AccountServer also need to restart
	SetRunLabel(0);
}

CSQLDatabase *AuthThread::GetSQLDatabase() {
	return m_pAuth;
}

long AuthThread::GenSid(char const* szName) {
#define SHA1_DIGEST_LEN 20
	char md[SHA1_DIGEST_LEN];
	// Generate information sources
	char buf[256];
	int buf_len = _snprintf_s(buf, sizeof(buf), _TRUNCATE, "%s%d", szName, GetTickCount());
	if (buf_len >= sizeof buf)
		throw std::out_of_range("buffer overflow in GenSid()\n");

	// Generate a summary
	sha1((unsigned char *)buf, buf_len, (unsigned char *)md);

	// Remove the first 4 bytes
	long* ptr = (long *)md;
	return ptr[0];
}

void AuthThread::ResetAccount() {
	
	/*
	Here I see the code dizzy, if the ability to recommend rewriting, the current external main thread CDataBaseCtrl object can be initialized and post-database
	Note: Here m_nIndex 0 is not necessarily the first time, therefore, there may be a situation, when other threads have to carry out the work of verification services, 
	and index number 0 at this time to come in the initialization form will lead to form resources access conflict, Resulting in initialization failure
	*/

	// Discard the use of the following code
	return;	
}

void AuthThread::KickAccount(std::string& strGroup, int nId) {
	GroupServer2* pGs = g_As2->FindGroup(strGroup.c_str());
	if (pGs != NULL) {
		WPacket wpkt = pGs->GetWPacket();
		wpkt.WriteCmd(CMD_AP_KICKUSER);
		wpkt.WriteShort(ERR_AP_LOGINTWICE);
		wpkt.WriteLong(nId);
		pGs->SendData(wpkt);
	}
}

int AuthThread::Run() {
	Init();
	while (!GetExitFlag()) {

		// Giving 1 second to collect the network packets in the queue
		g_Auth.PeekPacket(1000);
	}
	Exit();

	ExitThread();
	return 0;
}

void AuthThread::SetRunLabel(int nRunLabel) {
	AuthThreadPool::RunLabel[m_nIndex] = nRunLabel;
}


int volatile AuthThreadPool::RunLabel[AT_MAXNUM] = { 0 };
DWORD volatile AuthThreadPool::RunLast[AT_MAXNUM] = { 0 };
DWORD volatile AuthThreadPool::RunConsume[AT_MAXNUM] = { 0 };

unsigned int volatile AuthThreadPool::uiAuthCount = 0;

void AuthThreadPool::IncAuthCount() { 
	++uiAuthCount; 
}

unsigned int AuthThreadPool::GetAuthCount() { 
	return uiAuthCount;
}

AuthThreadPool::AuthThreadPool() {
	for (int i = 0; i < AT_MAXNUM; ++i) {
		m_Pool[i] = new AuthThread(i);
	}
	AuthThread::LoadConfig();
}

AuthThreadPool::~AuthThreadPool() {
	for (int i = 0; i < AT_MAXNUM; ++i) {
		if (m_Pool[i] != NULL) {
			delete m_Pool[i];
			m_Pool[i] = NULL;
		}
	}
}

void AuthThreadPool::Launch() {
	for (int i = 0; i < AT_MAXNUM; ++i) {
		m_Pool[i]->Launch();
	}
}

void AuthThreadPool::NotifyToExit() {
	for (int i = 0; i < AT_MAXNUM; ++i) {
		m_Pool[i]->NotifyToExit();
	}
}

void AuthThreadPool::WaitForExit() {
	printf(RES_STRING(AS_ACCOUNTSERVER2_CPP_00006));
	for (int i = 0; i < AT_MAXNUM; ++i) {
		m_Pool[i]->WaitForExit(-1);
	}
}


// Network framework object
AccountServer2* g_As2 = NULL;
