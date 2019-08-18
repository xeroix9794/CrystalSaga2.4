/* Generated by Together */

#ifndef SUBMAP_H
#define SUBMAP_H
#include "GameAppNet.h"
#include <tchar.h>
#include <stdio.h>
#include "EntitySpawn.h"
#include "SwitchMapRecord.h"
#include "CompCommand.h"
#include "Item.h"
#include "Range.h"
#include "EyeshotCell.h"
#include "StateCell.h"
#include "Weather.h"
#include "GameApp.h"
#include "MapRes.h"

#define defRANGE_MGRUNIU_NUM	10000

class MgrUnit;
class CActiveMgrUnitL;
class Entity;
class CFightAble;

// 地图外角色队列，当怪物死亡后加入到此列表，重生后离开此列表
class	COutMapCha
{
public:
	struct SMgrUnit
	{
		bool	IsValidCha(void) {if (!pCCha) return false; return pCCha->GetID() == ulChaID;}

		dbc::Long	lLeftTick1;	// 剩余时间
		dbc::Long	lLeftTick2;	// 剩余时间
		dbc::Char	chAction;	// 定时的行动
		CCharacter	*pCCha;
		dbc::uLong	ulChaID;
		SSwitchMapInfo	SwitchInfo;
		dbc::Char	chStep;

		SMgrUnit	*pSNext;
	};

	COutMapCha(unsigned short usFreq = 2000);
	~COutMapCha();

	void	Add(CCharacter *pCObj, dbc::uLong ulChaID, SSwitchMapInfo *pSwitchInfo, dbc::Char chAction, dbc::Long lLeftTick1, dbc::Long lLeftTick2);
	void	Run(unsigned long ulCurTick);
	void	Drop();
	void	ExecTimeCha(SMgrUnit *pChaInfo);

private:
	unsigned long	m_ulTick;
	unsigned short	m_usFreq;	// 执行心跳（频率）

	SMgrUnit	*m_pSExecQueue;	// 执行队列
	SMgrUnit	*m_pSFreeQueue;	// 空闲队列

};

// 地图副本类
class SubMap
{
public:
	SubMap();
	virtual ~SubMap();

	bool	Init(CMapRes *pCMapRes, dbc::Short sCopyNO = 0);
	bool	IsRun(void) {return m_bIsRun;}

	const char*  GetName(void) {return m_pCMapRes->GetName();}

	//pt返回在管理单元中的坐标.
	Rect   GetEyeshot(Point &pt)const;
	Rect   GetEyeshot(short sMgrUnitX, short sMgrUnitY)const;
	void   GetEyeshotCenter(Point &pt);
	Rect   GetHoldStateCell(Point &pt, dbc::Long lRadius) const;
	void   GetHoldStateCellCenter(Point &pt);

	void   Add(Entity * ent);
	void   Delete(Entity * ent);
	void   MoveInStateCell(CCharacter* pCCha, const Point &SSrcPos, const Point &STarPos);
	void   MoveInMapMask(CCharacter* pCCha, const Point &SSrcPos, const Point &STarPos);

	bool EnsurePos(Square* pSEntShape, Entity *ent, dbc::cLong clSearchRadius = 40 * 100);
	bool Enter(Square* pSEntShape, Entity *ent, dbc::cLong clSearchRadius = 40 * 100);
	void GoOut(Entity *ent);
	void MoveTo(Entity *pCEnt, const Point &STar);
	void RefreshEyeshot(Entity *pCEnt, bool bEyeshot, bool bHide, bool bShow);

	bool IsValidPos(dbc::Long lPosX, dbc::Long lPosY)
	{
		const Rect	&MapRange = GetRange();
		if (lPosX < MapRange.ltop.x || lPosX >= MapRange.rbtm.x
			|| lPosY < MapRange.ltop.y || lPosY >= MapRange.rbtm.y)
			return false;
		return true;
	}
	dbc::uShort	GetAreaAttr(dbc::Long lPosX, dbc::Long lPosY)
	{
		dbc::Short	sUnitWidth, sUnitHeight;
		dbc::Short	sUnitX, sUnitY;
		dbc::uShort	usAreaAttr;

		m_pCMapRes->m_CTerrain.GetUnitSize(&sUnitWidth, &sUnitHeight);
		sUnitX = Short(lPosX / sUnitWidth);
		sUnitY = Short(lPosY / sUnitHeight);
		m_pCMapRes->m_CTerrain.GetUnitAttr(sUnitX, sUnitY, usAreaAttr);

		return usAreaAttr;
	}
	dbc::uShort	GetAreaAttr(const Point &Pos)
	{
		return GetAreaAttr(Pos.x, Pos.y);
	}

	void NotiStateCellToEyeshot(dbc::Short sCellX, dbc::Short sCellY);

	// 在范围内搜索实体（依赖状态单元进行搜索，即搜索不到道具类）
	void		BeginSearchInRange(dbc::Long *plRangeBParam, dbc::Long *plRangeEParam, bool bIncludeHideEnti = false);
	CCharacter*	GetNextCharacterInRange(void);

	// 向范围增加技能状态
	bool		RangeAddState(dbc::uChar uchFightID, dbc::uLong ulSrcWorldID, dbc::Long lSrcHandle,
				dbc::Char chObjType, dbc::Char chObjHabitat, dbc::Char chEffType, dbc::Short *sStateParam);
	bool		RangeAddState(Rect *pSRange, dbc::uChar uchFightID, dbc::uLong ulSrcWorldID, dbc::Long lSrcHandle,
				dbc::Char chObjType, dbc::Char chObjHabitat, dbc::Char chEffType, dbc::Short *sStateParam);

	// 搜寻实体指针
	CCharacter*	FindCharacter( dbc::uLong ulID, const Point& point );
	// 检测位置是否可以通过
	bool		IsMoveAble(Entity *pCEnt, dbc::Long lPosX, dbc::Long lPosY);

	void		LoadMonsterInfo(void);

	BOOL		LoadNpc();

	BOOL		LoadEventEntity();

	CNpcRecord* GetNpcInfo( USHORT sNpcID );

	CItem*		ItemSpawn(const SItemGrid *pItemInfo, dbc::Long lPosX, dbc::Long lPosY, dbc::Char chSpawnType,
		dbc::Long lFromEntityID = 0, dbc::Long lProtChaID = 0, dbc::Long lProtChaHandle = 0, dbc::Long lProtTime = 0, dbc::Long lOnTick = 0,
		CEvent	*pCEvent = NULL);
	CCharacter*	ChaSpawn(dbc::Long lChaInfoID, dbc::Char chCtrlType, dbc::Short sAngle, Point *pSPos, bool bEyeshotAbility = false,
		dbc::cChar *cszChaName = 0, const long clSearchRadius = 120 * 100);

	void		EntryOpen(void);
	void		EntryClose(void);
	void		Open(void);
	void		Run(DWORD dwCurTime);
	void		Close(void);
	void		ClearSurfaceState(void);
	void		ResetNotPlyCha(void);
	void		ClearPlayerCha(void);
	void		ClearAllCha(void);
	void		ClearAllMonster(void);
	void		ClearAllMonsterByName(const char* szMonsName);
	void		Notice( const char *szString );

	dbc::Short	GetEyeshotCellWidth(void) const {return m_pCMapRes->m_csEyeshotCellWidth;}
	dbc::Short	GetEyeshotCellHeight(void) const {return m_pCMapRes->m_csEyeshotCellHeight;}
	dbc::Short	GetEyeshotCellLin(void) const {return m_pCMapRes->m_sEyeshotCellLin;}
	dbc::Short	GetEyeshotCellCol(void) const {return m_pCMapRes->m_sEyeshotCellCol;}
	dbc::Short	GetStateCellWidth(void) const {return m_pCMapRes->m_csStateCellWidth;}
	dbc::Short	GetStateCellHeight(void) const {return m_pCMapRes->m_csStateCellHeight;}
	dbc::Short	GetStateCellLin(void) const {return m_pCMapRes->m_sStateCellLin;}
	dbc::Short	GetStateCellCol(void) const {return m_pCMapRes->m_sStateCellCol;}
	dbc::Short	GetBlockCellWidth(void) const {return m_pCMapRes->m_csBlockUnitWidth;}
	dbc::Short	GetBlockCellHeight(void) const {return m_pCMapRes->m_csBlockUnitHeight;}
	bool		GetTerrainCellSize(dbc::Short *psWidth, dbc::Short *psHeight) {return m_pCMapRes->m_CTerrain.GetUnitSize(psWidth, psHeight);}
	bool		GetTerrainCellAttr(dbc::Short sUnitX, dbc::Short sUnitY, dbc::uShort &usAttribute) {return m_pCMapRes->m_CTerrain.GetUnitAttr(sUnitX, sUnitY, usAttribute);}
	bool		GetTerrainCellIsland(dbc::Short sUnitX, dbc::Short sUnitY, dbc::uChar &uchIsland) {return m_pCMapRes->m_CTerrain.GetUnitIsland(sUnitX, sUnitY, uchIsland);}
	BYTE		IsBlock(dbc::Long lCellX, dbc::Long lCellY) {return m_pCMapRes->m_CBlock.IsBlock(lCellX, lCellY);}
	const Rect&	GetRange(void) const {return m_pCMapRes->GetRange();}
	BYTE		GetMapID() {return m_pCMapRes->GetMapID();}
	dbc::Short	GetEyeshotWidth(void) const {return m_pCMapRes->m_eyeshotwidth;}
	bool		CanPK(void) {return m_pCMapRes->CanPK();}
	bool		CanSavePos(void) {return m_pCMapRes->CanSavePos();}
	void		BeforePlyOutMap(CCharacter *pCCha);
	void		AfterPlyEnterMap(CCharacter *pCCha);
	dbc::Long	GetMonsterNum(void) {return m_pCMapRes->m_pCMonsterSpawn->GetChaCount();}
	CMapRes*	GetMapRes(void) {return m_pCMapRes;}
	void		SetCopyNO(dbc::Short sCopyNO) {m_sCopyNO = sCopyNO;}
	dbc::Short	GetCopyNO(void) {return m_sCopyNO;}
	dbc::Long	GetPlayerNum(void) {return m_lPlayerNum;}
	dbc::Long	GetInfoParam(dbc::Char chParamID) {if (chParamID < 0 || chParamID >= defMAPCOPY_INFO_PARAM_NUM) return 0; return m_lInfoParam[chParamID];}
	bool		SetInfoParam(dbc::Char chParamID, dbc::Long lParamVal) {if (chParamID < 0 || chParamID >= defMAPCOPY_INFO_PARAM_NUM) return false; m_lInfoParam[chParamID] = lParamVal; return true;}
	void		BeginGetPlyCha(void);
	CCharacter*	GetNextPlyCha(void);
	bool		CheckRun(void);
	void		DealActivePlayer(string & function);//2006.10.12wsj处理乱斗白银城的胜者.
	void		DealPlayer(string & function);
	//void		OnPlayerDie(CCharacter	*pCCha)//2006.10.12wsj
	//{
	//	Point		l_pt = pCCha->GetPos();
	//	Rect		l_rect = GetEyeshot(l_pt);

	//	if (!pCCha->m_pCEyeshotHost)
	//	{
	//		LG("地图角色操作错误", "角色死完 %s 统计地图内存活角色数时，角色视野单元[%d, %d]%s ，发现不在视野单元中\n", GetName(), l_pt.x, l_pt.y, pCCha->GetLogName());
	//		return;
	//	}
	//	if (pCCha->m_pCEyeshotHost != &m_pCEyeshotCell[l_pt.y][l_pt.x])
	//	{
	//		LG("地图角色操作错误", "角色死完 %s  统计地图内存活角色数时，角色视野单元[%d, %d]%s ，发现其与记录视野单元[%d, %d]不符\n", GetName(), l_pt.x, l_pt.y, pCCha->GetLogName(), pCCha->m_pCEyeshotHost->m_sPosX, pCCha->m_pCEyeshotHost->m_sPosY);
	//		return;
	//	}
	//	m_lActivePlayerNum--;
	//};
	void SetSpecialInter(int interva)//2006.10.12wsj
	{
		m_timeSpecialRun.SetInterval(interva);
	}
		

	dbc::Long GetActivePlayer()
	{
		CCharacter * pCha;
		long lActivePlayerNum = 0;
		BeginGetPlyCha();

		while(pCha = GetNextPlyCha())
		{
			if(pCha->IsPlayerCha() && pCha->IsLiveing())
			{
				lActivePlayerNum++;
			}
		}
		return lActivePlayerNum;
	}//2006.10.12wsj

	dbc::Long	CountEyeshotPlyActiveNum(dbc::Long lCellX, dbc::Long lCellY);
	void	ActiveEyeshotCell(dbc::Long lCellX, dbc::Long lCellY)
	{
		if (m_pCEyeshotCell[lCellY][lCellX].m_lActNum == 0)
			m_CEyeshotCellL.Add(&m_pCEyeshotCell[lCellY][lCellX]);
		m_pCEyeshotCell[lCellY][lCellX].m_lActNum++;
	}
	void	InactiveEyeshotCell(dbc::Long lCellX, dbc::Long lCellY)
	{
		m_pCEyeshotCell[lCellY][lCellX].m_lActNum--;
		if (m_pCEyeshotCell[lCellY][lCellX].m_lActNum == 0)
		{
			if (CountEyeshotPlyActiveNum(lCellX, lCellY) > 0) // 存在视野激活的玩家角色，这种情况不应该出现
			{
				m_pCEyeshotCell[lCellY][lCellX].m_lActNum++;
				//LG("视野激活错误", "!!!视野单元[%d,%d]停止激活时，发现其中存在有视野能力的玩家角色\n", lCellX, lCellY);
				LG("eyeshot activation error", "when eyeshot cell[%d,%d] stop activation ,find the character player who has eyeshot ability.\n", lCellX, lCellY);
			}
			else
				m_CEyeshotCellL.Del(&m_pCEyeshotCell[lCellY][lCellX]);
		}
	}
	bool	IsValidStateCell(dbc::Long x, dbc::Long y)
	{
		if (m_pCStateCell[y][x])
			return true;
		return false;
	}
	CChaListNode*	StateCellAddCha(dbc::Long lCellX, dbc::Long lCellY, CCharacter *pCCha, bool bIn)
	{
		CheckStateCell(lCellX, lCellY);
		return m_pCStateCell[lCellY][lCellX]->AddCharacter(pCCha, bIn);
	}

	void	StateCellDelCha(dbc::Long lCellX, dbc::Long lCellY, CChaListNode *pCChaNode)
	{
		m_pCStateCell[lCellY][lCellX]->DelCharacter(pCChaNode);
		ReleaseStateCell(lCellX, lCellY);
	}

	void	ActiveStateCell(dbc::Long lCellX, dbc::Long lCellY)
	{
		if (m_pCStateCell[lCellY][lCellX]->m_lActNum == 0)
			m_CStateCellL.Add(m_pCStateCell[lCellY][lCellX]);
		m_pCStateCell[lCellY][lCellX]->m_lActNum++;
		ActiveEyeshotCell(m_pCStateCell[lCellY][lCellX]->m_pCEyeshotCell->m_sPosX, m_pCStateCell[lCellY][lCellX]->m_pCEyeshotCell->m_sPosY);
	}
	void	InactiveStateCell(dbc::Long lCellX, dbc::Long lCellY)
	{
		InactiveEyeshotCell(m_pCStateCell[lCellY][lCellX]->m_pCEyeshotCell->m_sPosX, m_pCStateCell[lCellY][lCellX]->m_pCEyeshotCell->m_sPosY);
		m_pCStateCell[lCellY][lCellX]->m_lActNum--;
		if (m_pCStateCell[lCellY][lCellX]->m_lActNum == 0)
		{
			m_CStateCellL.Del(m_pCStateCell[lCellY][lCellX]);
			ReleaseStateCell(lCellX, lCellY);
		}
	}

	bool	AddCellState(dbc::uChar uchFightID, dbc::uLong ulSrcWorldID, dbc::Long lSrcHandle, dbc::Char chObjType, dbc::Char chObjHabitat, dbc::Char chEffType,
		dbc::Long lCellX, dbc::Long lCellY, dbc::uChar uchStateID, dbc::uChar uchStateLv, dbc::uLong ulStartTick, dbc::Long lOnTick, dbc::Char chType, dbc::Char chWithCenter)
	{
		bool	bValid = true;
		if (!m_pCStateCell[lCellY][lCellX])
		{
			bValid = false;
			CheckStateCell(lCellX, lCellY);
		}
		bool bHasState = m_pCStateCell[lCellY][lCellX]->HasState(uchStateID);
		bool bAddSuc = m_pCStateCell[lCellY][lCellX]->AddState(uchFightID, ulSrcWorldID, lSrcHandle, chObjType, chObjHabitat, chEffType, uchStateID, uchStateLv, ulStartTick, lOnTick, chType, chWithCenter);
		if (!bValid && !bAddSuc)
			ReleaseStateCell(lCellX, lCellY);
		if (!bHasState && bAddSuc)
			ActiveStateCell(lCellX, lCellY);

		return bAddSuc;
	}
	void	ReleaseStateCell(dbc::Long x, dbc::Long y)
	{
		if (m_pCStateCell[y][x] && m_pCStateCell[y][x]->GetChaNum() == 0 && m_pCStateCell[y][x]->GetStateNum() == 0)
		{
			m_pCStateCell[y][x]->Free();
			m_pCStateCell[y][x] = 0;
		}
	}

	CEyeshotCell	**m_pCEyeshotCell; // 视野单元
	CStateCell		***m_pCStateCell; // 状态单元
	COutMapCha		m_COutMapCha; // 地图外角色
	//供随机访问的地图信息数组
	CActEyeshotCell	m_CEyeshotCellL;	// 激活的视野单元列表
	CActStateCell	m_CStateCellL;		// 激活的状态单元列表

	bool			m_bIsRun;		// 标示本地图是否在运行
	CWeatherMgr		m_WeatherMgr;	// 天气管理器

private:
	CMapRes	*m_pCMapRes;
	CTimer	m_timeSpecialRun;

	void	CheckStateCell(dbc::Long x, dbc::Long y)
	{
		if (!m_pCStateCell[y][x])
		{
			m_pCStateCell[y][x] = g_pGameApp->m_MapStateCellHeap.Get();
			m_pCStateCell[y][x]->m_sPosX = (Short)x;
			m_pCStateCell[y][x]->m_sPosY = (Short)y;
			m_pCStateCell[y][x]->m_pCEyeshotCell = &m_pCEyeshotCell[y * GetStateCellWidth() / GetEyeshotCellWidth()][x * GetStateCellWidth() / GetEyeshotCellHeight()];
		}
	}

	// 范围内搜索
	struct
	{
		bool				m_bIncludeHideEnti;							// 是否搜索隐身的实体
		dbc::Short			m_sRangeMgrUnitNum;							// 范围内管理单元的数目
		dbc::Long			m_lRangeMgrUnit[defRANGE_MGRUNIU_NUM][2];	// 范围内的管理单元编号列表（x,y）
		dbc::Long			m_lRangeCentUnit[2];						// 范围中心单元;
		dbc::Short			m_sRangeCurMgrUnit;							// 搜索过程中，当前的管理单元
		CChaListNode*		m_pRangeCurEntiNode;						// 搜索过程中，当前的实体
		CRangeBase			*m_pCBaseRange;

		CRangeStick		m_CStickRange;
		CRangeFan		m_CFanRange;
		CRangeSquare	m_CSquareCRange;
		CRangeCircle	m_CCircleRange;
	};

	dbc::Short	m_sCopyNO;
	CTimer		m_timeScriptRun;

	dbc::Long	m_lPlayerNum;
//	dbc::Long	m_lActivePlayerNum;
	dbc::Long	m_lInfoParam[defMAPCOPY_INFO_PARAM_NUM];

};

#endif //SUBMAP_H
