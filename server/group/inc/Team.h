#ifndef TEAM_H
#define TEAM_H

#include "PreAlloc.h"
#include "RunBiDirectChain.h"
#include "TeamMember.h"

_DBC_USING

class Player;

class Team : public PreAllocStru, public RunBiDirectItem<::Team>, public RunBiDirectChain<TeamMember> {
	friend class TeamMember;

public:
	Team(uLong size);
	
	void Free() { 
		PreAllocStru::Free(); 
	}

	long BeginRun();
	long EndRun();
	Player* GetLeader();
	Player* GetMember(DWORD dwID);

private:
	~Team();
	void Initially();
	void Finally();
	Mutex m_mtxTeam;
};
#endif //TEAM_H
