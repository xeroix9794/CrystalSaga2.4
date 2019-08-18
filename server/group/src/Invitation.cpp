#include "Invitation.h"
#include "GroupServerApp.h"

static PreAllocHeapPtr<Invited> gheap_Invited(1, 1000);
void Invitation::Initially() {
	RunBiDirectChain<Invited>::Initially();
	RunCtrlObj<Invitation>::Initially();
}

void Invitation::Finally() {
	RunCtrlObj<Invitation>::Finally();
	RunBiDirectChain<Invited>::Finally();
}

PtInviter Invitation::BeginInvited(Player *inviter) {
	if (inviter->m_currcha < 0)
		return 0;

	PtInviter l_ptinviter = 0;
	RunChainGetArmor<Invited> l(*this);
	EndInvited(inviter);
	if (GetInvitedCount() == GetInvitedMax()) {
		l_ptinviter = GetFirstItem()->m_ptinviter;
		EndInvited(l_ptinviter.m_ply);
	}

	Invited *l_invited = gheap_Invited.Get();
	l_invited->m_ptinviter.m_chaid = inviter->m_chaid[inviter->m_currcha];
	l_invited->m_ptinviter.m_ply = inviter;
	l_invited->m_tick = g_gpsvr->GetCurrentTick();
	if (l_invited->_BeginRun(this) == 1) {
		g_gpsvr->GetProcessor()->AddTask(RunCtrlObj<Invitation>::_BeginRun(&(g_gpsvr->m_plyproc)));
	}

	l.unlock();
	return l_ptinviter;
}

PtInviter Invitation::EndInvited(Player *inviter) {
	PtInviter l_ptinviter = 0;
	RunChainGetArmor<Invited> l(*this);
	Invited	*l_invited;
	while ((l_invited = GetNextItem()) && l_invited->m_ptinviter.m_ply != inviter){}
	if (l_invited) {
		l_ptinviter = l_invited->m_ptinviter;
		if (l_invited->_EndRun() == 1) {
			RunCtrlObj<Invitation>::_EndRun();
		}
		l_invited->Free();
	}
	l.unlock();
	return l_ptinviter;
}

PtInviter Invitation::EndInvited(uLong cha_id) {
	PtInviter l_ptinviter = 0;
	RunChainGetArmor<Invited> l(*this);
	Invited	*l_invited;
	while ((l_invited = GetNextItem()) && l_invited->m_ptinviter.m_chaid != cha_id){}
	if (l_invited) {
		l_ptinviter = l_invited->m_ptinviter;
		if (l_invited->_EndRun() == 1) {
			RunCtrlObj<Invitation>::_EndRun();
		}
		l_invited->Free();
	}
	l.unlock();
	return l_ptinviter;
}

Invited *Invitation::FindInvitedByInviterChaID(uLong cha_id) {
	Invited	*l_invited = 0;
	RunChainGetArmor<Invited> l(*this);
	while ((l_invited = GetNextItem()) && l_invited->m_ptinviter.m_chaid != cha_id){}
	l.unlock();
	return l_invited;
}

void Invitation::_Run() {
	RunChainGetArmor<Invited> l(*this);
	for (Invited *l_invited = GetNextItem(); l_invited; l_invited = GetNextItem()) {
		InvitedCheck(l_invited);
	}
	l.unlock();
}
