#ifndef MASTER_H
#define MASTER_H

#include "Invitation.h"
#include "i18n.h"

// extern const cChar* gc_master_group;
// extern const cChar* gc_prentice_group;

class Master : public Invitation {
public:
	PtInviter MasterBeginInvited(Player *inviter) {
		return Invitation::BeginInvited(inviter);
	}

	PtInviter MasterEndInvited(Player *inviter) {
		return Invitation::EndInvited(inviter);
	}

	PtInviter MasterEndInvited(uLong cha_id) {
		return Invitation::EndInvited(cha_id);
	}

	Invited *MasterFindInvitedByInviterChaID(uLong cha_id) {
		return Invitation::FindInvitedByInviterChaID(cha_id);
	}

protected:
	Master() {
	}

	~Master() {
	}

private:
	virtual	void MasterInvitedCheck(Invited	*invited) = 0;
	long GetInvitedMax();
	void InvitedCheck(Invited *invited);

public:
	InterLockedLong m_CurrMasterNum;
	InterLockedLong m_CurrPrenticeNum;

	static const cChar* GetMasterGroup();
	static const cChar* GetPrenriceGroup();
};

#endif //MASTER_H
