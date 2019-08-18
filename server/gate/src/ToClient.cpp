#include "gateserver.h"

ToClient::ToClient(char const* fname, ThreadPool* proc, ThreadPool* comm) : TcpServerApp(this, proc, comm, false), RPCMGR(this), m_maxcon(500), m_atexit(0), m_calltotal(0) {
	IniFile inf(fname);
	m_maxcon = atoi(inf["ToClient"]["MaxConnection"]); 
	char buffer[255];
	_snprintf_s(buffer, sizeof(buffer), _TRUNCATE, RES_STRING(GS_TOCLIENT_CPP_00001), m_maxcon);
	std::cout << "[GateServer] : " << buffer << std::endl;
	m_version = atoi(inf["Main"]["Version"]);
	IniSection& is = inf["ToClient"];
	cChar* ip = is["IP"]; uShort port = atoi(is["Port"]);
	_comm_enc = atoi(is["CommEncrypt"]);

#ifdef NET_CAL

	m_checkSpan = atoi(is["CheckSpan"]);
	m_checkWaring = atoi(is["CheckWaring"]);
	m_checkError = atoi(is["CheckError"]);

	// Minimum one second
	if (m_checkSpan < 1)
		m_checkSpan = 1;

	// At least five times per second
	if (m_checkWaring < (5 * m_checkSpan))
		m_checkWaring = (5 * m_checkSpan);

	// At least ten times per second
	if (m_checkError < (7 * m_checkSpan))
		m_checkError = (7 * m_checkSpan);

#endif

	SetPKParse(0, 2, 16 * 1024, 100); BeginWork(atoi(is["EnablePing"]), 1);

	if (OpenListenSocket(port, ip) != 0)
		THROW_EXCP(excp, "ToClient listen failed\n");
}

ToClient::~ToClient() {
	m_atexit = 1;
	while (m_calltotal) {
		Sleep(1);
	}
	ShutDown(12 * 1000);
}

#ifdef NET_CAL

void ToClient::SetCheckSpan(uShort checkSpan) {
	if (checkSpan > 1) {

		// At least ten times per second
		if (m_checkError < (7 * checkSpan))
			m_checkError = (7 * checkSpan);

		// At least five times per second
		if (m_checkWaring < (5 * checkSpan))
			m_checkWaring = (5 * checkSpan);

		m_checkSpan = checkSpan;
	}
}

void ToClient::SetCheckWaring(uShort checkWaring) {
	if (checkWaring > (5 * m_checkSpan) && checkWaring < m_checkError) {
		m_checkWaring = checkWaring;
	}
}

void ToClient::SetCheckError(uShort checkError) {
	if (checkError > (7 * m_checkSpan))
		m_checkError = checkError;
}

#endif

bool ToClient::DoCommand(DataSocket* datasock, cChar *cmdline) {
	if (!strncmp(cmdline, "getbandwidth", sizeof("getbandwidth"))) {
		BandwidthStat	l_band = GetBandwidthStat();
		char buffer[255];
		_snprintf_s(buffer, sizeof(buffer), _TRUNCATE, RES_STRING(GS_TOCLIENT_CPP_00002), GetSockTotal(), l_band.m_sendpktps, l_band.m_sendpkts, l_band.m_sendbyteps / 1024, l_band.m_sendbytes / 1024, l_band.m_recvpktps, l_band.m_recvpkts, l_band.m_recvbyteps / 1024, l_band.m_recvbytes / 1024);

		WPacket l_wpk = GetWPacket();
		l_wpk.WriteCmd(CMD_MC_SYSINFO);
		l_wpk.WriteString(buffer);
		SendData(datasock, l_wpk);
		return true;
	}
	else if (!strncmp(cmdline, "GameState", sizeof("GameState"))) {
		Player *l_ply = (Player*)datasock->GetPointer();
		if (l_ply && g_gtsvr->gm_conn) {
			WPacket l_wpk = GetWPacket();
			l_wpk.WriteCmd(CMD_TM_STATE);
			l_wpk.WriteLong(l_ply->m_dbid);
			l_wpk.WriteLong(MakeULong(l_ply));
			g_gtsvr->gm_conn->SendAllGameServer(l_wpk);
		}
		return true;
	}
	return false;
}

bool ToClient::OnConnect(DataSocket* datasock) {
	if (!g_gtsvr->gp_conn->IsReady()) {
		LogLine l_line(g_gatelog);
		l_line << newln << "client: " << datasock->GetPeerIP() << "	come, groupserver is't ready, force disconnect...";
		return false;
	}

	if (GetSockTotal() <= m_maxcon) {
		datasock->SetRecvBuf(64 * 1024); datasock->SetSendBuf(64 * 1024);
		LogLine l_line(g_gatelog);
		l_line << newln << "client: " << datasock->GetPeerIP() << "	come...Socket num: " << GetSockTotal() + 1;
		return true;
	}
	else {
		LogLine l_line(g_gatelog);
		l_line << newln << "client: " << datasock->GetPeerIP() << "	come, greater than " << m_maxcon << " player, force disconnect...";
		return false;
	}
}

void ToClient::OnConnected(DataSocket* datasock) {
	Player* l_ply = g_gtsvr->player_heap.Get();
	if (!l_ply) {
		printf("error: poor mem %s!\n", datasock->GetPeerIP());
		Disconnect(datasock);
		return;
	}
	// Prevent duplicate entry
	if (!l_ply->InitReference(datasock)) {
		printf("warning: forbid %s repeat connect !", datasock->GetPeerIP());
		l_ply->Free();
		Disconnect(datasock);
		return;
	}

#ifdef NET_CAL
	l_ply->m_cmdNum = 0;
	l_ply->m_waringNum = 0;
	l_ply->m_lashTick = GetCurrentTick();
#endif

	SYSTEMTIME l_st;
	::GetLocalTime(&l_st);
	_snprintf_s(l_ply->m_chapstr, sizeof(l_ply->m_chapstr), _TRUNCATE, "[%02d-%02d %02d:%02d:%02d:%03d]", l_st.wMonth, l_st.wDay, l_st.wHour, l_st.wMinute, l_st.wSecond, l_st.wMilliseconds);

	WPacket	l_wpk = GetWPacket();
	l_wpk.WriteCmd(CMD_MC_CHAPSTR);
	l_wpk.WriteString(l_ply->m_chapstr);
	SendData(datasock, l_wpk);

#if NET_PROTOCOL_ENCRYPT
	static int g_sKeyData = short(g_gtsvr->cli_conn->GetVersion() * g_gtsvr->cli_conn->GetVersion() * 0x1232222);
	int nNoise = g_sKeyData * int(*(int*)(l_ply->m_chapstr + strlen(l_ply->m_chapstr) - 4));
	init_Noise(nNoise, l_ply->m_szSendKey);
	init_Noise(nNoise, l_ply->m_szRecvKey);
#endif

}

void ToClient::OnDisconnect(DataSocket* datasock, int reason) {
	LogLine l_line(g_gatelog);
	l_line << newln << "client: " << datasock->GetPeerIP() << " gone...Socket num: " << GetSockTotal() << " ,reason=" << GetDisconnectErrText(reason).c_str();
	l_line << endln;

	RPacket l_rpk = 0;
	CM_LOGOUT(datasock, l_rpk);
}

dstring	ToClient::GetDisconnectErrText(int reason) {
	switch (reason) {
	case -21:
		return RES_STRING(GS_TOCLIENT_CPP_00011);
	case -23:
		return RES_STRING(GS_TOCLIENT_CPP_00012);
	case -24:
		return	RES_STRING(GS_TOCLIENT_CPP_00013);
	case -25:
		return RES_STRING(GS_TOCLIENT_CPP_00014);
	case -27:
		return RES_STRING(GS_TOCLIENT_CPP_00015);
	case -29:
		return RES_STRING(GS_TOCLIENT_CPP_00016);
	case -31:
		return RES_STRING(GS_TOCLIENT_CPP_00017);
	default:
		return TcpServerApp::GetDisconnectErrText(reason);
	}
}

WPacket ToClient::OnServeCall(DataSocket* datasock, RPacket &in_para) {
	uShort l_cmd = in_para.ReadCmd();

	switch (l_cmd) {
	
	case CMD_CM_LOGOUT: {
		CM_LOGOUT(datasock, in_para);
		Disconnect(datasock, 65, -27);
		return	0;
	}
	
	// The default is forwarded to GameServer
	default: {
		break;
	}
	
	}
	return 0;
}
#define ENABLE_PACKET_LOG
void ToClient::OnProcessData(DataSocket* datasock, RPacket &recvbuf) {
	uShort l_cmd = recvbuf.ReadCmd();
	try {
#ifdef ENABLE_PACKET_LOG
		printf_s("==============================================\n");
		printf_s("Packet Command: %d\n", l_cmd);
		printf_s("Buffer size: %d\n", recvbuf.Size());
		printf_s("packet data len: %d\n", recvbuf.GetDataLen());
		printf_s("packet pkt len: %d\n", recvbuf.GetPacketLen());
		printf_s("==============================================\n");
#endif
#ifdef NET_CAL
		Player *l_ply = (Player*)datasock->GetPointer();
		if (l_ply) {
			uLong l_tick = GetCurrentTick();
			
			// DANGER / UNSIGNED SUBTRACTION
			if (l_tick - l_ply->m_lashTick < (uLong)1000 * m_checkSpan) {
				l_ply->m_cmdNum++;

				if (l_ply->m_cmdNum > m_checkError) {
					std::cout << "[" << datasock->GetPeerIP() << "] Disconnect" << l_ply->m_actid << " cmd[" << l_cmd << "] (" << l_ply->m_cmdNum << ") error!\n";

					LogLine l_line(g_chkattack);
					l_line << newln << "[" << datasock->GetPeerIP() << "] Disconnect" << l_ply->m_actid << " cmd=" << l_cmd << " (" << l_ply->m_cmdNum << ")error!";

					dbc::WPacket l_wpk = GetWPacket();
					l_wpk.WriteCmd(CMD_MC_LOGIN);

					// Error Code
					l_wpk.WriteShort(ERR_MC_NETEXCP);

					// Sent to the client
					SendData(datasock, l_wpk); 
					this->Disconnect(datasock, 100, -31);
				}
				else if (l_ply->m_cmdNum > m_checkWaring) {
					LogLine l_line(g_chkattack);
					l_line << newln << "[" << datasock->GetPeerIP() << "] " << l_ply->m_actid << " cmd=" << l_cmd << " (" << l_ply->m_cmdNum << ")waring!";
					l_ply->m_waringNum++;

					// If the number of consecutive warnings is too high
					if (l_ply->m_waringNum > 2) {
						std::cout << "[" << datasock->GetPeerIP() << "] Disconnect" << l_ply->m_actid << " cmd[" << l_cmd << "] (" << l_ply->m_cmdNum << ") more warring !\n";

						LogLine l_line(g_chkattack);
						l_line << newln << "[" << datasock->GetPeerIP() << "] Disconnect" << l_ply->m_actid << " cmd=" << l_cmd << " (" << l_ply->m_cmdNum << ")more warring!";

						dbc::WPacket l_wpk = GetWPacket();
						l_wpk.WriteCmd(CMD_MC_LOGIN);

						// Error Code
						l_wpk.WriteShort(ERR_MC_NETEXCP);

						// Sent to the client
						SendData(datasock, l_wpk);
						this->Disconnect(datasock, 100, -31);
					}
					else if (datasock->m_recvbyteps > 1024)
					{
						printf("DDoS Caught [%s] disconnected!", datasock->GetPeerIP());
						dbc::WPacket l_wpk = GetWPacket();
						l_wpk.WriteCmd(CMD_MC_LOGIN);
						l_wpk.WriteShort(ERR_MC_NETEXCP);
						SendData(datasock, l_wpk);

						this->Disconnect(datasock, 100, -31);
					}
				}
			}
			else {
				if (l_ply->m_cmdNum < m_checkWaring) {
					// Not consecutive, the number of warnings canceled
					l_ply->m_waringNum = 0;
				}

				l_ply->m_cmdNum = 0;
				l_ply->m_lashTick = l_tick;
			}
		}
#endif

		switch (l_cmd) {
		case CMD_CM_LOGIN:		// Accepts the username / password pair for authentication, and returns a list of valid roles on all server groups under the user name.
		case CMD_CM_LOGOUT:		// Synchronous call
		case CMD_CM_BGNPLAY:	// Receive the selected role name into the Group
		case CMD_CM_NEWCHA: //Server authentication, and then notify the GameServer role into the figure space.
		case CMD_CM_ENDPLAY:
		case CMD_CM_DELCHA:
		case CMD_CM_CREATE_PASSWORD2:
		
		case CMD_CM_UPDATE_PASSWORD2: {

			// If GroupServer, not prepared, of course, not landing
			if (g_gtsvr->gp_conn->IsReady()) {
				++m_calltotal;
				if (m_atexit) {
					--m_calltotal;
					return;
				}

				TransmitCall * l_tc = g_gtsvr->m_tch.Get();
				l_tc->Init(datasock, recvbuf);
				GetProcessor()->AddTask(l_tc);
			}
			else {
				LG("ToGroupServerError", "l_cmd = %d Login \n", l_cmd);
				dbc::WPacket l_wpk = GetWPacket();
				l_wpk.WriteCmd(CMD_MC_LOGIN);
				l_wpk.WriteShort(ERR_MC_NETEXCP);	// Error Code
				SendData(datasock, l_wpk);			// Sent to the client		
				this->Disconnect(datasock, 100, -31);
			}
		} break;
		case CMD_CM_CHECK_PING:{
			if (recvbuf.GetDataLen() == 2 && recvbuf.GetPacketLen() == 8)
			{
				goto ForwardCMD;
			}
			break;
		}
		case CMD_CP_PING: {
			if (g_gtsvr->gp_conn->IsReady()) {
				Player *l_ply = (Player*)datasock->GetPointer();
				if (l_ply && l_ply->gp_addr && l_ply->gm_addr) {
					WPacket l_wpk = GetWPacket();
					l_wpk.WriteCmd(CMD_CP_PING);
					l_wpk.WriteLong(GetTickCount() - l_ply->m_pingtime);
					l_ply->m_pingtime = 0;

					l_wpk.WriteLong(MakeULong(l_ply));
					l_wpk.WriteLong(l_ply->gp_addr);
					g_gtsvr->gp_conn->SendData(g_gtsvr->gp_conn->get_datasock(), l_wpk);
				}
			}
			else {
				LG("ToGroupServerError", "l_cmd = %d Ping \n", l_cmd);
				dbc::WPacket l_wpk = GetWPacket();
				l_wpk.WriteCmd(CMD_MC_LOGIN);
				l_wpk.WriteShort(ERR_MC_NETEXCP);	// Error Code
				SendData(datasock, l_wpk);			// Sent to the client
				this->Disconnect(datasock, 100, -31);
			}
		} break;

		case CMD_CM_SAY: {
			cChar *l_str = recvbuf.ReadString();
			if (!l_str) {
				break;
			}

			if (*l_str == '&' && DoCommand(datasock, l_str + 1)) {
				break;
			}

			if (strstr(l_str, "#21")) {
				break;
			}

			Player *l_ply = (Player*)datasock->GetPointer();
			if (l_ply && l_ply->m_estop) {
				if (GetTickCount() - l_ply->m_lestoptick >= 1000 * 60 * 2) {
					WPacket l_wpk = GetWPacket();
					l_wpk.WriteCmd(CMD_TP_ESTOPUSER_CHECK);
					l_wpk.WriteLong(l_ply->m_actid);
					g_gtsvr->gp_conn->SendData(g_gtsvr->gp_conn->get_datasock(), l_wpk);
				}

				WPacket l_wpk = GetWPacket();
				l_wpk.WriteCmd(CMD_MC_SYSINFO);
				l_wpk.WriteString(RES_STRING(GS_TOCLIENT_CPP_00018));
				g_gtsvr->gp_conn->SendData(l_ply->m_datasock, l_wpk);
				break;
			}
		}

		// The default forwarding to the GroupServer or GameServer
		default:
		ForwardCMD:
			if (l_cmd / 500 == CMD_CM_BASE / 500) {
				// Forward to GameServer
				Player *l_ply = (Player*)datasock->GetPointer();
				if (l_ply) {
					uLong l_gpaddr = l_ply->gp_addr;
					uLong l_gmaddr = l_ply->gm_addr;
					GameServer *l_game = l_ply->game;

					if (l_gpaddr && l_gmaddr && l_game) {
						WPacket	l_wpk = WPacket(recvbuf).Duplicate();
						l_wpk.WriteLong(MakeULong(l_ply));
						l_wpk.WriteLong(l_gmaddr);
						g_gtsvr->gm_conn->SendData(l_ply->game->m_datasock, l_wpk);
					}
				}
			}
			else if (l_cmd / 500 == CMD_CP_BASE / 500) {

				// Forwarded to GroupServer
				Player *l_ply = (Player*)datasock->GetPointer();
				if (l_ply) {
					
					// When GroupServer is ready to send messages, to avoid blocking
					if (g_gtsvr->gp_conn->IsReady()) {
						uLong l_gpaddr = l_ply->gp_addr;
						uLong l_gmaddr = l_ply->gm_addr;
						if (l_gpaddr && l_gmaddr) {
							WPacket	l_wpk = WPacket(recvbuf).Duplicate();
							l_wpk.WriteLong(MakeULong(l_ply));
							l_wpk.WriteLong(l_gpaddr);
							g_gtsvr->gp_conn->SendData(g_gtsvr->gp_conn->get_datasock(), l_wpk);
						}
					}
					else {
						LG("ToGroupServerError", "l_cmd = %d IsReady \n", l_cmd);
						dbc::WPacket l_wpk = GetWPacket();
						l_wpk.WriteCmd(CMD_MC_LOGIN);
						
						// Error Code
						l_wpk.WriteShort(ERR_MC_NETEXCP);
						
						// Sent to the client
						SendData(datasock, l_wpk);
						this->Disconnect(datasock, 100, -31);
					}
				}
			}

			break;
		}

		static uLong l_last = GetTickCount();
		if (datasock->m_recvbyteps > 1024 * 5) {
			uLong	l_tick = GetCurrentTick();
			if (l_tick - l_last > 1000 * 30) {
				Disconnect(datasock, 100, -25);
				l_last = l_tick;
				std::cout << "[" << datasock->GetPeerIP() << "] sending big packet (>5K/s) attack server,please deal with!\n";
				LogLine l_line(g_chkattack);
				l_line << newln << "[" << datasock->GetPeerIP() << "] sending big packet (>5K/s) attack server,please deal with!";
			}
		}

	}
	catch (...) {
		LG("ToClientError", "l_cmd = %d\n", l_cmd);
	}

	return;
}

long TransmitCall::Process() {
	LogLine l_line(g_gatelog);
	if (!g_gtsvr->gp_conn->IsReady()) {
		g_gtsvr->cli_conn->Disconnect(m_datasock, 50, -27);
		l_line << newln << "IsReady = false";
	}
	uShort l_cmd = m_recvbuf.ReadCmd();
	try {
		switch (l_cmd) {
		
		// Accepts the username / password pair for authentication, and returns a list of valid roles on all server groups under the user name. 
		case CMD_CM_LOGIN:
			g_gtsvr->cli_conn->CM_LOGIN(m_datasock, m_recvbuf);
			break;
		
		// Synchronous call
		case CMD_CM_LOGOUT:
			g_gtsvr->cli_conn->CM_LOGOUT(m_datasock, m_recvbuf);
			g_gtsvr->cli_conn->Disconnect(m_datasock, 50, -27);
			break;

		// Receive the selected role name into the GroupServer authentication, and then notify the GameServer role into the figure space
		case CMD_CM_BGNPLAY:
			g_gtsvr->cli_conn->CM_BGNPLAY(m_datasock, m_recvbuf);
			break;

		case CMD_CM_ENDPLAY:
			g_gtsvr->cli_conn->CM_ENDPLAY(m_datasock, m_recvbuf);
			break;
		case CMD_CM_NEWCHA:
			g_gtsvr->cli_conn->CM_NEWCHA(m_datasock, m_recvbuf);
			break;
		case CMD_CM_DELCHA:
			g_gtsvr->cli_conn->CM_DELCHA(m_datasock, m_recvbuf);
			break;
		case CMD_CM_CREATE_PASSWORD2:
			g_gtsvr->cli_conn->CM_CREATE_PASSWORD2(m_datasock, m_recvbuf);
			break;
		case CMD_CM_UPDATE_PASSWORD2:
			g_gtsvr->cli_conn->CM_UPDATE_PASSWORD2(m_datasock, m_recvbuf);
			break;
		}
	}
	catch (...) {
		try {
			g_gtsvr->cli_conn->Disconnect(m_datasock, 50, -27);
		}
		catch (...) {

			// MRF_REMOVE: Is Useless.
		}
		l_line << newln << "IsReady = false exception:" << l_cmd;
	}
	--(g_gtsvr->cli_conn->m_calltotal);
	return 0;
}

void ToClient::CM_LOGIN(DataSocket* datasock, RPacket& recvbuf) {
	uLong l_ulMilliseconds = 30 * 1000;

	// Whether to use plug-in
	uLong l_tick = GetTickCount() - recvbuf.GetTickCount();
	bool bCheat = false;
	if (l_ulMilliseconds > l_tick) {
		l_ulMilliseconds = l_ulMilliseconds - l_tick;
		if (m_version != recvbuf.ReverseReadShort()) {
			WPacket l_wpk = GetWPacket();
			l_wpk.WriteCmd(CMD_MC_LOGIN);

			// Error Code
			l_wpk.WriteShort(ERR_MC_VER_ERROR);
	
			// Sent to the client
			SendData(datasock, l_wpk);
			LogLine l_line(g_gatelog);

			l_line << newln << "client: " << datasock->GetPeerIP() << "	login error: client and server can't match!";
			Disconnect(datasock, 100, -31);
			return;
		}

		// Judgment when using the plug
		if (recvbuf.ReverseReadShort() != 911) {
			bCheat = true;
		}
		else {
			recvbuf.DiscardLast(static_cast<uLong>(sizeof(uShort)));
		}
		recvbuf.DiscardLast(static_cast<uLong>(sizeof(uShort)));

		ToGroupServer* l_gps = g_gtsvr->gp_conn;
		Player* l_ply = reinterpret_cast<Player *>(datasock->GetPointer());
		
		// The organization repeats itself
		if (!l_ply) {
			return;
		}

		string szLocale = "";
		szLocale = recvbuf.ReadString();

		WPacket l_wpk = WPacket(recvbuf).Duplicate();

		l_wpk.WriteCmd(CMD_TP_USER_LOGIN);
		l_wpk.WriteString(l_ply->m_chapstr);
		l_wpk.WriteLong(inet_addr(datasock->GetPeerIP()));

		// Attached to the memory address on the GateServer
		l_wpk.WriteLong(MakeULong(l_ply)); 

		if (bCheat) {
			l_wpk.WriteShort(0);
		}
		else {
			l_wpk.WriteShort(911);
		}

		RPacket l_rpk = l_gps->SyncCall(l_gps->get_datasock(), l_wpk, l_ulMilliseconds);
		uShort l_errno = 0;
		if (l_rpk.HasData() == 0) {
			l_wpk = GetWPacket();
			l_wpk.WriteCmd(CMD_MC_LOGIN);
			l_wpk.WriteShort(ERR_MC_NETEXCP);	// Error Code
			SendData(datasock, l_wpk);			// Sent to the client
			LogLine ll_line(g_gatelog);
			ll_line << newln << "client: " << datasock->GetPeerIP() << "	login error: GroupServer is disresponse!" << endln;
			Disconnect(datasock, 100, -31);
		}
		else if (l_errno = l_rpk.ReadShort()) {
			l_wpk = l_rpk;
			l_wpk.WriteCmd(CMD_MC_LOGIN);
			SendData(datasock, l_wpk);
			LogLine l_line(g_gatelog);
			l_line << newln << "client: " << datasock->GetPeerIP() << "	login error, error:" << l_errno << endln;
			Disconnect(datasock, 100, -31);
		}
		else {

			// Placed in the election / build / delete role status
			l_ply->m_status = 1;
			std::string sessionKey = l_rpk.ReadString();

			l_ply->gp_addr = l_rpk.ReverseReadLong();		// Save the player's memory address on the GroupServer
			l_ply->m_loginID = l_rpk.ReverseReadLong();		// Account DB id
			l_ply->m_actid = l_rpk.ReverseReadLong();
			BYTE byPassword = l_rpk.ReverseReadChar();
			l_ply->comm_key_len = l_rpk.ReverseReadShort();
			memcpy(l_ply->comm_textkey, l_rpk.GetDataAddr() + l_rpk.GetDataLen() - 15 - l_ply->comm_key_len, l_ply->comm_key_len);
			l_rpk.DiscardLast(sizeof(uLong) * 3 + 2 + 1 + l_ply->comm_key_len + 2);

			l_wpk = WPacket(l_rpk).Duplicate();
			l_wpk.WriteCmd(CMD_MC_LOGIN);
			l_wpk.WriteChar(byPassword);
			l_wpk.WriteLong(_comm_enc);
			l_wpk.WriteLong(0x3214);
			SendData(datasock, l_wpk);
			LogLine l_line(g_gatelog);
			l_line << newln << "client: " << datasock->GetPeerIP() << "	login ok." << endln;

			// Starts encryption
			l_ply->enc = true;

			l_ply->BeginRun();	
		}
	}
	else {
		WPacket l_wpk = GetWPacket();
		l_wpk.WriteCmd(CMD_MC_LOGIN);
		l_wpk.WriteShort(ERR_MC_NETEXCP);	// Error Code
		SendData(datasock, l_wpk);			// Sent to the client
		LogLine l_line(g_gatelog);
		l_line << newln << "client: " << datasock->GetPeerIP() << "	login error: packet time out!" << endln;
		Disconnect(datasock, 100, -31);
	}
}

WPacket ToClient::CM_DISCONN(DataSocket* datasock, RPacket& recvbuf) {
	uLong l_ulMilliseconds = 30 * 1000;
	uLong l_tick = GetTickCount() - recvbuf.GetTickCount();
	l_ulMilliseconds = (l_ulMilliseconds > l_tick) ? l_ulMilliseconds - l_tick : 1;

	WPacket	l_retpk = 0;
	Player *l_ply = 0;
	MutexArmor lock(g_gtsvr->_mtxother);
	l_ply = (Player*)datasock->GetPointer();
	datasock->SetPointer(0);
	lock.unlock();

	if (l_ply) {
		l_ply->EndRun();

		MutexArmor l_lockStat(l_ply->m_mtxstat);

		// The timing of this command is illegal, because the current player is not in the selected role, can not choose another role
		if (l_ply->m_status == 0) {
			WPacket	l_wpk = datasock->GetWPacket();
			l_retpk.WriteShort(ERR_MC_NOTLOGIN);

		}
		else {
			WPacket l_wpk = GetWPacket();

			l_wpk.WriteCmd(CMD_TP_DISC);
			l_wpk.WriteLong(l_ply->m_actid);
			l_wpk.WriteLong(inet_addr(datasock->GetPeerIP()));
			l_wpk.WriteString(GetDisconnectErrText(datasock->GetDisconnectReason() ? datasock->GetDisconnectReason() : -27).c_str());
			g_gtsvr->gp_conn->SendData(g_gtsvr->gp_conn->get_datasock(), l_wpk);

			GameServer	*l_game = l_ply->game;
			if ((l_ply->m_status == 2) && l_ply->gm_addr && l_game && l_game->m_datasock) {
				// Notify the GameServer GoOut map
				LogLine l_line(g_gatelog);
				l_line << newln << "client: " << datasock->GetPeerIP() << ":" << datasock->GetPeerPort() << "	GoOut map,Gate to[" << l_game->m_datasock->GetPeerIP() << "] send GoOutMap command,dbid:" << l_ply->m_dbid
					   << uppercase << hex << ",Gate address:" << MakeULong(l_ply) << ",Game address:" << l_ply->gm_addr << dec << nouppercase << endln;
				
				WPacket	l_wpk = l_game->m_datasock->GetWPacket();
				l_wpk.WriteCmd(CMD_TM_GOOUTMAP);

				l_wpk.WriteChar(0);

				l_wpk.WriteLong(MakeULong(l_ply));
				
				// Attached to the address on the GameServer
				l_wpk.WriteLong(l_ply->gm_addr);

				// Block the back of the data to the GameServer
				l_ply->game = 0;
				l_ply->gm_addr = 0;

				SendData(l_game->m_datasock, l_wpk);
			}

			// Notify GroupServer Logout
			l_wpk = g_gtsvr->gp_conn->get_datasock()->GetWPacket();
			l_wpk.WriteCmd(CMD_TP_USER_LOGOUT);
			l_wpk.WriteLong(MakeULong(l_ply));
			l_wpk.WriteLong(l_ply->gp_addr);
			l_ply->gp_addr = 0;
			l_retpk = g_gtsvr->gp_conn->SyncCall(g_gtsvr->gp_conn->get_datasock(), l_wpk, l_ulMilliseconds);
		}
		l_lockStat.unlock();
		l_ply->Free();
	}
	return l_retpk;
}

WPacket ToClient::CM_LOGOUT(DataSocket* datasock, RPacket& recvbuf) {
	uLong l_ulMilliseconds = 30 * 1000;
	uLong l_tick = GetTickCount() - recvbuf.GetTickCount();
	l_ulMilliseconds = (l_ulMilliseconds > l_tick) ? l_ulMilliseconds - l_tick : 1;

	WPacket	l_retpk = 0;
	Player *l_ply = 0;
	MutexArmor lock(g_gtsvr->_mtxother);
	l_ply = (Player*)datasock->GetPointer();

	if (l_ply)
		l_ply->m_datasock = NULL;

	datasock->SetPointer(0);
	lock.unlock();

	if (l_ply) {
		l_ply->EndRun();

		// g_gtsvr->m_ToMMS->Logout(datasock->GetPeerIP(), datasock->GetPeerPort(), l_ply->m_actid);

		MutexArmor l_lockStat(l_ply->m_mtxstat);
		try {

			// The timing of this command is illegal, because the current player is not in the selected role, can not choose another role
			if (l_ply->m_status == 0) {
				WPacket	l_wpk = datasock->GetWPacket();
				l_retpk.WriteShort(ERR_MC_NOTLOGIN);
			}
			else {
				WPacket l_wpk = GetWPacket();
				l_wpk.WriteCmd(CMD_TP_DISC);
				l_wpk.WriteLong(l_ply->m_actid);
				l_wpk.WriteLong(inet_addr(datasock->GetPeerIP()));
				l_wpk.WriteString(GetDisconnectErrText(datasock->GetDisconnectReason() ? datasock->GetDisconnectReason() : -27).c_str());
				g_gtsvr->gp_conn->SendData(g_gtsvr->gp_conn->get_datasock(), l_wpk);

				/*
				int tick = 0;
				while(l_ply->m_switch && tick < 5) {
					::Sleep(5);
					LogLine l_line(g_gatelog);
					l_line<<newln<<"client: "<<l_ply->m_dbid <<" switch map!" << tick;
					tick ++;
				}
				*/

				if (l_ply->m_switch) {
					LogLine l_line(g_gatelog);
					l_line << newln << "client: " << l_ply->m_dbid << " switch map error!";
				}

				GameServer *l_game = l_ply->game;
				if ((l_ply->m_status == 2) && l_ply->gm_addr && l_game && l_game->m_datasock) {

					// Notify the GameServer GoOut map
					LogLine l_line(g_gatelog);
					l_line << newln << "client: " << datasock->GetPeerIP() << ":" << datasock->GetPeerPort() << "	GoOut map,Gate to [" << l_game->m_datasock->GetPeerIP() << "]send GoOutMap command,dbid:" << l_ply->m_dbid
						   << uppercase << hex << ",Gate address:" << MakeULong(l_ply) << ",Game address:" << l_ply->gm_addr << dec << nouppercase << endln;
					
					WPacket	l_wpk = l_game->m_datasock->GetWPacket();
					l_wpk.WriteCmd(CMD_TM_GOOUTMAP);
					l_wpk.WriteChar(0);
					l_wpk.WriteLong(MakeULong(l_ply));

					// Attached to the address on the GameServer
					l_wpk.WriteLong(l_ply->gm_addr);
					
					// Block the back of the data to the GameServer
					l_ply->game = 0;
					l_ply->gm_addr = 0;

					SendData(l_game->m_datasock, l_wpk);
				}

				// Notify GroupServer Logout
				l_wpk = g_gtsvr->gp_conn->get_datasock()->GetWPacket();
				l_wpk.WriteCmd(CMD_TP_USER_LOGOUT);
				l_wpk.WriteLong(MakeULong(l_ply));
				l_wpk.WriteLong(l_ply->gp_addr);
				l_ply->gp_addr = 0;
				l_retpk = g_gtsvr->gp_conn->SyncCall(g_gtsvr->gp_conn->get_datasock(), l_wpk, l_ulMilliseconds);
			}
		}
		catch (...) {
			LogLine l_line(g_gatelog);
			l_line << newln << "Error exit!";
		}
		l_lockStat.unlock();

#ifdef NET_CAL
		l_ply->m_cmdNum = 0;
		l_ply->m_waringNum = 0;
		l_ply->Free();
#endif

	}
	return l_retpk;
}

void ToClient::CM_BGNPLAY(DataSocket* datasock, RPacket& recvbuf) {
	uLong l_ulMilliseconds = 30 * 1000;
	uLong l_tick = GetTickCount() - recvbuf.GetTickCount();
	if (l_ulMilliseconds > l_tick) {
		l_ulMilliseconds = l_ulMilliseconds - l_tick;
		Player *l_ply = (Player*)datasock->GetPointer();
		if (l_ply) {
			MutexArmor l_lockStat(l_ply->m_mtxstat);

			// The timing of this command is illegal, because the current player is not in the selected role, can not choose another role
			if (l_ply->m_status != 1 || !l_ply->gp_addr) {
				WPacket	l_wpk = datasock->GetWPacket();
				l_wpk.WriteCmd(CMD_MC_BGNPLAY);
				l_wpk.WriteShort(ERR_MC_NOTSELCHA);
				SendData(datasock, l_wpk);
			}
			else {

				// Verify the legitimacy of the role played
				WPacket	l_wpk = WPacket(recvbuf).Duplicate();
				l_wpk.WriteCmd(CMD_TP_BGNPLAY);
				l_wpk.WriteLong(MakeULong(l_ply));
				l_wpk.WriteLong(l_ply->gp_addr);
				RPacket	l_rpk = g_gtsvr->gp_conn->SyncCall(g_gtsvr->gp_conn->get_datasock(), l_wpk, l_ulMilliseconds);
				uShort	l_errno;

				// Network Error
				if (!l_rpk.HasData()) {
					l_wpk = datasock->GetWPacket();
					l_wpk.WriteCmd(CMD_MC_BGNPLAY);
					l_wpk.WriteShort(ERR_MC_NETEXCP);
					SendData(datasock, l_wpk);
				}

				// Play the role is not legitimate
				else if (l_errno = l_rpk.ReadShort()) {
					l_wpk = l_rpk;
					l_wpk.WriteCmd(CMD_MC_BGNPLAY);
					SendData(datasock, l_wpk);
					if (l_errno == ERR_PT_KICKUSER) {
						Disconnect(datasock, 100, -25);
					}
				}

				// Select the role of success, return a success message, and the map name and the successful role ID to the GameServer.
				else {
					memset(l_ply->m_password, 0, sizeof(l_ply->m_password));

					// Password of the secondary role
					strncpy_s(l_ply->m_password, sizeof(l_ply->m_password), l_rpk.ReadString(), _TRUNCATE); 

					l_ply->m_dbid = l_rpk.ReadLong();			// Role ID
					l_ply->m_worldid = l_rpk.ReadLong();		// The unique ID assigned by GroupServer
					cChar *l_map = l_rpk.ReadString();
					l_ply->m_sGarnerWiner = l_rpk.ReadShort();
					GameServer *l_game = g_gtsvr->gm_conn->find(l_map);
					
					// The destination map is unreachable
					if (!l_game) {
						l_wpk = datasock->GetWPacket();
						l_wpk.WriteCmd(CMD_MC_BGNPLAY);
						l_wpk.WriteShort(ERR_MC_NOTARRIVE);
						SendData(datasock, l_wpk);
					}

					// MRF_SOURCE_LIMIT: Players can't exceed 15000 ( Contradicts the 10240 rule )
					// Too many
					else if (l_game->m_plynum > 15000) {
						l_wpk = datasock->GetWPacket();
						l_wpk.WriteCmd(CMD_MC_BGNPLAY);
						l_wpk.WriteShort(ERR_MC_TOOMANYPLY);
						SendData(datasock, l_wpk);
					}
					
					else {
						// Select the role of success, placed in the game play state
						l_ply->m_status = 2;

						// No synchronization, just a simple reference
						l_game->m_plynum++;

						// Notify GameServer to enter the map
						LogLine l_line(g_gatelog);
						l_line << newln << "client: " << datasock->GetPeerIP() << ":" << datasock->GetPeerPort() << "	BeginPlay entry map,Gate to[" << l_game->m_datasock->GetPeerIP() << "]send EnterMap command,dbid:" << l_ply->m_dbid
							   << uppercase << hex << ",Gate address:" << MakeULong(l_ply) << dec << nouppercase << endln;
						
						// According to the map to find GameServer, and then request GameServer to enter this map
						l_game->EnterMap(l_ply, l_ply->m_loginID, l_ply->m_dbid, l_ply->m_worldid, l_map, -1, 0, 0, 0, l_ply->m_sGarnerWiner);
					}
				}
			}
			l_lockStat.unlock();
		}
	}
	else {
		WPacket l_wpk = datasock->GetWPacket();
		l_wpk.WriteCmd(CMD_MC_BGNPLAY);
		l_wpk.WriteShort(ERR_MC_NETEXCP);
		SendData(datasock, l_wpk);
	}
}

void ToClient::CM_ENDPLAY(DataSocket* datasock, RPacket& recvbuf) {
	uLong l_ulMilliseconds = 30 * 1000;
	uLong l_tick = GetTickCount() - recvbuf.GetTickCount();
	l_ulMilliseconds = (l_ulMilliseconds > l_tick) ? l_ulMilliseconds - l_tick : 1;

	Player *l_ply = (Player*)datasock->GetPointer();
	if (l_ply) {
		MutexArmor l_lockStat(l_ply->m_mtxstat);

		// The timing of this command is illegal because the current player is not in a role and can not reselect another character
		if (l_ply->m_status != 2 || !l_ply->gm_addr) {
			WPacket	l_wpk = datasock->GetWPacket();
			l_wpk.WriteCmd(CMD_MC_ENDPLAY);
			l_wpk.WriteShort(ERR_MC_NOTPLAY);
			SendData(datasock, l_wpk);
			Disconnect(datasock, 100, -25);
		}
		else {
			GameServer *l_game = l_ply->game;
			if (l_game && l_game->m_datasock) {

				// Enter the selected role screen state
				l_ply->m_status = 1;

				l_game->m_plynum--;
		
				// Notice GameServer out of the map
				LogLine l_line(g_gatelog);
				l_line << newln << "client: " << datasock->GetPeerIP() << ":" << datasock->GetPeerPort() << "	GoOut map,Gate to[" << l_game->m_datasock->GetPeerIP() << "] send GoOutMap command,dbid:" << l_ply->m_dbid
					   << uppercase << hex << ",Gate address:" << MakeULong(l_ply) << ",Game address:" << l_ply->gm_addr << dec << nouppercase << endln;
				
				WPacket l_wpk = WPacket(recvbuf).Duplicate();
				l_wpk.WriteCmd(CMD_TM_GOOUTMAP);

				l_wpk.WriteChar(0);

				l_wpk.WriteLong(MakeULong(l_ply));
				l_wpk.WriteLong(l_ply->gm_addr);			// Attach the address on the GameServer

				l_ply->game = 0;							// Block the back of the data to the GameServer
				l_ply->gm_addr = 0;

				g_gtsvr->gm_conn->SendData(l_game->m_datasock, l_wpk);
				// g_gtsvr->gm_conn->SyncCall(l_game->m_datasock,l_wpk);
				
				// Tells GroupServer to end the game
				l_wpk = WPacket(recvbuf).Duplicate();
				l_wpk.WriteCmd(CMD_TP_ENDPLAY);
				l_wpk.WriteLong(MakeULong(l_ply));
				l_wpk.WriteLong(l_ply->gp_addr);
				l_wpk = g_gtsvr->gp_conn->SyncCall(g_gtsvr->gp_conn->get_datasock(), l_wpk, l_ulMilliseconds);
				if (!l_wpk.HasData()) {
					l_wpk = datasock->GetWPacket();
					l_wpk.WriteCmd(CMD_MC_ENDPLAY);
					l_wpk.WriteShort(ERR_MC_NETEXCP);
					SendData(datasock, l_wpk);
					Disconnect(datasock, 100, -25);
				}
				else {
					uShort err = RPacket(l_wpk).ReadShort();
					if (err == ERR_PT_INERR || err == ERR_PT_KICKUSER) {
						Disconnect(datasock, 100, -25);
					}
					else {

						// Return to the Client
						l_wpk.WriteCmd(CMD_MC_ENDPLAY);
						SendData(datasock, l_wpk);
						l_ply->m_dbid = 0;
						l_ply->m_worldid = 0;
					}

					/*
					// Return to the Client
					l_wpk.WriteCmd(CMD_MC_ENDPLAY);
					SendData(datasock, l_wpk);
					l_ply->m_dbid	= 0;
					l_ply->m_worldid = 0;
					if (RPacket(l_wpk).ReadShort() == ERR_PT_KICKUSER) {
						Disconnect(datasock,100,-25);
					}
					*/

				}
			}
		}
		l_lockStat.unlock();
	}
}

void ToClient::CM_NEWCHA(DataSocket* datasock, RPacket& recvbuf) {
	uLong l_ulMilliseconds = 30 * 1000;
	uLong l_tick = GetTickCount() - recvbuf.GetTickCount();
	if (l_ulMilliseconds > l_tick) {
		l_ulMilliseconds = l_ulMilliseconds - l_tick;
		Player *l_ply = (Player*)datasock->GetPointer();
		if (l_ply) {
			MutexArmor l_lockStat(l_ply->m_mtxstat);
			if (l_ply->m_status != 1 || !l_ply->gp_addr) {
				WPacket	l_wpk = datasock->GetWPacket();
				l_wpk.WriteCmd(CMD_MC_NEWCHA);
				l_wpk.WriteShort(ERR_MC_NOTSELCHA);
				SendData(datasock, l_wpk);
			}
			else {
				// Call GroupServer
				WPacket l_wpk = WPacket(recvbuf).Duplicate();
				l_wpk.WriteCmd(CMD_TP_NEWCHA);
				l_wpk.WriteLong(MakeULong(l_ply));
				l_wpk.WriteLong(l_ply->gp_addr);	// Incidental address
				l_wpk = g_gtsvr->gp_conn->SyncCall(g_gtsvr->gp_conn->get_datasock(), l_wpk, l_ulMilliseconds);
				if (!l_wpk.HasData()) {
					l_wpk = datasock->GetWPacket();
					l_wpk.WriteCmd(CMD_MC_NEWCHA);
					l_wpk.WriteShort(ERR_MC_NETEXCP);
					SendData(datasock, l_wpk);
					Disconnect(datasock, 100, -25);
				}
				else {
					// Return to the Client
					l_wpk.WriteCmd(CMD_MC_NEWCHA);
					SendData(datasock, l_wpk);
					if (RPacket(l_wpk).ReadShort() == ERR_PT_KICKUSER) {
						Disconnect(datasock, 100, -25);
					}
				}
			}
			l_lockStat.unlock();
		}
	}
	else {
		WPacket	l_wpk = datasock->GetWPacket();
		l_wpk.WriteCmd(CMD_MC_NEWCHA);
		l_wpk.WriteShort(ERR_MC_NETEXCP);
		SendData(datasock, l_wpk);
		Disconnect(datasock, 100, -25);
	}
}

void ToClient::CM_DELCHA(DataSocket* datasock, RPacket& recvbuf) {
	uLong l_ulMilliseconds = 30 * 1000;
	uLong l_tick = GetTickCount() - recvbuf.GetTickCount();
	if (l_ulMilliseconds > l_tick) {
		l_ulMilliseconds = l_ulMilliseconds - l_tick;
		Player *l_ply = (Player*)datasock->GetPointer();
		if (l_ply) {
			MutexArmor l_lockStat(l_ply->m_mtxstat);
			if (l_ply->m_status != 1 || !l_ply->gp_addr) {
				WPacket	l_wpk = datasock->GetWPacket();
				l_wpk.WriteCmd(CMD_MC_DELCHA);
				l_wpk.WriteShort(ERR_MC_NOTSELCHA);
				SendData(datasock, l_wpk);
			}
			else {
				// Call GroupServer
				WPacket l_wpk = WPacket(recvbuf).Duplicate();
				l_wpk.WriteCmd(CMD_TP_DELCHA);
				l_wpk.WriteLong(MakeULong(l_ply));
				l_wpk.WriteLong(l_ply->gp_addr);	// Incidental address
				l_wpk = g_gtsvr->gp_conn->SyncCall(g_gtsvr->gp_conn->get_datasock(), l_wpk);
				if (!l_wpk.HasData()) {
					l_wpk = datasock->GetWPacket();
					l_wpk.WriteCmd(CMD_MC_DELCHA);
					l_wpk.WriteShort(ERR_MC_NETEXCP);
					SendData(datasock, l_wpk);
					Disconnect(datasock, 100, -25);
				}
				else {
					// Return to Client
					l_wpk.WriteCmd(CMD_MC_DELCHA);
					SendData(datasock, l_wpk);
					if (RPacket(l_wpk).ReadShort() == ERR_PT_KICKUSER) {
						Disconnect(datasock, 100, -25);
					}
				}
			}
			l_lockStat.unlock();
		}
	}
	else {
		WPacket	l_wpk = datasock->GetWPacket();
		l_wpk.WriteCmd(CMD_MC_DELCHA);
		l_wpk.WriteShort(ERR_MC_NETEXCP);
		SendData(datasock, l_wpk);
		Disconnect(datasock, 100, -25);
	}
}

void ToClient::CM_CREATE_PASSWORD2(DataSocket* datasock, RPacket& recvbuf) {
	uLong l_ulMilliseconds = 30 * 1000;
	uLong l_tick = GetTickCount() - recvbuf.GetTickCount();
	if (l_ulMilliseconds > l_tick) {
		l_ulMilliseconds = l_ulMilliseconds - l_tick;
		Player *l_ply = (Player*)datasock->GetPointer();
		if (l_ply) {
			MutexArmor l_lockStat(l_ply->m_mtxstat);
			if (l_ply->m_status != 1 || !l_ply->gp_addr) {
				WPacket	l_wpk = datasock->GetWPacket();
				l_wpk.WriteCmd(CMD_MC_DELCHA);
				l_wpk.WriteShort(ERR_MC_NOTSELCHA);
				SendData(datasock, l_wpk);
			}
			else {

				// Call GroupServer
				WPacket l_log(recvbuf);
				const char * pszPW = recvbuf.ReadString();
				WPacket l_wpk = WPacket(recvbuf).Duplicate();
				l_wpk.WriteCmd(CMD_TP_CREATE_PASSWORD2);
				l_wpk.WriteLong(MakeULong(l_ply));
				l_wpk.WriteLong(l_ply->gp_addr);
				l_wpk = g_gtsvr->gp_conn->SyncCall(g_gtsvr->gp_conn->get_datasock(), l_wpk);
				if (!l_wpk.HasData()) {
					l_wpk = datasock->GetWPacket();
					l_wpk.WriteCmd(CMD_MC_CREATE_PASSWORD2);
					l_wpk.WriteShort(ERR_MC_NETEXCP);
					SendData(datasock, l_wpk);
					Disconnect(datasock, 100, -25);
				}
				else {
					
					// Return to Client
					l_wpk.WriteCmd(CMD_MC_CREATE_PASSWORD2);
					SendData(datasock, l_wpk);
					if (RPacket(l_wpk).ReadShort() == ERR_PT_KICKUSER) {
						Disconnect(datasock, 100, -25);
					}
				}
			}
			l_lockStat.unlock();
		}
	}
	else {
		WPacket	l_wpk = datasock->GetWPacket();
		l_wpk.WriteCmd(CMD_MC_CREATE_PASSWORD2);
		l_wpk.WriteShort(ERR_MC_NETEXCP);
		SendData(datasock, l_wpk);
		Disconnect(datasock, 100, -25);
	}
}

void ToClient::CM_UPDATE_PASSWORD2(DataSocket* datasock, RPacket& recvbuf) {
	uLong l_ulMilliseconds = 30 * 1000;
	uLong l_tick = GetTickCount() - recvbuf.GetTickCount();
	if (l_ulMilliseconds > l_tick) {
		l_ulMilliseconds = l_ulMilliseconds - l_tick;

		Player *l_ply = (Player*)datasock->GetPointer();
		if (l_ply) {
			MutexArmor l_lockStat(l_ply->m_mtxstat);
			if (l_ply->m_status != 1 || !l_ply->gp_addr) {
				WPacket	l_wpk = datasock->GetWPacket();
				l_wpk.WriteCmd(CMD_MC_DELCHA);
				l_wpk.WriteShort(ERR_MC_NOTSELCHA);
				SendData(datasock, l_wpk);
			}
			else {

				// Call GroupServer
				WPacket l_wpk = WPacket(recvbuf).Duplicate();
				l_wpk.WriteCmd(CMD_TP_UPDATE_PASSWORD2);
				l_wpk.WriteLong(MakeULong(l_ply));
				l_wpk.WriteLong(l_ply->gp_addr);
				l_wpk = g_gtsvr->gp_conn->SyncCall(g_gtsvr->gp_conn->get_datasock(), l_wpk);
				if (!l_wpk.HasData()) {
					l_wpk = datasock->GetWPacket();
					l_wpk.WriteCmd(CMD_MC_UPDATE_PASSWORD2);
					l_wpk.WriteShort(ERR_MC_NETEXCP);
					SendData(datasock, l_wpk);
					Disconnect(datasock, 100, -25);
				}
				else {

					// Return to Client
					l_wpk.WriteCmd(CMD_MC_UPDATE_PASSWORD2);
					SendData(datasock, l_wpk);
					if (RPacket(l_wpk).ReadShort() == ERR_PT_KICKUSER) {
						Disconnect(datasock, 100, -25);
					}
				}
			}
			l_lockStat.unlock();
		}
	}
	else {
		WPacket	l_wpk = datasock->GetWPacket();
		l_wpk.WriteCmd(CMD_MC_UPDATE_PASSWORD2);
		l_wpk.WriteShort(ERR_MC_NETEXCP);
		SendData(datasock, l_wpk);
		Disconnect(datasock, 100, -25);
	}
}

void ToClient::OnEncrypt(DataSocket *datasock, char *ciphertext, const char *text, uLong &len) {
	TcpCommApp::OnEncrypt(datasock, ciphertext, text, len);
	if (_comm_enc > 0) {
		Player* ply = (Player *)datasock->GetPointer();
		if (ply&&ply->enc) {
			try {

#if NET_PROTOCOL_ENCRYPT
				encrypt_Noise(ply->m_szSendKey, ciphertext, len);
#endif

				encrypt_B(ciphertext, len, ply->comm_textkey, ply->comm_key_len);
			}
			catch (...) {
				LogLine l_line(g_gateerr);
				l_line << newln << "GameServer= [" << datasock->GetPeerIP() << "] ToClient::OnEncrypt Error!";
				l_line << endln;
			}
		}

	}
	return;
}

void ToClient::OnDecrypt(DataSocket *datasock, char *ciphertext, uLong &len) {
	TcpCommApp::OnDecrypt(datasock, ciphertext, len);
	if (_comm_enc > 0) {
		Player* ply = (Player *)datasock->GetPointer();
		if (ply&&ply->enc) {
			try {
				encrypt_B(ciphertext, len, ply->comm_textkey, ply->comm_key_len, false);

#if NET_PROTOCOL_ENCRYPT
				decrypt_Noise(ply->m_szRecvKey, ciphertext, len);
#endif

			}
			catch (...) {
				LogLine l_line(g_gateerr);
				l_line << newln << "GameServer= [" << datasock->GetPeerIP() << "] ToClient::OnDecrypt Error!";
				l_line << endln;
			}
		}
	}
	return;
}

void ToClient::post_mapcrash_msg(Player* ply) {
	if (ply->m_datasock == NULL) 
		return;
	
	WPacket pk = ply->m_datasock->GetWPacket();
	pk.WriteCmd(CMD_MC_MAPCRASH);
	pk.WriteString(RES_STRING(GS_TOCLIENT_CPP_00031));
	SendData(ply->m_datasock, pk);
}
