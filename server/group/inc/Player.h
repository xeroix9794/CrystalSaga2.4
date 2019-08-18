#ifndef PLAYER_H
#define PLAYER_H

#include <xstring>

#include "PreAlloc.h"
#include "RunBiDirectChain.h"
#include "TeamMember.h"
#include "GuildMember.h"
#include "GateServer.h"
#include "Friend.h"
#include "Master.h"
#include "SessionChat.h"
#include "Guild.h"

class Team;

_DBC_USING

class Player : public PreAllocStru, public RunBiDirectItem < Player > , public TeamMember, public GuildMember , public Friend, public Master {
public:
	enum { 
		emMaxCharacters = 10,
		emMaxSessChat = 30, 
		emMaxSessPlayer = 100 
	};

	Player(uLong size);
	void Free(){ PreAllocStru::Free(); }
	bool BeginRun();
	bool EndRun();
	void EndPlay(DataSocket *datasock);
	void EndPlayReset();

	void DoCommand(cChar *cmd);
	void CountEstopTime();
	void EstopPlayer(cChar* plyname, uLong lTimes);
	void DelEstopPlayer(cChar* plyname);
	void DisablePlayer(cChar* plyname, uLong lTimes);
	void SendSysInfo(cChar	*info);
	char FindIndexByChaName(cChar *chaname);
	Chat_Session *FindSessByID(uLong sessid);
	bool IsEstop();
	void CheckEstop();
	struct {
		uLong m_acctLoginID;			// Account Server Account ID
		uLong m_acctid;
		uLong m_sessid;
		std::string m_acctname;			// Account Name
		std::string m_passport;							
		std::string m_password;			// Secondary password
		char m_gm;
		char m_clientip[16];			// The client IP address
		char m_lastip[16];				// Last login IP
		char m_lastreason[50];			// Disconnect Reason last time
		char m_lastleavetime[21];
		Player *m_pingply;
	};

	struct {
		Mutex m_mtxCha;
		char m_chanum;								// Number of roles
		char m_bp_currcha;							// BeginPlay time to determine the currcha
		char m_currcha;								// The current role index
		long m_chaid[emMaxCharacters];				// Role ID list
		std::string m_chaname[emMaxCharacters];		// A list of role names
		std::string m_motto[emMaxCharacters];		// Role motto
		short m_icon[emMaxCharacters];				// A small character icon
		uLong m_guild[emMaxCharacters];				// Association ID
		bool m_estop[emMaxCharacters];				// Whether it was banned
		unsigned long m_guildPermission[emMaxCharacters];
		bool volatile m_bWG;						// Whether to use the plug
	};

	struct {
		InterLockedLong m_chatnum;					// Number of current session chats
		bool volatile m_refuse_tome;
		bool volatile m_refuse_sess;
		Mutex m_mtxChat;
		uLong volatile m_chatarranum;
		Chat_Player *volatile m_chat[emMaxSessChat];
	};

	struct {
		InterLockedLong m_worldtick;
		InterLockedLong m_worldticktemp;
		InterLockedLong m_tradetick;
		InterLockedLong m_tradeticktemp;
		InterLockedLong m_toyoutick;
	};
	
	// * @supplierCardinality 1..*
	GateServer *m_gate;
	uLong m_gtAddr;									// The ID assigned on the Game Server is actually the memory address of the internal data structure.
	bool m_bCheat;
	long m_lChatMoney;
	long m_lTradeChatMoney;
	bool m_bNew;

private:
	~Player();
	void Initially();
	void Finally();
	virtual	void TeamInvitedCheck(Invited	*invited);
	virtual	void FrndInvitedCheck(Invited	*invited);
	virtual void MasterInvitedCheck(Invited *invited);
	virtual	void GuildInvitedCheck(Invited	*invited);
};

#endif	//PLAYER_H

