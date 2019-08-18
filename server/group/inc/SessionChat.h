#ifndef SESSIONCHAT_H
#define SESSIONCHAT_H

#include "PreAlloc.h"
#include "RunBiDirectChain.h"

_DBC_USING

class Player;
class Chat_Session;

class Chat_Player : public  RunBiDirectItem<Chat_Player>, public PreAllocStru {
	friend class Chat_Session;

public:
	Chat_Player(uLong size) :PreAllocStru(size){}
	void Free(){ PreAllocStru::Free(); }
	void Initially();
	void Finally();

	Chat_Session *GetSession();

private:
	Player	*m_ply;
};

class Chat_Session : public RunBiDirectChain<Chat_Player>, public RunBiDirectItem<Chat_Session>, public PreAllocStru {
	friend class GroupServerApp;

public:
	Chat_Session(uLong size) :PreAllocStru(size){}
	void Free(){ PreAllocStru::Free(); }
	void Initially();
	void Finally();

	uLong GetID(){ return m_sessid; }
	long AddPlayer(Player *ply);
	long DelPlayer(Player *ply);
	Player *GetNextPlayer() { Chat_Player *l_chatply = RunBiDirectChain<Chat_Player>::GetNextItem(); return l_chatply ? l_chatply->m_ply : 0; }
	Player *GetFirstPlayer(){ Chat_Player *l_chatply = RunBiDirectChain<Chat_Player>::GetFirstItem(); return l_chatply ? l_chatply->m_ply : 0; }

private:

	uLong m_sessid;
};

#endif