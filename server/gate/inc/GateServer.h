#ifndef _GATESERVER_H_
#define _GATESERVER_H_

#include <iostream>
#include <map>
#include <time.h>
#include <dstring.h>
#include <datasocket.h>
#include <threadpool.h>
#include <commrpc.h>
#include <point.h>
#include <inifile.h>
#include <gamecommon.h>
#include <prealloc.h>
#include <ntservice.h>
#include <LogStream.h>
#include <algo.h>

#include <stdlib.h>

#include "i18n.h"

#include "pi_Memory.h"
#include "pi_Alloc.h"

#include "util.h"

using namespace std;
using namespace dbc;

// Timed exit mechanism Macros
// #define CHAEXIT_ONTIME
extern char region[3];

enum PlayStatus {

};

// About the Client Connection section
struct Player;
class ToClient : public TcpServerApp, public RPCMGR {
	friend class TransmitCall;

public:
	ToClient(char const* fname, ThreadPool* proc, ThreadPool* comm);
	~ToClient();

	void post_mapcrash_msg(Player* ply);
	dstring	GetDisconnectErrText(int reason);

	void SetMaxCon(uShort maxcon) {
		m_maxcon = maxcon; 
	}

	uShort GetMaxCon() { 
		return m_maxcon; 
	}
	
	void CM_LOGIN(DataSocket* datasock, RPacket& recvbuf);
	WPacket CM_LOGOUT(DataSocket* datasock, RPacket& recvbuf);
	WPacket CM_DISCONN(DataSocket* datasock, RPacket& recvbuf);
	void CM_BGNPLAY(DataSocket* datasock, RPacket& recvbuf);
	void CM_ENDPLAY(DataSocket* datasock, RPacket& recvbuf);
	void CM_NEWCHA(DataSocket* datasock, RPacket& recvbuf);
	void CM_DELCHA(DataSocket* datasock, RPacket& recvbuf);
	void CM_CREATE_PASSWORD2(DataSocket* datasock, RPacket& recvbuf);
	void CM_UPDATE_PASSWORD2(DataSocket* datasock, RPacket& recvbuf);
	
	uShort GetVersion() { 
		return m_version; 
	}
	
	int GetCallTotal() {
		return m_calltotal; 
	}

#ifdef NET_CAL
	uShort GetCheckSpan() { 
		return m_checkSpan; 
	}

	uShort GetCheckWaring() { 
		return m_checkWaring; 
	}

	uShort GetCheckError() { 
		return m_checkError; 
	}

	void SetCheckSpan(uShort checkSpan);
	void SetCheckWaring(uShort checkWaring);
	void SetCheckError(uShort checkError);
#endif

private:
	bool DoCommand(DataSocket* datasock, cChar *cmdline);

	// Return value: true-Allow connection, false-Connection is not allowed

	virtual bool OnConnect(DataSocket* datasock);
	virtual void OnConnected(DataSocket* datasock);
	virtual void OnDisconnect(DataSocket* datasock, int reason);
	virtual void OnProcessData(DataSocket* datasock, RPacket &recvbuf);
	virtual WPacket OnServeCall(DataSocket* datasock, RPacket &in_para);
	
	virtual bool OnSendBlock(DataSocket	*datasock) {
		return false; 
	}

	// Communication Encryption
	virtual void OnEncrypt(DataSocket *datasock, char *ciphertext, const char *text, uLong &len);
	virtual void OnDecrypt(DataSocket *datasock, char *ciphertext, uLong &len);

	InterLockedLong	m_atexit, m_calltotal;
	volatile uShort m_maxcon;
	uShort m_version;

	// Encryption Algorithm Index
	int _comm_enc;

#ifdef NET_CAL
	volatile uShort	m_checkSpan;
	volatile uShort	m_checkWaring;
	volatile uShort	m_checkError;

	// Check the number of duplicate IPs
	// volatile uShort m_checkIPNum;

	// Check Duplicate IP Interval
	// volatile uShort m_checkIPSpan;

#endif

	IMPLEMENT_CDELETE(ToClient)
};

// About the GameServer connection section

class ToGameServer;

#define MAX_MAP 100
#define MAX_GAM 100

struct GameServer : public PreAllocStru {
	friend class PreAllocHeap < GameServer > ;

private:
	GameServer(uLong Size) : PreAllocStru(Size), m_datasock(NULL), next(NULL) {}
	~GameServer() {
		if (m_datasock != NULL) {
			m_datasock->SetPointer(NULL);
			m_datasock = NULL;
		}
	}

	GameServer(GameServer const&) : PreAllocStru(1) {
	}

	GameServer& operator =(GameServer const&) {
	}

	void Initially();
	void Finally();

public:
	// The role of the map into the map of the server location in the map (x, y) winer specify whether the role is the king of chaos.
	void EnterMap(Player *ply, uLong actid, uLong dbid, uLong worldid, cChar *map, Long lMapCpyNO, uLong x, uLong y, char entertype, short swiner);

	// GameServer encountered abnormal closure
	void DeleteGameServer(int reason);

public:
	InterLockedLong m_plynum;
	string gamename;
	string ip;
	uShort port;
	DataSocket* m_datasock;
	GameServer* next;
	string maplist[MAX_MAP];
	uShort mapcnt;
};

class ToGameServer : public TcpServerApp, public RPCMGR {
	friend class ToGroupServer;

public:
	ToGameServer(char const* fname, ThreadPool* proc, ThreadPool* comm);
	~ToGameServer();

	GameServer* find(cChar* mapname);

	void SendAllGameServer(WPacket &in_para);
private:
	virtual bool OnConnect(DataSocket* datasock); 
	virtual void OnDisconnect(DataSocket* datasock, int reason);
	virtual	WPacket OnServeCall(DataSocket* datasock, RPacket &in_para);
	virtual void OnProcessData(DataSocket* datasock, RPacket &recvbuf);

	PreAllocHeap<GameServer> _game_heap;						// GameServer describes the object heap
	void MT_LOGIN(DataSocket* datasock, RPacket& recvbuf);		// GameServer Login to the GateServer

	GameServer* _game_list;									   // Stores a linked list of GameServer description objects
	short _game_num;
	void _add_game(GameServer* game);
	bool _exist_game(char const* game);
	void _del_game(GameServer* game);
	map<string, GameServer*> _map_game;						  // The map name corresponds to the GameServer description object
	Mutex _mut_game;

	IMPLEMENT_CDELETE(ToGameServer)
};

// About the GroupServer connection section
class ToGroupServer;
class ConnectGroupServer : public Task {
public:
	ConnectGroupServer(ToGroupServer* tgts) { 
		_tgps = tgts; 
		_timeout = 3000; 
	}

private:
	virtual long Process();
	virtual Task* Lastly();

	ToGroupServer* _tgps;
	DWORD _timeout;
};

struct GroupServer {
	GroupServer() : datasock(NULL), next(NULL) {
	}
	string ip; uShort port;
	DataSocket* datasock;
	GroupServer* next;
};

class ToGroupServer : public TcpClientApp, public RPCMGR {
	friend class ConnectGroupServer;
	friend void ToGameServer::MT_LOGIN(DataSocket* datasock, RPacket& rpk);

public:
	ToGroupServer(char const* fname, ThreadPool* proc, ThreadPool* comm);
	~ToGroupServer();

	DataSocket* get_datasock() const { 
		return _gs.datasock; 
	}
	RPacket get_playerlist();

	int GetCallTotal() {
		return m_calltotal;
	}

	bool IsSync() {
		return m_bSync; 
	}

	void SetSync(bool sync = true) { 
		m_bSync = sync; 
	}

	// Get Ready
	bool IsReady() { 
		return (!m_bSync && _connected); 
	}

private:
	virtual bool OnConnect(DataSocket* datasock); 
	virtual void OnDisconnect(DataSocket* datasock, int reason);
	virtual void OnProcessData(DataSocket* datasock, RPacket &recvbuf);
	virtual WPacket OnServeCall(DataSocket* datasock, RPacket &in_para);

	InterLockedLong	m_atexit, m_calltotal;

	// GateServer 
	string _myself;

	GroupServer _gs;
	bool volatile _connected;
	bool volatile m_bSync;

	IMPLEMENT_CDELETE(ToGroupServer)
};

class ToMMS;
class ConnectMMS : public Task {
public:
	ConnectMMS(ToMMS* tmms) {
		_tmms = tmms; _timeout = 3000; 
	}

private:
	virtual long Process();
	virtual Task* Lastly();

	ToMMS* _tmms;
	DWORD _timeout;
};

class ToMMS : public TcpClientApp, public RPCMGR {
	friend class ConnectMMS;
	struct MMS {
		MMS() : datasock(NULL) {
		}

		string ip; 
		uShort port;
		string type;
		uShort key;

		DataSocket* datasock;
	};

public:
	ToMMS(const char* configeName, ThreadPool* proc, ThreadPool* comm);
	~ToMMS();

	int GetCallTotal() { 
		return m_calltotal; 
	}

	void Login(cChar* ip, uShort port, int accountID);
	void Logout(cChar* ip, uShort port, int accountID);

private:
	virtual bool OnConnect(DataSocket* datasock);
	virtual void OnDisconnect(DataSocket* datasock, int reason);
	virtual void OnProcessData(DataSocket* datasock, RPacket &recvbuf);
	virtual WPacket OnServeCall(DataSocket* datasock, RPacket &in_para);

	InterLockedLong	m_atexit, m_calltotal;
	bool volatile _connected;
	MMS	m_MMS;
};

// GateServer itself
struct Player : public PreAllocStru, public RunBiDirectItem < Player > {
	friend class PreAllocHeap < Player > ;
	friend class DelayLogout;

public:
	bool InitReference(DataSocket* datasock);
	void Free() { 
		PreAllocStru::Free(); 
	}

	bool BeginRun();
	bool EndRun();

	char m_chapstr[20];
	char m_password[ROLE_MAXSIZE_PASSWORD2 + 1];
	char m_szSendKey[4];
	char m_szRecvKey[4];

	uLong volatile m_actid;
	uLong volatile m_loginID;
	uLong volatile m_dbid;				// The database ID of the current role
	uLong volatile m_worldid;			// The unique ID of the current role
	uLong volatile m_pingtime;
	uInt volatile comm_key_len;			// Communication key length
	char comm_textkey[12];				// The key for encrypted communication between Game Server and Client
	InterLockedLong gm_addr;			// A pointer to the Player object on the GameServer
	InterLockedLong gp_addr;			// A pointer to the Player object on the GroupServer
	DataSocket* volatile m_datasock;	// This Player's GateServer <-> Client connection
	GameServer* volatile game;			// This Player is the current GameServer description object for this Player
	volatile bool enc;					// Whether the communication data is encrypted

	// Whether or not to speak
	uLong volatile m_lestoptick;
	bool volatile m_estop;
	short volatile m_sGarnerWiner;

	struct {
		Mutex m_mtxstat;						// 0: Lock m_status;
		volatile char m_status;					// 0: Invalid; 1. Select the role during; 2. Play the game.
		volatile char m_switch;					// 0: invalid; 1 switch map (to prevent the map switch, because the server slow response, resulting in two players at the same time in two maps exist)

#ifdef NET_CAL	
		volatile uLong m_cmdNum;				// Record the number of commands within a second, analyze the network status
		volatile uLong m_lashTick; 
		volatile uLong m_waringNum;				// Record the number of commands within a second, analyze the network status
#endif

	};

private:
	Player(uLong Size) : PreAllocStru(Size), m_datasock(NULL), game(NULL), gm_addr(0), gp_addr(0), m_dbid(0), m_worldid(0), m_status(0), m_sGarnerWiner(0) {
		m_mtxstat.Create(false);
	}

	~Player() {
		if (m_datasock != NULL) {
			m_datasock->SetPointer(NULL); 
			m_datasock = NULL; 
		}
	}

	Player(Player const&) : PreAllocStru(1) {
	}

	Player& operator =(Player const&) {
	}

	void Initially(); 
	void Finally();
};

class TransmitCall :public PreAllocTask {
public:
	TransmitCall(uLong size) : PreAllocTask(size) {
	};

	void Init(DataSocket* datasock, RPacket &recvbuf) {
		m_datasock = datasock; m_recvbuf = recvbuf; 
	}

	long Process();

	DataSocket* m_datasock;
	RPacket m_recvbuf;
};

class GateServer {
public:
	GateServer(char const* fname);
	~GateServer();

	void RunLoop(); // The main loop

	ThreadPool	*m_clproc, *m_clcomm, *m_gpcomm, *m_gpproc, *m_gmcomm;

	/*
	// Task pool with MMS
	ThreadPool * m_mmsproc, * m_mmscomm;

	// Connection object with MMS (active reconnection mechanism)
	ToMMS * m_ToMMS;
	*/

	// Connection object with GroupServer (active reconnection mechanism)
	ToGroupServer* gp_conn;

	// Connection object with GameServer (passive)
	ToGameServer* gm_conn;

	// Connection object with the client (passive)
	ToClient* cli_conn;
	Mutex _mtxother;

	// Player object heap
	PreAllocHeap<Player> player_heap;

	// Players with tables
	RunBiDirectChain<Player> m_plylst;

	PreAllocHeap<TransmitCall>	m_tch;

	IMPLEMENT_CDELETE(GateServer)
};

extern GateServer* g_gtsvr;
extern bool volatile g_appexit;

class CUdpManage;
class CUdpServer;
class GateServerApp : public NTService {
public:
	GateServerApp();
	void ServiceStart();
	void ServiceStop();

	virtual cChar *SetSvcName() const { 
		return "GateServer"; 
	}

	// The display name defaults to the service name
	virtual cChar *SetDispName() const {
		return "[Kop Online]Gate Server"; 
	}
	
	// The default supports pausing and resuming operations
	virtual bool CanPaused() const { 
		return true; 
	}

	/*

	int GetShowMin() { 
		return _nShowMin;		
	}
	
	int GetShowMax() { 
		return _nShowMax;		
	}

	void SetShowRange(int min, int max) {
		_nShowMin=min; _nShowMax=max;	
	}
	*/
private:	
	
	/*
	// Used to get the game statistics through udp before the client is connected.
	CUdpManage* _pUdpManage;
	static void _NotifySocketNumEvent(CUdpManage* pManage, CUdpServer* pUdpServer, const char* szClientIP, unsigned int nClientPort, const char* pData, int len);
	static int _nShowMin;	
	static int _nShowMax;	
	*/
};

extern LogStream g_gateerr;
extern LogStream g_gatelog;
extern LogStream g_chkattack;
extern LogStream g_gateconnect;
// extern LogStream g_gatepacket;
extern GateServerApp* g_app;
extern LogStream g_Loginlog;

#endif // _GATESERVER_H_
