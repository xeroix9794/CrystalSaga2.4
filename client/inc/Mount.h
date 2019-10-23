#pragma once
#include "Scene.h"
#include "SceneNode.h"
#include "Character.h"
#include "LECharacter.h"
#include "CompCommand.h"
#include "CharacterModel.h"
#include "publicfun.h"
#include "CharacterAction.h"
#include "BoolSet.h"
#include "CharacterRecord.h"
#include "Actor.h"
#include "FindPath.h"
#include "SteadyFrame.h"

class CShadeEff;
class CEffectObj;
class CGameScene;
class CActor;
class CSkillRecord;
class CChaRecord;
class CSkillStateRecord;
struct stSkillState;
class CEvent;
struct xShipInfo;
class CSceneHeight;

namespace GUI
{
	class CHeadSay;
}

class CMount : public CSceneNode, public CCharacterModel
{
	friend class CHeadSay;
	friend class CChaStateMgr;
	friend class CCharacter;

public:
	void CreateMount();
	void DestoryMount();


private:
	bool bIsMount = false;
	int	 curMountID = 0;
	int prevMountID = 0;
public:
	// This is public and this is wrong but meh it's a mess anyways
	CCharacter* pCha;
	CCharacter* pChaMount;
	bool GetIsMount() { return bIsMount; }
	void SetIsMount(bool isMount) { this->bIsMount = isMount; }
	int GetMountID() { return _Attr.get(ATTR_EXTEND9); }
	void SetMountID() { this->curMountID = _Attr.get(ATTR_EXTEND9);}
	int GetPrevMountID() { return prevMountID; }
	int SetPrevMountID(int prevID) { this->prevMountID = prevID; }
public:
	CMount(CCharacter* character, int mountID = 0);
	virtual ~CMount();
	void Render(int id);

public: // 技能运行状态    
	CChaStateMgr*	GetStateMgr()						{ return _pChaState;			}

private:
	CChaStateMgr*	_pChaState;
	CBoolSet		_ChaState;			// 玩家状态集合
	CChaRecord*     _pDefaultChaInfo;
	CActor*         _pActor;

	int prevX, prevY, prevMount;

};