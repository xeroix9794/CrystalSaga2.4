//=============================================================================
// FileName: Player.cpp
// Creater: ZhangXuedong
// Date: 2004.10.19
// Comment: CPlayer class
//=============================================================================

#include "Player.h"
#include "TryUtil.h"
#include "GameApp.h"
#include "GameDB.h"
#include "CharStall.h"
#include "FightAble.h"
_DBC_USING;

CPlayer::CPlayer():
m_sGarnerWiner(0)
{T_B
    Init(NULL, 0);
	m_chGMLev = 0;
	m_dwValidFlag = PLAYER_INVALID_FLAG;

	m_CMapMask.AddMap("garner", defMAP_GARNER_WIDTH, defMAP_GARNER_HEIGHT);
	m_CMapMask.AddMap("magicsea", defMAP_MAGICSEA_WIDTH, defMAP_MAGICSEA_HEIGHT);
	m_CMapMask.AddMap("darkblue", defMAP_DARKBLUE_WIDTH, defMAP_DARKBLUE_HEIGHT);

	// Add by lark.li 20080812 begin
	m_CMapMask.AddMap("winterland", defMAP_DARKBLUE_WIDTH, defMAP_DARKBLUE_HEIGHT);
	// End

	//m_CMapMask.AddMap("eastgoaf", defMAP_EASTGOAF_WIDTH, defMAP_EASTGOAF_HEIGHT);
	//m_CMapMask.AddMap("lonetower", defMAP_LONETOWER_WIDTH, defMAP_LONETOWER_HEIGHT);
	m_lLightSize = g_lDeftMMaskLight;

	//��ʼ��Ħ���ʹ�������
	m_lMoBean = 0;
	m_lRplMoney = 0;
	m_lVipID = 0;
	m_strLifeSkillinfo = "";
	// Modify by lark.li 20080317
	memset(this->m_szPassword, 0 , sizeof(this->m_szPassword));
	//End
T_E}

void CPlayer::Initially()
{T_B
	bIsValid = true;
    Init(NULL, 0);
    _nTeamMemberCnt = 0;
	m_chGMLev = 0;	
	_dwTeamLeaderID = 0;

	m_szPassword[0] = 0;
	m_dwDBActId = 0;
	m_pCtrlCha = 0;
	m_pMainCha = NULL;
	m_dwLaunchID = -1;
	m_sBerthID = -1;
	m_sxPos = 0;
	m_syPos = 0;
	m_sDir = 0;	
	m_byNumBoat = 0;
	memset( m_Boat, 0, sizeof(CCharacter*)*MAX_CHAR_BOAT );

	m_ulLoginCha[0] = enumLOGIN_CHA_MAIN;
	SetMapMaskDBID(0);
	ResetMapMaskChange();
	m_chBankNum = 0;
	m_pCBankNpc = 0;
	for (char i = 0; i < MAX_BANK_NUM; i++)
	{
		m_lBankDBID[i] = 0;
		m_CBank[i].Init();
	}
	m_szMaskMapName[0] = '\0';

	m_GuildState	 = emGuildInitVal;
	m_bIsGuildLeader = false;
	memset( m_szGuildName, 0, defGUILD_NAME_LEN );
	memset( m_szGuildMotto, 0, defGUILD_MOTTO_LEN );

	CCharMission::Initially();

	m_pMakingBoat = NULL;
	m_pStallData = NULL;

	SetChallengeType(enumFIGHT_NONE);
	SetInRepair(false);
	SetInForge(false);
	m_pCRepairman = NULL;
	m_pCForgeman = NULL;
T_E}

void CPlayer::Finally()
{T_B
	GetNextPlayer() = NULL;

	if(m_pMainCha)
	{
		m_pMainCha->TradeClear( *this );
		m_pMainCha->Free();
	}
	CCharMission::Finally();

	for( int i = 0; i < m_byNumBoat; i++ )
	{
		if( m_Boat[i] )
		{
			if (m_Boat[i]->GetShip())
				m_Boat[i]->GetShip()->Free();
			m_Boat[i]->SetShip(0);
			m_Boat[i]->Free();
			m_Boat[i] = 0;
		}
	}

	m_pCtrlCha = 0;
	m_pMainCha = NULL;

	// �����еĴ�ֻ
	if( m_pMakingBoat )
	{
		m_pMakingBoat->Free();
		m_pMakingBoat = NULL;
	}

	if( m_pStallData )
	{
		m_pStallData->Free();
		m_pStallData = NULL;
	}

	bIsValid = false;
T_E}

void CPlayer::Free()
{T_B
	if(g_pGameApp->m_pCPlySpace)
		g_pGameApp->m_pCPlySpace->ReturnPly(m_lHandle);
T_E}

BOOL CPlayer::HasAllBoatInBerth( USHORT sBerthID )
{
	for( int i = 0; i < m_byNumBoat; i++ )
	{
		if( m_Boat[i]->getAttr( ATTR_BOAT_BERTH ) == sBerthID )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CPlayer::HasBoatInBerth( USHORT sBerthID )
{T_B
	for( int i = 0; i < m_byNumBoat; i++ )
	{
		if( m_Boat[i]->getAttr( ATTR_BOAT_BERTH ) == sBerthID && m_Boat[i]->getAttr( ATTR_BOAT_ISDEAD ) == 0 )
		{
			return TRUE;
		}
	}
	return FALSE;
T_E}

BOOL CPlayer::HasDeadBoatInBerth( USHORT sBerthID )
{
	for( int i = 0; i < m_byNumBoat; i++ )
	{
		if( /*m_Boat[i]->getAttr( ATTR_BOAT_BERTH ) == sBerthID && */m_Boat[i]->getAttr( ATTR_BOAT_ISDEAD ) != 0 )
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CPlayer::GetAllBerthBoat( USHORT sBerthID, BYTE& byNumBoat, BOAT_BERTH_DATA& Data )
{
	BYTE byIndex = 0;
	for( BYTE i = 0; i < m_byNumBoat; i++ )
	{
		//if( m_Boat[i]->getAttr( ATTR_BOAT_BERTH ) == sBerthID )
		//{
		Data.byID[byIndex] = i;
		Data.byState[byIndex] = (BYTE)m_Boat[i]->getAttr( ATTR_BOAT_ISDEAD );
		//strncpy( Data.szName[byIndex], m_Boat[i]->GetName(), BOAT_MAXSIZE_BOATNAME - 1 );
		strncpy_s( Data.szName[byIndex],sizeof(Data.szName[byIndex]), m_Boat[i]->GetName(),_TRUNCATE);

		if( Data.byState[byIndex] != 0 )
			{
				//strcat( Data.szName[byIndex], "  [״̬����û]" );
				//strcat( Data.szName[byIndex], RES_STRING(GM_PLAYER_CPP_00001) );
				strncat_s( Data.szName[byIndex],sizeof(Data.szName[byIndex]),  RES_STRING(GM_PLAYER_CPP_00001),_TRUNCATE);
				Data.byState[byIndex] = BS_DEAD;
			}
			else
			{
				if( m_Boat[i]->getAttr( ATTR_HP ) < (m_Boat[i]->getAttr( ATTR_MXHP )/2) )
				{
					//strcat( Data.szName[byIndex], "  [״̬��������]" );
					//strcat( Data.szName[byIndex], RES_STRING(GM_PLAYER_CPP_00002) );
					strncat_s( Data.szName[byIndex],sizeof(Data.szName[byIndex]), RES_STRING(GM_PLAYER_CPP_00002),_TRUNCATE);
					Data.byState[byIndex] = BS_NOHP;
				}
				else if( m_Boat[i]->getAttr( ATTR_SP ) < (m_Boat[i]->getAttr( ATTR_MXSP )/2) )
				{
					//strcat( Data.szName[byIndex], "  [״̬����������]" );	
					//strcat( Data.szName[byIndex], RES_STRING(GM_PLAYER_CPP_00003) );	
					strncat_s( Data.szName[byIndex],sizeof(Data.szName[byIndex]),  RES_STRING(GM_PLAYER_CPP_00003),_TRUNCATE);
					Data.byState[byIndex] = BS_NOSP;
				}
				else
				{					
					//strcat( Data.szName[byIndex], "  [״̬������]" );
					//strcat( Data.szName[byIndex], RES_STRING(GM_PLAYER_CPP_00004) );
				strncat_s( Data.szName[byIndex],sizeof(Data.szName[byIndex]), RES_STRING(GM_PLAYER_CPP_00004),_TRUNCATE);
				Data.byState[byIndex] = BS_GOOD;
			}
		}

		byIndex++;
		//}
	}
	byNumBoat = byIndex;
}

void CPlayer::GetBerthBoat( USHORT sBerthID, BYTE& byNumBoat, BOAT_BERTH_DATA& Data )
{T_B
	BYTE byIndex = 0;
	for( BYTE i = 0; i < m_byNumBoat; i++ )
	{
		if(/* m_Boat[i]->getAttr( ATTR_BOAT_BERTH ) == sBerthID &&*/ m_Boat[i]->getAttr( ATTR_BOAT_ISDEAD ) == 0 )
		{
			Data.byID[byIndex] = i;
			Data.byState[byIndex] = 0;
			//strncpy( Data.szName[byIndex], m_Boat[i]->GetName(), BOAT_MAXSIZE_BOATNAME - 1 );
			strncpy_s( Data.szName[byIndex],sizeof(Data.szName[byIndex]) ,m_Boat[i]->GetName(),_TRUNCATE);
			byIndex++;
		}
	}
	byNumBoat = byIndex;
T_E}

void CPlayer::GetDeadBerthBoat( USHORT sBerthID, BYTE& byNumBoat, BOAT_BERTH_DATA& Data )
{
	BYTE byIndex = 0;
	for( BYTE i = 0; i < m_byNumBoat; i++ )
	{
		if(/* m_Boat[i]->getAttr( ATTR_BOAT_BERTH ) == sBerthID && */m_Boat[i]->getAttr( ATTR_BOAT_ISDEAD ) != 0 )
		{			
			Data.byID[byIndex] = i;
			Data.byState[byIndex] = 0;
			//strncpy( Data.szName[byIndex], m_Boat[i]->GetName(), BOAT_MAXSIZE_BOATNAME - 1 );
			strncpy_s( Data.szName[byIndex],sizeof(Data.szName[byIndex]) ,m_Boat[i]->GetName(), _TRUNCATE);
			byIndex++;
		}
	}
	byNumBoat = byIndex;
}

BOOL CPlayer::AddBoat( CCharacter& Boat )
{T_B
	if( IsBoatFull() ) return FALSE;
	
	Boat.SetPlayer( this );
	m_Boat[m_byNumBoat] = &Boat;
	m_byNumBoat++;

	if( m_pMainCha )
	{
		Boat.SetPlayer( this );
		Boat.m_CChaAttr.SetChangeFlag();		
		m_pMainCha->SynAddItemCha( &Boat );
		// m_pMainCha->BoatAdd( Boat );
	}
	return TRUE;
T_E}

BOOL CPlayer::ClearBoat( DWORD dwBoatDBID )
{
	for( BYTE i = 0; i < m_byNumBoat; i++ )
	{
		if( m_Boat[i] && m_Boat[i]->getAttr( ATTR_BOAT_DBID ) == dwBoatDBID )
		{
			if( m_pMainCha )
			{
				m_pMainCha->SynDelItemCha( m_Boat[i] );
				//m_pMainCha->BoatClear( *m_Boat[i] );
			}
			m_Boat[i]->Free();

			CCharacter* Boat[MAX_CHAR_BOAT];
			memcpy( Boat, m_Boat, sizeof(CCharacter*)*MAX_CHAR_BOAT );
			memset( m_Boat + i, 0, sizeof(CCharacter*)*(m_byNumBoat - i ) );
			memcpy( m_Boat + i, Boat + i + 1, sizeof(CCharacter*)*(m_byNumBoat - i - 1) );
			m_byNumBoat--;
			return TRUE;
		}
	}
	return FALSE;
}

void CPlayer::RefreshBoatAttr(void)
{
	for (BYTE i = 0; i < m_byNumBoat; i++)
	{
		if (m_Boat[i])
			g_CParser.DoString( "Ship_ExAttrCheck", enumSCRIPT_RETURN_NONE, 0, enumSCRIPT_PARAM_LIGHTUSERDATA, 2, GetMainCha(), m_Boat[i], DOSTRING_PARAM_END );
	}
}

CCharacter* CPlayer::GetBoat( DWORD dwBoatDBID )
{
	for( BYTE i = 0; i < m_byNumBoat; i++ )
	{
		if( m_Boat[i] && m_Boat[i]->getAttr( ATTR_BOAT_DBID ) == dwBoatDBID )
		{
			return m_Boat[i];
		}
	}
	return NULL;
}

BYTE CPlayer::GetBoatIndexByDBID(DWORD dwBoatDBID)
{
	for( BYTE i = 0; i < m_byNumBoat; i++ )
	{
		if( m_Boat[i] && m_Boat[i]->getAttr( ATTR_BOAT_DBID ) == dwBoatDBID )
		{
			return i;
		}
	}
	return -1;
}

CCharacter* CPlayer::GetLuanchOut()
{T_B
	if( m_dwLaunchID == -1 ) {
		return NULL;
	}

	for( int i = 0; i < m_byNumBoat; i++ )
	{
		if( m_Boat[i]->getAttr( ATTR_BOAT_DBID ) == m_dwLaunchID )
		{
			return m_Boat[i];
		}
	}
	return NULL;
T_E}

void CPlayer::SetBerth( USHORT sBerthID, USHORT sxPos, USHORT syPos, USHORT sDir )
{
	m_sBerthID = sBerthID;
	m_sxPos = sxPos;
	m_syPos = syPos;
	m_sDir = sDir;
}

void CPlayer::GetBerth( USHORT& sBerthID, USHORT& sxPos, USHORT& syPos, USHORT& sDir )
{
	sBerthID = m_sBerthID;
	sxPos = m_sxPos;
	syPos = m_syPos;
	sDir = m_sDir;
}

// ����йز�������
// ��ӵ�������
void CPlayer::AddTeamMember(uplayer *pUPlayer)
{T_B
	//LG("team", "Ϊ[%s]��Ӷ���: [dbid = %d] [gate_addr=%d]\n", GetCtrlCha()->GetLogName(), pUPlayer->m_dwDBChaId, pUPlayer->m_ulGateAddr);
	if(_nTeamMemberCnt>=MAX_TEAM_MEMBER)
	{
		//LG("team", "Ҫ������Team����ʱ�ﵽ����[%d], �޷����\n", MAX_TEAM_MEMBER);
		return;
	}
	_Team[_nTeamMemberCnt] = *pUPlayer;
	_nTeamMemberCnt++;
	//LG("team", "��ӳɹ� ���ж������� %d\n", _nTeamMemberCnt);
T_E}

// ������ж��Ѽ�¼
void CPlayer::ClearTeamMember()
{T_B
    _dwTeamLeaderID = 0;
	NoticeTeamLeaderID();

	_nTeamMemberCnt = 0;
T_E}

void CPlayer::LeaveTeam()
{
	ClearTeamMember();
}

void CPlayer::UpdateTeam()
{
}

// Э�� : ��Player��Ϊ��ӳ�Ա����Ϣ���͸����ѵĿͻ���
void CPlayer::NoticeTeamMemberData()
{T_B
	int	nTMemberCnt = GetTeamMemberCnt();
	if(nTMemberCnt==0) return;

	CCharacter *pCha = GetCtrlCha();
	CCharacter *pMainCha = pCha->GetPlayer()->GetMainCha();
	
	
	//LG("team", "[%s]��Ϊ��Ա��������Ա�Ŀͻ���[%d��]֪ͨ������Ϣ\n", pCha->GetName(), nTMemberCnt);

    WPACKET	wpk = GETWPACKET();
	WRITE_CMD(wpk, CMD_MC_TEAM);
	WRITE_LONG(wpk, pMainCha->GetID()); // ���͸��ͻ����Լ��Ľ�ɫΨһID��Ϊ��ʶ
	WRITE_LONG(wpk, (long)pCha->getAttr(ATTR_HP));
	WRITE_LONG(wpk, (long)pCha->getAttr(ATTR_MXHP));
	WRITE_LONG(wpk, (long)pCha->getAttr(ATTR_SP));
	WRITE_LONG(wpk, (long)pCha->getAttr(ATTR_MXSP));
	
	WRITE_LONG(wpk, (long)pMainCha->getAttr(ATTR_LV)); // д���˵ļ�����Ϣ	
	pMainCha->WriteLookData(wpk, LOOK_TEAM);	 // д���˵������Ϣ, ����Ӧ�ô���Ϊ��ʾ������ۺͼ���
#ifdef TEAM_BUFF
	pCha->WriteSkillStateMore(wpk);
#endif
	SENDTOCLIENT2(wpk, nTMemberCnt, _Team);
T_E}

void CPlayer::NoticeTeamLeaderID(void)
{
	int	nTMemberCnt = GetTeamMemberCnt();
	if(nTMemberCnt==0) return;

	CCharacter *pCha = GetCtrlCha();

	WPACKET pk	=GETWPACKET();
	WRITE_CMD(pk, CMD_MC_TLEADER_ID);
	WRITE_LONG(pk, pCha->GetID());
	WRITE_LONG(pk, getTeamLeaderID());

	SENDTOCLIENT2(pk, nTMemberCnt, _Team);
	pCha->NotiChgToEyeshot(pk);
	//pCha->ReflectINFof(pCha, pk);

	//pCha->m_CLog.Log("ͬ�������ţ��ӳ� %d���Լ� %d.", getTeamLeaderID(), pCha->GetID());
	pCha->m_CLog.Log("in phase teamID:header  %d,oneself %d.", getTeamLeaderID(), pCha->GetID());
}

// ������ͬ�ӵĳ�Ա��ɫ
CCharacter* CPlayer::GetTeamMemberCha(int nNo)
{
	CPlayer *pOther = g_pGameApp->GetPlayerByDBID(_Team[nNo].m_dwDBChaId);
	if (!pOther)
		return 0;
	return pOther->GetCtrlCha();
}

CPlayer* CPlayer::GetNextTeamPly(void)
{
	Short	sMemCnt = (Short)GetTeamMemberCnt();
	CPlayer	*pCPly;
	while (m_sGetTeamPlyCount < sMemCnt)
	{
		pCPly = g_pGameApp->GetPlayerByDBID(_Team[m_sGetTeamPlyCount++].m_dwDBChaId);
		if (pCPly)
			return pCPly;
	}
	return 0;
}

bool CPlayer::SetChallengeParam(dbc::Char chParamID, dbc::Long lParamVal)
{
	m_lChallengeParam[chParamID] = lParamVal;

	return true;
}

Long CPlayer::GetChallengeParam(dbc::Char chParamID)
{
	return m_lChallengeParam[chParamID];
}

bool CPlayer::HasChallengeObj(void)
{
	return GetChallengeType() != enumFIGHT_NONE ? true : false;
}

void CPlayer::ClearChallengeObj(bool bAll)
{
	if (!HasChallengeObj())
		return;

	if (bAll)
	{
		Char	chLoop = (Char)GetChallengeParam(0);
		if (chLoop > MAX_TEAM_MEMBER * 2)
			chLoop = MAX_TEAM_MEMBER * 2;
		CPlayer	*pCPly;
		for (Char i = 0; i < chLoop; i++)
		{
			pCPly = g_pGameApp->IsValidPlayer(GetChallengeParam(i * 2 + 2), GetChallengeParam(i * 2 + 2 + 1));
			if (!pCPly)
				continue;
			pCPly->SetChallengeType(enumFIGHT_NONE);
		}
	}

	SetChallengeType(enumFIGHT_NONE);
}

// chPosType 1��װ����.2��������
bool CPlayer::SetRepairPosInfo(dbc::Char chPosType, dbc::Char chPosID)
{
	m_chRepairPosType = chPosType;
	m_chRepairPosID = chPosID;

	CCharacter	*pCCha = GetCtrlCha();
	if (chPosType == 1)
	{
		if (chPosID < enumEQUIP_HEAD || chPosID >= enumEQUIP_NUM)
			return false;
		if (!g_IsRealItemID(pCCha->m_SChaPart.SLink[chPosID].sID))
			return false;
		m_SRepairItem = pCCha->m_SChaPart.SLink[chPosID];
		m_pSRepairItem = pCCha->m_SChaPart.SLink + chPosID;
	}
	else if (chPosType == 2)
	{
		SItemGrid	*pSItemCont = pCCha->GetKitbag()->GetGridContByID(chPosID);
		if (!pSItemCont)
			return false;
		m_SRepairItem = *pSItemCont;
		m_pSRepairItem = pSItemCont;
	}

	return true;
}

bool CPlayer::OpenRepair(CCharacter *pCNpc)
{
	m_pCRepairman = pCNpc;
	GetCtrlCha()->SynBeginItemRepair();
	return true;
}

bool CPlayer::OpenForge(CCharacter *pCNpc)
{
	if (IsInForge())
	{
		//SystemNotice("֮ǰ������û�����!");
		SystemNotice(RES_STRING(GM_PLAYER_CPP_00005));
		return false;
	}
	m_pCForgeman = pCNpc;
	GetCtrlCha()->SynBeginItemForge();
	GetCtrlCha()->ForgeAction();
	return true;
}

// Add by lark.li 20080514 begin
bool CPlayer::OpenLottery(CCharacter *pCNpc)
{
	//SystemNotice("���Ʊ��������ԣ�");

	m_pCLotteryman = pCNpc;
	GetCtrlCha()->SynBeginItemLottery();
	return true;
}
// End

//Add by sunny.sun 20080716
//Begin
bool CPlayer::OpenAmphitheater(CCharacter *pCNpc)
{
	m_pCAmphitheaterman = pCNpc;
	GetCtrlCha()->SynBeginItemAmphitheater();
	return true;
}

bool CPlayer::OpenJewelryUp( CCharacter* pCNpc )
{
	if (IsInForge())
	{
		//SystemNotice("֮ǰ������û�����!");
		SystemNotice(RES_STRING(GM_PLAYER_CPP_00005));
		return false;
	}
	m_pCForgeman = pCNpc;
	GetCtrlCha()->SynBeginItemJewelryUp();
	GetCtrlCha()->ForgeAction();
	return true;
}
//End

bool CPlayer::OpenUnite(CCharacter *pCNpc)
{
	if (IsInForge())
	{
		//SystemNotice("֮ǰ������û�����!");
		SystemNotice(RES_STRING(GM_PLAYER_CPP_00005));
		return false;
	}
	m_pCForgeman = pCNpc;
	GetCtrlCha()->SynBeginItemUnite();
	GetCtrlCha()->ForgeAction();
	return true;
}

bool CPlayer::OpenMilling(CCharacter *pCNpc)
{
	if (IsInForge())
	{
		//SystemNotice("֮ǰ������û�����!");
		SystemNotice(RES_STRING(GM_PLAYER_CPP_00005));
		return false;
	}
	m_pCForgeman = pCNpc;
	GetCtrlCha()->SynBeginItemMilling();
	GetCtrlCha()->ForgeAction();
	return true;
}

bool CPlayer::OpenFusion(CCharacter *pCNpc)
{
	if (IsInForge())
	{
		//SystemNotice("֮ǰ������û�����!");
		SystemNotice(RES_STRING(GM_PLAYER_CPP_00005));
		return false;
	}
	m_pCForgeman = pCNpc;
	GetCtrlCha()->SynBeginItemFusion();
	GetCtrlCha()->ForgeAction();
	return true;
}

bool CPlayer::OpenUpgrade(CCharacter *pCNpc)
{
	if (IsInForge())
	{
		//SystemNotice("֮ǰ������û�����!");
		SystemNotice(RES_STRING(GM_PLAYER_CPP_00005));
		return false;
	}
	m_pCForgeman = pCNpc;
	GetCtrlCha()->SynBeginItemUpgrade();
	GetCtrlCha()->ForgeAction();
	return true;
}

bool CPlayer::OpenEidolonMetempsychosis(CCharacter *pCNpc)
{
	if (IsInForge())
	{
		//SystemNotice("֮ǰ������û�����!");
		SystemNotice(RES_STRING(GM_PLAYER_CPP_00005));
		return false;
	}
	m_pCForgeman = pCNpc;
	GetCtrlCha()->SynBeginItemEidolonMetempsychosis();
	GetCtrlCha()->ForgeAction();
	return true;
}

bool CPlayer::OpenEidolonFusion(CCharacter *pCNpc)
{
	if (IsInForge())
	{
		//SystemNotice("֮ǰ������û�����!");
		SystemNotice(RES_STRING(GM_PLAYER_CPP_00005));
		return false;
	}
	m_pCForgeman = pCNpc;
	GetCtrlCha()->SynBeginItemEidolonFusion();
	GetCtrlCha()->ForgeAction();
	return true;
}

bool CPlayer::OpenPurify(CCharacter *pCNpc)
{
	if (IsInForge())
	{
		//SystemNotice("֮ǰ������û�����!");
		SystemNotice(RES_STRING(GM_PLAYER_CPP_00005));
		return false;
	}
	m_pCForgeman = pCNpc;
	GetCtrlCha()->SynBeginItemPurify();
	GetCtrlCha()->ForgeAction();
	return true;
}

bool CPlayer::OpenFix(CCharacter *pCNpc)
{
	if (IsInForge())
	{
		//SystemNotice("֮ǰ������û�����!");
		SystemNotice(RES_STRING(GM_PLAYER_CPP_00005));
		return false;
	}
	m_pCForgeman = pCNpc;
	GetCtrlCha()->SynBeginItemFix();
	GetCtrlCha()->ForgeAction();
	return true;
}

bool CPlayer::OpenEnergy(CCharacter *pCNpc)
{
	if (IsInForge())
	{
		//SystemNotice("֮ǰ������û�����!");
		SystemNotice(RES_STRING(GM_PLAYER_CPP_00005));
		return false;
	}
	m_pCForgeman = pCNpc;
	GetCtrlCha()->SynBeginItemEnergy();
	GetCtrlCha()->ForgeAction();
	return true;
}

bool CPlayer::OpenGMSend(CCharacter *pCNpc)
{
	GetCtrlCha()->SynBeginGMSend();
	return true;
}

bool CPlayer::OpenGMRecv(CCharacter *pCNpc)
{
	GetCtrlCha()->SynBeginGMRecv(pCNpc->GetID());
	return true;
}

bool CPlayer::OpenGetStone(CCharacter *pCNpc)
{
	if (IsInForge())
	{
		//SystemNotice("֮ǰ������û�����!");
		SystemNotice(RES_STRING(GM_PLAYER_CPP_00005));
		return false;
	}
	m_pCForgeman = pCNpc;
	GetCtrlCha()->SynBeginGetStone();
	GetCtrlCha()->ForgeAction();
	return true;
}

bool CPlayer::OpenTiger(CCharacter *pCNpc)
{
	if (IsInForge())
	{
		//SystemNotice("֮ǰ������û�����!");
		SystemNotice(RES_STRING(GM_PLAYER_CPP_00005));
		return false;
	}
	m_pCForgeman = pCNpc;
	GetCtrlCha()->SynBeginTiger();
	GetCtrlCha()->ForgeAction();
	return true;
}

void CPlayer::SystemNotice( const char szData[], ... )
{T_B
	// Modify by lark.li 20080801 begin
	char szTemp[250];
	memset(szTemp, 0, sizeof(szTemp));
	va_list list;
	va_start( list, szData );
	//_vsnprintf(szTemp, sizeof(szTemp) - 1, szData, list );
	_vsnprintf_s(szTemp, sizeof(szTemp),_TRUNCATE, szData, list );

	//vsprintf( szTemp, szData, list );
	// End
	va_end( list );

	WPACKET packet = GETWPACKET();
	WRITE_CMD(packet, CMD_MC_SYSINFO );
	WRITE_SEQ(packet, szTemp, uShort( strlen(szTemp) ) + 1 );
	
	WRITE_LONG(packet, GetDBChaId());
	WRITE_LONG(packet, GetGateAddr());
	WRITE_SHORT(packet, 1);

	GetGate()->SendData(packet);
T_E}

bool CPlayer::RefreshMapMask(const char *szMapName, long lPosX, long lPosY)
{
	if (!strcmp(szMapName, m_szMaskMapName))
	{
		if (m_CMapMask.UpdateMapMask((char *)szMapName, lPosX, lPosY, m_lLightSize))
			SetMapMaskChange();
		return true;
	}

	return false;
}

bool CPlayer::AddBank(void)
{
	return game_db.CreatePlyBank(this);
}

bool CPlayer::SaveBank(char chBankNO)
{
	return game_db.SavePlyBank(this, chBankNO);
}

bool CPlayer::SetBankChangeFlag(char chBankNO, bool bChange)
{
	if (GetCurBankNum() == 0)
		return true;

	char	chStart, chEnd;
	if (chBankNO < 0)
	{
		chStart = 0;
		chEnd = GetCurBankNum() - 1;
	}
	else
	{
		if (chBankNO >= GetCurBankNum())
			return false;
		chStart = chEnd = chBankNO;
	}

	CKitbag	*pCBank;
	for (char i = chStart; i <= chEnd; i++)
	{
		pCBank = GetBank(i);
		if (!pCBank)
			continue;
		pCBank->SetChangeFlag(bChange);
	}

	return true;
}

bool CPlayer::SynBank(char chBankNO, char chType)
{
	if (GetCurBankNum() == 0)
		return true;

	char	chStart, chEnd;
	if (chBankNO < 0)
	{
		chStart = 0;
		chEnd = GetCurBankNum() - 1;
	}
	else
	{
		if (chBankNO >= GetCurBankNum())
			return false;
		chStart = chEnd = chBankNO;
	}

	WPACKET WtPk = GETWPACKET();
	WRITE_CMD(WtPk, CMD_MC_NOTIACTION);	// ͨ���ж�
	WRITE_LONG(WtPk, m_pCtrlCha->GetID());
	WRITE_LONG(WtPk, m_pCtrlCha->m_ulPacketID);
	WRITE_CHAR(WtPk, enumACTION_BANK);

	CKitbag	*pCBank;
	for (char i = chStart; i <= chEnd; i++)
	{
		pCBank = GetBank(i);
		if (!pCBank)
			continue;
		m_pCtrlCha->WriteKitbag(pCBank, WtPk, chType);
	}
	m_pCtrlCha->ReflectINFof(m_pCtrlCha, WtPk);

	return true;
}

bool CPlayer::BankCanOpen(CCharacter *pCNpc)
{
	CCharacter	*pCCtrlCha = GetCtrlCha();
	if (pCCtrlCha != GetMainCha())
	{
		//pCCtrlCha->SystemNotice("��ǰ��ɫ���ܽ�������");
		pCCtrlCha->SystemNotice(RES_STRING(GM_PLAYER_CPP_00006));
		return false;
	}
	if (m_pCBankNpc)
	{
		//pCCtrlCha->SystemNotice("�����Ѿ��ڴ�״̬");
		pCCtrlCha->SystemNotice(RES_STRING(GM_PLAYER_CPP_00007));
		return false;
	}
	if (!pCNpc)
	{
		//pCCtrlCha->SystemNotice("����NPC������");
		pCCtrlCha->SystemNotice(RES_STRING(GM_PLAYER_CPP_00008));
		return false;
	}
	if (pCCtrlCha->HasTradeAction())
	{
		//pCCtrlCha->SystemNotice("�Ѿ��ڽ�����");
		pCCtrlCha->SystemNotice(RES_STRING(GM_PLAYER_CPP_00009));
		return false;
	}
	// �����жϣ���Npc����ʱ�Ѿ����˼��
	if (!pCCtrlCha->IsRangePoint(pCNpc->GetPos(), defBANK_DISTANCE))
	{
		//pCCtrlCha->SystemNotice("����̫Զ");
		pCCtrlCha->SystemNotice(RES_STRING(GM_PLAYER_CPP_00010));
		return false;
	}
	if (!pCCtrlCha->TradeAction(true))
	{
		//pCCtrlCha->SystemNotice("���ý���״̬ʧ��");
		pCCtrlCha->SystemNotice(RES_STRING(GM_PLAYER_CPP_00011));
		return false;
	}

	return true;
}

bool CPlayer::OpenBank(CCharacter *pCNpc)
{
	CCharacter	*pCCtrlCha = GetCtrlCha();

	if (!SetBankChangeFlag(0))
	{
		pCCtrlCha->TradeAction(false);
		//pCCtrlCha->SystemNotice("ȡ��������ʧ��");
		pCCtrlCha->SystemNotice(RES_STRING(GM_PLAYER_CPP_00012));
		return false;
	}
	if (!SynBank(0))
	{
		pCCtrlCha->TradeAction(false);
		//pCCtrlCha->SystemNotice("ȡ��������ʧ��");
		pCCtrlCha->SystemNotice(RES_STRING(GM_PLAYER_CPP_00012));
		return false;
	}
	m_pCBankNpc = pCNpc;

	return true;
}

void CPlayer::CloseBank(void)
{
	m_pCBankNpc = 0;
	GetMainCha()->TradeAction(false);
}

bool CPlayer::SetBankSaveFlag(char chBankNO, bool bChange)
{
	if (GetCurBankNum() == 0)
		return false;

	char	chStart, chEnd;
	if (chBankNO < 0)
	{
		chStart = 0;
		chEnd = GetCurBankNum() - 1;
	}
	else
	{
		if (chBankNO >= GetCurBankNum())
			return false;
		chStart = chEnd = chBankNO;
	}

	for (char i = chStart; i <= chEnd; i++)
		m_bBankChange[i] = bChange;

	return true;
}

bool CPlayer::BankWillSave(char chBankNO)
{
	if (GetCurBankNum() == 0 || chBankNO >= GetCurBankNum())
		return false;

	return m_bBankChange[chBankNO];
}

bool CPlayer::BankHasItem(USHORT sItemID, USHORT& sCount)
{
	int nCount = 0;
	CItemRecord* pItem = GetItemRecordInfo( sItemID );
	if( pItem == NULL )
	{
		//SystemNotice( "BankHasItem:�������Ʒ��������!ID = %d", sItemID );
		SystemNotice( RES_STRING(GM_PLAYER_CPP_00013), sItemID );
		return FALSE;
	}

	USHORT sNum = m_CBank[0].GetUseGridNum();
	SItemGrid *pGridCont;
	if( !pItem->GetIsPile() )
	{
		for( int i = 0; i < sNum; i++ )
		{
			pGridCont = m_CBank[0].GetGridContByNum( i );
			if( pGridCont )
			{
				if( sItemID == pGridCont->sID )
				{
					nCount++;
					if( nCount >= sCount )
						break;
				}
			}
		}
	}
	else
	{
		for( int i = 0; i < sNum; i++ )
		{
			pGridCont = m_CBank[0].GetGridContByNum( i );
			if( pGridCont )
			{
				if( sItemID == pGridCont->sID )
				{
					nCount += (USHORT)pGridCont->sNum;;
					if( nCount >= sCount )
						break;
				}
			}
		}
	}

	return nCount >= sCount;
}

char* CPlayer::BankDBIDData2String(char *szSStateBuf, int nLen)
{
	if (!szSStateBuf) return NULL;

	char	szData[512];
	int		nBufLen = 0, nDataLen;
	szSStateBuf[0] = '\0';

	for (char i = 0; i < m_chBankNum; i++)
	{
		//sprintf(szData, "%d", m_lBankDBID[i]);
		_snprintf_s(szData,sizeof(szData),_TRUNCATE, "%d", m_lBankDBID[i]);

		nDataLen = (int)strlen(szData);
		if (nBufLen + nDataLen >= nLen) return NULL;
		//strcat(szSStateBuf, szData);
		strncat_s( szSStateBuf,nLen, szData,_TRUNCATE);
		nBufLen += nDataLen;

		if (i < m_chBankNum - 1)
		{
			//sprintf(szData, ",", m_lBankDBID[i]);
			_snprintf_s(szData,sizeof(szData),_TRUNCATE, ",", m_lBankDBID[i]);

			nDataLen = (int)strlen(szData);
			if (nBufLen + nDataLen >= nLen) return NULL;
			//strcat(szSStateBuf, szData);
			strncat_s( szSStateBuf,nLen, szData,_TRUNCATE);
			nBufLen += nDataLen;
		}
	}

	return szSStateBuf;
}

bool CPlayer::Strin2BankDBIDData(std::string &strData)
{
	if (!strcmp(strData.c_str(), ""))
		return true;

	std::string strList[MAX_BANK_NUM];
	int nSegNum = Util_ResolveTextLine(strData.c_str(), strList, MAX_BANK_NUM, ',');
	if (nSegNum > MAX_BANK_NUM)
		return false;

	short	sTCount = 0;
	for (int i = 0; i < nSegNum; i++)
	{
		AddBankDBID(Str2Int(strList[i]));
	}

	return true;
}

void CPlayer::CheckChaItemFinalData()
{
	// ���
	cChar	*szScript = "check_item_final_data";
	CCharacter	*pCMainCha = GetMainCha();
	for (int i = 0; i < enumEQUIP_NUM; i++)
	{
		if (g_IsRealItemID(pCMainCha->m_SChaPart.SLink[i].sID))
		{
			if(pCMainCha->m_SChaPart.SLink[i].InitAttr())
				g_CParser.DoString(szScript, enumSCRIPT_RETURN_NONE, 0, enumSCRIPT_PARAM_LIGHTUSERDATA, 1, pCMainCha->m_SChaPart.SLink + i, DOSTRING_PARAM_END);
		}
	}

	// ����
	SItemGrid	*pGridCont;
	Short	sUseNum = pCMainCha->GetKitbag()->GetUseGridNum();
	for (int i = 0; i < sUseNum; i++)
	{
		pGridCont = pCMainCha->GetKitbag()->GetGridContByNum(i);
		if (!pGridCont)
			continue;
		if(pGridCont->InitAttr())
			g_CParser.DoString(szScript, enumSCRIPT_RETURN_NONE, 0, enumSCRIPT_PARAM_LIGHTUSERDATA, 1, pGridCont, DOSTRING_PARAM_END);
	}

	// ����
	for (int j = 0; j < MAX_BANK_NUM; j++)
	{
		sUseNum = m_CBank[j].GetUseGridNum();
		for (int i = 0; i < sUseNum; i++)
		{
			pGridCont = m_CBank[j].GetGridContByNum(i);
			if (!pGridCont)
				continue;
			if(pGridCont->InitAttr())
				g_CParser.DoString(szScript, enumSCRIPT_RETURN_NONE, 0, enumSCRIPT_PARAM_LIGHTUSERDATA, 1, pGridCont, DOSTRING_PARAM_END);
		}
	}
}

bool CPlayer::String2BankData(char chBankNO, std::string &strData)
{
	if (::String2KitbagData(GetBank(chBankNO), strData))
	{
		if (m_pMainCha)
			m_pMainCha->CheckBagItemValid(GetBank(chBankNO));
		return true;
	}

	return false;
}

void CPlayer::Run(DWORD dwCurTime)
{
	if (HasChallengeObj())
	{
		if (m_timerChallenge.IsOK(dwCurTime))
		{
			LG("teamPK_flow", "time out!\n");
			//SystemNotice("���볬ʱ!");
			SystemNotice(RES_STRING(GM_PLAYER_CPP_00014));
			ClearChallengeObj(false);
		}
	}
}
