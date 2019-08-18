#pragma once

#include "atltime.h"

#include <map>

class CSQLDatabase;

// This class only allows the main thread to call
class CDataBaseCtrl {
public:
	CDataBaseCtrl(void);
	~CDataBaseCtrl(void);
	bool CreateObject();
	void ReleaseObject();

	bool KickUser(std::string strUserName);
	void SetExpScale(std::string strUserName, long time);
	
	bool UserLogin(int nUserID, std::string strUserName, std::string strIP);
	bool UserLogout(int nUserID);
	bool UserLoginMap(std::string strUserName, std::string strPassport);
	bool UserLogoutMap(std::string strUserName);
	bool OperAccountBan(std::string strActName, int iban);

private:
	struct sPlayerData {
		CTime ctLoginTime;
	};
	typedef std::map<std::string, CDataBaseCtrl::sPlayerData> StringMap;

private:
	bool Connect();
	bool IsConnect();
	void Disconnect();

private:
	std::string m_strServerIP;
	std::string m_strServerDB;
	std::string m_strUserID;
	std::string m_strUserPwd;
	CSQLDatabase* m_pDataBase;
	StringMap m_mapUsers;
};
