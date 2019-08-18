#ifndef INVITATION1_H
#define INVITATION1_H

#include "PreAlloc.h"
#include "RunCtrlThrd.h"

_DBC_USING

class Player;

struct PtInviter {
	PtInviter(Player *ply = 0, uLong chaid = 0) :m_ply(ply), m_chaid(chaid) {
	}

	operator bool() {
		return m_ply != 0;
	}

	Player *operator->() {
		return m_ply;
	}

	Player *volatile m_ply;
	uLong volatile m_chaid;
};

class Invited : public PreAllocStru, public RunBiDirectItem < ::Invited > {
public:
	friend class Invitation;
	
	Invited(uLong size) :PreAllocStru(size), m_ptinviter(0), m_tick(0) {
	}

	virtual	void Free() {
		PreAllocStru::Free();
	}

private:
	virtual void Initially() {
		RunBiDirectItem<::Invited>::Initially(); m_ptinviter.m_ply = 0; m_ptinviter.m_chaid = 0; m_tick = 0;
	}

	virtual void Finally() {
		RunBiDirectItem<::Invited>::Finally();	 m_ptinviter.m_ply = 0; m_ptinviter.m_chaid = 0; m_tick = 0;
	}

public:
	PtInviter m_ptinviter;
	uLong volatile m_tick;
};

class Invitation : public RunCtrlObj<Invitation>, public RunBiDirectChain < Invited > {
public:
	enum {
		CtrlThrdMax = 1,
		RunObjMax = 5000,
		TimePrecision = 1000
	};

protected:
	Invitation(){}
	~Invitation(){}
	void Initially();
	void Finally();

	PtInviter BeginInvited(Player * inviter);
	PtInviter EndInvited(Player * inviter);
	PtInviter EndInvited(uLong	  cha_id);
	Invited	*FindInvitedByInviterChaID(uLong cha_id);

	long GetInvitedCount(){ return RunBiDirectChain<Invited>::GetTotal(); }

private:
	virtual	void InvitedCheck(Invited	*invited) = 0;
	virtual long GetInvitedMax() = 0;
	void _Run();
};
#endif //INVITATION1_H
