#include <iostream>
#include "IniFile.h"
#include "GroupServerApp.h"
#include "GameCommon.h"
#include "Team.h"
#include "Script.h"
#include "Parser.h"

_DBC_USING

void InitDBSvrConnect(GroupServerApp &gpapp);

// AccountServer Connection
void InitACTSvrConnect(GroupServerApp &gpapp);


void GroupServerApp::Initialize() {
	
	// Sleep(20000);
	// HWND hWin = ::GetConsoleWindow();
	// DWORD prcId = 0;
	// GetWindowThreadProcessId(hWin, &prcId);
	// HANDLE hStd = GetStdHandle(STD_OUTPUT_HANDLE);
	// SetConsoleTextAttribute(hStd, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	m_dwCheatCount = 0;
	LogLine l_line(g_LogConnect);
	l_line << newln << "begin connect database" << endln;
	InitDBSvrConnect(*this);
	m_tblguilds->InitAllGuilds(7);
	dstring	l_acct = m_cfg["AccountServer"]["AcctEnable"];
	if (l_acct != "0") {
		l_line << newln << "begin init AccountServer connect" << endln;
		InitACTSvrConnect(*this);
	}

	// std::string strConsoleTitle;
	// strConsoleTitle += "GroupServer";
	// SetConsoleTitle(strConsoleTitle.c_str());

	LG("init", "init Lua Script...\n");
	if (!InitLuaScript()) {
		THROW_EXCP(excp, "init LUA script failed!");
	}

	if (!PrivilegeCheck::Instance()->Init("GMCmd.txt")) {
		l_line << newln << "init GMCmd.txt failed!" << endln;
		THROW_EXCP(excp, "init GMCmd.txt failed!");
	}

	// Finally, open its own port to listen for connections from the GateServer / GameServer:
	l_line << newln << "begin read GroupServer.cfg" << endln;
	IniSection &l_is = m_cfg["Main"];
	cChar *l_ip = l_is["Listen_IP"];
	uShort l_port = atoi(l_is["Listen_Port"]);

	const_cha.MaxChaNum = atoi(m_cfg["character"]["MaxCharacterNum"]);
	const_cha.MaxChaNum = min(const_cha.MaxChaNum, Player::emMaxCharacters);
	const_cha.MaxIconVal = atoi(m_cfg["character"]["MaxValue"]);
	const_cha.MaxLoginUsr = atoi(m_cfg["character"]["MaxPerson"]);

	const_frnd.InvitedMax = atoi(m_cfg["friend"]["MaxInvitedNum"]);
	const_frnd.PendTimeOut = atoi(m_cfg["friend"]["InvitedTime"]) * 1000;
	const_frnd.FriendMax = atoi(m_cfg["friend"]["MaxFriendNum"]);
	const_frnd.FriendGroupMax = atoi(m_cfg["friend"]["FriendGroupNum"]);

	const_team.InvitedMax = atoi(m_cfg["team"]["MaxInvitedNum"]);
	const_team.PendTimeOut = atoi(m_cfg["team"]["InvitedTime"]) * 1000;
	const_team.MemberMax = atoi(m_cfg["team"]["MaxPersonNum"]);

	const_master.InvitedMax = 5;
	const_master.PendTimeOut = 30000;
	const_master.MasterMax = 1;
	const_master.PrenticeMax = 4;

	const_guild.InvitedMax = 5;
	const_guild.PendTimeOut = 30000;
	const_guild.GuildMemberMax = 80;

	const_chat.MaxSession = atoi(m_cfg["chat"]["MaxChatNumPerPlayer"]);
	const_chat.MaxSession = min(const_chat.MaxSession, Player::emMaxSessChat);
	const_chat.MaxPlayer = atoi(m_cfg["chat"]["maxPlayerPerChat"]);
	const_chat.MaxPlayer = min(const_chat.MaxPlayer, Player::emMaxSessPlayer);

	const_interval.World = atoi(m_cfg["chat"]["WorldInterval"]) * 1000;
	const_interval.Trade = atoi(m_cfg["chat"]["TradeInterval"]) * 1000;
	const_interval.ToYou = atoi(m_cfg["chat"]["ToYouInterval"]) * 1000;


	if (!InitMasterRelation()) {
		THROW_EXCP(excp, RES_STRING(GP_GROUPSERVERAPPINIT_CPP_00001));
	}

	l_line << newln << "begin load cha_name_filter.txt" << endln;
	CTextFilter::LoadFile("cha_name_filter.txt");
	l_line << newln << "begin listen port:" << l_port << endln;
	if (OpenListenSocket(l_port, l_ip)) {
		char l_content[500];
		CFormatParameter param(3);
		param.setString(0, l_ip);
		param.setLong(1, l_port);
		param.setLong(2, l_port);
		RES_FORMAT_STRING(GP_GROUPSERVERAPPINIT_CPP_00002, param, l_content);
		THROW_EXCP(excpSock, l_content);
	}
	l_line << newln << "open listen port:" << l_port << "success, init is ok" << endln;
}

void InitDBSvrConnect(GroupServerApp &gpapp) {
	LG("group_sql", RES_STRING(GP_GROUPSERVERAPPINIT_CPP_00030));
	string	l_errinfo;

	cChar *l_ip = gpapp.m_cfg["Database"]["IP"];
	cChar *l_db = gpapp.m_cfg["Database"]["DB"];
	cChar *l_login = gpapp.m_cfg["Database"]["Login"];
	cChar *l_passwd = gpapp.m_cfg["Database"]["Password"];
	std::string l_str;
	dbpswd_out(l_passwd, int(strlen(l_passwd)), l_str);
	gpapp.m_cfg_db.enable_errinfo();

	LG("group_sql", "begin connect database, ip = [%s], db = [%s]\n", l_ip, l_db);
	if (!gpapp.m_cfg_db.connect(const_cast<char*>(l_ip), const_cast<char*>(l_db), const_cast<char*>(l_login), const_cast<char*>(l_str.c_str()), l_errinfo)) {
		LG("group_sql", "connect database failed, error[%s]\n", l_errinfo.c_str());
		THROW_EXCP(excpDB, l_errinfo.c_str());
	}

	LG("group_sql", "connect database ok, begin init datatable\n");

	gpapp.m_tblaccounts = new TBLAccounts(&(gpapp.m_cfg_db));
	gpapp.m_tblcharaters = new TBLCharacters(&(gpapp.m_cfg_db));
	gpapp.m_tblpersoninfo = new TBLPersonInfo(&(gpapp.m_cfg_db));
	gpapp.m_tblpersonavatar = new TBLPersonAvatar(&(gpapp.m_cfg_db));
	gpapp.m_tblfriends = new TBLFriends(&(gpapp.m_cfg_db));
	gpapp.m_tblmaster = new TBLMaster(&(gpapp.m_cfg_db));
	gpapp.m_tblguilds = new TBLGuilds(&(gpapp.m_cfg_db));
	gpapp.m_tblX1 = new friend_tbl(&(gpapp.m_cfg_db));
	gpapp.m_tbLparam = new TBLParam(&(gpapp.m_cfg_db));

#ifdef SHUI_JING
	gpapp.m_tblcrystal = new TBLCrystalTrade(&(gpapp.m_cfg_db));
#endif

	LG("group_sql", "begin check table [account] \n");
	if (!gpapp.m_tblaccounts->IsReady()) {
		LG("group_sql", " check table [account] failed\n");
		THROW_EXCP(excpDB, RES_STRING(GP_GROUPSERVERAPPINIT_CPP_00003));
	}

	LG("group_sql", "begin check table [guild]\n");
	if (!gpapp.m_tblguilds->IsReady()) {
		LG("group_sql", " check table [guild] failed \n");
		THROW_EXCP(excpDB, RES_STRING(GP_GROUPSERVERAPPINIT_CPP_00004));
	}
	gpapp.m_tblcharaters->ZeroAddr();

	LG("group_sql", "begin check table [param]\n");

	if (gpapp.m_tbLparam->InitParam())
		LG("group_sql", "init database success\n");
}

void InitACTSvrConnect(GroupServerApp &gpapp) {
	gpapp.m_name = gpapp.m_cfg["Main"]["Name"];
	IniSection &l_is = gpapp.m_cfg["AccountServer"];
	dstring	l_passwd = l_is["Password"];
	cChar *l_ip = l_is["IP"];
	uShort l_port = atoi(l_is["Port"]);

	char buffer[512];
	while (!g_exit && !g_gpsvr->m_acctsock) {
		gpapp.m_acctsock = gpapp.Connect(l_ip, l_port);
		if (!gpapp.m_acctsock) {
			memset(buffer, 0, sizeof(buffer));
			CFormatParameter param(2);
			param.setString(0, l_ip);
			param.setLong(1, l_port);
			RES_FORMAT_STRING(GP_GROUPSERVERAPPINIT_CPP_00005, param, buffer);
			std::cout << buffer << std::endl;
			Sleep(5000);
			continue;
		}

		WPacket	l_wpk = gpapp.GetWPacket();
		l_wpk.WriteCmd(CMD_PA_LOGIN);
		l_wpk.WriteString(gpapp.m_name);
		l_wpk.WriteString(l_passwd);
		RPacket l_rpk = gpapp.SyncCall(gpapp.m_acctsock, l_wpk);
		if (!l_rpk.HasData()) {
			memset(buffer, 0, sizeof(buffer));
			CFormatParameter param(2);
			param.setString(0, l_ip);
			param.setLong(1, l_port);
			RES_FORMAT_STRING(GP_GROUPSERVERAPPINIT_CPP_00006, param, buffer);
			std::cout << buffer << std::endl;
			gpapp.Disconnect(gpapp.m_acctsock, 0, 100);
			break;
		}

		// A non-zero indicates a failure.
		if (l_rpk.ReadShort()) {
			memset(buffer, 0, sizeof(buffer));
			CFormatParameter param(2);
			param.setString(0, l_ip);
			param.setLong(1, l_port);
			RES_FORMAT_STRING(GP_GROUPSERVERAPPINIT_CPP_00007, param, buffer);
			std::cout << buffer << std::endl;
			gpapp.Disconnect(gpapp.m_acctsock, 0, 200);
			break;
		}
		gpapp.m_acctsock->SetPointer(0);
	}
}
