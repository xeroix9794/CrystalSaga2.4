#include <LogStream.h>
#include "AccountServer2.h"
#include "gamecommon.h"
#include "inifile.h"
#include "util.h"
#include "GlobalVariable.h"

LogStream g_MMSConnect("MMS_Connect");

long ConnectMMS::Process() {
	_tmms->m_calltotal++;
	DataSocket* datasock = NULL;
	while (!GetExitFlag() && !_tmms->m_atexit) {		
		if (_tmms->_connected) {

			// Already connected
			Sleep(1000);
		} 
		else {
			LogLine l_line(g_MMSConnect);
			l_line<<newln<<"Connect MapManageServer Begin"<<endln;
			datasock = _tmms->Connect(_tmms->m_MMS.ip.c_str(), _tmms->m_MMS.port);
			if (datasock == NULL) {
				LogLine l_line(g_MMSConnect);
				l_line<<newln<<"Connect MapManageServer failed, will retry 5 ms later"<<endln;
				Sleep(5000);
				continue;
			} else {
				// Log on to GroupServer
				WPacket pk = _tmms->GetWPacket();
				pk.WriteCmd(CMD_SO_LOGIN);
				pk.WriteString(_tmms->m_MMS.type.c_str());
				pk.WriteShort(_tmms->m_MMS.key);

				RPacket retpk = _tmms->SyncCall(datasock, pk);
				int err = retpk.ReadShort();
				if (!retpk.HasData() || err == ERR_OS_NOTMATCH_VERSION) {
					LogLine l_line(g_MMSConnect);
					l_line<<newln<<"Login MapManageServer failed, will retry 5 ms later"<<endln;
					datasock = NULL;
					Sleep(5000);
					_tmms->Disconnect(datasock);
				}
				else {
					LogLine l_line(g_MMSConnect);
					l_line<<newln<<"Login MapManageServer success"<<endln;
					_tmms->m_MMS.datasock = datasock;
					_tmms->_connected = true;
					datasock = NULL;
				}
			}
		}
	}
	return 0;
}

Task *ConnectMMS::Lastly() {
	--(_tmms->m_calltotal);
	return Task::Lastly();
}

ToMMS::ToMMS(const char* configeName, ThreadPool* proc, ThreadPool* comm)  : TcpClientApp(this, proc, comm), RPCMGR(this), m_MMS(), m_calltotal(0), m_atexit(0), _connected(false) {
	IniFile ini(configeName);
	IniSection& is = ini["MMS"];
	
	m_MMS.type = is["Type"];
	m_MMS.key = atoi(is["Key"]);
	m_MMS.ip = is["IP"];
	m_MMS.port = atoi(is["Port"]);

	SetPKParse(0, 2, 4 * 1024, 100);
	BeginWork(atoi(is["EnablePing"]));
}

ToMMS::~ToMMS() {
	m_atexit = 1;
	while(m_calltotal) {
		Sleep(1); }
	ShutDown(12 * 1000);
}

bool ToMMS::OnConnect(DataSocket* datasock) {
	datasock->SetRecvBuf(64 * 1024);
	datasock->SetSendBuf(64 * 1024);
	LogLine l_line(g_MMSConnect);
	l_line<<newln<<"connect MMS Server: "<<datasock->GetPeerIP()<<",Socket num:"<<GetSockTotal()+1;
	return true;
}

void ToMMS::OnDisconnect(DataSocket* datasock, int reason) {
	LogLine l_line(g_MMSConnect);
	l_line<<newln<<"disconnection with MMS Server,Socket num: "<<GetSockTotal()<<",reason ="<<GetDisconnectErrText(reason).c_str()<<", reconnecting..."<<endln;
	_connected = false;
}


void ToMMS::OnProcessData(DataSocket* datasock, RPacket &recvbuf) {
	uShort	l_cmd	=recvbuf.ReadCmd();
	switch(l_cmd) {
	case CMD_OS_PING:
		WPacket sendbuf = this->GetWPacket();
		sendbuf.WriteCmd(CMD_SO_PING);
		sendbuf.WriteString(this->m_MMS.type.c_str());
		sendbuf.WriteShort(this->m_MMS.key);
		sendbuf.WriteLongLong(recvbuf.ReadLongLong()); // TimeStamp
		this->SendData(datasock, sendbuf);
		break;
	}
}

WPacket ToMMS::OnServeCall(DataSocket* datasock, RPacket &in_para) {
	return NULL;
}

void ToMMS::Login(cChar* ip, uShort port, int accountID) {
	WPacket wPacket = this->GetWPacket();
	wPacket.WriteCmd(CMD_SO_ON_LINE);
	wPacket.WriteString(this->m_MMS.type.c_str());
	wPacket.WriteShort(this->m_MMS.key);
	wPacket.WriteString(ip);
	wPacket.WriteShort(port);
	wPacket.WriteLong(accountID);
	SendData(m_MMS.datasock, wPacket);
}

void ToMMS::Logout(cChar* ip, uShort port, int accountID) {
	WPacket wPacket = this->GetWPacket();
	wPacket.WriteCmd(CMD_SO_OFF_LINE);
	wPacket.WriteString(this->m_MMS.type.c_str());
	wPacket.WriteShort(this->m_MMS.key);
	wPacket.WriteString(ip);
	wPacket.WriteShort(port);
	wPacket.WriteLong(accountID);
	SendData(m_MMS.datasock,wPacket);
}