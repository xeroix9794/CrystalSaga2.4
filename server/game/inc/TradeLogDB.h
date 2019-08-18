
#include "stdafx.h"
#include <time.h>


// Logר�ñ�
class CTradeTableLog : public cfl_rs
{
public:
    CTradeTableLog(cfl_db *pDB)
        :cfl_rs(pDB, "Trade_Log", 10)
    {T_B
    
	T_E}

	void ExecLogSQL(char* time, const char* gameServerName, const char* action, const char *pszChaFrom, const char *pszChaTo, const char *pszTrade)
	{
		char format[] = "insert into Trade_Log(ExecuteTime,GameServer,[Action],[From],[To],Memo) values ('%s','%s','%s','%s','%s','%s')";
		
		char sql[1024];
		memset(sql, 0, sizeof(sql));
		//sprintf(sql, format, time, gameServerName, action, pszChaFrom, pszChaTo, pszTrade);
		_snprintf_s(sql,sizeof(sql),_TRUNCATE, format, time, gameServerName, action, pszChaFrom, pszChaTo, pszTrade);

		SQLRETURN l_sqlret  =  this->exec_sql_direct(sql);
		if(!DBOK(l_sqlret))
		{
			//LG("gamelog", "���log��¼ʧ��, sql = [%s]!\n", sql);
			LG("gamelog", "add log note failed, sql = [%s]!\n", sql);
		}
	}
};

class CTradeLogDB
{
public:

    CTradeLogDB::CTradeLogDB()
    : _connect(), _tab_log(NULL), m_bInitOK(FALSE)
    {T_B

    T_E}

    BOOL    Init();

	void ExecLogSQL(const char* gameServerName, const char* action, const char *pszChaFrom, const char *pszChaTo, const char *pszTrade)
	{
		time_t ltime;
		time(&ltime);

		//tm* ttm  = localtime(&ltime);
		tm ttm;
		localtime_s( &ttm, &ltime );
		char buff[20];
		buff[19] = 0;
        //sprintf(buff, "%04i/%02i/%02i %02i:%02i:%02i", ttm->tm_year + 1900, ttm->tm_mon + 1, ttm->tm_mday, ttm->tm_hour, ttm->tm_min, ttm->tm_sec);
		_snprintf_s(buff,sizeof(buff),_TRUNCATE, "%04i/%02i/%02i %02i:%02i:%02i", ttm.tm_year + 1900, ttm.tm_mon + 1, ttm.tm_mday, ttm.tm_hour, ttm.tm_min, ttm.tm_sec);
		
		//2008/03/21 12:00:00
		_tab_log->ExecLogSQL(buff, gameServerName, action, pszChaFrom, pszChaTo, pszTrade);
	}
	
	BOOL	m_bInitOK;

protected:

    cfl_db			_connect;
	CTradeTableLog*		_tab_log;
};


extern CTradeLogDB tradeLog_db;
