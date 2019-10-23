#include "Stdafx.h"
#include "Character.h"
#include "Point.h"
#include "LEShadeMap.h"
#include "leparticlectrl.h"
#include "Scene.h"
#include "SceneItem.h"
#include "CharacterAction.h"
#include "actor.h"
#include "STStateObj.h"
//add by ALLEN 2007-10-16
#include "state_reading.h"

#include "EffectObj.h"
#include "SkillRecord.h"
#include "worldscene.h"
#include "ItemRecord.h"
#include "EffectObj.h"
#include "terrainattrib.h"
#include "GameApp.h"
#include "MusicSet.h"
#include "STAttack.h"
#include "UIHeadSay.h"
#include "UICommand.h"
#include "CommFunc.h"
#include "effdelay.h"
#include "SkillStateRecord.h"
#include "uistartform.h"
#include "netprotocol.h"
#include "uiboatform.h"
#include "steadyframe.h"
#include "event.h"
#include "chastate.h"
#include "shipset.h"
#include "SceneHeight.h"
#include "stpose.h"
#include "UIBoothForm.h"
#include "stmove.h"
#include "UIEquipForm.h"
#include "UIItemCommand.h"
#include "UIMiniMapForm.h"
#include "caLua.h"
#include "PacketCmd.h"
#include "..\inc\Mount.h"


extern CGameApp*		g_pGameApp;
extern bool				g_IsShowShop;

// ========================
CMount::CMount(CCharacter* character, int mountID)
: prevMountID(0)
{
  
	this->pCha = character;
	this->curMountID = _Attr.get(ATTR_EXTEND9);

	_pActor			= new CActor( character );
	
    _Attr.set(ATTR_MXHP,  20);
    _Attr.set(ATTR_HP,     20);
    _Attr.set(ATTR_MSPD, 400);

//	_pHeadSay		= new CHeadSay( character );


	_pChaState = new CChaStateMgr(character);
	_ChaState.AllTrue();


}

CMount::~CMount() 
{
	//delete _pActor;
	//delete _pChaState;
	//delete _pSceneHeight;

	SAFE_DELETE(_pActor);       // UIµ±»ú´¦Àí
	SAFE_DELETE(_pChaState);    // UIµ±»ú´¦Àí
	//SAFE_DELETE(_pHeadSay);
} 

/*
void CCharacter::Render() {
	if (!_pScene->IsPointVisible((float)_nCurX / 100.0f, (float)_nCurY / 100.0f)) {
		return;
	}

	g_Render.EnableZBuffer(TRUE);
	CSceneNode::Render();
	CCharacterModel::Render();

	// Mount Start

	if (this->GetScene()->GetSceneTypeID() == enumWorldScene)
	{

		// Get Mount ID
		CItemCommand* itemData;

		itemData = (g_stUIEquip.GetEquipItem(enumEQUIP_MOUNTS) != NULL) ? g_stUIEquip.GetEquipItem(enumEQUIP_MOUNTS) : NULL;

		// check data before moving forward
		//assert(itemData != nullptr);

		int mountID = 791;//(itemData != NULL) ? itemData->GetItemInfo()->sItemEffect[0] : 0; // Store Mount ID in effect
		int mountHeightOff = (itemData != NULL) ? itemData->GetItemInfo()->sItemEffect[1] : 0; // Store Mount ID in effect
		mount = new CMount(mountID, mountHeightOff, GetTypeID(), GetCurX(), GetCurY());


		if (mountID > 0 && IsMainCha())
		{
			SetMountID(mountID);
		}

		// If the player has no mount AND mount attr is set
		// For testing purposes I just checked for lv...
		if (!GetIsMount() && _Attr.get(ATTR_EXTEND9) > 0) {

			// pChaMount is a nested CCharacter 
			// ID 791 is Baby Black Dragon from CharacterInfo
			pChaMount = _pScene->AddCharacter(_Attr.get(ATTR_EXTEND9));

			// To remove the "Player" default name
			pChaMount->setName("");

			// Summon the pChaMount at the exact position of the character
			pChaMount->setPos(mount->GetMountX(), mount->GetMountY());

			// Should really make a new control type enumCHACTRL_MOUNT
			pChaMount->setChaCtrlType(enumCHACTRL_PLAYER_PET);

			// Initial Values for the prev coordinates(Will be used to switch between poses)
			prevX = GetCurX();
			prevY = GetCurY();

			// Elevate the character
			if (mountHeightOff > 0)
				setHeightOff(mount->GetHeightOff());
			else
				setHeightOff(145);

			// Initial Seat
			PlayPose(GetPose(mount->GetCharacterPose()), PLAY_LOOP_SMOOTH);

			// Pose velocity = MSPD / 800 (For 60FPS)
			pChaMount->SetPoseVelocity((float)pChaMount->getMoveSpeed() / 800.0);

			// Record the mount state
			SetIsMount(true);
		}

		// If the character is mounted AND mount attr is not set or set to -1
		// For test purposes it's just the reverse check lv condition
		if (GetIsMount() && mountID == 0) {

			// Marks the character for destruction
			pChaMount->SetValid(false);

			// Send him back to the ground...
			setHeightOff(0);

			// Record the mount state
			SetIsMount(false);

			this->chMountID = 0;

			// STAND UP
			PlayPose(GetPose(POSE_WAITING), PLAY_LOOP_SMOOTH);
		}

		// If mount is running then...
		if (GetIsMount()) {

			// Contiounsly update the mount's MSPD to character's MSPD so that they move in SYNC
			pChaMount->setMoveSpeed(getMoveSpeed());

			// Contiously update the character's angle to match that of the player.
			pChaMount->setYaw(getYaw());

			int curX = GetCurX();
			int curY = GetCurY();

			// If there exists a change in coordinates, change pose and move...
			if (curX != prevX || curY != prevY) {

				// Play the running pose.
				pChaMount->PlayPose(pChaMount->GetPose(POSE_RUN), PLAY_LOOP_SMOOTH);

				// Move
				pChaMount->ForceMove(curX, curY);

				// Update prev coordinates
				prevX = curX;
				prevY = curY;
			}
			else {

				// If the character stands still then run the waiting pose.
				pChaMount->PlayPose(pChaMount->GetPose(POSE_WAITING), PLAY_LOOP_SMOOTH);
			}
		}
	}

	}
	// MOUNT_SYSTEM_END
	*/


void CMount::Render(int id) {
if (!_pScene->IsPointVisible((float)_nCurX / 100.0f, (float)_nCurY / 100.0f)) {
	return;
}

g_Render.EnableZBuffer(TRUE);
CSceneNode::Render();
CCharacterModel::Render();

// If the player has no mount AND mount attr is set
// For testing purposes I just checked for lv...
if (!GetIsMount() && _Attr.get(ATTR_EXTEND9) > 0) {

		pCha->SetMountID(_Attr.get(ATTR_EXTEND9));

		if(pCha->GetPrevMountID() == 0)
			pCha->SetPrevMountID(_Attr.get(ATTR_EXTEND9));

		pChaMount = _pScene->AddCharacter(_Attr.get(ATTR_EXTEND9));

		if(pCha->GetPrevMountID() == 0)
			pCha->SetMountID(pCha->GetMountID());

		// To remove the "Player" default name
		pChaMount->setName("");

		

		// Summon the pChaMount at the exact position of the character
		pChaMount->setPos(GetCurX(), GetCurY());

		// Should really make a new control type enumCHACTRL_MOUNT
		pChaMount->setChaCtrlType(enumCHACTRL_PLAYER_MOUNT);

		// Initial Values for the prev coordinates(Will be used to switch between poses)
		prevX = GetCurX();
		prevY = GetCurY();

		// Elevate the character
		setHeightOff(145);

		// Initial Seat
		PlayPose(pCha->GetPose(POSE_SEAT), PLAY_LOOP_SMOOTH);

		// Pose velocity = MSPD / 800 (For 60FPS)
		pChaMount->SetPoseVelocity((float)pChaMount->getMoveSpeed() / 800.0);

		// Record the mount state
		SetIsMount(true);
}

// If the character is mounted AND mount attr is not set or set to -1
// For test purposes it's just the reverse check lv condition
//if ((GetIsMount() && _Attr.get(ATTR_EXTEND9) <= 0) && (GetIsMount() && _Attr.get(ATTR_EXTEND9) != this->prevMount)) {
if((GetIsMount() && _Attr.get(ATTR_EXTEND9) != prevMount) || (GetIsMount() && _Attr.get(ATTR_EXTEND9) == 0)){
	// Marks the character for destruction

	if (prevMount > 0 && GetMountID() > 0) {
		prevMount = GetMountID();
		pChaMount->ReCreate(GetMountID());
	}
	else {

		pChaMount->SetValid(false);

		// Send him back to the ground...
		setHeightOff(0);

		// Record the mount state
		SetIsMount(false);

		// STAND UP
		PlayPose(pCha->GetPose(POSE_WAITING), PLAY_LOOP_SMOOTH);
	}
}

// If mount is running then...

if (GetIsMount()) {

	// Contiounsly update the mount's MSPD to character's MSPD so that they move in SYNC
	pChaMount->setMoveSpeed(pCha->getMoveSpeed());

	// Contiously update the character's angle to match that of the player.
	pChaMount->setYaw(getYaw());

	int curX = GetCurX();
	int curY = GetCurY();
	int curMount = _Attr.get(ATTR_EXTEND9);

	// If there exists a change in coordinates, change pose and move...
	if (curX != prevX || curY != prevY) {

		if (_Attr.get(ATTR_EXTEND9) != prevMount)
		{
			pChaMount->ReCreate(GetMountID());
		}

		// Play the running pose.
		pChaMount->PlayPose(pChaMount->GetPose(POSE_RUN), PLAY_LOOP_SMOOTH);

		// Move
		pChaMount->ForceMove(curX, curY);

		// Update prev coordinates
		prevX = curX;
		prevY = curY;
	}
	else {

		// If the character stands still then run the waiting pose.
		pChaMount->PlayPose(pChaMount->GetPose(POSE_WAITING), PLAY_LOOP_SMOOTH);
	}

}

// MOUNT_SYSTEM_END
}

/*
void CCharacter::Render() {
	if (!_pScene->IsPointVisible((float)_nCurX / 100.0f, (float)_nCurY / 100.0f)) {
		return;
	}

	g_Render.EnableZBuffer(TRUE);
	CSceneNode::Render();
	CCharacterModel::Render();

	// Mount Start

	if (this->GetScene()->GetSceneTypeID() == enumWorldScene)
	{

		// Get Mount ID
		CItemCommand* itemData;

		itemData = (g_stUIEquip.GetEquipItem(enumEQUIP_MOUNTS) != NULL) ? g_stUIEquip.GetEquipItem(enumEQUIP_MOUNTS) : NULL;

		// check data before moving forward
		//assert(itemData != nullptr);

		int mountID = (itemData != NULL) ? itemData->GetItemInfo()->sItemEffect[0] : 0; // Store Mount ID
		int mountHeightOff = (itemData != NULL) ? itemData->GetItemInfo()->sItemEffect[1] : 0; // Store Mount height offset

		mount = new CMount(mountID, mountHeightOff, GetTypeID(), GetCurX(), GetCurY());

		// If the player has no mount AND mount attr is set
		// For testing purposes I just checked for lv...
		if (!GetIsMount() && mount->mountID > 0 && IsPlayer() && !IsNPC() && !IsMonster()) {

			// pChaMount is a nested CCharacter 
			// ID 791 is Baby Black Dragon from CharacterInfo
			pChaMount = _pScene->AddCharacter(mount->mountID);

			// To remove the "Player" default name
			pChaMount->setName("");

			// Summon the pChaMount at the exact position of the character
			pChaMount->setPos(mount->GetMountX(), mount->GetMountY());

			// Should really make a new control type enumCHACTRL_MOUNT
			pChaMount->setChaCtrlType(enumCHACTRL_PLAYER_PET);

			// Initial Values for the prev coordinates(Will be used to switch between poses)
			prevX = GetCurX();
			prevY = GetCurY();

			// Elevate the character
			if (mountHeightOff > 0)
				setHeightOff(mount->GetHeightOff());
			else
				setHeightOff(145);

			// Initial Seat
			PlayPose(GetPose(mount->GetCharacterPose()), PLAY_LOOP_SMOOTH);

			// Pose velocity = MSPD / 800 (For 60FPS)
			pChaMount->SetPoseVelocity((float)pChaMount->getMoveSpeed() / 800.0);

			// Record the mount state
			SetIsMount(true);
		}
		// If the character is mounted AND mount attr is not set or set to -1
		// For test purposes it's just the reverse check lv condition
		if (GetIsMount() && mount->mountID == 0) {

			// Marks the character for destruction
			pChaMount->SetValid(false);

			// Send him back to the ground...
			setHeightOff(0);

			// Record the mount state
			SetIsMount(false);

			// STAND UP
			PlayPose(GetPose(POSE_WAITING), PLAY_LOOP_SMOOTH);

			mount = nullptr;
		}



		// If mount is running then...
		if (GetIsMount()) {

			// Contiounsly update the mount's MSPD to character's MSPD so that they move in SYNC
			pChaMount->setMoveSpeed(getMoveSpeed());

			// Contiously update the character's angle to match that of the player.
			pChaMount->setYaw(getYaw());

			int curX = GetCurX();
			int curY = GetCurY();

			// If there exists a change in coordinates, change pose and move...
			if (GetCurX() != prevX || GetCurY() != prevY) {

				// Play the running pose.
				pChaMount->PlayPose(pChaMount->GetPose(POSE_RUN), PLAY_LOOP_SMOOTH);

				mount->UpdateX(GetCurX());
				mount->UpdateY(GetCurY());

				// Move
				pChaMount->ForceMove(curX, curY);

				// Update prev coordinates

				prevX = GetCurX();
				prevY = GetCurY();
			}
			else {

				// If the character stands still then run the waiting pose.
				pChaMount->PlayPose(pChaMount->GetPose(POSE_WAITING), PLAY_LOOP_SMOOTH);
			}
		}
	}
	// MOUNT_SYSTEM_END
}*/