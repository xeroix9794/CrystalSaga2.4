#ifndef GRPNAMESVR_H
#define GRPNAMESVR_H

#include "ThreadPool.h"
#include "CommRPC.h"
#include "PreAlloc.h"
#include "RunCtrlThrd.h"
#include "IniFile.h"
#include "LogStream.h"

#include "GateServer.h"
#include "Player.h"
#include "Guild.h"
#include "Team.h"
#include "SessionChat.h"
#include "DBConnect.h"

#include "i18n.h"

#include "pi_Memory.h"
#include "pi_Alloc.h"

_DBC_USING

struct stPersonInfo {
	char szMotto[40];
	bool bShowMotto;
	char szSex[10];
	short sAge;
	char szName[50];
	char szAnimalZodiac[50];
	char szBloodType[50];
	int iBirthday;
	char szState[50];
	char szCity[50];
	char szConstellation[50];
	char szCareer[50];
	int iSize;
	char pAvatar[8096];
	bool bPprevent;
	int iSupport;
	int iOppose;

	stPersonInfo() {
		memset(this, 0, sizeof(stPersonInfo));
	}
};

struct stQueryPersonInfo {
	char sChaName[64];
	bool bHavePic;
	char cSex[4];
	int	nMinAge[2];
	char szAnimalZodiac[4];
	int	iBirthday[2];


	char szState[32];
	char szCity[32];
	char szConstellation[16];
	char szCareer[32];
	int	nPageItemNum;
	int	nCurPage;

	stQueryPersonInfo() {
		memset(this, 0, sizeof(stQueryPersonInfo));
	}
};

struct stQueryResoultPersonInfo {
	char sChaName[64];
	int	nMinAge;
	char cSex[4];
	char szState[32];
	char nCity[32];

	stQueryResoultPersonInfo() {
		memset(this, 0, sizeof(stQueryResoultPersonInfo));
	}
};

class ToMMS;
class ConnectMMS : public Task {
public:
	ConnectMMS(ToMMS* tmms) {
		_tmms = tmms;
		_timeout = 3000;
	}

private:
	virtual long Process();
	virtual Task *Lastly();
	ToMMS *_tmms;
	DWORD _timeout;
};

class ToMMS : public TcpClientApp, public RPCMGR {
	friend class ConnectMMS;
	struct MMS {
		MMS() : datasock(NULL) {
		}

		string ip; uShort port;
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


// AccountServer from over to prevent multiple landing, because many AccountServer, so here to check.
typedef std::vector<std::string> LoginNameList;
class LoginManageList {
public:
	LoginManageList();
	virtual ~LoginManageList();

	bool Insert(const std::string& name);
	bool Remove(const std::string& name);

	bool Query(const std::string& name);

	void Lock();
	void UnLock();

private:
	bool _Query(const std::string& name);

	CRITICAL_SECTION m_cs;
	LoginNameList m_list;
};

class GroupServerApp : public TcpServerApp, public RPCMGR {
public:
	GroupServerApp(ThreadPool *proc, ThreadPool *comm);
	~GroupServerApp();

	// Use the number of plug-ins...
	uLong m_dwCheatCount;

private:
	friend void Player::EndPlay(DataSocket *datasock);
	void Initialize();
	void Finalize();

	// Returns: true-Allows the connection, false-Disables the connection
	virtual	bool OnConnect(DataSocket *datasock);
	
	virtual void OnDisconnect(DataSocket *datasock, int reason);
	virtual	void OnProcessData(DataSocket *datasock, RPacket &recvbuf);
	virtual	WPacket	OnServeCall(DataSocket *datasock, RPacket &in_para);
	bool GetCHAsFromDBByPlayer(Player *player, WPacket &wpk);

	WPacket	TP_LOGIN(DataSocket *datasock, RPacket &pk);
	WPacket	TP_USER_LOGIN(DataSocket *datasock, RPacket &pk);
	WPacket TP_USER_LOGOUT(Player *ply, DataSocket *datasock, RPacket &pk);
	WPacket	TP_BGNPLAY(Player *ply, DataSocket *datasock, RPacket &pk);
	WPacket TP_ENDPLAY(Player *ply, DataSocket *datasock, RPacket &pk);
	WPacket	TP_NEWCHA(Player *ply, DataSocket *datasock, RPacket &pk);
	WPacket	TP_DELCHA(Player *ply, DataSocket *datasock, RPacket &pk);
	WPacket	TP_CREATE_PASSWORD2(Player *ply, DataSocket *datasock, RPacket &pk);
	WPacket TP_UPDATE_PASSWORD2(Player *ply, DataSocket *datasock, RPacket &pk);

	void MP_ENTERMAP(Player *ply, DataSocket *datasock, RPacket &pk);
	void MP_ONLINE(Player *ply);
	void MP_SWITCH(Player *ply);
	void PC_FRND_INIT(Player *ply);
	void PC_GULD_INIT(Player *ply);
	void PC_MASTER_INIT(Player *ply);

	void MP_TEAM_CREATE(Player *ply, DataSocket *datasock, RPacket &pk);

	void MP_MASTER_CREATE(Player *ply, DataSocket *datasock, RPacket &pk);
	void MP_MASTER_DEL(Player *ply, DataSocket *datasock, RPacket &pk);
	void MP_MASTER_FINISH(Player *ply, DataSocket *datasock, RPacket &pk);

	WPacket	TP_REQPLYLST(DataSocket *datasock, RPacket &pk);

	void KillUserByName(const char* accoutName);

	void AP_KICKUSER(DataSocket *datasock, RPacket &pk);
	void AP_KICKUSER2(DataSocket* datasock, uLong acctid);
	void AP_EXPSCALE(DataSocket* datasock, RPacket &pk);

	void TP_DISC(DataSocket *datasock, RPacket &pk);
	void TP_ESTOPUSER_CHECK(DataSocket *datasock, RPacket &pk);

	void CP_TEAM_INVITE(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_TEAM_ACCEPT(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_TEAM_REFUSE(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_TEAM_LEAVE(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_TEAM_KICK(Player *ply, DataSocket *datasock, RPacket &pk);

	void CP_FRND_INVITE(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_FRND_ACCEPT(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_FRND_REFUSE(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_FRND_DELETE(Player *ply, DataSocket *datasock, RPacket &pk);

	void CP_FRND_DEL_GROUP(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_FRND_ADD_GROUP(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_FRND_MOVE_GROUP(Player *ply, DataSocket *datasock, RPacket &pk);


	void CP_FRND_CHANGE_GROUP(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_FRND_REFRESH_INFO(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_CHANGE_PERSONINFO(Player *ply, DataSocket *datasock, RPacket &pk);

	void CP_QUERY_PERSONINFO(Player *ply, DataSocket *datasock, RPacket &pk);

	void CP_MASTER_INVITE(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_MASTER_ACCEPT(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_MASTER_REFUSE(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_MASTER_DELETE(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_PRENTICE_DELETE(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_MASTER_REFRESH_INFO(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_PRENTICE_REFRESH_INFO(Player *ply, DataSocket *datasock, RPacket &pk);

	void CP_REFUSETOME(Player *ply, DataSocket *datasock, RPacket &pk);

#ifdef MANAGE_VER	
	void CP_GMSAY(Player *ply, DataSocket *datasock, RPacket &pk);
#endif

	void CP_GM1SAY(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_GM1SAY1(Player *ply, DataSocket *datasock, RPacket &pk);

	void CP_SAY2TRADE(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_SAY2ALL(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_SAY2YOU(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_SAY2TEM(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_SAY2GUD(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_SAY2GUILD(Player *ply, DataSocket *datasock, RPacket &pk);

	void MP_SAY2ALL(Player *ply, DataSocket *datasock, RPacket &pk);
	void MP_SAY2TRADE(Player *ply, DataSocket *datasock, RPacket &pk);

	void CP_SESS_CREATE(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_SESS_SAY(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_SESS_ADD(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_SESS_LEAVE(Player *ply, DataSocket *datasock, RPacket &pk);

	void MP_GUILD_APPROVE(Player *ply, DataSocket *datasock, RPacket &pk);
	void MP_GUILD_CREATE(Player *ply, DataSocket *datasock, RPacket &pk);
	void MP_GUILD_KICK(Player *ply, DataSocket *datasock, RPacket &pk);
	void MP_GUILD_LEAVE(Player *ply, DataSocket *datasock, RPacket &pk);
	void MP_GUILD_DISBAND(Player *ply, DataSocket *datasock, RPacket &pk);
	void MP_GUILD_MOTTO(Player *ply, DataSocket *datasock, RPacket	&pk);
	void MP_GUILD_CHALLMONEY(Player *ply, DataSocket *datasock, RPacket &pk);
	void MP_GUILD_CHALL_PRIZEMONEY(Player *ply, DataSocket *datasock, RPacket &pk);

	void CP_GUILD_INVITE(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_GUILD_ACCEPT(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_GUILD_REFUSE(Player *ply, DataSocket *datasock, RPacket &pk);

	void MP_GM_BANACCOUNT(Player *ply, DataSocket *datasock, RPacket &pk);

	void CP_PING(Player *ply, DataSocket *datasock, RPacket &pk);
	void CP_REPORT_WG(Player *ply, DataSocket *datasock, RPacket &pk);

	WPacket	TP_SYNC_PLYLST(DataSocket *datasock, RPacket &pk);
	WPacket	OS_LOGIN(DataSocket *datasock, RPacket &pk);
	void OS_PING(DataSocket *datasock, RPacket &pk);

	void MP_GARNER2_UPDATE(Player *ply, DataSocket *datasock, RPacket &pk);

	bool CheckFunction(string mapName, string funName);

public:
	// The client requests rankings
	void CP_GARNER2_GETORDER(Player *ply, DataSocket *datasock, RPacket &pk);
	
	void KickUser(DataSocket *datasock, uLong gpaddr, uLong gtaddr);
	void SendToClient(Player* ply[], short cli_num, const WPacket &wpk);
	void SendToClient(Player* ply, WPacket &wpk);

	GateServer *FindGateSByName(cChar *gatename);
	Guild *FindGuildByName(cChar *gldname);
	Guild *FindGuildByGldID(uLong	gldid);
	Guild *FindGuildByLeadID(uLong chaid);
	Player *FindPlayerByChaName(cChar *plyname);
	Player *FindPlayerByChaID(uLong chaid);

#ifdef MANAGE_VER	
	void Execute(Player *ply, std::string msg[]);
#endif

	Player *GetPlayerByChaID(uLong chaid);
	bool AddPlayerToList(uLong chaid, Player *pPlayer);
	bool DelPlayerFromList(uLong chaid);

	enum{ GATE_MAX = 50 };
	GateServer m_gate[GATE_MAX];

	GroupServerAgent m_groupServerAgent;

	struct {
		Mutex m_mtxlogin;
		Mutex m_mtxSyn;
		short m_gatenum;
		DataSocket *m_acctsock;
		dstring m_name;
		Mutex m_mtxAgent;
	};

	// Internal data structure
	PreAllocHeap<Player> m_plyheap;
	RunBiDirectChain<Player> m_plylst;
	PreAllocHeap<Guild> m_gldheap;
	RunBiDirectChain<Guild> m_gldlst;
	PreAllocHeap<Team> m_teamheap;
	RunBiDirectChain<Team> m_teamlst;
	RunCtrlMgr<Invitation> m_plyproc;

	RunBiDirectChain<Chat_Session> m_sesslst;
	Chat_Session *AddSession();
	void  DelSession(Chat_Session *sess);

	// Database connection£º
	struct {
		Mutex m_mtxDB;
		cfl_db m_cfg_db;
		TBLAccounts	*m_tblaccounts;
		TBLCharacters *m_tblcharaters;

		TBLPersonInfo* m_tblpersoninfo;
		TBLPersonAvatar* m_tblpersonavatar;

		TBLFriends *m_tblfriends;
		TBLMaster *m_tblmaster;
		TBLGuilds *m_tblguilds;
		TBLParam *m_tbLparam;
		friend_tbl *m_tblX1;

#ifdef SHUI_JING
		TBLCrystalTrade * m_tblcrystal;
#endif

		IniFile	m_cfg;
		InterLockedLong	m_curChaNum;
		InterLockedLong	m_curWGChaNum;
	};

	struct {
		char MaxChaNum;
		short MaxIconVal;
		short MaxLoginUsr;
	} const_cha;

	struct {
		char InvitedMax;		// = 5;
		uLong PendTimeOut;		// = 3 0 * 1000
		char  FriendMax;		// = 100
		char FriendGroupMax;	// = 10
	} const_frnd;

	struct {
		char InvitedMax;		// = 5
		uLong PendTimeOut;		// = 30 * 1000
		char  MemberMax;		// = 5
	} const_team;

	struct {
		char InvitedMax;		// = 5;
		uLong PendTimeOut;		// = 30 * 1000
		char MasterMax;			// = 1
		char PrenticeMax;		// = 4
	} const_master;

	struct {
		char InvitedMax;
		uLong PendTimeOut;
		char GuildMemberMax;
	} const_guild;

	struct {
		uShort MaxSession;		// = 3
		uShort MaxPlayer;		// = 30
	} const_chat;

	struct {
		uLong World;
		uLong Trade;
		uLong ToYou;
	} const_interval;

private:
	bool InitMasterRelation();
	int GetMasterCount(uLong cha_id);
	int GetPrenticeCount(uLong cha_id);
	int HasMaster(uLong cha_id1, uLong cha_id2);
	bool AddMaster(uLong cha_id1, uLong cha_id2);
	bool DelMaster(uLong cha_id1, uLong cha_id2);
	bool FinishMaster(uLong cha_id);

	map<uLong, uLong> m_mapMasterRelation;	// Mentoring relationship
	map<uLong, Player *> m_mapPlayerList;	// Role mapping table
	map<string, string> m_mapBirthplace;

	LoginManageList m_LoginList;			// Log in all account records to prevent double login
};

class GMBBS :public Timer {
	enum {
		em_words = 30
	};

public:
	GMBBS(uLong interval);
	bool AddBBS(uLong inter, uLong times, const char *word);

private:
	~GMBBS();
	void Process();
	struct {
		uLong volatile m_start;
		uLong volatile m_inter;
		uLong volatile m_times;
		dstring m_word;
	} m_queue[em_words];

	Mutex m_mtxque;

	uLong	m_dwCount;
};

// AccountServer Connection
extern void InitACTSvrConnect(GroupServerApp &gpapp);
extern GroupServerApp *g_gpsvr;
extern InterLockedLong g_exit;
extern InterLockedLong g_ref;
extern LogStream g_LogGrpServer;
extern LogStream g_LogErrServer;
extern LogStream g_LogGuild;
extern LogStream g_LogFriend;
extern LogStream g_LogMaster;
extern LogStream g_LogTeam;
extern LogStream g_LogConnect;
extern LogStream g_LogDB;
extern LogStream g_LogGarner2;
extern GMBBS *g_gmbbs;

#endif //GRPNAMESVR_H
