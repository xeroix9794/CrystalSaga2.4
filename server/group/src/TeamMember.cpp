#include "TeamMember.h"
#include "Team.h"
#include "GroupServerApp.h"

void TeamMember::Initially() {
	RunBiDirectItem<TeamMember>::Initially();
	Invitation::Initially();
}

void TeamMember::Finally() {
	Invitation::Finally();
	RunBiDirectItem<TeamMember>::Finally();
}

long TeamMember::GetInvitedMax() {
	return g_gpsvr->const_team.InvitedMax;
}

long TeamMember::JoinTeam(const char *szName) {
	long l_lret = 0;
	if (GetTeam()) 
		return l_lret;
	
	Player *ply;
	ply = g_gpsvr->FindPlayerByChaName(szName);
	if (!ply) 
		return l_lret;
	
	PtInviter l_ptinviter;
	l_ptinviter = TeamEndInvited(ply);
	if (l_ptinviter && l_ptinviter->m_currcha >= 0 && l_ptinviter.m_chaid == l_ptinviter->m_chaid[l_ptinviter->m_currcha]) {
		Team *l_team = 0;
		while (!l_team) {
			l_team = l_ptinviter->GetTeam();
			if (l_team) {
				if (l_team->GetLeader() != l_ptinviter.m_ply) {
					l_team = 0;
					break;
				}
				l_team->m_mtxTeam.lock();
			}
			else {
				l_team = g_gpsvr->m_teamheap.Get();
				l_team->m_mtxTeam.lock();
				if (!static_cast<TeamMember*>(l_ptinviter.m_ply)->_BeginRun(l_team)) {
					l_team->m_mtxTeam.unlock();
					l_team->Free();
					l_team = 0;
				}
			}
		}

		if (l_team && (l_lret = _BeginRun(l_team)) == 1) {
			_EndRun();
			l_lret = 0;
			l_team->m_mtxTeam.unlock();
			l_team->Free();
			l_team = 0;
		}
		else if (l_team) {
			l_team->m_mtxTeam.unlock();
		}
	}

	return l_lret;
}

long TeamMember::JoinTeam(uLong cha_id) {
	long l_lret = 0;
	if (GetTeam())
		return l_lret;
	
	PtInviter l_ptinviter = TeamEndInvited(cha_id);
	if (l_ptinviter && l_ptinviter->m_currcha >= 0 && l_ptinviter.m_chaid == l_ptinviter->m_chaid[l_ptinviter->m_currcha]) {
		Team *l_team = 0;
		while (!l_team) {
			l_team = l_ptinviter->GetTeam();
			if (l_team) {
				if (l_team->GetLeader() != l_ptinviter.m_ply) {
					l_team = 0;
					break;
				}
				l_team->m_mtxTeam.lock();
			}
			else {
				l_team = g_gpsvr->m_teamheap.Get();
				l_team->m_mtxTeam.lock();
				if (!static_cast<TeamMember*>(l_ptinviter.m_ply)->_BeginRun(l_team)) {
					l_team->m_mtxTeam.unlock();
					l_team->Free();
					l_team = 0;
				}
			}
		}

		if (l_team && (l_lret = _BeginRun(l_team)) == 1) {
			_EndRun();
			l_lret = 0;
			l_team->m_mtxTeam.unlock();
			l_team->Free();
			l_team = 0;
		}
		else if (l_team) {
			l_team->m_mtxTeam.unlock();
		}
	}
	
	return l_lret;
}

long TeamMember::LeaveTeam() {
	long l_lret = 0;
	Team *l_team = GetTeam();
	if (l_team) {
		MutexArmor l_mtxTeam(l_team->m_mtxTeam);
		if ((l_lret = _EndRun()) == 1) {
			if (l_team->EndRun()) {
				l_team->Free();
			}
		}
	}
	
	return l_lret;
}

Team *TeamMember::GetTeam() {
	return static_cast<Team*>(RunBiDirectItem<TeamMember>::GetChain());
}

Player *TeamMember::GetLeader() {
	Team *l_team = GetTeam();
	if (l_team)
		return l_team->GetLeader();
	else
		return 0;
}

long TeamMember::_BeginRun(Team *team) {
	return RunBiDirectItem<TeamMember>::_BeginRun(team);
}

long TeamMember::_EndRun() {
	return RunBiDirectItem<TeamMember>::_EndRun();
}

void TeamMember::InvitedCheck(Invited *invited) {
	TeamInvitedCheck(invited);
}
