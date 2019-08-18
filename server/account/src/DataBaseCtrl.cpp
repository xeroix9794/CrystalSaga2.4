#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#include "commrpc.h"
#include "util.h"
#include "databasectrl.h"
#include "inifile.h"
#include "GlobalVariable.h"
#include "AccountServer2.h"
#include "NetCommand.h"
#include "NetRetCode.h"

CDataBaseCtrl::CDataBaseCtrl(void) {
	m_strServerIP = "";
	m_strServerDB = "";
	m_strUserID = "";
	m_strUserPwd = "";
	m_pDataBase = NULL;
}

CDataBaseCtrl::~CDataBaseCtrl(void) {
	if (IsConnect()) {
		ReleaseObject();
	}
}

bool CDataBaseCtrl::CreateObject() {
	// Read the configuration
	char buf[512];
	dbc::IniFile inf(g_strCfgFile.c_str());
	dbc::IniSection& is = inf["db"];
	std::string strTmp = "";
	try {
		_snprintf_s(buf, sizeof(buf), "dbserver", _TRUNCATE);
		m_strServerIP = is[buf].c_str();

		_snprintf_s(buf, sizeof(buf), "db", _TRUNCATE);
		m_strServerDB = is[buf].c_str();

		_snprintf_s(buf, sizeof(buf), "userid", _TRUNCATE);
		m_strUserID = is[buf].c_str();

		_snprintf_s(buf, sizeof(buf), "passwd", _TRUNCATE);
		strTmp = is[buf].c_str();
	}
	catch (dbc::excp& e) {
		cout << e.what() << endl;
		return false;
	}

	dbpswd_out(strTmp.c_str(), (int)strTmp.length(), m_strUserPwd);
	if (!Connect()) 
		return false;

	// Initialize and test the database fields
	try {
		m_pDataBase->ExecuteSQL("select ban from account_login where 1 = 2");

		m_pDataBase->ExecuteSQL("select log_id, user_id, user_name, login_time, logout_time, login_ip from user_log where 1= 2");
		m_pDataBase->ExecuteSQL("select act_name, SessionKey, act_id, create_time, update_time from act_album where 1 = 2");
	}
	catch (CSQLException* se) {
		LG("DBExcp", "Check data field failure! SQL Exception in CDataBaseCtrl::CreateObject(): %s\n", se->m_strError.c_str());
		printf("Check data field failure! SQL Exception in CDataBaseCtrl::CreateObject(): %s\r\n", se->m_strError.c_str());
		return false;
	}
	catch (...) {
		LG("DBExcp", "Check data field failure! unknown exception raised from CDataBaseCtrl::CreateObject()\n");
		printf("Check data field failure! unknown exception raised from CDataBaseCtrl::CreateObject()\n");
		return false;
	}

	return true;
}

void CDataBaseCtrl::ReleaseObject() {
	Disconnect();
}

bool CDataBaseCtrl::Connect() {
	if (IsConnect()) return true;

	// Create a database object
	try {
		m_pDataBase = new CSQLDatabase();
	}
	catch (std::bad_alloc& e) {
		LG("DBExcp", "CDataBaseCtrl::CreateObject() new failed: %s\n", e.what());
		m_pDataBase = NULL;
		return false;
	}
	catch (...) {
		LG("DBExcp", "CDataBaseCtrl::CreateObject() unknown exception\n");
		m_pDataBase = NULL;
		return false;
	}

	// Connect to the database
	char buf[512] = { 0 };
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "DRIVER={SQL Server};SERVER=%s;UID=%s;PWD=%s;DATABASE=%s", m_strServerIP.c_str(), m_strUserID.c_str(), m_strUserPwd.c_str(), m_strServerDB.c_str());
	
	if (!m_pDataBase->Open(buf)) {
		SAFE_DELETE(m_pDataBase);
		return false;
	}
	m_pDataBase->SetAutoCommit(true);
	
	return true;
}

bool CDataBaseCtrl::IsConnect() {
	return (m_pDataBase != NULL);
}

void CDataBaseCtrl::Disconnect() {
	if (m_pDataBase != NULL) {
		try {
			m_pDataBase->Close();
		}
		catch (...) {
			LG("DBExcp", "Exception raised when CDataBaseCtrl::Disconnect()\n");
		}
		SAFE_DELETE(m_pDataBase);
	}
}

bool CDataBaseCtrl::UserLogin(int nUserID, string strUserName, string strIP) {
	if (!strUserName.c_str() || strUserName == "") {
		LG("AccountServer", "CDataBaseCtrl::UserLogin: parameter strUserName is empty or null\n");
		return false;
	}

	if (!strIP.c_str()) 
		strIP = "";

	char buf[1024];
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "insert into user_log (user_id, user_name, login_time, login_ip) values (%d, '%s', getdate(), '%s')", nUserID, strUserName.c_str(), strIP.c_str());
	string strSQL = buf;
	
	if (!IsConnect()) 
		Connect();

	if (IsConnect()) {
		try {
			m_pDataBase->ExecuteSQL(strSQL.c_str());
			return true;
		}
		catch (CSQLException* se) {
			LG("DBExcp", "SQL Exception in CDataBaseCtrl::UserLogin: %s\n", se->m_strError.c_str());
		}
		catch (...) {
			LG("DBExcp", "unknown exception raised from CDataBaseCtrl::UserLogin\n");
		}
	}
	LG("AccountServer", "CDataBaseCtrl::UserLogin: A record of user login cannot be saved! UserID=%d UserName=%s IP=%s\n\n", nUserID, strUserName.c_str(), strIP.c_str());

	Disconnect();
	return false;
}

bool CDataBaseCtrl::UserLogout(int nUserID) {
	char buf[1024];
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "update user_log set logout_time=getdate() where log_id=(select max(log_id) from user_log where user_id=%d)", nUserID);
	string strSQL = buf;

	if (!IsConnect()) 
		Connect();

	if (IsConnect()) {
		try {
			m_pDataBase->ExecuteSQL(strSQL.c_str());
			return true;
		}
		catch (CSQLException* se) {
			LG("DBExcp", "SQL Exception in CDataBaseCtrl::UserLogout: %s\n", se->m_strError.c_str());
		}
		catch (...) {
			LG("DBExcp", "unknown exception raised from CDataBaseCtrl::UserLogout\n");
		}
	}
	LG("AccountServer", "CDataBaseCtrl::UserLogout: A record of user logout cannot be saved! UserID=%d \n", nUserID);

	Disconnect();
	return false;
}

bool CDataBaseCtrl::KickUser(std::string strUserName) {
	if (!strUserName.c_str() || strUserName == "") {
		LG("AccountServer", "CDataBaseCtrl::KickUser: parameter strUserName is empty or null\n");
		return false;
	}
	
	char buf[1024];
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "select id, login_group, last_logout_time from account_login where name='%s'", strUserName.c_str());
	string strSQL = buf;
	
	if (!IsConnect()) 
		Connect();

	std::string strUserLeave = "";
	std::string strGroupServerName = "";
	if (IsConnect()) {
		try {
			CSQLRecordset rs(*m_pDataBase);
			rs.SQLExecDirect(strSQL.c_str());
			if (rs.SQLFetch()) {
				int nUserID = rs.nSQLGetData(1);
				strGroupServerName = rs.SQLGetData(2);
				strUserLeave = rs.SQLGetData(3);
				if (strGroupServerName != "") {
					if (g_As2) {
						GroupServer2* pGs = g_As2->FindGroup(strGroupServerName.c_str());
						if (pGs != NULL) {
							WPacket wpkt = pGs->GetWPacket();
							wpkt.WriteCmd(CMD_AP_KICKUSER);
							wpkt.WriteShort(ERR_AP_NOBILL);
							wpkt.WriteLong(nUserID);
							pGs->SendData(wpkt);
							return true;
						}
						else {
							LG("AccountServer", "CDataBaseCtrl::KickUser pGs=NULL, groupname = %s error!UserName=%s\r\n", strGroupServerName.c_str(), strUserName.c_str());
						}
					}
					else {
						LG("AccountServer", "CDataBaseCtrl::KickUser a_As2 error!UserName=%s\r\n", strUserName.c_str());
					}
				}
				else {
					LG("AccountServer", "CDataBaseCtrl::KickUser groupname error!UserName=%s\r\n", strUserName.c_str());
				}
			}
			else {
				LG("AccountServer", "CDataBaseCtrl::KickUser db error!UserName=%s\r\n", strUserName.c_str());
			}
		}
		catch (CSQLException* se) {
			LG("DBExcp", "SQL Exception in CDataBaseCtrl::KickUser: %s \n", se->m_strError.c_str());
		}
		catch (...) {
			LG("DBExcp", "Unknown exception raised from CDataBaseCtrl::KickUser\n");
		}
	}
	else {
		LG("DBExcp", "Unknown exception raised from CDataBaseCtrl::KickUser Valid connect db, Username=%s\n", strUserName.c_str());
	}

	LG("AccountServer", "CDataBaseCtrl::KickUser: Kick user failed! Username=%s, group_name = %s, last_leave = %s \n", strUserName.c_str(), strGroupServerName.c_str(), strUserLeave.c_str());
	Disconnect();

	return false;
}

void CDataBaseCtrl::SetExpScale(std::string strUserName, long time) {
	if (!strUserName.c_str() || strUserName == "") {
		LG("AccountServer", "CDataBaseCtrl::SetExpScale: parameter strUserName is empty or null\n");
		return;
	}

	char strSQL[1024];
	_snprintf_s(strSQL, sizeof(strSQL), _TRUNCATE, "select id, login_group, last_logout_time from account_login where name='%s'", strUserName.c_str());
	if (!IsConnect()) 
		Connect();

	std::string strUserLeave = "";
	std::string strGroupServerName = "";
	if (IsConnect()) {
		try {
			CSQLRecordset rs(*m_pDataBase);
			rs.SQLExecDirect(strSQL);
			if (rs.SQLFetch()) {
				int nUserID = rs.nSQLGetData(1);
				strGroupServerName = rs.SQLGetData(2);
				strUserLeave = rs.SQLGetData(3);
				if (strGroupServerName != "") {
					if (g_As2) {
						GroupServer2* pGs = g_As2->FindGroup(strGroupServerName.c_str());
						if (pGs != NULL) {
							WPacket wpkt = pGs->GetWPacket();
							wpkt.WriteCmd(CMD_AP_EXPSCALE);
							wpkt.WriteLong(nUserID);
							wpkt.WriteLong(time);
							pGs->SendData(wpkt);
							return;
						}
						else {
							LG("AccountServer", "CDataBaseCtrl::SetExpScale pGs=NULL, groupname = %s error!UserName=%s\r\n", strGroupServerName.c_str(), strUserName.c_str());
						}
					}
					else {
						LG("AccountServer", "CDataBaseCtrl::SetExpScale a_As2 error!UserName=%s\r\n", strUserName.c_str());
					}
				}
				else {
					LG("AccountServer", "CDataBaseCtrl::SetExpScale groupname error!UserName=%s\r\n", strUserName.c_str());
				}
			}
			else {
				LG("AccountServer", "CDataBaseCtrl::SetExpScale db error!UserName=%s\r\n", strUserName.c_str());
			}
		}
		catch (CSQLException* se) {
			LG("DBExcp", "SQL Exception in CDataBaseCtrl::SetExpScale: %s \n", se->m_strError.c_str());
		}
		catch (...) {
			LG("DBExcp", "Unknown exception raised from CDataBaseCtrl::SetExpScale\n");
		}
	}
	else {
		LG("DBExcp", "Unknown exception raised from CDataBaseCtrl::SetExpScale Valid connect db, Username=%s\n", strUserName.c_str());
	}

	LG("AccountServer", "CDataBaseCtrl::SetExpScale: Set user Exp Scale failed! Username=%s, group_name = %s, last_leave = %s \n", strUserName.c_str(), strGroupServerName.c_str(), strUserLeave.c_str());
	Disconnect();
}

bool CDataBaseCtrl::UserLoginMap(std::string strUserName, std::string strPassport) {
	/*
	if (!strUserName.c_str() || strUserName=="") {
		LG("AccountServer", "CDataBaseCtrl::UserLoginMap: parameter strUserName is empty or null\n");
		return false;
	}

	CDataBaseCtrl::sPlayerData sData;
	sData.ctLoginTime = CTime::GetCurrentTime();
	m_mapUsers[strUserName.c_str()] = sData;
	*/
	return true;
}

bool CDataBaseCtrl::UserLogoutMap(std::string strUserName) {
	/*
	if (!strUserName.c_str() || strUserName=="") {
		LG("AccountServer", "CDataBaseCtrl::UserLogoutMap: parameter strUserName is empty or null\n");
		return false;
	}

	StringMap::const_iterator iter=m_mapUsers.find(strUserName.c_str());
	if (iter==m_mapUsers.end()) {
		LG("AccountServer", "CDataBaseCtrl::UserLogoutMap : User [%s] not found in map, unable update the live time when logout\n", strUserName.c_str());
		return false;
	}

	CDataBaseCtrl::sPlayerData sData=iter->second;
	CTimeSpan ctSpan=CTime::GetCurrentTime() - sData.ctLoginTime;
	if (ctSpan > CTimeSpan(5) && ctSpan< CTimeSpan(30, 0, 0, 0)) {
		LG("AccountServer", "CDataBaseCtrl::UserLogoutMap: Update the live time! Username=%s \n", strUserName.c_str());
		m_mapUsers.erase(strUserName.c_str());
		
		return true;
	}
	m_mapUsers.erase(strUserName.c_str());
	Disconnect();
	*/
	return false;
}

bool CDataBaseCtrl::OperAccountBan(std::string strActName, int iban) {
	if (!strActName.c_str() || strActName == "") {
		LG("AccountServer", "CDataBaseCtrl::OperAccountBan: parameter strActName is empty or null\n");
		return false;
	}

	char buf[1024];
	_snprintf_s(buf, sizeof(buf), _TRUNCATE, "update account_login set ban = %i where name = '%s'", iban, strActName.c_str());
	string strSQL = buf;
	if (!IsConnect()) 
		Connect();
	
	if (IsConnect()) {
		try {
			m_pDataBase->ExecuteSQL(strSQL.c_str());
			return true;
		}
		catch (CSQLException* se) {
			LG("DBExcp", "SQL Exception in CDataBaseCtrl::OperAccountBan: %s\n", se->m_strError.c_str());
		}
		catch (...) {
			LG("DBExcp", "unknown exception raised from CDataBaseCtrl::OperAccountBan\n");
		}
	}
	LG("AccountServer", "CDataBaseCtrl::OperAccountBan: A record of user account cannot be ban! accName[%s]\n", strActName.c_str());

	Disconnect();
	return false;
}
