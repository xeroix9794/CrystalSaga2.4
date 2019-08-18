//
#pragma once

#include "LightEngine.h"


class RenderStateMgr
{
private:
    drIDeviceObject* _dev_obj;
    drIRenderStateAtomSet* _rsa_scene;
    drIRenderStateAtomSet* _rsa_cha;
    drIRenderStateAtomSet* _rsa_sceneobj;
    drIRenderStateAtomSet* _rsa_sceneitem;
    drIRenderStateAtomSet* _rsa_terrain;
    drIRenderStateAtomSet* _rsa_transpobj;

    D3DLIGHTX _scnobj_lgt;
    D3DLIGHTX _scnobj_lgt_old[3];
    BOOL _scnobj_lgt_enable_old[3];

    D3DLIGHTX _cha_lgt;
    D3DLIGHTX _cha_lgt_old;
    BOOL _cha_lgt_enable_old;

public:
    RenderStateMgr();
    ~RenderStateMgr();

    HRESULT Init(drIDeviceObject* dev_obj);

    HRESULT BeginScene();
    HRESULT BeginCharacter();
    HRESULT BeginSceneObject();
    HRESULT BeginSceneItem();
    HRESULT BeginTerrain();
    HRESULT BeginTranspObject();
    HRESULT EndScene();
    HRESULT EndCharacter();
    HRESULT EndSceneObject();
    HRESULT EndSceneItem();
    HRESULT EndTerrain();
    HRESULT EndTranspObject();

};
