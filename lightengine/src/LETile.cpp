#include "Stdafx.h"
#include "LETile.h"
#include "LEMath.h"
#include "drTimer.h"
DR_USING;

int LETile::Offset[4][2] =
{
  0, 0,
  1, 0,
  0, 1,
  1, 1
};


//--------------------------- 
// ������ͼ��(���䰡, �����)
//---------------------------
void LETile::AddTexLayer(BYTE btTexNo, BYTE btAlphaNo)
{
	if (IsDefault()) return;

	int i, j;
	if (btTexNo == 0)
	{
		for (i = 0; i < 4; i++) TexLayer[i].btTexNo = 0;
		return;
	}

	LETileTex TempTexLayer[5];


	BYTE btLayerCnt = 0;

	memcpy(&TempTexLayer, &TexLayer, 8);
	for (i = 0; i < 4; i++)
	{
		if (TexLayer[i].btTexNo) btLayerCnt++;
		else break;
	}

	if (btTexNo == TexLayer[btLayerCnt - 1].btTexNo)
	{
		if (TexLayer[btLayerCnt - 1].SetAlphaNo(btAlphaNo) == 15)
		{
			ResetTexLayer(btTexNo);
			return;
		}
		TempTexLayer[btLayerCnt - 1].SetAlphaNo(btAlphaNo);
	}
	else
	{
		TempTexLayer[btLayerCnt].btTexNo = btTexNo;
		TempTexLayer[btLayerCnt].btAlphaNo = btAlphaNo;
		btLayerCnt++;
	}

	int nLayerValid[5] = { 1, 1, 1, 1, 1 };

	for (i = 0; i < btLayerCnt - 1; i++)
	{
		BYTE btCurAlpha = TempTexLayer[i].btAlphaNo;
		BYTE btMergeAlpha = 0;
		for (j = i + 1; j < btLayerCnt; j++)
		{
			btMergeAlpha |= TempTexLayer[j].btAlphaNo;
			if (btMergeAlpha >= btCurAlpha && (btMergeAlpha & btCurAlpha) == btCurAlpha)
			{
				nLayerValid[i] = 0;
				break;
			}
		}
	}

	BYTE btValid = 0;
	for (i = 0; i < btLayerCnt; i++)
	{
		if (nLayerValid[i])
		{
			TexLayer[btValid].btTexNo = TempTexLayer[i].btTexNo;
			TexLayer[btValid].btAlphaNo = TempTexLayer[i].btAlphaNo;
			btValid++;
		}
	}

	TexLayer[0].btAlphaNo = 15;
	if (btValid < 4)
	{
		TexLayer[btValid].btTexNo = 0; // �˲㿪ʼ��Ч
	}
}


LETileVertex		LETile::_TVertex[4];
LESeaTileVertex		LETile::_SVertex[4];

//-----------------------------
// ������Ⱦ, ��VertexBuffer��ʽ
//-----------------------------



void LETile::RenderTerrain(int nX, int nY, LETile *TileList[4])
{
	// �����йصõ�render state ambient ����ɫӦ��ʹ��ͳһ�Ļ���
	// ����ÿ��render��ͨ��device����̬�õ�
	drColorValue4b amb, vert_amb, c, t, x;
	g_Render.GetDevice()->GetRenderState(D3DRS_AMBIENT, &amb.color);

	float fOff = 0.0f;
	for (int i = 0; i < 4; i++)
	{
		LETile* _pCurTile = TileList[i];
		// if(_pCurTile->getBlock()) _pCurTile->dwColor = 0xf1000000;		

		_TVertex[i].dwColor = _pCurTile->dwTColor;


		c.color = _pCurTile->dwColor;
		t.color = _pCurTile->dwTColor;
		vert_amb.a = (amb.a * c.a) / 255;
		vert_amb.r = (amb.r * c.r) / 255;
		vert_amb.g = (amb.g * c.g) / 255;
		vert_amb.b = (amb.b * c.b) / 255;
		//_TVertex[i].dwColor = vert_amb.color;
		x.a = (vert_amb.a + t.a) > 255 ? 255 : (vert_amb.a + t.a);
		x.r = (DWORD)(vert_amb.r + t.r) > 255 ? 255 : (vert_amb.r + t.r);
		x.g = (DWORD)(vert_amb.g + t.g) > 255 ? 255 : (vert_amb.g + t.g);
		x.b = (DWORD)(vert_amb.b + t.b) > 255 ? 255 : (vert_amb.b + t.b);

		dwXColor = x.color;
		_TVertex[i].dwColor = dwXColor;

		_TVertex[i].p = VECTOR3((float)(nX + Offset[i][0]), (float)(nY + Offset[i][1]), _pCurTile->fHeight);
	}
}

void LETile::RenderSea(int nX, int nY, int nTileSize)
{
	for (int i = 0; i < 4; i++)
	{
		_SVertex[i].p = VECTOR3((float)(nX + Offset[i][0] * nTileSize), (float)(nY + Offset[i][1] * nTileSize), SEA_LEVEL);
	}
#if(defined DRAW_TERRAIN_USE_DYNAMIC_BUFFER)
	drInterfaceMgr* imgr = g_Render.GetInterfaceMgr();
	drIDynamicStreamMgr* dsm = imgr->res_mgr->GetDynamicStreamMgr();
	dsm->BindDataVB(0, &_SVertex, sizeof(LESeaTileVertex) * 4, sizeof(LESeaTileVertex));
	dsm->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
#else
	g_Render.GetDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &_SVertex, sizeof(LESeaTileVertex));
#endif
}


