//=============================================================================
// FileName: SkillTemp.h
// Creater: ZhangXuedong
// Date: 2005.03.03
// Comment: Skill Temp Data
//=============================================================================

#ifndef SKILLTEMP_H
#define SKILLTEMP_H

#include "PreAlloc.h"

#define defSKILL_RANGE_BASEP_NUM	3	// �������������������Χ�������꣬����
#define defSKILL_RANGE_EXTEP_NUM	4	// ������չ������������Χ���ͣ����Ͳ�����
#define defSKILL_STATE_PARAM_NUM	3	// ����״̬������������ţ��ȼ�������ʱ�䣩

class CSkillTempData : public dbc::PreAllocStru
{
public:
	CSkillTempData(unsigned long ulSize = 1) : PreAllocStru(1)
	{
		sUseEndure = 0;
		sUseEnergy = 0;
		sUseSP = 0;
		lResumeTime = 0;
		sRange[0] = enumRANGE_TYPE_NONE;
		sStateParam[0] = SSTATE_NONE;
	}

	short	sUseEndure;	// �ԡ��;öȡ�������
	short	sUseEnergy;	// �ԡ�������������
	short	sUseSP;		// �ԡ�SP��������

	long	lResumeTime;// ���ͷ���Ҫ��ʱ�䣨���룩
	short	sRange[defSKILL_RANGE_EXTEP_NUM];	// ��������������+������
	short	sStateParam[defSKILL_STATE_PARAM_NUM];	// ״̬��ţ��ȼ�������ʱ��

protected:

private:
	void	Initially();
	void	Finally();

};

inline void CSkillTempData::Initially()
{
	sUseEndure = 0;
	sUseEnergy = 0;
	sUseSP = 0;
	lResumeTime = 0;
	sRange[0] = enumRANGE_TYPE_NONE;
	sStateParam[0] = SSTATE_NONE;
}

inline void CSkillTempData::Finally()
{
}

#endif // SKILLTEMP_H
