#include <iostream>

#include "GroupServerApp.h"
#include "GameCommon.h"
#include "CommFunc.h"
#include "DBConnect.h"
#include "Team.h"

// MRF_WHY: Why was that ever there?
// Timed exit mechanism Macros
// #define CHAEXIT_ONTIME

// FEATURE: GUILD_BANK
#include "GuildBankMsg.h"


LoginManageList::LoginManageList() {
	InitializeCriticalSection(&m_cs);
}

LoginManageList::~LoginManageList() {
	DeleteCriticalSection(&m_cs);
}

bool LoginManageList::Insert(const std::string& name) {
	bool ret = false;
	Lock();
	if (!_Query(name)) {
		m_list.push_back(name);
		ret = true;
	}
	UnLock();
	return ret;
}

bool LoginManageList::Remove(const std::string& name) {
	bool ret = false;
	LoginNameList::iterator it;
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

bool LoginManageList::Query(const std::string& name) {
	bool ret = false;
	Lock();

	ret = _Query(name);

	UnLock();
	return ret;
}

bool LoginManageList::_Query(const std::string& name) {
	bool ret = false;
	LoginNameList::iterator it;

	for (it = m_list.begin(); it != m_list.end(); it++) {
		if ((*it) == name) {
			ret = true;
			break;
		}
	}

	return ret;
}

void LoginManageList::Lock() {
	EnterCriticalSection(&m_cs);
}

void LoginManageList::UnLock() {
	LeaveCriticalSection(&m_cs);
}

// Returns: true-Allows the connection, false-Disables the connection
bool GroupServerApp::OnConnect(DataSocket *datasock) {
	datasock->SetRecvBuf(64 * 1024);
	datasock->SetSendBuf(64 * 1024);
	if (!datasock->IsServer()) {
		LogLine	l_line(g_LogConnect);
		l_line << newln << "AccountServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "come ! Socket num:" << GetSockTotal() + 1 << endln;
		std::cout << "[GroupServer] : AccountServer: " << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << " Connected ! Socket num:" << GetSockTotal() + 1 << std::endl;
	}
	else {
		LogLine	l_line(g_LogConnect);
		l_line << newln << "GateServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "come ! Socket num:" << GetSockTotal() + 1 << endln;
		std::cout << "[GroupServer] : GateServer: " << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << " Connected ! Socket num:" << GetSockTotal() + 1 << std::endl;
	}
	return true;
}

void GroupServerApp::OnDisconnect(DataSocket *datasock, int reason) {
	if (!datasock->IsServer()) {
		m_mtxlogin.lock();
		try {
			if (g_gpsvr->m_acctsock == datasock) {
				g_gpsvr->m_acctsock = 0;
			}
		}
		catch (...) {
			// MRF_REMOVE: Is Useless.
		}

		m_mtxlogin.unlock();
		LogLine	l_line(g_LogConnect);
		l_line << newln << "AccountServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "go ! Socket num:" << GetSockTotal() << "reason=(" << reason << ")" << GetDisconnectErrText(reason) << endln;
		std::cout << "AccountServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "go ! Socket num:" << GetSockTotal() << "reason=" << GetDisconnectErrText(reason).c_str() << std::endl;

		if (reason == DS_SHUTDOWN || reason == DS_DISCONN) {
			return;
		}

		std::cout << "after 5 seconds reconnect......" << std::endl;
		Sleep(5000);
		InitACTSvrConnect(*g_gpsvr);
	}
	else {
		GateServer	*l_gate = (GateServer*)datasock->GetPointer();
		if (l_gate) {
			m_mtxlogin.lock();
			try {
				l_gate->SetDataSock(0);
			}
			catch (...) {
				// MRF_REMOVE: Is Useless.
			}

			m_mtxlogin.unlock();
			LogLine	l_line(g_LogConnect);
			l_line << newln << "GateServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "go ! Socket num:" << GetSockTotal() << "reason=(" << reason << ")" << GetDisconnectErrText(reason) << endln;
			std::cout << "GateServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "go ! Socket num:" << GetSockTotal() << "reason=" << GetDisconnectErrText(reason).c_str() << std::endl;

			Player *l_ply = 0;
			RunChainGetArmor<Player> l(m_plylst);
			while (l_ply = m_plylst.GetNextItem()) {
				if (l_ply->m_gate == l_gate) {
					RPacket	l_rpk;
					TP_USER_LOGOUT(l_ply, datasock, l_rpk);
				}
			}
			l.unlock();
		}
	}
}



WPacket	GroupServerApp::OnServeCall(DataSocket *datasock, RPacket &pk) {
	uShort l_cmd = pk.ReadCmd();
	switch (l_cmd) {
	case CMD_TP_LOGIN:			return TP_LOGIN(datasock, pk);
	case CMD_TP_USER_LOGIN:		return TP_USER_LOGIN(datasock, pk);
	case CMD_TP_REQPLYLST:		return TP_REQPLYLST(datasock, pk);
	case CMD_TP_SYNC_PLYLST:	return TP_SYNC_PLYLST(datasock, pk);
	case CMD_OS_LOGIN:			return OS_LOGIN(datasock, pk);
	}

	Player* l_ply = reinterpret_cast<Player *>(MakePointer(pk.ReverseReadLong()));
	uLong l_gtaddr = pk.ReverseReadLong();
	uLong l_plygt = 0;

	try {
		l_plygt = l_ply->m_gtAddr;
	}
	catch (...) {
		l_ply = 0;
	}

	if (!l_ply || l_gtaddr != l_plygt) {
		WPacket	l_retpk = GetWPacket();
		l_retpk.WriteShort(ERR_PT_KICKUSER);
		return l_retpk;
	}

	switch (l_cmd) {
	case CMD_TP_USER_LOGOUT:		return TP_USER_LOGOUT(l_ply, datasock, pk);
	case CMD_TP_BGNPLAY:			return TP_BGNPLAY(l_ply, datasock, pk);
	case CMD_TP_ENDPLAY:			return TP_ENDPLAY(l_ply, datasock, pk);
	case CMD_TP_NEWCHA:				return TP_NEWCHA(l_ply, datasock, pk);
	case CMD_TP_DELCHA:				return TP_DELCHA(l_ply, datasock, pk);
	case CMD_TP_CREATE_PASSWORD2:	return TP_CREATE_PASSWORD2(l_ply, datasock, pk);
	case CMD_TP_UPDATE_PASSWORD2:	return TP_UPDATE_PASSWORD2(l_ply, datasock, pk);
	}

	return 0;
}

void GroupServerApp::OnProcessData(DataSocket *datasock, RPacket &recvbuf) {
	try {
		uShort l_cmd = recvbuf.ReadCmd();
		switch (l_cmd) {
		case CMD_AP_KICKUSER:
			recvbuf.ReadShort();
			AP_KICKUSER(datasock, recvbuf);
			return;

			// Anti-addiction, whatever that means...
		case CMD_AP_EXPSCALE:
			AP_EXPSCALE(datasock, recvbuf);
			return;
		case CMD_TP_DISC:
			TP_DISC(datasock, recvbuf);
			return;
		case CMD_TP_ESTOPUSER_CHECK:
			TP_ESTOPUSER_CHECK(datasock, recvbuf);
			return;
		case CMD_MP_GUILD_CHALL_PRIZEMONEY:
			MP_GUILD_CHALL_PRIZEMONEY(NULL, datasock, recvbuf);
			return;
		case CMD_OS_PING:
			OS_PING(datasock, recvbuf);
			return;
		case CMD_MP_GM1SAY1:
			CP_GM1SAY1(NULL, datasock, recvbuf);
			return;
		case CMD_MP_GM1SAY:
			CP_GM1SAY(NULL, datasock, recvbuf);
			return;
		}

		Player* l_ply = reinterpret_cast<Player *>(MakePointer(recvbuf.ReverseReadLong()));
		uLong l_gtaddr = recvbuf.ReverseReadLong();
		try {
			if (!l_ply || (l_cmd != CMD_MP_ENTERMAP &&l_ply->m_currcha < 0) || l_gtaddr != l_ply->m_gtAddr) {
				KickUser(datasock, MakeULong(l_ply), l_gtaddr);
				return;
			}
		}
		catch (...) {
			KickUser(datasock, MakeULong(l_ply), l_gtaddr);
			return;
		}

		switch (l_cmd) {
			// FEATURE: GUILD_BANK
		case CMD_CP_GUILDBANK: {
			int guildID = l_ply->m_guild[l_ply->m_currcha];
			if (guildID < 1 || guildID > 200) {
				return;
			}

			WPacket	l_wpk = WPacket(recvbuf).Duplicate();
			if (l_cmd == CMD_CP_GUILDBANK) {
				l_wpk.WriteCmd(CMD_PM_GUILDBANK);
			}

			GuildBankMsg gbm;
			gbm.player = l_ply;
			gbm.msg = &l_wpk;

			int queueSize = guildBankMsgQueue[guildID].size();

			if (queueSize >= 10) {
				WPacket	l_wpk = GetWPacket();
				l_wpk.WriteCmd(CMD_PC_ERRMSG);
				l_wpk.WriteString("Guild Bank is currently busy. Try again later.");
				SendToClient(l_ply, l_wpk);
				return;
			}

			guildBankMsgQueue[guildID].push_back(gbm);

			if (guildBankMsgQueue[guildID].size() == 1) {
				SendToClient(l_ply, l_wpk);
			}

			break;
		}
		case CMD_MP_GUILDBANK: {
			int guildID = recvbuf.ReadLong();
			if (guildID < 1 || guildID > 200) {
				return;
			}

			guildBankMsgQueue[guildID].erase(guildBankMsgQueue[guildID].begin());

			if (guildBankMsgQueue[guildID].size() > 0) {
				GuildBankMsg gbm = guildBankMsgQueue[guildID].at(0);
				SendToClient((Player*)gbm.player, (WPacket&)gbm.msg);
			}

			break;
		}
		case CMD_CP_PING:
			CP_PING(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_REPORT_WG:
			CP_REPORT_WG(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_GUILD_MOTTO:
			MP_GUILD_MOTTO(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_GUILD_CHALLMONEY:
			MP_GUILD_CHALLMONEY(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_GUILD_DISBAND:
			MP_GUILD_DISBAND(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_GUILD_LEAVE:
			MP_GUILD_LEAVE(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_GUILD_KICK:
			MP_GUILD_KICK(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_GUILD_CREATE:
			MP_GUILD_CREATE(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_GUILD_APPROVE:
			MP_GUILD_APPROVE(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_GUILD_INVITE:
			CP_GUILD_INVITE(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_GUILD_ACCEPT:
			CP_GUILD_ACCEPT(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_GUILD_REFUSE:
			CP_GUILD_REFUSE(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_SAY2ALL:
			MP_SAY2ALL(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_SAY2TRADE:
			MP_SAY2TRADE(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_REFUSETOME:
			CP_REFUSETOME(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_GM1SAY:
			CP_GM1SAY(l_ply, datasock, recvbuf);
			return;

#ifdef MANAGE_VER
		case CMD_CP_GMSAY:
			CP_GMSAY(l_ply, datasock, recvbuf);
			return;
#endif

		case CMD_CP_GM1SAY1:
			CP_GM1SAY1(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_SAY2TRADE:
			CP_SAY2TRADE(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_SAY2ALL:
			CP_SAY2ALL(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_SAY2YOU:
			CP_SAY2YOU(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_SAY2TEM:
			CP_SAY2TEM(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_SAY2GUD:
			CP_SAY2GUD(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_SAY2GUILD:
			CP_SAY2GUD(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_TEAM_INVITE:
			CP_TEAM_INVITE(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_TEAM_ACCEPT:
			CP_TEAM_ACCEPT(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_TEAM_REFUSE:
			CP_TEAM_REFUSE(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_TEAM_LEAVE:
			CP_TEAM_LEAVE(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_TEAM_KICK:
			CP_TEAM_KICK(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_TEAM_CREATE:
			MP_TEAM_CREATE(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_ENTERMAP:
			MP_ENTERMAP(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_MASTER_CREATE:
			MP_MASTER_CREATE(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_MASTER_DEL:
			MP_MASTER_DEL(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_MASTER_FINISH:
			MP_MASTER_FINISH(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_MASTER_REFRESH_INFO:
			CP_MASTER_REFRESH_INFO(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_PRENTICE_REFRESH_INFO:
			CP_PRENTICE_REFRESH_INFO(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_FRND_INVITE:
			CP_FRND_INVITE(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_FRND_ACCEPT:
			CP_FRND_ACCEPT(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_FRND_REFUSE:
			CP_FRND_REFUSE(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_FRND_DELETE:
			CP_FRND_DELETE(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_FRND_DEL_GROUP:
			CP_FRND_DEL_GROUP(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_FRND_ADD_GROUP:
			CP_FRND_ADD_GROUP(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_FRND_MOVE_GROUP:
			CP_FRND_MOVE_GROUP(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_FRND_CHANGE_GROUP:
			CP_FRND_CHANGE_GROUP(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_FRND_REFRESH_INFO:
			CP_FRND_REFRESH_INFO(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_CHANGE_PERSONINFO:
			CP_CHANGE_PERSONINFO(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_QUERY_PERSONINFO:
			CP_QUERY_PERSONINFO(l_ply, datasock, recvbuf);
		case CMD_CP_SESS_CREATE:
			CP_SESS_CREATE(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_SESS_SAY:
			CP_SESS_SAY(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_SESS_ADD:
			CP_SESS_ADD(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_SESS_LEAVE:
			CP_SESS_LEAVE(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_GARNER2_CGETORDER:
			CP_GARNER2_GETORDER(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_GARNER2_UPDATE:
			MP_GARNER2_UPDATE(l_ply, datasock, recvbuf);
			return;
		case CMD_MP_GMBANACCOUNT:
			MP_GM_BANACCOUNT(l_ply, datasock, recvbuf);
			return;
		case CMD_CP_GUILDATTR: {
			int guildID = l_ply->m_guild[l_ply->m_currcha];
			//printf("%d", guildID);
			Guild *guild = FindGuildByGldID(guildID);
			if (guildID == 0 || !guild) {
				//printf("Fail?");
				return;
			}

			unsigned long perm = (l_ply->m_guildPermission[l_ply->m_currcha] & emGldPermAttr);
			if (perm != emGldPermAttr) {
				//printf("fail2?");
				l_ply->SendSysInfo("You do not have permission to do this.");
				return;
			}

			int attr = recvbuf.ReadChar();
			int cost;
			int maxLv;
			switch (attr) {
			case ATTR_PDEF:
				maxLv = 20;
				cost = 10;
				break;
			case ATTR_MSPD:
				maxLv = 10;
				cost = 5;
				break;
			case ATTR_ASPD:
				maxLv = 10;
				cost = 4;
				break;
			case ATTR_DEF:
			case ATTR_HIT:
			case ATTR_FLEE:
			case ATTR_MXATK:
				maxLv = 20;
				cost = 3;
				break;
			case ATTR_HREC:
			case ATTR_SREC:
				cost = 2;
				maxLv = 30;
				break;
			case ATTR_MXHP:
			case ATTR_MXSP:
				cost = 1;
				maxLv = 100;
				break;
			default:
				return;
			}
			//int availableStatPoints = m_tblguilds->GetGuildStatPoint(guildID);
			if (guild->m_point == 0) {
				//printf("Error %d", 1);
				WPacket	l_wpk = GetWPacket();
				l_wpk.WriteCmd(CMD_PC_ERRMSG);
				l_wpk.WriteString("Guild has no gold. Failed to add stat.");
				SendToClient(l_ply, l_wpk);
				return;
			}


			int currentLv = m_tblguilds->GetGuildAttr(guildID, attr);
			//printf("Current Lv : %d", currentLv);
			int upgCost = (currentLv + 1)*cost;
			if (guild->m_point < upgCost) {
			//	printf("Error %d", 2);

				WPacket	l_wpk = GetWPacket();
				l_wpk.WriteCmd(CMD_PC_ERRMSG);
				l_wpk.WriteString("Guild does not have enough gold. Failed to add stat.");
				SendToClient(l_ply, l_wpk);
				return;
			}
			else if (currentLv >= maxLv) {
				//printf("Error %d", 3);

				WPacket	l_wpk = GetWPacket();
				l_wpk.WriteCmd(CMD_PC_ERRMSG);
				l_wpk.WriteString("Attribute has already reached the max value.");
				SendToClient(l_ply, l_wpk);
				return;
			}
			guild->m_point -= upgCost;
			if (!m_tblguilds->IncrementGuildAttr(guildID, attr, upgCost)) {
				guild->m_point += upgCost;
				WPacket	l_wpk = GetWPacket();
				l_wpk.WriteCmd(CMD_PC_ERRMSG);
				l_wpk.WriteString("Error. Please try again later.");
				SendToClient(l_ply, l_wpk);
				return;
			}

			char luaCmd[32];
			sprintf(luaCmd, "IncrementGuildAttr(%d,%d)", guildID, attr);
			WPacket	l_wpk = GetWPacket();
			l_wpk.WriteCmd(CMD_MM_DO_STRING);
			l_wpk.WriteLong(l_ply->m_chaid[l_ply->m_currcha]);
			l_wpk.WriteString(luaCmd);
			SendToClient(l_ply, l_wpk);

			Player *l_ply1;
			Player *l_plylst[10240];
			short	l_plynum = 0;
			RunChainGetArmor<GuildMember> l(*guild);
			while (l_ply1 = static_cast<Player*>(guild->GetNextItem())) {
				l_plylst[l_plynum] = l_ply1;
				l_plynum++;
			}
			l.unlock();
			m_tblguilds->SaveGuildPoints(guildID, guild->m_point, guild->m_level, guild->m_exp);
			WPacket l_wpkClient = GetWPacket();
			l_wpkClient.WriteCmd(CMD_PC_UPDATEGUILDATTR);
			l_wpkClient.WriteLong(guild->m_level);
			l_wpkClient.WriteLong(guild->m_exp);
			l_wpkClient.WriteLong(guild->m_point);
			l_wpkClient.WriteLong(currentLv + 1);
			SendToClient(l_plylst, l_plynum, l_wpkClient);

			break;

		}
		case CMD_MP_GUILDNOTICE: {
			int guildID = recvbuf.ReadLong();
			string szData;
			szData = recvbuf.ReadString();
			Guild* guild = FindGuildByGldID(guildID);
			Player *l_ply1;
			Player *l_plylst[10240];
			short	l_plynum = 0;
			char lua[32];
			RunChainGetArmor<GuildMember> l(*guild);
			while (l_ply1 = static_cast<Player*>(guild->GetNextItem())) {
				l_plylst[l_plynum] = l_ply1;
				l_plynum++;
				if (!szData.empty()) {
					//l_ply1->SendSysInfo(szData.c_str());
				}
			}
			WPacket pk = GetWPacket();
			pk.WriteCmd(CMD_PC_GUILDNOTICE);
			pk.WriteLong(guildID);
			pk.WriteString(szData.c_str());
			SendToClient(l_plylst, l_plynum, pk);
			l.unlock();
		}
		case CMD_MP_GUILDADDEXP: {
			int guildID = recvbuf.ReadLong();
			int exp = recvbuf.ReadLong();


			Guild *guild = FindGuildByGldID(guildID);
			//guild->m_level; //guild Level
			//guild->m_exp;//guild exp
			//guild->m_point;//guild gold

			//cout << guild->m_level << "\n";

			guild->m_exp += exp;
			cout << "EXP " + guild->m_exp  << endl;
			bool lvUp = false;
			while (true) {
				int guildExpReq = pow(guild->m_level * 10, 2) + 1000;
				if (guild->m_exp > guildExpReq) {
					guild->m_exp -= guildExpReq;
					guild->m_level += 1;
					guild->m_point += ceil((float)guild->m_level / 10);
					lvUp = true;
				}
				else {
					break;
				}
			}

			char msg[64];
			sprintf(msg, "Guild Level Up! Now Level %d.", guild->m_level);

			Player *l_ply1;
			Player *l_plylst[10240];
			short	l_plynum = 0;

			RunChainGetArmor<GuildMember> l(*guild);
			while (l_ply1 = static_cast<Player*>(guild->GetNextItem())) {
				l_plylst[l_plynum] = l_ply1;
				l_plynum++;
				if (lvUp) {
					l_ply1->SendSysInfo(msg);
				}
			}
			l.unlock();
			m_tblguilds->SaveGuildPoints(guildID, guild->m_point, guild->m_level, guild->m_exp);
			WPacket l_wpk = GetWPacket();
			l_wpk.WriteCmd(CMD_PC_UPDATEGUILDATTR);
			l_wpk.WriteLong(guild->m_level);
			l_wpk.WriteLong(guild->m_exp);
			l_wpk.WriteLong(guild->m_point);
			SendToClient(l_plylst, l_plynum, l_wpk);

			char luaCmd[32];
			sprintf(luaCmd, "SetGuildLevel(%d,%d)", guildID, guild->m_level);
			WPacket	l_wpkLua = GetWPacket();
			l_wpkLua.WriteCmd(CMD_MM_DO_STRING);
			l_wpkLua.WriteLong(l_ply->m_chaid[l_ply->m_currcha]);
			l_wpkLua.WriteString(luaCmd);
			SendToClient(l_ply, l_wpkLua);

			break;
		}
		case CMD_MP_GUILD_PERM: {
			int targetID = recvbuf.ReadLong();
			unsigned long permission = recvbuf.ReadLong();

			Guild	*l_guild = l_ply->GetGuild();
			if (!l_guild) {
				return;
			}
			Player	*l_dst = l_guild->FindGuildMemByChaID(targetID);
			//update in memory if player is online
			if (l_dst && l_dst->m_currcha >= 0) {
				l_dst->m_guildPermission[l_dst->m_currcha] = permission;
			}
			//if player is on char select screen, find desired char and update
			//else if (l_dst){
			//	for (int i = 0; i < 10; i++){
			//		if (l_dst->m_chaid[i] == targetID){
			//			l_dst->m_guildPermission[i] = permission;
			//			break;
			//		}
			//	}
			//}
			//notify all in guild that permission has changed.
			WPacket l_wpk = GetWPacket();
			l_wpk.WriteCmd(CMD_PC_GUILD_PERM);
			l_wpk.WriteLong(targetID);
			l_wpk.WriteLong(permission);

			Player *l_plylst[10240];
			short	l_plynum = 0;

			RunChainGetArmor<GuildMember> l(*l_guild);
			while (l_ply = static_cast<Player	*>(l_guild->GetNextItem())) {
				l_plylst[l_plynum] = l_ply;
				l_plynum++;
			}
			l.unlock();

			SendToClient(l_plylst, l_plynum, l_wpk);

		}
		}
	}
	catch (...) {
		// MRF_WHY: NO BUG WILL BE DETECTED.
		return;
	}
}

WPacket GroupServerApp::TP_SYNC_PLYLST(DataSocket *datasock, RPacket &pk) {
	WPacket	l_retpk = GetWPacket();
	m_mtxSyn.lock();
	try {
		uLong num = pk.ReadLong();
		cChar *l_gatename = pk.ReadString();
		GateServer* pServer = this->FindGateSByName(l_gatename);
		if (pServer) {
			l_retpk.WriteShort(ERR_SUCCESS);
			l_retpk.WriteShort((uShort)num);
			for (int i = 0; i < (int)num; i++) {
				Player	*l_ply = g_gpsvr->m_plyheap.Get();
				if (l_ply) {
					l_retpk.WriteShort(1);
					uLong test = MakeULong(l_ply);
					l_retpk.WriteLong(MakeULong(l_ply));

					l_ply->m_gate = pServer;
					l_ply->m_gtAddr = pk.ReadLong();
					l_ply->m_acctLoginID = pk.ReadLong();
					l_ply->m_acctid = pk.ReadLong();
					l_ply->BeginRun();
				}
				else {
					l_retpk.WriteShort(0);
				}
			}
		}
		else
			l_retpk.WriteShort(ERR_PT_LOGFAIL);
	}
	catch (...) {
		l_retpk.WriteShort(ERR_PT_LOGFAIL);
	}
	m_mtxSyn.unlock();
	return l_retpk;
}

WPacket GroupServerApp::OS_LOGIN(DataSocket *datasock, RPacket &pk) {
	WPacket	l_retpk = GetWPacket();
	if (pk.ReadShort() != atoi(m_cfg["Main"]["Version"])) {
		l_retpk.WriteShort(ERR_OS_NOTMATCH_VERSION);
		Disconnect(datasock, 100, -15);
		return l_retpk;
	}
	cChar *agentName = pk.ReadString();

	LogLine	l_line(g_LogConnect);
	l_line << newln << "AgentServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "OS_LOGIN1" << endln;

	m_mtxAgent.lock();

	l_line << newln << "AgentServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "OS_LOGIN2" << endln;
	try {
		if (m_groupServerAgent.SetDataSock(datasock)) {
			l_retpk.WriteShort(ERR_SUCCESS);
			std::cout << "AgentServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "Re-log successful!" << std::endl;
		}
		else {
			l_retpk.WriteShort(ERR_PT_SAMEGATENAME);
			std::cout << "AgentServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "Login failed: A same name is already logged on in AgentServer" << std::endl;
			Disconnect(datasock);
		}
	}
	catch (...) {
		// MRF_REMOVE: Is Useless.
	}

	m_mtxAgent.unlock();
	l_line << newln << "AgentServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "OS_LOGIN3" << endln;
	return l_retpk;
}

void GroupServerApp::OS_PING(DataSocket *datasock, RPacket &pk) {
	WPacket l_wpk = GetWPacket();
	l_wpk.WriteCmd(CMD_SO_PING);
	l_wpk.WriteLong(m_plyheap.GetUsedNum());
	SendData(datasock, l_wpk);
}

void GroupServerApp::CP_PING(Player *ply, DataSocket *datasock, RPacket	&pk) {
	Player *l_ply = ply->m_pingply;
	if (!l_ply)	return;
	ply->m_pingply = 0;
	char l_buf[256];
	_snprintf_s(l_buf, sizeof(l_buf), _TRUNCATE, RES_STRING(GP_GROUPSERVERAPPMASTER_CPP_00001), ply->m_chaname[ply->m_currcha].c_str(), pk.ReadLong());
	l_ply->SendSysInfo(l_buf);
}

void GroupServerApp::CP_REPORT_WG(Player *ply, DataSocket *datasock, RPacket &pk) {
	// Statistics players use plug-in quantity
	if (!ply->m_bWG) {
		ply->m_bWG = TRUE;
		m_curWGChaNum++;
	}
}

void GroupServerApp::KickUser(DataSocket *datasock, uLong gpaddr, uLong gtaddr) {
	WPacket l_wpk = GetWPacket();
	l_wpk.WriteCmd(CMD_PT_KICKUSER);
	l_wpk.WriteLong(gpaddr);
	l_wpk.WriteLong(gtaddr);
	l_wpk.WriteShort(1);
	SendData(datasock, l_wpk);
}

WPacket	GroupServerApp::TP_LOGIN(DataSocket *datasock, RPacket &pk) {
	WPacket	l_retpk = GetWPacket();
	if (pk.ReadShort() != atoi(m_cfg["Main"]["Version"])) {
		l_retpk.WriteShort(ERR_PT_LOGFAIL);
		Disconnect(datasock, 100, -15);
		return l_retpk;
	}

	cChar *l_gatename = pk.ReadString();
	LogLine	l_line(g_LogConnect);
	l_line << newln << "GateServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "TP_LOGIN1" << endln;

	m_mtxlogin.lock();

	l_line << newln << "GateServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "TP_LOGIN2" << endln;

	try {
		GateServer *l_gate = FindGateSByName(l_gatename);
		if (l_gate) {
			if (l_gate->SetDataSock(datasock)) {
				l_gate->SetSync();
				l_retpk.WriteShort(ERR_SUCCESS);
				std::cout << "GateServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "re login success!" << std::endl;
			}
			else {
				l_retpk.WriteShort(ERR_PT_SAMEGATENAME);
				std::cout << "GateServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "login failed: exsit the same name's GateServer " << std::endl;
				Disconnect(datasock);
			}
		}
		else if (m_gatenum < GATE_MAX) {
			m_gate[m_gatenum].SetSync();
			l_retpk.WriteShort(ERR_SUCCESS);
			m_gate[m_gatenum].m_name = l_gatename;
			m_gate[m_gatenum].SetDataSock(datasock);
			m_gatenum++;
			std::cout << "[GroupServer] : GateServer: " << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << " login successful" << std::endl;
		}
		else {
			l_retpk.WriteShort(ERR_PT_LOGFAIL);
			std::cout << "GateServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "login failed!" << std::endl;
			Disconnect(datasock);
		}
	}
	catch (...) {
		// MRF_REMOVE: Is Useless.
	}
	m_mtxlogin.unlock();
	l_line << newln << "GateServer:" << datasock->GetPeerIP() << "," << datasock->GetPeerPort() << "TP_LOGIN3" << endln;
	return l_retpk;
}

WPacket	GroupServerApp::TP_REQPLYLST(DataSocket *datasock, RPacket &pk) {
	GateServer *l_gate = (GateServer *)datasock->GetPointer();
	if (!l_gate) {
		return 0;
	}

	WPacket	l_retpk = GetWPacket();
	Player* l_ply; uShort l_plynum = 0;
	RunChainGetArmor<Player> l(m_plylst);
	for (l_ply = m_plylst.GetNextItem(); l_ply; l_ply = m_plylst.GetNextItem()) {
		if (l_ply->m_gate != l_gate || l_ply->m_currcha < 0)
			continue;

		l_retpk.WriteLong(l_ply->m_gtAddr);
		l_retpk.WriteLong(l_ply->m_chaid[l_ply->m_currcha]);
		l_plynum++;
	}

	l.unlock();
	l_retpk.WriteShort(l_plynum);

	return l_retpk;
}

bool GroupServerApp::GetCHAsFromDBByPlayer(Player *player, WPacket &wpk) {
	MutexArmor l_lockDB(m_mtxDB);
	int l_row = 0;

	// New activation account
	if ((l_row = m_tblaccounts->FetchRowByActName(player->m_acctname.c_str())) == 0) {
		player->m_bNew = true;
		player->m_password.clear();
		if (!m_tblaccounts->InsertRow(player->m_acctLoginID, player->m_acctname.c_str(), "0", atoi(m_cfg["account"]["equal"]) == 1 ? true : false))
			return false;

		if (!m_tblaccounts->FetchRowByActName(player->m_acctname.c_str()))
			return false;

		player->m_acctid = m_tblaccounts->GetActID();

		// MRF_WHY: Why is it always 0.
		// Number of characters
		wpk.WriteChar(0);
	}
	else if (l_row == 1) {
		player->m_bNew = false;
		player->m_acctid = m_tblaccounts->GetActID();
		player->m_password = m_tblaccounts->GetPassword();
		player->m_gm = m_tblaccounts->GetGM();
		std::string l_chaid[Player::emMaxCharacters];
		player->m_chanum = char(Util_ResolveTextLine(m_tblaccounts->GetChaIDs(), l_chaid, Player::emMaxCharacters, ';'));

		// Number of characters
		wpk.WriteChar(player->m_chanum);
		char i = 0, j = 0;
		for (i = 0, j = 0; j < player->m_chanum; i++, j++) {
			player->m_chaid[i] = atoi(l_chaid[j].c_str());
			if (!player->m_chaid[i]) {
				wpk.WriteChar(0);
				i--;
			}
			else {
				if ((l_row = m_tblcharaters->FetchRowByChaID(player->m_chaid[i])) == 1) {
					// Gets the appearance data
					char *l_look = const_cast<char*>(m_tblcharaters->GetLook());
					LOOK look;
					MemSet((char*)&look, 0, sizeof(LOOK));
					try {
						Strin2LookData(&look, std::string(l_look));
					}
					catch (...) {
						LogLine l_line(g_LogGrpServer);
						l_line << newln << "enum account [" << player->m_acctname << "]'s char[ID:" << player->m_chaid[i] << "] find appearance data exception." << endln;
						wpk.WriteChar(0);
						i--;
						continue;
					}

					// Flag this role data is valid
					wpk.WriteChar(1);
					player->m_chaname[i] = m_tblcharaters->GetChaName();
					player->m_motto[i] = m_tblcharaters->GetMotto();
					player->m_icon[i] = m_tblcharaters->GetIcon();
					player->m_guild[i] = m_tblcharaters->GetGuildID();
					player->m_guildPermission[i] = m_tblcharaters->GetGuildPermission();

					wpk.WriteString(m_tblcharaters->GetChaName());
					wpk.WriteString(g_GetJobName(atoi(m_tblcharaters->GetJob())));
					wpk.WriteShort(m_tblcharaters->GetDegree());
					wpk.WriteSequence((char*)&look, sizeof(LOOK));
				}
				else {
					if (l_row == 0) {
						LogLine l_line(g_LogGrpServer);
						l_line << newln << "enum account[" << player->m_acctname << "]'s char[ID:" << player->m_chaid[i] << "] can't find data in table character.";
					}
					else {
						LogLine l_line(g_LogGrpServer);
						l_line << newln << "enum account[" << player->m_acctname << "]'s char[ID:" << player->m_chaid[i] << "] databse exception.";
					}

					// Flag this role data is invalid
					wpk.WriteChar(0);
					i--;
				}
			}
		}
		player->m_chanum = i;
	}
	else {
		LogLine l_line(g_LogGrpServer);
		l_line << newln << "enum account[" << player->m_acctname << "] operate table account failed";
	}
	return true;
}

WPacket	GroupServerApp::TP_USER_LOGIN(DataSocket *datasock, RPacket &pk) {
	uLong l_ulMilliseconds = 25 * 1000;
	uLong l_tick = GetTickCount() - pk.GetTickCount();

	if (l_ulMilliseconds > l_tick) {
		l_ulMilliseconds = l_ulMilliseconds - l_tick;
		WPacket	l_retpk = 0;
		Player *l_ply = g_gpsvr->m_plyheap.Get();

		bool bCheat = (pk.ReverseReadShort() == 911) ? false : true;
		if (bCheat) {
			m_dwCheatCount++;
			l_ply->m_bCheat = true;
		}

		l_ply->m_gate = (GateServer *)datasock->GetPointer();
		l_ply->m_gtAddr = pk.ReverseReadLong();
		in_addr l_ina;
		l_ina.S_un.S_addr = pk.ReverseReadLong();
		strncpy_s(l_ply->m_clientip, sizeof(l_ply->m_clientip), inet_ntoa(l_ina), _TRUNCATE);

		pk.DiscardLast(static_cast<uLong>(sizeof(uShort)));
		pk.DiscardLast(4);

		uShort l_len;
		string szLocale = "";
		szLocale = pk.ReadString();

		cChar *l_passport = pk.ReadString(&l_len);
		if (!l_passport || strlen(l_passport) >= 64) {
			l_ply->Free();
			l_retpk = GetWPacket();
			l_retpk.WriteCmd(ERR_PT_INERR);
			return l_retpk;
		}
		l_ply->m_passport = l_passport;

		cChar *l_acctname = pk.ReadString(&l_len);
		if (!l_acctname || strlen(l_acctname) > 128) {
			l_ply->Free();
			l_retpk = GetWPacket();
			l_retpk.WriteShort(ERR_PT_INERR);
			return l_retpk;
		}

		/*
		if (strchr(l_acctname, '\'') || !IsValidName(l_acctname, l_len)) {
		LogLine l_line(g_LogGrpServer);
		l_line << newln << "(" << l_ply->m_clientip << "):[" << l_ply->m_acctname << "]的名字中因包括非法的单引号'字符而被拒绝登录";
		l_line << endln;
		l_ply->Free();
		l_retpk = GetWPacket();
		l_retpk.WriteShort(ERR_PT_INVALIDDAT);
		return l_retpk;
		}
		*/

		if (m_plylst.GetTotal() >= const_cha.MaxLoginUsr) {
			l_ply->Free();
			l_retpk = GetWPacket();
			l_retpk.WriteShort(ERR_MC_TOOMANYPLY);
			return l_retpk;
		}
		l_ply->m_acctname = l_acctname;

		/*
		// The code here will cause the player to be kicked even if the password is incorrect
		// Account verification, there is no time to land

		if (m_LoginList.Query(l_acctname)) {
		LG("AuthExcp", "Account %s multilogin at same times.1\n", l_acctname);
		KillUserByName(l_acctname);
		WPacket	l_retpk = GetWPacket();
		l_retpk.WriteShort(ERR_AP_LOGGED);
		return l_retpk;
		}
		*/

		WPacket l_wpk = pk;
		l_wpk.WriteCmd(CMD_PA_USER_LOGIN);

		// AccountServer Authentication
		RPacket	l_rpk = SyncCall(m_acctsock, l_wpk, l_ulMilliseconds);
		uShort	l_errno;
		if (!l_rpk.HasData()) {
			l_retpk = GetWPacket();

			// Error Code
			l_retpk.WriteShort(ERR_PT_NETEXCP);
			LogLine l_line(g_LogGrpServer);
			l_line << newln << "(" << l_ply->m_clientip << "):[" << l_ply->m_acctname << "] login net failed" << endln;
			l_ply->Free();
		}
		else if ((l_errno = l_rpk.ReadShort()) && (l_rpk.ReadCmd() == CMD_AP_KICKUSER)) {
			AP_KICKUSER(datasock, RPacket(l_rpk));
			l_rpk.DiscardLast(sizeof(uLong));
			l_retpk = l_rpk;
		}
		else if (l_errno) {
			l_retpk = GetWPacket();

			// Error Code
			l_retpk.WriteShort(l_errno);
			l_retpk = l_rpk;

			LogLine l_line(g_LogGrpServer);
			l_line << newln << "(" << l_ply->m_clientip << "):[" << l_ply->m_acctname << "]login failed, error:" << l_errno << endln;
			l_ply->Free();
		}
		else {
			l_ply->m_acctLoginID = l_rpk.ReadLong();
			uShort l_keylen, l_textlen;
			cChar* l_key = l_rpk.ReadSequence(l_keylen);
			cChar* l_text = l_rpk.ReadSequence(l_textlen);
			l_ply->m_sessid = l_rpk.ReadLong();
			std::string sessionKey = l_rpk.ReadString();

			l_retpk = GetWPacket();

			// Successful return value
			l_retpk.WriteShort(ERR_SUCCESS);

			l_retpk.WriteString(sessionKey.c_str());

			// Returns the encryption key
			l_retpk.WriteSequence(l_key, l_keylen);

			// Enter the database to take the role list
			GetCHAsFromDBByPlayer(l_ply, l_retpk);

			if (l_ply->m_password.length() < 32) {
				LogLine l_line(g_LogErrServer);
				l_line << newln << "account:" << l_ply->m_acctname << "ID:" << l_ply->m_acctid << "Key:(" << l_key << ")" << "len" << l_keylen << "PW2:" << l_ply->m_password << endln;
			}

			// Returns the plaintext key
			l_retpk.WriteSequence(l_text, l_textlen);
			l_retpk.WriteShort(l_textlen);

			if (l_ply->m_password.length() > 0 && !l_ply->m_bNew) {
				l_retpk.WriteChar(1);
			}
			else {
				l_retpk.WriteChar(0);
			}

			l_retpk.WriteLong(l_ply->m_acctid);
			l_retpk.WriteLong(l_ply->m_acctLoginID);

			// Include your own address
			l_retpk.WriteLong(MakeULong(l_ply));

			if (!m_LoginList.Insert(l_acctname)) {
				LG("AuthExcp", "Account %s multilogin at same times.2\n", l_acctname);
				KillUserByName(l_acctname);
				WPacket	l_retpk = GetWPacket();

				// Error Code
				l_retpk.WriteShort(ERR_AP_LOGGED);
				l_ply->Free();
				return l_retpk;
			}

			l_ply->BeginRun();

			// AddPlayerToList(l_ply->m_chaid[l_ply->m_currcha], l_ply);
			LogLine l_line(g_LogGrpServer);
			l_line << newln << "(" << l_ply->m_clientip << "):[" << l_ply->m_acctname << "]login success, \t nline/total:" << m_plylst.GetTotal() << "/" << long(m_curChaNum) << endln;
		}
		return l_retpk;
	}
	else {
		WPacket	l_retpk = GetWPacket();

		// Error Code
		l_retpk.WriteShort(ERR_PT_NETEXCP);
		LogLine l_line(g_LogGrpServer);
		l_line << newln << "authentication packet time out." << endln;
		return l_retpk;
	}
}

void GroupServerApp::AP_KICKUSER2(DataSocket* datasock, uLong acctid) {
	Player *l_ply = 0;
	uLong l_acctid = acctid;
	RunChainGetArmor<Player> l(m_plylst);
	for (l_ply = m_plylst.GetNextItem(); l_ply; l_ply = m_plylst.GetNextItem()) {
		if (l_ply->m_acctid == l_acctid) {
			break;
		}
	}

	if (l_ply) {
		l_ply->EndPlay(datasock);
		if (l_ply->EndRun()) {
			if (l_ply->m_currcha != -1) {

				// An End Accounting command is sent to the AccountServer
				WPacket l_wpk = GetWPacket();
				l_wpk.WriteCmd(CMD_PA_USER_BILLEND);
				l_wpk.WriteString(l_ply->m_acctname.c_str());
				SendData(m_acctsock, l_wpk);
			}

			// Send the LogOut command to AccountServer 
			WPacket l_wpk = GetWPacket();

			/*
			l_wpk.WriteCmd(CMD_PA_USER_LOGOUT);
			l_wpk.WriteLong(l_ply->m_acctid);
			l_wpk.WriteLong(l_ply->m_sessid);
			SendData(m_acctsock,l_wpk);

			// Send the Kicker command to GateServer
			l_wpk	=GetWPacket();
			*/

			l_wpk.WriteCmd(CMD_AP_KICKUSER);
			SendToClient(l_ply, l_wpk);

			// Kicking success
			LogLine l_line(g_LogGrpServer);
			l_line << newln << "recieved killed acctid/acctname:[" << l_acctid << "]/[" << l_ply->m_acctname << "] command!" << endln;
		}

		// Delete from the list
		m_LoginList.Remove(l_ply->m_acctname);
		l_ply->Free();
	}
	else {
		l.unlock();
		LogLine l_line(g_LogGrpServer);
		l_line << newln << "recieved kill acctid:[" << l_acctid << "] command(not in play list)!";
	}
}

void GroupServerApp::KillUserByName(const char* accoutName) {
	Player *l_ply = 0;
	RunChainGetArmor<Player> l(m_plylst);
	for (l_ply = m_plylst.GetNextItem(); l_ply; l_ply = m_plylst.GetNextItem()) {
		if (strcmp(l_ply->m_acctname.c_str(), accoutName) == 0) {
			break;
		}
	}

	if (l_ply) {
		l_ply->EndPlay(NULL); {
			if (l_ply->m_currcha != -1) {

				// Send an End Accounting command to the AccountServer
				WPacket l_wpk = GetWPacket();
				l_wpk.WriteCmd(CMD_PA_USER_BILLEND);
				l_wpk.WriteString(l_ply->m_acctname.c_str());
				SendData(m_acctsock, l_wpk);
			}

			// Send the LogOut command to AccountServer
			WPacket l_wpk = GetWPacket();
			l_wpk.WriteCmd(CMD_PA_USER_LOGOUT);
			l_wpk.WriteLong(l_ply->m_acctid);
			l_wpk.WriteLong(l_ply->m_sessid);
			SendData(m_acctsock, l_wpk);

			// Send the Kicker command to the GateServer
			l_wpk = GetWPacket();
			l_wpk.WriteCmd(CMD_AP_KICKUSER);
			SendToClient(l_ply, l_wpk);

			// Kicking success
			LogLine l_line(g_LogGrpServer);
			l_line << newln << "recieved killed acctname:[" << l_ply->m_acctname << "] command!" << endln;
		}

		// Remove from the list
		m_LoginList.Remove(l_ply->m_acctname);
		l_ply->Free();
	}
	else {
		l.unlock();
		LogLine l_line(g_LogGrpServer);
		l_line << newln << "recieved kill acctname:[" << accoutName << "] command(not in play list)!";
	}
}

void GroupServerApp::AP_KICKUSER(DataSocket *datasock, RPacket &pk) {
	Player *l_ply = 0;
	uLong l_acctid = pk.ReadLong();
	RunChainGetArmor<Player> l(m_plylst);
	for (l_ply = m_plylst.GetNextItem(); l_ply; l_ply = m_plylst.GetNextItem()) {
		if (l_ply->m_acctLoginID == l_acctid) {
			break;
		}
	}

	if (l_ply) {
		l_ply->EndPlay(datasock);
		if (l_ply->EndRun()) {
			if (l_ply->m_currcha != -1) {

				// An End Accounting command is sent to the AccountServer
				WPacket l_wpk = GetWPacket();
				l_wpk.WriteCmd(CMD_PA_USER_BILLEND);
				l_wpk.WriteString(l_ply->m_acctname.c_str());
				SendData(m_acctsock, l_wpk);
			}

			// Send the LogOut command to AccountServer
			WPacket l_wpk = GetWPacket();
			l_wpk.WriteCmd(CMD_PA_USER_LOGOUT);
			l_wpk.WriteLong(l_ply->m_acctid);
			l_wpk.WriteLong(l_ply->m_sessid);
			SendData(m_acctsock, l_wpk);

			// Send the Kicker command to the GateServer
			l_wpk = GetWPacket();
			l_wpk.WriteCmd(CMD_AP_KICKUSER);
			SendToClient(l_ply, l_wpk);

			// Kicking success
			LogLine l_line(g_LogGrpServer);
			l_line << newln << "recieved killed acctid/acctname:[" << l_ply->m_acctid << "]/[" << l_ply->m_acctname << "] command!" << endln;
		}

		// Remove form the list
		m_LoginList.Remove(l_ply->m_acctname);
		l_ply->Free();
	}
	else {
		l.unlock();
		LogLine l_line(g_LogGrpServer);
		l_line << newln << "recieved kill acctid:[" << l_acctid << "] command(not in play list)!";
	}
}

// Anti-addiction, whatever that is...
void GroupServerApp::AP_EXPSCALE(DataSocket* datasock, RPacket &pk) {
	Player *l_ply = 0;
	uLong ulChaID = pk.ReadLong();
	uLong ulTime = pk.ReadLong();

	RunChainGetArmor<Player> l(m_plylst);
	for (l_ply = m_plylst.GetNextItem(); l_ply; l_ply = m_plylst.GetNextItem()) {
		if (l_ply->m_acctLoginID == ulChaID) {
			break;
		}
	}

	if (l_ply) {
		WPacket	l_wpk = GetWPacket();
		l_wpk.WriteCmd(CMD_PM_EXPSCALE);
		l_wpk.WriteLong(l_ply->m_chaid[l_ply->m_currcha]);
		l_wpk.WriteLong(ulTime);

		SendToClient(l_ply, l_wpk);
	}
}

void GroupServerApp::TP_DISC(DataSocket *datasock, RPacket &pk) {
	uLong l_actid = pk.ReadLong();
	in_addr	l_ina;
	l_ina.S_un.S_addr = pk.ReadLong();
	cChar *l_reason = pk.ReadString();
	MutexArmor l_lockDB(m_mtxDB);
	m_tblaccounts->SetDiscInfo(l_actid, inet_ntoa(l_ina), l_reason);
	l_lockDB.unlock();
}

void GroupServerApp::TP_ESTOPUSER_CHECK(DataSocket *datasock, RPacket &pk) {
	Player *l_ply = 0;
	uLong l_acctid = pk.ReadLong();
	RunChainGetArmor<Player> l(m_plylst);
	for (l_ply = m_plylst.GetNextItem(); l_ply; l_ply = m_plylst.GetNextItem()) {
		if (l_ply->m_acctid == l_acctid) {
			break;
		}
	}

	if (l_ply) {
		l_ply->IsEstop();
	}
}

WPacket GroupServerApp::TP_USER_LOGOUT(Player *ply, DataSocket *datasock, RPacket &pk) {
	WPacket	l_retpk = GetWPacket();
	if (!ply || !ply->m_gtAddr) {
		l_retpk.WriteShort(ERR_PT_INERR);
		return l_retpk;
	}

	uLong l_acctid = ply->m_acctid;
	if (!l_acctid || l_acctid != ply->m_acctid) {
		l_retpk.WriteShort(ERR_PT_INERR);
		return l_retpk;
	}

	RunChainGetArmor<Player> l(m_plylst);
	if (ply->m_bCheat) {
		if (m_dwCheatCount > 0)
			m_dwCheatCount--;
	}

	ply->EndPlay(datasock);

	if (ply->EndRun()) {
		if (ply->m_currcha != -1) {
			// Send the End Billing command
			WPacket	l_wpk = GetWPacket();
			l_wpk.WriteCmd(CMD_PA_USER_BILLEND);
			l_wpk.WriteString(ply->m_acctname.c_str());
			SendData(m_acctsock, l_wpk);
		}

		// Send the LogOut command to AccountServer
		WPacket	l_wpk = GetWPacket();
		l_wpk.WriteCmd(CMD_PA_USER_LOGOUT);
		l_wpk.WriteLong(ply->m_acctLoginID);
		l_wpk.WriteLong(ply->m_sessid);
		SendData(m_acctsock, l_wpk);

		// Log out successfully
		LogLine l_line(g_LogGrpServer);
		l_line << newln << "(" << ply->m_clientip << "):[" << ply->m_acctname << "]logout,\t online/total:" << m_plylst.GetTotal() << "/" << long(m_curChaNum);
	}
	else {
		LogLine l_line(g_LogGrpServer);
		l_line << newln << "(" << ply->m_clientip << "):[" << ply->m_acctname << "]logout,\t online/total:" << m_plylst.GetTotal() << "/" << long(m_curChaNum);
	}

	if (ply->m_bWG) {
		m_curWGChaNum--;
	}

	// Delete the list
	m_LoginList.Remove(ply->m_acctname);
	ply->Free();
	l.unlock();

	// Returns the success error code
	l_retpk.WriteShort(ERR_SUCCESS);
	return l_retpk;
}

WPacket	GroupServerApp::TP_BGNPLAY(Player *ply, DataSocket *datasock, RPacket &pk) {
	WPacket l_retpk = GetWPacket();
	if (ply->m_currcha >= 0) {

		// An internal error should be the value of Day
		l_retpk.WriteShort(ERR_PT_INERR);
	}
	else {
		uShort l_len;
		cChar* l_cha = pk.ReadString(&l_len);
		if (!l_cha || l_len > 16) {

			// An internal error should be the value of Day
			l_retpk.WriteShort(ERR_PT_INERR);
		}
		else {
			MutexArmor l_lockCha(ply->m_mtxCha);
			ply->m_bp_currcha = ply->FindIndexByChaName(l_cha);

			// This account does not have this role
			if (ply->m_bp_currcha < 0) {
				l_retpk.WriteShort(ERR_PT_INVALIDCHA);
			}
			else if (ply->m_password.length() <= 0) {
				l_retpk.WriteShort(ERR_PT_INVALID_PW2);
			}
			// Legal role
			else {
				MutexArmor l_lockDB(m_mtxDB);
				if (m_tblcharaters->FetchRowByChaID(ply->m_chaid[ply->m_bp_currcha]) < 1) {
					l_retpk.WriteShort(ERR_PT_INERR);
				}
				else {
					ply->m_guildPermission[ply->m_bp_currcha] = m_tblcharaters->GetGuildPermission();
					int	l_maxval = atoi(m_cfg["Database"]["MaxVal"]);
					if (l_maxval && (m_tblcharaters->GetStr() > l_maxval || m_tblcharaters->GetDex()>l_maxval || m_tblcharaters->GetAgi() > l_maxval || m_tblcharaters->GetCon() > l_maxval
						|| m_tblcharaters->GetSta() > l_maxval || m_tblcharaters->GetLuk() > l_maxval)) {
						l_retpk.WriteShort(ERR_PT_BADBOY);
						WPacket l_wpk = GetWPacket();
						l_wpk.WriteCmd(CMD_MC_SYSINFO);
						char l_buf[256];
						CFormatParameter param(2);
						param.setString(0, ply->m_acctname.c_str());
						param.setString(1, ply->m_chaname[ply->m_bp_currcha].c_str());
						RES_FORMAT_STRING(GP_GROUPSERVERAPPSERV_CPP_00002, param, l_buf);
						l_wpk.WriteString(l_buf);

						// MRF_SOURCE_LIMIT: Players can't exceed 10240
						Player *l_plylst[10240];
						short l_plynum = 0;
						RunChainGetArmor<Player> l(m_plylst);
						for (Player *l_plyr = m_plylst.GetNextItem(); l_plyr; l_plyr = m_plylst.GetNextItem()) {
							if (l_plyr->m_bp_currcha < 0)continue;
							l_plylst[l_plynum] = l_plyr;
							l_plynum++;
						}
						l.unlock();
						SendToClient(l_plylst, l_plynum, l_wpk);
						ply->m_currcha = -1;
					}
					else {
						short swiner = 0;

						// Returns success information
						l_retpk.WriteShort(ERR_SUCCESS);					// The role is successfully returned 
						l_retpk.WriteString(ply->m_password.c_str());
						l_retpk.WriteLong(ply->m_chaid[ply->m_bp_currcha]);	// Role ID
						l_retpk.WriteLong(ply->m_chaid[ply->m_bp_currcha]);	// World ID
						l_retpk.WriteString(m_tblcharaters->GetMap());		// The map name of the last offline
						for (int i = 0; i < MAXORDERNUM; i++) {
							if (ply->m_chaid[ply->m_bp_currcha] == m_tbLparam->GetOrderData(i)->nid) {
								swiner = i + 1;
								break;
							}
						}

						l_retpk.WriteShort(swiner);
						LogLine l_line(g_LogGrpServer);
						l_line << newln << "(" << ply->m_clientip << "):[" << ply->m_acctname << "]select char[" << l_cha << "]begin paly...，\t online/total:" << m_plylst.GetTotal() << "/" << long(m_curChaNum);
					}
				}
				l_lockDB.unlock();
			}
			l_lockCha.unlock();
		}
	}
	return l_retpk;
}

WPacket GroupServerApp::TP_ENDPLAY(Player *ply, DataSocket *datasock, RPacket &pk) {
	WPacket l_retpk = GetWPacket();
	if (ply->m_currcha < 0) {
		// A system inconsistency error at the expected value
		l_retpk.WriteShort(ERR_PT_INERR);
	}
	else {
		// Rerun role successfully returned
		l_retpk.WriteShort(ERR_SUCCESS);
		ply->EndPlay(datasock);

		LogLine l_line(g_LogGrpServer);
		l_line << newln << "(" << ply->m_clientip << "):[" << ply->m_acctname << "]exit char[" << ply->m_chaname[ply->m_currcha] << "]begin entry select char UI...，\t online/total:"
			<< m_plylst.GetTotal() << "/" << long(m_curChaNum);
		l_line << endln;

		// Set to invalid current role
		ply->m_currcha = -1;
		ply->EndPlayReset();

		// The End Billing command is sent
		WPacket	l_wpk = GetWPacket();
		l_wpk.WriteCmd(CMD_PA_USER_BILLEND);
		l_wpk.WriteString(ply->m_acctname.c_str());
		SendData(m_acctsock, l_wpk);

		// Remove the role table from the database
		GetCHAsFromDBByPlayer(ply, l_retpk);
	}
	return l_retpk;
}

void GroupServerApp::MP_ENTERMAP(Player *ply, DataSocket *datasock, RPacket &pk) {
	char l_isSwitch;
	if (!(l_isSwitch = pk.ReadChar()) && ply->m_currcha < 0) {
		MP_ONLINE(ply);
	}
	else if (l_isSwitch && ply->m_currcha >= 0) {
		MP_SWITCH(ply);
	}
}

void GroupServerApp::MP_ONLINE(Player *ply) {
	MutexArmor l_lockDB(m_mtxDB);
	ply->m_currcha = ply->m_bp_currcha;
	ply->m_bp_currcha = -1;
	m_tblcharaters->SetAddr(ply->m_chaid[ply->m_currcha], MakeULong(ply));
	++m_curChaNum;
	LogLine l_line(g_LogGrpServer);

	l_line << newln << "(" << ply->m_clientip << "):[" << ply->m_acctname << "] char [" << ply->m_chaname[ply->m_currcha] << "]entry map server begin play...，\t online/total:" << m_plylst.GetTotal() << "/" << long(m_curChaNum);
	l_line << endln;

	// Send the start accounting command
	WPacket	l_wpk = GetWPacket();
	l_wpk.WriteCmd(CMD_PA_USER_BILLBGN);
	l_wpk.WriteString(ply->m_acctname.c_str());
	l_wpk.WriteString(ply->m_passport.c_str());
	SendData(m_acctsock, l_wpk);

	ply->CheckEstop();

	PC_FRND_INIT(ply);
	PC_GULD_INIT(ply);
	PC_MASTER_INIT(ply);
}

WPacket	GroupServerApp::TP_NEWCHA(Player *ply, DataSocket *datasock, RPacket &pk) {
	WPacket l_retpk = GetWPacket();
	if (ply->m_currcha >= 0) {

		// A system inconsistency error at the expected value
		l_retpk.WriteShort(ERR_PT_INERR);
		return l_retpk;
	}
	MutexArmor l_lockCha(ply->m_mtxCha);

	// The system has reached the maximum number of characters11
	if (ply->m_chanum >= const_cha.MaxChaNum) {
		l_retpk.WriteShort(ERR_PT_TOMAXCHA);
		return l_retpk;
	}

	uShort l_len;
	cChar *l_chaname = pk.ReadString(&l_len);
	if (!l_chaname) {
		l_retpk.WriteShort(ERR_PT_INERR);
		return l_retpk;
	}
	else if (l_len > 16) {
		l_retpk.WriteShort(ERR_PT_TOOBIGCHANM);
		return l_retpk;
	}
	// Role names can not have single quotation marks'
	else if (!IsValidName(l_chaname, l_len) || !CTextFilter::IsLegalText(CTextFilter::NAME_TABLE, l_chaname)) {
		// Role name is invalid
		l_retpk.WriteShort(ERR_PT_ERRCHANAME);
		return l_retpk;
	}

	/*
	cChar* l_birth = pk.ReadString(&l_len);
	dstring	l_birth1 = "";
	cChar* l_birth2 = 0;
	if (l_birth && l_len < 30 && !strchr(l_birth, '/')) {
	l_birth1 = dstring("/") << l_birth << "/";
	try {
	l_birth2 = this->m_cfg["Character"]["place of birth"];
	}
	catch (...)	{
	l_birth2 = 0;
	}
	}
	else {
	l_birth2 = 0;
	}
	if (!l_birth2 || !strstr(l_birth2, l_birth1)) {
	// Place of birth is illegal
	l_retpk.WriteShort(ERR_PT_INVALIDBIRTH);
	}
	else {
	*/

	cChar* l_birth = pk.ReadString(&l_len);
	dstring	 l_birth1 = "";
	cChar * l_map = 0;
	if (l_birth && l_len < 30 && !strchr(l_birth, '/')) {
		l_birth1 = l_birth;
		try {
			map<string, string>::iterator it = m_mapBirthplace.find(string(l_birth));
			if (it != m_mapBirthplace.end()) {
				l_map = it->second.c_str();
			}
		}
		catch (...) {
			l_map = 0;
		}
	}
	else {
		l_map = 0;
	}

	if (!l_map) {
		// Place of birth is illegal
		l_retpk.WriteShort(ERR_PT_INVALIDBIRTH);
		return l_retpk;
	}
	char tmp[100];
	const LOOK	*look = reinterpret_cast<const LOOK*>(pk.ReadSequence(l_len));
	if (!look || l_len != sizeof(LOOK)) {
		sprintf_s(tmp, "%d sizeof(%d)", l_len, sizeof(LOOK));
		MessageBoxA(0, tmp, "", 0);
		l_retpk.WriteShort(ERR_PT_INVALIDDAT);
		return l_retpk;
	}

	char l_look[defLOOK_DATA_STRING_LEN];
	try {
		if (!LookData2String(look, l_look, defLOOK_DATA_STRING_LEN)) {
			l_retpk.WriteShort(ERR_PT_INVALIDDAT);
			return l_retpk;
		}
	}
	catch (...) {
		LogLine l_line(g_LogGrpServer);
		l_line << newln << "account[" << ply->m_acctname << "]new char[" << l_chaname << "]find appreance data convert exception" << endln;
		l_retpk.WriteShort(ERR_PT_INVALIDDAT);
		return l_retpk;
	}

	MutexArmor l_lockDB(m_mtxDB);

	// Duplicate role names
	if (!m_tblcharaters->InsertRow(l_chaname, ply->m_acctid, l_birth, l_map, l_look)) {
		LogLine l_line(g_LogGrpServer);
		l_line << newln << "account[" << ply->m_acctname << "]new char[" << l_chaname << "]the same name exception" << endln;
		l_retpk.WriteShort(ERR_PT_SAMECHANAME);
		return l_retpk;
	}

	l_retpk.WriteShort(ERR_SUCCESS);

	// Update the role ID list CharIDs for the account in the database
	if (m_tblcharaters->FetchRowByChaName(l_chaname) < 1) {
		l_retpk.WriteShort(ERR_PT_INERR);
		LogLine l_line(g_LogGrpServer);
		l_line << newln << "account[" << ply->m_acctname << "] when create char , can't restore database failed.";
		return l_retpk;
	}

	ply->m_chaid[ply->m_chanum] = m_tblcharaters->Getcha_id();
	ply->m_chaname[ply->m_chanum] = l_chaname;
	ply->m_motto[ply->m_chanum] = m_tblcharaters->GetMottonByName();
	ply->m_icon[ply->m_chanum] = m_tblcharaters->GetIconByName();
	ply->m_guild[ply->m_chanum] = 0;
	ply->m_chanum++;

	char l_CharIDs[80];	l_CharIDs[0] = 0;
	char l_buf[20];
	for (char i = 0; i < ply->m_chanum; i++) {
		_itoa_s(ply->m_chaid[i], l_buf, sizeof(l_buf), 10);
		strncat_s(l_CharIDs, sizeof(l_CharIDs), l_buf, _TRUNCATE);
		strncat_s(l_CharIDs, sizeof(l_CharIDs), ";", _TRUNCATE);
	}
	m_tblaccounts->UpdateRow(ply->m_acctid, l_CharIDs);
	l_lockDB.unlock();
	LogLine l_line(g_LogGrpServer);
	l_line << newln << "(" << ply->m_clientip << "):[" << ply->m_acctname << "]create char [" << l_chaname << "]，\t online/total:" << m_plylst.GetTotal() << "/" << long(m_curChaNum) << endln;
	l_lockCha.unlock();
	return l_retpk;
}

WPacket	GroupServerApp::TP_DELCHA(Player *ply, DataSocket *datasock, RPacket &pk) {
	WPacket		l_retpk = GetWPacket();
	if (ply->m_currcha >= 0) {
		
		// A system inconsistency error at the expected value
		l_retpk.WriteShort(ERR_PT_INERR);
	}
	else {
		uShort l_len, l_len2;
		cChar* l_cha = pk.ReadString(&l_len);
		cChar* l_pw2 = pk.ReadString(&l_len2);
		if (!l_cha || l_len > 16) {
			l_retpk.WriteShort(ERR_PT_INERR);
		}
		else if (strcmp(l_pw2, ply->m_password.c_str()) != 0) {
			l_retpk.WriteShort(ERR_PT_INVALID_PW2);
		}
		else {
			MutexArmor l_lockCha(ply->m_mtxCha);
			char l_chaidx = ply->FindIndexByChaName(l_cha);
			
			// This account does not have this role
			if (l_chaidx < 0) {
				l_retpk.WriteShort(ERR_PT_INVALIDCHA);
			}
			// Legitimate role
			else if (FindGuildByLeadID(ply->m_chaid[l_chaidx])) {
				l_retpk.WriteShort(ERR_PT_ISGLDLEADER);
			}
			/*
#ifdef SHUI_JING
			else if (m_tblcrystal->IsHasBuyorSale(ply->m_chaid[l_chaidx], 0) || m_tblcrystal->IsHasBuyorSale(ply->m_chaid[l_chaidx], 1)) {
				l_retpk.WriteShort(ERR_PT_HASCRYSTALTRADE);
			}
#endif	*/

			else {
				bool bFlag = false;
				int	l_chaid = ply->m_chaid[l_chaidx];
				char l_CharIDs[80]; l_CharIDs[0] = 0;
				char l_buf[20];
				for (char i = 0; i < ply->m_chanum; i++) {
					if (i == l_chaidx) 
						continue;
					
					_itoa_s(ply->m_chaid[i], l_buf, sizeof(l_buf), 10);
					strncat_s(l_CharIDs, sizeof(l_CharIDs), l_buf, _TRUNCATE);
					strncat_s(l_CharIDs, sizeof(l_CharIDs), ";", _TRUNCATE);
				}

				MutexArmor l_lockDB(m_mtxDB);

				// Judge whether the guild members of the role, or delete the guild list information
				if (m_tblaccounts->begin_tran()) {
					if (!m_tblaccounts->UpdateRow(ply->m_acctid, l_CharIDs) || !m_tblcharaters->BackupRow(l_chaid) || !m_tblaccounts->commit_tran()) {
						m_tblaccounts->rollback();
					}
					else {
						bFlag = true;
					}
				}
				l_lockDB.unlock();

				if (bFlag) {
					// Delete role successfully returned
					l_retpk.WriteShort(ERR_SUCCESS);
					ply->m_chanum--;
					for (; l_chaidx < ply->m_chanum; l_chaidx++) {
						ply->m_chaid[l_chaidx] = ply->m_chaid[l_chaidx + 1];
						ply->m_chaname[l_chaidx] = ply->m_chaname[l_chaidx + 1];
						ply->m_motto[l_chaidx] = ply->m_motto[l_chaidx + 1];
						ply->m_icon[l_chaidx] = ply->m_icon[l_chaidx + 1];
						ply->m_guild[l_chaidx] = ply->m_guild[l_chaidx + 1];
					}

					LogLine l_line(g_LogGrpServer);
					l_line << newln << "(" << ply->m_clientip << "):[" << ply->m_acctname << RES_STRING(GP_GROUPSERVERAPPSERV_CPP_00043) << l_cha << RES_STRING(GP_GROUPSERVERAPPSERV_CPP_00044) << m_plylst.GetTotal() << "/" << long(m_curChaNum) << endln;
				}
				else {
					// The server is busy, please wait
					l_retpk.WriteShort(ERR_PT_SERVERBUSY);
					LogLine l_line(g_LogGrpServer);
					l_line << newln << "(" << ply->m_clientip << "):[" << ply->m_acctname << "]delete char failed[" << l_cha << "]，\t GetTotal/m_curChaNum:" << m_plylst.GetTotal() << "/" << long(m_curChaNum) << endln;
				}
			}
			l_lockCha.unlock();
		}
	}
	return l_retpk;
}

WPacket	GroupServerApp::TP_CREATE_PASSWORD2(Player *ply, DataSocket *datasock, RPacket &pk) {
	WPacket l_retpk = GetWPacket();
	std::string strPassword = pk.ReadString();

	if (ply->m_password.length() == 0 && strPassword.length() <= ROLE_MAXSIZE_PASSWORD2) {
		MutexArmor l_lockDB(m_mtxDB);
		if (m_tblaccounts->UpdatePassword(ply->m_acctid, strPassword.c_str())) {
			ply->m_password = strPassword;
			l_retpk.WriteShort(ERR_SUCCESS);
			LogLine l_line(g_LogErrServer);
			l_line << newln << "account:" << ply->m_acctname << "ID:" << ply->m_acctid << "database operate success:(" << strPassword.c_str() << ")" << endln;
		}
		else {
			l_retpk.WriteShort(ERR_PT_SERVERBUSY);
			LogLine l_line(g_LogErrServer);
			l_line << newln << "account:" << ply->m_acctname << "ID:" << ply->m_acctid << "database operate failed:(" << strPassword.c_str() << ")" << endln;
		}
	}
	else {
		l_retpk.WriteShort(ERR_PT_INVALID_PW2);
		LogLine l_line(g_LogErrServer);
		l_line << newln << "account:" << ply->m_acctname << "ID:" << ply->m_acctid << "wrong second password:(" << strPassword.c_str() << ")" << endln;
	}

	return l_retpk;
}

WPacket GroupServerApp::TP_UPDATE_PASSWORD2(Player *ply, DataSocket *datasock, RPacket &pk) {
	WPacket l_retpk = GetWPacket();
	std::string strOld = pk.ReadString();
	std::string strPassword = pk.ReadString();

	if (ply->m_password == strOld && strPassword.length() <= ROLE_MAXSIZE_PASSWORD2) {
		MutexArmor l_lockDB(m_mtxDB);
		if (m_tblaccounts->UpdatePassword(ply->m_acctid, strPassword.c_str())) {
			ply->m_password = strPassword;
			l_retpk.WriteShort(ERR_SUCCESS);
		}
		else {
			l_retpk.WriteShort(ERR_PT_SERVERBUSY);
		}
	}
	else {
		l_retpk.WriteShort(ERR_PT_INVALID_PW2);
	}
	return l_retpk;
}

void GroupServerApp::MP_GARNER2_UPDATE(Player *ply, DataSocket *datasock, RPacket &pk) {
	ORDERINFO orderinfo;
	orderinfo.nid = pk.ReadLong();
	string strChaName = pk.ReadString();
	orderinfo.nlev = pk.ReadLong();
	string strjob = pk.ReadString();
	short sFightpoint = pk.ReadShort();

	if (strChaName.length() > 20) {
		LogLine	l_line(g_LogGarner2);
		l_line << newln << "order data exception";
		return;
	}
	strncpy_s(orderinfo.strname, sizeof(orderinfo.strname), strChaName.c_str(), _TRUNCATE);

	if (strjob.length() > 100) {
		LogLine	l_line(g_LogGarner2);
		l_line << newln << "order data exception";
		return;
	}
	strncpy_s(orderinfo.strjob, sizeof(orderinfo.strjob), strjob.c_str(), _TRUNCATE);
	orderinfo.nfightpoint = (long)sFightpoint;

	MutexArmor l_lockDB(m_mtxDB);
	m_tbLparam->UpdateOrder(orderinfo);
	CP_GARNER2_GETORDER(ply, datasock, pk);
}

void GroupServerApp::CP_GARNER2_GETORDER(Player *ply, DataSocket *datasock, RPacket &pk) {
	WPacket l_wpk = g_gpsvr->GetWPacket();
	ORDERINFO* porder;
	l_wpk.WriteCmd(CMD_PC_GARNER2_ORDER);
	for (int i = 0; i < MAXORDERNUM; i++) {
		porder = m_tbLparam->GetOrderData(i);
		l_wpk.WriteString(porder->strname);
		l_wpk.WriteLong(porder->nlev);
		l_wpk.WriteString(porder->strjob);
		l_wpk.WriteLong(porder->nfightpoint);
	}
	g_gpsvr->SendToClient(ply, l_wpk);

}

void GroupServerApp::MP_GM_BANACCOUNT(Player *ply, DataSocket *datasock, RPacket &pk) {
	string actName = pk.ReadString();
	WPacket l_wpk = g_gpsvr->GetWPacket();
	l_wpk.WriteCmd(CMD_PA_GMBANACCOUNT);
	l_wpk.WriteString(actName.c_str());
	g_gpsvr->SendData(g_gpsvr->m_acctsock, l_wpk);
}