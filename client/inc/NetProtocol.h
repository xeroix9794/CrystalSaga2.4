#pragma once
#include "GameCommon.h"
#include "Point.h"
#include "Kitbag.h"
#include "SkillStateType.h"
#include "Tools.h"
#include "Shipset.h"

#include "uiAmphitheaterForm.h"

class CActor;
class CCharacter;
class CSceneItem;
struct xShipBuildInfo;
struct BOAT_BERTH_DATA;

#define defMOVE_LIST_POINT_NUM	32
struct SMoveList
{
	POINT	SPos[defMOVE_LIST_POINT_NUM];
	int		nPointNum;
	DWORD   dwState;

	SMoveList() : nPointNum(0), dwState(0){ };
};

#define defMAX_POS_NUM	32

struct stNetMoveInfo // enumACTION_MOVE
{
	DWORD dwAveragePing; // ��λ�����룬ͨ�� NetIF::GetAveragePing() ��ã���ֵ�����ٶȾ���Ԥ�ƶ��ľ���
	POINT pos_buf[defMAX_POS_NUM]; // �ƶ��յ����У�������ǰλ�ã�
	DWORD pos_num; // ��Ч��ĸ���

	stNetMoveInfo() : dwAveragePing(0), pos_num(0) { };
};

struct stNetSkillInfo // enumACTION_SKILL
{
	BYTE	byFightID;
	char	chMove;		// 1��ֱ��ʹ�ü��ܡ�2���ƶ���λ����ʹ�ü���

	long	lSkillID;		// 0����������>0��ħ������

	// ��Ŀ����ʵ�壬��ֱ��ʾWorldID,Handle����Ŀ�������꣬��ֱ��ʾ�����x,y
	struct
	{
		long	lTarInfo1;
		long	lTarInfo2;
	};

	stNetMoveInfo	SMove;

	stNetSkillInfo() : byFightID(0), chMove(0), lSkillID(0), lTarInfo1(0), lTarInfo2(0) { };
};

struct stNetNotiMove // enumACTION_MOVE
{
	short	sState;					// ״̬���μ�CompCommand.h EMoveState����
	short	sStopState;				// ֹͣ״̬enumEXISTS_WAITING��enumEXISTS_SLEEPING
	POINT	SPos[defMAX_POS_NUM];	// �ƶ��յ����У�������ǰλ�ã�
	long	nPointNum;				// ��Ч��ĸ���

	stNetNotiMove() : sState(0), sStopState(0), nPointNum(0) { };
};

struct stEffect
{
	long	lAttrID;	// Ӱ�������ID
	LONG64	lVal;		// Ӱ�����ֵ

	stEffect() : lAttrID(0), lVal(0){ }
};

struct stSkillState
{
	BYTE	chID;
	BYTE	chLv;       // Ϊ��ɾ�����״̬,������������״̬

	stSkillState() : chID(0), chLv(0) { };
};

struct stAreaSkillState
{
	BYTE	chID;
	BYTE	chLv;
	long	lWorldID;	// ʩ���ߵ�Ψһ��ʶ
	unsigned char	uchFightID;

	stAreaSkillState() : chID(0), chLv(0), lWorldID(0), uchFightID(0) { };
};

struct stNetNotiSkillRepresent //���ܱ��� enumACTION_SKILL_SRC
{
	BYTE	byFightID;
	short	sAngle;         // ����serverͨ��
	short	sState;			// ״̬���μ�CompCommand.h EFightState��
	short	sStopState;		// ֹͣ״̬enumEXISTS_WAITING��enumEXISTS_SLEEPING
	char	chCrt;          // 0��û�б�����1�����ڱ���

	long	lSkillID;		// ���ܱ��
	long	lSkillSpeed;	// ����ʹ��Ƶ�ʣ����룩
	char	chObjType;		// Ŀ�����ͣ�0��ʵ�塣1������
	long	lTargetID;		// Ŀ��ID
	POINT	STargetPoint;	// Ŀ���
	short	sExecTime;	    // ���ô���,���ڵ�����

	CSizeArray<stEffect>		SEffect;	// Ӱ��Ľ������
	CSizeArray<stSkillState>	SState;		// ����״̬

	stNetNotiSkillRepresent() : byFightID(0), sAngle(0), sState(0), sStopState(0), chCrt(0), lSkillID(0), lSkillSpeed(0), chObjType(0),
		lTargetID(0), sExecTime(0)
	{

	}
};

struct stNetNotiSkillEffect // ���ܵ����ý�� enumACTION_SKILL_TAR
{
	BYTE	byFightID;
	short	sState;			// ״̬���μ�CompCommand.h EFightState��
	bool	bDoubleAttack;	// ˫������
	bool	bMiss;			// Miss;
	bool	bBeatBack;		// �Ƿ����
	Point	SPos;			// ���˺��λ��
	long	lSkillID;		// ʹ�õļ���ID
	char	chObjType;		// Ŀ�����ͣ�0��ʵ�塣1������
	long	lSrcID;			// ʹ�÷��Ľ�ɫID
	Point	SSrcPos;		// ʹ�÷���λ��
	Point	SSkillTPos;		// �������
	short	sExecTime;		// ���ô���,Ϊһ

	// ����
	CSizeArray<stEffect>		SEffect;	// Ӱ��Ľ������
	CSizeArray<stSkillState>	SState;		// ����״̬

	// ����Դ
	short						sSrcState;	// ״̬���μ�CompCommand.h EFightState��
	CSizeArray<stEffect>		SSrcEffect;	// Ӱ��Ľ������
	CSizeArray<stSkillState>	SSrcState;	// ����״̬

	stNetNotiSkillEffect() : byFightID(0), sState(0), bDoubleAttack(false), bMiss(false), bBeatBack(false),
			lSkillID(0), chObjType(0), lSrcID(0), sExecTime(0), sSrcState(0)
	{

	}
};

struct stNetPKCtrl
{
	bool	bInGymkhana;	// �ھ���������ʱ����������bInPK
	bool	bInPK;			// ��PK

	void	Exec( CCharacter* pCha );
	void	Exec( unsigned long	ulWorldID );

	stNetPKCtrl() : bInGymkhana(false), bInPK(false) { };
};

struct stNetChaSideInfo
{
	char	chSideID;

	stNetChaSideInfo() : chSideID(0) { };
};

struct stNetAppendLook
{
	short	sLookID[defESPE_KBGRID_NUM];
	bool	bValid[defESPE_KBGRID_NUM];
	short	sEquipID[5];
	bool	eValid[5];
	void	Exec(unsigned long	ulWorldID);
	void	Exec(CCharacter* pCha);

	stNetAppendLook()
	{
		memset(sLookID, 0, sizeof(sLookID));
		memset(bValid, 0, sizeof(bValid));
		memset(sEquipID, 0, sizeof(sEquipID));
		memset(eValid, 0, sizeof(eValid));

	}
};

class CSceneNode;
class CEvent;
struct stNetEvent
{
	long			lEntityID;
	char			chEntityType;					// 1-��ɫ,2-����
	unsigned short	usEventID;
	const char*		cszEventName;

	CEvent*			ChangeEvent();					// �ı��¼�����

	CEvent*			Exec( CSceneNode* pNode );		// �����¼�

	stNetEvent() : lEntityID(0), chEntityType(0), usEventID(0), cszEventName(NULL) { };
};

struct stNetLookInfo
{
	char	chSynType;	// �μ� ESynLookType����ֵΪenumSYN_LOOK_CHANGEʱ��ֻ��SLook.sID��ֵ����0���Ÿ��¸�λ��
	stNetChangeChaPart	SLook;

	stNetLookInfo() : chSynType(0) { };
};

struct stNetActorCreate				// ������ɫ����Ϣ
{
	stNetActorCreate() : ulWorldID(0), lHandle(0), ulChaID(0), chCtrlType(0), sAngle(0), ulTLeaderID(0), sState(0)
		, sIcon(0), chSeeType(enumENTITY_SEEN_NEW), szCommName(NULL), strGuildName(""), strMottoName(""), strGuildMotto(""),
		strStallName(""), szName(NULL) {}

	unsigned long	ulWorldID;
	unsigned long	ulCommID;		// ͨ��ID
	const char		*szCommName;	// ͨ�н�ɫ��
	long			lHandle;		// ����������Ϣ��ԭֵ����
	unsigned long	ulChaID;
	char			chCtrlType;		// �������ͣ���ң�NPC������ȡ��ο�CompCommand.h EChaCtrlType��
	Circle			SArea;
	short			sAngle;			// ����serverͨ��
	unsigned long	ulTLeaderID;	// �ӳ�ID��û��Ϊ0
	short			sState;			// ״̬ 0x00�������С�0x01�������С�0x02��������
	char* strMottoName;
	short			sIcon;
	long			lGuildID;
	long			lGuildColor;
	long			lNameColor;
	long			lGuildPerm;
	char* strGuildName;
	char* strGuildMotto;
	char* strStallName;
	char			*szName;
	char			chSeeType;		// ���ֵ�����,EEntitySeenType
	char			chGMLv;			// GM�ȼ�

	stNetChaSideInfo	SSideInfo;	// �ֱ���Ϣ
	stNetLookInfo		SLookInfo;
	stNetPKCtrl		SPKCtrl;
	stNetEvent		SEvent;
	stNetAppendLook	SAppendLook;

	char			chMainCha;		// 0-������ɫ,1-Ϊ����,2-���ǵĵ���

	CCharacter*		CreateCha();
	void			SetValue( CCharacter* pCha );
};

struct stNetNPCShow
{
	BYTE			byNpcType;	 // ��Ұ����ʱ 
    BYTE            byNpcState;  // �����NPC,�򸽴�NPC״̬

	void			SetNpcShow( CCharacter* pCha );

	stNetNPCShow() : byNpcType(0), byNpcState(0) { };
};

struct stNetLeanInfo // �п�
{
	char	chState;

	long	lPose;
	long	lAngle;
	long	lPosX, lPosY;
	long	lHeight;

	stNetLeanInfo() : chState(0), lPose(0), lAngle(0), lPosX(0), lPosY(0), lHeight(0) { };
};

struct stNetSwitchMap // ��ɫ�������Ϣ
{
	short	sEnterRet;
	char	*szMapName;
	char	chEnterType;
	bool	bIsNewCha;
	bool	bCanTeam;	// ��ͼ�Ƿ�ɲ�������

	stNetSwitchMap() : sEnterRet(0), szMapName(NULL), chEnterType(0), bCanTeam(false), bIsNewCha(false) { };
};

// ˵��
struct stNetSysInfo		// ϵͳ��Ϣ����ʾĳ�˲������ϵ�
{
	const char *m_sysinfo;		// ϵͳ��Ϣ����
	stNetSysInfo() : m_sysinfo(NULL) { };
};

struct stNetSay			// ��Ұ��˵��
{
	unsigned long	m_srcid;	// ˵���˵�ID
	const char	*	m_content;	// ����

	stNetSay() : m_srcid(0), m_content(NULL) { };
};

// ʰȡ��������
// 1.�ͻ���ʰȡ������,����ʰȡ���ߵ�������
// 2.������֪ͨ���ͻ���,�����������仯
// 3.������֪ͨ���пͻ���,�����ϵ�����ʧ
// 
// 
// װ������
// 1.�ڵ�����ѡ������϶���װ����,����װ��Э��
//    �ɹ�:1.֪ͨ���пͻ�����۷����仯,ͬʱ���ͻ��˸�����Ӧװ����
//         2.֪ͨ���ͻ��˵����������仯

struct stNetItemCreate	// ��Ʒ���֣���ʧ
{
	long	lWorldID;
	long	lHandle;		// ��������Ϣ��ԭֵ����
	long	lEntityAddr;	// ����˵�ʵ���ַ
	long	lID;
	Point	SPos;
	short	sAngle;
	short	sNum;			// ��Ʒ����
	char	chAppeType;		// �������ͣ��μ�CompCommand.h EItemAppearType��
	long	lFromID;		// �׳�Դ��ID�����Ϊ0���򲻴����׳�Դ

	stNetEvent	SEvent;

	stNetItemCreate() : lWorldID(0), lHandle(0), lEntityAddr(0), lID(0), sAngle(0), sNum(0),chAppeType(0),  lFromID(0) { };
};

struct stNetItemUnfix	// ����жװ
{
	char    chLinkID;	// Link��
	short	sGridID;	// ���ڵ�����,��������Ʒ����λ��,-1,��������Ʒ��,��ָ��λ��,-2,�����ڵ���
    long	lPosX;		// �����ڵ����λ��
    long	lPosY;

	stNetItemUnfix() : chLinkID(0), sGridID(0), lPosX(0), lPosY(0) { };
};

struct stNetItemPick	// ���ߵ�ʰȡ
{
	long	lWorldID;
	long	lHandle;
};

struct stNetItemThrow	// ���ߵĶ���
{
	short	sGridID;	// ��Ʒ��λ��
	long	lNum;		// ������Ʒ�ĸ���
	long	lPosX;		// �����ڵ����λ��
	long	lPosY;
};

struct stNetItemPos		// ���߸ı������е�λ��
{
	short	sSrcGridID;	// ��Ʒ��λ��
	short	sSrcNum;	// Դ��Ʒ������,���Ϊ�㣬����ȫ����Ʒ����
	short	sTarGridID;	// ��Ʒ��λ��
};

struct stNetBank		// ���н���
{
	char	chSrcType;	// Դ������ 0�������� 1������
	short	sSrcID;		// ��Ʒ��λ��
	short	sSrcNum;	// ��Ʒ������
	char	chTarType;	// Ŀ�������
	short	sTarID;		// ��Ʒ��λ��
};

struct stNetTempKitbag	// ��ʱ������������
{
	short   sSrcGridID;	// ��ʱ����λ��
	short	sSrcNum;	// �϶�����
	short	sTarGridID;	// ����λ��
};

struct stNetUseItem		// ʹ�õ���
{
	stNetUseItem() {sTarGridID = -1;}
	short	sGridID;	// ��Ʒ��λ��
	short	sTarGridID;	// Ŀ��λ�ã����ڸ�����ιʳ�������
};

struct stNetDelItem		// ɾ������
{
	short	sGridID;	// ��Ʒ��λ��
};

struct stNetItemInfo	// ������Ϣ
{
	char	chType;
	short	sGridID;	// ��Ʒ��λ��
};

struct stTempChangeChaPart
{
	DWORD			dwPartID;
	DWORD           dwItemID;
};

struct stNetKitbag				    // ��������������
{
	char	chBagType;				// 0�������� 1������
	char	chType;					// �������ͣ��ο�CompCommand.h��ESynKitbagType��
    int     nKeybagNum;             // ������������
    struct stGrid
    {
	    short	    sGridID;		// ���ID
	    SItemGrid	SGridContent;
    };
    int     nGridNum;               // ��Ч�ĸ�����
    stGrid  Grid[defMAX_KBITEM_NUM_PER_TYPE];     
};

struct stNetSkillBag			    // ���¼�����
{
	char			chType;			// �������ͣ��ο�CompCommand.h��ESynSkillBagType��
	CSizeArray<SSkillGridEx>		SBag;	// ��������
};

struct stNetDefaultSkill
{
	short	sSkillID;

	void	Exec(void);
};

struct stNetSkillState				// ���¼���״̬
{
	char	chType;					// �������ͣ�δ���壩
	CSizeArray<stSkillState> SState;
};

struct stNetChangeCha				// ������ɫЭ��
{
	unsigned long	ulMainChaID;	// ����ID
};

struct stNetActivateEvent
{
	long	lTargetID;
	long	lHandle;
	short	sEventID;
};

struct stNetFace                    // �ͻ��˻�����,��ӦenumACTION_FACE
{
    short   sPose;              
    short	sAngle;
};

struct stLookEnergy
{
	short	sEnergy[enumEQUIP_NUM];
};

// �Ի�������Ϣ
typedef struct _NET_FUNCITEM
{
	char szFunc[ROLE_MAXNUM_FUNCITEMSIZE];
} NET_FUNCITEM, *PNET_FUNCITEM;

typedef struct _NET_MISITEM
{
	char szMis[ROLE_MAXNUM_FUNCITEMSIZE];
	BYTE byState;

} NET_MISITEM, *PNET_MISITEM;

typedef struct _NET_FUNCPAGE
{
	char szTalk[ROLE_MAXNUM_DESPSIZE];
	NET_FUNCITEM FuncItem[ROLE_MAXNUM_FUNCITEM];
	NET_MISITEM  MisItem[ROLE_MAXNUM_CAPACITY];

} NET_FUNCPAGE, *PNET_FUNCPAGE;

// �����б���Ϣ
typedef struct _NET_MISSIONLIST
{
	BYTE byListType;
	BYTE byPrev;
	BYTE byNext;
	BYTE byPrevCmd;
	BYTE byNextCmd;
	BYTE byItemCount;
	NET_FUNCPAGE FuncPage;

} NET_MISSIONLIST, *PNET_MISSIONLIST;


#define HELPINFO_DESPSIZE 4096

// ��ʾͼ�Σ����֣���������
typedef struct _NET_HELPINFO
{
	BYTE byType;
	union
	{
		char szDesp[HELPINFO_DESPSIZE];//[ROLE_MAXNUM_DESPSIZE];
		USHORT sID;
	};

} NET_HELPINFO, *PNET_HELPINFO;

// ����ҳ��Ϣ
typedef struct _NET_MISNEED
{
	BYTE byType;
	union
	{
		struct
		{
			WORD wParam1;
			WORD wParam2;
			WORD wParam3;
		};
		char szNeed[ROLE_MAXNUM_NEEDDESPSIZE];
	};

} NET_MISNEED, *PNET_MISNEED;

typedef struct _NET_MISPRIZE
{
	BYTE byType;
	WORD wParam1;
	WORD wParam2;

} NET_MISPRIZE, *PNET_MISPRIZE;

typedef struct _NET_MISPAGE
{
	BYTE byNeedNum;
	NET_MISNEED MisNeed[ROLE_MAXNUM_MISNEED];

	BYTE byPrizeSelType;
	BYTE byPrizeNum;
	NET_MISPRIZE MisPrize[ROLE_MAXNUM_MISPRIZE];

	// 
	char szName[ROLE_MAXSIZE_MISNAME];
	char szDesp[ROLE_MAXNUM_DESPSIZE];

} NET_MISPAGE, *PNET_MISPAGE;

typedef struct _NET_MISLOG
{
	BYTE byType;
	BYTE byState;
	WORD wMisID;

} NET_MISLOG, *PNET_MISLOG;

typedef struct _NET_MISLOG_LIST
{
	BYTE	   byNumLog;
	NET_MISLOG MisLog[ROLE_MAXNUM_MISSION];

} NET_MISLOG_LIST, *PNET_MISLOG_LIST;

// ���״�����Ϣ
typedef struct _NET_TRADEPAGE
{
	BYTE   byCount;
	USHORT sItemID[ROLE_MAXNUM_TRADEITEM];
	USHORT sCount[ROLE_MAXNUM_TRADEITEM];
	DWORD  dwPrice[ROLE_MAXNUM_TRADEITEM];
	BYTE   byLevel[ROLE_MAXNUM_TRADEITEM];

} NET_TRADEPAGE, *PNET_TRADEPAGE;

typedef struct _NET_TRADEINFO
{
	NET_TRADEPAGE TradePage[mission::MAXTRADE_ITEMTYPE];

	_NET_TRADEINFO()
	{
		Clear();
	}

	void Clear()
	{
		memset( TradePage, 0, sizeof(NET_TRADEPAGE)*mission::MAXTRADE_ITEMTYPE );
	}

} NET_TRADEINFO, *PNETTRADEINFO;

struct NET_CHARTRADE_BOATDATA
{
	char szName[BOAT_MAXSIZE_BOATNAME];
	USHORT sBoatID;
	USHORT sLevel;
	DWORD dwExp;
	DWORD dwHp;
	DWORD dwMaxHp;
	DWORD dwSp;
	DWORD dwMaxSp;
	DWORD dwMinAttack;
	DWORD dwMaxAttack;
	DWORD dwDef;
	DWORD dwSpeed;
	DWORD dwShootSpeed;
	BYTE  byHasItem;
	BYTE  byCapacity;
	DWORD dwPrice;

};

// ��ɫ������Ʒʵ����Ϣ�ṹ
typedef struct _NET_CHARTRADE_ITEMDATA
{
	BYTE byForgeLv;
	BYTE byHasAttr;

	short sInstAttr[defITEM_INSTANCE_ATTR_NUM][2];	// ʵ������
	short sEndure[2];	// �;öȣ���ǰֵ/���ֵ��
	short sEnergy[2];	// ��������ǰֵ/���ֵ��

	long	lDBParam[enumITEMDBP_MAXNUM];	// ���ݿ����
	bool	bValid;

} NET_CHARTRADE_ITEMDATA, *PNET_CHARTRADE_ITEMDATA;

// ������ս��Ϣ�ṹ
#define MAX_GUILD_CHALLLEVEL				3	// ����������ǰ��
typedef struct _NET_GUILD_CHALLINFO
{
	BYTE byIsLeader;
	BYTE byLevel[MAX_GUILD_CHALLLEVEL];
	BYTE byStart[MAX_GUILD_CHALLLEVEL];
	char szGuild[MAX_GUILD_CHALLLEVEL][64];
	char szChall[MAX_GUILD_CHALLLEVEL][64];
	DWORD dwMoney[MAX_GUILD_CHALLLEVEL];

} NET_GUILD_CHALLINFO, *PNET_GUILD_CHALLINFO;
struct NetChaBehave
{
	const char	*	sCharName;			//��ɫ��
	const char	*	sJob;				//ְҵ
	short			iDegree;			//��ɫ�ȼ�
	const LOOK	*	sLook;				//�������
};

struct stNetShortCutChange
{
	char     chIndex;
	char     chType;
	short    shyGrid;
};

struct stNetNpcMission
{
    BYTE    byType;     // ����
    USHORT  sNum;		// ��Ҫ�ݻ����������
    USHORT  sID;	    // ���ݻ��������ID
    USHORT  sCount;     // ����ɼ���    
};

struct stNetAreaState
{
	short				sAreaX;			// ��
	short				sAreaY;
	char				chStateNum;
	stAreaSkillState	State[AREA_STATE_NUM];
};

struct stNetChaAttr
{
	char		chType;
	short		sNum;
	stEffect	SEff[MAX_ATTR_CLIENT];
};

struct stNetQueryRelive
{
	char		chType;	// ͬCompCommand��EPlayerReliveType
	const char	*szSrcChaName;
};

// ������Ҫ���������
struct stNetOpenHair
{
	void Exec();
};

// �����������
struct stNetUpdateHair
{
	short	sScriptID;			// ��Ӧ��Hair�ű�ID,���Ϊ0�����ر�������
	short	sGridLoc[4];		// ��Ʒ���ڵĸ���
};

// �������͵ķ���ֵ
struct stNetUpdateHairRes
{
	unsigned long	ulWorldID;			// ��Ӧ��ɫ
	int				nScriptID;			// ������Ľű�ID
	const char*		szReason;			// ����ʱ��ԭ��,�����ɹ�Ϊ:ok,����Ϊ��ķ���Ϊ��fail,����ʧ��Ϊ:����ԭ��

	void	Exec();
};

// �Է��յ��Ķ�����ս����
struct stNetTeamFightAsk
{
	struct
	{
		const char*	szName;
		const char*	szJob;
		char		chLv;
		unsigned short usVictoryNum;
		unsigned short usFightNum;
	}Info[10];
	char	chSideNum1;
	char	chSideNum2;

	void	Exec();
};

struct stNetItemRepairAsk
{
	const char	*cszItemName;
	long	lRepairMoney;

	void	Exec();
};

struct stSCNetItemForgeAsk
{
	char	chType;
	long	lMoney;

	void	Exec();
};

struct stNetItemForgeAnswer
{
	long	lChaID;
	char	chType;
	char	chResult;
	void	Exec();
};

// Add by lark.li 20080516 begin
struct stSCNetItemLotteryAsk
{
	long	lMoney;

	void	Exec();
};

struct stNetItemLotteryAnswer
{
	long	lChaID;
	char	chResult;
	void	Exec();
};
// End

#define	defMAX_FORGE_GROUP_NUM	defMAX_ITEM_FORGE_GROUP

struct SForgeCell
{
	SForgeCell() : sCellNum(0), pCell(0) {}
	~SForgeCell() 
	{
		delete[] pCell;
	}

	short	sCellNum;
	struct  SCell
	{
		short	sPosID;
		short	sNum;
	} *pCell;
};

struct stNetItemForgeAsk
{
	char	chType;	// 1��������2���ϳ�
	SForgeCell	SGroup[defMAX_FORGE_GROUP_NUM];
};

struct SLotteryCell
{
	SLotteryCell() : sCellNum(0), pCell(0) {}
	~SLotteryCell() 
	{
		delete[] pCell;
	}

	short	sCellNum;
	struct  SCell
	{
		short	sPosID;
		short	sNum;
	} *pCell;
};

#define	defMAX_LOTTERY_GROUP_NUM	7

struct stNetItemLotteryAsk
{
	SLotteryCell	SGroup[defMAX_LOTTERY_GROUP_NUM];
};

struct stNetEspeItem
{
	char	chNum;
	struct
	{
		short	sPos;
		short	sEndure;
		short	sEnergy;
	} SContent[4];
};

// �������˽ṹ��
struct stBlackTrade
{
	short sIndex;		// 
	short sSrcID;		// ������ƷID
	short sSrcNum;		// ������Ʒ����
	short sTarID;		// Ŀ����ƷID
	short sTarNum;		// Ŀ����Ʒ����
	short sTimeNum;		// timeֵ
};


struct stChurchChallenge
{
	short sChurchID;	// id
	char  szName[32];	// name
	char  szChaName[32];// ��ǰ������
	short sCount;		// ����
	long  nBasePrice;	// �׼�
	long  nMinbid;		// ��ͳ���
	long  nCurPrice;	// ��ǰ���ļ�

	stChurchChallenge()	{ memset(this, 0, sizeof(stChurchChallenge)); }
};

// Add by lark.li 20080807 
struct stPersonInfo
{
	char szMotto[40];				// ǩ��
	bool bShowMotto;				// ��ʾǩ������
	char szSex[10];					// �Ա�
	short sAge;						// ����
	char szName[50];				// ����
	char szAnimalZodiac[50];		// ����
	char szBloodType[50];			// Ѫ��
	int  iBirthday;					// ���գ���
	char szState[50];				// �ݣ�ʡ��
	char szCity[50];				// ���У�����
	char szConstellation[50];		// ����
	char szCareer[50];				// ְҵ
	int iSize;						// ͷ���С([8 * 1024])
	char pAvatar[8096];					// ͷ��
	bool bPprevent;					// �Ƿ���ֹ��Ϣ
	int iSupport;					// �ʻ���
	int iOppose;					// ��������

	stPersonInfo&  operator =(const stPersonInfo & _info)
	{
		//	strcpy(str1,str2) was modified to strncpy_s(str1,str2)
		strncpy_s(szMotto,sizeof(szMotto),_info.szMotto,_TRUNCATE);
		bShowMotto = _info.bShowMotto;
		strncpy_s(szSex,sizeof(szSex),_info.szSex,_TRUNCATE);
		sAge = _info.sAge;
		strncpy_s(szName,sizeof(szName),_info.szName,_TRUNCATE);
		strncpy_s(szAnimalZodiac,sizeof(szAnimalZodiac),_info.szAnimalZodiac,_TRUNCATE);
		strncpy_s(szBloodType,sizeof(szBloodType),_info.szBloodType,_TRUNCATE);
		iBirthday = _info.iBirthday;
		strncpy_s(szState,sizeof(szState),_info.szState,_TRUNCATE);
		strncpy_s(szCity,sizeof(szCity),_info.szCity,_TRUNCATE);
		strncpy_s(szConstellation,sizeof(szConstellation),_info.szConstellation,_TRUNCATE);
		strncpy_s(szCareer,sizeof(szCareer),_info.szCareer,_TRUNCATE);
		iSize = _info.iSize;
		strncpy_s(pAvatar,sizeof(pAvatar),_info.pAvatar,_TRUNCATE);
		bPprevent = _info.bPprevent;
		iSupport = _info.iSupport;
		iOppose = _info.iOppose;
		return *this;
	}
	stPersonInfo()
	{
		memset(this, 0, sizeof(stPersonInfo));
		/*memset(szMotto, 0, sizeof(szMotto));
		bShowMotto = false;
		memset(szSex, 0, sizeof(szSex);
		sAge = 0;
		memset(szName, 0, sizeof(szName));
		memset(szAnimalZodiac, 0, sizeof(szAnimalZodiac));
		memset(szBloodType, 0, sizeof(szBloodType));
		memset(szBirthday, 0, sizeof(szBirthday));
		memset(szState, 0, sizeof(szState));
		memset(szCity, 0, sizeof(szCity));
		memset(szConstellation, 0, sizeof(szConstellation));
		memset(szCareer, 0, sizeof(szCareer));
		iSize = 0;
		memset(szAvatar, 0, sizeof(szAvatar));
		bPprevent = false;
		iSupport = 0;
		iOppose = 0;*/
	}
};

struct stQueryPersonInfo
{
	char	sChaName[64];			// ����
	bool	bHavePic;				// �Ƿ���ʾͷ��
	char	cSex[4];				// �Ա�	  1���� 0��Ů��
	int		nMinAge[2];				// �����ѯ��m_nMinAge[1]����m_nMinAge[0]��Сֵ
	char	szAnimalZodiac[4];		// ��Ф
	int		iBirthday[2];			// ��ѯ�������m_nBirth[1]����m_nBirth[0]��Сֵ
									// 0xffff0000&month   0xffff&day
	char	szState[32];			// ʡ��
	char	szCity[32];				// ����
	char	szConstellation[16];	// ����		
	char	szCareer[32];			// ����
	int		nPageItemNum;			// һҳ��ʾ�ĸ���
	int		nCurPage;				// ��ǰҳ��
	
	stQueryPersonInfo()
	{
		memset(this, 0, sizeof(stQueryPersonInfo));
	}

	stQueryPersonInfo& operator =(const stQueryPersonInfo& _info)
	{
		//	strcpy was modified to strncpy_s
		strncpy_s(sChaName,sizeof(sChaName),_info.sChaName,_TRUNCATE);
		bHavePic = _info.bHavePic;
		strncpy_s(cSex,sizeof(cSex),_info.cSex,_TRUNCATE);
		nMinAge[0] = _info.nMinAge[0];
		nMinAge[1] = _info.nMinAge[1];
		strncpy_s(szAnimalZodiac,sizeof(szAnimalZodiac),_info.szAnimalZodiac,_TRUNCATE);
		iBirthday[0] = _info.iBirthday[0];
		iBirthday[1] = _info.iBirthday[1];
		strncpy_s(szState,sizeof(szState),_info.szState,_TRUNCATE);
		strncpy_s(szCity,sizeof(szCity),_info.szCity,_TRUNCATE);
		strncpy_s(szConstellation,sizeof(szConstellation),_info.szConstellation,_TRUNCATE);
		strncpy_s(szCareer,sizeof(szCareer),_info.szCareer,_TRUNCATE);
		nCurPage = _info.nCurPage;
		return *this;
	}
};
struct stQueryResoultPersonInfo
{
	char	sChaName[64];		// ����
	int		nMinAge;			// ����
	char	cSex[4];			// �Ա�	  1���� 0��Ů��
	char	szState[32];		// ʡ��
	char	nCity[32];			// ����
	char	pAvatar[8096];		// ͷ��ͼƬ

	
	stQueryResoultPersonInfo()
	{
		memset(this, 0, sizeof(stQueryResoultPersonInfo));
	}

	stQueryResoultPersonInfo& operator =(const stQueryResoultPersonInfo& _info)
	{
		//	strcpy was modified to strncpy_s for safe
		strncpy_s(sChaName,sizeof(sChaName),_info.sChaName,_TRUNCATE);
		nMinAge = _info.nMinAge;
		strncpy_s(cSex,sizeof(cSex),_info.cSex,_TRUNCATE);
		strncpy_s(szState,sizeof(szState),_info.szState,_TRUNCATE);
		strncpy_s(nCity,sizeof(nCity),_info.nCity,_TRUNCATE);
		strncpy_s(pAvatar,sizeof(pAvatar),_info.pAvatar,_TRUNCATE);
		return   *this;
	}
};

// End

extern void	NetLoginSuccess(char byPassword, char iCharNum,NetChaBehave chabehave[]);//����Key//��ɫ��Ŀ//��ɫ��Ϣ
extern void NetLoginFailure(unsigned short Errno);	                //��½������Ϣ
extern void	NetBeginPlay(unsigned short Errno);
extern void	NetEndPlay(char iCharNum,NetChaBehave chabehave[]);
extern void NetNewCha(unsigned short Errno);
extern void NetDelCha(unsigned short Errno);
extern void NetCreatePassword2(unsigned short Errno);
extern void NetUpdatePassword2(unsigned short Errno);

extern void NetActorDestroy(unsigned int nID, char chSeeType);
extern void NetActorMove(unsigned int nID, stNetNotiMove &list);
extern void NetActorSkillRep(unsigned int nID, stNetNotiSkillRepresent &skill);
extern void NetActorSkillEff(unsigned int nID, stNetNotiSkillEffect &skill);
extern void NetActorLean(unsigned int nID, stNetLeanInfo &lean);
extern void NetSwitchMap(stNetSwitchMap	&switchmap);
extern void NetSysInfo(stNetSysInfo &sysinfo);
extern void NetSideInfo(const char szName[], const char szInfo[]);
extern void NetBickerInfo( const char szData[] );
extern void NetSay(stNetSay &netsay);
extern CSceneItem* NetCreateItem(stNetItemCreate &pSCreateInfo);
extern void NetItemDisappear(unsigned int nID);
extern void NetChangeChaPart( CCharacter* pCha, stNetLookInfo &SLookInfo );
extern void NetChangeChaPart( unsigned int nID, stNetLookInfo &SLookInfo );
extern void NetTempChangeChaPart( unsigned int nID, stTempChangeChaPart &SPart );
extern void NetActorChangeCha(unsigned int nID, stNetChangeCha &SChangeCha);
extern void NetShowTalk( const char szTalk[], BYTE byCmd, DWORD dwNpcID );
extern void NetShowHelp( const NET_HELPINFO& Info );
extern void NetShowMapCrash(const char szTalk[]);
extern void NetShowFunction( BYTE byFuncPage, BYTE byItemNum, BYTE byMisNum, const NET_FUNCPAGE& FuncArray, DWORD dwNpcID );
extern void NetShowMissionList( DWORD dwNpcID, const NET_MISSIONLIST& MisList );
extern void NetShowMisPage( DWORD dwNpcID, BYTE byCmd, const NET_MISPAGE& page );
extern void	NetMisLogList( NET_MISLOG_LIST& List );
extern void NetShowMisLog( WORD wMisID, const NET_MISPAGE& page );
extern void NetMisLogClear( WORD wMisID );
extern void NetMisLogAdd( WORD wMisID, BYTE byState );
extern void NetMisLogState( WORD wID, BYTE byState );
extern void NetCloseTalk( DWORD dwNpcID );
extern void NetCreateBoat( const xShipBuildInfo& Info );
extern void NetUpdateBoat( const xShipBuildInfo& Info );
extern void NetBoatInfo( DWORD dwBoatID, const char szName[], const xShipBuildInfo& Info );
extern void NetShowBoatList( DWORD dwNpcID, BYTE byNumBoat,  const BOAT_BERTH_DATA& Data, BYTE byType = mission::BERTH_LUANCH_LIST );
extern void NetChangeChaLookEnergy( unsigned int nID, stLookEnergy &SLookEnergy );
extern void NetQueryRelive(unsigned int nID, stNetQueryRelive &SQueryRelive);
extern void NetPreMoveTime(unsigned long ulTime);
extern void NetMapMask(unsigned int nID, BYTE *pMask, long lLen);

// ��ʾnpc������Ʒ����
extern void NetShowTrade( const NET_TRADEINFO& TradeInfo, BYTE byCmd, DWORD dwNpcID, DWORD dwParam );
extern void NetUpdateTradeAllData( const NET_TRADEINFO& TradeInfo, BYTE byCmd, DWORD dwNpcID, DWORD dwParam );
extern void NetUpdateTradeData( DWORD dwNpcID, BYTE byPage, BYTE byIndex, USHORT sItemID, USHORT sCount, DWORD dwPrice );

// ��npc���׽��
extern void NetTradeResult( BYTE byCmd, BYTE byIndex, BYTE byCount, USHORT sItemID, DWORD dwMoney );

// ��ʾ��ɫ��������
extern void NetShowCharTradeRequest( BYTE byType, DWORD dwRequestID );

// ��ʼ���н�ɫ����
extern void NetShowCharTradeInfo( BYTE byType, DWORD dwAcceptID, DWORD dwRequestID );

// ȡ����ɫ����
extern void NetCancelCharTrade( DWORD dwCharID );

// ��ɫ����ȷ�Ͻ���������Ϣ
extern void NetValidateTradeData( DWORD dwCharID );

// ��ɫ����ȷ�Ͻ���
extern void NetValidateTrade( DWORD dwCharID );

// ��ɫ�����϶�����֤����Ʒ
extern void NetTradeAddBoat( DWORD dwCharID, BYTE byOpType, USHORT sItemID, BYTE byIndex, 
							BYTE byCount, BYTE byItemIndex, const NET_CHARTRADE_BOATDATA& Data );

// ��ɫ�����϶���Ʒ
extern void NetTradeAddItem( DWORD dwCharID, BYTE byOpType, USHORT sItemID, BYTE byIndex, 
							BYTE byCount, BYTE byItemIndex, const NET_CHARTRADE_ITEMDATA& Data );

// ��ɫ���׽�����ʾ��Ǯ����
extern void NetTradeShowMoney( DWORD dwCharID, DWORD dwMoney );

// ��ɫ���׽��
extern void NetTradeSuccess();
extern void NetTradeFailed();

// ��̯
extern void NetStallInfo( DWORD dwCharID, BYTE byNum, const char szName[] );
extern void NetStallAddBoat( BYTE byGrid, USHORT sItemID, BYTE byCount, DWORD dwMoney, NET_CHARTRADE_BOATDATA& Data );
extern void NetStallAddItem( BYTE byGrid, USHORT sItemID, BYTE byCount, DWORD dwMoney, NET_CHARTRADE_ITEMDATA& Data );
extern void NetStallDelGoods( DWORD dwCharID, BYTE byGrid, BYTE byCount );
extern void NetStallClose( DWORD dwCharID );
extern void NetStallSuccess( DWORD dwCharID );
extern void NetStallName(DWORD dwCharID, const char *szStallName);
extern void NetStartExit( DWORD dwExitTime );
extern void NetCancelExit();
extern void NetSynAttr( DWORD dwWorldID, char chType, short sNum, stEffect *pEffect );
extern void NetFace(DWORD dwCharID, stNetFace& netface, char chType);
extern void NetChangeKitbag(DWORD dwChaID, stNetKitbag& SKitbag);
extern void NetNpcStateChange( DWORD dwChaID, BYTE byState );
extern void NetEntityStateChange( DWORD dwEntityID, BYTE byState );
extern void NetShortCut( DWORD dwChaID, stNetShortCut& stShortCut );  
extern void NetTriggerAction( stNetNpcMission& info );
extern void NetShowForge();

extern void NetShowLottery();	//Add by lark.li 20080514
extern void NetShowAmphitheater(AmphitheaterData & data);	//Add by lark.li 20080514

extern void NetAmphitheaterAnswer(AmphitheaterData& data);//Add by sunny.sun20080726

extern void NetShowUnite();
extern void NetShowMilling();
extern void NetShowFusion();
extern void NetShowUpgrade();
extern void NetShowEidolonMetempsychosis();
extern void NetShowEidolonFusion();
extern void NetShowPurify();
extern void NetShowGetStone();
extern void NetShowRepairOven();
extern void NetShowEnergy();
extern void NetShowTiger();

extern void NetSyncChar(char type, unsigned long cID, unsigned long colorID);
extern void NetSynSkillBag(DWORD dwCharID, stNetSkillBag *pSSkillBag);
extern void NetSynSkillState(DWORD dwCharID, stNetSkillState *pSSkillState);
extern void NetAreaStateBeginSee(stNetAreaState *pState);
extern void NetAreaStateEndSee(stNetAreaState *pState);
extern void NetFailedAction( char chState );
extern void NetShowMessage( long lMes );
extern void NetChaTLeaderID(long lID, long lLeaderID);
extern void NetChaEmotion( long lID, short sEmotion );

extern void NetChaSideInfo( long lID, stNetChaSideInfo &SNetSideInfo );
extern void NetBeginRepairItem(void);

extern void NetItemUseSuccess(unsigned int nID, short sItemID);
extern void NetKitbagCapacity(unsigned int nID, short sKbCap);
extern void NetEspeItem(unsigned int nID, stNetEspeItem& SEspeItem);

extern void NetKitbagCheckAnswer(bool bLock);
extern void NetChaPlayEffect(unsigned int uiWorldID, int nEffectID);

extern void NetChurchChallenge(const stChurchChallenge* pInfo);

extern void NetClose(); //�������磬�ͷ��ڴ� by Waiting 2009-06-18