#ifndef TEAMMEMBER_H
#define TEAMMEMBER_H
#include <RunBiDirectChain.h>
#include "Invitation.h"

_DBC_USING

class Team;

class TeamMember : public RunBiDirectItem<TeamMember>, public Invitation {
public:
	long JoinTeam(uLong cha_id);
	long JoinTeam(const char *szName);
	long LeaveTeam();
	Team* GetTeam();
	Player* GetLeader();

	PtInviter TeamBeginInvited(Player* inviter) { 
		return Invitation::BeginInvited(inviter); 
	}
	
	PtInviter TeamEndInvited(Player* inviter) { 
		return Invitation::EndInvited(inviter); 
	}

	PtInviter TeamEndInvited(uLong cha_id) {
		return Invitation::EndInvited(cha_id); 
	}

	Invited* TeamFindInvitedByInviterChaID(uLong cha_id) {
		return Invitation::FindInvitedByInviterChaID(cha_id); 
	}

protected:
	TeamMember(){};
	~TeamMember(){}
	void Initially();
	void Finally();

private:
	virtual	void TeamInvitedCheck(Invited *invited) = 0;
	long GetInvitedMax();
	void InvitedCheck(Invited *invited);
	long _BeginRun(Team *team);
	long _EndRun();
};
#endif //TEAMMEMBER_H
