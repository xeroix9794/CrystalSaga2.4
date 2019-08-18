#include <LogStream.h>
#include "AccountServer2.h"
#include "gamecommon.h"
#include "inifile.h"
#include "util.h"
#include "GlobalVariable.h"

LogStream g_BillingConnect("Billing_Connect");

long ConnectBilling::Process() {

	// Does not start
	if (!_billing->m_Billing.enable)
		return 0;

	_billing->m_calltotal++;

	DataSocket* datasock = NULL;
	while (!GetExitFlag() && !_billing->m_atexit) {
		if (_billing->_connected) {

			// Already connected
			Sleep(1000);
		}
		else {
			LogLine l_line(g_BillingConnect);
			l_line << newln << "Connect BillingServer Begin" << endln;
			datasock = _billing->Connect(_billing->m_Billing.ip.c_str(), _billing->m_Billing.port);
			if (datasock == NULL) {
				LogLine l_line(g_BillingConnect);
				l_line << newln << "Connect BillingServer failed, will retry 5 ms later" << endln;
				Sleep(5000);
				continue;
			}
			else {

				// Log on to GroupServer
				WPacket pk = _billing->GetWPacket();
				pk.WriteCmd(CMD_AB_LOGIN);

				RPacket retpk = _billing->SyncCall(datasock, pk);
				int err = retpk.ReadShort();
				if (!retpk.HasData() || err != ERR_SUCCESS) {
					LogLine l_line(g_BillingConnect);
					l_line << newln << "Login BillingServer failed, will retry 5 ms later" << endln;
					datasock = NULL;
					Sleep(5000);
					_billing->Disconnect(datasock);
				}
				else {
					LogLine l_line(g_BillingConnect);
					l_line << newln << "Login BillingServer success" << endln;
					_billing->m_Billing.datasock = datasock;
					_billing->_connected = true;
					datasock = NULL;
				}
			}
		}
	}
	return 0;
}

Task *ConnectBilling::Lastly() {
	--(_billing->m_calltotal);
	return Task::Lastly();
}

ToBilling::ToBilling(const char* configeName, ThreadPool* proc, ThreadPool* comm) : TcpClientApp(this, proc, comm), RPCMGR(this), m_Billing(), m_calltotal(0), m_atexit(0), _connected(false) {
	IniFile ini(configeName);
	IniSection& is = ini["Billing"];

	m_Billing.enable = atoi(is["Enable"]);
	m_Billing.killuser = atoi(is["Kickuser"]);
	m_Billing.ip = is["IP"];
	m_Billing.port = atoi(is["Port"]);

	SetPKParse(0, 2, 4 * 1024, 100);
	BeginWork(atoi(is["EnablePing"]));
}

ToBilling::~ToBilling() {
	m_atexit = 1;
	while (m_calltotal) {
		Sleep(1);
	}
	ShutDown(12 * 1000);
}

bool ToBilling::OnConnect(DataSocket* datasock) {
	datasock->SetRecvBuf(64 * 1024);
	datasock->SetSendBuf(64 * 1024);
	LogLine l_line(g_BillingConnect);
	l_line << newln << "connect Billing Server: " << datasock->GetPeerIP() << ",Socket num:" << GetSockTotal() + 1;
	return true;
}

void ToBilling::OnDisconnect(DataSocket* datasock, int reason) {
	LogLine l_line(g_BillingConnect);
	l_line << newln << "disconnection with Billing Server,Socket num: " << GetSockTotal() << ",reason =" << GetDisconnectErrText(reason).c_str() << ", reconnecting..." << endln;
	_connected = false;
}

void ToBilling::OnProcessData(DataSocket* datasock, RPacket &recvbuf) {
	if (_connected == false)
		return;

	uShort	l_cmd = recvbuf.ReadCmd();
	string name;

	switch (l_cmd) {
	case CMD_BA_USER_TIME: {
		cShort hour = recvbuf.ReadShort();
		cShort num = recvbuf.ReadShort();
		for (int i = 0; i < num; i++) {
			name = recvbuf.ReadString();

			g_MainDBHandle.SetExpScale(name, hour);
		}
	}

	// Read information
	break;
	case CMD_BA_KILL_USER:
		if (this->m_Billing.killuser) {
			cShort num = recvbuf.ReadShort();
			for (int i = 0; i < num; i++) {
				name = recvbuf.ReadString();
				if (!g_MainDBHandle.KickUser(name)) {
					LG("AccountServer", "CBillService::VerifyBillingCode: KickUser failed! UserName=%s\r\n", name.c_str());
				}
				else {
					LG("AccountServer", "CBillService::VerifyBillingCode: KickUser success! UserName=%s\r\n", name.c_str());
				}
			}
		}
		break;
	}
}

WPacket ToBilling::OnServeCall(DataSocket* datasock, RPacket &in_para) {
	return NULL;
}

void ToBilling::UserLogin(std::string strUserName, std::string strPassport) {
	WPacket wPacket = this->GetWPacket();
	wPacket.WriteCmd(CMD_AB_USER_LOGINT);
	wPacket.WriteString(strUserName.c_str());
	wPacket.WriteString(strPassport.c_str());
	SendData(m_Billing.datasock, wPacket);
}

void ToBilling::UserLogout(std::string strUserName) {
	WPacket wPacket = this->GetWPacket();
	wPacket.WriteCmd(CMD_AB_USER_LOGOUT);
	wPacket.WriteString(strUserName.c_str());
	SendData(m_Billing.datasock, wPacket);
}