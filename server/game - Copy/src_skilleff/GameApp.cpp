//=============================================================================
// FileName: GameApp.cpp
// Creater: ZhangXuedong
// Date: 2004.11.04
// Comment: CGameApp class
//=============================================================================

#include "stdafx.h"
#include "GameApp.h"
#include "GameAppNet.h"
#include "SystemDialog.h"
#include "lua_gamectrl.h"
#include "GameDB.h"

#include "TradeLogDB.h" //Add by lark.li 20080324

#include "EntityAlloc.h"
#include "Character.h"
#include "Player.h"
#include "AttachManage.h"
#include "Item.h"
#include "CharTrade.h"
#include "Config.h"
#include "JobInitEquip.h"

#include "CharacterRecord.h"
#include "SkillRecord.h"
#include "SkillStateRecord.h"
#include "ItemRecord.h"
#include "ItemAttr.h"
#include "LevelRecord.h"
#include "SailLvRecord.h"
#include "LifeLvRecord.h"
//#include "CharForge.h" -������ -Waiting Mark 2009-03-25
#include "HairRecord.h"
#include "Parser.h"
#include "WorldEudemon.h"
#include "Birthplace.h"
#include "CharBoat.h"
#include "MapEntry.h"
#include "CharStall.h" // Ϊ�˲����ڴ�
#include "MapCopyRecord.h"

_DBC_USING;
bool		g_bLogEntity = false;

CItemRecordAttr*  g_pCItemAttr = NULL;
CCharacter*	g_pCSystemCha = NULL;
SubMap*		g_pScriptMap = NULL;		// �ű��ó�ʼ����ͼ��Ϣȫ�ֱ���
long		g_lDeftMMaskLight = 21;
string		g_strChaState[2];

// ��Ϸѭ���е�ǰ�����ֵ�ִ�е�cha
uLong		g_ulCurID = defINVALID_CHA_ID;
Long		g_lCurHandle = defINVALID_CHA_HANDLE;
//


extern BOOL g_bGameEnd;
//extern std::string g_strLogName;

char		szChaInfoName[256]			= "CharacterInfo";
char		szSkillInfoName[256]		= "skillinfo";
char		szSkillStateInfoName[256]	= "skilleff";
char		szChaLvUpName[256]			= "character_lvup";
char		szChaSailLvUpName[256]		= "saillvup";
char		szChaLifeLvUpName[256]		= "lifelvup";
char		szItemInfoName[256]			= "ItemInfo";
char		szInitChaItName[256]		= "Int_Cha_Item";
// char		g_szForgeTable[256]			= "forgeitem";  �����ñ�ı� -Waiting Mark 2009-03-25
char		szIslandInfoName[256]		= "AreaSet";
char		szHairInfoName[64]			= "Hairs";		// �ɸ����ķ��ͼ�¼��
char		szNoLockItemName[64]		= "NoLock";//Add by sunny.sun 20090710
char		szMapCopyInfoName[64]		= "MapCopySet";//Add by sunny.sun 20090811

CAreaSet * CAreaSet::_Instance = NULL;

void ChaException(uLong ulCurID, Long lCurHandle)
{
	if(g_ulCurID == defINVALID_CHA_ID || g_lCurHandle == defINVALID_CHA_HANDLE)
	{
		LG("exception3", "unknown (ID:%u, Handle:%d)occur abnormity\n", ulCurID, lCurHandle);
		return ;
	}

	Entity	*pCEnti = g_pGameApp->IsValidEntity(ulCurID, lCurHandle);
	if (!pCEnti)
	{
		//LG("exception3", "δ֪��ʵ�壨ID:%u, Handle:%d���������쳣\n", ulCurID, lCurHandle);
		LG("exception3", "unknown entity(ID:%u, Handle:%d)occur abnormity\n", ulCurID, lCurHandle);
		return;
	}
	CCharacter *pCurCha = pCEnti->IsCharacter();
	if (!pCurCha)
	{
		//LG("exception3", "δ֪�Ľ�ɫ��ID:%u, Handle:%d���������쳣\n", ulCurID, lCurHandle);
		LG("exception3", "unknown character(ID:%u, Handle:%d)occur abnormity\n", ulCurID, lCurHandle);
		return;
	}

	try
	{
		//LG("exception3", "��ɫ[%s] [%s]�����쳣, �����߳���Ϸ\n", pCurCha->GetLogName(), pCurCha->GetPlyMainCha()->GetLogName());
		LG("exception3", "character[%s] [%s]occur abnormity, will be kick out game\n", pCurCha->GetLogName(), pCurCha->GetPlyMainCha()->GetLogName());
		// ....
		CPlayer	*pCPlayer = pCurCha->GetPlayer();
		if (pCPlayer)
		{
			//LG("exception3", "��ҽ�ɫ[%s] [%s]�����쳣, [GoOutGame]\n", pCurCha->GetLogName(), pCurCha->GetPlyMainCha()->GetLogName());
			LG("exception3", "player character[%s] [%s]occur abnormity, [GoOutGame]\n", pCurCha->GetLogName(), pCurCha->GetPlyMainCha()->GetLogName());
			KICKPLAYER(pCPlayer, 0);
			LG("exception3", "End [KICKPLAYER], Begin[GoOutGame]\n");
			g_pGameApp->GoOutGame(pCPlayer, true);
			LG("exception3", "End [GoOutGame]\n");
			return;
		}
		else
		{
			//LG("exception3", "�ͷŹ����ɫ[%s], Begin\n", pCurCha->GetName());
			LG("exception3", "release bugbear character[%s], Begin\n", pCurCha->GetName());
			pCurCha->Free();
			//LG("exception3", "End �ͷŹ����ɫ\n");
			LG("exception3", "End release bugbear character\n");
		}
	}
	catch (...)
	{
		//LG("exception3", "����ɫѭ�������쳣ʱ, �߳���ɫ�Ĺ������ַ����쳣, [%s]\n", pCurCha->GetName());
		LG("exception3", "when character loop occur abnormity, the process kick character occur abnormity again, [%s]\n", pCurCha->GetName());
	}
}

// ��Ϸ�߼����߳�
DWORD WINAPI g_GameLogicProcess(LPVOID lpParameter)
{
#ifdef __CATCH
	SEHTranslator translator;
#endif	
	DWORD dwLastTick;
	DWORD dwCurTick;
	DWORD dwRunTick;
	//SYSTEMTIME st;
	static char szLogTime[128] = {0};
	char szTemp[128] = {0};
	//std::string strLogName = "GameServerLog";

	//GameServerLG(g_strLogName.c_str(), "GameServer Begin...\n");

    while (!g_bGameEnd)
	{
		T_B
		
		DWORD	dwInterval = 50; // ����

		//lua_FrameMove();
		if(g_pGameApp->m_bExecLuaCmd) 
		{
			lua_dofile(g_pLuaState, "tmp.txt");
			g_pGameApp->m_bExecLuaCmd = FALSE;
		}

		//GameServerLG(g_strLogName.c_str(), "Loop Begin...\n");

		//״̬����
		dwLastTick = GetTickCount();
		g_pGameApp->Run(dwLastTick);
		dwCurTick = GetTickCount();
		dwRunTick = dwCurTick - dwLastTick;

		//GameServerLG(g_strLogName.c_str(), "GameAppRun End!\n");

		//����������Ϣ����
		dwLastTick = dwCurTick;
		PEEKPACKET(50 > dwRunTick ? 50 - dwRunTick : 0);
		dwCurTick = GetTickCount();
		dwRunTick += dwCurTick - dwLastTick;

		//GameServerLG(g_strLogName.c_str(), "PEEKPACKET End!\n");

		//TradeServer��Ϣ����
		dwLastTick = dwCurTick;
        g_gmsvr->GetTradeServer()->PeekMsg(50 > dwRunTick ? 50 - dwRunTick : 0);
		g_StoreSystem.Run(dwCurTick, 10000, 10000);
		dwCurTick = GetTickCount();
		dwRunTick += dwCurTick - dwLastTick;

		//GameServerLG(g_strLogName.c_str(), "Loop End, RunTime[%ld]ms!\n", dwRunTick);
		    
		//g_pGameApp->m_dwRunStep = 104;
		//Add by sunny.sun 20090519
		bool begin = false;
#ifdef SHUI_JING
		if( g_Config.m_ResetFlag )
		{
			g_pGameApp->m_ResetTimer = CTime::GetCurrentTime();
			if(  g_pGameApp->m_ResetTimer.GetDayOfWeek()-1 == 0 && g_pGameApp->m_ResetTimer.GetHour() == 3
				&& g_pGameApp->m_ResetTimer.GetMinute() == 0 && g_pGameApp->m_ResetTimer.GetSecond() == 0)
			{
				begin = true;
			}
			if( begin )
			{
				g_pGameApp->ResetHangState();
				LG("Crystal_Ret","Begin Reset the Flat!\n");
				begin = false;
				g_pGameApp->m_ResetTimer = dwCurTick;
			}
		}
#endif

		T_EXIT
	}
	//LG("init", "��Ϸ�߳̽���!\n");
	LG("init", "game thread finish!\n");
	return 0;
}

// Add by lark.li 20080324 begin
void CDBLogMgr::TradeLog(const char* action, const char *pszChaFrom, const char *pszChaTo, const char *pszTrade)
{
	if( g_Config.m_bTradeLogIsConfig )
	{
		tradeLog_db.ExecLogSQL(g_Config.m_szName, action, pszChaFrom, pszChaTo, pszTrade);
	}
	else
	{
		game_db.ExecTradeLogSQL(g_Config.m_szName, action, pszChaFrom, pszChaTo, pszTrade);
	}
}

// End

// ����Log 5���ַ����ֶ�, ���һ������Ϊ8000�ַ�����
void CDBLogMgr::Log(const char *type, const char *c1, const char *c2, const char *c3, const char *c4, const char *p, BOOL bAddToList)
{	
	return;
	
	if(bAddToList)
	{
		SDBLogData *pData = &_LogPool[_nPoolUseLoc];
		_nPoolUseLoc++;
		if(_nPoolUseLoc >= MAX_DBLOG_POOL)
		{
			_nPoolUseLoc = 0;
		}
		//sprintf(pData->szLog, "insert gamelog (action, c1, c2, c3, c4, content) \
		//	   values('%s', '%s', '%s', '%s', '%s', '%s')", type, c1, c2, c3, c4, p);
		_snprintf_s(pData->szLog,sizeof(pData->szLog),_TRUNCATE, "insert gamelog (action, c1, c2, c3, c4, content) \
							  values('%s', '%s', '%s', '%s', '%s', '%s')", type, c1, c2, c3, c4, p);

		_LogList.push_back(pData);
	}
	else
	{
		char szLog[8192];	
		//sprintf(szLog, "insert gamelog (action, c1, c2, c3, c4, content) \
		//	   values('%s', '%s', '%s', '%s', '%s', '%s')", type, c1, c2, c3, c4, p);
		_snprintf_s(szLog,sizeof(szLog),_TRUNCATE, "insert gamelog (action, c1, c2, c3, c4, content) \
					   values('%s', '%s', '%s', '%s', '%s', '%s')", type, c1, c2, c3, c4, p);

		game_db.ExecLogSQL(szLog);
	}
}


// ����ʱ����, ÿ��д��һ��������log��DB
void CDBLogMgr::HandleLogList() 
{
	if(_LogList.empty()) 
	{
		_nLogLeft = 0;
		return;
	}
		
	_nLogLeft = (int)(_LogList.size());
		
	
	LETimer t; t.Begin();
	
	
	BOOL bFlush = FALSE;
	// ÿ��ִֻ�й̶�����
	for(int i = 0; i < _nPerLogCnt; i++)
	{
		if(_LogList.empty()) break;
		SDBLogData *pData = _LogList.front();
		game_db.ExecLogSQL(pData->szLog);
		_LogList.pop_front();

		// �����ִ�е�λ���Ѿ�����pool����ʹ�õ�λ��
		if(pData->nLoc > _nPoolUseLoc)
		{
			if(pData->nLoc - _nPoolUseLoc < 10) // ����λ���Ѿ��ܽӽ�, ��Ҫ�ص���, ����
			{
				bFlush = TRUE;
				//LG("dblog_error", "DBLogPoolλ�ü����ص�HandleLoc=[%d], PoolUseLoc=[%d]����DBLog�ٶȲ�����, ִ��Flush����ʣ��log!\n", pData->nLoc, _nPoolUseLoc);
				LG("dblog_error", "DBLogPool position will superpose HandleLoc=[%d], PoolUseLoc=[%d]deal with DBLog speed abnormity, carry out Flush all leavings log!\n", pData->nLoc, _nPoolUseLoc);
				break;
			}
		}
	}

	if(bFlush)
	{
		FlushLogList();
	}

	LG("dblog", "dblog exec sql use time = %d\n", t.End());
}

// GameServer�رյ�ʱ��, ��֤ʣ�µ�log��д��DB
void CDBLogMgr::FlushLogList()  
{
	for(list<SDBLogData*>::iterator it = _LogList.begin(); it!=_LogList.end(); it++)
	{
		SDBLogData *pData = (*it);
		game_db.ExecLogSQL(pData->szLog);
	}

	_LogList.clear();
}




CGameApp::CGameApp()
:_dwLastTick(0),
 _dwTempRunCnt(0),
 m_dwRunCnt(0),
 m_dwFPS(0),
 m_bExecLuaCmd(0),
 m_dwChaCnt(0),
 m_dwPlayerCnt(0),
 m_dwRunStep(0),
 m_CabinHeap(1, 1000),
 m_MapStateCellHeap(1, 2000),
 m_ChaListHeap(1, 2000),
 m_StateCellNodeHeap(1, 1000),
 m_SkillTDataHeap(1, defMAX_SKILL_NO),
 m_TradeDataHeap(1, ROLE_MAXSIZE_TRADEDATA),
 m_StallDataHeap(1, ROLE_MAXSIZE_STALLDATA),
 m_mapnum(0),
 m_ulLeftSec(0)
{T_B
	extern CGameApp *g_pGameApp;
	g_pGameApp = this;
	for (int i = 0; i < MAX_GATE; i++)
		m_GatePlayer[i].pCPlayerL = 0;
	for (int i = 0; i <= defMAX_SKILL_NO; i++)
		for (int j = 0; j <= defMAX_SKILL_LV; j++)
			m_pCSkillTData[i][j] = 0;

	m_lCabinHeapNum = 0;
	m_lTradeDataHeapNum = 0;
	m_lSkillTDataHeapNum = 0;
	m_lMapMgrUnitHeapNum = 0;
	m_lEntityListHeapNum = 0;
	m_lMgrNodeHeapNum = 0;
	m_pCEntSpace = 0;
	m_pCPlySpace = 0;
	m_PicSet = NULL;
	ChaAttrMaxValInit(false);
T_E}


CGameApp::~CGameApp()
{T_B
	
	//Log("�ر�", "haha", "", "" ,"", "");
	Log("close", "haha", "", "" ,"", "");
	FlushLogList();

	for(short i=0; i< m_mapnum; i++)
	{
		SAFE_DELETE(m_MapList[i]);
	}
	
	CloseLuaScript();
	//g_CParser.Free();
	
	SAFE_DELETE(m_CChaRecordSet);
	SAFE_DELETE(m_CSkillRecordSet);
	SAFE_DELETE(m_pCEntSpace);
	SAFE_DELETE(m_pCPlySpace);

	SAFE_DELETE(m_PicSet);
	SAFE_DELETE(m_CNoLockRecordSet);//Add by sunny.sun 20090710
	SAFE_DELETE(m_CMapCopyRecordSet);//Add by sunny.sun 20090811

	m_vecVolunteerList.clear();
T_E}


const char* GetResPath(const char *pszRes)
{
	static char g_szTableName[255];
	string str = g_Config.m_szResDir;
	if(str.size() > 0)
	{
		str+="/";
	}
	str+=pszRes;
	//strcpy(g_szTableName, str.c_str());
	strncpy_s( g_szTableName, sizeof(g_szTableName), str.c_str(), _TRUNCATE );
	return g_szTableName;
}

BOOL LoadTable(CRawDataSet *pTable, const char *pszTableName)
{
	string str = GetResPath(pszTableName);
	if(pTable->LoadRawDataInfo(str.c_str())==FALSE)
	{
		//LG("error", "msg��ȡ���[%s]ʧ��\n", str.c_str());
		LG("error", RES_STRING(GM_GAMEAPP_CPP_00003), str.c_str());
		return FALSE;
	}
	return TRUE;
}


BOOL CGameApp::Init()
{T_B

	//LG("init", "��ʼ��ʼ��GameApp\n");
	LG("init", "start initialization GameApp\n");

	//LG("init", "��ʼ�����ݿ�...\n");
	LG("init", "initialization DB...\n");

    string pswd;
    char* buf = g_Config.m_szDBPass;
    int buf_len = (int)strlen(buf);

	switch (buf_len % 3)
	{
        case 0: break;
        case 1: /*strcat(buf, "==")*/strncat_s( buf,sizeof(g_Config.m_szDBPass), "==",_TRUNCATE); break;
        case 2: /*strcat(buf, "=")*/strncat_s( buf,sizeof(g_Config.m_szDBPass), "=",_TRUNCATE); break;
	}

    dbpswd_out(buf, (int)strlen(buf), pswd);

	strncpy(g_Config.m_szDBPass, pswd.c_str(), sizeof(g_Config.m_szDBPass));

	if(game_db.Init())
	{	// ��ʼ�����ݿ�
		LG("init", "database init...ok\n");
	}
	else
	{
		LG("init", "database init...Fail!\n");
		return FALSE;
	}

#ifdef UPDATE_MUL_THREAD
	// ��Ϊ�޸�Ϊ�������ӳش���Ҫ�������޸�Ϊ���ݿ�ģ���
	CGameDBPool::Instance(3);
#endif

	// Add by lark.li 20080324 begin
	if(tradeLog_db.Init())
	{	// ��ʼ�����ݿ�
		LG("init", "database init...ok\n");
	}
	else
	{
		LG("init", "database init...Fail!\n");
		return FALSE;
	}
	// End

	ChaAttrMaxValInit(false);
	if( !CTextFilter::LoadFile( GetResPath("ChaNameFilter.txt" )) )
	{
		//LG( "init", "msg��ʼ��װ�����ֹ�����Ϣ�ļ�ʧ��!\n" );
		LG( "init", RES_STRING(GM_GAMEAPP_CPP_00004) );
		return FALSE;		
	}
	//m_Ident.m_maxID = g_Config.m_ulBaseID; //���ID�������Ļ���
	m_Ident.m_maxID = 0xafffffff; //���ID�������Ļ���
	if(!m_Ident.m_maxID)
		//LG("init", "�����ID����!!!\n");
		LG("init", "error ID base!!!\n");

	m_pCPlySpace = new CPlayerAlloc(g_Config.m_nMaxPly);
	if (!m_pCPlySpace)
	{
		//LG("init", "msg������ҿռ����!, �˳�!\n");
		LG("init", RES_STRING(GM_GAMEAPP_CPP_00005));
		return FALSE;
	}

	m_pCEntSpace = new CEntityAlloc(g_Config.m_nMaxCha + g_Config.m_nMaxPly * 3, g_Config.m_nMaxItem, g_Config.m_nMaxTNpc);
	if (!m_pCEntSpace)
	{
		//LG("init", "msg�����ɫ�ռ����!, �˳�!\n");
		LG("init", RES_STRING(GM_GAMEAPP_CPP_00017));
		return FALSE;
	}
	g_pCSystemCha = GetNewCharacter();
	// Add by lark.li 20081117 begin
	if(!g_pCSystemCha)
	{
		//LG("init", "msg�����ɫ�ռ����!, �˳�!\n");
		LG("init", RES_STRING(GM_GAMEAPP_CPP_00017));
		return FALSE;
	}
	// End
	g_pCSystemCha->SetID(m_Ident.GetID());
	//g_pCSystemCha->SetName("ϵͳ");
	g_pCSystemCha->SetName(RES_STRING(GM_GAMEAPP_CPP_00018));
	g_pCSystemCha->setAttr(ATTR_CHATYPE, enumCHACTRL_NONE, 1);

    //LG("init", "��ʼ�������Entity�ڴ�\n");
	LG("init", "start to assign every Entity memory\n");

#if MEM_CHECK
	//��������΢�����
	size_t t1 = sizeof(CPassengerMgr) * 1000;
	size_t t2 = sizeof(mission::CTradeData) * ROLE_MAXSIZE_TRADEDATA;
	size_t t3 = sizeof(mission::CStallData) * ROLE_MAXSIZE_STALLDATA;
	size_t t4 = sizeof(CSkillTempData) * defMAX_SKILL_NO;
	size_t t5 = sizeof(CStateCell) * 2000;
	size_t t6 = sizeof(CChaListNode) * 2000;
	size_t t7 = sizeof(CStateCellNode) * 1000;

	size_t t = (t1 + t2 + t3 + t4 + t5 + t6 + t7) / 1024 / 1024;
#endif
	m_CabinHeap.Init();
	m_TradeDataHeap.Init();
	m_MapStateCellHeap.Init();
	m_ChaListHeap.Init();
	m_SkillTDataHeap.Init();
	m_StateCellNodeHeap.Init();
	m_StallDataHeap.Init();

	//LG("init", "��ʼ��������...\n");
	LG("init", "initialization every form...\n");
	int	nItemRecordNum = g_Config.m_ItemInfoLimit; //CItemRecord::enumItemMax;
	int mCharRecordNum = g_Config.m_CharInfoLimit;
	int mSkilRecordNum = g_Config.m_SkilInfoLimit;
	int mSEffRecordNum = g_Config.m_SkillEffLimit;

	m_CChaRecordSet = new CChaRecordSet(0, mCharRecordNum);
	m_CSkillRecordSet = new CSkillRecordSet(0, mSkilRecordNum);
	m_CSkillStateRecordSet = new CSkillStateRecordSet(0, mSEffRecordNum);
	/*m_CChaRecordSet			= new CChaRecordSet(0, 2500);*/			LoadTable(m_CChaRecordSet,		 szChaInfoName);
	/*m_CSkillRecordSet		= new CSkillRecordSet(0, defMAX_SKILL_NO);*/			LoadTable(m_CSkillRecordSet,	 szSkillInfoName);
	m_CLevelRecordSet		= new CLevelRecordSet(0, 600);			LoadTable(m_CLevelRecordSet,	 szChaLvUpName);
	m_CSailLvRecord			= new CSailLvRecordSet(0, 600);			LoadTable(m_CSailLvRecord,		 szChaSailLvUpName);
	m_CLifeLvRecord			= new CLifeLvRecordSet(0, 600);			LoadTable(m_CLifeLvRecord,		 szChaLifeLvUpName);
	m_CHairRecord			= new CHairRecordSet(0, 300);			LoadTable(m_CHairRecord,		 szHairInfoName);	
	// Modify by lark.li 20080822 begin
	//m_CSkillStateRecordSet	= new CSkillStateRecordSet(0, SKILL_STATE_MAXID);		LoadTable(m_CSkillStateRecordSet,szSkillStateInfoName);
	
	/*m_CSkillStateRecordSet	= new CSkillStateRecordSet(0, 300);*/
	LoadTable(m_CSkillStateRecordSet,szSkillStateInfoName);
	
	// End
	
	m_CItemRecordSet		= new CItemRecordSet(0, nItemRecordNum);LoadTable(m_CItemRecordSet, szItemInfoName);
	m_CNoLockRecordSet			= new CNoLockRecordSet(0, 300);			LoadTable(m_CNoLockRecordSet, szNoLockItemName);//Add by sunny.sun 20090710
	m_CMapCopyRecordSet		= new CMapCopyRecordSet(0,30);			LoadTable(m_CMapCopyRecordSet,	szMapCopyInfoName);//Add by sunny.sun 20090811

	//�������ʱ����
	/*m_PicSet = new CPicSet();
	if(!m_PicSet->init())
	{
		LG( "init", "msg��ȡͼƬʧ�ܣ�" );
		return FALSE;
	}*/
	
	g_pCItemAttr = new CItemRecordAttr[nItemRecordNum];
	for (int i = 0; i < nItemRecordNum; i++)
	{
		g_pCItemAttr[i].Init(i);
	}
	
	m_CJobEquipRecordSet = new CJobEquipRecordSet(0, MAX_JOB_TYPE + 1); LoadTable(m_CJobEquipRecordSet, szInitChaItName);
	m_CAreaRecordSet = new CAreaSet(0, 256);							LoadTable(m_CAreaRecordSet, szIslandInfoName);

//�����ñ�ı� -Waiting Mark 2009-03-25
// 	if( !g_ForgeSystem.LoadForgeData( g_szForgeTable ) )
// 	{
// 		//LG( "init", "msg��ȡ�������ݱ���Ϣʧ�ܣ�" );
// 		LG( "init", RES_STRING(GM_GAMEAPP_CPP_00006) );
// 		return FALSE;
// 	}

	if( !g_CharBoat.Load( "ShipInfo", "ShipItemInfo"))
	{
		//LG( "init", "msg��ȡ��ֻ���ݱ���Ϣʧ�ܣ�" );
		LG( "init", RES_STRING(GM_GAMEAPP_CPP_00007) );
		return FALSE;
	}

	//LG("init", "��ʼ��Lua Script...\n");
	LG("init", "initialization Lua Script...\n");
	InitLuaScript();

	if(InitMap()==FALSE)
	{
		//LG("init", "��ͼ��ʼ��ʧ��!, �˳�!\n");
		LG("init", "initialization map failed!, exit!\n");
		return FALSE;
	}

	InitSStateTraOnTime();

	if (!IsChaAttrMaxValInit())
	{
		//LG("init", "��ɫ�������ֵ...Fail!\n");
		LG("init", "character attribute max...Fail!\n");
		return FALSE;
	}

	//LG("init", "GameApp��ʼ������!\n");
	LG("init", "GameApp initialization finish!\n");

	//LG("size", "sizeof(Entity) = %d,  sizeof(Character) = %d\n", sizeof(Entity), sizeof(CCharacter));
	
	m_CTimerItem.Begin(3 * 1000);

	return TRUE;
T_E}
    



void CGameApp::Run(DWORD dwCurTime)
{T_B
	//m_dwRunStep = 0;

	DWORD	dwChaCnt, dwPlayerCnt, dwActiveMgrUnit;
	dwChaCnt = m_dwChaCnt;
	dwPlayerCnt = m_dwPlayerCnt;
	dwActiveMgrUnit = m_dwActiveMgrUnit;

	static DWORD	dwRunTick = GetTickCount();
	static Long		lLogTime = 0;
	LETimer t, t1;
	t1.Begin();
	//m_dwRunStep = 1;

	t.Begin();
    MgrUnitRun(dwCurTime);
	DWORD	dwMgrUnitRunTime = t.End();

	//GameServerLG(g_strLogName.c_str(), "MgrUnitRun End!\n");

	t.Begin();
	GameItemRun(dwCurTime);
	DWORD	dwItemRunTime = t.End();

	//GameServerLG(g_strLogName.c_str(), "GameItemRun End!\n");

	t.Begin();
	MapMgrRun(dwCurTime);
	DWORD	dwMapMgrRunTime = t.End();

	//GameServerLG(g_strLogName.c_str(), "MapMgrRun End!\n");

	t.Begin();
	DWORD	dwDataStatisticTime = t.End();

	t.Begin();
	//m_dwRunStep = 100;
    g_CTimeSkillMgr.Run(dwCurTime);
	DWORD	dwSkillMgrRunTime = t.End();

	//GameServerLG(g_strLogName.c_str(), "TimeSkillMgrRun End!\n");

	t.Begin();
	//m_dwRunStep = 103;

	//GameServerLG(g_strLogName.c_str(), "SaveMap End!\n");

	DWORD	dwGameRunTime = t1.End();

	// ��Ϸѭ�����ʺ���ʱ��ͳ��
    if(dwCurTime - _dwLastTick >= 1000)
    {
        m_dwFPS       = _dwTempRunCnt;
        _dwTempRunCnt = 0;
        _dwLastTick   = dwCurTime;

		if(m_dwFPS < 16)
			LG("GameRunTime", "fps = %u\n", m_dwFPS);
    }
    m_dwRunCnt++;     // ѭ���ܴ���, ��������ʱ���
    _dwTempRunCnt++;  // ֡����
    //m_dwRunStep = 104;

	if (m_ulLeftSec > 0 && m_CTimerReset.IsOK(dwCurTime))
	{
		NotiGameReset(m_ulLeftSec);
	    //m_dwRunStep = 501;
		m_ulLeftSec--;
		if (m_ulLeftSec == 0) // �����ɫ��������Ϸ���˺�Ĵ��벻Ӧ�������ý�ɫ
		{
			SaveAllPlayer();
		    //m_dwRunStep = 502;
			g_bGameEnd = true;
			return;
		}
	}
    //m_dwRunStep = 503;

	// ��¼����ʱ��
	DWORD	dwRunTimeKey = 100;
	bool	bLogRunTime = false;
    if (dwCurTime - dwRunTick >= 30 * 1000)
		bLogRunTime = true;
	if (dwGameRunTime >= dwRunTimeKey)
	{
		if (lLogTime >= 0)
		{
			lLogTime++;
			if (lLogTime <= 6) bLogRunTime = true;
			else lLogTime = -1;
		}
		else
		{
			lLogTime--;
			if (lLogTime < -1000) lLogTime = 0;
		}
	}
	else
	{
		if (lLogTime > 0)
			lLogTime = 0;
	}
	if (bLogRunTime)
	{
		dwRunTick = dwCurTime;

		if (dwGameRunTime >= dwRunTimeKey)
		{
			//LG("GameRunTime", "!!!GameRunTime = %u\t\tMgrUnitRunTime = %u\tItemRunTime = %u\tMapMgrRunTime = %u\tDataStatisticTime = %u\tSkillMgrRunTime = %u\tMapMaskSaveTime = %u\n",
			LG("GameRunTime", "!!!GameRunTime = %u\t\tMgrUnitRunTime = %u\tItemRunTime = %u\tMapMgrRunTime = %u\tDataStatisticTime = %u\tSkillMgrRunTime = %u\n",
				//dwGameRunTime, dwMgrUnitRunTime, dwItemRunTime, dwMapMgrRunTime, dwDataStatisticTime, dwSkillMgrRunTime, dwMapMaskSaveTime);
				dwGameRunTime, dwMgrUnitRunTime, dwItemRunTime, dwMapMgrRunTime, dwDataStatisticTime, dwSkillMgrRunTime);
		}
		else
		{
			//LG("GameRunTime", "...GameRunTime = %u\t\tMgrUnitRunTime = %u\tItemRunTime = %u\tMapMgrRunTime = %u\tDataStatisticTime = %u\tSkillMgrRunTime = %u\tMapMaskSaveTime = %u\n",
			LG("GameRunTime", "...GameRunTime = %u\t\tMgrUnitRunTime = %u\tItemRunTime = %u\tMapMgrRunTime = %u\tDataStatisticTime = %u\tSkillMgrRunTime = %u\n",
				//dwGameRunTime, dwMgrUnitRunTime, dwItemRunTime, dwMapMgrRunTime, dwDataStatisticTime, dwSkillMgrRunTime, dwMapMaskSaveTime);
				dwGameRunTime, dwMgrUnitRunTime, dwItemRunTime, dwMapMgrRunTime, dwDataStatisticTime, dwSkillMgrRunTime);
		}
	}
	//
T_E}

void CGameApp::MgrUnitRun(DWORD dwCurTime)
{T_B
	SubMap		*pCSubMap;

	m_dwChaCnt = m_dwPlayerCnt;
	m_dwActiveMgrUnit = 0;

	static DWORD	dwTick = 0;
	if (dwCurTime - dwTick >= 1 * 60 * 1000)
	{
		dwTick = dwCurTime;
		LG("EntityNum", "Ply[%5d %5d %5d],\tCha[%5d %5d %5d],\tItem[%5d %5d %5d],\tTNpc[%5d %5d %5d]\n",
			m_pCPlySpace->GetHoldPlyNum(), m_pCPlySpace->GetMaxHoldPlyNum(), m_pCPlySpace->GetAllocPlyNum(),
			m_pCEntSpace->GetHoldChaNum(), m_pCEntSpace->GetMaxHoldChaNum(), m_pCEntSpace->GetAllocChaNum(),
			m_pCEntSpace->GetHoldItemNum(), m_pCEntSpace->GetMaxHoldItemNum(), m_pCEntSpace->GetAllocItemNum(),
			m_pCEntSpace->GetHoldTNpcNum(), m_pCEntSpace->GetMaxHoldTNpcNum(), m_pCEntSpace->GetAllocTNpcNum());
	}

	CEyeshotCell	*pCMgrUnit;
	CStateCell		*pCStateCell;
	Entity			*pCEnt, *pCFlagEnt;
	Short			sMapCpyNum;
	DWORD			dwActMgrCellNum;

	LETimer			t, t1, t2;
	DWORD			dwPartRunTime[8];
	Char			chPartCount;
	CCharacter		*pCLongTimeCha;
	DWORD			dwTempTick1, dwTempTick2;
	static DWORD	dwMapRunTick[MAX_MAP] = {0};
	static Long		lMapLogTime[MAX_MAP] = {0};
	bool			bLogRun;
	DWORD			dwRTimeKey = 60;

	// ��ͼ����
	for (short i = 0; i < m_mapnum; i++)
	{
		if (!m_MapList[i]->IsOpen()) continue;

		t1.Begin();
		dwActMgrCellNum = 0;
		pCLongTimeCha = 0;
		dwTempTick1 = 0;
		chPartCount = 0;

		sMapCpyNum = m_MapList[i]->GetCopyNum();
		// ��������
		for (short j = 0; j < sMapCpyNum; j++)
		{
			pCSubMap = m_MapList[i]->GetCopy(j);
			if (!pCSubMap)
				continue;

			// ���ͼ������������������Ҹ�������Ԥ��ֵʱ�򿪣�
			pCSubMap->CheckRun();
			if (!pCSubMap->IsRun())
				continue;

			extern SubMap *g_pScriptMap;
			g_pScriptMap = pCSubMap; // ��ô����AIѭ����, �Ͳ���ָ����ǰ��ͼ

			m_dwChaCnt += pCSubMap->GetMonsterNum();

			// �������Ұ��Ԫ�б�
			dwActMgrCellNum += pCSubMap->m_CEyeshotCellL.GetActiveNum();

			chPartCount = 0;

			t.Begin();

			// ����������
			pCSubMap->m_WeatherMgr.Run(pCSubMap);
			dwPartRunTime[chPartCount++] = t.End();
			
			//m_dwRunStep++;
			
			t.Begin();
			pCSubMap->m_CEyeshotCellL.BeginGet();
			// ��Ұ��Ԫ�࣬��¼��Ԫ�ڵ����н�ɫ�͵��ߣ��Լ�״̬��Ԫ��
			while (pCMgrUnit = pCSubMap->m_CEyeshotCellL.GetNext())
			{
				// ��ɫ��
				pCEnt = pCMgrUnit->m_pCChaL;
				while (pCEnt)
				{
					g_ulCurID = pCEnt->GetID();
					g_lCurHandle = pCEnt->GetHandle();

					t2.Begin();
					pCEnt->Run(dwCurTime);
					dwTempTick2 = t2.End();
					if (dwTempTick2 > dwTempTick1)
					{
						pCLongTimeCha = pCEnt->IsCharacter();
						dwTempTick1 = dwTempTick2;
					}

					pCFlagEnt = pCEnt;
					pCEnt = pCEnt->m_pCEyeshotCellNext;
					((CCharacter *)pCFlagEnt)->RunEnd( dwCurTime );
				}

				g_ulCurID = defINVALID_CHA_ID;
				g_lCurHandle = defINVALID_CHA_HANDLE;
			}
			dwPartRunTime[chPartCount++] = t.End();

			t.Begin();
			long	lActCount = 0;

			// �����״̬��Ԫ�б�
			long	lTarNum = pCSubMap->m_CStateCellL.GetActiveNum();
			pCSubMap->m_CStateCellL.BeginGet();
			while (pCStateCell = pCSubMap->m_CStateCellL.GetNext())
			{
				if (++lActCount > lTarNum)
				{
					//LG("״̬��Ԫ����", "ʵ�ʼ����� %d\n", lTarNum);
					LG("state cell error", "fact activation number %d\n", lTarNum);
					break;
				}
				pCStateCell->StateRun(dwCurTime, pCSubMap);
			}
			dwPartRunTime[chPartCount++] = t.End();
		}
		m_dwActiveMgrUnit += dwActMgrCellNum;

		DWORD dwMMgrRunTime = t1.End();
		bLogRun = false;
		if (dwCurTime - dwMapRunTick[i] >= 30 * 1000)
			bLogRun = true;
		if (dwMMgrRunTime >= dwRTimeKey)
		{
			if (lMapLogTime[i] >= 0)
			{
				lMapLogTime[i]++;
				if (lMapLogTime[i] <= 6) bLogRun = true;
				else lMapLogTime[i] = -1;
			}
			else
			{
				lMapLogTime[i]--;
				if (lMapLogTime[i] < -1000) lMapLogTime[i] = 0;
			}
		}
		else
		{
			if (lMapLogTime[i] > 0)
				lMapLogTime[i] = 0;
		}
		if(bLogRun)
		{
			dwMapRunTick[i] = dwCurTime;

			if (chPartCount > 0)
			{
				if (dwMMgrRunTime >= dwRTimeKey)
					/*LG("map_time", "!!!%s(%d)���� = %d ms, WeatherRun = %u, CharacterRun = %u, StateRun = %u, ActiveMgrUnitNum = %d\n",
						m_MapList[i]->GetName(), sMapCpyNum, dwMMgrRunTime, dwPartRunTime[0], dwPartRunTime[1], dwPartRunTime[2], dwActMgrCellNum);*/
						LG("map_time", "!!!%s(%d) expend = %d ms, WeatherRun = %u, CharacterRun = %u, StateRun = %u, ActiveMgrUnitNum = %d\n",
						m_MapList[i]->GetName(), sMapCpyNum, dwMMgrRunTime, dwPartRunTime[0], dwPartRunTime[1], dwPartRunTime[2], dwActMgrCellNum);
				else
					/*LG("map_time", "...%s(%d)���� = %d ms, WeatherRun = %u, CharacterRun = %u, StateRun = %u, ActiveMgrUnitNum = %d\n",
						m_MapList[i]->GetName(), sMapCpyNum, dwMMgrRunTime, dwPartRunTime[0], dwPartRunTime[1], dwPartRunTime[2], dwActMgrCellNum);*/
						LG("map_time", "...%s(%d)expend = %d ms, WeatherRun = %u, CharacterRun = %u, StateRun = %u, ActiveMgrUnitNum = %d\n",
						m_MapList[i]->GetName(), sMapCpyNum, dwMMgrRunTime, dwPartRunTime[0], dwPartRunTime[1], dwPartRunTime[2], dwActMgrCellNum);

				if (pCLongTimeCha)
				{
					LG("map_time", "\t\"%s\" Check = %u, ActCache = %u, Resume = %u, Player = %u, AI = %u, Area = %u, Die = %u, Mission = %u, Team = %u, SkillState = %u, Move = %u, Fight = %u, DB = %u, Ping = %u\n",
						pCLongTimeCha->GetLogName(),
						pCLongTimeCha->m_dwCellRunTime[0], pCLongTimeCha->m_dwCellRunTime[1],
						pCLongTimeCha->m_dwCellRunTime[2], pCLongTimeCha->m_dwCellRunTime[3],
						pCLongTimeCha->m_dwCellRunTime[4], pCLongTimeCha->m_dwCellRunTime[5],
						pCLongTimeCha->m_dwCellRunTime[6], pCLongTimeCha->m_dwCellRunTime[7],
						pCLongTimeCha->m_dwCellRunTime[8], pCLongTimeCha->m_dwCellRunTime[9],
						pCLongTimeCha->m_dwCellRunTime[10], pCLongTimeCha->m_dwCellRunTime[11],
						pCLongTimeCha->m_dwCellRunTime[12], pCLongTimeCha->m_dwCellRunTime[13]);
				}
			}
		}
	}
T_E}

void CGameApp::GameItemRun(DWORD dwCurTime)
{
	if (!m_CTimerItem.IsOK(dwCurTime))
		return;

	m_pCEntSpace->BeginGetItem();
	CItem	*pCCur;
	while (pCCur = m_pCEntSpace->GetNextItem())
		pCCur->Run(dwCurTime);
}

void CGameApp::MapMgrRun(DWORD dwCurTime)
{
	CMapRes	*pCMap;

	for (short i = 0; i < m_mapnum; i++)
	{
		pCMap = m_MapList[i];
		if (!pCMap->IsValid()) continue;

		pCMap->Run(dwCurTime);
	}
}

void CGameApp::SetEntityEnableLog(bool bValid)
{T_B
	SubMap			*pCSubMap;
	//CEyeshotCell	*pUnit;
	Entity			*pCEnt;

	if (g_bLogEntity == bValid)
		return;

	g_bLogEntity = bValid;
	short	sMapCpyNum;
	for (short i = 0; i < m_mapnum; i++)
	{
		if (!m_MapList[i]->IsValid()) continue;
		sMapCpyNum = m_MapList[i]->GetCopyNum();
		for (short j = 0; j < sMapCpyNum; j++)
		{
			pCSubMap = m_MapList[i]->GetCopy(j);
			if (!pCSubMap)
				continue;

			for (short m = 0; m < pCSubMap->GetEyeshotCellLin(); m++)
			{
				for (short n = 0; n < pCSubMap->GetEyeshotCellCol(); n++)
				{
					pCEnt = pCSubMap->m_pCEyeshotCell[m][n].m_pCChaL;
					while (pCEnt)
					{
						pCEnt->m_CLog.SetEnable(bValid);
						pCEnt = pCEnt->m_pCEyeshotCellNext;
					}
					pCEnt = pCSubMap->m_pCEyeshotCell[m][n].m_pCItemL;
					while (pCEnt)
					{
						pCEnt->m_CLog.SetEnable(bValid);
						pCEnt = pCEnt->m_pCEyeshotCellNext;
					}
				}
			}
		}
	}
T_E}

// ����һ���µ����
CPlayer* CGameApp::GetNewPlayer()
{T_B
	return m_pCPlySpace->GetNewPly();
T_E}

// ������Ҿ��ȡָ��
CPlayer* CGameApp::GetPlayer(long lHandle)
{T_B
	return m_pCPlySpace->GetPly(lHandle);
T_E}

CPlayer* CGameApp::IsValidPlayer(long lID, long lHandle)
{T_B
	CPlayer	*pCPly = m_pCPlySpace->GetPly(lHandle);
	if (!pCPly)
		return 0;
	if (pCPly->GetID() != lID)
		return 0;

	return pCPly;
T_E}

// ����������ݽṹ����ȡ���ݿ�
// chType��0����ɫ���ߡ�1����ͼ�л�
CPlayer* CGameApp::CreateGamePlayer(const char szPassword[], uLong ulChaDBId, uLong ulWorldId, const char *cszMapName, char chType)
{T_B
	CPlayer	*pCOldPly = GetPlayerByDBID(ulChaDBId);
	if (pCOldPly)
	{
		//LG("�ظ���ɫ", "��ɫ %s[%u] ����ʱ�����ָý�ɫ�Ѿ����ڣ�\n", pCOldPly->GetMainCha()->GetName(), ulChaDBId);
		LG("repeat character", "when character %s[%u] enter,find it has exist!\n", pCOldPly->GetMainCha()->GetName(), ulChaDBId);
		pCOldPly->GetCtrlCha()->BreakAction();
		pCOldPly->MisLogout();
		pCOldPly->MisGooutMap();

		ReleaseGamePlayer( pCOldPly );
		//return NULL;
	}

    CPlayer  *l_player  = GetNewPlayer();
    CCharacter *pCMainCha = GetNewCharacter();
    if (!l_player || !pCMainCha)
	{
		if(l_player)
			l_player->Free();

		if(pCMainCha)
			pCMainCha->Free();

        // ȱ��character��free
       // LG("enter_map", "��������ҽ�ɫ��ID = %u��ʱ�������ڴ�ʧ�� \n", ulChaDBId);
		 LG("enter_map", "when create new player character(ID = %u),memory assign failed \n", ulChaDBId);
        return NULL;
	}

	l_player->SetPassword( szPassword );
	l_player->SetID(ulWorldId);
    pCMainCha->SetPlayer(l_player);
    pCMainCha->SetID(ulWorldId);

    l_player->SetMisChar( *pCMainCha );
    l_player->SetMainCha(pCMainCha);
	l_player->SetCtrlCha(pCMainCha);

    l_player->SetDBChaId(ulChaDBId);
	l_player->SetMaskMapName(cszMapName);

    LG("enter_map", "cha_id = %d, begin read data from database: \n", ulChaDBId);

	if (!game_db.ReadPlayer(l_player, ulChaDBId)) // �������쳣���ж���Gate������
    {
       // LG("enter_map", "cha_id = %d, ��ȡ��ɫ����ʱ�����ִ��󣬵���GameServer���GateServer�Ͽ�����\n", ulChaDBId);
		LG("enter_map", "cha_id = %d,when get character data,appear error,lead to GameServer with GateServer disconnection\n", ulChaDBId);
        l_player->Free();
        return NULL;
    }

	// ������ҽ�ɫЯ���Ĵ�ֻ
	if(!g_CharBoat.LoadBoat( *l_player->GetMainCha(), chType ))
    {
        LG("boat_excp", "Character %s[%d] Load Boat Failt.\n", l_player->GetMainCha()->GetName(), ulChaDBId);
        l_player->Free();
        return NULL;
    }

    AddPlayerIdx(ulChaDBId, l_player);
    g_pGameApp->m_dwPlayerCnt++;

	//LG("Play_Count", "%d,%d,%s,Entery\n", ::GetCurrentThreadId(), g_pGameApp->m_dwPlayerCnt, l_player->GetActName());

    //LG("enter_map", "cha type = %d\n", pCMainCha->m_SChaPart.sTypeID); 
    //for(int i = 0; i < enumEQUIP_NUM; i++)
    //LG("enter_map", "look [%d] = %d\n", i, pCMainCha->m_SChaPart.SLink[i].sID);

    LG("enter_map", "cha_id = %d (%s) end entermap\n", ulChaDBId, pCMainCha->GetName());
    //LG("enter_map", "cha_id = %d, logname = [%s] end enter\n", ulChaDBId, pCMainCha->m_CLog.GetLogName());

    return l_player;
T_E}

// ����������ϣ��黹�ڴ�
void CGameApp::ReleaseGamePlayer(CPlayer* pPlayer)
{T_B
    // ����д���ݿ⣬�ͷŽ�ɫ
    if(pPlayer && pPlayer->IsValid())
	{
		CCharacter	*pCCha = pPlayer->GetCtrlCha();
		if (!pCCha)
		{
			LG("error", "ReleaseGamePlayer1\n");
			return;
		}
		bool	bIsDie = !pCCha->IsLiveing();
		bool	bSavePos = false;
		SubMap	*pSrcMap = pCCha->GetSubMap();
		Point	SSrcPos;

		if (pSrcMap)
			pSrcMap->BeforePlyOutMap(pCCha);
		if (pSrcMap)
			bSavePos = pCCha->GetSubMap()->CanSavePos();
		if (bIsDie || !bSavePos) // ��ɫ�Ѿ�����
		{
			if (bIsDie)
				g_CParser.DoString("Relive", enumSCRIPT_RETURN_NONE, 0, enumSCRIPT_PARAM_LIGHTUSERDATA, 1, pCCha, DOSTRING_PARAM_END);

			SSrcPos = pCCha->GetPos();
			if (pCCha->IsBoat()) // ��ֻ
			{
				pCCha->SetToMainCha(bIsDie);

				pPlayer->GetMainCha()->ResetBirthInfo();
			}
			else
			{
				pCCha->ResetBirthInfo();
			}
		}

		LG("enter_map", "cha_id = %d, begin goout\n", pPlayer->GetDBChaId());

		//LG("Thread", "ReleaseGamePlayer %d\n", ::GetCurrentThreadId());

		if(game_db.SavePlayer(pPlayer, enumSAVE_TYPE_OFFLINE)==false)
		{
			//LG("enter_map", "SavePlayer[%s]ʧ��", pPlayer->GetMainCha()->GetName());
			LG("enter_map", "SavePlayer[%s] failed\n", pPlayer->GetMainCha()->GetName());
		}

		if (bIsDie || !bSavePos) // ��ɫ�Ѿ�����
		{
			game_db.SavePlayerPos(pPlayer);
			pCCha->SetSubMap(pSrcMap);
			pCCha->SetPos(SSrcPos);
		}

		DelPlayerIdx(pPlayer->GetDBChaId());
		g_pGameApp->m_dwPlayerCnt--;

		//LG("Play_Count", "%d,%d,%s,Leave\n", ::GetCurrentThreadId(), g_pGameApp->m_dwPlayerCnt, pPlayer->GetActName());

		pPlayer->Free();

		//////////////////////////////////////////////////////////////////////////
		// ɾ��gate server��Ӧ��ά����Ϣ
		pPlayer->OnLogoff();
		DELPLAYER(pPlayer);
		//////////////////////////////////////////////////////////////////////////

		LG("enter_map", "cha_id = %d, goout\n", pPlayer->GetDBChaId());
	}
	else
	{
		LG("error", "ReleaseGamePlayer2\n");
	}
T_E}

void CGameApp::GoOutGame(CPlayer* pPlayer, bool bOffLine)
{T_B
    if(pPlayer && pPlayer->IsValid())
	{
		if (pPlayer->GetStallData()) { return; }
		
		//�޸�������
		if(pPlayer->GetMainCha()->IsVolunteer())
		{
			pPlayer->GetMainCha()->Cmd_DelVolunteer();
		}

		pPlayer->GetCtrlCha()->BreakAction();
		if (bOffLine) // ���ߣ��������л���ͼ��
			pPlayer->MisLogout();
		pPlayer->MisGooutMap();

		ReleaseGamePlayer(pPlayer);
	}
	else
	{
		LG("error", "GoOutGame\n");
	}
T_E}

void CGameApp::NoticePlayerLogin(CPlayer *pCPlayer)
{
	if (!pCPlayer || !pCPlayer->GetCtrlCha())
		return;

	WPACKET WtPk  = GETWPACKET();
	WRITE_CMD(WtPk, CMD_MM_LOGIN);
	WRITE_LONG(WtPk, 0);
	WRITE_STRING(WtPk, pCPlayer->GetCtrlCha()->GetName());

	BEGINGETGATE();
	GateServer	*pGateServer;
	while (pGateServer = GETNEXTGATE())
	{
		WRITE_LONG(WtPk, 0);
		WRITE_LONG(WtPk, 0);
		WRITE_SHORT(WtPk, 1);
		pGateServer->SendData(WtPk);
		break;
	}
}

void CGameApp::AfterPlayerLogin(const char *cszPlyName)
{
	if (!cszPlyName)
		return;
	g_CDMapEntry.AfterPlayerLogin(cszPlyName);
}

// ����һ���µĽ�ɫ
CCharacter* CGameApp::GetNewCharacter()
{T_B
	return m_pCEntSpace->GetNewCha();
T_E}

// ����һ���µĵ���
CItem* CGameApp::GetNewItem()
{T_B
	return m_pCEntSpace->GetNewItem();
T_E}

// ����һ���µ�NPC
mission::CTalkNpc* CGameApp::GetNewTNpc()
{T_B
	return m_pCEntSpace->GetNewTNpc();
T_E}

Entity* CGameApp::GetEntity(long lHandle)
{T_B
	return m_pCEntSpace->GetEntity(lHandle);
T_E}

// ��ɫ����Чʵ��
Entity* CGameApp::IsValidEntity(unsigned long ulID, long lHandle)
{T_B
	Entity	*pEnti = g_pGameApp->GetEntity(lHandle);
	if (!pEnti)
		return 0;
	if (!pEnti->IsValid()
		|| ulID != pEnti->GetID())
		return 0;

	return pEnti;
T_E}

// ��ɫ����Чʵ�壬�����ڵ�ͼ��������״̬
Entity* CGameApp::IsLiveingEntity(unsigned long ulID, long lHandle)
{T_B
	Entity	*pEnti = IsValidEntity(ulID, lHandle);
	if (!pEnti)
		return 0;
	if (pEnti->IsCharacter() != g_pCSystemCha)
		if (!pEnti->IsLiveing() || !pEnti->GetSubMap())
			return 0;

	return pEnti;
T_E}

// ��ɫ����Чʵ�壬�����ڵ�ͼ
Entity* CGameApp::IsMapEntity(unsigned long ulID, long lHandle)
{T_B
	Entity	*pEnti = IsValidEntity(ulID, lHandle);
	if (!pEnti)
		return 0;
	if (pEnti->IsCharacter() != g_pCSystemCha)
		if (!pEnti->GetSubMap())
			return 0;

	return pEnti;
T_E}

// ��ɫ����Чʵ�壬������
Entity* CGameApp::IsLifeEntity(unsigned long ulID, long lHandle)
{T_B
	Entity	*pEnti = IsValidEntity(ulID, lHandle);
	if (!pEnti)
		return 0;
	if (pEnti->IsCharacter() != g_pCSystemCha)
		if (!pEnti->IsLiveing())
			return 0;

	return pEnti;
T_E}

// ��ʼ�����е�ͼ
BOOL CGameApp::InitMap()
{T_B
	//LG("init", "��ʼ����ͼ�б�...\n");
	LG("init", "initialization map list...\n");
	
	mission::g_WorldEudemon.Load( "Eudemon", g_Config.m_szEqument, -1 );

	//��ʼ��Map��Ϣ
	m_mapnum = g_Config.m_nMapCnt;
	if(m_mapnum < 1)
	{
		//LG("init", "���õ�ͼ����Ϊ0,û�е�ͼ����ʼ��, �˳�!\n");
		LG("init", "collocate map number 0,no map was initialization, exit!\n");
		return FALSE;
	}

	m_strMapNameList = "";

	//��ʼ����ͼ���ݽṹ
	memset(m_MapList, 0, sizeof(CMapRes*) * MAX_MAP);
	for(short i=0; i< m_mapnum; i++)
	{
		m_MapList[i] = new CMapRes();
		m_MapList[i]->SetName(g_Config.m_szMapList[i]);

		//strcpy(m_MapList[i]->m_szObstacleFile, m_MapList[i]->GetName());		//�ϰ��ļ���
		strncpy_s( m_MapList[i]->m_szObstacleFile, sizeof(m_MapList[i]->m_szObstacleFile), m_MapList[i]->GetName(), _TRUNCATE );
		//strcat(m_MapList[i]->m_szObstacleFile, "\\"); 
		strncat_s( m_MapList[i]->m_szObstacleFile,sizeof(m_MapList[i]->m_szObstacleFile), "\\",_TRUNCATE);
		//strcat(m_MapList[i]->m_szObstacleFile, m_MapList[i]->GetName());
		strncat_s( m_MapList[i]->m_szObstacleFile,sizeof(m_MapList[i]->m_szObstacleFile), m_MapList[i]->GetName(),_TRUNCATE);
		//strcat(m_MapList[i]->m_szObstacleFile, ".blk");
		strncat_s( m_MapList[i]->m_szObstacleFile,sizeof(m_MapList[i]->m_szObstacleFile), ".blk",_TRUNCATE);
		//strcpy(m_MapList[i]->m_szSectionFile, m_MapList[i]->GetName());			//���������ļ���
		strncpy_s( m_MapList[i]->m_szSectionFile, sizeof(m_MapList[i]->m_szSectionFile), m_MapList[i]->GetName(), _TRUNCATE );
		//strcat(m_MapList[i]->m_szSectionFile, "\\");
		strncat_s( m_MapList[i]->m_szSectionFile,sizeof(m_MapList[i]->m_szSectionFile), "\\",_TRUNCATE);
		//strcat(m_MapList[i]->m_szSectionFile, m_MapList[i]->GetName());
		strncat_s( m_MapList[i]->m_szSectionFile,sizeof(m_MapList[i]->m_szSectionFile), m_MapList[i]->GetName(),_TRUNCATE);
		//strcpy(m_MapList[i]->m_szMonsterSpawnFile, m_MapList[i]->GetName());	//����ˢ���ļ���
		strncpy_s( m_MapList[i]->m_szMonsterSpawnFile, sizeof(m_MapList[i]->m_szMonsterSpawnFile), m_MapList[i]->GetName(), _TRUNCATE );
		//strcat(m_MapList[i]->m_szMonsterSpawnFile, "\\");
		strncat_s( m_MapList[i]->m_szMonsterSpawnFile,sizeof(m_MapList[i]->m_szMonsterSpawnFile), "\\",_TRUNCATE);
		//strcat(m_MapList[i]->m_szMonsterSpawnFile, m_MapList[i]->GetName());
		strncat_s( m_MapList[i]->m_szMonsterSpawnFile,sizeof(m_MapList[i]->m_szMonsterSpawnFile), m_MapList[i]->GetName(),_TRUNCATE);
		//strcat(m_MapList[i]->m_szMonsterSpawnFile, "ChaSpn");
		strncat_s( m_MapList[i]->m_szMonsterSpawnFile,sizeof(m_MapList[i]->m_szMonsterSpawnFile), "ChaSpn",_TRUNCATE);
		//strcpy(m_MapList[i]->m_szNpcSpawnFile, m_MapList[i]->GetName());		//npc�����ļ���
		strncpy_s( m_MapList[i]->m_szNpcSpawnFile, sizeof(m_MapList[i]->m_szNpcSpawnFile), m_MapList[i]->GetName(), _TRUNCATE );
		//strcat(m_MapList[i]->m_szNpcSpawnFile, "\\");
		strncat_s( m_MapList[i]->m_szNpcSpawnFile,sizeof(m_MapList[i]->m_szNpcSpawnFile), "\\",_TRUNCATE);
		//strcat(m_MapList[i]->m_szNpcSpawnFile, m_MapList[i]->GetName());
		strncat_s( m_MapList[i]->m_szNpcSpawnFile,sizeof(m_MapList[i]->m_szNpcSpawnFile), m_MapList[i]->GetName(),_TRUNCATE);
		//strcat(m_MapList[i]->m_szNpcSpawnFile, "NPC");
		strncat_s( m_MapList[i]->m_szNpcSpawnFile,sizeof(m_MapList[i]->m_szNpcSpawnFile), "NPC",_TRUNCATE);
		//strcpy(m_MapList[i]->m_szMapSwitchFile, m_MapList[i]->GetName());		//��ͼ�л��ļ���
		strncpy_s( m_MapList[i]->m_szMapSwitchFile, sizeof(m_MapList[i]->m_szMapSwitchFile), m_MapList[i]->GetName(), _TRUNCATE );
		//strcat(m_MapList[i]->m_szMapSwitchFile, "\\");
		strncat_s( m_MapList[i]->m_szMapSwitchFile, sizeof(m_MapList[i]->m_szMapSwitchFile), "\\", _TRUNCATE );
		//strcat(m_MapList[i]->m_szMapSwitchFile, m_MapList[i]->GetName());
		strncat_s( m_MapList[i]->m_szMapSwitchFile,sizeof(m_MapList[i]->m_szMapSwitchFile), m_MapList[i]->GetName(),_TRUNCATE);
		//strcat(m_MapList[i]->m_szMapSwitchFile, "SwhMap");
		strncat_s( m_MapList[i]->m_szMapSwitchFile,sizeof(m_MapList[i]->m_szMapSwitchFile), "SwhMap",_TRUNCATE);
		//strcpy(m_MapList[i]->m_szMonsterCofFile, m_MapList[i]->GetName());		//�����Ĺ���ˢ���ļ���
		strncpy_s( m_MapList[i]->m_szMonsterCofFile, sizeof(m_MapList[i]->m_szMonsterCofFile), m_MapList[i]->GetName(), _TRUNCATE );
		//strcat(m_MapList[i]->m_szMonsterCofFile, "\\");
		strncat_s( m_MapList[i]->m_szMonsterCofFile,sizeof(m_MapList[i]->m_szMonsterCofFile), "\\",_TRUNCATE);
		//strcat(m_MapList[i]->m_szMonsterCofFile, m_MapList[i]->GetName());
		strncat_s( m_MapList[i]->m_szMonsterCofFile,sizeof(m_MapList[i]->m_szMonsterCofFile), m_MapList[i]->GetName(),_TRUNCATE);
		//strcat(m_MapList[i]->m_szMonsterCofFile, "monster_conf.lua");
		strncat_s( m_MapList[i]->m_szMonsterCofFile,sizeof(m_MapList[i]->m_szMonsterCofFile), "monster_conf.lua",_TRUNCATE);
		//strcpy(m_MapList[i]->m_szCtrlFile, m_MapList[i]->GetName());		//��ͼ�����ļ���
		strncpy_s( m_MapList[i]->m_szCtrlFile, sizeof(m_MapList[i]->m_szCtrlFile), m_MapList[i]->GetName(), _TRUNCATE );
		//strcat(m_MapList[i]->m_szCtrlFile, "\\");
		strncat_s( m_MapList[i]->m_szCtrlFile,sizeof(m_MapList[i]->m_szCtrlFile), "\\",_TRUNCATE);
		//strcat(m_MapList[i]->m_szCtrlFile, "ctrl.lua");
		strncat_s( m_MapList[i]->m_szCtrlFile,sizeof(m_MapList[i]->m_szCtrlFile), "ctrl.lua",_TRUNCATE);
		//strcpy(m_MapList[i]->m_szEntryFile, m_MapList[i]->GetName());		//��ͼ����ļ���
		strncpy_s( m_MapList[i]->m_szEntryFile, sizeof(m_MapList[i]->m_szEntryFile), m_MapList[i]->GetName(), _TRUNCATE );
		//strcat(m_MapList[i]->m_szEntryFile, "\\");
		strncat_s( m_MapList[i]->m_szEntryFile,sizeof(m_MapList[i]->m_szEntryFile), "\\",_TRUNCATE);
		//strcat(m_MapList[i]->m_szEntryFile, "entry.lua");
		strncat_s( m_MapList[i]->m_szEntryFile,sizeof(m_MapList[i]->m_szEntryFile), "entry.lua",_TRUNCATE);

		if(m_MapList[i]->Init()==FALSE)
		{
			//LG("init", "��ͼ[%s]��ʼ��ʧ��!\n", m_MapList[i]->GetName());
			LG("init", "map [%s] initialization failed!\n", m_MapList[i]->GetName());
			g_Config.m_btMapOK[i] = 0;
			continue;
		}
		else
		{
			g_Config.m_btMapOK[i] = 1; // ��ʼ���ɹ����
		}
		m_strMapNameList += m_MapList[i]->GetName();
		m_strMapNameList += ";";
		//LG("init", "��ͼ[%s] ok!\n", m_MapList[i]->GetName());
		LG("init", "map [%s] ok!\n", m_MapList[i]->GetName());
	}
	
	
	lua_dofile(g_pLuaState, GetResPath("script/help/AddHelpNPC.lua"));
	
	lua_dofile(g_pLuaState, GetResPath("script/monster/mlist.lua"));
	
	return TRUE;

T_E}

BOOL CGameApp::SummonNpc( BYTE byMapID, USHORT sAreaID, const char szNpc[], USHORT sTime )
{T_B
	CMapRes* pMap = g_MapID.GetMap( byMapID );
	if( pMap )
	{
		return pMap->SummonNpc( sAreaID, szNpc, sTime );
	}

	return FALSE;
T_E}

// ͨ�����Ʋ����ӵ�ͼ
CMapRes *CGameApp::FindMapByName(const char *mapname, bool bIncUnRun)
{T_B
	if (!mapname)
		return 0;

	short i=0;
	for(i=0;i < m_mapnum;i++)
	{
		CMapRes *pMap = m_MapList[i];
		if(!pMap) continue; // �п��ܻ�û�г�ʼ��
	    //�޸�С��ͼ��һ��ʱ���޷������BUG
		/* if (!bIncUnRun)
			if (!(pMap->IsOpen())) continue;
		*/
		if(!strcmp(pMap->GetName(), mapname))
		{
			break;
		}
	}
	if(i < m_mapnum)
	{
		return m_MapList[i];
	}
	else
	{
		return 0;
	}
T_E}

void CGameApp::LoadAllTable()
{T_B
	LoadCharacterInfo();
	LoadSkillInfo();
	LoadItemInfo();
T_E}

void CGameApp::LoadCharacterInfo()
{T_B
	m_CChaRecordSet->Release();
	LoadTable(m_CChaRecordSet, szChaInfoName);
T_E}

void CGameApp::LoadSkillInfo()
{T_B
	m_CSkillRecordSet->Release();
	LoadTable(m_CSkillRecordSet, szSkillInfoName);
T_E}

void CGameApp::LoadItemInfo()
{T_B
	m_CItemRecordSet->Release();
	LoadTable(m_CItemRecordSet, szItemInfoName);
T_E}

BOOL CGameApp::ReloadNpcInfo( CCharacter& character )
{T_B
	g_pGameApp->BeginGetTNpc();
	mission::CTalkNpc* pTalkNpc = NULL;
	while( (pTalkNpc = g_pGameApp->GetNextTNpc()) )
	{
		if( !pTalkNpc->InitScript( pTalkNpc->GetInitFunc(), pTalkNpc->GetName() ) )
		{
			//character.SystemNotice( "����װ��NPC[%s]��ʼ����Ϣ�ű�����[%s]ʧ��!", pTalkNpc->GetInitFunc(), pTalkNpc->GetName() );
			//character.SystemNotice( RES_STRING(GM_GAMEAPP_CPP_00001), pTalkNpc->GetInitFunc(), pTalkNpc->GetName() );
			char szData[128];
			CFormatParameter param(2);
			param.setString( 0, pTalkNpc->GetInitFunc() );
			param.setString( 1, pTalkNpc->GetName() );
			RES_FORMAT_STRING( GM_GAMEAPP_CPP_00001, param, szData );
			character.SystemNotice( szData );
		}
	}

	//mission::g_WorldEudemon.Load( "Eudemon", "�ػ���", -1 );
	mission::g_WorldEudemon.Load( "Eudemon", "Eudemon", -1 );
	return TRUE;
T_E}

mission::CNpc* CGameApp::FindNpc( const char szName[] )
{
	if( szName )
	{
		for(short i=0; i< m_mapnum; i++)
		{
			mission::CNpc* pNpc = m_MapList[i]->FindNpc( szName );
			if( pNpc ) return pNpc;
		}
	}
	return NULL;
}

void CGameApp::NotiGameReset(unsigned long ulLeftSec)
{T_B
	char	szNotiMsg[1024];

	//sprintf(szNotiMsg, "��Ϸ������\"%s\"���� %u ������������ڵ�ͼ\"%s\"�ϵ��������׼��", g_Config.m_szName, ulLeftSec, m_strMapNameList.c_str());
	//sprintf(szNotiMsg, RES_STRING(GM_GAMEAPP_CPP_00002), g_Config.m_szName, ulLeftSec, m_strMapNameList.c_str());
	//_snprintf_s(szNotiMsg,sizeof(szNotiMsg) ,_TRUNCATE,RES_STRING(GM_GAMEAPP_CPP_00002), g_Config.m_szName, ulLeftSec, m_strMapNameList.c_str());
	CFormatParameter param(3);
	param.setString( 0, g_Config.m_szName );
	param.setDouble( 1, ulLeftSec );
	param.setString( 2, m_strMapNameList.c_str() );
	RES_FORMAT_STRING( GM_GAMEAPP_CPP_00002, param, szNotiMsg );

	WPACKET wpk	= GETWPACKET();
	WRITE_CMD(wpk, CMD_MC_SYSINFO);
	WRITE_STRING(wpk, szNotiMsg);
	NotiPkToWorld(wpk);
T_E}

void CGameApp::BeginGetTNpc(void) 
{T_B
	m_pCEntSpace->BeginGetTNpc();
T_E}

mission::CTalkNpc*	CGameApp::GetNextTNpc(void)
{T_B
	return m_pCEntSpace->GetNextTNpc();
T_E}

void CGameApp::SaveAllPlayer(void)
{T_B
	BEGINGETGATE();
	CPlayer	*pCPlayer;
	GateServer	*pGateServer;
	LG("enter_map", "Begin SaveAllPlayer==============================================================\n");
	while (pGateServer = GETNEXTGATE())
	{
		if (!BEGINGETPLAYER(pGateServer))
			continue;
		int nCount = 0;
		while (pCPlayer = (CPlayer *)GETNEXTPLAYER(pGateServer))
		{
			if (++nCount > GETPLAYERCOUNT(pGateServer))
			{
				//LG("����������", "�����Ŀ:%u, %s\n", GETPLAYERCOUNT(pGateServer), "SaveAllPlayer");
				LG("player chain list error", "player number:%u, %s\n", GETPLAYERCOUNT(pGateServer), "SaveAllPlayer");
				break;
			}
			//LG("Thread", "SaveAllPlayer %d\n", ::GetCurrentThreadId());

			LG("enter_map", "SaveAllPlayer\n");

			game_db.SavePlayer(pCPlayer, enumSAVE_TYPE_OFFLINE);
			LG("enter_map", "\n");
		}
	}
	//game_db.GetMapMaskTable()->SaveAll(); // �˳�ʱ, һ���Դ���ʣ�µı�����ͼ����
	LG("enter_map", "End SaveAllPlayer################################################################\n");
T_E}

void CGameApp::DataStatistic(void)
{
	Long	lCabinHeapNum = m_CabinHeap.GetUsedNum();
	Long	lTradeDataHeapNum = m_TradeDataHeap.GetUsedNum();
	Long	lSkillTDataHeapNum = m_SkillTDataHeap.GetUsedNum();
	Long	lMapMgrUnitHeapNum = m_MapStateCellHeap.GetUsedNum();
	Long	lEntityListHeapNum = m_ChaListHeap.GetUsedNum();
	Long	lMgrNodeHeapNum = m_StateCellNodeHeap.GetUsedNum();

	if (lCabinHeapNum > m_lCabinHeapNum || lTradeDataHeapNum > m_lTradeDataHeapNum || lSkillTDataHeapNum > m_lSkillTDataHeapNum
		|| lMapMgrUnitHeapNum > m_lMapMgrUnitHeapNum || lEntityListHeapNum > m_lEntityListHeapNum || lMgrNodeHeapNum > m_lMgrNodeHeapNum)
	{
		if (m_lCabinHeapNum < lCabinHeapNum)
			m_lCabinHeapNum = lCabinHeapNum;
		if (m_lTradeDataHeapNum < lTradeDataHeapNum)
			m_lTradeDataHeapNum = lTradeDataHeapNum;
		if (m_lSkillTDataHeapNum < lSkillTDataHeapNum)
			m_lSkillTDataHeapNum = lSkillTDataHeapNum;
		if (m_lMapMgrUnitHeapNum < lMapMgrUnitHeapNum)
			m_lMapMgrUnitHeapNum = lMapMgrUnitHeapNum;
		if (m_lEntityListHeapNum < lEntityListHeapNum)
			m_lEntityListHeapNum = lEntityListHeapNum;
		if (m_lMgrNodeHeapNum < lMgrNodeHeapNum)
			m_lMgrNodeHeapNum = lMgrNodeHeapNum;

		LG("ջʹ��ͳ��", "Cabin=%4u,\tTrade=%8u,\tSkillTemp=%8u,\tMapMgrUnit=%8u,\tEntityList=%8u,\tMgrNode=%8u\n",
			m_lCabinHeapNum, m_lTradeDataHeapNum, m_lSkillTDataHeapNum, m_lMapMgrUnitHeapNum, m_lEntityListHeapNum, m_lMgrNodeHeapNum);
	}
}

// ֪ͨ��������GameServer�ϵ����
void CGameApp::WorldNotice(const char *szString)
{
	if (!szString)
		return;

	LG("WorldNotice", "WorldNotice: len = %d\n", strlen(szString));
	LG("WorldNotice", "WorldNotice: contend = %s\n", szString);

	WPACKET WtPk  = GETWPACKET();
	WRITE_CMD(WtPk, CMD_MM_NOTICE);
	WRITE_LONG(WtPk, 0);
	WRITE_STRING(WtPk, szString);

	BEGINGETGATE();
	GateServer	*pGateServer;
	while (pGateServer = GETNEXTGATE())
	{
		WRITE_LONG(WtPk, 0);
		WRITE_LONG(WtPk, 0);
		WRITE_SHORT(WtPk, 1);
		pGateServer->SendData(WtPk);
		break;
	}
}

//add by sunny.sun20080804
void CGameApp::ScrollNotice(const char * szString,int SetNum)
{
	if(!szString)
		return;
	WPACKET WtPk  = GETWPACKET();
	WRITE_CMD(WtPk, CMD_MP_GM1SAY1);
	WRITE_STRING(WtPk, szString);
	WRITE_LONG(WtPk, SetNum);
	SENDTOGROUP(WtPk);
}
//add by sunny.sun20080821
void CGameApp::GMNotice( const char * szString )
{
	if(!szString)
		return;
	WPACKET WtPk  = GETWPACKET();
	WRITE_CMD(WtPk, CMD_MP_GM1SAY);
	WRITE_STRING(WtPk, szString);
	SENDTOGROUP(WtPk);
}

// ֪ͨ��GameServer�ϵ����
void CGameApp::LocalNotice(const char *szString)
{
	if (!szString)
		return;

	WPACKET wpk	= GETWPACKET();
	WRITE_CMD(wpk, CMD_MC_SYSINFO);
	WRITE_STRING(wpk, szString);
	SENDTOWORLD(wpk);
}

// ָ֪ͨ����ң����szChaName == ""����ͨ�汾��GameServer���������
void CGameApp::ChaNotice(const char *szNotiString, const char *szChaName)
{
	if (!szNotiString || !szChaName)
		return;

	WPACKET WtPk  = GETWPACKET();
	WRITE_CMD(WtPk, CMD_MM_CHA_NOTICE);
	WRITE_LONG(WtPk, 0);
	WRITE_STRING(WtPk, szNotiString);
	WRITE_STRING(WtPk, szChaName);

	BEGINGETGATE();
	GateServer	*pGateServer = NULL;
	while (pGateServer = GETNEXTGATE())
	{
		WRITE_LONG(WtPk, 0);
		WRITE_LONG(WtPk, 0);
		WRITE_SHORT(WtPk, 1);
		pGateServer->SendData(WtPk);
		break;
	}
}

int CGameApp::LogPlayer()
{
	for(map<DWORD, CPlayer*>::iterator it = _PlayerIdx.begin(); it != _PlayerIdx.end(); it++)
	{
		//DWORD       m_dwLoginID;    //  Account DB ID
		//DWORD		m_dwDBActId;	// ���ݿ��˻�ID
		//dbc::Char	m_chGMLev;		// GM�ȼ�
		//long		m_lMapMaskDBID;
		//dbc::Char	m_chActName[ACT_NAME_LEN];	// �˺���
		
		//it->second->
		if(it->second)
		{
			LG("LogPlayers", "Key = %d, DB_ID = %d, Name = %s, Valid = %d \n", it->first, it->second->GetDBActId(), it->second->GetActName(),
				 it->second->IsValid() );
		}
		else
		{
			LG("LogPlayers", "Key = %d \n", it->first);
		}
	}

	return (int)_PlayerIdx.size();
}

#ifdef SHUI_JING
void CGameApp::ResetHangState()
{T_B
	vector< vector< string > > dataHangTrade;
	if( !game_db.ResetCrystalState( dataHangTrade ) )
		return;
	for( int i = 0; i< (int)dataHangTrade.size(); i++ )
	{
		DWORD curr_cha_id = atoi(dataHangTrade[i][0].c_str());
		string curr_cha_name = dataHangTrade[i][1].c_str();
		string curr_act_name = dataHangTrade[i][2].c_str();
		DWORD curr_cha_type = atoi(dataHangTrade[i][3].c_str());
		DWORD curr_cha_num = atoi(dataHangTrade[i][4].c_str());
		DWORD curr_cha_price = atoi(dataHangTrade[i][5].c_str());

		LG("Crystal_Ret","Begin Reset Character Name[%s],player[%s],cha_id[%ld],tradetype[%d],hangnumber[%ld],hangprice[%ld].....\n",
			curr_cha_name.c_str(), curr_cha_name.c_str(), curr_cha_id, curr_cha_type, curr_cha_num, curr_cha_price);
		if( curr_cha_type == CrystalTradeType::enumbuy )
		{
			if( game_db.IsHasBuyorSale( curr_cha_id, CrystalTradeType::enumbuy ) )
			{
				if( game_db.ChangeStateByTemp( curr_cha_id, CrystalTradeType::enumbuy, CrystalTradeType::enumstop ) )
				{
					CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(curr_cha_id);
					if( pPlayer )
					{
						CCharacter* pCha = pPlayer->GetMainCha();
						if( pCha )
						{
							//��ȡ����,����Ǯ
							DWORD flatMoney = pCha->GetFlatMoney();
							pCha->SetFlatMoney( flatMoney + curr_cha_num*curr_cha_price );
							pCha->SynGetCrystalState();//֪ͨ���
							pCha->Cmd_CrystalBuyAndSaleList();
							LG("Crystal_Ret","Player online and Reset buy trade to start Successful!\n");
						}
					}
					else
					{
						game_db.SetdwFlatMoney( curr_cha_id, curr_cha_price * curr_cha_num );
						LG("Crystal_Ret","Player Not online and Reset buy trade to start!\n");
					}
				}
			}
		}
		else if( curr_cha_type == CrystalTradeType::enumsale )
		{
			if( game_db.IsHasBuyorSale( curr_cha_id, CrystalTradeType::enumsale ) )
			{
				if( game_db.ChangeStateByTemp( curr_cha_id, CrystalTradeType::enumsale, CrystalTradeType::enumstop ) )
				{
					//������,�����ˮ����Ǯ
					CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(curr_cha_id);
					if( pPlayer )
					{
						CCharacter* pCha = pPlayer->GetMainCha();
						if( pCha )
						{
							if( !g_StoreSystem.RequestCrystalAdd( pCha, pCha->GetPlayer()->GetActName(), pCha->GetID(), curr_cha_num, curr_cha_price, curr_cha_num, curr_cha_num, 2, 0, 0))
							{
								LG("Crystal_Ret","Player online and Reset Sale trade to failed and change state again!\n");
								game_db.ChangeStateByTemp( curr_cha_id, CrystalTradeType::enumsale, CrystalTradeType::enumhang ) ;
								continue;
							}
							LG("Crystal_Ret","Player online and Reset Sale trade to start!\n");
						}
					}
					else
					{
						if( !g_StoreSystem.RequestCrystalAdd(curr_act_name, curr_cha_id, curr_cha_name, curr_cha_num, curr_cha_price, curr_cha_num, curr_cha_num, 0 ))
						{
							LG("Crystal_Ret","Player Not online and Reset Sale trade to failed and change state again!\n");
							game_db.ChangeStateByTemp( curr_cha_id, CrystalTradeType::enumsale, CrystalTradeType::enumhang ) ;
							continue;
						}
						LG("Crystal_Ret","Player Not online and Reset Sale trade to start!\n");
					}
				}
			}
		
		}
	}

T_E}
#endif