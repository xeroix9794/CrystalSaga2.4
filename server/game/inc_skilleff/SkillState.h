//=============================================================================
// FileName: SkillState.h
// Creater: ZhangXuedong
// Date: 2005.01.13
// Comment: Skill State
//=============================================================================

#ifndef SKILLSTATE_H
#define SKILLSTATE_H

//#define COMPACT_MEM_2  // �ڴ�ѹ���汾��ʹ��STL���������滻�����԰汾

#ifdef COMPACT_MEM_2
#include <map>
using namespace std;
#endif

#include "GameAppNet.h"
#include "SkillStateType.h"
#include "CompCommand.h"
#include "SkillStateRecord.h"

struct SSkillStateUnit
{
#ifdef COMPACT_MEM_2
	// ����Ҫ�����ѯ
#else
	unsigned short	uchReverseID;
#endif

	unsigned short	uchStateID;		// ״̬ID
	unsigned char	uchStateLv;		// ״̬����

	char			chCenter;		// ���ڱ�ʾ�Ƿ�������״̬�����ģ������ڵر�״̬������
	unsigned char	uchFightID;		// �����ڿͻ��˽���˳��ʶ��
	char			chObjType;		// ״̬���õ�Ŀ�����ͣ��������з��������ڵ���״̬�����õ�Ŀ�����ѡ��
	char			chObjHabitat;	// ״̬���õ�Ŀ����Ϣ�����ͣ�½�أ��������ܣ������ڵ���״̬�����õ�Ŀ�����ѡ��
	char			chEffType;		// ״̬���õ�Ч�����ͣ�����ģ��к��ģ������ڵ���״̬�����õ�Ŀ�����ѡ��
	// ״̬�ͷ��ߵı�ʾ
	unsigned long	ulSrcWorldID;
	long			lSrcHandle;
	//

	long			lOnTick;		// �롣-1��ʾ���޳���ʱ�䣬>0��ʾ���޳���ʱ�䣬0δʹ��
	unsigned long	ulStartTick;	// ����
	unsigned long	ulLastTick;		// ����

	unsigned short	GetStateID() {return uchStateID;}
	unsigned char	GetStateLv() {return uchStateLv;}
};

// TODO�޸ģ�ÿ����ɫ��Ҫ����ȫ���ļ���״̬��ֻ�����Լ�ӵ�еļ���״̬
// ��ɫ��ʼ��ʱ�� 8200Byte ���ٵ� 1084Byte�������� ��8200-1084��*24000/1024/1024= 163M�ڴ�
// ��ͼ��ʼ��ʱ�� 8200Byte ���ٵ� 1084Byte�������� ��8200-1084��*11600/1024/1024= 79M�ڴ�
class CSkillState
{
public:
	void	InitSkillState(unsigned short uchMaxState = AREA_STATE_MAXID);

	CSkillState(unsigned short uchMaxState = AREA_STATE_MAXID)
	{
		InitSkillState(uchMaxState);
		
	}

	bool	AddSkillState(unsigned char uchFightID, unsigned long ulSrcWorldID, long lSrcHandle, char chObjType, char chObjHabitat, char chEffType,
			unsigned short, unsigned char uchStateLv, unsigned long ulStartTick, long lOnTick, char chType, char chWithCenter = 0);
	bool	DelSkillState(unsigned short uchStateID);
	void	ResetSkillState(void);
	bool	NeedResetState( unsigned short uchStateID );
	bool	HasState(unsigned short uchStateID, unsigned char uchStateLv);
	bool	HasState(unsigned short uchStateID);
	SSkillStateUnit*	GetSStateByID(unsigned short uchStateID);
#ifdef COMPACT_MEM_2
	unsigned char		GetStateNum(void) {return (unsigned char)m_SStateMap.size();}
#else
	SSkillStateUnit*	GetSStateByNum(unsigned short uchNum);
	unsigned short		GetStateNum(void) {return m_uchStateNum;}
	unsigned short		GetReverseID(unsigned short uchStateID);
#endif

	void	ResetSkillStateChangeFlag();
	bool	GetSkillStateChangeBitFlag(long lBit);
#ifdef COMPACT_MEM_2
	unsigned char	GetSkillStateChangeNum(void) {return (unsigned char)m_SStateChangeMap.size();}
#else
	void	SetSkillStateChangeBitFlag(long lBit);
	unsigned short	GetSkillStateChangeNum(void) {return m_uchChangeNum;}

	void	BeginGetState(void) {m_uchCurGetNo = m_uchStateNum;}
	SSkillStateUnit*	GetNextState(void);

	SSkillStateUnit	*m_pSState[SKILL_STATE_MAXID + 1];
	unsigned short	m_uchStateNum;
#endif

	bool	WriteState(WPACKET &pk);

protected:

#ifdef COMPACT_MEM_2
public:
	map<unsigned char, SSkillStateUnit> m_SStateMap;
	map<unsigned char, SSkillStateUnit> m_SStateChangeMap;

	unsigned char	m_uchMaxState;
#else
private:
	unsigned short	m_uchMaxState;
	SSkillStateUnit	m_SState[SKILL_STATE_MAXID + 1];

	char			m_szChangeFlag[SSTATE_SIGN_BYTE_NUM]; // ��ʶ�ı�ļ���״̬
	unsigned char	m_uchChangeNum; // ����״̬�ı�ĸ���
	unsigned short	m_uchCurGetNo;
#endif
};

inline void CSkillState::InitSkillState(unsigned short uchMaxState)
{T_B
	if (uchMaxState > SKILL_STATE_MAXID)
		uchMaxState = SKILL_STATE_MAXID;
	m_uchMaxState = uchMaxState;

#ifdef COMPACT_MEM_2
	m_SStateMap.clear();
	m_SStateChangeMap.clear();
#else
	m_uchStateNum = 0;
	for (unsigned short i = 0; i <= m_uchMaxState; i++)
	{
		m_SState[i].uchStateID = i;
		m_SState[i].uchStateLv = 0;
	}
#endif
T_E}

inline void CSkillState::ResetSkillState(void)
{
#ifdef COMPACT_MEM_2
	m_SStateMap.clear();
	m_SStateChangeMap.clear();
#else
	for (unsigned short i = 0; i < m_uchStateNum; i++)
		m_pSState[i]->uchStateLv = 0;
	m_uchStateNum = 0;
	ResetSkillStateChangeFlag();
#endif
}

inline bool CSkillState::AddSkillState(unsigned char uchFightID, unsigned long ulSrcWorldID, long lSrcHandle, char chObjType, char chObjHabitat, char chEffType,
							 unsigned short uchStateID, unsigned char uchStateLv, unsigned long ulStartTick, long lOnTick, char chType, char chWithCenter)
{T_B
#ifdef COMPACT_MEM_2
	if (uchStateID < 1 || uchStateID > m_uchMaxState)
		return false;
	if (uchStateLv <= 0)
		return false;

	map<unsigned char, SSkillStateUnit>::iterator it = m_SStateMap.find(uchStateID);
	// ���������
	if(it == m_SStateMap.end())
	{
		if(m_SStateMap.size() >= m_uchMaxState)
			return false;

		SSkillStateUnit unit;

		unit.uchStateID = uchStateID;
		unit.chCenter = chWithCenter;
		unit.uchFightID = uchFightID;
		unit.ulSrcWorldID = ulSrcWorldID;
		unit.lSrcHandle = lSrcHandle;
		unit.chObjType = chObjType;
		unit.chObjHabitat = chObjHabitat;
		unit.chEffType = chEffType;
		unit.uchStateLv = uchStateLv;
		unit.ulStartTick = ulStartTick;
		unit.ulLastTick = ulStartTick;
		unit.lOnTick = lOnTick;

		m_SStateMap.insert(map<unsigned char, SSkillStateUnit>::value_type(uchStateID, unit));
		m_SStateChangeMap[uchStateID] = unit;
	}
	else 
	{
		// ���ڵ����滻
		if (chType == enumSSTATE_ADD_EQUALORLARGER)
		{
			if (uchStateLv < it->second.uchStateLv)
				return false;

			it->second.chCenter = chWithCenter;
			it->second.uchFightID = uchFightID;
			it->second.ulSrcWorldID = ulSrcWorldID;
			it->second.lSrcHandle = lSrcHandle;
			it->second.chObjType = chObjType;
			it->second.chObjHabitat = chObjHabitat;
			it->second.chEffType = chEffType;
			it->second.uchStateLv = uchStateLv;
			it->second.ulStartTick = ulStartTick;
			it->second.ulLastTick = ulStartTick;
			it->second.lOnTick = lOnTick;

			m_SStateChangeMap[uchStateID] = it->second;
		}
		else if (chType == enumSSTATE_ADD_LARGER) // �����滻
		{
			if (uchStateLv <= it->second.uchStateLv)
				return false;

			it->second.chCenter = chWithCenter;
			it->second.uchFightID = uchFightID;
			it->second.ulSrcWorldID = ulSrcWorldID;
			it->second.lSrcHandle = lSrcHandle;
			it->second.chObjType = chObjType;
			it->second.chObjHabitat = chObjHabitat;
			it->second.chEffType = chEffType;
			it->second.uchStateLv = uchStateLv;
			it->second.ulStartTick = ulStartTick;
			it->second.ulLastTick = ulStartTick;
			it->second.lOnTick = lOnTick;

			m_SStateChangeMap[uchStateID] = it->second;
		}
		else if (chType == enumSSTATE_NOTADD)
		{
			return false;
		}
		else if (chType == enumSSTATE_ADD)
		{
			it->second.chCenter = chWithCenter;
			it->second.uchFightID = uchFightID;
			it->second.ulSrcWorldID = ulSrcWorldID;
			it->second.lSrcHandle = lSrcHandle;
			it->second.chObjType = chObjType;
			it->second.chObjHabitat = chObjHabitat;
			it->second.chEffType = chEffType;
			it->second.uchStateLv = uchStateLv;
			it->second.ulStartTick = ulStartTick;
			it->second.ulLastTick = ulStartTick;
			it->second.lOnTick = lOnTick;

			m_SStateChangeMap[uchStateID] = it->second;
		}
	}
#else
	if (uchStateID < 1 || uchStateID > m_uchMaxState)
		return false;
	if (uchStateLv <= 0)
		return false;

	if (m_SState[uchStateID].uchStateLv == 0)
	{
		if (m_uchStateNum >= m_uchMaxState)
			return false;

		m_pSState[m_uchStateNum] = m_SState + uchStateID;
		m_SState[uchStateID].uchReverseID = m_uchStateNum;
		m_SState[uchStateID].chCenter = chWithCenter;
		m_SState[uchStateID].uchFightID = uchFightID;
		m_SState[uchStateID].ulSrcWorldID = ulSrcWorldID;
		m_SState[uchStateID].lSrcHandle = lSrcHandle;
		m_SState[uchStateID].chObjType = chObjType;
		m_SState[uchStateID].chObjHabitat = chObjHabitat;
		m_SState[uchStateID].chEffType = chEffType;
		m_SState[uchStateID].uchStateLv = uchStateLv;
		m_SState[uchStateID].ulStartTick = ulStartTick;
		m_SState[uchStateID].ulLastTick = ulStartTick;
		m_SState[uchStateID].lOnTick = lOnTick;
		m_uchStateNum++;

		SetSkillStateChangeBitFlag(uchStateID);
	}
	else if (chType == enumSSTATE_ADD_EQUALORLARGER)
	{
		if (uchStateLv < m_SState[uchStateID].uchStateLv)
			return false;

		m_SState[uchStateID].ulSrcWorldID = ulSrcWorldID;
		m_SState[uchStateID].chCenter = chWithCenter;
		m_SState[uchStateID].uchFightID = uchFightID;
		m_SState[uchStateID].lSrcHandle = lSrcHandle;
		m_SState[uchStateID].chObjType = chObjType;
		m_SState[uchStateID].chObjHabitat = chObjHabitat;
		m_SState[uchStateID].chEffType = chEffType;
		m_SState[uchStateID].uchStateLv = uchStateLv;
		m_SState[uchStateID].ulStartTick = ulStartTick;
		m_SState[uchStateID].ulLastTick = ulStartTick;
		m_SState[uchStateID].lOnTick = lOnTick;

		SetSkillStateChangeBitFlag(uchStateID);
	}
	else if (chType == enumSSTATE_ADD_LARGER)
	{
		if (uchStateLv <= m_SState[uchStateID].uchStateLv)
			return false;

		m_SState[uchStateID].ulSrcWorldID = ulSrcWorldID;
		m_SState[uchStateID].chCenter = chWithCenter;
		m_SState[uchStateID].uchFightID = uchFightID;
		m_SState[uchStateID].lSrcHandle = lSrcHandle;
		m_SState[uchStateID].chObjType = chObjType;
		m_SState[uchStateID].chObjHabitat = chObjHabitat;
		m_SState[uchStateID].chEffType = chEffType;
		m_SState[uchStateID].uchStateLv = uchStateLv;
		m_SState[uchStateID].ulStartTick = ulStartTick;
		m_SState[uchStateID].ulLastTick = ulStartTick;
		m_SState[uchStateID].lOnTick = lOnTick;

		SetSkillStateChangeBitFlag(uchStateID);
	}
	else if (chType == enumSSTATE_NOTADD)
	{
		return false;
	}
	else if (chType == enumSSTATE_ADD)
	{
		m_SState[uchStateID].ulSrcWorldID = ulSrcWorldID;
		m_SState[uchStateID].chCenter = chWithCenter;
		m_SState[uchStateID].uchFightID = uchFightID;
		m_SState[uchStateID].lSrcHandle = lSrcHandle;
		m_SState[uchStateID].chObjType = chObjType;
		m_SState[uchStateID].chObjHabitat = chObjHabitat;
		m_SState[uchStateID].chEffType = chEffType;
		m_SState[uchStateID].uchStateLv = uchStateLv;
		m_SState[uchStateID].ulStartTick = ulStartTick;
		m_SState[uchStateID].ulLastTick = ulStartTick;
		m_SState[uchStateID].lOnTick = lOnTick;

		SetSkillStateChangeBitFlag(uchStateID);
	}
#endif

	return true;
T_E}

inline bool CSkillState::DelSkillState(unsigned short uchStateID)
{T_B
#ifdef COMPACT_MEM_2
	if (uchStateID < 1 || uchStateID > m_uchMaxState || m_SStateMap.size() < 1)
		return false;

	map<unsigned char, SSkillStateUnit>::iterator it = m_SStateMap.find(uchStateID);
	// ���������
	if(it == m_SStateMap.end())
		return false;

	it->second.uchStateLv = 0;
	m_SStateChangeMap[uchStateID] = it->second;
	m_SStateMap.erase(it);
#else
	if (uchStateID < 1 || uchStateID > m_uchMaxState || m_uchStateNum < 1)
		return false;

	if (m_SState[uchStateID].uchStateLv != 0)
	{
		if (m_uchCurGetNo == m_uchStateNum)
			m_uchCurGetNo--;
		m_uchStateNum--;
		m_SState[uchStateID].uchStateLv = 0;
		m_pSState[m_SState[uchStateID].uchReverseID] = m_pSState[m_uchStateNum];
		m_pSState[m_uchStateNum]->uchReverseID = m_SState[uchStateID].uchReverseID;

		SetSkillStateChangeBitFlag(uchStateID);
	}
#endif
	return true;
T_E}

inline SSkillStateUnit* CSkillState::GetSStateByID(unsigned short uchStateID)
{T_B
#ifdef COMPACT_MEM_2
	if (uchStateID < 1 || uchStateID > m_uchMaxState || m_SStateMap.size() < 1)
		return 0;

	map<unsigned char, SSkillStateUnit>::iterator it = m_SStateMap.find(uchStateID);
	// ���������
	if(it == m_SStateMap.end())
		return 0;
	else
		return &it->second;
#else
	if (uchStateID < 1 || uchStateID > m_uchMaxState)
		return 0;

	if (m_SState[uchStateID].uchStateLv > 0)
		return m_SState + uchStateID;
	else
		return 0;
#endif
T_E}

#ifdef COMPACT_MEM_2
#else
inline SSkillStateUnit* CSkillState::GetSStateByNum(unsigned short uchNum)
{T_B
	if (uchNum < 0 || uchNum >= m_uchStateNum)
		return 0;

	if (m_pSState[uchNum]->uchStateLv > 0)
		return m_pSState[uchNum];
	else // �÷�֧��Ӧ��ִ��
	{
		//LG("״̬�������", "msg״̬�����ͳ�Ƹ�����ʵ�ʸ���������!");
		return 0;
	}
T_E}
#endif

#ifdef COMPACT_MEM_2
#else
inline unsigned short CSkillState::GetReverseID(unsigned short uchStateID)
{
	if (uchStateID < 1 || uchStateID > m_uchMaxState)
		return SKILL_STATE_MAXID;

	if (m_SState[uchStateID].uchStateLv <= 0)
		return SKILL_STATE_MAXID;

	return m_SState[uchStateID].uchReverseID;
}
#endif

inline bool CSkillState::NeedResetState( unsigned short uchStateID )
{
#ifdef COMPACT_MEM_2
	if (uchStateID < 1 || uchStateID > m_uchMaxState)
		return false;

	map<unsigned char, SSkillStateUnit>::iterator it = m_SStateMap.find(uchStateID);
	// ���������
	if(it == m_SStateMap.end())
		return false;

	if (it->second.uchStateLv <= 0)
		return false;

	if(it->second.lOnTick == -1 ) 
		return true;
#else
	if (uchStateID < 1 || uchStateID > m_uchMaxState)
		return false;

	if (m_SState[uchStateID].uchStateLv <= 0)
		return false;

	if(m_SState[uchStateID].lOnTick == -1 ) 
		return true;
#endif
	return false;
}

inline bool CSkillState::HasState(unsigned short uchStateID, unsigned char uchStateLv)
{
#ifdef COMPACT_MEM_2
	if (uchStateID < 1 || uchStateID > m_uchMaxState)
		return false;

	map<unsigned char, SSkillStateUnit>::iterator it = m_SStateMap.find(uchStateID);
	// ���������
	if(it == m_SStateMap.end())
		return false;

	if (it->second.uchStateLv <= 0 || it->second.uchStateLv != uchStateLv)
		return false;
#else
	if (uchStateID < 1 || uchStateID > m_uchMaxState)
		return false;

	if (m_SState[uchStateID].uchStateLv <= 0 || m_SState[uchStateID].uchStateLv != uchStateLv)
		return false;
#endif
	return true;
}

inline bool CSkillState::HasState(unsigned short uchStateID)
{
#ifdef COMPACT_MEM_2
	if (uchStateID < 1 || uchStateID > m_uchMaxState)
		return false;

	map<unsigned char, SSkillStateUnit>::iterator it = m_SStateMap.find(uchStateID);
	// ���������
	if(it == m_SStateMap.end())
		return false;

	if (it->second.uchStateLv <= 0)
		return false;
#else
	if (uchStateID < 1 || uchStateID > m_uchMaxState)
		return false;

	if (m_SState[uchStateID].uchStateLv <= 0)
		return false;
#endif
	return true;
}

inline void CSkillState::ResetSkillStateChangeFlag()
{T_B
#ifdef COMPACT_MEM_2
	this->m_SStateChangeMap.clear();
#else
	memset(m_szChangeFlag, 0, SSTATE_SIGN_BYTE_NUM);
	m_uchChangeNum = 0;
#endif
T_E}

#ifdef COMPACT_MEM_2
#else
inline void CSkillState::SetSkillStateChangeBitFlag(long lBit)
{T_B
	if (lBit > m_uchMaxState)
		return;

	short sByteNO, sBitNO;
	sByteNO = short(lBit / 8);
	sBitNO = short(lBit % 8);

	char	chSetFlag = 0x01 << sBitNO;

	if (!(m_szChangeFlag[sByteNO] & chSetFlag))
		m_uchChangeNum++;

	m_szChangeFlag[sByteNO] |= chSetFlag;
T_E}
#endif

inline bool	CSkillState::GetSkillStateChangeBitFlag(long lBit)
{T_B
#ifdef COMPACT_MEM_2
	// lBit ��Ϊ״̬ID
	map<unsigned char, SSkillStateUnit>::iterator it = m_SStateChangeMap.find((const unsigned char)lBit);
	// ���������
	if(it == m_SStateChangeMap.end())
		return false;

	return true;
#else
	if (lBit > m_uchMaxState)
		return false;

	short sByteNO, sBitNO;
	sByteNO = short(lBit / 8);
	sBitNO = short(lBit % 8);

	return m_szChangeFlag[sByteNO] & (0x01 << sBitNO) ? true : false;
#endif
T_E}

#ifdef COMPACT_MEM_2
#else
inline SSkillStateUnit* CSkillState::GetNextState(void)
{
	if (m_uchCurGetNo <= m_uchStateNum)
		return GetSStateByNum(--m_uchCurGetNo);
	return 0;
}
#endif

inline bool CSkillState::WriteState(WPACKET &pk)
{
#ifdef COMPACT_MEM_2
	WRITE_CHAR(pk, (dbc::uChar)m_SStateMap.size());

	if (m_SStateMap.size() <= 0)
		return false;

	CSkillStateRecord	*pCStateRec;
	SSkillStateUnit *pUnit;

	for (map<unsigned char, SSkillStateUnit>::iterator it = m_SStateMap.begin(); it != m_SStateMap.end(); it++)
	{
		pUnit = &it->second;

		pCStateRec = GetCSkillStateRecordInfo(pUnit->GetStateID());
		if (pCStateRec->IsShowCenter == 1 && pUnit->chCenter == 0)
		{
			WRITE_CHAR(pk, 0);
			continue;
		}
		WRITE_CHAR(pk, pUnit->uchStateID);
		WRITE_CHAR(pk, pUnit->uchStateLv);
		WRITE_LONG(pk, pUnit->ulSrcWorldID);
		WRITE_CHAR(pk, pUnit->uchFightID);
	}
#else
	WRITE_SHORT(pk, m_uchStateNum);

	if (m_uchStateNum <= 0)
		return false;

	CSkillStateRecord	*pCStateRec;
	for (unsigned short j = 0; j < m_uchStateNum; j++)
	{
		pCStateRec = GetCSkillStateRecordInfo(m_pSState[j]->GetStateID());
		if (pCStateRec->IsShowCenter == 1 && m_pSState[j]->chCenter == 0)
		{
			WRITE_CHAR(pk, 0);
			continue;
		}
		WRITE_SHORT(pk, m_pSState[j]->uchStateID);
		WRITE_CHAR(pk, m_pSState[j]->uchStateLv);
		WRITE_LONG(pk, m_pSState[j]->ulSrcWorldID);
		WRITE_CHAR(pk, m_pSState[j]->uchFightID);
	}
#endif
	return true;
}

#endif // SKILLSTATE_H