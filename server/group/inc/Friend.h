#ifndef FRIEND_H
#define FRIEND_H
#include "Invitation.h"

class Friend : public Invitation {
public:
	PtInviter FrndBeginInvited(Player *inviter) {
		return Invitation::BeginInvited(inviter);
	}

	PtInviter FrndEndInvited(Player *inviter) {
		return Invitation::EndInvited(inviter);
	}

	PtInviter FrndEndInvited(uLong cha_id) {
		return Invitation::EndInvited(cha_id);
	}

	Invited	*FrndFindInvitedByInviterChaID(uLong cha_id) {
		return Invitation::FindInvitedByInviterChaID(cha_id);
	}

protected:
	Friend() {
	}

	~Friend() {
	}

private:
	virtual	void FrndInvitedCheck(Invited *invited) = 0;
	long GetInvitedMax();
	void InvitedCheck(Invited *invited);

public:
	InterLockedLong m_CurrFriendNum;
	static const cChar* GetStandardGroup();
};
#endif // FRIEND_H
