//=============================================================================
// FileName: StateCell.h
// Creater: ZhangXuedong
// Date: 2005.04.29
// Comment: Map State Cell
//=============================================================================

#ifndef STATECELL_H
#define STATECELL_H

#include "PreAlloc.h"
#include "Character.h"

class CChaListNode : public dbc::PreAllocStru
{
public:
	CChaListNode(unsigned long ulSize = 1) : PreAllocStru(1)
	{
		m_pCCha = 0;
		m_pCNext = 0;
		m_pCLast = 0;
	}

	bool			m_bIn;	// �ڵ�Ԫ��
	CCharacter		*m_pCCha;
	CStateCellNode	*m_pCEntStateNode;

	CChaListNode	*m_pCNext;
	CChaListNode	*m_pCLast;

protected:
	void	Initially();
	void	Finally();

};

inline void CChaListNode::Initially()
{
	m_pCCha = 0;
	m_pCNext = 0;
	m_pCLast = 0;
}

inline void CChaListNode::Finally()
{
}

class CEyeshotCell;

// ״̬��Ԫ��
class CStateCell : public dbc::PreAllocStru
{
public:
	CStateCell(unsigned long ulSize = 1) : PreAllocStru(1)
	{
		m_sPosX = 0;
		m_sPosY = 0;
		m_lActNum = 0;
		m_lChaNum = 0;
		m_pCChaIn = 0;
		m_pCChaCross = 0;

		m_pCNext = 0;
		m_pCLast = 0;
	}

	long			GetChaNum(void) {return m_lChaNum;}

#ifdef COMPACT_MEM_2
	long			GetStateNum(void) {return (long)m_CSkillState.m_SStateMap.size();}
#else
	long			GetStateNum(void) {return m_CSkillState.m_uchStateNum;}
#endif

	CChaListNode*	AddCharacter(CCharacter *pCCha, bool bIn);
	void			DelCharacter(CChaListNode *pCEntNode);
	void			SetCharacterIn(CChaListNode *pCEntNode, bool bIn = true);

	bool			HasState(unsigned char uchStateID) {return m_CSkillState.HasState(uchStateID);}
	bool			HasState(unsigned char uchStateID, unsigned char uchStateLv) {return m_CSkillState.HasState(uchStateID, uchStateLv);}
	bool			AddState(unsigned char uchFightID, unsigned long ulSrcWorldID, long lSrcHandle, char chObjType, char chObjHabitat, char chEffType,
						unsigned char uchStateID, unsigned char uchStateLv, unsigned long ulStartTick, long lOnTick, char chType, char chWithCenter);
	void			DelState(unsigned char uchStateID);
#ifdef COMPACT_MEM_2
#else
	bool			AddStateToCharacter(unsigned char uchStateNo, CCharacter *pCCha, long lOnTime, char chAddType, bool bNotice = true);
	bool			ResetStateToCharacter(unsigned char uchStateNo, CCharacter *pCCha, long lOnTime, char chAddType, bool bNotice = true );
#endif
	bool			AddStateToCharacter(SSkillStateUnit	*pSStateUnit, CCharacter *pCCha, long lOnTime, char chAddType, bool bNotice = true);
	bool			ResetStateToCharacter(SSkillStateUnit	*pSStateUnit, CCharacter *pCCha, long lOnTime, char chAddType, bool bNotice = true );

	void			DropState(SubMap *pCMap);
	void			StateRun(unsigned long ulCurTick, SubMap *pCMap);

	void			StateBeginSeen(Entity *pCEnt);
	void			StateEndSeen(Entity *pCEnt);

	short			m_sPosX;		// λ��
	short			m_sPosY;		// λ��
	long			m_lActNum;		// �������
	long			m_lChaNum;		// ��ɫ����

	CChaListNode	*m_pCChaIn;		// ��¼���ĵ��ڷ�Χ�ڵĽ�ɫ
	CChaListNode	*m_pCChaCross;	// ��¼���ĵ㲻�ڷ�Χ�ڣ����뷶Χ�ཻ�Ľ�ɫ

	CSkillState		m_CSkillState;	// �ر�ļ���״̬

	CEyeshotCell	*m_pCEyeshotCell;	// ���ڵ���Ұ��Ԫָ��

	CStateCell		*m_pCNext;		// ָ�򡰼����״̬��Ԫ������ָ��
	CStateCell		*m_pCLast;

protected:

private:
	void	Initially();
	void	Finally();

};

inline void CStateCell::Initially()
{
	m_lActNum = 0;
	m_lChaNum = 0;
	m_pCChaIn = 0;
	m_pCChaCross = 0;

	m_CSkillState.InitSkillState();
	m_pCEyeshotCell = 0;

	m_pCNext = 0;
	m_pCLast = 0;
}

inline void CStateCell::Finally()
{
}

class CActStateCell // �����״̬��Ԫ����
{
public:
	CActStateCell()
	{
		m_pHead  = 0;
		m_lCount = 0;
	}

	void		Add(CStateCell *pObj);
	void		Del(CStateCell *pObj);

	void		BeginGet(void);
	CStateCell*	GetNext(void);

	long		GetActiveNum(void) {return m_lCount;}

protected:

private:
	CStateCell	*m_pHead;

	CStateCell	*m_pCur;

	long		m_lCount;

};

inline void CActStateCell::Add(CStateCell *pObj)
{T_B
	if (pObj->m_pCLast || pObj->m_pCNext)
	{
		//LG("״̬��Ԫ��������", "��״̬��Ԫ����ʵ�� [%d,%d] ʱ��������û��������ǰ�ĵ�Ԫ", pObj->m_sPosX, pObj->m_sPosY);
		LG("state cell operator error", "when add entity[%d,%d] to state cell��find it is not break away foregone manage cell", pObj->m_sPosX, pObj->m_sPosY);
		return;
	}

	pObj->m_pCLast = 0;
	if (pObj->m_pCNext = m_pHead)
		m_pHead->m_pCLast = pObj;
	m_pHead = pObj;

	m_lCount++;
T_E}

inline void CActStateCell::Del(CStateCell *pObj)
{T_B
	if (!pObj)
		return;
	if (m_pCur == pObj)
		m_pCur = pObj->m_pCNext;

	if (pObj->m_pCLast)
		pObj->m_pCLast->m_pCNext = pObj->m_pCNext;
	if (pObj->m_pCNext)
		pObj->m_pCNext->m_pCLast = pObj->m_pCLast;
	if (m_pHead == pObj)
		if (m_pHead = pObj->m_pCNext)
			m_pHead->m_pCLast = 0;
	pObj->m_pCNext = 0;
	pObj->m_pCLast = 0;

	m_lCount--;
T_E}

inline void CActStateCell::BeginGet()
{T_B
	m_pCur = m_pHead;
T_E}

inline CStateCell* CActStateCell::GetNext()
{T_B
	CStateCell	*pRet = m_pCur;

	if (m_pCur)
		m_pCur = m_pCur->m_pCNext;

	return pRet;
T_E}

class CStateCellNode : public dbc::PreAllocStru
{
public:
	CStateCellNode(unsigned long ulSize = 1) : PreAllocStru(1)
	{
		m_pCStateCell = 0;
		m_pCChaNode = 0;
		m_pCNext = 0;
		m_pCLast = 0;
	}

	CStateCell		*m_pCStateCell;
	CChaListNode	*m_pCChaNode;

	CStateCellNode	*m_pCNext;
	CStateCellNode	*m_pCLast;

protected:
	void	Initially();
	void	Finally();

};

inline void CStateCellNode::Initially()
{
	m_pCStateCell = 0;
	m_pCChaNode = 0;
	m_pCNext = 0;
	m_pCLast = 0;
}

inline void CStateCellNode::Finally()
{
}

#endif // STATECELL_H