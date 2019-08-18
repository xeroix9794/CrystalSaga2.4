//=============================================================================
// FileName: CommFunc.h
// Creater: ZhangXuedong
// Date: 2005.01.06
// Comment: 
//	2005.4.28	Arcol	add the text filter manager class: CTextFilter
//=============================================================================

#ifndef COMMFUNC_H
#define COMMFUNC_H

#include "CompCommand.h"
#include "SkillRecord.h"
#include "CharacterRecord.h"
#include "ItemRecord.h"
#include "ItemAttrType.h"
#include "JobType.h"
#include "NetRetCode.h"
#include "i18n.h"
#include "StringPoolL.h"

class CommonRes
{
public:
	static const char* GetJobName(int index);
	static const char* GetCityName(int index);
};

extern bool KitbagStringConv(short sKbCapacity, std::string &strData);

//=============================================================================
/*---------------------------------------------------------------
* ��;:���ڼ�ⴴ���Ľ�ɫ��������Ƿ�Ϸ�
* nPart - ��Ӧ���ID,nValue - ��۵�ֵ
* ����ֵ����������Ƿ�Ϸ���
*/
extern bool g_IsValidLook( int nType, int nPart, long nValue );

/*---------------------------------------------------------------
* ulAreaMask ��������
* ����ֵ��true ����false ½��
*/
inline bool g_IsSea(unsigned short usAreaMask)
{
	return !(usAreaMask & enumAREA_TYPE_LAND);
}

inline bool g_IsLand(unsigned short usAreaMask)
{
	return (usAreaMask & enumAREA_TYPE_LAND) || (usAreaMask & enumAREA_TYPE_BRIDGE);
}

// ���ݴ���������ֵ���ID
// ���ؿ���ʹ�õ�Ĭ�ϼ���,����-1,û�м���
extern int g_GetItemSkill( int nLeftItemID, int nRightItemID );

extern BOOL IsDist( int x1, int y1, int x2, int y2, DWORD dwDist );

// �Ƿ���ȷ�ļ���Ŀ��
extern int	g_IsRightSkillTar(int nTChaCtrlType, bool bTIsDie, bool bTChaBeSkilled, int nTChaArea,
					  int nSChaCtrlType, int nSSkillObjType, int nSSkillObjHabitat, int nSSkillEffType,
					  bool bIsTeammate, bool bIsFriend, bool bIsSelf);

/*---------------------------------------------------------------
* ����:���֣����֣�����ĵ���ID�����ܱ�š�
* ����ֵ:1-��ʹ��,0-����ʹ��,-1-���ܲ�����
*/
extern int g_IsUseSkill( stNetChangeChaPart *pSEquip, int nSkillID );
extern bool g_IsRealItemID(int nItemID);

inline int g_IsUseSkill( stNetChangeChaPart *pSEquip, CSkillRecord *p )
{
	if( !p )  return -1;

	return g_IsUseSkill( pSEquip, p->nID );
}

inline int g_IsUseSeaLiveSkill(long lFitNo, CSkillRecord *p)
{
	if( !p )  return -1;

	for( int i=0; i<defSKILL_ITEM_NEED_NUM; i++ )
	{	
		if( p->sItemNeed[0][i][0] == cchSkillRecordKeyValue )
			break;

		if (p->sItemNeed[0][i][0] == enumSKILL_ITEM_NEED_ID)
		{
			if (p->sItemNeed[0][i][1] == lFitNo)
				return 1;
		}
	}

	return 0;
}

inline bool g_IsPlyCtrlCha(int nChaCtrlType)
{
	if (nChaCtrlType == enumCHACTRL_PLAYER || nChaCtrlType == enumCHACTRL_PLAYER_PET)
		return true;
	return false;
}

inline bool g_IsMonsCtrlCha(int nChaCtrlType)
{
	if (nChaCtrlType == enumCHACTRL_MONS
		|| nChaCtrlType == enumCHACTRL_MONS_TREE
		|| nChaCtrlType == enumCHACTRL_MONS_MINE
		|| nChaCtrlType == enumCHACTRL_MONS_FISH
		|| nChaCtrlType == enumCHACTRL_MONS_DBOAT
		|| nChaCtrlType == enumCHACTRL_MONS_REPAIRABLE
		)
		return true;
	return false;
}

inline bool g_IsNPCCtrlCha(int nChaCtrlType)
{
	if (nChaCtrlType == enumCHACTRL_NPC || nChaCtrlType == enumCHACTRL_NPC_EVENT)
		return true;
	return false;
}

inline bool g_IsChaEnemyCtrlSide(int nSCtrlType, int nTCtrlType)
{
	if (g_IsPlyCtrlCha(nSCtrlType) & g_IsPlyCtrlCha(nTCtrlType))
		return false;
	if (g_IsMonsCtrlCha(nSCtrlType) & g_IsMonsCtrlCha(nTCtrlType))
		return false;
	return true;
}

inline bool g_IsFileExist(const char *szFileName)
{
    FILE	*fp = NULL;
    //if (NULL == (fp = fopen(szFileName,"rb")))
	if (fopen_s(&fp, szFileName,"rb") != 0)
        return false;

	if(fp == NULL)
		return false;

    if (fp) fclose(fp);
    return true;
}

extern char* LookData2String(const stNetChangeChaPart *pLook, char *szLookBuf, int nLen, bool bNewLook = true);
extern const char* LookData2StringEx(const stNetChangeChaPart *pLook, StringPoolL& lookString, int index, bool bNewLook = true);

extern bool Strin2LookData(stNetChangeChaPart *pLook, std::string &strData);
extern char* ShortcutData2String(const stNetShortCut *pShortcut, char *szShortcutBuf, int nLen);
extern const char* ShortcutData2StringEx(const stNetShortCut *pShortcut, StringPoolL& lookString, int index);

extern bool String2ShortcutData(stNetShortCut *pShortcut, std::string &strData);


inline long g_ConvItemAttrTypeToCha(long lItemAttrType)
{
	if (lItemAttrType >= ITEMATTR_COE_STR && lItemAttrType <= ITEMATTR_COE_PDEF)
		return lItemAttrType + (ATTR_ITEMC_STR - ITEMATTR_COE_STR);
	else if (lItemAttrType >= ITEMATTR_VAL_STR && lItemAttrType <= ITEMATTR_VAL_PDEF)
		return lItemAttrType + (ATTR_ITEMV_STR - ITEMATTR_VAL_STR);
	else
		return 0;
}

// ��Ӧ�������͵Ĳ�������
inline short g_GetRangeParamNum(char RangeType)
{
	short	sParamNum = 0;
	switch (RangeType)
	{
	case	enumRANGE_TYPE_STICK:
		sParamNum = 2;
		break;
	case	enumRANGE_TYPE_FAN:
		sParamNum = 2;
		break;
	case	enumRANGE_TYPE_SQUARE:
		sParamNum = 1;
		break;
	case	enumRANGE_TYPE_CIRCLE:
		sParamNum = 1;
		break;
	}

	return sParamNum + 1;
}

//=============================================================================
// chChaType ��ɫ����
// chChaTerrType ��ɫ��ռ������
// bIsBlock �Ƿ��ϰ�
// ulAreaMask ��������
// ����ֵ��true ���ڸõ�Ԫ���ƶ���false �����ƶ�
//=============================================================================
inline bool	g_IsMoveAble(char chChaCtrlType, char chChaTerrType, unsigned short usAreaMask)
{
	bool	bRet1 = false;
	if (chChaTerrType == defCHA_TERRITORY_DISCRETIONAL)
		bRet1 = true;
	else if (chChaTerrType == defCHA_TERRITORY_LAND)
	{
		if (usAreaMask & enumAREA_TYPE_LAND || usAreaMask & enumAREA_TYPE_BRIDGE)
			bRet1 = true;
	}
	else if (chChaTerrType == defCHA_TERRITORY_SEA)
	{
		if (!(usAreaMask & enumAREA_TYPE_LAND))
			bRet1 = true;
	}

	bool	bRet2 = true;
	if (usAreaMask & enumAREA_TYPE_NOT_FIGHT) // ��ս������
	{
		if (g_IsMonsCtrlCha(chChaCtrlType))
			bRet2 = false;
	}

	return bRet1 && bRet2;
}

inline const char* g_GetJobName(short sJobID)
{
	if (sJobID < 0 || sJobID >= MAX_JOB_TYPE)
		//return g_szJobName[0];
		CommonRes::GetJobName(0);

	//return g_szJobName[sJobID];
	return CommonRes::GetJobName(sJobID);
}

inline short g_GetJobID(const char *szJobName)
{
	for (short i = 0; i < MAX_JOB_TYPE; i++)
	{
		//if (!strcmp(g_szJobName[i], szJobName))
		if (!strcmp(CommonRes::GetJobName(i), szJobName))
			return i;
	}

	return 0;
}

inline const char* g_GetCityName(short sCityID)
{
	if (sCityID < 0 || sCityID >= defMAX_CITY_NUM)
		return "";

	//return g_szCityName[sCityID];
	return CommonRes::GetCityName(sCityID);
}

inline short g_GetCityID(const char *szCityName)
{
	for (short i = 0; i < defMAX_CITY_NUM; i++)
	{
		//if (!strcmp(g_szCityName[i], szCityName))
		if (!strcmp(CommonRes::GetCityName(i), szCityName))
			return i;
	}

	return -1;
}

inline bool g_IsSeatPose( int pose )
{
	return 16==pose;
}

// �������ַ�����
inline bool g_IsValidFightState( int nState )
{
	return nState < enumFSTATE_TARGET_NO;
}

inline bool g_ExistStateIsDie(char chState)
{
	if (chState >= enumEXISTS_WITHERING)
		return true;

	return false;
}

inline const char* g_GetItemAttrExplain( int v )
{
    switch( v )
    {
    case ITEMATTR_COE_STR: return RES_STRING(CO_COMMFUNC_H_00001);// "�����ӳ�"; 
    case ITEMATTR_COE_AGI: return RES_STRING(CO_COMMFUNC_H_00002);// "���ݼӳ�"; 
    case ITEMATTR_COE_DEX: return RES_STRING(CO_COMMFUNC_H_00003);// "רע�ӳ�"; 
    case ITEMATTR_COE_CON: return RES_STRING(CO_COMMFUNC_H_00004);// "���ʼӳ�"; 
    case ITEMATTR_COE_STA: return RES_STRING(CO_COMMFUNC_H_00005);// "����ӳ�"; 
    case ITEMATTR_COE_LUK: return RES_STRING(CO_COMMFUNC_H_00006);// "���˼ӳ�"; 
    case ITEMATTR_COE_ASPD: return RES_STRING(CO_COMMFUNC_H_00007);// "����Ƶ�ʼӳ�"; 
    case ITEMATTR_COE_ADIS: return RES_STRING(CO_COMMFUNC_H_00008);// "��������ӳ�"; 
    case ITEMATTR_COE_MNATK: return RES_STRING(CO_COMMFUNC_H_00009);// "��С�������ӳ�"; 
    case ITEMATTR_COE_MXATK: return RES_STRING(CO_COMMFUNC_H_00010);// "��󹥻����ӳ�"; 
    case ITEMATTR_COE_DEF: return RES_STRING(CO_COMMFUNC_H_00011);// "�����ӳ�"; 
    case ITEMATTR_COE_MXHP: return RES_STRING(CO_COMMFUNC_H_00012);// "���HP�ӳ�"; 
    case ITEMATTR_COE_MXSP: return RES_STRING(CO_COMMFUNC_H_00013);// "���SP�ӳ�"; 
    case ITEMATTR_COE_FLEE: return RES_STRING(CO_COMMFUNC_H_00014);// "�����ʼӳ�"; 
    case ITEMATTR_COE_HIT: return RES_STRING(CO_COMMFUNC_H_00015);// "�����ʼӳ�"; 
    case ITEMATTR_COE_CRT: return RES_STRING(CO_COMMFUNC_H_00016);// "�����ʼӳ�"; 
    case ITEMATTR_COE_MF: return RES_STRING(CO_COMMFUNC_H_00017);// "Ѱ���ʼӳ�"; 
    case ITEMATTR_COE_HREC: return RES_STRING(CO_COMMFUNC_H_00018);// "HP�ָ��ٶȼӳ�"; 
    case ITEMATTR_COE_SREC: return RES_STRING(CO_COMMFUNC_H_00019);// "SP�ָ��ٶȼӳ�"; 
    case ITEMATTR_COE_MSPD: return RES_STRING(CO_COMMFUNC_H_00020);// "�ƶ��ٶȼӳ�"; 
    case ITEMATTR_COE_COL: return RES_STRING(CO_COMMFUNC_H_00021);// "��Դ�ɼ��ٶȼӳ�"; 

    case ITEMATTR_VAL_STR: return RES_STRING(CO_COMMFUNC_H_00001);// "�����ӳ�"; 
    case ITEMATTR_VAL_AGI: return RES_STRING(CO_COMMFUNC_H_00002);// "���ݼӳ�"; 
    case ITEMATTR_VAL_DEX: return RES_STRING(CO_COMMFUNC_H_00003);// "רע�ӳ�"; 
    case ITEMATTR_VAL_CON: return RES_STRING(CO_COMMFUNC_H_00004);// "���ʼӳ�"; 
    case ITEMATTR_VAL_STA: return RES_STRING(CO_COMMFUNC_H_00005);// "����ӳ�"; 
    case ITEMATTR_VAL_LUK: return RES_STRING(CO_COMMFUNC_H_00006);// "���˼ӳ�"; 
    case ITEMATTR_VAL_ASPD: return RES_STRING(CO_COMMFUNC_H_00007);// "����Ƶ�ʼӳ�"; 
    case ITEMATTR_VAL_ADIS: return RES_STRING(CO_COMMFUNC_H_00008);// "��������ӳ�"; 
    case ITEMATTR_VAL_MNATK: return RES_STRING(CO_COMMFUNC_H_00009);// "��С�������ӳ�"; 
    case ITEMATTR_VAL_MXATK: return RES_STRING(CO_COMMFUNC_H_00010);// "��󹥻����ӳ�"; 
    case ITEMATTR_VAL_DEF: return RES_STRING(CO_COMMFUNC_H_00011);// "�����ӳ�"; 
    case ITEMATTR_VAL_MXHP: return RES_STRING(CO_COMMFUNC_H_00012);// "���HP�ӳ�"; 
    case ITEMATTR_VAL_MXSP: return RES_STRING(CO_COMMFUNC_H_00013);// "���SP�ӳ�"; 
    case ITEMATTR_VAL_FLEE: return RES_STRING(CO_COMMFUNC_H_00014);// "�����ʼӳ�"; 
    case ITEMATTR_VAL_HIT: return RES_STRING(CO_COMMFUNC_H_00015);// "�����ʼӳ�"; 
    case ITEMATTR_VAL_CRT: return RES_STRING(CO_COMMFUNC_H_00016);// "�����ʼӳ�"; 
    case ITEMATTR_VAL_MF: return RES_STRING(CO_COMMFUNC_H_00017);// "Ѱ���ʼӳ�"; 
    case ITEMATTR_VAL_HREC: return RES_STRING(CO_COMMFUNC_H_00018);// "HP�ָ��ٶȼӳ�"; 
    case ITEMATTR_VAL_SREC: return RES_STRING(CO_COMMFUNC_H_00019);// "SP�ָ��ٶȼӳ�"; 
    case ITEMATTR_VAL_MSPD: return RES_STRING(CO_COMMFUNC_H_00020);// "�ƶ��ٶȼӳ�"; 
    case ITEMATTR_VAL_COL: return RES_STRING(CO_COMMFUNC_H_00021);// "��Դ�ɼ��ٶȼӳ�"; 

	case ITEMATTR_VAL_PDEF: return RES_STRING(CO_COMMFUNC_H_00022);// "����ֿ��ӳ�";
    case ITEMATTR_MAXURE: return RES_STRING(CO_COMMFUNC_H_00023);// "����;ö�"; 
    case ITEMATTR_MAXENERGY: return RES_STRING(CO_COMMFUNC_H_00024);// "�������"; 
    default: return RES_STRING(CO_COMMFUNC_H_00025);// "δ֪��������";
    }
}

inline const char* g_GetServerError( int error_code )  // ����������
{
	switch( error_code )
	{
	case ERR_AP_INVALIDUSER:return RES_STRING(CO_COMMFUNC_H_00026);// "��Ч�˺�";
	case ERR_AP_INVALIDPWD:	return RES_STRING(CO_COMMFUNC_H_00027);// "�������";
	case ERR_AP_ACTIVEUSER: return RES_STRING(CO_COMMFUNC_H_00028);// "�����û�ʧ��";
    case ERR_AP_DISABLELOGIN: return RES_STRING(CO_COMMFUNC_H_00029);// "Ŀǰ���Ľ�ɫ���������ߴ��̹����У����Ժ��ٳ��Ե�¼��";
    case ERR_AP_LOGGED: return RES_STRING(CO_COMMFUNC_H_00030);// "���ʺ��Ѵ��ڵ�¼״̬";
    case ERR_AP_BANUSER: return RES_STRING(CO_COMMFUNC_H_00031);// "�ʺ��ѷ�ͣ";
	// Add by lark.li 20080825 begin
	case ERR_AP_PBANUSER: return RES_STRING(CO_COMMFUNC_H_00108);// "�Ѿ�ʹ�����ܱ���";
	// End
	case ERR_AP_GPSLOGGED: return RES_STRING(CO_COMMFUNC_H_00032);// "��GroupServer�ѵ�¼";
	case ERR_AP_GPSAUTHFAIL: return RES_STRING(CO_COMMFUNC_H_00033);// "��GroupServer��֤ʧ��";
	case ERR_AP_SAVING:	return RES_STRING(CO_COMMFUNC_H_00034);// "���ڱ�����Ľ�ɫ����15�������...";
    case ERR_AP_LOGINTWICE: return RES_STRING(CO_COMMFUNC_H_00035);// "����˺���Զ���ٴε�¼";
    case ERR_AP_ONLINE: return RES_STRING(CO_COMMFUNC_H_00036);// "����˺�������";
	case ERR_AP_DISCONN: return RES_STRING(CO_COMMFUNC_H_00037);// "GroupServer�ѶϿ�";
	case ERR_AP_UNKNOWNCMD: return RES_STRING(CO_COMMFUNC_H_00038);// "δ֪Э�飬������";
	case ERR_AP_TLSWRONG: return RES_STRING(CO_COMMFUNC_H_00039);// "���ش洢����";
	case ERR_AP_NOBILL:	return RES_STRING(CO_COMMFUNC_H_00040);// "���˺��ѹ��ڣ����ֵ��";

	case ERR_PT_LOGFAIL:	return RES_STRING(CO_COMMFUNC_H_00041);// "GateServer��GroupServer�ĵ�¼ʧ��";
	case ERR_PT_SAMEGATENAME:return RES_STRING(CO_COMMFUNC_H_00042);// "GateServer���ѵ�¼GateServer����";

	case ERR_PT_INVALIDDAT:return RES_STRING(CO_COMMFUNC_H_00043);// "��Ч�����ݸ�ʽ";
	case ERR_PT_INERR: return RES_STRING(CO_COMMFUNC_H_00044);// "������֮��Ĳ��������Դ���";
	case ERR_PT_NETEXCP: return RES_STRING(CO_COMMFUNC_H_00045);// "�ʺŷ���������";	// GroupServer���ֵĵ�AccuntServer���������
	case ERR_PT_DBEXCP: return RES_STRING(CO_COMMFUNC_H_00046);// "���ݿ����������";	// GroupServer���ֵĵ�Database�Ĺ���
	case ERR_PT_INVALIDCHA: return RES_STRING(CO_COMMFUNC_H_00047);// "��ǰ�ʺŲ�ӵ������(ѡ��/ɾ��)�Ľ�ɫ";
	case ERR_PT_TOMAXCHA: return RES_STRING(CO_COMMFUNC_H_00048);// "�Ѿ��ﵽ����ܴ����Ľ�ɫ����";
	case ERR_PT_SAMECHANAME: return RES_STRING(CO_COMMFUNC_H_00049);// "�ظ��Ľ�ɫ��";
	case ERR_PT_INVALIDBIRTH: return RES_STRING(CO_COMMFUNC_H_00050);// "�Ƿ��ĳ�����";
	case ERR_PT_TOOBIGCHANM:return RES_STRING(CO_COMMFUNC_H_00051);// "��ɫ��̫��";
	case ERR_PT_ISGLDLEADER:return RES_STRING(CO_COMMFUNC_H_00052);// "���᲻��һ���޳������Ƚ�ɢ������ɾ����ɫ";
	case ERR_PT_ERRCHANAME:return RES_STRING(CO_COMMFUNC_H_00053);// "�Ƿ��Ľ�ɫ����";
	case ERR_PT_SERVERBUSY: return RES_STRING(CO_COMMFUNC_H_00054);// "ϵͳæ�����Ժ�����";
	case ERR_PT_TOOBIGPW2: return RES_STRING(CO_COMMFUNC_H_00055);// "�������볤�ȷǷ�";
	case ERR_PT_INVALID_PW2: return RES_STRING(CO_COMMFUNC_H_00056);// "δ������ɫ������������";
	case ERR_PT_BADBOY:	return RES_STRING(CO_COMMFUNC_H_00057);// "���ӣ����BT���Ѿ��Զ�����������ͨ��������Ҫ����Ϊ�䣬�����ٷ���";
	
	// Add by lark.li 20080825 begin
	case ERR_PT_BANUSER: return RES_STRING(CO_COMMFUNC_H_00031);// "�ʺ��ѷ�ͣ";
	case ERR_PT_PBANUSER: return RES_STRING(CO_COMMFUNC_H_00108);// "�Ѿ�ʹ�����ܱ���";
	// End

	case ERR_MC_NETEXCP: return RES_STRING(CO_COMMFUNC_H_00058);// "GateServer���ֵ������쳣";
	case ERR_MC_NOTSELCHA: return RES_STRING(CO_COMMFUNC_H_00059);// "��ǰδ����ѡ���ɫ״̬";
	case ERR_MC_NOTPLAY: return RES_STRING(CO_COMMFUNC_H_00060);// "��ǰ����������Ϸ״̬�����ܷ���ENDPLAY����";
	case ERR_MC_NOTARRIVE: return RES_STRING(CO_COMMFUNC_H_00061);// "Ŀ���ͼ���ɵ���";
	case ERR_MC_TOOMANYPLY: return RES_STRING(CO_COMMFUNC_H_00062);// "������������������, ��ѡ�������������Ϸ!";
	case ERR_MC_NOTLOGIN: return RES_STRING(CO_COMMFUNC_H_00063);// "����δ��½";
	case ERR_MC_VER_ERROR: return RES_STRING(CO_COMMFUNC_H_00064);// "�ͻ��˵İ汾�Ŵ���,�������ܾ���¼��";
	case ERR_MC_ENTER_ERROR: return RES_STRING(CO_COMMFUNC_H_00065);// "�����ͼʧ�ܣ�";
	case ERR_MC_ENTER_POS: return RES_STRING(CO_COMMFUNC_H_00066);// "��ͼλ�÷Ƿ����������ͻس������У������µ�½��";
	
	// Add by lark.li 20080825 begin
	case ERR_MC_BANUSER: return RES_STRING(CO_COMMFUNC_H_00031);// "�ʺ��ѷ�ͣ";
	case ERR_MC_PBANUSER: return RES_STRING(CO_COMMFUNC_H_00108);// "�Ѿ�ʹ�����ܱ���";
	// End

	case ERR_TM_OVERNAME: return RES_STRING(CO_COMMFUNC_H_00067);// "GameServer���ظ�";
	case ERR_TM_OVERMAP: return RES_STRING(CO_COMMFUNC_H_00068);// "GameServer�ϵ�ͼ���ظ�";
	case ERR_TM_MAPERR: return RES_STRING(CO_COMMFUNC_H_00069);// "GameServer��ͼ�����﷨����";

	case ERR_SUCCESS:return RES_STRING(CO_COMMFUNC_H_00070);// "Jack̫BT�ˣ���ȷҲ������ʲô����";
	default:
		{
			int l_error_code	=error_code;
			l_error_code	/=500;
			l_error_code	*=500;
			static char l_buffer[500];
			char		l_convt[20];

			_itoa_s(error_code,l_convt, sizeof(l_convt), 10);

			switch ( l_error_code )
			{
			case ERR_MC_BASE:
				{
					//return strcat(strcpy(l_buffer,itoa(error_code,l_convt,10)),RES_STRING(CO_COMMFUNC_H_00071)); //"(GameServer/GateServer->Client���صĴ�����ռ�1��500)");
					strncpy_s(l_buffer, sizeof(l_buffer),l_convt, _TRUNCATE);
					strncat_s(l_buffer, sizeof(l_buffer), RES_STRING(CO_COMMFUNC_H_00071), _TRUNCATE);
				}
			case ERR_PT_BASE: 		
				{
					//return strcat(strcpy(l_buffer,itoa(error_code,l_convt,10)),RES_STRING(CO_COMMFUNC_H_00072)); //"(GroupServer->GateServer���صĴ�����ռ�501��1000)"); 
					strncpy_s(l_buffer, sizeof(l_buffer),l_convt, _TRUNCATE);
					strncat_s(l_buffer, sizeof(l_buffer), RES_STRING(CO_COMMFUNC_H_00072), _TRUNCATE);
				}
			case ERR_AP_BASE: 
				{
				
					//return strcat(strcpy(l_buffer,itoa(error_code,l_convt,10)),RES_STRING(CO_COMMFUNC_H_00073)); //"(AccountServer->GroupServer���صĴ�����ռ�1001��1500)");
					strncpy_s(l_buffer, sizeof(l_buffer),l_convt, _TRUNCATE);
					strncat_s(l_buffer, sizeof(l_buffer), RES_STRING(CO_COMMFUNC_H_00073), _TRUNCATE);
				}
			case ERR_MT_BASE: 
				{
					//return strcat(strcpy(l_buffer,itoa(error_code,l_convt,10)),RES_STRING(CO_COMMFUNC_H_00074)); //"(GameServer->GateServer���صĴ�����ռ�1501��2000)");
					strncpy_s(l_buffer, sizeof(l_buffer),l_convt, _TRUNCATE);
					strncat_s(l_buffer, sizeof(l_buffer), RES_STRING(CO_COMMFUNC_H_00074), _TRUNCATE);
				}
			default:
				{
					//return strcat(strcpy(l_buffer,itoa(error_code,l_convt,10)),RES_STRING(CO_COMMFUNC_H_00075)); //"(Jack̫BT�ˣ�Ū���������Ҷ�����ʶ��)");
					strncpy_s(l_buffer, sizeof(l_buffer),l_convt, _TRUNCATE);
					strncat_s(l_buffer, sizeof(l_buffer), RES_STRING(CO_COMMFUNC_H_00075), _TRUNCATE);
				}
			}
			return l_buffer;
		}
	}
}
inline bool isAlphaNumeric(const char *name, unsigned short len) {
	const unsigned char *l_name = reinterpret_cast<const unsigned char *>(name);
	if (len == 0) {
		return false;
	}
	for (unsigned short i = 0; i < len; i++) {
		if (!l_name[i] || !isalnum(l_name[i])) {
			return false;
		}
	}
	return true;
}

inline bool isNumeric(const char *name, unsigned short len) {
	const unsigned char *l_name = reinterpret_cast<const unsigned char *>(name);
	if (len == 0) {
		return false;
	}
	for (unsigned short i = 0; i < len; i++) {
		if (!l_name[i] || !isdigit(l_name[i])) {
			return false;
		}
	}
	return true;
}


//���������ܰ�������ַ�����GBK˫�ֽں����ַ��������ԡ���������ַ����������Եȡ�
//nameΪֻ�����д�Сд��ĸ���ֺͺ��֣�ȥ��ȫ�ǿո񣩲ŷ���true;
//len����Ϊ�ַ���name�ĳ���=strlen(name),��������βNULL�ַ���
inline bool IsValidName(const char *name,unsigned short len)
{
	const unsigned char *l_name =reinterpret_cast<const unsigned char *>(name);
	bool l_ishan	=false;
	for(unsigned short i=0;i<len;i++)
	{
		if(!l_name[i])
		{
			return false;
		}else if(l_ishan)
		{
			if(l_name[i-1] ==0xA1 && l_name[i] ==0xA1)	//����ȫ�ǿո�
			{
				return false;
			}
			if(l_name[i] >0x3F && l_name[i] <0xFF && l_name[i] !=0x7F)
			{
				l_ishan =false;
			}else
			{
				return false;
			}
		}else if(l_name[i]>0x80 && l_name[i] <0xFF)
		{
			l_ishan	=true;
			// Modify by lark.li 20080927 
		//}else if((l_name[i] >='A' && l_name[i] <='Z') ||(l_name[i] >='a' && l_name[i] <='z') ||(l_name[i] >='0' && l_name[i] <='9'))
			}else if((l_name[i] >='A' && l_name[i] <='Z') ||(l_name[i] >='a' && l_name[i] <='z') ||(l_name[i] >='0' && l_name[i] <='9') || (l_name[i] == '@') || (l_name[i] == '.'))
				// End
		{
			
		}else
		{
			return false;
		}
	}
	return !l_ishan;
}

inline const char* g_GetUseItemFailedInfo(short sErrorID)
{
	switch (sErrorID)
	{
	case	enumITEMOPT_SUCCESS:
		return RES_STRING(CO_COMMFUNC_H_00076);// "���߲����ɹ�";
		break;
	case	enumITEMOPT_ERROR_NONE:
		return RES_STRING(CO_COMMFUNC_H_00077);// "���߲�����";
		break;
	case	enumITEMOPT_ERROR_KBFULL:
		return RES_STRING(CO_COMMFUNC_H_00078);// "����������";
		break;
	case	enumITEMOPT_ERROR_UNUSE:
		return RES_STRING(CO_COMMFUNC_H_00079);// "����ʹ��ʧ��";
		break;
	case	enumITEMOPT_ERROR_UNPICKUP:
		return RES_STRING(CO_COMMFUNC_H_00080);// "���߲���ʰȡ";
		break;
	case	enumITEMOPT_ERROR_UNTHROW:
		return RES_STRING(CO_COMMFUNC_H_00081);// "���߲��ܶ���";
		break;
	case	enumITEMOPT_ERROR_UNDEL:
		return RES_STRING(CO_COMMFUNC_H_00082);// "���߲�������";
		break;
	case	enumITEMOPT_ERROR_KBLOCK:
		return RES_STRING(CO_COMMFUNC_H_00083);// "��������������״̬";
		break;
	case	enumITEMOPT_ERROR_DISTANCE:
		return RES_STRING(CO_COMMFUNC_H_00084);// "����̫Զ";
		break;
	case	enumITEMOPT_ERROR_EQUIPLV:
		return RES_STRING(CO_COMMFUNC_H_00085);// "װ���ȼ�������";
		break;
	case	enumITEMOPT_ERROR_EQUIPJOB:
		return RES_STRING(CO_COMMFUNC_H_00086);// "װ��ְҵ������";
		break;
	case	enumITEMOPT_ERROR_STATE:
		return RES_STRING(CO_COMMFUNC_H_00087);// "��״̬�²��ܲ�������";
		break;
	case	enumITEMOPT_ERROR_PROTECT:
		return RES_STRING(CO_COMMFUNC_H_00088);// "���߱�����";
		break;
	case	enumITEMOPT_ERROR_AREA:
		return RES_STRING(CO_COMMFUNC_H_00089);// "��ͬ����������";
		break;
	case	enumITEMOPT_ERROR_BODY:
		return RES_STRING(CO_COMMFUNC_H_00090);// "���Ͳ�ƥ��";
		break;
	case	enumITEMOPT_ERROR_TYPE:
		return RES_STRING(CO_COMMFUNC_H_00091);// "�˵����޷����";
		break;
	case	enumITEMOPT_ERROR_INVALID:
		return RES_STRING(CO_COMMFUNC_H_00092);// "��Ч�ĵ���";
		break;
	case	enumITEMOPT_ERROR_KBRANGE:
		return RES_STRING(CO_COMMFUNC_H_00093);// "������������Χ";
		break;
		// add by ning.yan 2008-11-11 begin
	case	enumITEMOPT_ERROR_ITEMLOCK:
		return RES_STRING(CO_COMMFUNC_H_00109);// "���ߴ�������״̬"
		break;
		//end
	default:
		return RES_STRING(CO_COMMFUNC_H_00094);// "δ֪�ĵ��߲���ʧ����";
		break;
	}
}

// ��g_IsRightSkillTar(...)�ķ���ֵת��Ϊ�ַ���
inline const char* g_GetCheckSkillTarRetInfo(int nErrorID)
{
	switch (nErrorID)
	{
	case	enumESKILL_SUCCESS:
		return RES_STRING(CO_COMMFUNC_H_00095);// "����ʹ�óɹ�";
		break;
	case	enumESKILL_FAILD_NPC:
		return RES_STRING(CO_COMMFUNC_H_00096);// "Ŀ����NPC";
		break;
	case	enumESKILL_FAILD_NOT_SKILLED:
		return RES_STRING(CO_COMMFUNC_H_00097);// "Ŀ�겻�ܱ�����";
		break;
	case	enumESKILL_FAILD_SAFETY_BELT:
		return RES_STRING(CO_COMMFUNC_H_00098);// "Ŀ���ڰ�ȫ��";
		break;
	case	enumESKILL_FAILD_NOT_LAND:
		return RES_STRING(CO_COMMFUNC_H_00099);// "����½�ؼ���";
		break;
	case	enumESKILL_FAILD_NOT_SEA:
		return RES_STRING(CO_COMMFUNC_H_00100);// "���Ǻ�����";
		break;
	case	enumESKILL_FAILD_ONLY_SELF:
		return RES_STRING(CO_COMMFUNC_H_00101);// "����ֻ�������Լ�";
		break;
	case	enumESKILL_FAILD_ONLY_DIEPLY:
		return RES_STRING(CO_COMMFUNC_H_00102);// "����ֻ���������ʬ��";
		break;
	case	enumESKILL_FAILD_FRIEND:
		return RES_STRING(CO_COMMFUNC_H_00103);// "���ܲ��������ѷ�";
		break;
	case	enumESKILL_FAILD_NOT_FRIEND:
		return RES_STRING(CO_COMMFUNC_H_00104);// "����ֻ�������ѷ�";
		break;
	case	enumESKILL_FAILD_NOT_PALYER:
		return RES_STRING(CO_COMMFUNC_H_00105);// "����ֻ���������";
		break;
	case	enumESKILL_FAILD_NOT_MONS:
		return RES_STRING(CO_COMMFUNC_H_00106);// "����ֻ�����ڹ���";
		break;
	default:
		return RES_STRING(CO_COMMFUNC_H_00107);// "δ֪�ļ���ʹ��ʧ����";
		break;
	}
}

class CTextFilter
{
public:
#define eTableMax 5
	enum eFilterTable{NAME_TABLE=0, DIALOG_TABLE=1, MAX_TABLE=eTableMax};
	/*
	* Warning : Do not use MAX_TABLE enum value, it just use for the maximum limit definition,
	*			If you want to expand this enum table value more than the default number eTableMax(5),
	*			please increase the eTableMax definition
	*/

	CTextFilter();
	~CTextFilter();
	static bool Add(const eFilterTable eTable, const char *szFilterText);
	static bool IsLegalText(const eFilterTable eTable, const string strText);
	static bool Filter(const eFilterTable eTable, string &strText);
	static bool LoadFile(const char *szFileName, const eFilterTable eTable=NAME_TABLE);
	static BYTE* GetNowSign(const eFilterTable eTable);
	static void Clear(); //����ڴ� Add By Waiting 2009-06-17

private:
	static bool ReplaceText(string &strText, const string *pstrFilterText);
	static bool bCheckLegalText(const string &strText, const string *pstrIllegalText);

	static vector<string> m_FilterTable[eTableMax];
	static BYTE m_NowSign[eTableMax][8];
};

#endif // COMMFUNC_H