#include "gateserver.h"
#include <algorithm>

ToGameServer::ToGameServer(char const* fname, ThreadPool* proc, ThreadPool* comm) : TcpServerApp(this, proc, comm), RPCMGR(this), _mut_game(), _game_heap(1, 20), _game_list(NULL), _map_game() {
	_mut_game.Create(false); _game_num = 0;

	// Start listening
	IniFile inf(fname);
	IniSection& is = inf["ToGameServer"];
	cChar* ip = is["IP"]; uShort port = atoi(is["Port"]);

	// Starts the PING thread
	SetPKParse(0, 2, 64 * 1024, 400); BeginWork(atoi(is["EnablePing"]));
	if (OpenListenSocket(port, ip) != 0)
		THROW_EXCP(excp, "ToGameServer listen failed");
}

ToGameServer::~ToGameServer() {
	ShutDown(12 * 1000);
}

void ToGameServer::_add_game(GameServer* game) {
	game->next = _game_list;
	_game_list = game;
	++_game_num;
}

bool ToGameServer::_exist_game(char const* game) {
	GameServer* curr = _game_list;
	bool exist = false;
	while (curr) {
		if (curr->gamename == game) { exist = true; break; }
		curr = curr->next;
	}
	return exist;
}

void ToGameServer::_del_game(GameServer* game) {
	GameServer* curr = _game_list;
	GameServer* prev = 0;
	while (curr) {
		if (curr == game) 
			break;
		prev = curr; curr = curr->next;
	}

	if (curr) {
		if (prev)
			prev->next = curr->next;
		else
			_game_list = curr->next;
		--_game_num;
	}
}

bool ToGameServer::OnConnect(DataSocket* datasock) {
	datasock->SetPointer(0);
	datasock->SetRecvBuf(64 * 1024);
	datasock->SetSendBuf(64 * 1024);
	LogLine l_line(g_gatelog);
	l_line << newln << "GameServer= [" << datasock->GetPeerIP() << "] come,Socket num= " << GetSockTotal() + 1;
	return true;
}

// reason value: 0 - local program exits normally; - the network is shut down by the peer; - 1-Socket error; - the packet length exceeds the limit.
void ToGameServer::OnDisconnect(DataSocket* datasock, int reason) {
	LogLine l_line(g_gatelog);
	l_line << newln << "GameServer= [" << datasock->GetPeerIP() << "] gone,Socket num= " << GetSockTotal() + 1 << ",reason= " << GetDisconnectErrText(reason).c_str();
	l_line << endln;
	if (reason == DS_SHUTDOWN || reason == DS_DISCONN) 
		return;
	
	GameServer* l_game = (GameServer *)(datasock->GetPointer());
	bool already_delete = false;
	if (l_game == NULL)
		return;

	// Remove this GameServer from the linked list
	_mut_game.lock();
	try {
		l_game = (GameServer *)(datasock->GetPointer());
		if (l_game != NULL) {
			l_game->DeleteGameServer(reason);
			l_line << newln << " delete [" << l_game->gamename.c_str() << "]" << endln;
			_del_game(l_game);
			for (int i = 0; i < l_game->mapcnt; ++i) {
				l_line << newln << "delete map [" << l_game->maplist[i].c_str() << "]" << endln;
				_map_game.erase(l_game->maplist[i]);
			}
			l_game->mapcnt = 0;
			l_game->Free();
			datasock->SetPointer(NULL);
		}
		else {
			already_delete = true;
		}
	}
	catch (...) {
		l_line << newln << "Exception raised from OnDisconnect{delete GameServer from list}" << endln;
	}
	_mut_game.unlock();

	if (already_delete) 
		return;

	try {

		// Notify all users who connect to this GameServer through this GateServer: The map server is down
		RPacket retpk = g_gtsvr->gp_conn->get_playerlist();

		// The number of players on this GateServer
		uShort ply_cnt = retpk.ReverseReadShort();

		Player* ply_addr; uLong db_id;
		Player** ply_array = new Player*[ply_cnt];
		uShort l_notcount = 0;
		for (uShort i = 0; i < ply_cnt; ++i) {
			ply_addr = (Player *)MakePointer(retpk.ReadLong());
			db_id = (uLong)retpk.ReadLong();

			if (ply_addr == NULL)
				continue;

			if (l_game == ply_addr->game) {
				ply_array[i - l_notcount] = ply_addr;
			}
			else {
				l_notcount++;
				continue;
			}
		
			// Secondary confirmation
			try {
				uLong tmp_id = ply_addr->m_dbid;
				
				// This role is offline 
				if (tmp_id != db_id)
					continue;
			}

			// An exception is raised, indicating that this role is also no longer online 
			catch (...) {
				l_line << newln << "Exception raised from OnDisconnect{uLong tmp_id = ply_addr->m_dbid}" << endln;
				continue;
			}

			try {

				// This user is still online and sends a map server failure message
				g_gtsvr->cli_conn->post_mapcrash_msg(ply_addr); 
			}
			catch (...) {
				l_line << newln << "Exception raised from OnDisconnect{cli_conn->post_mapcrash_msg(ply_addr)}" << endln;
				continue;
			}
			continue;
		}

		ply_cnt -= l_notcount;
		l_line << newln << "becaulse GameServer trouble, notice " << ply_cnt << " user offline" << endln;
		for (uShort i = 0; i < ply_cnt; ++i) {

			// Immediately disconnect the connection
			try {
				l_line << newln << "because GameServer trouble, disconnect [" << ply_array[i]->m_datasock->GetPeerIP() << "] " << endln;
				g_gtsvr->cli_conn->Disconnect(ply_array[i]->m_datasock, 100, -29);
			}
			catch (...) {
				l_line << newln << "Exception raised from OnDisconnect{Disconnect}" << endln;
			}
		}
		delete[] ply_array;
	}
	catch (...) {
		l_line << newln << "Exception raised from OnDisconnect{exit}" << endln;
	}
}

WPacket ToGameServer::OnServeCall(DataSocket* datasock, RPacket &in_para) {
	return NULL;
}

void ToGameServer::SendAllGameServer(WPacket &in_para) {
	vector<GameServer*> mapV;
	for (map<string, GameServer*>::iterator it = _map_game.begin(); it != _map_game.end(); it++) {
		if (it->second) {
			vector<GameServer*>::iterator mapIt = std::find(mapV.begin(), mapV.end(), it->second);
			if (mapIt == mapV.end()) {
				if (it->second->m_datasock)
					this->SendData(it->second->m_datasock, in_para);
				mapV.push_back(it->second);
			}
		}
	}
}

void ToGameServer::OnProcessData(DataSocket* datasock, RPacket &recvbuf) {
	GameServer* l_game = (GameServer *)(datasock->GetPointer());
	uShort l_cmd = recvbuf.ReadCmd();
	try {
		l_cmd = recvbuf.ReadCmd();

		switch (l_cmd) {
		case CMD_MT_LOGIN:
			MT_LOGIN(datasock, recvbuf);
			break;

		case CMD_MT_SWITCHMAP: {
			RPacket	l_rpk = recvbuf;

			// L_aimnum is always equal to one
			uShort	l_aimnum = l_rpk.ReverseReadShort();

			Player *l_ply = (Player *)MakePointer(l_rpk.ReverseReadLong());
			
			// ChaID
			if (l_ply->m_dbid != l_rpk.ReverseReadLong()) {
				break;
			}

			uChar l_return = l_rpk.ReverseReadChar();
			recvbuf.DiscardLast(sizeof(uChar) + sizeof(uLong) * 2 * l_aimnum + sizeof(uShort));

			cChar *l_srcmap = l_rpk.ReadString();
			Long lSrcMapCopyNO = l_rpk.ReadLong();
			
			// coordinates
			uLong l_srcx = l_rpk.ReadLong();
			uLong l_srcy = l_rpk.ReadLong();

			cChar *l_map = l_rpk.ReadString();
			Long lMapCopyNO = l_rpk.ReadLong();
			
			// coordinates
			uLong l_x = l_rpk.ReadLong();
			uLong l_y = l_rpk.ReadLong();

			GameServer *l_game = g_gtsvr->gm_conn->find(l_map);

			LogLine l_line(g_gatelog);
			if (l_game) {
				l_ply->game->m_plynum--;
				l_ply->game = 0;
				l_ply->gm_addr = 0;
				l_ply->m_switch = 1;
				l_line << newln << "clinet: " << l_ply->m_datasock->GetPeerIP() << ":" << l_ply->m_datasock->GetPeerPort() << "	Switch to map,to Gate[" << l_game->m_datasock->GetPeerIP() << "]send EnterMap command,dbid:" << l_ply->m_dbid
					   << uppercase << hex << ",Gate address:" << MakeULong(l_ply) << dec << nouppercase << endln;

				// According to the map to find GameServer, and then request GameServer to enter this map.
				l_game->EnterMap(l_ply, l_ply->m_loginID, l_ply->m_dbid, l_ply->m_worldid, l_map, lMapCopyNO, l_x, l_y, 1, l_ply->m_sGarnerWiner);
				l_game->m_plynum++;
			}

			// The destination map is unreachable and re-enters the source map
			else if (!l_return) {
				WPacket	l_wpk = datasock->GetWPacket();
				l_wpk.WriteCmd(CMD_MC_SYSINFO);
				l_wpk.WriteString(dstring("[") << l_map << "] can't reach, pealse retry later!");
				l_ply->m_datasock->SendData(l_wpk);
				l_line << newln << "clinet: " << l_ply->m_datasock->GetPeerIP() << ":" << l_ply->m_datasock->GetPeerPort() << "	Switch back map,to Gate[" << l_ply->game->m_datasock->GetPeerIP() << "]send EnterMap command,dbid:" << l_ply->m_dbid
					<< uppercase << hex << ",Gate address:" << MakeULong(l_ply) << dec << nouppercase << endln;

				// According to the map to find GameServer, and then request GameServer to enter this map.
				l_ply->game->EnterMap(l_ply, l_ply->m_loginID, l_ply->m_dbid, l_ply->m_worldid, l_srcmap, lSrcMapCopyNO, l_srcx, l_srcy, 1, l_ply->m_sGarnerWiner);
			}
			else {
				g_gtsvr->cli_conn->Disconnect(l_ply->m_datasock, 0, -24);
			}
			break;
		}
		case CMD_CM_SKILLUPGRADE: {
			RPacket l_rpk = recvbuf;
			Short	sSkillID = l_rpk.ReadShort();
			Char	chAddGrade = l_rpk.ReadChar();
			if (sSkillID > 500)
			{
				break;
			}
			break;
		}
		case CMD_MC_ENTERMAP: {
			RPacket	l_rpk = recvbuf;

			// L_aimnum is always equal to one
			uShort l_aimnum = l_rpk.ReverseReadShort();

			Player *l_ply = (Player *)MakePointer(l_rpk.ReverseReadLong());
			if (l_ply == NULL)
				break;

			uLong l_dbid = l_rpk.ReverseReadLong();
			LogLine l_line(g_gatelog);

			// ChaID
			if (l_ply->m_dbid != l_dbid) {
				l_line << newln << "recieve from [" << datasock->GetPeerIP() << "] EnterMap command ,can't match DBID:locale [" << l_ply->m_dbid << "],far[" << l_dbid << "]" << uppercase << hex << ",Gate address:" << MakeULong(l_ply) << endln;
				return;
			}

			uShort	l_retcode = l_rpk.ReadShort();
			if (l_retcode == ERR_SUCCESS) {
				l_ply->game = l_game;
				l_ply->gm_addr = l_rpk.ReverseReadLong();
				l_game->m_plynum = l_rpk.ReverseReadLong();
				char l_isSwitch = l_rpk.ReverseReadChar();
				l_ply->m_switch = 0;

				l_line << newln << "clinet: " << l_ply->m_datasock->GetPeerIP() << ":" << l_ply->m_datasock->GetPeerPort() << "	recieve Gate  from [" << datasock->GetPeerIP() << "]success EnterMap command,Game address:"
					   << uppercase << hex << l_ply->gm_addr << ",Gate address:" << MakeULong(l_ply) << dec << nouppercase << endln;

				recvbuf.DiscardLast(sizeof(uShort) + sizeof(uLong) * 2 * l_aimnum + sizeof(uLong) * 2 + sizeof(uChar));
				g_gtsvr->cli_conn->SendData(l_ply->m_datasock, recvbuf); {
					WPacket l_wpk = GetWPacket();
					l_wpk.WriteCmd(CMD_MP_ENTERMAP);
					l_wpk.WriteChar(l_isSwitch);
					l_wpk.WriteLong(MakeULong(l_ply));
					l_wpk.WriteLong(l_ply->gp_addr);
					g_gtsvr->gp_conn->SendData(g_gtsvr->gp_conn->get_datasock(), l_wpk);
				}

			}
			else {
				l_ply->m_switch = 0;
				l_ply->m_status = 1;
				l_game->m_plynum--;
				l_line << newln << "clinet: " << l_ply->m_datasock->GetPeerIP() << ":" << l_ply->m_datasock->GetPeerPort() << "	Gate recieve from [" << datasock->GetPeerIP() << "]failed EnterMap command ,Error:"  << l_retcode << endln;
				recvbuf.DiscardLast(sizeof(uShort) + sizeof(uLong) * 2 * l_aimnum);
				g_gtsvr->cli_conn->Disconnect(l_ply->m_datasock, 10, -33);
			}
			break;
		}
		case CMD_MT_KICKUSER: {
			uShort	l_aimnum = recvbuf.ReverseReadShort();
			for (uShort i = 0; i < l_aimnum; i++) {
				Player *l_ply = (Player *)MakePointer(recvbuf.ReverseReadLong());
				if (l_ply && l_ply->m_dbid == recvbuf.ReverseReadLong()) {

					// -23 The error code indicates that GameServer requires that someone be kicked off
					g_gtsvr->cli_conn->Disconnect(l_ply->m_datasock, 0, -23);
				}
			}
			break;
		}
		/*case CMD_MC_PORTALTIMES﻿: {
			RPacket rpk = recvbuf;
			Player *player = (Player *)MakePointer(recvbuf.ReverseReadLong());
			rpk.DiscardLast(sizeof(sizeof(uLong)));
			auto wpk = WPacket(rpk).Duplicate();
			g_gtsvr->cli_conn->SendData(player->m_datasock, wpk);
		} break;*/
		case CMD_MT_MAPENTRY: {
			WPacket l_wpk = WPacket(recvbuf).Duplicate();
			RPacket	l_rpk = recvbuf;
			cChar *l_map = l_rpk.ReadString();

			if (l_map == NULL)
				break;

			GameServer *l_game = g_gtsvr->gm_conn->find(l_map);
			if (l_game) {
				LG("ToGameServer", "CMD_MT_MAPENTRY = %s\n", l_map);
				l_wpk.WriteCmd(CMD_TM_MAPENTRY);
				g_gtsvr->gm_conn->SendData(l_game->m_datasock, l_wpk);
			}
			else {
				l_wpk.WriteCmd(CMD_TM_MAPENTRY_NOMAP);
				g_gtsvr->gm_conn->SendData(datasock, l_wpk);
			}
		} 
		break;
		
		case CMD_MT_STATE: {
			RPacket	l_rpk = recvbuf;

			Player *l_ply = (Player *)MakePointer(l_rpk.ReverseReadLong());
			if (l_ply == NULL)
				break;

			uLong l_dbid = l_rpk.ReverseReadLong();

			// ChaID
			if (l_ply->m_dbid == l_dbid) {
				WPacket	l_wpk = datasock->GetWPacket();
				l_wpk.WriteCmd(CMD_MC_SYSINFO);
				l_wpk.WriteString(l_rpk.ReadString());
				l_ply->m_datasock->SendData(l_wpk);
			}
		}
		break;

		case CMD_MT_MAP_ADMIN: {

			// Is always 1
			uShort	l_aimnum = recvbuf.ReverseReadShort();

			Player *l_ply = (Player *)MakePointer(recvbuf.ReverseReadLong());

			// ChaID
			if (l_ply->m_dbid != recvbuf.ReverseReadLong()) {
				LG("ToGameServer", "CMD_MT_MAP_ADMIN error id\n");
				break;
			}

			WPacket l_wpk = WPacket(recvbuf).Duplicate();
			RPacket	l_rpk = recvbuf;

			// The map name of the operation
			cChar *l_map = l_rpk.ReadString();

			if (l_map == NULL) {
				LG("ToGameServer", "CMD_MT_MAP_ADMIN error map\n");
				break;
			}

			GameServer *l_game = g_gtsvr->gm_conn->find(l_map);
			if (l_game) {
				LG("ToGameServer", "CMD_MT_MAP_ADMIN = %s\n", l_map);
				l_wpk.WriteCmd(CMD_TM_MAP_ADMIN);
				g_gtsvr->gm_conn->SendData(l_game->m_datasock, l_wpk);
			}
			else {
				WPacket	l_wpk = datasock->GetWPacket();
				l_wpk.WriteCmd(CMD_MC_SYSINFO);
				l_wpk.WriteString(dstring("[") << l_map << "] can't find!");
				l_ply->m_datasock->SendData(l_wpk);
			}
		}
		break;

		// Default Forwarding
		default: {
			if (l_cmd / 500 == CMD_MC_BASE / 500) {
				RPacket	l_rpk = recvbuf;
				uShort	l_aimnum = l_rpk.ReverseReadShort();
				recvbuf.DiscardLast(sizeof(uLong) * 2 * l_aimnum + sizeof(uShort));
				for (uShort i = 0; i < l_aimnum; i++) {
					Player *l_ply = (Player *)MakePointer(l_rpk.ReverseReadLong());
					if (l_ply->m_dbid == l_rpk.ReverseReadLong()) {
						g_gtsvr->cli_conn->SendData(l_ply->m_datasock, recvbuf);
					}
				}
			}
			else if (l_cmd / 500 == CMD_MP_BASE / 500) {
				RPacket l_rpk = recvbuf;
				uShort l_aimnum = l_rpk.ReverseReadShort();
				recvbuf.DiscardLast(sizeof(uLong) * 2 * l_aimnum + sizeof(uShort));
				if (l_aimnum > 0) {
					WPacket l_wpk, l_wpk0 = WPacket(recvbuf).Duplicate();
					for (uShort i = 0; i < l_aimnum; i++) {
						Player *l_ply = (Player *)MakePointer(l_rpk.ReverseReadLong());
						if (l_ply->m_dbid == l_rpk.ReverseReadLong()) {
							l_wpk = l_wpk0;
							l_wpk.WriteLong(MakeULong(l_ply));
							l_wpk.WriteLong(l_ply->gp_addr);
							g_gtsvr->gp_conn->SendData(g_gtsvr->gp_conn->get_datasock(), l_wpk);
						}
					}
				}
				else {
					WPacket l_wpk = WPacket(recvbuf).Duplicate();
					g_gtsvr->gp_conn->SendData(g_gtsvr->gp_conn->get_datasock(), l_wpk);
				}
			}
			else if (l_cmd / 500 == CMD_MM_BASE / 500) {
				for (GameServer *l_game = _game_list; l_game; l_game = l_game->next) {
					g_gtsvr->gm_conn->SendData(l_game->m_datasock, recvbuf);
				}
			}
			break;
		}
		}
	}
	catch (...) {
		LG("ToGameServerError", "l_cmd = %d\n", l_cmd);
	}
}

void ToGameServer::MT_LOGIN(DataSocket* datasock, RPacket& rpk) {
	_mut_game.lock();

	cChar* gms_name = rpk.ReadString();
	cChar* map_list = rpk.ReadString();
	GameServer* gms = _game_heap.Get();

	if (gms == NULL) {
		LG("ToGameServerError", "GameServer == NULL\n");

		WPacket retpk = GetWPacket();
		retpk.WriteCmd(CMD_TM_LOGIN_ACK);
		retpk.WriteShort(ERR_TM_MAPERR);
		datasock->SetPointer(NULL);
		_mut_game.unlock();

		SendData(datasock, retpk);

		return;
	}

	gms->m_plynum = 0;
	gms->gamename = "";
	gms->ip = "";
	gms->port = 0;
	gms->m_datasock = NULL;
	gms->next = NULL;

	for (int i = 0; i < MAX_MAP; i++)
		gms->maplist[i] = "";
	
	gms->mapcnt = 0;

	WPacket retpk = GetWPacket();
	bool valid = true;
	int i;

	retpk.WriteCmd(CMD_TM_LOGIN_ACK);
	int cnt = Util_ResolveTextLine(map_list, gms->maplist, MAX_MAP, ';', 0);
	LogLine l_line(g_gatelog);

	l_line << newln << "recieve GameServer [" << gms_name << "] map string [" << map_list << "] total [" << cnt << "]" << endln;
	
	// MAP string syntax error
	if (cnt <= 0) {
		l_line << newln << "map string [" << map_list << "] has syntax mistake, please use ';'compart" << endln;
		retpk.WriteShort(ERR_TM_MAPERR);
		datasock->SetPointer(NULL);
		gms->Free();
	}
	else {
		gms->gamename = gms_name;
		gms->mapcnt = cnt;
		try {
			do {

				// First check the GameServer name is registered
				if (_exist_game(gms_name)) {
					l_line << newln << "the same name GameServer exsit: " << gms_name << endln;
					retpk.WriteShort(ERR_TM_OVERNAME);
					datasock->SetPointer(NULL);
					valid = false; break;
				}

				// Second, check the map name will be repeated
				for (i = 0; i < cnt; ++i) {
					if (find(gms->maplist[i].c_str()) != NULL) {
						l_line << newln << "the same name MAP exsit: " << gms->maplist[i].c_str() << endln;
						retpk.WriteShort(ERR_TM_OVERMAP);
						datasock->SetPointer(NULL);
						valid = false;
						break;
					}
				}
			} while (false);

			// legal GameServer, added to the table
			if (valid) {

				// Add to the list
				_add_game(gms);
				
				l_line << newln << "add GameServer [" << gms_name << "] ok" << endln;
				
				// Add map
				for (i = 0; i < cnt; ++i) {
					l_line << newln << "add [" << gms_name << "]  [" << gms->maplist[i].c_str() << "] map ok" << endln;
					_map_game[gms->maplist[i]] = gms;
				}

				datasock->SetPointer(gms);
				gms->m_datasock = datasock;
				retpk.WriteShort(ERR_SUCCESS);
				retpk.WriteString(g_gtsvr->gp_conn->_myself.c_str());
			}

			// Illegal GateServer
			else {
				gms->Free();
			}
		}
		catch (...) {
			l_line << newln << "Exception raised from MT_LOGIN{add map}" << endln;
		}
	}
	_mut_game.unlock();
	SendData(datasock, retpk);
}

GameServer* ToGameServer::find(cChar* mapname) {
	map<string, GameServer*>::iterator it = _map_game.find(mapname);
	if (it == _map_game.end()) {
		LogLine l_line(g_gatelog);
		l_line << newln << "not found [" << mapname << "] map!!!";
		return NULL;
	}
	else
		return (*it).second;
}

void GameServer::Initially() {
	m_plynum = 0;
	gamename = ""; ip = ""; port = 0;
	m_datasock = NULL; next = NULL;
	mapcnt = 0;
}

void GameServer::Finally() {
	m_plynum = 0;
	gamename = ""; ip = ""; port = 0;
	m_datasock = NULL; next = NULL;
	mapcnt = 0;
}

void GameServer::EnterMap(Player *ply, uLong actid, uLong dbid, uLong worldid, cChar *map, Long lMapCpyNO, uLong x, uLong y, char entertype, short swiner) {
	WPacket l_wpk = m_datasock->GetWPacket();
	l_wpk.WriteCmd(CMD_TM_ENTERMAP);
	l_wpk.WriteLong(actid);
	l_wpk.WriteString(ply->m_password);
	l_wpk.WriteLong(dbid);
	l_wpk.WriteLong(worldid);
	l_wpk.WriteString(map);
	l_wpk.WriteLong(lMapCpyNO);
	l_wpk.WriteLong(x);
	l_wpk.WriteLong(y);
	l_wpk.WriteChar(entertype);
	l_wpk.WriteLong(MakeULong(ply));	// The first time to add their own address
	l_wpk.WriteShort(swiner);
	g_gtsvr->gm_conn->SendData(m_datasock, l_wpk);
}

void GameServer::DeleteGameServer(int reason) {
	/*
	WPacket l_wpk	=m_datasock->GetWPacket();
	l_wpk.WriteCmd(CMD_TM_DELETEMAP);
	l_wpk.WriteLong(reason);
	g_gtsvr->gm_conn->SendData(m_datasock, l_wpk);
	*/
}