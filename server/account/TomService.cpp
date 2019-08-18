#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "AccountServer2.h"
#include "inifile.h"
#include "GlobalVariable.h"

CTomService::CTomService(void)
{
	//m_hTomMod = NULL;
#ifndef NOAUTHDLL
	pUserAttestFun = NULL;
	pGameOnlineFun = NULL;
#endif
	m_bEnableService = false;
	IniFile inf(g_strCfgFile.c_str());
	IniSection& is = inf["tom"];
	int enable = atoi(is["enable_tom"]);
	m_bEnableService = (enable > 0);
}

CTomService::~CTomService(void)
{

}

bool CTomService::InitService()
{
	if (!m_bEnableService) return true;
	printf("Initialize Tom service system... ");
	return true;
}
bool CTomService::pUserAttestFun(char* lpszUserName, char* lpszMD5PSW)
{
	// TODO: string authentification
	printf("lpszUserName: %s\n", lpszUserName);
	printf("lpszMD5PSW: %s\n", lpszMD5PSW);
	return true;
}


bool CTomService::VerifyLoginMember(const char* lpszUserName, const char* lpszMD5PSW)
{
	if (!m_bEnableService) return false;
	return pUserAttestFun((char*)lpszUserName, (char*)lpszMD5PSW);
}

void CTomService::ReportMemberCounts(int nCounts)
{
	if (!m_bEnableService) return;
	//printf("Report online user numbers : %d\r\n", nCounts);
	LG("MemberCount", "Report online user numbers : %d\r\n", nCounts);
	pGameOnlineFun(nCounts);
}

bool CTomService::IsEnable()
{
	return m_bEnableService;
}
//
//CTomService::eState CTomService::CheckMemberState(const char* lpszUserName)
//{
//	//m_mapUserState.find(lpszUserName);
//	return eState_LoginLocked;
//}


