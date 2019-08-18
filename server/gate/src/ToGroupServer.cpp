#include "gateserver.h"

dbc::cuShort g_version = 103;

long ConnectGroupServer::Process() {
	T_B
		_tgps->m_calltotal++;
		DataSocket* datasock = NULL;
		while (!GetExitFlag() && !_tgps->m_atexit) {
			if (_tgps->_connected) {
				if (_tgps->IsSync() && g_gtsvr) {
					Player	*l_ply = 0;
					RunChainGetArmor<Player> l(g_gtsvr->m_plylst);
					WPacket pk = _tgps->GetWPacket();
					pk.WriteCmd(CMD_TP_SYNC_PLYLST);

					// Total
					int ply_cnt = g_gtsvr->m_plylst.GetTotal();
					pk.WriteLong(ply_cnt);

					// Gateserver's name
					pk.WriteString(_tgps->_myself.c_str());
					Player** ply_array = new Player*[ply_cnt];

					int i = 0;
					for (l_ply = g_gtsvr->m_plylst.GetNextItem(); l_ply; l_ply = g_gtsvr->m_plylst.GetNextItem()) {
						uLong test = MakeULong(l_ply);

						// Attached to the memory address on the GateServer
						pk.WriteLong(MakeULong(l_ply));
						pk.WriteLong(l_ply->m_loginID);
						pk.WriteLong(l_ply->m_actid);
						ply_array[i++] = l_ply;
					}

					RPacket retpk = _tgps->SyncCall(_tgps->get_datasock(), pk);
					int err = retpk.ReadShort();
					if (!retpk.HasData() || err == ERR_PT_LOGFAIL) {
						Sleep(5000);
						_tgps->Disconnect(_tgps->get_datasock());

						// Failed
					}
					else {
						int num = retpk.ReadShort();
						if (num != ply_cnt) {
							Sleep(5000);
							_tgps->Disconnect(_tgps->get_datasock());

							// Failed
						}
						else {
							for (int i = 0; i < num; i++) {
								if (retpk.ReadShort() == 1) {
									uLong test = retpk.ReadLong();;
									ply_array[i]->gp_addr = test;
								}
							}
						}
						_tgps->SetSync(false);
					}
					delete[] ply_array;
				}

				// Already connected
				Sleep(1000);
			}
			else {

				// Not connected or disconnected, re-connected!
				LogLine l_line(g_gateconnect);
				l_line << newln << RES_STRING(GS_TOGROUPSERVER_CPP_00001) << endln;
				
				datasock = _tgps->Connect(_tgps->_gs.ip.c_str(), _tgps->_gs.port);
				if (datasock == NULL) {
					LogLine l_line(g_gateconnect);
					l_line << newln << RES_STRING(GS_TOGROUPSERVER_CPP_00002) << endln;
					Sleep(5000);
					continue;
				}
				else {

					// Log on to GroupServer
					WPacket pk = _tgps->GetWPacket();
					pk.WriteCmd(CMD_TP_LOGIN);
					pk.WriteShort(g_version);
					pk.WriteString(_tgps->_myself.c_str());

					RPacket retpk = _tgps->SyncCall(datasock, pk);
					int err = retpk.ReadShort();
					if (!retpk.HasData() || err == ERR_PT_LOGFAIL) {
						LogLine l_line(g_gateconnect);
						l_line << newln << RES_STRING(GS_TOGROUPSERVER_CPP_00003) << endln;
						datasock = NULL;
						Sleep(5000);
						_tgps->Disconnect(datasock);
					}
					else {
						LogLine l_line(g_gateconnect);
						l_line << newln << RES_STRING(GS_TOGROUPSERVER_CPP_00004) << endln;
						_tgps->_gs.datasock = datasock;
						_tgps->_connected = true;

						_tgps->SetSync();

						datasock = NULL;
					}
				}
			}
		}
	T_FINAL
	return 0;
}

Task *ConnectGroupServer::Lastly() {
	--(_tgps->m_calltotal);
	return Task::Lastly();
}

ToGroupServer::ToGroupServer(char const* fname, ThreadPool* proc, ThreadPool* comm) : TcpClientApp(this, proc, comm), RPCMGR(this), _gs(), _connected(false), m_atexit(0), m_calltotal(0), m_bSync(false), _myself() {
	IniFile inf(fname);
	IniSection& is = inf["GroupServer"];
	_myself = inf["Main"]["Name"];
	_gs.ip = is["IP"];
	_gs.port = atoi(is["Port"]);

	// Start the PING thread

	SetPKParse(0, 2, 64 * 1024, 400);
	BeginWork(atoi(is["EnablePing"]));
}

ToGroupServer::~ToGroupServer() {
	m_atexit = 1;
	while (m_calltotal) {
		Sleep(1);
	}
	ShutDown(12 * 1000);
}

// Return value: true-Allow connection, false-Connection is not allowed
bool ToGroupServer::OnConnect(DataSocket* datasock) {
	datasock->SetRecvBuf(64 * 1024);
	datasock->SetSendBuf(64 * 1024);
	LogLine l_line(g_gateconnect);
	l_line << newln << "connect GroupServer: " << datasock->GetPeerIP() << ",Socket num:" << GetSockTotal() + 1;
	return true;
}

// Reason value: 0 - local program exits normally; - the network is shut down by the peer; - 1-Socket error; - the packet length exceeds the limit.
void ToGroupServer::OnDisconnect(DataSocket* datasock, int reason) {
	// Activate the ConnnectGroupServer thread
	LogLine l_line(g_gateconnect);
	l_line << newln << "disconnection with GroupServer,Socket num: " << GetSockTotal() << ",reason =" << GetDisconnectErrText(reason).c_str() << ", reconnecting..." << endln;

	Player	*l_ply = 0;
	RunChainGetArmor<Player> l(g_gtsvr->m_plylst);
	for (l_ply = g_gtsvr->m_plylst.GetNextItem(); l_ply; l_ply = g_gtsvr->m_plylst.GetNextItem()) {
		if (l_ply) {
			if (l_ply->m_status != 2)
				g_gtsvr->cli_conn->Disconnect(l_ply->m_datasock, 100, -29);
		}
	}

	if (!g_appexit) {
		_connected = false; 
	}
}

WPacket ToGroupServer::OnServeCall(DataSocket* datasock, RPacket &in_para) {
	uShort l_cmd = in_para.ReadCmd();
	WPacket retpk = GetWPacket();
	switch (l_cmd) {
	case 0:
	default:
		break;
	}
	return retpk;
}

void ToGroupServer::OnProcessData(DataSocket* datasock, RPacket &recvbuf) {
	uShort l_cmd = recvbuf.ReadCmd();
	try {
		switch (l_cmd) {
		case CMD_PM_TEAM: {
			for (GameServer *l_game = g_gtsvr->gm_conn->_game_list; l_game; l_game = l_game->next) {
				g_gtsvr->gm_conn->SendData(l_game->m_datasock, recvbuf);
			}
			break;
		}
		case CMD_MM_DO_STRING: {
			for (GameServer *l_game = g_gtsvr->gm_conn->_game_list; l_game; l_game = l_game->next) {
				g_gtsvr->gm_conn->SendData(l_game->m_datasock, recvbuf);
			}
			break;
		}
		case CMD_AP_KICKUSER:
		case CMD_PT_KICKUSER: {
			uShort	l_aimnum = recvbuf.ReverseReadShort();
			Player *l_ply = (Player *)MakePointer(recvbuf.ReverseReadLong());
			if (l_ply && l_ply->gp_addr == recvbuf.ReverseReadLong()) {
				LogLine l_line(g_gatelog);
				l_line << newln << "GroupServer kill person,l_ply->m_dbid =" << l_ply->m_dbid << endln;
				g_gtsvr->cli_conn->Disconnect(l_ply->m_datasock, 0, -21);
			}
			else if (l_ply) {
				LogLine l_line(g_gatelog);
				l_line << newln << "GroupServer kick person, but can't kick person,l_ply->m_dbid =" << l_ply->m_dbid << endln;
			}
			break;
		}
		case CMD_PT_DEL_ESTOPUSER: {
			uShort l_aimnum = recvbuf.ReverseReadShort();
			Player *l_ply = (Player *)MakePointer(recvbuf.ReverseReadLong());
			if (l_ply && l_ply->gp_addr == recvbuf.ReverseReadLong()) {
				LogLine l_line(g_gatelog);
				l_line << newln << "GroupServer del estop user,operator success,l_ply->m_dbid =" << l_ply->m_dbid << endln;
				l_ply->m_estop = false;
			}
			else if (l_ply) {
				LogLine l_line(g_gatelog);
				l_line << newln << "GroupServer del estop user, but can't operator success,l_ply->m_dbid =" << l_ply->m_dbid << endln;
			}
		}
		break;
		case CMD_PT_ESTOPUSER: {
			uShort	l_aimnum = recvbuf.ReverseReadShort();
			Player *l_ply = (Player *)MakePointer(recvbuf.ReverseReadLong());
			if (l_ply && l_ply->gp_addr == recvbuf.ReverseReadLong()) {
				LogLine l_line(g_gatelog);
				l_line << newln << "GroupServer del estop user,operator success,l_ply->m_dbid =" << l_ply->m_dbid << endln;
				l_ply->m_estop = true;
			}
			else if (l_ply) {
				LogLine l_line(g_gatelog);
				l_line << newln << "GroupServer del estop user, but can't operator success,l_ply->m_dbid =" << l_ply->m_dbid << endln;
			}
		}
		break;
		case CMD_MC_SYSINFO:
			l_cmd = CMD_PC_BASE;
		default: {
			if (l_cmd / 500 == CMD_PC_BASE / 500) {
				RPacket	l_rpk = recvbuf;
				uShort l_aimnum = l_rpk.ReverseReadShort();
				recvbuf.DiscardLast(sizeof(uLong) * 2 * l_aimnum + sizeof(uShort));
				Player *l_ply = 0;
				for (uShort i = 0; i < l_aimnum; i++) {
					l_ply = (Player *)MakePointer(l_rpk.ReverseReadLong());
					if (l_ply->gp_addr == l_rpk.ReverseReadLong()) {
						g_gtsvr->cli_conn->SendData(l_ply->m_datasock, recvbuf);
					}
					else {
						l_ply = 0;
					}
				}

				if (l_cmd == CMD_PC_CHANGE_PERSONINFO && l_ply) {
					WPacket	l_wpk = recvbuf;
					l_wpk.WriteCmd(CMD_TM_CHANGE_PERSONINFO);
					l_wpk.WriteLong(MakeULong(l_ply));

					// Attached to the memory address on the GameServer
					l_wpk.WriteLong(l_ply->gm_addr);	
					g_gtsvr->gm_conn->SendData(l_ply->game->m_datasock, l_wpk);
					break;
				}

				if (l_cmd == CMD_PC_PING && l_ply) {
					l_ply->m_pingtime = GetTickCount();
					break;
				}
			}
			else if (l_cmd / 500 == CMD_PM_BASE / 500) {
				RPacket	l_rpk = recvbuf;
				uShort	l_aimnum = l_rpk.ReverseReadShort();
				recvbuf.DiscardLast(sizeof(uLong) * 2 * l_aimnum + sizeof(uShort));
				if (!l_aimnum) {
					WPacket	l_wpk = WPacket(recvbuf).Duplicate();
					l_wpk.WriteLong(0);
					for (GameServer *l_game = g_gtsvr->gm_conn->_game_list; l_game; l_game = l_game->next) {
						g_gtsvr->gm_conn->SendData(l_game->m_datasock, l_wpk);
					}
				}
				else {
					WPacket l_wpk, l_wpk0 = WPacket(recvbuf).Duplicate();
					for (uShort i = 0; i < l_aimnum; i++) {
						Player *l_ply = (Player *)MakePointer(l_rpk.ReverseReadLong());
						if (l_ply->gp_addr == l_rpk.ReverseReadLong() && l_ply->game) {
							l_wpk = l_wpk0;
							l_wpk.WriteLong(MakeULong(l_ply));
							l_wpk.WriteLong(l_ply->gm_addr);
							g_gtsvr->gm_conn->SendData(l_ply->game->m_datasock, l_wpk);
						}
					}
				}
			}
			break;
		}
		}
	}
	catch (...) {
		LG("ToGroupServerError", "l_cmd = %d\n", l_cmd);
	}
}

// Get a list of all users from GroupServer
RPacket ToGroupServer::get_playerlist() {
	WPacket pk = GetWPacket();
	pk.WriteCmd(CMD_TP_REQPLYLST);

	return SyncCall(_gs.datasock, pk);
}
