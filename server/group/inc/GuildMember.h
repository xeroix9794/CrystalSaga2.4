#ifndef GUILDMEMBER_H
#define GUILDMEMBER_H

#include "RunBiDirectChain.h"
#include "Invitation.h"

_DBC_USING

class Guild;

class GuildMember : public RunBiDirectItem<GuildMember>, public Invitation {
public:
	bool JoinGuild(Guild *gld);
	bool LeaveGuild();
	Guild *GetGuild();

	PtInviter GuildBeginInvited(Player * inviter) {
		return Invitation::BeginInvited(inviter);
	}

	PtInviter GuildEndInvited(Player * inviter){
		return Invitation::EndInvited(inviter);
	}

	PtInviter GuildEndInvited(uLong    cha_id) {
		return Invitation::EndInvited(cha_id);
	}

	Invited	*GuildFindInvitedByInviterChaID(uLong cha_id) {
		return Invitation::FindInvitedByInviterChaID(cha_id);
	}

protected:
	GuildMember(){};
	~GuildMember(){};
	void Initially();
	void Finally();

private:
	virtual	void GuildInvitedCheck(Invited	*invited) = 0;
	long GetInvitedMax();
	void InvitedCheck(Invited	*invited);
	long _BeginRun(Guild * guild);
	long _EndRun();

public:
	InterLockedLong m_CurrGuildNum;
};
#endif //GUILDMEMBER_H
