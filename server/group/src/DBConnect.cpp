#include "Friend.h"
#include "Master.h"
#include "DBConnect.h"
#include "Guild.h"
#include "GroupServerApp.h"
#include "GameCommon.h"

SQLRETURN Exec_sql_direct(const char *pszSQL, cfl_rs *pTable) {
	LG("group_sql", "Table [%s], begin execute SQL [%s]\n", pTable->get_table(), pszSQL);
	SQLRETURN r = pTable->exec_sql_direct(pszSQL);
	if (DBOK(r)) {
		LG("group_sql", "execute SQL success!");
	}
	else if (DBNODATA(r)) {
		LG("group_sql", "execute SQL, no result \n");
	}
	else {
		LG("group_sql", "execute SQL, failed!\n");
	}
	return r;
}

SQLRETURN Exec_sql(const char *pszSQL, char const* pdata, int len, cfl_rs *pTable) {
	LG("group_sql", "Table [%s], begin execute SQL [%s]\n", pTable->get_table(), pszSQL);
	SQLRETURN r = pTable->exec_param_sql(pszSQL, pdata, len);
	if (DBOK(r)) {
		LG("group_sql", "execute SQL success!");
	}
	else if (DBNODATA(r)) {
		LG("group_sql", "execute SQL, no result \n");
	}
	else {
		LG("group_sql", "execute SQL, failed!\n");
	}
	return r;
}

// TBL System
bool TBLAccounts::IsReady() {
	char sql[SQL_MAXLEN];
	strncpy_s(sql, sizeof(sql), "drop trigger [TR_D_Character_Friends]", _TRUNCATE);

	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	if (!DBOK(l_ret)) {
		LogLine l_line(g_LogDB);
		l_line << newln << "SQL:" << sql << " execute failed !";
	}
	strncpy_s(sql, sizeof(sql), "drop trigger [TR_I_Character]", _TRUNCATE);

	l_ret = Exec_sql_direct(sql, this);
	if (!DBOK(l_ret)) {
		LogLine l_line(g_LogDB);
		l_line << newln << "SQL:" << sql << " execute failed !";
	}
	strncpy_s(sql, sizeof(sql), "CREATE TRIGGER TR_D_Character_Friends ON character \n\
								 FOR DELETE \n\
								 AS\n\
								 BEGIN\n\
									 declare @@stat tinyint\n\
									 declare @@gid  int\n\
									 select @@stat =guild_stat,@@gid =guild_id from deleted\n\
									 DELETE friends where friends.cha_id1 IN(select cha_id from deleted)\n\
									 if(@@gid >0)\n\
									 BEGIN\n\
										update guild set try_total =try_total -(case when @@stat>0 then 1 else 0 end),\n\
										member_total =member_total -(case when @@stat >0 then 0 else 1 end)\n\
										where guild_id >0 and guild_id =@@gid\n\
									END\n\
								END\n\
								", _TRUNCATE);

	l_ret = Exec_sql_direct(sql, this);
	if (!DBOK(l_ret)) {
		LogLine l_line(g_LogDB);
		l_line << newln << "SQL:" << sql << " execute failed !";
		return false;
	}
	strncpy_s(sql, sizeof(sql), "CREATE TRIGGER TR_I_Character ON character\n\
								 FOR INSERT\n\
								 AS\n\
								 BEGIN\n\
									 declare @l_icon smallint\n\
									 select @l_icon =convert(smallint,SUBSTRING(inserted.look,5,1)) from inserted\n\
									update character set icon =@l_icon where cha_id in (select cha_id from inserted)\n\
								END\n\
								", _TRUNCATE);

	l_ret = Exec_sql_direct(sql, this);
	if (!DBOK(l_ret)) {
		LogLine l_line(g_LogDB);
		l_line << newln << "SQL:" << sql << " execute failed !";
		return false;
	}

	return true;
}

// TBL Account
void TBLAccounts::AddStatLog(long login, long play, long wgplay) {
	char sql[SQL_MAXLEN];

	// The id field of the account_save table is the primary key
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "insert stat_log (login_num , play_num, wgplay_num) values (%d, %d, %d)", login, play, wgplay);
	Exec_sql_direct(sql, this);
}

bool TBLAccounts::SetDiscInfo(int actid, const char *cli_ip, const char *reason) {
	char sql[SQL_MAXLEN];

	// The id field of the account_save table is the primary key
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set last_ip='%s',disc_reason ='%s',last_leave =getdate() where act_id =%d", _get_table(), cli_ip, reason, actid);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return  (DBOK(l_ret)) ? true : false;
}

bool TBLAccounts::InsertRow(int act_id, const char *act_name, const char *cha_ids, bool equal) {
	char sql[SQL_MAXLEN];
	DWORD dwActID = 0;

	if (equal) {
		dwActID = act_id;
	}
	else {
		std::string buf[1];

		char param[80];
		_snprintf_s(param, sizeof(param), _TRUNCATE, "TOP 1 act_id");

		char filter[80];
		_snprintf_s(filter, sizeof(filter), _TRUNCATE, "ORDER BY act_id DESC");

		int r1 = 0;
		int r = _get_rowOderby(buf, 1, param, filter, &r1);
		if (DBOK(r) && r1 > 0) {
			dwActID = atol(buf[0].c_str()) + 1;
		}
		else {
			dwActID = 1;
		}
	}

	// The id field of the account_save table is the primary key
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "insert %s (act_id, act_name, cha_ids) values (%d, '%s', '%s')", _get_table(), dwActID, act_name, cha_ids);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLAccounts::UpdateRow(int act_id, const char *cha_ids) {
	char sql[SQL_MAXLEN];

	// The id field of the account_save table is the primary key
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set cha_ids='%s' where act_id=%d", _get_table(), cha_ids, act_id);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLAccounts::UpdatePassword(int act_id, const char szPassword[]) {
	char sql[SQL_MAXLEN];
	char password[40];
	size_t size;
	memset(password, 0, sizeof(password));

	int nResult = Util::ConvertDBParam(szPassword, password, sizeof(password), size);

	// MRF_RECHECK: Should have checked nResult first.

	// The id field of the account_save table is the primary key
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set password='%s' where act_id=%d", _get_table(), password, act_id);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

int TBLAccounts::FetchRowByActName(const char szAccount[]) {
	int l_retrow = 0;
	char* param = "act_id,gm,cha_ids,password,last_ip,disc_reason,convert(varchar(20),last_leave,120)";
	char filter[200];
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "act_name='%s'", szAccount);

	if (_get_row(m_buf, 7, param, filter, &l_retrow)) {
		if (l_retrow == 1 && get_affected_rows() == 1) {
			return l_retrow;
		}
		else {
			return 0;
		}
	}
	else {
		return -1;
	}
}

int TBLAccounts::FetchRowByActID(int act_id) {
	int l_retrow = 0;
	char* param = "act_name,gm,cha_ids,password,last_ip,disc_reason,convert(varchar(20),last_leave,120)";
	char filter[200];
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "act_id=%d", act_id);
	if (_get_row(m_buf, 7, param, filter, &l_retrow)) {
		if (l_retrow == 1 && get_affected_rows() == 1) {
			return l_retrow;
		}
		else {
			return 0;
		}
	}
	else {
		return -1;
	}
}

// TBLCharacters
bool TBLCharacters::ZeroAddr() {
	char sql[SQL_MAXLEN];

	// The id field of the account_save table is the primary key
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set mem_addr =0 where mem_addr != 0", _get_table());
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLCharacters::SetAddr(long cha_id, long addr) {
	char sql[SQL_MAXLEN];

	// The id field of the account_save table is the primary key
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set mem_addr =%d where cha_id =%d", _get_table(), addr, cha_id);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLCharacters::InsertRow(const char *cha_name, int act_id, const char *birth, const char *map, const char *look) {
	char sql[SQL_MAXLEN];

	// The id field of the account_save table is the primary key
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "insert %s (cha_name, act_id, birth,map,look) values ('%s', %d, '%s','%s', '%s')", _get_table(), cha_name, act_id, birth, map, look);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLCharacters::UpdateInfo(unsigned long cha_id, unsigned short icon, const char * motto) {
	char sql[SQL_MAXLEN];

	// The id field of the account_save table is the primary key
	char buff[255];
	memset(buff, 0, sizeof(buff));
	size_t count;
	int size = Util::ConvertDBParam(motto, buff, sizeof(buff), count);
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set icon =%d,motto ='%s' where cha_id =%d", _get_table(), icon, buff, cha_id);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

int TBLCharacters::FetchRowByChaName(const char *cha_name) {
	int l_retrow = 0;
	char* param = "cha_id,motto,icon";
	char filter[200];
	char buff[100];
	size_t size;
	memset(buff, 0, sizeof(buff));

	int nResult = Util::ConvertDBParam(cha_name, buff, sizeof(buff), size);

	// MRF_RECHECK: Should have checked nResult first.

	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "cha_name='%s'", buff);
	if (_get_row(m_buf, 3, param, filter, &l_retrow)) {
		if (l_retrow == 1 && get_affected_rows() == 1) {
			return l_retrow;
		}
		else {
			return 0;
		}
	}
	else {
		return -1;
	}
}

bool TBLCharacters::FetchAccidByChaName(const char *cha_name, int& cha_accid) {
	int l_retrow = 0;
	char* param = "act_id";
	char filter[200];
	char buff[100];
	memset(buff, 0, sizeof(buff));

	size_t size;
	int nResult = Util::ConvertDBParam(cha_name, buff, sizeof(buff), size);

	// MRF_RECHECK: Should have checked nResult first.

	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "cha_name='%s'", buff);
	if (_get_row(m_buf, 1, param, filter, &l_retrow)) {
		if (l_retrow == 1 && get_affected_rows() == 1) {
			cha_accid = atoi(m_buf[0].c_str());
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

bool TBLCharacters::StartEstopTime(int cha_id) {
	char sql[SQL_MAXLEN];
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set estop = getdate() where cha_id =%d", _get_table(), cha_id);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLCharacters::EndEstopTime(int cha_id) {
	char sql[SQL_MAXLEN];
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set estoptime = estoptime - datediff(minute, estop, getdate()) where cha_id =%d", _get_table(), cha_id);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLCharacters::IsEstop(int cha_id) {
	int l_retrow = 0;
	char* param = "estop";
	char filter[200];
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "cha_id = %d and dateadd(minute, estoptime, estop) > getdate()", cha_id);
	if (_get_row(m_buf, 1, param, filter, &l_retrow)) {
		if (l_retrow == 1 && get_affected_rows() == 1) {
			return true;
		}
		else {
			return false;
		}
	}
	return true;
}

bool TBLCharacters::Estop(const char *cha_name, uLong lTimes) {
	char sql[SQL_MAXLEN];
	char buff[100];
	size_t size;
	memset(buff, 0, sizeof(buff));

	int nResult = Util::ConvertDBParam(cha_name, buff, sizeof(buff), size);

	// MRF_RECHECK: Should have checked nResult first.

	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set estop = getdate(), estoptime = %d where cha_name ='%s'", _get_table(), lTimes, buff);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLCharacters::AddMoney(int cha_id, DWORD dwMoney) {
	char sql[SQL_MAXLEN];
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set gd = gd + %d where cha_id ='%d'", _get_table(), dwMoney, cha_id);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLCharacters::DelEstop(const char *cha_name) {
	char sql[SQL_MAXLEN];
	char buff[100];
	size_t size;
	memset(buff, 0, sizeof(buff));
	int nResult = Util::ConvertDBParam(cha_name, buff, sizeof(buff), size);
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set estoptime = %d where cha_name ='%s'", _get_table(), 0, buff);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

int TBLCharacters::FetchRowByChaID(int cha_id) {
	int l_retrow = 0;
	string param = string("c.cha_name,c.motto,c.icon,\
							case when c.guild_stat =0 then c.guild_id else 0 end,\
							case when c.guild_stat <>0 or c.guild_id =0 then '")
							+ string("(none]")
							+ string("' else g.guild_name end,\
							c.job,c.degree,c.map,c.map_x,c.map_y,c.look,c.str,c.dex,c.agi,c.con,c.sta,c.luk,c.guild_permission");

	char filter[200];
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "c.guild_id =g.guild_id and c.cha_id=%d", cha_id);
	std::string	l_tblname = _tbl_name;
	_tbl_name = "character c,guild g";

	bool l_bret = false;
	try {
		l_bret = _get_row(m_buf, CHA_MAXCOL, const_cast<char*>(param.c_str()), filter, &l_retrow);
	}
	catch (...) {
		LG("group_sql", "TBLCharacters::FetchRowByChaID execute SQL, failed!,cha_id =%d\n", cha_id);
	}

	_tbl_name = l_tblname;
	if (l_bret) {
		if (l_retrow == 1 && get_affected_rows() == 1) {
			return l_retrow;
		}
		else {
			return 0;
		}
	}
	else {
		return -1;
	}
}

int TBLCharacters::FetchActNameRowByChaID(int cha_id, string & act_name) {
	int l_retrow = 0;
	string param = string("a.act_name");
	char filter[200];
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "c.cha_id = %d and c.act_id = a.act_id ", cha_id);
	std::string	l_tblname = _tbl_name;
	_tbl_name = "character c,account a";

	bool l_bret = false;
	try {
		l_bret = _get_row(m_buf, CHA_MAXCOL, const_cast<char*>(param.c_str()), filter, &l_retrow);
	}
	catch (...) {
		LG("group_sql", "TBLCharacters::FetchActNameRowByChaID execute SQL, failed!,cha_id =%d\n", cha_id);
	}

	_tbl_name = l_tblname;
	if (l_bret) {
		if (l_retrow == 1 && get_affected_rows() == 1) {
			act_name = m_buf[0];
			return l_retrow;
		}
		else {
			return 0;
		}
	}
	else {
		return -1;
	}
}

bool TBLCharacters::BackupRow(int cha_id) {
	char sql[SQL_MAXLEN];
	string buf[3];
	char filter[80];
	char*param = "guild_id, guild_stat";
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "cha_id =%d", cha_id);
	int l_retrow = 0;
	DWORD dwGuildID;
	BYTE byType;

	bool bret = _get_row(buf, 2, param, filter, &l_retrow);
	if (l_retrow == 1) {
		dwGuildID = atoi(buf[0].c_str());
		byType = atoi(buf[1].c_str());
		if (dwGuildID > 0) {

			// Reduce the Association(Guild) of information count
			if (byType == emGldMembStatNormal) {
				// Already a member
				_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update guild set member_total =member_total -1 where guild_id =%d and member_total > 0", dwGuildID);
				SQLRETURN l_sqlret = Exec_sql_direct(sql, this);
				if (!DBOK(l_sqlret)) {
					LG("GuildSystem", "1>Reject:delete cha£¬update guild count failed! database sql failed .ret = ", l_sqlret);
					return false;
				}
				else {
					if (get_affected_rows() != 1) {
						LG("GuildSystem", "2>Reject:delete cha£¬update guild count failed! database sql failed .ret = ", l_sqlret);
						return false;
					}
					else {
						// Nothing happens, this else statement should be removed.
					}
				}
			}
			else {
				// Currently applying
				_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update guild set try_total =try_total -1 where guild_id =%d and try_total > 0", dwGuildID);
				SQLRETURN l_sqlret = Exec_sql_direct(sql, this);
				if (!DBOK(l_sqlret)) {
					LG("GuildSystem", "1>BackupRow:delete cha£¬update guild count failed! database sql failed .ret = ", l_sqlret);
					return false;
				}
				else {
					if (get_affected_rows() != 1) {
						LG("GuildSystem", "2>BackupRow:delete cha£¬update guild count failed! database sql failed .ret = ", l_sqlret);
						return false;
					}
					else {
						// Nothing happens, this else statement should be removed.
					}
				}
			}
		}
	}
	else {
		LG("GuildSystem", "BackupRow:delete cha£¬get guild info failed! database sql failed.cha_id = ", cha_id);
		return false;
	}

	// The deletion time is independent
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set delflag =1,deldate =getdate() where cha_id=%d", _get_table(), cha_id);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

// MRF_REWRITE: This requires to be rewritten as we deleted lots of these stuff already.
bool TBLPersonInfo::AddInfo(int cha_id, stPersonInfo* pInfo) {
	char sql[SQL_MAXLEN];

	char buszMotto[40];
	memset(buszMotto, 0, sizeof(buszMotto));

	char buszName[50];
	memset(buszName, 0, sizeof(buszName));

	char buszAnimalZodiac[50];
	memset(buszAnimalZodiac, 0, sizeof(buszAnimalZodiac));

	char buszBloodType[50];
	memset(buszBloodType, 0, sizeof(buszBloodType));

	char buszState[50];
	memset(buszState, 0, sizeof(buszState));

	char buszCity[50];
	memset(buszCity, 0, sizeof(buszCity));

	char buszConstellation[50];
	memset(buszConstellation, 0, sizeof(buszConstellation));

	char buszCareer[50];
	memset(buszCareer, 0, sizeof(buszCareer));

	size_t sizeMotto, sizeName, sizeAnimalZodiac, sizeBloodType, sizeState, sizeCity, sizeConstellation, sizeCareer;
	int nResultMotto = Util::ConvertDBParam(pInfo->szMotto, buszMotto, sizeof(buszMotto), sizeMotto);
	int nResultName = Util::ConvertDBParam(pInfo->szName, buszName, sizeof(buszName), sizeName);
	int nResultAnimalZodiac = Util::ConvertDBParam(pInfo->szAnimalZodiac, buszAnimalZodiac, sizeof(buszAnimalZodiac), sizeAnimalZodiac);
	int nResultBloodType = Util::ConvertDBParam(pInfo->szBloodType, buszBloodType, sizeof(buszBloodType), sizeBloodType);
	int nResultState = Util::ConvertDBParam(pInfo->szState, buszState, sizeof(buszState), sizeState);
	int nResultCity = Util::ConvertDBParam(pInfo->szCity, buszCity, sizeof(buszCity), sizeCity);
	int nResultConstellation = Util::ConvertDBParam(pInfo->szConstellation, buszConstellation, sizeof(buszConstellation), sizeConstellation);
	int nResultCareer = Util::ConvertDBParam(pInfo->szCareer, buszCareer, sizeof(buszCareer), sizeCareer);

	// MRF_RECHECK: Should have checked nResult first.

	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "INSERT INTO [%s] ([cha_id],[motto],[showmotto],[sex],[age],[name],[animal_zodiac],[blood_type] \
	,[birthday],[state],[city],[constellation],[career],[avatarsize],[prevent],[support],[oppose]) \
	VALUES( %d, '%s',%d, '%s', %d, '%s', '%s', '%s', %d, '%s', '%s', '%s', '%s', %d , %d, %d, %d)",
	_get_table(), cha_id, buszMotto, pInfo->bShowMotto, pInfo->szSex, pInfo->sAge, buszName, buszAnimalZodiac,
	buszBloodType, pInfo->iBirthday, buszState, buszCity, buszConstellation, buszCareer, pInfo->iSize,
	pInfo->bPprevent, pInfo->iSupport, pInfo->iOppose);

	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
	return true;
}

bool TBLPersonInfo::GetInfo(int cha_id, stPersonInfo* pInfo) {
	int l_retrow = 0;
	char* param = "[motto],[showmotto],[sex],[age],[name],[animal_zodiac],[blood_type],[birthday],[state],[city],[constellation],[career],[avatarsize],[prevent],[support],[oppose]";
	char filter[200];
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "cha_id=%d", cha_id);

	if (_get_row(m_buf, 16, param, filter, &l_retrow)) {
		if (l_retrow == 1 && get_affected_rows() == 1) {
			int index = 0;
			strncpy_s(pInfo->szMotto, sizeof(pInfo->szMotto), m_buf[index++].c_str(), _TRUNCATE);

			pInfo->bShowMotto = (atoi(m_buf[index++].c_str()) == 1 ? true : false);

			strncpy_s(pInfo->szSex, sizeof(pInfo->szSex), m_buf[index++].c_str(), _TRUNCATE);

			pInfo->sAge = atoi(m_buf[index++].c_str());

			strncpy_s(pInfo->szName, sizeof(pInfo->szName), m_buf[index++].c_str(), _TRUNCATE);

			strncpy_s(pInfo->szAnimalZodiac, sizeof(pInfo->szAnimalZodiac), m_buf[index++].c_str(), _TRUNCATE);

			strncpy_s(pInfo->szBloodType, sizeof(pInfo->szBloodType), m_buf[index++].c_str(), _TRUNCATE);

			pInfo->iBirthday = atoi(m_buf[index++].c_str());

			strncpy_s(pInfo->szState, sizeof(pInfo->szState), m_buf[index++].c_str(), _TRUNCATE);

			strncpy_s(pInfo->szCity, sizeof(pInfo->szCity), m_buf[index++].c_str(), _TRUNCATE);

			strncpy_s(pInfo->szConstellation, sizeof(pInfo->szConstellation), m_buf[index++].c_str(), _TRUNCATE);

			strncpy_s(pInfo->szCareer, sizeof(pInfo->szCareer), m_buf[index++].c_str(), _TRUNCATE);

			pInfo->iSize = atoi(m_buf[index++].c_str());
			pInfo->bPprevent = (atoi(m_buf[index++].c_str()) == 1 ? true : false);
			pInfo->iSupport = atoi(m_buf[index++].c_str());
			pInfo->iOppose = atoi(m_buf[index++].c_str());

			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}

	return true;
}

bool TBLPersonInfo::DelInfo(int cha_id) {
	char sql[SQL_MAXLEN];

	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "delete %s where (cha_id=%d )", _get_table(), cha_id);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLPersonInfo::Query(stQueryPersonInfo* pQuery, stQueryResoultPersonInfo pResult[], int& num, int& totalpage, int& totalrecord) {

	// Get the number of pages
	char filter[1024];
	int sorttype = 1;
	totalpage = 0;
	totalrecord = 0;

	vector< vector< string> > rowV;

	if (pQuery->nCurPage < 0) {
		pQuery->nCurPage = 1;
	}

	char busChaName[100];
	size_t size;
	memset(busChaName, 0, sizeof(busChaName));

	int nResult = Util::ConvertDBParam(pQuery->sChaName, busChaName, sizeof(busChaName), size);

	// MRF_RECHECK: Should have checked nResult first.

	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "where cha_name = '%s'", busChaName);

	if (get_page_data("VW_Person", "cha_id, cha_name, age, sex, state, city", pQuery->nPageItemNum, pQuery->nCurPage, filter, "cha_id", sorttype, totalpage, totalrecord, rowV)) {
		num = (int)rowV.size();
		int i = 0;
		for (vector< vector< string> >::iterator it = rowV.begin(); it != rowV.end(); it++, i++) {
			// The first field does not
			strncpy_s(pResult[i].sChaName, sizeof(pResult[i].sChaName), (*it)[1].c_str(), _TRUNCATE);

			if ((*it)[2].size() == 0)
				pResult[i].nMinAge = -1;
			else
				pResult[i].nMinAge = atoi((*it)[2].c_str());

			strncpy_s(pResult[i].cSex, sizeof(pResult[i].cSex), (*it)[3].c_str(), _TRUNCATE);

			strncpy_s(pResult[i].szState, sizeof(pResult[i].szState), (*it)[4].c_str(), _TRUNCATE);

			strncpy_s(pResult[i].nCity, sizeof(pResult[i].nCity), (*it)[5].c_str(), _TRUNCATE);
		}

		return true;
	}

	return false;
}

bool TBLPersonAvatar::AddInfo(int cha_id, stPersonInfo* pInfo) {
	char sql[SQL_MAXLEN];

	// The id field of the account_save table is the primary key
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "INSERT INTO [%s] ([cha_id],[avatar]) VALUES( %d, ?)", _get_table(), cha_id);

	SQLRETURN l_ret = Exec_sql(sql, pInfo->pAvatar, pInfo->iSize, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLPersonAvatar::GetInfo(int cha_id, stPersonInfo* pInfo) {
	int l_retrow = 0;
	char* param = "[avatar]";
	char filter[200];
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "cha_id=%d", cha_id);

	if (_get_bin_field(pInfo->pAvatar, pInfo->iSize, param, filter, &l_retrow)) {
		if (l_retrow == 1 && get_affected_rows() == 1) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}

	return true;
}

bool TBLPersonAvatar::DelInfo(int cha_id) {
	char sql[SQL_MAXLEN];

	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "delete %s where (cha_id=%d )", _get_table(), cha_id);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

// TBLFriends
int TBLFriends::GetFriendsCount(long cha_id1, long cha_id2, const char* groupname) {
	int l_retrow = 0;
	char filter[200];
	char* param1 = "count(*) num";
	if (groupname == NULL)
		_snprintf_s(filter, sizeof(filter), _TRUNCATE, "(cha_id1=%d AND cha_id2 =%d)OR(cha_id1=%d AND cha_id2 =%d)", cha_id1, cha_id2, cha_id2, cha_id1);
	else
		_snprintf_s(filter, sizeof(filter), _TRUNCATE, "(cha_id1=%d AND cha_id2 <> -1) AND (relation='%s')", cha_id1, groupname);

	if (_get_row(m_buf, FRD_MAXCOL, param1, filter, &l_retrow) && l_retrow == 1 && get_affected_rows() == 1) {
		return atoi(m_buf[0].c_str());
	}
	else {
		return -1;
	}
}

bool TBLFriends::GroupIsExsit(long cha_id1, long cha_id2, const char * groupname) {
	int l_retrow = 0;
	char filter[200];
	char* param1 = "count(*) num";
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "(cha_id1=%d AND cha_id2 = %d) AND (relation='%s')", cha_id1, cha_id2, groupname);

	if (_get_row(m_buf, FRD_MAXCOL, param1, filter, &l_retrow) && l_retrow == 1 && get_affected_rows() == 1) {
		return (atoi(m_buf[0].c_str()) == 1);
	}
	else {
		return false;
	}
}

bool TBLFriends::MoveGroup(long cha_id1, long cha_id2, const char *oldgroup, const char *newgroup) {
	char sql[SQL_MAXLEN];
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set relation = '%s' where cha_id1 = %d and cha_id2 = %d and relation = '%s'", _get_table(), newgroup, cha_id1, cha_id2, oldgroup);

	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

int TBLFriends::GetGroupCount(long cha_id1) {
	int l_retrow = 0;
	char filter[200];
	char buffer[255];
	memset(buffer, 0, sizeof(buffer));

	char* param1 = "count(*) num";
	_snprintf_s(filter, sizeof(filter), "1=1");
	_tbl_name = "(select distinct friends.relation relation from friends where friends.cha_id1 =%d and friends.cha_id2 = -1) cc";
	_snprintf_s(buffer, sizeof(buffer), _TRUNCATE, _tbl_name.c_str(), cha_id1);
	_tbl_name = buffer;
	if (_get_row(m_buf, FRD_MAXCOL, param1, filter, &l_retrow) && l_retrow == 1 && get_affected_rows() == 1) {
		_tbl_name = "friends";
		return atoi(m_buf[0].c_str());
	}
	else {
		_tbl_name = "friends";
		return -1;
	}
}

unsigned long TBLFriends::GetFriendAddr(long cha_id1, long cha_id2) {
	int l_retrow = 0;
	char filter[200];
	char* param = "character.mem_addr addr";

	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "(friends.cha_id1=%d AND friends.cha_id2 =%d)", cha_id1, cha_id2);
	_tbl_name = "character (nolock) INNER JOIN friends ON character.cha_id = friends.cha_id2";
	if (_get_row(m_buf, FRD_MAXCOL, param, filter, &l_retrow) && l_retrow == 1 && get_affected_rows() == 1) {
		_tbl_name = "friends";
		return atoi(m_buf[0].c_str());
	}
	else {
		_tbl_name = "friends";
		return 0;
	}
}

bool TBLFriends::UpdateGroup(long cha_id1, const char *oldgroup, const char *newgroup) {
	char sql[SQL_MAXLEN];
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set relation ='%s' where cha_id1=%d AND relation = '%s'", _get_table(), newgroup, cha_id1, oldgroup);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLFriends::DeleteGroup(long cha_id, const char *groupname) {
	char sql[SQL_MAXLEN];

	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "delete %s where cha_id1=%d AND cha_id2 = -1 AND relation ='%s' ", _get_table(), cha_id, groupname);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLFriends::AddGroup(long cha_id, const char *groupname) {
	char sql[SQL_MAXLEN];

	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "insert into %s (cha_id1,cha_id2,relation,createtime) values(%d , -1 , '%s', getdate())", _get_table(), cha_id, groupname);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLFriends::AddFriend(long cha_id1, long cha_id2) {
	char sql[SQL_MAXLEN];

	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "insert %s (cha_id1,cha_id2,relation) values(%d,%d,'%s')", _get_table(), cha_id1, cha_id2, Friend::GetStandardGroup());

	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	if (DBOK(l_ret) && get_affected_rows() == 1) {
		_snprintf_s(sql, sizeof(sql), _TRUNCATE, "insert %s (cha_id1,cha_id2,relation) values(%d,%d,'%s')", _get_table(), cha_id2, cha_id1, Friend::GetStandardGroup());

		SQLRETURN l_ret = Exec_sql_direct(sql, this);
		return (DBOK(l_ret)) ? true : false;
	}

	return false;
}

bool TBLFriends::DelFriend(long cha_id1, long cha_id2) {
	char sql[SQL_MAXLEN];


	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "delete %s where (cha_id1=%d AND cha_id2 =%d)OR(cha_id1=%d AND cha_id2 =%d)", _get_table(), cha_id1, cha_id2, cha_id2, cha_id1);

	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

// TBLMaster
int TBLMaster::GetMasterCount(long cha_id) {
	int l_retrow = 0;
	char filter[200];
	char* param1 = "count(*) num";

	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "(cha_id1=%d)", cha_id);
	if (_get_row(m_buf, MASTER_MAXCOL, param1, filter, &l_retrow) && l_retrow == 1 && get_affected_rows() == 1) {
		return atoi(m_buf[0].c_str());
	}
	else {
		return -1;
	}
}

int TBLMaster::GetPrenticeCount(long cha_id) {
	int l_retrow = 0;
	char filter[200];
	char* param1 = "count(*) num";
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "(cha_id2=%d AND finish=0)", cha_id);
	if (_get_row(m_buf, MASTER_MAXCOL, param1, filter, &l_retrow) && l_retrow == 1 && get_affected_rows() == 1) {
		return atoi(m_buf[0].c_str());
	}
	else {
		return -1;
	}
}

int TBLMaster::HasMaster(long cha_id1, long cha_id2) {
	int l_retrow = 0;
	char filter[200];
	char* param1 = "count(*) num";
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "(cha_id1=%d AND cha_id2=%d)", cha_id1, cha_id2);
	if (_get_row(m_buf, MASTER_MAXCOL, param1, filter, &l_retrow) && l_retrow == 1 && get_affected_rows() == 1) {
		return atoi(m_buf[0].c_str());
	}
	else {
		return -1;
	}
}

bool TBLMaster::AddMaster(long cha_id1, long cha_id2) {
	char sql[SQL_MAXLEN];
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "insert %s (cha_id1,cha_id2,finish) values(%d,%d,%d)", _get_table(), cha_id1, cha_id2, 0);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLMaster::DelMaster(long cha_id1, long cha_id2) {
	char sql[SQL_MAXLEN];
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "delete %s where (cha_id1=%d AND cha_id2 =%d)", _get_table(), cha_id1, cha_id2);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLMaster::FinishMaster(long cha_id) {
	char sql[SQL_MAXLEN];
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update %s set finish=1 where cha_id1=%d", _get_table(), cha_id);
	SQLRETURN l_ret = Exec_sql_direct(sql, this);
	return (DBOK(l_ret)) ? true : false;
}

bool TBLMaster::InitMasterRelation(map<uLong, uLong> &mapMasterRelation) {
	static char const query_master_format[SQL_MAXLEN] = "select cha_id1 cha_id1,cha_id2 cha_id2 from %s";

	bool ret = false;
	char sql[SQL_MAXLEN];
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, query_master_format, _get_table());

	// Perform a query operation
	SQLRETURN sqlret;
	SQLHSTMT hstmt = SQL_NULL_HSTMT;
	SQLSMALLINT col_num = 0;
	bool found = true;

	try {
		do {
			sqlret = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &hstmt);
			if ((sqlret != SQL_SUCCESS) && (sqlret != SQL_SUCCESS_WITH_INFO)) {
				handle_err(_hdbc, SQL_HANDLE_DBC, sqlret);
				throw 1;
			}

			sqlret = SQLExecDirect(hstmt, (SQLCHAR *)sql, SQL_NTS);
			if (sqlret != SQL_SUCCESS) {
				handle_err(hstmt, SQL_HANDLE_STMT, sqlret);
				if (sqlret != SQL_SUCCESS_WITH_INFO)
					throw 2;
			}

			sqlret = SQLNumResultCols(hstmt, &col_num);
			col_num = min(col_num, MAX_COL);
			col_num = min(col_num, _max_col);

			// Bind Column
			int i = 0;
			for (i = 0; i < col_num; ++i) {
				SQLBindCol(hstmt, UWORD(i + 1), SQL_C_CHAR, _buf[i], MAX_DATALEN, &_buf_len[i]);
			}

			// Fetch each Row
			for (i = 0; ((sqlret = SQLFetch(hstmt)) == SQL_SUCCESS) || (sqlret == SQL_SUCCESS_WITH_INFO); ++i) {
				if (sqlret != SQL_SUCCESS) handle_err(hstmt, SQL_HANDLE_STMT, sqlret, sql);

				uLong ulPID = atoi((char const *)_buf[0]);
				uLong ulMID = atoi((char const *)_buf[1]);

				mapMasterRelation[ulPID] = ulMID;
			}

			SQLFreeStmt(hstmt, SQL_CLOSE);
			SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
			SQLFreeStmt(hstmt, SQL_UNBIND);
			ret = true;

		} while (0);
	}
	catch (...) {
		LogLine	l_line(g_LogMaster);
		l_line << newln << "Unknown Exception raised when InitMasterRelation()";
	}

	if (hstmt != SQL_NULL_HSTMT) {
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		hstmt = SQL_NULL_HSTMT;
	}

	return ret;
}

bool TBLMaster::GetMasterData(master_dat* farray, int& array_num, unsigned int cha_id) {
	static char const query_master_format[SQL_MAXLEN] =
		"select '' relation,count(*) addr,0 cha_id,'' cha_name,0 icon,'' motto from ( \
		 select distinct master.relation relation from character INNER JOIN \
		 master ON character.cha_id = master.cha_id2 where master.cha_id1 = %d \
		 ) cc union select master.relation relation,count(character.mem_addr) addr,0 \
		 cha_id,'' cha_name,1 icon,'' motto from character INNER JOIN master ON \
		 character.cha_id = master.cha_id2 where master.cha_id1 = %d group by relation \
		 union select master.relation relation,character.mem_addr addr,character.cha_id \
		 cha_id,character.cha_name cha_name,character.icon icon,character.motto motto \
		from character INNER JOIN master ON character.cha_id = master.cha_id2 \
		where master.cha_id1 = %d order by relation,cha_id,icon";

	if (farray == NULL || array_num <= 0 || cha_id == 0)
		return false;

	bool ret = false;
	char sql[SQL_MAXLEN];
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, query_master_format, cha_id, cha_id, cha_id);

	// Perform a query operation
	SQLRETURN sqlret;
	SQLHSTMT hstmt = SQL_NULL_HSTMT;
	SQLSMALLINT col_num = 0;
	bool found = true;

	try {
		do {
			sqlret = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &hstmt);
			if ((sqlret != SQL_SUCCESS) && (sqlret != SQL_SUCCESS_WITH_INFO)) {
				handle_err(_hdbc, SQL_HANDLE_DBC, sqlret);
				throw 1;
			}

			sqlret = SQLExecDirect(hstmt, (SQLCHAR *)sql, SQL_NTS);
			if (sqlret != SQL_SUCCESS) {
				handle_err(hstmt, SQL_HANDLE_STMT, sqlret);
				if (sqlret != SQL_SUCCESS_WITH_INFO)
					throw 2;
			}

			sqlret = SQLNumResultCols(hstmt, &col_num);
			col_num = min(col_num, MAX_COL);
			col_num = min(col_num, _max_col);

			// Bind Column
			int i = 0;
			for (i = 0; i < col_num; ++i) {
				SQLBindCol(hstmt, UWORD(i + 1), SQL_C_CHAR, _buf[i], MAX_DATALEN, &_buf_len[i]);
			}

			// Fetch each Row
			for (i = 0; ((sqlret = SQLFetch(hstmt)) == SQL_SUCCESS) || (sqlret == SQL_SUCCESS_WITH_INFO); ++i) {
				if (i >= array_num) {
					break;
				}

				if (sqlret != SQL_SUCCESS) handle_err(hstmt, SQL_HANDLE_STMT, sqlret, sql);

				farray[i].relation = (char const *)_buf[0];
				farray[i].memaddr = atoi((char const *)_buf[1]);
				farray[i].cha_id = atoi((char const *)_buf[2]);
				farray[i].cha_name = (char const *)_buf[3];
				farray[i].icon_id = atoi((char const *)_buf[4]);
				farray[i].motto = (char const *)_buf[5];
			}

			// The number of rows fetched
			array_num = i;

			SQLFreeStmt(hstmt, SQL_CLOSE);
			SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
			SQLFreeStmt(hstmt, SQL_UNBIND);
			ret = true;

		} while (0);
	}
	catch (...) {
		LogLine	l_line(g_LogMaster);
		l_line << newln << "Unknown Exception raised when GetMasterData()";
	}

	if (hstmt != SQL_NULL_HSTMT) {
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		hstmt = SQL_NULL_HSTMT;
	}

	return ret;
}

bool TBLMaster::GetPrenticeData(master_dat* farray, int& array_num, unsigned int cha_id) {
	static char const query_prentice_format[SQL_MAXLEN] =
		"select '' relation,count(*) addr,0 cha_id,'' cha_name,0 icon,'' motto from ( \
		 select distinct master.relation relation from character INNER JOIN \
		 master ON character.cha_id = master.cha_id1 where master.cha_id2 = %d \
		 ) cc union select master.relation relation,count(character.mem_addr) addr,0 \
		cha_id,'' cha_name,1 icon,'' motto from character INNER JOIN master ON \
		character.cha_id = master.cha_id1 where master.cha_id2 = %d group by relation \
		union select master.relation relation,character.mem_addr addr,character.cha_id \
		cha_id,character.cha_name cha_name,character.icon icon,character.motto motto \
		from character INNER JOIN master ON character.cha_id = master.cha_id1 \
		where master.cha_id2 = %d order by relation,cha_id,icon";

	if (farray == NULL || array_num <= 0 || cha_id == 0) return false;

	bool ret = false;
	char sql[SQL_MAXLEN];
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, query_prentice_format, cha_id, cha_id, cha_id);

	// Perform a query operation
	SQLRETURN sqlret;
	SQLHSTMT hstmt = SQL_NULL_HSTMT;
	SQLSMALLINT col_num = 0;
	bool found = true;

	try {
		do {
			sqlret = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &hstmt);
			if ((sqlret != SQL_SUCCESS) && (sqlret != SQL_SUCCESS_WITH_INFO)) {
				handle_err(_hdbc, SQL_HANDLE_DBC, sqlret);
				throw 1;
			}

			sqlret = SQLExecDirect(hstmt, (SQLCHAR *)sql, SQL_NTS);
			if (sqlret != SQL_SUCCESS) {
				handle_err(hstmt, SQL_HANDLE_STMT, sqlret);
				if (sqlret != SQL_SUCCESS_WITH_INFO)
					throw 2;
			}

			sqlret = SQLNumResultCols(hstmt, &col_num);
			col_num = min(col_num, MAX_COL);
			col_num = min(col_num, _max_col);

			// Bind Column
			int i = 0;
			for (i = 0; i < col_num; ++i) {
				SQLBindCol(hstmt, UWORD(i + 1), SQL_C_CHAR, _buf[i], MAX_DATALEN, &_buf_len[i]);
			}

			// Fetch each Row
			for (i = 0; ((sqlret = SQLFetch(hstmt)) == SQL_SUCCESS) || (sqlret == SQL_SUCCESS_WITH_INFO); ++i) {
				if (i >= array_num) {
					break;
				}

				if (sqlret != SQL_SUCCESS)
					handle_err(hstmt, SQL_HANDLE_STMT, sqlret, sql);

				farray[i].relation = (char const *)_buf[0];
				farray[i].memaddr = atoi((char const *)_buf[1]);
				farray[i].cha_id = atoi((char const *)_buf[2]);
				farray[i].cha_name = (char const *)_buf[3];
				farray[i].icon_id = atoi((char const *)_buf[4]);
				farray[i].motto = (char const *)_buf[5];
			}

			// The number of rows fetched
			array_num = i;

			SQLFreeStmt(hstmt, SQL_CLOSE);
			SQLFreeStmt(hstmt, SQL_RESET_PARAMS);
			SQLFreeStmt(hstmt, SQL_UNBIND);
			ret = true;

		} while (0);
	}
	catch (...) {
		LogLine	l_line(g_LogMaster);
		l_line << newln << "Unknown Exception raised when GetPrenticeData()";
	}

	if (hstmt != SQL_NULL_HSTMT) {
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		hstmt = SQL_NULL_HSTMT;
	}

	return ret;
}

// TBLGuilds
bool TBLGuilds::IsReady() {
	int l_retrow = 0;
	char* param = "count(*)";
	if (_get_row(m_buf, 1, param, 0, &l_retrow)) {
		if (l_retrow == 1 && get_affected_rows() == 1 && atoi(m_buf[0].c_str()) == 199) {
			return true;
		}
	}
	return false;
}

// FEATURE : Guild Stats
int TBLGuilds::GetGuildStatPoint(int guildid) {
	string buf[3];
	char filter[80];
	char*param = "money";
	sprintf(filter, "guild_id =%d", guildid);
	int	 l_retrow = 0;
	bool l_ret = _get_row(buf, 1, param, filter, &l_retrow);
	if (l_retrow == 1)
	{
		return atoi(buf[0].c_str());
	}
	return 0;
}
bool TBLGuilds::SaveGuildColor(int guildID, int color) {
	char buff[255];
	sprintf(buff, "update guild set color = %d where guild_id = %d", color, guildID);
	SQLRETURN sqlret;
	sqlret = _db->exec_sql_direct(buff);
	if (sqlret != SQL_SUCCESS)
	{
		LG("guildAttrError", "Save guildAttr Error SQL = %s", buff);
		return false;
	}
	return true;
}


int TBLGuilds::GetGuildColor(int guildid) {
	string buf[1];
	char filter[80];
	char*param = "color";
	sprintf(filter, "guild_id =%d", guildid);
	int	 l_retrow = 0;
	bool l_ret = _get_row(buf, 1, param, filter, &l_retrow);
	if (l_retrow == 1)
	{
		return atoi(buf[0].c_str());
	}
	return 0;
}

bool TBLGuilds::SaveGuildPoints(int guildID, int money, int lv, int exp) {
	char buff[255];
	sprintf(buff, "update guild set money = %d, exp = %d, level = %d where guild_id = %d", money, exp, lv, guildID);
	SQLRETURN sqlret;
	sqlret = _db->exec_sql_direct(buff);
	if (sqlret != SQL_SUCCESS)
	{
		LG("guildAttrError", "Save guildAttr Error SQL = %s", buff);
		return false;
	}
	return true;
}

int TBLGuilds::GetGuildLevel(int guildid)
{
	string buf[3];
	char filter[80];
	char*param = "level";
	_snprintf_s(filter, _countof(filter), _TRUNCATE, "guild_id=%d", guildid);
	int	 l_retrow = 0;
	bool l_ret = _get_row(buf, 1, param, filter, &l_retrow);
	if (l_retrow == 1)
	{
		return atoi(buf[0].c_str());
	}
	return 0;
}

bool TBLGuilds::IncrementGuildAttr(int guildid, int attr, int upgCost) {
	char buff[255];
	char attrCol[32];
	if (!getGuildAttrCol(attr, attrCol)) {
		return false;
	}
	sprintf(buff, "update guild set %s = %s + 1, money = money - %d where guild_id = %d and money>=%d", attrCol, attrCol, upgCost, guildid, upgCost);
	SQLRETURN sqlret;
	sqlret = _db->exec_sql_direct(buff);
	if (sqlret != SQL_SUCCESS)
	{
		LG("guildAttrError", "Save guildAttr Error SQL = %s", buff);
		return false;
	}
	return true;
}

int TBLGuilds::GetGuildAttr(int guildid, int attr) {
	string buf[3];
	char attrCol[32];
	if (!getGuildAttrCol(attr, attrCol)) {
		printf("Error retriving column");
		return 0;
	}
	char filter[80];
	sprintf(filter, "guild_id =%d", guildid);
	int	 l_retrow = 0;
	bool l_ret = _get_row(buf, 1, attrCol, filter, &l_retrow);
	if (l_retrow == 1)
	{
		return atoi(buf[0].c_str());
	}
	return 0;
}

int TBLGuilds::FetchRowByName(const char *guild_name) {
	int l_retrow = 0;
	char* param = "guild_id";
	char filter[200];
	char buff[66];
	size_t size;
	memset(buff, 0, sizeof(buff));

	int nResult = Util::ConvertDBParam(guild_name, buff, sizeof(buff), size);
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, "guild_name='%s'", buff);

	if (_get_row(m_buf, 1, param, filter, &l_retrow)) {
		if (l_retrow == 1 && get_affected_rows() == 1) {
			return l_retrow;
		}
		else {
			return 0;
		}
	}
	else {
		return -1;
	}
}

bool TBLGuilds::Disband(uLong gldid) {
	char sql[SQL_MAXLEN];
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update guild set motto ='',passwd ='',leader_id =0,stat =0,money =0,exp =0,member_total =0,try_total =0\
											  where guild_id =%d", gldid);

	SQLRETURN l_sqlret = Exec_sql_direct(sql, this);
	if (!DBOK(l_sqlret)) {
		if (DBNODATA(l_sqlret)) {
			LogLine	l_line(g_LogGuild);
			l_line << newln << "dismiss guild SQL failed2! guild ID:" << gldid;
			return false;
		}
		else {
			LogLine	l_line(g_LogGuild);
			l_line << newln << "dismiss guild SQL failed1! guild ID:" << gldid;

			// Ordinary SQL error
			return false;
		}
	}

	_snprintf_s(sql, sizeof(sql), _TRUNCATE, "update character set guild_id =0 ,guild_stat =0,guild_permission =0\
											  where guild_id =%d", gldid);

	l_sqlret = Exec_sql_direct(sql, this);
	if (!DBOK(l_sqlret)) {
		LogLine	l_line(g_LogGuild);
		l_line << newln << "dismiss guild SQL failed3! guild ID:" << gldid;

		// Ordinary SQL error
		return false;
	}

	return true;
}

int TBLGuilds::GetRowValue(int guildID, char* gParameter)
{
	string buf[3];
	char filter[80];
	char*param = "level";
	_snprintf_s(filter, _countof(filter), _TRUNCATE, "guild_id=%d", guildID);
	int	 l_retrow = 0;
	bool l_ret = _get_row(buf, 1, param, filter, &l_retrow);
	if (l_retrow == 1)
	{
		//printf(buf[0].c_str());
		return atoi(buf[0].c_str());
	}
	return 0;
	//char tmp[SQL_MAXLEN] = { 0 };
	//_snprintf_s(tmp, _countof(tmp), _TRUNCATE, "SELECT g.money, g.exp, g.level FROM guild As g WHERE (g.guild_id=%d)", guildID);
	//bool l_ret = false;
	//SQLRETURN sqlret;
	//SQLHSTMT hstmt = SQL_NULL_HSTMT;
	//SQLSMALLINT col_num = 0;
	//bool found = true;

}

bool TBLGuilds::InitAllGuilds(char disband_days) {
	string sql_syntax = "";
	if (disband_days < 1) {
		return false;
	}
	else {
		sql_syntax = string("	select g.guild_id, g.guild_name, g.motto, g.leader_id,g.type,g.stat,\
						g.money, g.exp, g.member_total, g.level, g.try_total,g.disband_date,\
						case when g.stat>0 then DATEDIFF(mi,g.disband_date,GETDATE()) else 0 end TotalMins ")
			+ string(", case when g.stat>0 then %d*24*60 -DATEDIFF(mi,g.disband_date,GETDATE()) else 0 end LeaveMins ")
			+ string(" from guild As g where (g.guild_id >0) ");

	}

	bool l_ret = false;
	char sql[SQL_MAXLEN];
	_snprintf_s(sql, sizeof(sql), _TRUNCATE, sql_syntax.c_str(), disband_days);

	// Perform a query operation
	SQLRETURN sqlret;
	SQLHSTMT hstmt = SQL_NULL_HSTMT;
	SQLSMALLINT col_num = 0;
	bool found = true;

	try {
		sqlret = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &hstmt);
		if ((sqlret != SQL_SUCCESS) && (sqlret != SQL_SUCCESS_WITH_INFO)) {
			handle_err(_hdbc, SQL_HANDLE_DBC, sqlret);
			throw 1;
		}

		sqlret = SQLExecDirect(hstmt, (SQLCHAR *)sql, SQL_NTS);
		if (sqlret != SQL_SUCCESS) {
			handle_err(hstmt, SQL_HANDLE_STMT, sqlret);
			if (sqlret != SQL_SUCCESS_WITH_INFO)
				throw 2;
		}

		sqlret = SQLNumResultCols(hstmt, &col_num);
		col_num = min(col_num, MAX_COL);
		col_num = min(col_num, _max_col);

		// Bind Column
		for (int i = 0; i < col_num; ++i)
		{
			SQLBindCol(hstmt, UWORD(i + 1), SQL_C_CHAR, _buf[i], MAX_DATALEN, &_buf_len[i]);
		}

		// Fetch each Row
		for (int f_row = 1; (sqlret = SQLFetch(hstmt)) == SQL_SUCCESS || sqlret == SQL_SUCCESS_WITH_INFO; ++f_row) {
			if (sqlret != SQL_SUCCESS) {
				handle_err(hstmt, SQL_HANDLE_STMT, sqlret);
			}

			Guild *l_gld = Guild::Alloc();

			l_gld->m_id = atol((cChar *)_buf[0]);

			strncpy_s(l_gld->m_name, sizeof(l_gld->m_name), (cChar *)_buf[1], _TRUNCATE);

			strncpy_s(l_gld->m_motto, sizeof(l_gld->m_motto), (cChar *)_buf[2], _TRUNCATE);


			l_gld->m_leaderID = atol((cChar*)_buf[3]);
			l_gld->m_type = atoi((cChar *)_buf[4]);
			l_gld->m_stat = atoi((cChar *)_buf[5]);
			// The guild dissolved the remaining minutes
			l_gld->m_remain_minute = atol((cChar *)_buf[12]);
			l_gld->m_tick = GetTickCount();

			l_gld->m_point = atoi((cChar *)_buf[6]);	//guild money
			l_gld->m_exp = atoi((cChar *)_buf[7]);		//guild exp
			l_gld->m_level = atoi((cChar *)_buf[9]);		//guild level

		//	if(l_gld->m_id == 102)
		//		printf("\n -------------------------------------------[GUILD : %s] -------------------------------------------\n Guild Point is : %d \n Exp is : %d \n Level is : %d \n--------------------------------------------------------------------------------------", l_gld->m_name, l_gld->m_point, l_gld->m_exp, l_gld->m_level);
			
			l_gld->BeginRun();
		}

		SQLFreeStmt(hstmt, SQL_UNBIND);
		l_ret = true;
	}
	catch (int& e) {
		LogLine	l_line(g_LogGuild);
		l_line << newln << "init guild ODBC interface failed, InitAllGuilds() error:" << e;
	}
	catch (...) {
		LogLine	l_line(g_LogGuild);
		l_line << newln << "Unknown Exception raised when InitAllGuilds()";
	}

	if (hstmt != SQL_NULL_HSTMT) {
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		hstmt = SQL_NULL_HSTMT;
	}

	return l_ret;
}

bool TBLGuilds::SendGuildInfo(Player *ply) {
	WPacket l_togmSelf = g_gpsvr->GetWPacket();
	l_togmSelf.WriteCmd(CMD_PM_GUILDINFO);

	// Role DBID
	l_togmSelf.WriteLong(ply->m_chaid[ply->m_currcha]);

	// Guild Type
	l_togmSelf.WriteChar(ply->GetGuild()->m_type);

	// Guild ID
	l_togmSelf.WriteLong(ply->m_guild[ply->m_currcha]);

	// Leader ID
	l_togmSelf.WriteLong(ply->GetGuild()->m_leaderID);

	// Guild name
	l_togmSelf.WriteString(ply->GetGuild()->m_name);

	// Guild Motto
	l_togmSelf.WriteString(ply->GetGuild()->m_motto);

	ply->m_gate->GetDataSock()->SendData(l_togmSelf);

	return true;
}

bool TBLGuilds::InitGuildMember(Player *ply, uLong chaid, uLong gldid, int mode) {
	bool l_ret = false;
	if (ply && gldid == 0) {
		WPacket l_toSelf = g_gpsvr->GetWPacket();
		l_toSelf.WriteCmd(CMD_PC_GUILD);
		l_toSelf.WriteChar(MSG_GUILD_START);

		l_toSelf.WriteLong(0);
		l_toSelf.WriteChar(0);

		g_gpsvr->SendToClient(ply, l_toSelf);
	}
	else {
		const char *sql_syntax = 0;
		char sql[SQL_MAXLEN];
		sql_syntax =
			"	select c.mem_addr,c.cha_id, c.cha_name, c.motto, c.job, c.degree, c.icon, c.guild_permission\
				from character As c\
				where (c.guild_stat =0) and (c.guild_id =%d) and (c.delflag = 0)\
				";

		_snprintf_s(sql, sizeof(sql), _TRUNCATE, sql_syntax, gldid);

		// Perform a query operation
		SQLRETURN sqlret;
		SQLHSTMT hstmt = SQL_NULL_HSTMT;
		SQLSMALLINT col_num = 0;
		bool found = true;

		try {
			sqlret = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &hstmt);
			if ((sqlret != SQL_SUCCESS) && (sqlret != SQL_SUCCESS_WITH_INFO)) {
				handle_err(_hdbc, SQL_HANDLE_DBC, sqlret);
				throw 1;
			}

			sqlret = SQLExecDirect(hstmt, (SQLCHAR *)sql, SQL_NTS);
			if (sqlret != SQL_SUCCESS) {
				handle_err(hstmt, SQL_HANDLE_STMT, sqlret);
				if (sqlret != SQL_SUCCESS_WITH_INFO)
					throw 2;
			}

			sqlret = SQLNumResultCols(hstmt, &col_num);
			col_num = min(col_num, MAX_COL);
			col_num = min(col_num, _max_col);

			// Bind Column
			for (int i = 0; i < col_num; ++i) {
				SQLBindCol(hstmt, UWORD(i + 1), SQL_C_CHAR, _buf[i], MAX_DATALEN, &_buf_len[i]);
			}

			WPacket l_toGuild = g_gpsvr->GetWPacket();
			l_toGuild.WriteCmd(CMD_PC_GUILD);
			if (mode) {
				l_toGuild.WriteChar(MSG_GUILD_ADD);
			}
			else {
				l_toGuild.WriteChar(MSG_GUILD_ONLINE);
				l_toGuild.WriteLong(chaid);
			}

			WPacket l_toSelf, l_wpk0;
			if (ply) {
				l_wpk0 = g_gpsvr->GetWPacket();
				l_wpk0.WriteCmd(CMD_PC_GUILD);
				l_wpk0.WriteChar(MSG_GUILD_START);
			}

			bool l_hrd = false;
			Player *l_plylst[10240];
			short l_plynum = 0;

			long lPacketNum = 0;

			// Fetch each Row
			int f_row = 1;
			for (f_row = 1; (sqlret = SQLFetch(hstmt)) == SQL_SUCCESS || sqlret == SQL_SUCCESS_WITH_INFO; ++f_row) {
				if (sqlret != SQL_SUCCESS) {
					handle_err(hstmt, SQL_HANDLE_STMT, sqlret);
				}

				if (ply && (f_row % 20) == 1) {
					l_toSelf = l_wpk0;
				}

				if (ply && !l_hrd) {
					l_hrd = true;
					l_toSelf.WriteLong(ply->m_guild[ply->m_currcha]);
					l_toSelf.WriteString(ply->GetGuild()->m_name);
					l_toSelf.WriteLong(ply->GetGuild()->m_leaderID);
				}

				uLong l_memaddr = atol((cChar *)_buf[0]);
				if (l_memaddr) {
					l_plylst[l_plynum] = (Player*)MakePointer(l_memaddr);
					l_plynum++;
				}

				if (mode && chaid == atol((cChar*)_buf[1])) {
					l_toGuild.WriteChar(l_memaddr ? 1 : 0);								// Online
					l_toGuild.WriteLong(atol((cChar*)_buf[1]));							// ChaID
					l_toGuild.WriteString((cChar*)_buf[2]);								// ChaName
					l_toGuild.WriteString((cChar*)_buf[3]);								// Motto
					l_toGuild.WriteString(g_GetJobName(atoi((const char*)_buf[4])));	// Job
					l_toGuild.WriteShort(atoi((cChar*)_buf[5]));						// Degree
					l_toGuild.WriteShort(atoi((cChar*)_buf[6]));						// Icon
					l_toGuild.WriteShort(atoi((cChar*)_buf[7]));						// Permission
				}

				if (ply) {
					l_toSelf.WriteChar(l_memaddr ? 1 : 0);								// Online
					l_toSelf.WriteLong(atol((cChar*)_buf[1]));							// ChaID
					l_toSelf.WriteString((cChar*)_buf[2]);								// ChaName
					l_toSelf.WriteString((cChar*)_buf[3]);								// Motto
					l_toSelf.WriteString(g_GetJobName(atoi((const char*)_buf[4])));		// Job
					l_toSelf.WriteShort(atoi((cChar*)_buf[5]));							// Degree
					l_toSelf.WriteShort(atoi((cChar*)_buf[6]));							// Icon
					l_toSelf.WriteShort(atoi((cChar*)_buf[7]));							// Permission
				}

				if (ply && !(f_row % 20)) {
					l_toSelf.WriteLong(lPacketNum);
					lPacketNum++;

					// This includes the number of articles
					l_toSelf.WriteChar(((f_row - 1) % 20) + 1);
					g_gpsvr->SendToClient(ply, l_toSelf);
				}
			}

			if (ply && (f_row % 20) == 1) {
				l_toSelf = l_wpk0;
			}

			if (ply && !l_hrd) {
				l_hrd = true;
				l_toSelf.WriteLong(ply->m_guild[ply->m_currcha]);	// Guild ID
				l_toSelf.WriteString(ply->GetGuild()->m_name);		// Guild Name
				l_toSelf.WriteLong(ply->GetGuild()->m_leaderID);	// Leader ID
			}

			if (ply) {
				l_toSelf.WriteLong(lPacketNum);
				lPacketNum++;
				l_toSelf.WriteChar((f_row - 1) % 20);
				g_gpsvr->SendToClient(ply, l_toSelf);
			}

			LogLine	l_line(g_LogGuild);
			l_line << newln << "online guild num:" << l_plynum << endln;
			g_gpsvr->SendToClient(l_plylst, l_plynum, l_toGuild);

			SQLFreeStmt(hstmt, SQL_UNBIND);
			l_ret = true;
		}
		catch (int& e) {
			LogLine	l_line(g_LogGuild);
			l_line << newln << "init guild ODBC interface failed, InitGuildMember() error:" << e;
			l_line << newln << sql;
		}
		catch (...) {
			LogLine	l_line(g_LogGuild);
			l_line << newln << "Unknown Exception raised when InitGuildMember()";
		}

		if (hstmt != SQL_NULL_HSTMT) {
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			hstmt = SQL_NULL_HSTMT;
		}
	}

	return l_ret;
}

// Find the number of guild members
bool TBLGuilds::GetGuildMemberNum(uLong gldid, InterLockedLong& memberNum) {
	string buf[1];
	char filter[180];
	char* param = "member_total";
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, " guild_id = %d ", gldid);

	int l_retrow = 0;
	bool l_ret = false;
	l_ret = _get_row(m_buf, 1, param, filter, &l_retrow);

	if (l_ret&&l_retrow > 0) {
		memberNum = atoi(m_buf[0].c_str());
		return true;
	}

	return false;
}

bool TBLParam::InitParam(void) {
	string strSQL = "select param1,param2,param3,param4,param5,param6,param7,param8,param9,param10 from param where id = 1";
	SQLRETURN sqlret;
	SQLHSTMT hstmt = SQL_NULL_HSTMT;
	SQLINTEGER buf_len[MAXORDERNUM + MAXORDERNUM];
	bool found = true;
	try {
		sqlret = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &hstmt);
		if ((sqlret != SQL_SUCCESS) && (sqlret != SQL_SUCCESS_WITH_INFO)) {
			handle_err(_hdbc, SQL_HANDLE_DBC, sqlret);
			throw 1;
		}

		sqlret = SQLSetStmtAttr(hstmt, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)1, 0);
		if ((sqlret != SQL_SUCCESS) && (sqlret != SQL_SUCCESS_WITH_INFO)) {
			handle_err(_hdbc, SQL_HANDLE_DBC, sqlret);
			throw 1;
		}

		SQLBindCol(hstmt, 1, SQL_C_LONG, &m_nOrder[0].nid, 0, &(buf_len[0]));
		SQLBindCol(hstmt, 2, SQL_C_LONG, &m_nOrder[1].nid, 0, &buf_len[1]);
		SQLBindCol(hstmt, 3, SQL_C_LONG, &m_nOrder[2].nid, 0, &buf_len[2]);
		SQLBindCol(hstmt, 4, SQL_C_LONG, &m_nOrder[3].nid, 0, &buf_len[3]);
		SQLBindCol(hstmt, 5, SQL_C_LONG, &m_nOrder[4].nid, 0, &buf_len[4]);
		SQLBindCol(hstmt, 6, SQL_C_LONG, &m_nOrder[0].nfightpoint, 0, &buf_len[5]);
		SQLBindCol(hstmt, 7, SQL_C_LONG, &m_nOrder[1].nfightpoint, 0, &buf_len[6]);
		SQLBindCol(hstmt, 8, SQL_C_LONG, &m_nOrder[2].nfightpoint, 0, &buf_len[7]);
		SQLBindCol(hstmt, 9, SQL_C_LONG, &m_nOrder[3].nfightpoint, 0, &buf_len[8]);
		SQLBindCol(hstmt, 10, SQL_C_LONG, &m_nOrder[4].nfightpoint, 0, &buf_len[9]);
		sqlret = SQLExecDirect(hstmt, (SQLCHAR *)const_cast<char*>(strSQL.c_str()), SQL_NTS);
		if (sqlret != SQL_SUCCESS) {
			handle_err(hstmt, SQL_HANDLE_STMT, sqlret);
			if (sqlret != SQL_SUCCESS_WITH_INFO)
				throw 2;
		}

		sqlret = SQLFetch(hstmt);
		if (sqlret != SQL_SUCCESS) {
			if (sqlret != SQL_NO_DATA)
				handle_err(hstmt, SQL_HANDLE_STMT, sqlret);
		}
		SQLFreeStmt(hstmt, SQL_CLOSE);
		SQLFreeStmt(hstmt, SQL_UNBIND);
	}
	catch (int& e) {
		LogLine	l_line(g_LogGarner2);
		l_line << newln << "init guild ODBC interface failed, InitParam() error:" << e;
	}
	catch (...) {
		LogLine	l_line(g_LogGarner2);
		l_line << newln << "Unknown Exception raised when InitParam()";
	}

	char buff[255];
	int nlev;
	try {
		sqlret = SQLAllocHandle(SQL_HANDLE_STMT, _hdbc, &hstmt);
		if ((sqlret != SQL_SUCCESS) && (sqlret != SQL_SUCCESS_WITH_INFO)) {
			handle_err(_hdbc, SQL_HANDLE_DBC, sqlret);
			throw 1;
		}
		SQLBindCol(hstmt, 1, SQL_C_CHAR, _buf[0], MAX_DATALEN, &buf_len[0]);
		SQLBindCol(hstmt, 2, SQL_C_CHAR, _buf[1], MAX_DATALEN, &buf_len[1]);
		SQLBindCol(hstmt, 3, SQL_C_ULONG, &nlev, 0, &buf_len[2]);
		for (int n = 0; n < MAXORDERNUM; n++) {
			_snprintf_s(buff, sizeof(buff), _TRUNCATE, "select cha_name,job,degree from character where cha_id = %d ", m_nOrder[n].nid);
			sqlret = SQLExecDirect(hstmt, (SQLCHAR *)buff, SQL_NTS);
			if (sqlret != SQL_SUCCESS) {
				handle_err(hstmt, SQL_HANDLE_STMT, sqlret);
				if (sqlret != SQL_SUCCESS_WITH_INFO)
					throw 2;
			}
			int i = 0;
			if ((sqlret = SQLFetch(hstmt)) != SQL_NO_DATA) {
				if (sqlret == SQL_NO_DATA) {
					LogLine	l_line(g_LogGarner2);
					l_line << newln << "cha name query failed .cha ID£º" << m_nOrder[n].nid;
					continue;
				}

				if (sqlret != SQL_SUCCESS) {
					handle_err(hstmt, SQL_HANDLE_STMT, sqlret);
				}

				if (buf_len[0] > 20) {
					LogLine	l_line(g_LogGarner2);
					l_line << newln << "cha name query failed.";
					return false;
				}

				memcpy(m_nOrder[n].strname, _buf[0], buf_len[0]);
				m_nOrder[n].strname[buf_len[0]] = '\0';

				memcpy(m_nOrder[n].strjob, g_GetJobName(atoi((const char*)_buf[1])), buf_len[1]);
				m_nOrder[n].strjob[buf_len[1]] = '\0';

				m_nOrder[n].nlev = nlev;
			}

			SQLFreeStmt(hstmt, SQL_CLOSE);
		}

		SQLFreeStmt(hstmt, SQL_UNBIND);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	}
	catch (int& e) {
		LogLine	l_line(g_LogGarner2);
		l_line << newln << "init guild ODBC interface failed, InitParam() erro :" << e;
	}
	catch (...) {
		LogLine	l_line(g_LogGarner2);
		l_line << newln << "Unknown Exception raised when InitParam()";
	}

	return true;
}

bool TBLParam::SaveParam(void) {
	char buff[255];
	_snprintf_s(buff, sizeof(buff), _TRUNCATE, "update %s set param1 = %d,param2 = %d,param3 = %d,param4 = %d,param5 = %d,param6 = %d,param7 = %d,param8 = %d,param9 = %d,param10 = %d where id = 1", 
	_get_table(), m_nOrder[0].nid, m_nOrder[1].nid, m_nOrder[2].nid, m_nOrder[3].nid, m_nOrder[4].nid, m_nOrder[0].nfightpoint, m_nOrder[1].nfightpoint, m_nOrder[2].nfightpoint, m_nOrder[3].nfightpoint, 
	m_nOrder[4].nfightpoint);

	SQLRETURN sqlret;
	sqlret = _db->exec_sql_direct(buff);
	if (sqlret != SQL_SUCCESS) {
		LG("ParamErr", "Save Param Error SQL = %s", buff);
	}

	return true;
}

bool TBLParam::IsReady() {
	int l_retrow = 0;
	char* param = "count(*)";
	if (_get_row(m_buf, 1, param, 0, &l_retrow)) {
		if (l_retrow == 1 && get_affected_rows() == 1 && atoi(m_buf[0].c_str()) >= 199) {
			return true;
		}
	}

	return false;
}

void TBLParam::UpdateOrder(ORDERINFO &Order) {
	ORDERINFO ordertemp[MAXORDERNUM];
	memcpy(ordertemp, m_nOrder, sizeof(ORDERINFO)*MAXORDERNUM);

	int i = 0;
	int oldid = 0;
	for (i = 0; i < MAXORDERNUM; i++) {
		if (ordertemp[i].nfightpoint >= Order.nfightpoint) {
			if (ordertemp[i].nid == Order.nid)
				break;

			continue;
		}
		else {
			oldid = i;
			if (ordertemp[i].nid == Order.nid) {
				m_nOrder[i].nfightpoint = Order.nfightpoint;
				break;
			}
			memcpy(&m_nOrder[i++], &Order, sizeof(ORDERINFO));

			int n = -1;
			for (int a = i; a < MAXORDERNUM;) {
				if (ordertemp[a + n].nid == Order.nid) {
					n++;
					continue;
				}

				if (a + n < MAXORDERNUM)
					memcpy(&m_nOrder[a], &ordertemp[a + n], sizeof(ORDERINFO));
				else {
					strncpy_s(m_nOrder[a].strjob, sizeof(m_nOrder[a].strjob), "", _TRUNCATE);
					strncpy_s(m_nOrder[a].strname, sizeof(m_nOrder[a].strname), "", _TRUNCATE);
					m_nOrder[a].nid = -1;
					m_nOrder[a].nlev = 0;
					m_nOrder[a].nfightpoint = 0;
				}
				a++;
			}

			SaveParam();
			WPacket l_wpk = g_gpsvr->GetWPacket();
			l_wpk.WriteCmd(CMD_PM_GARNER2_UPDATE);
			for (i = 0; i < MAXORDERNUM; i++) {
				l_wpk.WriteLong(m_nOrder[i].nid);
			}

			l_wpk.WriteLong(oldid);
			l_wpk.WriteLong(0);
			for (int j = 0; j < GroupServerApp::GATE_MAX; j++) {
				if (g_gpsvr->m_gate[j].GetDataSock()) {
					g_gpsvr->m_gate[j].GetDataSock()->SendData(l_wpk);
					break;
				}
			}

			LogLine	l_line(g_LogGarner2);
			l_line << newln << "order chaned";
			break;
		}
	}
}

#ifdef SHUI_JING
bool TBLCrystalTrade::IsHasBuyorSale(long cha_id, int type) {
	string buf[1];
	char filter[180];
	char* param = "ishang";

	int isHang = 0;
	_snprintf_s(filter, sizeof(filter), _TRUNCATE, " cha_id = %d and tradetype = %d ", cha_id, type);

	int l_retrow = 0;
	bool l_ret = false;
	l_ret = _get_row(m_buf, 1, param, filter, &l_retrow);

	if (l_ret&&l_retrow > 0) {
		isHang = atoi(m_buf[0].c_str());
		if (isHang == 1)
			return true;
	}

	return false;
}
#endif
