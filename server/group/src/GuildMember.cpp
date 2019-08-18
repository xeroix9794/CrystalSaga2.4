#include "GuildMember.h"
#include "Guild.h"
#include "GroupServerApp.h"

bool GuildMember::JoinGuild(Guild *gld) {
	return _BeginRun(gld) ? true : false;
}

bool GuildMember::LeaveGuild() {
	return _EndRun() ? true : false;
}

Guild * GuildMember::GetGuild() {
	return static_cast<Guild *>(RunBiDirectItem<GuildMember>::GetChain());
}

long GuildMember::GetInvitedMax() {
	return g_gpsvr->const_guild.InvitedMax;
}

void GuildMember::InvitedCheck(Invited	*invited) {
	GuildInvitedCheck(invited);
}

long GuildMember::_BeginRun(Guild * guild) {
	return RunBiDirectItem<GuildMember>::_BeginRun(guild);
}

long GuildMember::_EndRun() {
	return RunBiDirectItem<GuildMember>::_EndRun();
}

void GuildMember::Initially() {
	RunBiDirectItem<GuildMember>::Initially();
	Invitation::Initially();
}

void GuildMember::Finally() {
	Invitation::Finally();
	RunBiDirectItem<GuildMember>::Finally();
}
