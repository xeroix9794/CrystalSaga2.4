#include "excp.h"
#include "DataSocket.h"
#include "GroupServerApp.h"
#include "Player.h"
#include "Team.h"
#include "GameCommon.h"

_DBC_USING

Player::Player(uLong size) :PreAllocStru(size), m_gate(0), m_gtAddr(0), m_refuse_sess(false), m_refuse_tome(false) {
	m_mtxCha.Create(false);
}

Player::~Player() {
}

void Player::Initially() {
	RunBiDirectItem<Player>::Initially();
	GuildMember::Initially();
	TeamMember::Initially();

	m_lastip[0] = 1;
	m_lastreason[0] = 1;
	m_lastleavetime[0] = 1;
	m_pingply = 0;

	m_gm = 0;
	m_gate = 0;
	m_gtAddr = 0;
	m_bp_currcha = -1;
	m_currcha = -1;
	m_chanum = 0;
	m_acctid = 0;
	m_acctLoginID = 0;
	m_acctname = "";
	m_chatnum = 0;
	m_chatarranum = 0;

	m_bWG = false;
	m_bCheat = false;

	m_lChatMoney = 1000;
	m_lTradeChatMoney = 1000;
	m_bNew = false;

	EndPlayReset();
}

void Player::Finally() {
	m_gate = 0;
	m_gtAddr = 0;
	m_bp_currcha = -1;
	m_currcha = -1;
	m_chanum = 0;
	m_acctid = 0;
	m_acctLoginID = 0;
	m_acctname = "";
	m_chatnum = 0;
	m_chatarranum = 0;

	m_lChatMoney = 1000;
	m_lTradeChatMoney = 1000;
	m_bNew = false;

	TeamMember::Finally();
	GuildMember::Finally();
	RunBiDirectItem<Player>::Finally();
}

bool Player::BeginRun() {
	return RunBiDirectItem<Player>::_BeginRun(&(g_gpsvr->m_plylst)) ? true : false;
}

bool Player::EndRun() {
	return RunBiDirectItem<Player>::_EndRun() ? true : false;
}

// MRF_WHY: Why does VC++ Shows an error even though it builds normally?! Should I fix it?!
void Player::EndPlay(DataSocket *datasock) {
	if (m_currcha >= 0) {
		CountEstopTime();
		if (GetTeam()) {
			RPacket l_rpk1;
			g_gpsvr->CP_TEAM_LEAVE(this, datasock, l_rpk1);
		}

		if (m_chatarranum) {
			WPacket l_wpk1 = g_gpsvr->GetWPacket();
			MutexArmor selock(m_mtxChat);
			while (m_chatarranum) {
				WPacket l_wpk2 = l_wpk1;
				l_wpk2.WriteLong(m_chat[0]->GetSession()->GetID());
				g_gpsvr->CP_SESS_LEAVE(this, datasock, RPacket(l_wpk2));
			}
			selock.unlock();
		}

		if (GetGuild()) {
			WPacket l_toGuild = g_gpsvr->GetWPacket();
			l_toGuild.WriteCmd(CMD_PC_GUILD);
			l_toGuild.WriteChar(MSG_GUILD_OFFLINE);
			l_toGuild.WriteLong(m_chaid[m_currcha]);

			Player *l_plylst[10240];
			short l_plynum = 0;

			RunChainGetArmor<GuildMember> l(*GetGuild());
			Player *l_ply;
			while (l_ply = static_cast<Player*>(GetGuild()->GetNextItem())) {
				if (l_ply != this) {
					l_plylst[l_plynum] = l_ply;
					l_plynum++;
				}
			}
			l.unlock();
			g_gpsvr->SendToClient(l_plylst, l_plynum, l_toGuild);
			LeaveGuild();
		}

		{
			MutexArmor l_lockDB(g_gpsvr->m_mtxDB);
			g_gpsvr->m_tblcharaters->SetAddr(m_chaid[m_currcha], 0);
			--(g_gpsvr->m_curChaNum);

			// Friends off the assembly line notice
			{
				friend_dat l_farray[200];
				int l_num = 200;
				g_gpsvr->m_tblX1->get_friend_dat(l_farray, l_num, m_chaid[m_currcha]);

				WPacket	l_toFrnd = g_gpsvr->GetWPacket();
				l_toFrnd.WriteCmd(CMD_PC_FRND_REFRESH);
				l_toFrnd.WriteChar(MSG_FRND_REFRESH_OFFLINE);
				l_toFrnd.WriteLong(m_chaid[m_currcha]);

				Player *l_plylst[10240];
				short l_plynum = 0;

				Player	*l_ply1; char	l_currcha;
				for (int i = 0; i < l_num; i++) {
					if (l_farray[i].cha_id && (l_ply1 = (Player*)MakePointer(l_farray[i].memaddr)) && ((l_currcha = l_ply1->m_currcha) >= 0) && (l_ply1->m_chaid[l_currcha] == l_farray[i].cha_id)) {
						l_plylst[l_plynum] = l_ply1;
						l_plynum++;
					}
				}
				g_gpsvr->SendToClient(l_plylst, l_plynum, l_toFrnd);
			}

			// Apprentice off the assembly line notice
			{
				TBLMaster::master_dat l_farray[200];
				int l_num = 200;
				g_gpsvr->m_tblmaster->GetMasterData(l_farray, l_num, m_chaid[m_currcha]);

				WPacket	l_toFrnd = g_gpsvr->GetWPacket();
				l_toFrnd.WriteCmd(CMD_PC_MASTER_REFRESH);
				l_toFrnd.WriteChar(MSG_PRENTICE_REFRESH_OFFLINE);
				l_toFrnd.WriteLong(m_chaid[m_currcha]);

				Player *l_plylst[10240];
				short l_plynum = 0;

				Player *l_ply1; 
				char l_currcha;
				for (int i = 0; i < l_num; i++) {
					if (l_farray[i].cha_id && (l_ply1 = (Player*)MakePointer(l_farray[i].memaddr)) && ((l_currcha = l_ply1->m_currcha) >= 0) && (l_ply1->m_chaid[l_currcha] == l_farray[i].cha_id)) {
						l_plylst[l_plynum] = l_ply1;
						l_plynum++;
					}
				}
				g_gpsvr->SendToClient(l_plylst, l_plynum, l_toFrnd);
				m_CurrMasterNum = 0;
			}

			// Instructor offline notification
			{
				TBLMaster::master_dat l_farray[200];
				int l_num = 200;
				g_gpsvr->m_tblmaster->GetPrenticeData(l_farray, l_num, m_chaid[m_currcha]);

				WPacket	l_toFrnd = g_gpsvr->GetWPacket();
				l_toFrnd.WriteCmd(CMD_PC_MASTER_REFRESH);
				l_toFrnd.WriteChar(MSG_MASTER_REFRESH_OFFLINE);
				l_toFrnd.WriteLong(m_chaid[m_currcha]);

				Player *l_plylst[10240];
				short l_plynum = 0;

				Player	*l_ply1; char	l_currcha;
				for (int i = 0; i < l_num; i++) {
					if (l_farray[i].cha_id && (l_ply1 = (Player*)MakePointer(l_farray[i].memaddr)) && ((l_currcha = l_ply1->m_currcha) >= 0) && (l_ply1->m_chaid[l_currcha] == l_farray[i].cha_id)) {
						l_plylst[l_plynum] = l_ply1;
						l_plynum++;
					}
				}
				g_gpsvr->SendToClient(l_plylst, l_plynum, l_toFrnd);
				m_CurrPrenticeNum = 0;
			}
			l_lockDB.unlock();
		}
	}
}

void Player::EndPlayReset() {
	m_refuse_tome = false;
	m_refuse_sess = false;
	m_worldtick = 0;
	m_tradetick = 0;
	m_worldticktemp = 0;
	m_tradeticktemp = 0;
	m_toyoutick = 0;
}

char Player::FindIndexByChaName(cChar *chaname) {
	if (!chaname)
		return -1;

	char i = 0;
	for (i = 0; i < m_chanum; i++) {
		if (m_chaname[i] == chaname)
			break;
	}

	if (i < m_chanum)
		return i;
	else
		return -1;
}

void Player::SendSysInfo(cChar	*info) {
	if (m_currcha < 0 || !m_gate || !m_gate->GetDataSock())return;
	WPacket l_wpk = m_gate->GetDataSock()->GetWPacket();
	l_wpk.WriteCmd(CMD_MC_SYSINFO);
	l_wpk.WriteString(info);
	g_gpsvr->SendToClient(this, l_wpk);
}

extern BOOL GetOnlineCount(DWORD& dwLoginNum, DWORD& dwPlayerNum);

void Player::DoCommand(cChar *cmd) {
	if ((!strcmp(cmd, "getlastconnection")) && PrivilegeCheck::Instance()->HasPrivilege(this->m_gm, "getlastconnection")) {
		MutexArmor l_lockDB(g_gpsvr->m_mtxDB);
		if (m_lastip[0] == 1) {
			g_gpsvr->m_tblaccounts->FetchRowByActID(m_acctid);
			strncpy_s(m_lastip, sizeof(m_lastip), g_gpsvr->m_tblaccounts->GetLast_IP(), _TRUNCATE);

			//strcpy(m_lastreason,g_gpsvr->m_tblaccounts->GetDisc_Reason());
			strncpy_s(m_lastreason, sizeof(m_lastreason), g_gpsvr->m_tblaccounts->GetDisc_Reason(), _TRUNCATE);

			//strcpy(m_lastleavetime,g_gpsvr->m_tblaccounts->GetLast_Leave());
			strncpy_s(m_lastleavetime, sizeof(m_lastleavetime), g_gpsvr->m_tblaccounts->GetLast_Leave(), _TRUNCATE);

		}
		l_lockDB.unlock();
		char l_buf[512];
		CFormatParameter param(3);
		param.setString(0, m_lastip);
		param.setString(1, m_lastleavetime);
		param.setString(2, m_lastreason);
		RES_FORMAT_STRING(GP_PLAYER_CPP_00001, param, l_buf);
		SendSysInfo(l_buf);
	}
	else if ((!strncmp(cmd, "getuserconnection ", strlen("getuserconnection "))) && PrivilegeCheck::Instance()->HasPrivilege(this->m_gm, "getuserconnection")) {
		Player	*l_ply = g_gpsvr->FindPlayerByChaName(cmd + strlen("getuserconnection "));
		if (!l_ply) {
			char l_buf[512];
			_snprintf_s(l_buf, sizeof(l_buf), _TRUNCATE, RES_STRING(GP_PLAYER_CPP_00002), cmd + strlen("getuserconnection "));
			SendSysInfo(l_buf);
		}
		else {
			MutexArmor l_lockDB(g_gpsvr->m_mtxDB);
			if (l_ply->m_lastip[0] == 1) {
				g_gpsvr->m_tblaccounts->FetchRowByActID(l_ply->m_acctid);
				strncpy_s(l_ply->m_lastip, sizeof(l_ply->m_lastip), g_gpsvr->m_tblaccounts->GetLast_IP(), _TRUNCATE);
				strncpy_s(l_ply->m_lastreason, sizeof(l_ply->m_lastreason), g_gpsvr->m_tblaccounts->GetDisc_Reason(), _TRUNCATE);
				strncpy_s(l_ply->m_lastleavetime, sizeof(l_ply->m_lastleavetime), g_gpsvr->m_tblaccounts->GetLast_Leave(), _TRUNCATE);
			}
			l_lockDB.unlock();
			
			char l_buf[512];
			CFormatParameter param(4);
			param.setString(0, cmd + strlen("getuserconnection "));
			param.setString(1, l_ply->m_lastip);
			param.setString(2, l_ply->m_lastleavetime);
			param.setString(3, l_ply->m_lastreason);
			RES_FORMAT_STRING(GP_PLAYER_CPP_00003, param, l_buf);
			SendSysInfo(l_buf);
		}
	}
	else if ((!strcmp(cmd, "getusernum+")) && PrivilegeCheck::Instance()->HasPrivilege(this->m_gm, "getusernum+")) {
		Player *l_ply = g_gpsvr->FindPlayerByChaName(m_chaname[m_currcha].c_str());
		if (l_ply) {
			if (l_ply->m_gm > 0) {
				MutexArmor l_lockDB(g_gpsvr->m_mtxDB);
				DWORD dwLoginNum = DWORD(g_gpsvr->m_plylst.GetTotal());
				DWORD dwPlayerNum = DWORD(g_gpsvr->m_curChaNum);
				if (GetOnlineCount(dwLoginNum, dwPlayerNum)) {
					char l_buf[512];
					CFormatParameter param(2);
					param.setLong(0, dwLoginNum);
					param.setLong(1, dwPlayerNum);
					RES_FORMAT_STRING(GP_PLAYER_CPP_00004, param, l_buf);
					SendSysInfo(l_buf);
				}
			}
		}
	}
	else if ((!strncmp(cmd, "ping ", strlen("ping "))) && PrivilegeCheck::Instance()->HasPrivilege(this->m_gm, "ping")) {
		Player *l_ply = g_gpsvr->FindPlayerByChaName(cmd + strlen("ping "));
		if (!l_ply) {
			char l_buf[512];
			_snprintf_s(l_buf, sizeof(l_buf), _TRUNCATE, RES_STRING(GP_PLAYER_CPP_00005), cmd + strlen("ping "));
			SendSysInfo(l_buf);
		}
		else {
			l_ply->m_pingply = this;
			WPacket	l_wpk = g_gpsvr->GetWPacket();
			l_wpk.WriteCmd(CMD_PC_PING);
			g_gpsvr->SendToClient(l_ply, l_wpk);
		}
	}
	else if ((!strncmp(cmd, "bbs ", strlen("bbs "))) && PrivilegeCheck::Instance()->HasPrivilege(this->m_gm, "bbs")) {
		char *l_dim;
		cChar *l_cmd = cmd + strlen("bbs ");
		l_dim = (char*)strchr(l_cmd, ',');
		if (!l_dim) 
			SendSysInfo(RES_STRING(GP_PLAYER_CPP_00006));

		*l_dim = 0;
		uLong l_inter = atoi(l_cmd);
		if (!l_inter)
			SendSysInfo(RES_STRING(GP_PLAYER_CPP_00007));
		
		l_cmd = l_dim + 1;
		l_dim = (char*)strchr(l_cmd, ',');
		if (!l_dim)
			SendSysInfo(RES_STRING(GP_PLAYER_CPP_00006));
		
		*l_dim = 0;
		uLong l_times = atoi(l_cmd);
		if (!l_times)
			SendSysInfo(RES_STRING(GP_PLAYER_CPP_00008));
		
		l_cmd = l_dim + 1;
		if (!strlen(l_cmd))
			SendSysInfo(RES_STRING(GP_PLAYER_CPP_00009));

		g_gmbbs->AddBBS(l_inter, l_times, l_cmd);
	}
	else if ((!strncmp(cmd, "estop", strlen("estop"))) && PrivilegeCheck::Instance()->HasPrivilege(this->m_gm, "estop")) {
		cChar* l_cmd = cmd + strlen("estop");
		l_cmd = strchr(l_cmd, ' ');
		if (!l_cmd) 
			SendSysInfo(RES_STRING(GP_PLAYER_CPP_00012));

		cChar* l_plyname = ++l_cmd;
		l_cmd = strchr(l_cmd, ',');
		if (!l_cmd) 
			SendSysInfo(RES_STRING(GP_PLAYER_CPP_00012));
	
		*const_cast<char*>(l_cmd) = 0;
		uLong lTimes = atoi(++l_cmd);
		EstopPlayer(l_plyname, lTimes);

	}
	else if ((!strncmp(cmd, "delestop", strlen("delestop"))) && PrivilegeCheck::Instance()->HasPrivilege(this->m_gm, "delestop")) {
		cChar* l_cmd = cmd + strlen("estop");
		l_cmd = strchr(l_cmd, ' ');
		if (!l_cmd) 
			SendSysInfo(RES_STRING(GP_PLAYER_CPP_00013));

		cChar* l_plyname = ++l_cmd;
		DelEstopPlayer(l_plyname);

	}
	else if ((!strncmp(cmd, "disable", strlen("disable"))) && PrivilegeCheck::Instance()->HasPrivilege(this->m_gm, "disable")) {
		// Mask the role account
		cChar* l_cmd = cmd + strlen("estop");
		l_cmd = strchr(l_cmd, ' ');

		if (!l_cmd) 
			SendSysInfo(RES_STRING(GP_PLAYER_CPP_00014));

		cChar* l_plyname = ++l_cmd;
		l_cmd = strchr(l_cmd, ',');

		if (!l_cmd) 
			SendSysInfo(RES_STRING(GP_PLAYER_CPP_00014));
	
		*const_cast<char*>(l_cmd) = 0;
		uLong lTimes = atoi(++l_cmd);
		DisablePlayer(l_plyname, lTimes);

	}
	else {
		SendSysInfo(RES_STRING(GP_PLAYER_CPP_00015));
		return;
	}
}

bool Player::IsEstop() {
	if (!m_estop[m_currcha])
		return false;
	
	uLong l_curtick = g_gpsvr->GetCurrentTick();
	if (l_curtick - this->m_worldtick > 1000 * 60 * 2) {
		MutexArmor l_lockDB(g_gpsvr->m_mtxDB);
		if (!g_gpsvr->m_tblcharaters->IsEstop(m_chaid[m_currcha])) {
			// The release command is sent
			WPacket l_wpk = g_gpsvr->GetWPacket();
			l_wpk.WriteCmd(CMD_PT_DEL_ESTOPUSER);
			g_gpsvr->SendToClient(this, l_wpk);

			SendSysInfo(RES_STRING(GP_PLAYER_CPP_00016));

			CountEstopTime();
			m_estop[m_currcha] = false;
			return false;
		}
		else {
			m_estop[m_currcha] = true;
			return true;
		}
	}
	return true;
}

void Player::CheckEstop() {
	MutexArmor l_lockDB(g_gpsvr->m_mtxDB);
	g_gpsvr->m_tblcharaters->StartEstopTime(m_chaid[m_currcha]);
	if (g_gpsvr->m_tblcharaters->IsEstop(m_chaid[m_currcha])) {
		// Send gag messages and commands
		WPacket l_wpk = g_gpsvr->GetWPacket();
		l_wpk.WriteCmd(CMD_PT_ESTOPUSER);
		g_gpsvr->SendToClient(this, l_wpk);

		SendSysInfo(RES_STRING(GP_PLAYER_CPP_00017));

		m_estop[m_currcha] = true;
		m_worldtick = g_gpsvr->GetCurrentTick();
	}
}

void Player::CountEstopTime() {
	MutexArmor l_lockDB(g_gpsvr->m_mtxDB);
	if (m_estop[m_currcha]) g_gpsvr->m_tblcharaters->EndEstopTime(m_chaid[m_currcha]);
}

void Player::DelEstopPlayer(cChar* plyname) {
	Player* ply = g_gpsvr->FindPlayerByChaName(plyname);
	if (ply) {

		// The role of online prohibition
		ply->m_estop[ply->m_currcha] = false;
	}

	// Database operations
	MutexArmor l_lockDB(g_gpsvr->m_mtxDB);
	if (!g_gpsvr->m_tblcharaters->DelEstop(plyname)) {
		char szData[128];
		_snprintf_s(szData, sizeof(szData), _TRUNCATE, RES_STRING(GP_PLAYER_CPP_00018), plyname);
		SendSysInfo(szData);
		return;
	}
	else {
		char szData[128];
		_snprintf_s(szData, sizeof(szData), _TRUNCATE, RES_STRING(GP_PLAYER_CPP_00019), plyname);
		SendSysInfo(szData);

		char szTemp[128];
		_snprintf_s(szTemp, sizeof(szTemp), _TRUNCATE, RES_STRING(GP_PLAYER_CPP_00020), m_chaname[m_currcha].c_str());
		ply->SendSysInfo(szTemp);

		// Send the silence command
		WPacket l_wpk = g_gpsvr->GetWPacket();
		l_wpk.WriteCmd(CMD_PT_DEL_ESTOPUSER);
		g_gpsvr->SendToClient(ply, l_wpk);

		LogLine l_line(g_LogGrpServer);
		l_line << newln << "char[" << m_chaname[m_currcha] << "]may speak plyname:[" << ply->m_chaname[ply->m_currcha] << "]" << endln;
	}
}

void Player::EstopPlayer(cChar* plyname, uLong lTimes) {
	Player* ply = g_gpsvr->FindPlayerByChaName(plyname);
	if (ply) {
		// The role of online prohibition
		ply->m_estop[ply->m_currcha] = true;
	}

	// Database operations
	MutexArmor l_lockDB(g_gpsvr->m_mtxDB);
	if (!g_gpsvr->m_tblcharaters->Estop(plyname, lTimes)) {
		char szData[128];
		_snprintf_s(szData, sizeof(szData), _TRUNCATE, RES_STRING(GP_PLAYER_CPP_00023), plyname);
		SendSysInfo(szData);
		return;
	}
	else {
		char szData[128];
		CFormatParameter param(2);
		param.setString(0, plyname);
		param.setLong(1, lTimes);
		RES_FORMAT_STRING(GP_PLAYER_CPP_00024, param, szData);
		SendSysInfo(szData);

		char szTemp[128];
		CFormatParameter paramTemp(2);
		paramTemp.setString(0, m_chaname[m_currcha].c_str());
		paramTemp.setLong(1, lTimes);
		RES_FORMAT_STRING(GP_PLAYER_CPP_00025, paramTemp, szTemp);
		ply->SendSysInfo(szTemp);

		// ·¢ËÍ½ûÑÔÃüÁî
		WPacket l_wpk = g_gpsvr->GetWPacket();
		l_wpk.WriteCmd(CMD_PT_ESTOPUSER);
		g_gpsvr->SendToClient(ply, l_wpk);

		LogLine l_line(g_LogGrpServer);
		l_line << newln << "char [" << m_chaname[m_currcha] << "] can't speack plyname:[" << ply->m_chaname[ply->m_currcha] << "]" << lTimes << "minute" << endln;
	}
}

void Player::DisablePlayer(cChar* plyname, uLong lTimes) {
	// Mask roles
	Player* ply = g_gpsvr->FindPlayerByChaName(plyname);
	if (ply) {
		ply->EndPlay(m_gate->GetDataSock());
		if (ply->EndRun()) {
			// An End Accounting command is sent to the AccountServer
			WPacket l_wpk = g_gpsvr->GetWPacket();
			l_wpk.WriteCmd(CMD_PA_USER_BILLEND);
			l_wpk.WriteString(ply->m_acctname.c_str());
			g_gpsvr->SendData(g_gpsvr->m_acctsock, l_wpk);

			// Send the Disable command to AccountServer
			l_wpk = g_gpsvr->GetWPacket();
			l_wpk.WriteCmd(CMD_PA_USER_DISABLE);
			l_wpk.WriteLong(ply->m_acctLoginID);
			l_wpk.WriteLong(lTimes);
			g_gpsvr->SendData(g_gpsvr->m_acctsock, l_wpk);

			// Send the Kicker command to the GameServer
			l_wpk = g_gpsvr->GetWPacket();
			l_wpk.WriteCmd(CMD_AP_KICKUSER);
			g_gpsvr->SendToClient(ply, l_wpk);

			char szData[128];
			_snprintf_s(szData, sizeof(szData), _TRUNCATE, RES_STRING(GP_PLAYER_CPP_00028), plyname);
			SendSysInfo(szData);

			// Kicking success
			LogLine l_line(g_LogGrpServer);
			l_line << newln << "screen acctid/acctname:[" << ply->m_acctid << "]/[" << ply->m_acctname << "],kill the player!" << endln;
		}
		ply->Free();
	}
	else {
		int cha_accid;
		if (!g_gpsvr->m_tblcharaters->FetchAccidByChaName(plyname, cha_accid)) {
			char szData[128];
			_snprintf_s(szData, sizeof(szData), _TRUNCATE, RES_STRING(GP_PLAYER_CPP_00031), plyname);
			SendSysInfo(szData);
			return;
		}
		else {
			// Send the Disable command to AccountServer
			WPacket l_wpk = g_gpsvr->GetWPacket();
			l_wpk.WriteCmd(CMD_PA_USER_DISABLE);
			l_wpk.WriteLong(m_acctLoginID);
			l_wpk.WriteLong(lTimes);
			g_gpsvr->SendData(g_gpsvr->m_acctsock, l_wpk);

			char szData[128];
			_snprintf_s(szData, sizeof(szData), _TRUNCATE, RES_STRING(GP_PLAYER_CPP_00032), plyname);
			SendSysInfo(szData);

			// Kicking success
			LogLine l_line(g_LogGrpServer);
			l_line << newln << "recieve screen acctid/plyname:[" << cha_accid << "]/[" << plyname << "]command!" << endln;
		}
	}
}