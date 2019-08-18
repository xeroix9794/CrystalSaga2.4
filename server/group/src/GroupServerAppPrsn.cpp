#include <iostream>
#include "GroupServerApp.h"
#include "GameCommon.h"

void GroupServerApp::CP_QUERY_PERSONINFO(Player *ply, DataSocket *datasock, RPacket &pk) {
	stQueryPersonInfo info;

	uShort l_len;
	strncpy_s(info.sChaName, sizeof(info.sChaName), pk.ReadString(&l_len), _TRUNCATE);
	info.bHavePic = (pk.ReadChar() == 1 ? true : false);
	strncpy_s(info.cSex, sizeof(info.cSex), pk.ReadString(&l_len), _TRUNCATE);
	info.nMinAge[0] = pk.ReadLong();
	info.nMinAge[1] = pk.ReadLong();
	strncpy_s(info.szAnimalZodiac, sizeof(info.szAnimalZodiac), pk.ReadString(&l_len), _TRUNCATE);
	info.iBirthday[0] = pk.ReadLong();
	info.iBirthday[1] = pk.ReadLong();
	strncpy_s(info.szState, sizeof(info.szState), pk.ReadString(&l_len), _TRUNCATE);
	strncpy_s(info.szCity, sizeof(info.szCity), pk.ReadString(&l_len), _TRUNCATE);
	strncpy_s(info.szConstellation, sizeof(info.szConstellation), pk.ReadString(&l_len), _TRUNCATE);
	strncpy_s(info.szCareer, sizeof(info.szCareer), pk.ReadString(&l_len), _TRUNCATE);
	info.nPageItemNum = pk.ReadLong();
	info.nCurPage = pk.ReadLong();

	int num;
	int totalpage;
	int totalrecord;

	MutexArmor l_lockDB(m_mtxDB);

	stQueryResoultPersonInfo result[10];
	m_tblpersoninfo->Query(&info, result, num, totalpage, totalrecord);
	l_lockDB.unlock();

	WPacket	l_wpk = GetWPacket();
	l_wpk.WriteCmd(CMD_PC_QUERY_PERSONINFO);
	l_wpk.WriteShort(num);
	l_wpk.WriteLong(totalpage);
	l_wpk.WriteLong(totalrecord);

	for (int i = 0; i < num; i++) {
		l_wpk.WriteString(result[i].sChaName);
		l_wpk.WriteShort(result[i].nMinAge);
		l_wpk.WriteString(result[i].cSex);
		l_wpk.WriteString(result[i].szState);
		l_wpk.WriteString(result[i].nCity);
	}
	SendToClient(ply, l_wpk);
}

void GroupServerApp::CP_CHANGE_PERSONINFO(Player *ply, DataSocket *datasock, RPacket &pk) {
	stPersonInfo info;

	uShort l_len;
	strncpy_s(info.szMotto, sizeof(info.szMotto), pk.ReadString(&l_len), _TRUNCATE);
	info.bShowMotto = (pk.ReadChar() == 1 ? true : false);
	strncpy_s(info.szSex, sizeof(info.szSex), pk.ReadString(&l_len), _TRUNCATE);
	info.sAge = (short)pk.ReadLong();
	strncpy_s(info.szName, sizeof(info.szName), pk.ReadString(&l_len), _TRUNCATE);
	strncpy_s(info.szAnimalZodiac, sizeof(info.szAnimalZodiac), pk.ReadString(&l_len), _TRUNCATE);
	strncpy_s(info.szBloodType, sizeof(info.szBloodType), pk.ReadString(&l_len), _TRUNCATE);
	info.iBirthday = pk.ReadLong();
	strncpy_s(info.szState, sizeof(info.szState), pk.ReadString(&l_len), _TRUNCATE);
	strncpy_s(info.szCity, sizeof(info.szCity), pk.ReadString(&l_len), _TRUNCATE);
	strncpy_s(info.szConstellation, sizeof(info.szConstellation), pk.ReadString(&l_len), _TRUNCATE);
	strncpy_s(info.szCareer, sizeof(info.szCareer), pk.ReadString(&l_len), _TRUNCATE);
	info.iSize = pk.ReadLong();

	if (info.iSize > 0 && info.iSize < 8 * 1024) {
		memcpy(info.pAvatar, pk.ReadSequence(l_len), info.iSize);
	}

	// MRF_WHY: Why was this updated? C++ should provide both versions.
	// info.bPprevent = pk.ReadChar();
	info.bPprevent = (pk.ReadChar() == 1 ? true : false);
	info.iSupport = pk.ReadLong();
	info.iOppose = pk.ReadLong();

	MutexArmor l_lockDB(m_mtxDB);
	ply->m_refuse_sess = info.bPprevent ? true : false;
	m_tblpersoninfo->DelInfo(ply->m_chaid[ply->m_currcha]);
	m_tblpersonavatar->DelInfo(ply->m_chaid[ply->m_currcha]);
	m_tblpersoninfo->AddInfo(ply->m_chaid[ply->m_currcha], &info);
	m_tblpersonavatar->AddInfo(ply->m_chaid[ply->m_currcha], &info);

	cChar *l_motto = pk.ReadString(&l_len);
	m_tblcharaters->UpdateInfo(ply->m_chaid[ply->m_currcha], 1, l_motto);

	l_lockDB.unlock();

	WPacket	l_wpk = GetWPacket();
	l_wpk.WriteCmd(CMD_PC_CHANGE_PERSONINFO);
	l_wpk.WriteString(l_motto);
	l_wpk.WriteShort(1);
	l_wpk.WriteChar(ply->m_refuse_sess ? 1 : 0);
	SendToClient(ply, l_wpk);
}

void GroupServerApp::CP_FRND_REFRESH_INFO(Player *ply, DataSocket *datasock, RPacket &pk) {
	uLong l_chaid = pk.ReadLong();
	MutexArmor l_lockDB(m_mtxDB);

	if (ply->m_chaid[ply->m_currcha] != l_chaid) {
		if (m_tblfriends->GetFriendsCount(ply->m_chaid[ply->m_currcha], l_chaid) != 2) {
			l_lockDB.unlock();
			ply->SendSysInfo(RES_STRING(GP_GROUPSERVERAPPPRSN_CPP_00003));
			return;
		}
	}

	stPersonInfo st;
	if (m_tblcharaters->FetchRowByChaID(l_chaid) == 1) {
		WPacket	l_wpk = GetWPacket();
		l_wpk.WriteCmd(CMD_PC_FRND_REFRESH_INFO);

		string act_name = "";
		if (m_tblcharaters->FetchActNameRowByChaID(l_chaid, act_name) != 1) {
			act_name = "";
		}

		l_wpk.WriteString(act_name.c_str());
		l_wpk.WriteLong(l_chaid);
		l_wpk.WriteString(m_tblcharaters->GetMotto());
		l_wpk.WriteShort(m_tblcharaters->GetIcon());
		l_wpk.WriteShort(m_tblcharaters->GetDegree());
		l_wpk.WriteString(m_tblcharaters->GetJob());
		l_wpk.WriteString(m_tblcharaters->GetGuildName());

		if (m_tblpersoninfo->GetInfo(l_chaid, &st) && m_tblpersonavatar->GetInfo(l_chaid, &st)) {
			l_wpk.WriteChar(true);
			l_wpk.WriteString(st.szMotto);				// Motto
			l_wpk.WriteChar(st.bShowMotto);				// Show Motto
			l_wpk.WriteString(st.szSex);				// Sex
			l_wpk.WriteLong(st.sAge);					// Age
			l_wpk.WriteString(st.szName);				// Name
			l_wpk.WriteString(st.szAnimalZodiac);		// Zodiac
			l_wpk.WriteString(st.szBloodType);			// Blood Type
			l_wpk.WriteLong(st.iBirthday);				// Birthday
			l_wpk.WriteString(st.szState);				// State
			l_wpk.WriteString(st.szCity);				// City
			l_wpk.WriteString(st.szConstellation);		// Constellation
			l_wpk.WriteString(st.szCareer);				// Career
			l_wpk.WriteLong(st.iSize);					// Size
			l_wpk.WriteSequence(st.pAvatar, st.iSize);	// Avatar
			l_wpk.WriteChar(st.bPprevent);				// Block Messagin?
			l_wpk.WriteLong(st.iSupport);				// Supporters count
			l_wpk.WriteLong(st.iOppose);				// Haters count
		}
		else
			l_wpk.WriteChar(false);

		l_lockDB.unlock();
		SendToClient(ply, l_wpk);
	}
	else {
		ply->SendSysInfo("Failed!");
	}
}

void GroupServerApp::CP_REFUSETOME(Player *ply, DataSocket *datasock, RPacket &pk) {
	ply->m_refuse_tome = pk.ReadChar() ? true : false;
}
