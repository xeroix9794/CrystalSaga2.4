#include "stdafx.h"
#include "SceneObj.h"
#include "SceneObjSet.h"
#include "Scene.h"
#include "LuaInterface.h"
#include "EffectObj.h"
#include "Character.h"
#include "SceneItem.h"
#include "worldscene.h"
#include "scenelight.h"
#include "LitLoad.h"
#include "GameApp.h"

CSceneObjSet* CSceneObjSet::_Instance = NULL;

CSceneObj::CSceneObj()
: CSceneNode(), _dwObjType( SCENEOBJ_TYPE_NORMAL ), _nMusicID(-1)
{
	_vPos = VECTOR3(0,0,0);

	_pTerrain = NULL;
	_iChaID = -1;
	_iOwerType= -1;
	_dwLastUpdateTime = 0;

    _dwCullingFlag = 1;
    _dwTranspFlag = 1;
    _dwTranspState = 0;
    _fTranspValue = 1.0f;

}

BOOL CSceneObj::_Create(int nScriptID,int nType)
{
	CSceneObjInfo *pInfo = GetSceneObjInfo(nScriptID);
    if(pInfo==NULL)
    {
        LG("scene", RES_STRING(CL_LANGUAGE_MATCH_354), nScriptID);
        return FALSE;
    }

    ///*
	if(S_OK!=Load(pInfo->szDataName, nScriptID))
    {
        LG("scene", "msg�������������ID��Ч : TypeID = %d\n", nScriptID);
        return FALSE;
    }
    //*/
    
    // ShowBoundingObject( 1 );
    // ShowHelperMesh( 1 );

	//lemon add flag for minimap
	setObjType(pInfo->nFlag);

    PlayDefaultAnimation();

	_nMusicID = -1;

    SceneLight::Reset();

    setYaw(0);

	//��Ұ��������Ч�ر�֧�� by Waiting 2009-07-28
	//��;���ڵر�߶ȣ�0�ĵط�����һ���͸����Ч��Ӱ����Ϊ��ر�λ�ù�����������Ҫʹ��ZBIAS ������Ƭ�����֧��16�㣬����Խ��Խ�ϲ� 
	if( pInfo->nAttachEffectID>0 && pInfo->nAttachEffectID<=16 )
	{
		DWORD num = this->GetPrimitiveNum();
		for(DWORD i = 0; i < num; i++)
		{
			assert(GetPrimitive(i));
			assert(GetPrimitive(i)->GetMeshAgent());
			assert(GetPrimitive(i)->GetMeshAgent()->GetMesh());
			GetPrimitive(i)->GetMeshAgent()->GetMesh()->AddStateToSet(D3DRS_ZBIAS, pInfo->nAttachEffectID);
		}
	}

    // debug by lsh
//     extern LitMgr g_lit_mgr;
//     DWORD num = this->GetPrimitiveNum();
//     for(DWORD i = 0; i < num; i++)
//     {
//         LitInfo* l = g_lit_mgr.Lit(1, i, pInfo->szDataName);
//         if(l)
//         {
//             drIPrimitive* p = GetPrimitive(i);
//             drPrimitiveTexLit(p, l->str_buf[0], ".\\texture\\lit\\", l->color_op, l->anim_type);
//         }
//     }
    // end

    return TRUE;
}

void CSceneObj::Render()
{
    if(_dwCullingFlag)
    {

        DWORD x = 0;
        drIModel* model = LESceneObject::GetObject();
        DWORD pri_num = model->GetPrimitiveNum();

        for( DWORD i = 0; i < pri_num; i++ )
        {
            drIPrimitive* p = LESceneObject::GetObject()->GetPrimitive( i );
            drMatrix44* m = p->GetMatrixGlobal();
            drIBoundingBox* box = p->GetHelperObject()->GetBoundingBox();
            if( box == NULL || box->GetObjNum() == 0 )
            {
                model->RenderPrimitive( i );
                x += 1;
                continue;
            }

            drMatrix44 mat;
            drBoundingBoxInfo* box_info = box->GetDataInfo( 0 );
            drMatrix44Multiply( &mat, &box_info->mat, m );

            drVector3 u = box_info->box.c - box_info->box.r;
            drVector3 v = box_info->box.c + box_info->box.r;
            drVec3Mat44Mul( &u, &mat );
            drVec3Mat44Mul( &v, &mat );

            BOOL bInTerrainRange = FALSE;
            LETerrain * pTerrain = GetScene()->GetTerrain();
            if(pTerrain)
            {
                if( pTerrain->IsPointVisible( u.x, u.y ) || pTerrain->IsPointVisible( u.x, v.y ) || 
                    pTerrain->IsPointVisible( v.x, v.y ) || pTerrain->IsPointVisible( v.x, u.y ) )
                {
                    bInTerrainRange = TRUE;
                }
            }
            else
            {
                bInTerrainRange = TRUE;
            }

            if(bInTerrainRange)
            {
                if(_pScene->IsSceneObjCulling()==FALSE)
                {
                    // added by billy ����Ӷ�����Ƿ�����׶����ж� (point 8 )              				
                    DWORD index = box->GetData();			   
                    if ( IsBoxVisible(  u ,v , index ) )
                    {
                        model->RenderPrimitive( i );
                        x += 1;;
                    }

                    box->SetData(index);
                }
                else
                {
                    // ��Ӷ�����Ƿ�����׶����ж� (point 8, line 12)
                    DWORD index = box->GetData();         				
                    if( IsBoxVisible_LineWithPlane(u,v, index) )
                    {
                        model->RenderPrimitive( i);
                        x += 1;
                    }   
                    box->SetData(index);
                }
            }
            else
            {
                int nnn = 1;
            }

        }
        if(x > 0 && x < pri_num)
        {
            int n1 = 1;
        }


        if( x > 0 )
        {
            model->RenderHelperObject();
        }
#ifdef __FPS_DEBUG__
        _pScene->m_dwRenderSceneObjCnt+=x;
#endif
    }
    else
    {
        LESceneObject::Render();
    }

	CSceneNode::Render();


	// g_Render.EnableMipmap(FALSE);
	

	// LESceneObject::Render();

	// int nX, nY; g_Render.GetScreenPos(nX, nY, D3DXVECTOR3((float)GetCurX() / 100.0f, (float)GetCurY() / 100.0f, 0.6f)); 
	// g_Render.Print(INFO_GAME, nX, nY - 40, "%d", _dwID); 
}


void CSceneObj::FrameMove(DWORD dwTimeParam)
{
	if(CGameApp::GetFrameFPS() == 30)
	{
		CSceneNode::FrameMove(dwTimeParam);

		setPos(_nCurX, _nCurY);
		//if(IsValid())
		//GetScene()->HandleSceneMsg(SCENEMSG_SCENEOBJ_MOVE,_nEffID,getID());
		//else
		//	GetScene()->HandleSceneMsg(SCENEMSG_SCENEOBJ_DESTROY,_nEffID,getID());

		// by lsh
		// ע�⣬�����FrameMove������������ã��������λ�����
		LESceneObject::FrameMove();	
	}
	else
	{
		if(_dwLastUpdateTime%2 == 0 )
		{
			CSceneNode::FrameMove(dwTimeParam);
			setPos(_nCurX, _nCurY);
			LESceneObject::FrameMove();	
			_dwLastUpdateTime++;
		}
		else
		{
			_dwLastUpdateTime++;
		}
	}

}


bool  CSceneObj::IsBoxVisible(  drVector3 vecMin, drVector3 vecMax, DWORD &index )
{
	static drVector3 vecBounds[8];

	vecBounds[0] =drVector3(vecMin.x, vecMin.y, vecMin.z );
	vecBounds[1] =drVector3(vecMax.x, vecMin.y, vecMin.z );
	vecBounds[2] =drVector3(vecMin.x, vecMax.y, vecMin.z );
	vecBounds[3] =drVector3(vecMax.x, vecMax.y, vecMin.z );
	vecBounds[4] =drVector3(vecMin.x, vecMin.y, vecMax.z );
	vecBounds[5] =drVector3(vecMax.x, vecMin.y, vecMax.z );
	vecBounds[6] =drVector3(vecMin.x, vecMax.y, vecMax.z );
	vecBounds[7] =drVector3(vecMax.x, vecMax.y, vecMax.z );

    BYTE bOutside[8];
    ZeroMemory( &bOutside, sizeof(bOutside) );

	LECullInfo *pCull = g_Render.GetCullInfo();

	//���ȼ���ϴμ�⵽��ƽ��ͷ�ڵĵ�

	for( int iPlane = 0; iPlane < 6; iPlane++ )
	{
		if( pCull->planeFrustum[iPlane].a * vecBounds[index].x +
            pCull->planeFrustum[iPlane].b * vecBounds[index].y +
            pCull->planeFrustum[iPlane].c * vecBounds[index].z +
            pCull->planeFrustum[iPlane].d < 0)
		{					
			bOutside[index] |= (1 << iPlane);	
		}
	
	}
#ifdef __FPS_DEBUG__
	_pScene->m_dwCullingTime++;
#endif
	if( bOutside[index] == 0 )
	{	
		index =index ;	
		return true ;
	}


    for( int iPoint = 0; iPoint < 8; iPoint++ )
    {
		
		if ( iPoint == (int)index)
			continue;

		for( int iPlane = 0; iPlane < 6; iPlane++ )
        {
            if( pCull->planeFrustum[iPlane].a * vecBounds[iPoint].x +
                pCull->planeFrustum[iPlane].b * vecBounds[iPoint].y +
                pCull->planeFrustum[iPlane].c * vecBounds[iPoint].z +
                pCull->planeFrustum[iPlane].d < 0)
            {
						
                bOutside[iPoint] |= (1 << iPlane);	
            }
		
		
        }
#ifdef __FPS_DEBUG__
        _pScene->m_dwCullingTime++;
#endif
        if( bOutside[iPoint] == 0 )
		{	
			index =iPoint ;
			return true;
		}
 	
    }
    
    return false;
}





bool  CSceneObj::IsBoxVisible_LineWithPlane(  drVector3 vecMin, drVector3 vecMax , DWORD &index)
{
//	return true;
	static D3DXVECTOR3 vecBounds[8];

	vecBounds[0] =D3DXVECTOR3(vecMin.x, vecMin.y, vecMin.z );
	vecBounds[1] =D3DXVECTOR3(vecMax.x, vecMin.y, vecMin.z );
	vecBounds[2] =D3DXVECTOR3(vecMin.x, vecMax.y, vecMin.z );
	vecBounds[3] =D3DXVECTOR3(vecMax.x, vecMax.y, vecMin.z );
	vecBounds[4] =D3DXVECTOR3(vecMin.x, vecMin.y, vecMax.z );
	vecBounds[5] =D3DXVECTOR3(vecMax.x, vecMin.y, vecMax.z );
	vecBounds[6] =D3DXVECTOR3(vecMin.x, vecMax.y, vecMax.z );
	vecBounds[7] =D3DXVECTOR3(vecMax.x, vecMax.y, vecMax.z );

    BYTE bOutside;
	ZeroMemory( &bOutside, sizeof(bOutside) );
	BYTE bOutsideArray[8];
 
	ZeroMemory( &bOutsideArray , sizeof( bOutsideArray ) );

	LECullInfo *pCull = g_Render.GetCullInfo();
	D3DXVECTOR3  vecPoint ;


	//���ȼ���ϴμ�⵽��ƽ��ͷ�ڵĵ�

	for( int iPlane = 0; iPlane < 6; iPlane++ )
	{
		if( pCull->planeFrustum[iPlane].a * vecBounds[index].x +
            pCull->planeFrustum[iPlane].b * vecBounds[index].y +
            pCull->planeFrustum[iPlane].c * vecBounds[index].z +
            pCull->planeFrustum[iPlane].d < 0)
		{					
			bOutsideArray[index] |= (1 << iPlane);	
		}
	
	}
#ifdef __FPS_DEBUG__
	_pScene->m_dwCullingTime++;
#endif
	if( bOutsideArray[index] == 0 )
	{	
		index =index ;	
		return true  ;
	}

	//����8����
    for( int iPoint = 0; iPoint < 8; iPoint++ )
    {
		if (iPoint == index )
			continue ;
	
		for( int iPlane = 0; iPlane < 6; iPlane++ )
        {
            if( pCull->planeFrustum[iPlane].a * vecBounds[iPoint].x +
                pCull->planeFrustum[iPlane].b * vecBounds[iPoint].y +
                pCull->planeFrustum[iPlane].c * vecBounds[iPoint].z +
                pCull->planeFrustum[iPlane].d < 0)
            {
						
                bOutsideArray[iPoint] |= (1 << iPlane);	
            }
		
		
        }
#ifdef __FPS_DEBUG__
        _pScene->m_dwCullingTime++;
#endif
        if( bOutsideArray[iPoint] == 0 )
		{	
			index = iPoint ;
			return true;
		}
 	
    }



//���Box�ı�

	 // Now see if any of the bounding box edges penetrate any of the faces of
    // the frustum
    D3DXVECTOR3 edge2[12][2] = 
    {
        vecBounds[0], vecBounds[1], // front bottom
        vecBounds[2], vecBounds[3], // front top
        vecBounds[0], vecBounds[2], // front left
        vecBounds[1], vecBounds[3], // front right
        vecBounds[4], vecBounds[5], // back bottom
        vecBounds[6], vecBounds[7], // back top
        vecBounds[4], vecBounds[6], // back left
        vecBounds[5], vecBounds[7], // back right
        vecBounds[0], vecBounds[4], // left bottom
        vecBounds[2], vecBounds[6], // left top
        vecBounds[1], vecBounds[5], // right bottom
        vecBounds[3], vecBounds[7], // right top
    };
    D3DXVECTOR3 face2[6][4] =
    {
        pCull->vecFrustum[0], pCull->vecFrustum[2], pCull->vecFrustum[3], pCull->vecFrustum[1], // front
        pCull->vecFrustum[4], pCull->vecFrustum[5], pCull->vecFrustum[7], pCull->vecFrustum[6], // back
        pCull->vecFrustum[0], pCull->vecFrustum[4], pCull->vecFrustum[6], pCull->vecFrustum[2], // left
        pCull->vecFrustum[1], pCull->vecFrustum[3], pCull->vecFrustum[7], pCull->vecFrustum[5], // right
        pCull->vecFrustum[2], pCull->vecFrustum[6], pCull->vecFrustum[7], pCull->vecFrustum[3], // top
        pCull->vecFrustum[0], pCull->vecFrustum[4], pCull->vecFrustum[5], pCull->vecFrustum[1], // bottom
    };

	D3DXVECTOR3* pEdge;
	D3DXVECTOR3* pFace;   
	pEdge = &edge2[0][0];

    for(INT  iEdge = 0; iEdge < 12; iEdge++ )
    {
        pFace = &face2[0][0];
        for( INT iFace = 0; iFace < 6; iFace++ )
        {
            if( EdgeIntersectsFace( pEdge, pFace, &pCull->planeFrustum[iFace] ) )
            {
                return true;
            }
            pFace += 4;
        }
        pEdge += 2;
    }


 	
    return false;
}

void CSceneObj::_UpdatePos()
{
	_vPos.x = (float)_nCurX / 100.0f;
	_vPos.y = (float)_nCurY / 100.0f;

    _fTerrainHeight = _pScene->GetTerrainHeight(_vPos.x, _vPos.y);

	if( !_isRBO() )
	{
		_vPos.z = _fTerrainHeight + (float)_nHeightOff / 100.0f;
	}
	else
	{
		_vPos.z = mRBOHeight + (float)_nHeightOff / 100.0f;
	}
       
	SetPos((float*)_vPos);
}

void CSceneObj::_UpdateHeight()
{
	if( !_isRBO() )
	{
		_vPos.z = _fTerrainHeight + (float)_nHeightOff / 100.0f;
	}
	else
	{
		_vPos.z = mRBOHeight + (float)_nHeightOff / 100.0f;
	}
	SetPos((float*)_vPos);
}

void CSceneObj::_UpdateValid(BOOL bValid)
{
    if(_bValid) _bHide = FALSE;
	if(_bValid == FALSE)
	{
	    _pScene->HandleSceneMsg(SCENEMSG_SCENEOBJ_DESTROY,getID());
	}
    // ֪ͨSceneά��SceneObj����������
    _pScene->HandleSceneMsg(SCENEMSG_SCENEOBJ_UPDATEVALID, bValid, _dwObjType, _dwID);
}

void CSceneObj::SetSceneLightInfo(int id)
{
    CSceneObjInfo *pInfo = GetSceneObjInfo(id);

    if(pInfo->nType == 3)
    {
        amb.a = 1.0f;
        amb.r = pInfo->btPointColor[0] / 255.0f;
        amb.g = pInfo->btPointColor[1] / 255.0f;
        amb.b = pInfo->btPointColor[2] / 255.0f;
        dif = amb;
        range = (float)pInfo->nRange;
        attenuation0 = 0;
        attenuation1 = pInfo->Attenuation1;
        attenuation2 = 0;

        type = SceneLight::SL_LIGHT;

        // check world scene
        if(pInfo->nAnimCtrlID != -1)
        {
            CWorldScene* wc = (CWorldScene*)_pScene;
            anim_light = wc->GetAnimLight(pInfo->nAnimCtrlID);
        }

		//lemon add@2004.10.20 for light
		_pTerrain = _pScene->GetTerrain();
		//_fRange = range;
		//_dwcolor = dif;

    }
    else if(pInfo->nType == 4)
    {
        amb.a = 1.0f;
        amb.r = pInfo->btEnvColor[0] / 255.0f;
        amb.g = pInfo->btEnvColor[1] / 255.0f;
        amb.b = pInfo->btEnvColor[2] / 255.0f;

        type = SceneLight::SL_AMBIENT;
    }
}

//////////////////////////////////////////////////////////////////////////
void	CSceneObj::UpdateLight()
{
	if(type != SceneLight::SL_LIGHT)
		return;
	CCharacter*  pCha = NULL;
	CSceneItem* pItem = NULL;
	CEffectObj*	pEff= NULL;

	switch(_iOwerType)
	{
	case 1://��ɫ��
		if(_iChaID >= 0)
		{
			pCha = _pScene->GetCha(_iChaID);
			if(pCha)
			{
				_vPos = pCha->GetPos();
				MoveLight(&_vPos);
			}
		}
		break;
	case 2://���ߵ�
		if(_iChaID >= 0)
		{
			pItem = _pScene->GetSceneItem(_iChaID);
			if(pItem)
			{
				_vPos = pItem->getPos();
				MoveLight(&_vPos);
			}
		}
		break;
	case 3://��Ч��
		if(_iChaID >= 0)
		{
			pEff = _pScene->GetEffect(_iChaID);
			if(pEff)
			{
				_vPos = pEff->getPos();
				_dwcolor = (D3DXCOLOR)dif;
				_dwcolor.a = 1;
				_fRange = range;
				MoveLight(&_vPos);
			}
		}
		break;

	default:
		_dwcolor = (D3DXCOLOR)dif * 0.7f;
		_dwcolor.a = 1;
		_fRange = range;
		MoveLight(&_vPos);
		break;
	}
}
void	CSceneObj::ClearLight()
{
	if(type != SceneLight::SL_LIGHT)
		return;


	if(!_pTerrain)
		return;

	LETile* pTile = NULL;
	int m = 0; int n = 0;

	for (int y = (int)_fy -1; y < (int)_fty + 1; y++)
	{
		for (int x = (int)_fx - 1; x < (int)_ftx +1; x++)
		{
			if(y < 0 || x < 0)
				continue;

			pTile = _pTerrain->GetTile(x, y);
			if(!pTile)
				continue;

			pTile->setTempColor(0);
			n++;
		}
		m++;
	}
}

void	CSceneObj::MoveLight(D3DXVECTOR3* SVerPos)
{
	if(!_pTerrain)
		return;
	LETile* pTile = NULL;
	int m = 0; int n = 0;

	_vPos = *SVerPos;

	_fx = _vPos.x - _fRange+1;
	_fy = _vPos.y - _fRange+1;
	_ftx = _vPos.x + _fRange+1;
	_fty = _vPos.y + _fRange+1;

    DWORD d;
    g_Render.GetDevice()->GetRenderState(D3DRS_AMBIENT, &d);
    D3DXCOLOR amb = d;


	D3DXCOLOR tcolor;
	LETile* pTileset = NULL;
	for (int y = (int)_fy; y < (int)_fty; y++)
	{
		for (int x = (int)_fx; x < (int)_ftx; x++)
		{
			if(y < 0 || x < 0)
				continue;

			pTile = _pTerrain->GetTile(x, y);
			if(!pTile)
				continue;
			float fd = D3DXVec2LengthSq(&(D3DXVECTOR2(float(x),float(y)) - D3DXVECTOR2(_vPos.x,_vPos.y)));
			D3DXCOLOR dwOrgColor = (D3DXCOLOR)pTile->dwColor;

			float flerp = fd / (_fRange *  _fRange);
			if(flerp >1)flerp = 1;

			//D3DXColorLerp(&tcolor,&_dwcolor,&dwOrgColor,flerp);
			//if(pTile->dwTColor != 0)
			//{
			//	//D3DXColorLerp(&tcolor,&tcolor,&(D3DXCOLOR)pTile->dwTColor,0.5f);
			//	D3DXColorAdd(&tcolor,&tcolor,&(D3DXCOLOR)pTile->dwTColor);
			//	tcolor -= dwOrgColor;

			//	tcolor.a = tcolor.a < 0 ? -tcolor.a: tcolor.a;
			//	tcolor.r = tcolor.r < 0 ? -tcolor.a: tcolor.r;
			//	tcolor.g = tcolor.g < 0 ? -tcolor.a: tcolor.g;
			//	tcolor.b = tcolor.b < 0 ? -tcolor.a: tcolor.b;
			//}
			//pTile->setTempColor(tcolor);

#if 1
            tcolor = pTile->dwTColor;
            float attenuation = 1.0f - flerp;
            tcolor.r += _dwcolor.r * attenuation;
            tcolor.g += _dwcolor.g * attenuation;
            tcolor.b += _dwcolor.b * attenuation;
            pTile->setTempColor(tcolor);
#endif
		}
	}
}

DWORD CSceneObj::GetObjTileColor()
{
    DWORD c = 0;
    D3DXVECTOR3 v = this->getPos();
    LETile* tile = _pScene->GetTerrain()->GetTile((int)v.x, (int)v.y);
    if(tile)
    {
        c = tile->dwColor;
    }

    return c;
}

void CSceneObj::SetMaterial(const D3DMATERIALX* mtl)
{
    D3DMATERIALX m = *mtl;

    LETerrain* t = _pScene->GetTerrain();
    CSceneObjInfo* info = GetSceneObjInfo(getTypeID());

    if(t && info->bShadeFlag)
    {
        D3DXVECTOR3 v = this->getPos();
        LETile* tile = t->GetTile((int)v.x, (int)v.y);
        if(tile)
        {
            D3DXCOLOR c = tile->dwColor;

            m.Ambient.r *= c.r;
            m.Ambient.g *= c.g;
            m.Ambient.b *= c.b;
        }
    }

    LESceneObject::SetMaterial(&m);
}

int CSceneObj::HitTestForInfluence(int* flag, D3DXVECTOR3* t_pos, const D3DXVECTOR3* nPos)
{
    drPickInfo p;
    D3DXVECTOR3 org(*nPos);
    D3DXVECTOR3 ray(0.0f, 0.0f, -1.0f);

    *flag = 0;

    if(IsValid() == 0)
        return 0;


    if(DR_SUCCEEDED(HitTestHelperMesh(&p, (drVector3*)&org, (drVector3*)&ray, "block")))
    {
        *flag = 2;
        *t_pos = *(D3DXVECTOR3*)&p.pos;
    }
    else if(DR_SUCCEEDED(HitTestHelperMesh(&p, (drVector3*)&org, (drVector3*)&ray, "terrain")))
    {
        *flag = 1;
        *t_pos = *(D3DXVECTOR3*)&p.pos;
    }

    return 1;


}

int CSceneObj::UpdateObjFadeInFadeOut(const drVector3* org, const drVector3* ray)
{
    //if(_dwTranspFlag == 0)
    //    return 0;

    CSceneObjInfo* info = GetSceneObjInfo(_dwTypeID);
    if(info == 0 || info->nFadeObjNum == 0)
        return 0;

    DWORD flag = 0;
    drPickInfo pi;

    float fade_limits = info->fFadeCoefficent;

    const float fade_coefficient = 0.012f;

    if(DR_SUCCEEDED(HitTestPrimitive(&pi, org, ray)))
    {
        _dwTranspState = 1;
        if(_fTranspValue > fade_limits)
        {
            _fTranspValue -= fade_coefficient;
            if(_fTranspValue < 0.0f)
                _fTranspValue  = 0.0f;

            flag = 1;
        }
    }
    else
    {
        if(_dwTranspState == 1)
        {
            if(_fTranspValue < 1.0f)
            {
                _fTranspValue += fade_coefficient;
                if(_fTranspValue > 1.0f)
                {
                    _fTranspValue = 1.0f;
                }

                flag = 1;
            }
            else
            {
                _dwTranspState = 0;
            }
        }
    }

    if(flag == 0)
        return 0;

    drIPrimitive* p;
    DWORD pri_num = GetPrimitiveNum();
    for(DWORD i = 0; i < pri_num; i++)
    {
        p = GetPrimitive(i);
        DWORD pri_id = p->GetID();
        for(int j = 0; j < info->nFadeObjNum; j++)
        {
            if(pri_id == info->nFadeObjSeq[j])
            {
                p->SetOpacity(_fTranspValue);
                p->SetState(STATE_VISIBLE, (_fTranspValue > 0.0f) ? 1 : 0);
            }
        }
    }

    return 1;
    
}
