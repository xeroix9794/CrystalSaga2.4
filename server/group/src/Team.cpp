#include "GroupServerApp.h"
#include "Team.h"

_DBC_USING

Team::Team(uLong size) : PreAllocStru(size) {
}

Team::~Team() {
}

void Team::Initially() {
	RunBiDirectItem<Team>::Initially();
	RunBiDirectChain<TeamMember>::Initially();
}

void Team::Finally() {
	RunBiDirectChain<TeamMember>::Finally();
	RunBiDirectItem<Team>::Finally();
}

long Team::BeginRun() {
	return _BeginRun(&(g_gpsvr->m_teamlst));
}

long Team::EndRun() {
	return _EndRun();
}

Player *Team::GetLeader() {
	return static_cast<Player *>(RunBiDirectChain<TeamMember>::GetFirstItem());
}

Player *Team::GetMember(DWORD dwID) {
	RunChainGetArmor<TeamMember> l(*this);
	Player* ply;
	for (int i = 0; i < this->GetTotal() && (ply = static_cast<Player*>(GetNextItem())); i++) {
		if (ply->m_currcha != -1 && ply->m_chaid[ply->m_currcha] == dwID) {
			return ply;
		}
	}
	return NULL;
}
