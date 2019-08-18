#ifndef GUILD_H
#define GUILD_H

#include "PreAlloc.h"
#include "RunBiDirectChain.h"
#include "GuildMember.h"

_DBC_USING

class Player;

class Guild : public PreAllocStru, public RunBiDirectItem<Guild>, public RunBiDirectChain < GuildMember > {
public:
	Guild(uLong size);
	void Free(){ PreAllocStru::Free(); }
	static Guild *Alloc();
	bool BeginRun();
	bool EndRun();
	Player *FindGuildMemByChaID(uLong id);

	uLong m_id;
	char m_name[17];			// The guild name
	char m_motto[51];			// Guild Motto
	char m_type;
	BitMaskStatus m_stat;		// 0x0-normal; 0x1-insufficient membership; 0x2-insufficient number of members; 0x4- insufficient reputation of the president
	uLong m_remain_minute;		// The guild dissolved the remaining minutes
	uLong m_tick;
	uLong m_leaderID;

	int				m_level;
	int				m_point;
	int				m_exp;
	DWORD			m_color;

	Player *m_leader;			// President, does not allow change
private:
	~Guild();
	void Initially();
	void Finally();

};

class ChkGuild : public Timer {
public:
	ChkGuild(uLong interval) : Timer(interval) {
	}

private:
	void Process();
};

#endif //GUILD_H
