//----------------------------------------------------------------------
// ����:������Ҽ���״̬
// ����:lh 2005-05-31
//----------------------------------------------------------------------
#pragma once

#include "boolset.h"
#include "skillstatetype.h"

struct stSkillState;
class CCharacter;
class CEffectObj;
class CSkillStateRecord;

class CChaStateMgr
{
public:
	CChaStateMgr(CCharacter* pCha);

	void		ChaDestroy();									// ��ɫ��Чʱ����
	void		ChaDied()	{ ChaDestroy();	}					// ��ɫ����ʱ����

	CBoolSet&	Synchro( stSkillState* pState, int nCount );	// ͬ��״̬ʱ����

	int					GetSkillStateNum()					{ return (int)_states.size();	}
	CSkillStateRecord*	GetSkillState( unsigned int nID )	{ return _states[nID]->pInfo;	}

	bool		HasSkillState( unsigned int nID );

	static CSkillStateRecord* GetLastActInfo()				{ return _pLastActInfo;			}
	static int				  GetLastShopLevel()			{ return _nShopLevel;			}

private:
	CCharacter*		_pCha;

	struct stChaState
	{
		bool				IsDel;

		unsigned char		chStateLv;
		CSkillStateRecord*	pInfo;
		CEffectObj*			pEffect;
	};

	stChaState		_sChaState[SKILL_STATE_MAXID];
	typedef vector<stChaState*>	states;
	states			_states;

	static	bool		_IsInit;
	static stChaState	_sInitState[SKILL_STATE_MAXID];

	static CSkillStateRecord*	_pLastActInfo;		// ����ͬ��ʱ�������һ���ж������ֵ�����
	static int					_nShopLevel;		// ����ʱ���ذ�̯״̬�ȼ�

};


inline bool CChaStateMgr::HasSkillState( unsigned int nID )		
{
	return nID<SKILL_STATE_MAXID && _sChaState[nID].chStateLv;
}
