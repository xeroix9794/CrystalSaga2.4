/* Generated by Together */

#include "Friend.h"
#include "GroupServerApp.h"

long Friend::GetInvitedMax() {
	return g_gpsvr->const_frnd.InvitedMax;
}

void Friend::InvitedCheck(Invited *invited) {
	FrndInvitedCheck(invited);
}

const cChar* Friend::GetStandardGroup() {
	return (const cChar*)RES_STRING(GP_FRIEND_H_00001);
}