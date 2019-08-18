#include "lua_gamectrl.h"
#include "Birthplace.h"


std::list<CCharacter*> g_HelpNPCList;
mission::CNpc* pTradeNPC = 0;

// ��ӳ������������
int lua_AddBirthPoint(lua_State *L)
{T_B
	// �����Ϸ����б�
    BOOL bValid = (lua_gettop(L)==4 && lua_isstring(L, 1) && lua_isstring(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4));
	if(!bValid) 
    {
		PARAM_ERROR;
		return 0;
	}

	const char *pszLocation = lua_tostring(L, 1);
	const char *pszMapName  = lua_tostring(L, 2);
	int x = (int)lua_tonumber(L, 3);
	int y = (int)lua_tonumber(L, 4);

	g_BirthMgr.AddBirthPoint(pszLocation, pszMapName, x, y);
	//LG("birth", "��ӳ�����[%s] [%s] %d %d\n", pszLocation, pszMapName, x, y);
	return 0;
T_E}

// ������г������������
int lua_ClearAllBirthPoint(lua_State *L)
{T_B
	g_BirthMgr.ClearAll();
	//LG("birth", "��������г�����\n");
	return 0;
T_E}

extern const char* GetResPath(const char *pszRes);
void ReloadAISdk()
{
	lua_dofile( g_pLuaState, GetResPath("script/ai/ai.lua"));
}

//char g_TradeName[][32] = 
//{
//	"����",
//	"��",	
//	"��",
//	"����",
//	"��",
//	"�����",
//	"������",
//	"���",
//	"����",
//	"����Ϸ",
//	"����Ϸ",
//	"��̯",
//	"����",
//	"ɾ��",
//	"����",
//	"װ��"
//};


//const char* g_TradeName[] = 
const char* GetTradeName(int index) 
{
	switch(index)
	{
	case 0:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00001);
	case 1:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00002);
	case 2:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00003);
	case 3:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00004);
	case 4:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00005);
	case 5:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00006);
	case 6:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00007);
	case 7:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00008);
	case 8:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00009);
	case 9:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00010);
	case 10:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00011);
	case 11:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00012);
	case 12:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00013);
	case 13:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00014);
	case 14:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00015);
	case 15:
		return RES_STRING(GM_LUA_GAMECTRL_CPP_00016);
	default:
		return "";
	}
};

#define TL_TIME_ONE_HOUR			6*60*60*1000
void TL(int nType, const char *pszCha1, const char *pszCha2, const char *pszTrade)
{
	if(!g_Config.m_bLogDB)
	{
		static short sInit = 1;
		static std::string strName;
		static DWORD dwLastTime = -1;
		static DWORD dwCount = 10000;
		if( dwCount++ > 1000 )
		{
			DWORD dwTime = GetTickCount();
			if( dwTime - dwLastTime >= TL_TIME_ONE_HOUR || sInit == 1 )
			{
				dwCount = 0;
				strName = "trade";
				dwLastTime = dwTime;
				SYSTEMTIME st;
				GetLocalTime( &st );
				char szData[128];
				//sprintf( szData, "%d-%d-%d-%d", st.wYear, st.wMonth, st.wDay, st.wHour );
				_snprintf_s( szData,sizeof(szData),_TRUNCATE, "%d-%d-%d-%d", st.wYear, st.wMonth, st.wDay, st.wHour );
				strName += szData;
			}
		}

		//LG(strName.c_str(), "%7s [%17s] [%17s] [%s]\n", g_TradeName[nType], pszCha1, pszCha2, pszTrade); 
		LG(strName.c_str(), "%7s [%17s] [%17s] [%s]\n", GetTradeName(nType), pszCha1, pszCha2, pszTrade); 
		//g_pGameApp->Log(g_TradeName[nType], pszCha1, "", pszCha2, "", pszTrade); 
		g_pGameApp->Log(GetTradeName(nType), pszCha1, "", pszCha2, "", pszTrade); 
		sInit = 0;
	}
	else
	{
		// Add by lark.li 20080324 begin
		//static CThrdLock lock;
		//lock.Lock();
		//g_pGameApp->TradeLog(g_TradeName[nType], pszCha1, pszCha2,pszTrade);
		//lock.Unlock();
		// End
	}
}


CCharacter *g_pTestCha = NULL;
CCharacter g_cc;

int lua_TestTest(lua_State *L)
{
	g_pTestCha = &g_cc;
	//g_pTestCha->SetName("����");
	g_pTestCha->SetName(RES_STRING(GM_LUA_GAMECTRL_CPP_00017));
	lua_pushlightuserdata(L, g_pTestCha);
	return 1;
}

int lua_TestTest1(lua_State *L)
{
	//g_pTestCha->SetName("new����");
	g_pTestCha->SetName(RES_STRING(GM_LUA_GAMECTRL_CPP_00018));
	g_pTestCha = NULL;
	return 0;
}

map<string, string> g_HelpList;

// ��Ӱ�����Ϣ, ����2������: �ؼ��� ��������
int lua_AddHelpInfo(lua_State *L)
{
	BOOL bValid = (lua_gettop(L)==2  && lua_isstring(L, 1) && lua_isstring(L, 2));
	if(!bValid)
	{
		return 0;
	}
	
	const char *pszKey  = (const char*)lua_tostring(L, 1);
	const char *pszText = (const char*)lua_tostring(L, 2);

	g_HelpList[pszKey] = pszText;
	
	return 0;
}

const char* FindHelpInfo(const char *pszKey)
{
	map<string, string>::iterator it = g_HelpList.find(pszKey);
	if(it==g_HelpList.end())
	{
		return NULL;
	}
	return (*it).second.c_str();
}

void AddHelpInfo(const char *pszKey, const char *pszInfo)
{
	// Add by lark.li 20090311
	if(!pszKey || !pszInfo)
		return;
	// End

	if(strlen(pszKey)==0)  return;
	if(strlen(pszInfo)==0) return;

	g_HelpList[pszKey] = pszInfo;

	//LG("help", "Ŀǰ������Ŀ�� = %d\n", g_HelpList.size());
	LG("help", "now helplist amount = %d\n", g_HelpList.size());
}

void AddMonsterHelp(int nScriptID, int x, int y)
{
	CChaRecord	*pCChaRecord = GetChaRecordInfo(nScriptID);
	if (pCChaRecord == NULL) return;	

	//char szHelp[255]; sprintf(szHelp, "��˵�ڱ������%d, %d������������������û!", x/100, y/100);
	char szHelp[255]; //sprintf(szHelp, RES_STRING(GM_LUA_GAMECTRL_CPP_00019), x/100, y/100);
	//_snprintf_s(szHelp,sizeof(szHelp),_TRUNCATE, RES_STRING(GM_LUA_GAMECTRL_CPP_00019), x/100, y/100);
	CFormatParameter param(2);
	param.setDouble( 0, x/100 );
	param.setDouble( 1, y/100 );

	RES_FORMAT_STRING( GM_LUA_GAMECTRL_CPP_00019, param, szHelp );
	AddHelpInfo(pCChaRecord->szDataName, szHelp);
}

void AddHelpNPC(CCharacter *pNPC)
{
	//LG("init", "�ɹ���Ӱ���NPC[%s]\n", pNPC->GetName());
	LG("init", "Succeed add HelpNPC[%s]\n", pNPC->GetName());
	g_HelpNPCList.push_back(pNPC);
}


// ͨ���ű���Ӱ���NPC
int lua_AddHelpNPC(lua_State *L)
{
	BOOL bValid = (lua_gettop(L)==1 && lua_isstring(L, 1));
	if(!bValid)
	{
		return 0;
	}
	
	const char *pszName  = (const char*)lua_tostring(L, 1); // ��ð���NPC������
	
	// �����ֲ���NPC����
	g_pGameApp->BeginGetTNpc();
	mission::CTalkNpc*	pCTNpc;
	while (pCTNpc = g_pGameApp->GetNextTNpc())
	{
		if (!strcmp(pCTNpc->GetName(), pszName))
			AddHelpNPC(pCTNpc);
	}
	
	return 0;
}

int lua_ClearHelpNPC(lua_State *L)
{
	g_HelpNPCList.clear();
	return 0;
}
//Add by sunny.sun 20090326
int lua_AddTradeNPC(lua_State *L)
{
	BOOL bValid = (lua_gettop(L)==1 && lua_isstring(L, 1));
	if(!bValid)
	{
		return 0;
	}
	
	const char *pszName  = (const char*)lua_tostring(L, 1); // ��ð���NPC������
	
	// �����ֲ���NPC����
	g_pGameApp->BeginGetTNpc();
	mission::CNpc*	pNpc;
	pNpc = g_pGameApp->FindNpc( pszName );
	if( pNpc )
	{
		pTradeNPC = pNpc;
	}
	return 0;
}

// ����DBLog
int lua_TestDBLog(lua_State *L)
{
	// �����Ϸ����б�
    BOOL bValid = (lua_gettop (L)==1 && lua_isnumber(L, 1));
	if(!bValid) 
    {
        PARAM_ERROR
        return 0;
    }

	int nCnt = (int)lua_tonumber(L, 1);

	LETimer t; t.Begin();
	for(int i = 0; i < nCnt; i++)
	{
		g_pGameApp->Log("newtest", "abcdefg", "1234567", "000000", "qqqppp", "abcdefghijklmnopqrstuvwxyz");
	}
	LG("dblog", "Add Time = %d\n", t.End());
	
	return 0;
}



void RegisterLuaAI(lua_State *L)
{T_B
	
	// ͨ��
	REGFN(view);
	REGFN(EXLG);
	REGFN(PRINT);
	REGFN(GetResPath);

	// For AI
	
	REGFN(SetCurMap);
	REGFN(GetChaID);
	REGFN(CreateCha);
	REGFN(CreateChaX);
	REGFN(CreateChaEx);
	REGFN(QueryChaAttr); 
	REGFN(GetChaAIType);
	REGFN(SetChaAIType);
	REGFN(GetChaTypeID);
	REGFN(GetChaVision);
	REGFN(GetChaTarget);
	REGFN(SetChaTarget);
	REGFN(GetChaHost);
	REGFN(SetChaHost);
	REGFN(GetPetNum);
	REGFN(GetChaFirstTarget);
	REGFN(GetChaPos);
	REGFN(GetChaBlockCnt);
	REGFN(SetChaBlockCnt);
	REGFN(ChaMove);
	REGFN(ChaMoveToSleep);
	REGFN(GetChaSpawnPos);
	REGFN(SetChaPatrolState);
	REGFN(GetChaPatrolState);
	REGFN(GetChaPatrolPos);
	REGFN(SetChaPatrolPos);
	REGFN(SetChaFaceAngle);
	REGFN(GetChaChaseRange);
	REGFN(SetChaChaseRange);
	REGFN(ChaUseSkill);
	REGFN(ChaUseSkill2);
	REGFN(GetChaByRange);
	REGFN(GetChaSetByRange);
	REGFN(ClearHideChaByRange);
	REGFN(IsChaFighting);
	REGFN(IsPosValid);
	REGFN(IsChaSleeping);
	REGFN(ChaActEyeshot);
	REGFN(GetChaFacePos);
	REGFN(SetChaEmotion);
	REGFN(FindItem);
	REGFN(PickItem);
	REGFN(GetItemPos);
	REGFN(EnableAI);
	REGFN(GetChaSkillNum);
	REGFN(GetChaSkillInfo);
	REGFN(GetChaHarmByNo);
	REGFN(GetFirstAtker);
	REGFN(AddHate);
	REGFN(GetChaHateByNo);
	REGFN(HarmLog);
	REGFN(SummonCha);
	REGFN(DelCha);
	REGFN(SetChaLifeTime);
	
	// ��ֵ����
	REGFN(SetChaAttrMax);
	REGFN(GetChaDefaultName);
	REGFN(SetChaAttrI);
	REGFN(GetChaAttrI);
	REGFN(IsPlayer);
	REGFN(IsChaInRegion);
	
	// ���
	REGFN(IsChaInTeam);
	REGFN(GetTeamCha);

	// �������������
	REGFN(AddBirthPoint);
	REGFN(ClearAllBirthPoint);

	// ��������
	REGFN(AddWeatherRegion);
	REGFN(ClearMapWeather);

	REGFN(TestTest);
	REGFN(TestTest1);
	
	// ����NPC
	REGFN(AddHelpInfo);
	REGFN(AddHelpNPC);
	REGFN(ClearHelpNPC);
	REGFN(AddTradeNPC);

	// ��ֻ��ʱ
	REGFN(SetBoatCtrlTick);
	REGFN(GetBoatCtrlTick);

    REGFN(GetRoleID);
	REGFN(GetRolebyID);
	REGFN(GetRolebyName);
	REGFN(UnlockItem);

	// ���Խű�
	REGFN(TestDBLog);
	
T_E}


/*
				��Ƭ�ٻ�����ʵ������
  
һ:	ʹ�ÿ�Ƭ����, ִ��summon����

��: Ϊsummon�����Ĺ������ö�ʱ��ʧ�ļ���״̬, ״̬����ʱ����ǹ��������ʱ��

��: summon�����Ĺ���������״̬ʱ�䵽����ʧ, ����ﱻ���

��: ���˾�����ת��ʱ, summon�����Ĺ��ﱻ�Զ����
	
��:	����AI

    function() û��Ŀ��
	
		if(��⸽���Ƿ��к��ʵ�Ŀ��)
		{
			����Ŀ�����
		}
		else
		{
			�����������̫Զ, �򿿽�
		}

		ȡ���������˵Ķ����б�ĵ�һ��
		if(��Ϊ��)
		{
			����Ŀ�����
		}
		
	end

  
	function() ��Ŀ��
		
		if(Ŀ���������̫Զ || Ŀ������ || Ŀ�������Ѳ�����)
		{
			���Ŀ��
		}
		else
		{
			��Ŀ��ʹ�ü���
		}
	end

 
��: ��Ƭ����������

    ����1: ������
    ����2: ��ʶ����ȼ�����ֵ
    ����3: ����ʹ�õĴ���

    ����1����ÿ�Ƭʱ����, ����2������3���Զ�̬�ı�



*/











































































































