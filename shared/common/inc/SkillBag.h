//=============================================================================
// FileName: SkillBag.h
// Creater: ZhangXuedong
// Date: 2005.02.17
// Comment: Skill Bag
//=============================================================================

#ifndef SKILLBAG_H
#define SKILLBAG_H

#include "CompCommand.h"
#include "StringPoolL.h"

//#define COMPACT_MEM  // �ڴ�ѹ���汾��ʹ��STL���������滻�����԰汾

#ifdef COMPACT_MEM
#include <map>
using namespace std;
#endif

#define defMAX_SKILL_NO		600
#define defMAX_SKILL_LV		20

// ���¼���ʱ,���Ͳο�CompCommand.h - ESynSkillBagType
// ����ȫ������ʱ,Ĭ�ϼ���Ϊ��һ��,����Ϊ���Ӽ���
// ����Ĭ�ϼ���ʱ,��ֻ��һ��

#define defSKILL_RANGE_PARAM_NUM	4

struct SSkillGrid
{
	char	chState;	// ����״̬���μ�CompCommand.h��ESkillUseState
	char	chLv;		// ���ܵȼ�
	short	sID;		// ���ܱ��

	long	lColdDownT;	// ���ܿ�ʼ�ָ���ʱ��
#ifdef COMPACT_MEM	
#else
	short	sReverseID;	// ���ڷ�����
#endif
};

struct SSkillGridEx // ������ͻ���ͬ������
{
	char	chState;	// ����״̬���μ�CompCommand.h��ESkillUseState
	char	chLv;		// ���ܵȼ�
	short	sID;		// ���ܱ��
	short	sUseSP;		// SP����ֵ
	short	sUseEndure;	// ���;öȡ�����ֵ
	short	sUseEnergy;	// ������������ֵ
	long	lResumeTime;// ���ͷ���Ҫ��ʱ�䣨���룩
	short	sRange[defSKILL_RANGE_PARAM_NUM]; // ��������������+������
};

// TODO�޸ģ�ÿ����ɫ��Ҫ����ȫ���ļ��ܣ�ֻ�����Լ�ӵ�еļ���
// ��ʼ��ʱ�� 10096Byte ���ٵ� 48Byte�������� ��10096-48��*24000��1024��1024= 230M�ڴ�
class CSkillBag
{
public:
	CSkillBag()
	{
		InitSkill();
	}
	void		InitSkill(void);
	bool		AddSkill(SSkillGrid *pGridCont);
	bool		DelSkill(short sSkillID);
	SSkillGrid*	GetSkillContByID(short sSkillID);

#ifdef COMPACT_MEM	
	SSkillGrid*	GetFirstSkill();	// �õ��������ܣ�ֻ�д�ʱ��ʹ��
#else
	SSkillGrid*	GetSkillContByNum(short sNum);
	SSkillGrid*	GetChangeSkill(short sChangeID);
#endif
	short		GetSkillNum();
	bool		HasSkill(short sID);
	bool		SetSkillState(short sID, char chState);
	char		GetSkillState(short sID);

	void		SetChangeSkillFlag(bool bChange = true);
	short		GetChangeSkillNum();

#ifdef COMPACT_MEM
	map<short,  SSkillGrid> m_SBagMap;
	map<short,  SSkillGrid> m_ChangeMap;
#else
	SSkillGrid	*m_pSBag[defMAX_SKILL_NO + 1];
	short		m_sSkillNum;
#endif
protected:

private:

#ifdef COMPACT_MEM
#else

	SSkillGrid	m_SBag[defMAX_SKILL_NO + 1];

	short		m_sChangeNum;
	short		m_sChangeSkill[defMAX_SKILL_NO + 1];
	char		m_szSkillIsChanged[defMAX_SKILL_NO + 1];
#endif
};

inline void CSkillBag::InitSkill(void)
{
#ifdef COMPACT_MEM
	m_SBagMap.clear();
	m_ChangeMap.clear();
#else
	m_sSkillNum = 0;

	for (short i = 0; i <= defMAX_SKILL_NO; i++)
	{
		m_SBag[i].sID = i;
		m_SBag[i].chLv = 0;
		m_SBag[i].lColdDownT = 0;
		m_SBag[i].chState = enumSUSTATE_INACTIVE;
	}

	SetChangeSkillFlag(false);
#endif
}

inline void CSkillBag::SetChangeSkillFlag(bool bChange)
{
#ifdef COMPACT_MEM
	if (!bChange)
		m_ChangeMap.clear();
	else
	{
		m_ChangeMap.clear();

		for(map<short,  SSkillGrid>::iterator it = m_SBagMap.begin(); it != m_SBagMap.end(); it++)
		{
			m_ChangeMap.insert(map<short,  SSkillGrid>::value_type(it->first, it->second));	
		}
	}
#else
	memset(m_szSkillIsChanged, 0, sizeof(char) * (defMAX_SKILL_NO + 1));
	if (bChange)
	{
		m_sChangeNum = m_sSkillNum;

		for (short i = 0; i < m_sChangeNum; i++)
		{
			m_sChangeSkill[i] = i;
			m_szSkillIsChanged[m_pSBag[i]->sID] = 1;
		}
	}
	else
		m_sChangeNum = 0;
#endif
}

inline bool CSkillBag::AddSkill(SSkillGrid *pGridCont)
{
#ifdef COMPACT_MEM
	if (pGridCont->sID < 0 || pGridCont->sID > defMAX_SKILL_NO)
		return false;

	if (pGridCont->chLv == 0)
		return false;

	if ((unsigned char)pGridCont->chLv > defMAX_SKILL_LV)
		pGridCont->chLv = defMAX_SKILL_LV;

	map<short,  SSkillGrid>::iterator it = m_SBagMap.find(pGridCont->sID);

	pGridCont->lColdDownT = 0;

	// �¼���
	if(it == m_SBagMap.end())
		m_SBagMap.insert(map<short,  SSkillGrid>::value_type(pGridCont->sID, *pGridCont));
	else
	{
		it->second.chLv = pGridCont->chLv;
		it->second.chState = pGridCont->chState;
		it->second.lColdDownT = pGridCont->lColdDownT;
	}

	// �оͲ�׷����
	it = m_ChangeMap.find(pGridCont->sID);
	if(it == m_ChangeMap.end())
		m_ChangeMap.insert(map<short,  SSkillGrid>::value_type(pGridCont->sID, *pGridCont));

#else
	if (pGridCont->sID < 0 || pGridCont->sID > defMAX_SKILL_NO)
		return false;

	if (pGridCont->chLv == 0)
		return false;

	if ((unsigned char)pGridCont->chLv > defMAX_SKILL_LV)
		pGridCont->chLv = defMAX_SKILL_LV;

	// ������¼Ӽ���������ϵ
	if (m_SBag[pGridCont->sID].chLv == 0)
	{
		m_pSBag[m_sSkillNum] = m_SBag + pGridCont->sID;
		m_SBag[pGridCont->sID].sReverseID = m_sSkillNum;  // �����������
		m_sSkillNum++;
	}

	// ����仯�ļ���
	if (m_SBag[pGridCont->sID].chState != pGridCont->chState || m_SBag[pGridCont->sID].chLv != pGridCont->chLv)
	{
		if (m_szSkillIsChanged[pGridCont->sID] == 0)
		{
			m_szSkillIsChanged[pGridCont->sID] = 1;
			m_sChangeSkill[m_sChangeNum] = m_SBag[pGridCont->sID].sReverseID;
			m_sChangeNum++;
		}
	}

	// ���漼��
	m_SBag[pGridCont->sID].chState = pGridCont->chState;
	m_SBag[pGridCont->sID].chLv = pGridCont->chLv;
	m_SBag[pGridCont->sID].lColdDownT = 0;
#endif

	return true;
}

inline bool CSkillBag::DelSkill(short sSkillID)
{
#ifdef COMPACT_MEM
	if (sSkillID < 0 || sSkillID > defMAX_SKILL_NO)
		return false;

	map<short,  SSkillGrid>::iterator it = m_SBagMap.find(sSkillID);

	// �ҵ�����ɾ��
	if(it != m_SBagMap.end())
	{
		m_SBagMap.erase(it);
	}

	// ɾ�����ܣ��Ѽ��ܵȼ��޸�Ϊ0��ʾɾ��
	it = m_SBagMap.find(sSkillID);
	
	if(it != m_SBagMap.end())
	{
		SSkillGrid grid;
		grid.sID = sSkillID;
		grid.chLv = 0;

		m_ChangeMap.insert(map<short,  SSkillGrid>::value_type(sSkillID, grid));
	}
	else
	{
		it->second.chLv = 0;
	}

#else
	if (sSkillID < 0 || sSkillID > defMAX_SKILL_NO)
		return false;

	// ɾ�����ܣ�ֻ�ǰѼ��ܵȼ��޸�Ϊ0��ʾɾ����
	if (m_SBag[sSkillID].chLv != 0)
	{
		m_sSkillNum--;
		m_SBag[sSkillID].chLv = 0;
		m_pSBag[m_SBag[sSkillID].sReverseID] = m_pSBag[m_sSkillNum];
		m_pSBag[m_sSkillNum]->sReverseID = m_SBag[sSkillID].sReverseID;
		m_pSBag[m_sSkillNum] = &m_SBag[sSkillID];
		m_SBag[sSkillID].sReverseID = m_sSkillNum;

		if (m_szSkillIsChanged[sSkillID] == 0)
		{
			m_szSkillIsChanged[sSkillID] = 1;
			m_sChangeSkill[m_sChangeNum] = m_sSkillNum;
			m_sChangeNum++;
		}
	}
#endif
	return true;
}

inline SSkillGrid* CSkillBag::GetSkillContByID(short sSkillID)
{
#ifdef COMPACT_MEM
	if (sSkillID < 0 || sSkillID > defMAX_SKILL_NO)
		return 0;

	map<short,  SSkillGrid>::iterator it = m_SBagMap.find(sSkillID);
	if(it == m_SBagMap.end())
		return 0;
	else
		return &it->second;

#else
	if (sSkillID < 0 || sSkillID > defMAX_SKILL_NO)
		return 0;

	if (m_SBag[sSkillID].chLv == 0)
		return 0;
	else
		return m_SBag + sSkillID;
#endif
}

#ifdef COMPACT_MEM
inline SSkillGrid* CSkillBag::GetFirstSkill()
{
	if(m_SBagMap.size() == 0)
		return 0;
	
	return &m_SBagMap.begin()->second;
}
#else
inline SSkillGrid* CSkillBag::GetSkillContByNum(short sNum)
{
	if (sNum < 0 || sNum >= m_sSkillNum)
		return 0;

	if (m_pSBag[sNum]->chLv == 0)
		return 0;
	else
		return m_pSBag[sNum];
}
#endif

inline short CSkillBag::GetSkillNum()
{
#ifdef COMPACT_MEM
	return (short)m_SBagMap.size();
#else
	return m_sSkillNum;
#endif
}

inline bool CSkillBag::HasSkill(short sSkillID)
{
#ifdef COMPACT_MEM
	if (sSkillID < 0 || sSkillID > defMAX_SKILL_NO)
		return false;

	map<short,  SSkillGrid>::iterator it = m_SBagMap.find(sSkillID);
	if(it != m_SBagMap.end())
		return true;
#else
	if (sSkillID < 0 || sSkillID > defMAX_SKILL_NO)
		return false;

	if (m_SBag[sSkillID].chLv > 0)
		return true;
#endif
	return false;
}

inline bool CSkillBag::SetSkillState(short sID, char chState)
{
#ifdef COMPACT_MEM
	if (sID < 0) // �������еļ���
	{
		m_ChangeMap.clear();

		for(map<short,  SSkillGrid>::iterator it = m_SBagMap.begin(); it != m_SBagMap.end(); it++)
		{
			if(it->second.chState == chState)
				continue;

			it->second.chState = chState;
			m_ChangeMap.insert(map<short,  SSkillGrid>::value_type(it->first, it->second));
		}
	}
	else
	{
		if (sID > defMAX_SKILL_NO)
			return false;
		else
		{
			map<short,  SSkillGrid>::iterator it = m_SBagMap.find(sID);
			if(it != m_SBagMap.end())
			{
				if(it->second.chState != chState)
				{
					it->second.chState = chState;
					
					map<short,  SSkillGrid>::iterator change = m_ChangeMap.find(sID);
					if(change == m_ChangeMap.end())
						m_ChangeMap.insert(map<short,  SSkillGrid>::value_type(it->first, it->second));
					else
					{
						change->second.chState = chState;	
					}
				}
			}
		}
	}
#else
	if (sID < 0) // �������еļ���
	{
		for (short i = 0; i < m_sSkillNum; i++)
		{
			if (m_pSBag[i]->chState == chState)
				continue;
			m_pSBag[i]->chState = chState;
			if (m_szSkillIsChanged[m_pSBag[i]->sID] == 0)
			{
				m_szSkillIsChanged[m_pSBag[i]->sID] = 1;
				m_sChangeSkill[i] = i;
				m_sChangeNum++;
			}
		}
	}
	else
	{
		if (sID > defMAX_SKILL_NO)
			return false;
		else
		{
			if (m_SBag[sID].chState != chState)
			{
				m_SBag[sID].chState = chState;
				if (m_szSkillIsChanged[sID] == 0)
				{
					m_szSkillIsChanged[sID] = 1;
					m_sChangeSkill[m_sChangeNum] = m_SBag[sID].sReverseID;
					m_sChangeNum++;
				}
			}
		}
	}
#endif
	return true;
}

inline char CSkillBag::GetSkillState(short sID)
{
#ifdef COMPACT_MEM
	if (sID < 0 || sID > defMAX_SKILL_NO)
		return enumSUSTATE_INACTIVE;

	map<short,  SSkillGrid>::iterator it = m_SBagMap.find(sID);
	if(it == m_SBagMap.end())
		return enumSUSTATE_INACTIVE;

	return it->second.chState;
#else
	if (sID < 0 || sID > defMAX_SKILL_NO)
		return enumSUSTATE_INACTIVE;

	if (m_SBag[sID].chLv == 0)
		return enumSUSTATE_INACTIVE;

	return m_SBag[sID].chState;
#endif
}

inline short CSkillBag::GetChangeSkillNum()
{
#ifdef COMPACT_MEM
	return (short)m_ChangeMap.size();
#else
	return m_sChangeNum;
#endif
}

#ifdef COMPACT_MEM
#else
inline SSkillGrid* CSkillBag::GetChangeSkill(short sChangeID)
{
	if (sChangeID < 0 || sChangeID >= m_sChangeNum)
		return 0;

	if (m_sChangeSkill[sChangeID] < 0 || m_sChangeSkill[sChangeID] > defMAX_SKILL_NO)
		return 0;

	return m_pSBag[m_sChangeSkill[sChangeID]];
}
#endif

//=============================================================================
inline char* SkillBagData2String(CSkillBag *pSkillBag, char *szStrBuf, int nLen)
{
#ifdef COMPACT_MEM
	if (!pSkillBag || !szStrBuf) return NULL;

	char	szData[512];
	int		nBufLen = 0, nDataLen;
	szStrBuf[0] = '\0';

	SSkillGrid *pGridCont;
	//sprintf(szData, "%d;", pSkillBag->m_sSkillNum);
	_snprintf_s(szData, sizeof(szData), _TRUNCATE, "%d;", pSkillBag->GetSkillNum());
	nDataLen = (int)strlen(szData);
	if (nBufLen + nDataLen >= nLen) return NULL;
	//strcat(szStrBuf, szData);
	strncat_s(szStrBuf, nLen, szData, _TRUNCATE);
	nBufLen += nDataLen;
	for(map<short,  SSkillGrid>::iterator it = pSkillBag->m_SBagMap.begin(); it != pSkillBag->m_SBagMap.end(); it++)
	{
		pGridCont = &it->second;
		//sprintf(szData, "%d,%d;", pGridCont->sID, pGridCont->chLv);
		_snprintf_s(szData, sizeof(szData), _TRUNCATE, "%d,%d;", pGridCont->sID, pGridCont->chLv);
		nDataLen = (int)strlen(szData);
		if (nBufLen + nDataLen >= nLen) return NULL;
		//strcat(szStrBuf, szData);
		strncat_s(szStrBuf, nLen, szData, _TRUNCATE);
		nBufLen += nDataLen;
	}
#else

	if (!pSkillBag || !szStrBuf) return NULL;

	char	szData[512];
	int		nBufLen = 0, nDataLen;
	szStrBuf[0] = '\0';

	SSkillGrid *pGridCont;
	//sprintf(szData, "%d;", pSkillBag->m_sSkillNum);
	_snprintf_s(szData, sizeof(szData), _TRUNCATE, "%d;", pSkillBag->GetSkillNum());
	nDataLen = (int)strlen(szData);
	if (nBufLen + nDataLen >= nLen) return NULL;
	//strcat(szStrBuf, szData);
	strncat_s(szStrBuf, nLen, szData, _TRUNCATE);
	nBufLen += nDataLen;
	for (int j = 0; j < pSkillBag->GetSkillNum(); j++)
	{
		pGridCont = pSkillBag->m_pSBag[j];
		//sprintf(szData, "%d,%d;", pGridCont->sID, pGridCont->chLv);
		_snprintf_s(szData, sizeof(szData), _TRUNCATE, "%d,%d;", pGridCont->sID, pGridCont->chLv);
		nDataLen = (int)strlen(szData);
		if (nBufLen + nDataLen >= nLen) return NULL;
		//strcat(szStrBuf, szData);
		strncat_s(szStrBuf, nLen, szData, _TRUNCATE);
		nBufLen += nDataLen;
	}
#endif
	return szStrBuf;
}

inline const char* SkillBagData2StringEx(CSkillBag *pSkillBag, StringPoolL& lookString, int index)
{
#ifdef COMPACT_MEM
	if (!pSkillBag) return NULL;

	char	szData[512];

	SSkillGrid *pGridCont;
	lookString[index].Printf("%d;", pSkillBag->GetSkillNum());

	for(map<short,  SSkillGrid>::iterator it = pSkillBag->m_SBagMap.begin(); it != pSkillBag->m_SBagMap.end(); it++)
	{
		pGridCont = &it->second;
		_snprintf_s(szData, sizeof(szData), _TRUNCATE, "%d,%d;", pGridCont->sID, pGridCont->chLv);

		lookString[index] += szData;
	}
#else
	if (!pSkillBag) return NULL;

	char	szData[512];

	SSkillGrid *pGridCont;
	lookString[index].Printf("%d;", pSkillBag->GetSkillNum());

	for (int j = 0; j < pSkillBag->m_sSkillNum; j++)
	{
		pGridCont = pSkillBag->m_pSBag[j];
		_snprintf_s(szData, sizeof(szData), _TRUNCATE, "%d,%d;", pGridCont->sID, pGridCont->chLv);

		lookString[index] += szData;
	}
#endif
	return (const char*)lookString[index];
}

inline bool String2SkillBagData(CSkillBag *pSkillBag, std::string &strData)
{
	if (!pSkillBag)
		return false;

	pSkillBag->InitSkill();

	int nCount = 0;
	std::string strList[defMAX_SKILL_NO + 1];
	std::string strSubList[2];
	Util_ResolveTextLine(strData.c_str(), strList, defMAX_SKILL_NO + 1, ';');
	short sSkillNum = Str2Int(strList[nCount++]);
	SSkillGrid SGridCont;
	for (int j = 0; j < sSkillNum; j++)
	{
		Util_ResolveTextLine(strList[nCount++].c_str(), strSubList, 3, ',');
		SGridCont.sID = Str2Int(strSubList[0]);
		SGridCont.chLv = Str2Int(strSubList[1]);

		SGridCont.chState = enumSUSTATE_INACTIVE;
		pSkillBag->AddSkill(&SGridCont);
	}

	return true;
}

#endif // SKILLBAG_H