#include "Master.h"
#include "GroupServerApp.h"

long Master::GetInvitedMax() {
	return g_gpsvr->const_master.InvitedMax;
}

void Master::InvitedCheck(Invited	*invited) {
	MasterInvitedCheck(invited);
}

const cChar* Master::GetMasterGroup() {
	return RES_STRING(GP_GROUPSERVERAPPMASTER_CPP_00018);
}

const cChar* Master::GetPrenriceGroup() {
	return RES_STRING(GP_GROUPSERVERAPPMASTER_CPP_00017);;
}

