#include "Stdafx.h"
#include "LETile.h"
#include "LEMap.h"
#include "drgraphicsutil.h"
#include "assert.h" 
#include <sys/types.h>
#include <sys/stat.h>
#include "LEMapDef.h"
#include "MyMemory.h"
#include "CPerformance.h"
#include "Terrain.h"

#pragma warning(disable:4311) //type cast warning

#define MAX_DEL_SECTION				400
#define MAX_SECTION_BUFFER          16

BOOL LEMap::Load(const char *pszMapName, BOOL bEdit)
{
	if (_fp != NULL)
	{
		fclose(_fp); _fp = NULL;
	}

	FILE *fp = NULL;
	if (bEdit)
	{
		_chmod(pszMapName, _S_IWRITE);

		fopen_s(&fp, pszMapName, "r+b");
	}
	else
	{
		fopen_s(&fp, pszMapName, "rb");
	}
	if (fp == NULL)
	{
		LG("map", "msgLoad Map [%s] Error!\n", pszMapName);
		return FALSE;
	}

	LEMapFileHeader header;

	DWORD dwReadSize = 0;

	fread(&header, sizeof(LEMapFileHeader), 1, fp);
	dwReadSize += sizeof(LEMapFileHeader);

	if (header.nMapFlag == LE_MAP_FLAG + 1)
	{
		fclose(fp);
		LG("map", "msg�õ�ͼ�ļ�[%s]�汾����, ��ʹ��MapTool�����������汾!", pszMapName);
		return FALSE;
	}

#ifdef NEW_VERSION
	if (header.nMapFlag != LE_MAP_FLAG + 3)
#else
	if (header.nMapFlag != LE_MAP_FLAG + 2)
#endif
	{
		fclose(fp);
		//LG("map", "msg[%s]������Ч�� LightEngine Map File!\n", pszMapName);
		return FALSE;
	}

	_nWidth = header.nWidth;
	_nHeight = header.nHeight;
	_nSectionWidth = header.nSectionWidth;
	_nSectionHeight = header.nSectionHeight;

	_nSectionCntX = _nWidth / _nSectionWidth;
	_nSectionCntY = _nHeight / _nSectionHeight;
	_nSectionCnt = _nSectionCntX * _nSectionCntY;

	_bEdit = bEdit;

	_fp = fp;

	// ��ȡȫ������
	_pOffsetIdx = new DWORD[_nSectionCnt];
	fread(_pOffsetIdx, _nSectionCnt * 4, 1, _fp);
	dwReadSize += _nSectionCnt * 4;

	if (!_bEdit)
	{
		m_dwMapPos = ftell(_fp);

		fseek(_fp, 0, SEEK_END);
		DWORD dwPos = ftell(_fp);

		DWORD dwMapDataSize = dwPos - dwReadSize;

		if (dwMapDataSize > m_dwMapDataSize)
		{
			SAFE_DELETE_ARRAY(m_pMapData);
			m_pMapData = new BYTE[dwMapDataSize];
			m_dwMapDataSize = dwMapDataSize;
		}
		fseek(_fp, dwReadSize, SEEK_SET);
		fread(m_pMapData, dwMapDataSize, 1, _fp);
		fclose(_fp);
		_fp = NULL;
	}


	ClearSectionArray();

	m_pBlock->Load(this);

	return TRUE;
}

int LEMap::GetValidSectionCnt()
{
	int nValidCnt = 0;
	for (int i = 0; i < _nSectionCnt; i++)
	{
		DWORD dwDataOff = *(_pOffsetIdx + i);
		if (dwDataOff) nValidCnt++;
	}
	return nValidCnt;
}

void LEMap::SetSectionTileData(int nX, int nY, BYTE btTexNo)
{
	if (btTexNo == 0 || !_bEdit) return;

	LEActiveMapSection *pSection = GetActiveSection(nX / _nSectionWidth, nY / _nSectionHeight);
	if (!pSection) return;

	SAFE_DELETE(pSection->pTileData);
	pSection->pTileData = new LETile[_nSectionWidth * _nSectionHeight];
	for (int y = 0; y < _nSectionHeight; y++)
	{
		for (int x = 0; x < _nSectionWidth; x++)
		{
			LETile *pTile = pSection->pTileData + y * _nSectionWidth + x;
			pTile->Init();
			pTile->TexLayer[0].btTexNo = btTexNo;
			pTile->TexLayer[0].btAlphaNo = 15;
			pTile->fHeight = 0.6f + (float)(rand() % 30) / 100.0f;
			pTile->dwColor = 0xffffffff;
			pTile->sRegion = 1; // ����½������
		}
	}
	// pSection->dwDataOffset = 0;

	_SaveSection(pSection);
}


void LEMap::_WriteSectionDataOffset(int nSectionX, int nSectionY, DWORD dwDataOffset)
{
	DWORD dwLoc = (nSectionY * _nSectionCntX + nSectionX);
	*(_pOffsetIdx + dwLoc) = dwDataOffset;
	fseek(_fp, sizeof(LEMapFileHeader) + 4 * dwLoc, SEEK_SET);
	fwrite(&dwDataOffset, 4, 1, _fp);
}

DWORD LEMap::_ReadSectionDataOffset(int nSectionX, int nSectionY)
{
	DWORD dwLoc = (nSectionY * _nSectionCntX + nSectionX);
	return *(_pOffsetIdx + dwLoc);
	//     
	//  fseek(_fp, sizeof(LEMapFileHeader) + 4 * dwLoc, SEEK_SET);
	// 	DWORD dwDataOffset; fread(&dwDataOffset, 4, 1, _fp);
	// 	return dwDataOffset;
}

//-----------------
// �����ͼ��������
//-----------------
void LEMap::_SaveSection(LEActiveMapSection *pSection)
{
	if (!_bEdit || _fp == NULL) return;

	// ����Ӧ��Ѱ�ҷ�����TileData���ݶ�
	if (pSection->dwDataOffset)
	{
		fseek(_fp, pSection->dwDataOffset, SEEK_SET);
	}
	else
	{
		fseek(_fp, 0, SEEK_END);
		pSection->dwDataOffset = ftell(_fp);
	}
#ifdef NEW_VERSION
	SNewFileTile tile;
#else
	SFileTile tile;
#endif
	for (int y = 0; y < _nSectionHeight; y++)
	{
		for (int x = 0; x < _nSectionWidth; x++)
		{
			LETile *pTile = pSection->pTileData + _nSectionWidth * y + x;
#ifdef NEW_VERSION
			TileInfo_8To5((BYTE*)(&pTile->TexLayer[0]), tile.dwTileInfo, tile.btTileInfo);
			tile.cHeight = (char)(pTile->fHeight * 100 / 10);
			tile.sColor = DR_RGBDWORDTO565(pTile->dwColor);
#else
			memcpy(&tile.t[0], &pTile->TexLayer[0], 8);
			tile.sHeight = (short)(pTile->fHeight * 100);
			tile.dwColor = pTile->dwColor;
#endif
			tile.sRegion = pTile->sRegion;
			tile.btIsland = pTile->btIsland;
			memcpy(&(tile.btBlock[0]), &(pTile->btBlock[0]), 4);
			fwrite(&tile, sizeof(tile), 1, _fp);
		}
	}
	_WriteSectionDataOffset(pSection->nX, pSection->nY, pSection->dwDataOffset);
}

//-----------------
// ��ȡ��ͼ��������
//-----------------
void LEMap::_LoadSectionData(LEActiveMapSection *pSection)
{
	int nSectionX = pSection->nX;
	int nSectionY = pSection->nY;

	pSection->dwDataOffset = _ReadSectionDataOffset(nSectionX, nSectionY);

	if (pSection->dwDataOffset == 0) return;

	DWORD dwPos = 0;
	if (_bEdit)
	{
		assert(_fp);
		fseek(_fp, pSection->dwDataOffset, SEEK_SET);
	}
	else
	{
		dwPos = pSection->dwDataOffset - m_dwMapPos;
	}

	pSection->pTileData = new LETile[_nSectionWidth * _nSectionHeight];

#ifdef NEW_VERSION
	SNewFileTile tile;
#else
	SFileTile tile;
#endif

	//IP("_LoadSectionData(%d,%d) Offset=%u\n", pSection->nX, pSection->nY,pSection->dwDataOffset);
	for (int y = 0; y < _nSectionHeight; y++)
	{
		for (int x = 0; x < _nSectionWidth; x++)
		{
			LETile *pTile = pSection->pTileData + _nSectionWidth * y + x;
			pTile->Init();
			if (_bEdit)
			{
				assert(_fp);
				fread(&tile, sizeof(tile), 1, _fp);
			}
			else
			{
				memcpy(&tile, m_pMapData + dwPos, sizeof(tile));
				dwPos += sizeof(tile);
			}
#ifdef NEW_VERSION            
			TileInfo_5To8(tile.dwTileInfo, tile.btTileInfo, (BYTE*)(&pTile->TexLayer[0]));
			pTile->dwColor = DR_RGB565TODWORD(tile.sColor);
			pTile->dwColor |= 0xff000000;
			pTile->fHeight = (float)(tile.cHeight * 10) / 100.0f;
#else 			
			pTile->dwColor = tile.dwColor;
			memcpy(&pTile->TexLayer[0], &(tile.t[0]), 8);
			pTile->fHeight = (float)(tile.sHeight) / 100.0f;
#endif            
			pTile->btIsland = tile.btIsland;
			pTile->sRegion = tile.sRegion;
			memcpy(&pTile->btBlock[0], &tile.btBlock[0], 4);
		}
	}
}


LEActiveMapSection *LEMap::LoadSectionData(int nSectionX, int nSectionY)
{
	LEActiveMapSection *pSection = new LEActiveMapSection;
	pSection->Init();
	pSection->nX = nSectionX;
	pSection->nY = nSectionY;

	_LoadSectionData(pSection);
	_ActiveSectionArray[nSectionY][nSectionX] = pSection;
	_ActiveSectionList.push_back(pSection);
	return pSection;
}

void LEMap::ClearSectionData(int nSectionX, int nSectionY)
{
	// ����ļ���ļ�¼
	_WriteSectionDataOffset(nSectionX, nSectionY, 0);

	// ����ڴ���ļ�¼
	LEActiveMapSection *pSection = GetActiveSection(nSectionX, nSectionY);
	if (pSection) // ��Section��δ�����ڴ�
	{
		_ActiveSectionList.remove(pSection);
		_ActiveSectionArray[nSectionY][nSectionX] = NULL;
		SAFE_DELETE(pSection);
	}
}

void LEMap::FullLoading()
{
	if (_fp == NULL) return;

	// ��ȡȫ��Section����
	for (int i = 0; i < _nSectionCnt; i++)
	{
		int nSectionX = i % _nSectionCntX;
		int nSectionY = i / _nSectionCntX;
		if (*(_pOffsetIdx + i) != 0)
		{
			LEActiveMapSection *pSection = LoadSectionData(nSectionX, nSectionY);
			_pfnProc(0, pSection->nX, pSection->nY, (DWORD)(pSection), this);
		}
	}
}

void LEMap::DynamicLoading(DWORD dwTimeParam)
{
	int nCenterSectionX = _fShowCenterX / _nSectionWidth;
	int nCenterSectionY = _fShowCenterY / _nSectionHeight;

	if (nCenterSectionX == _nLastSectionX && nCenterSectionY == _nLastSectionY)
	{
		return;
	}
	else
	{
		_nLastSectionX = nCenterSectionX;
		_nLastSectionY = nCenterSectionY;
	}
#ifdef __FPS_DEBUG__
	LETimer t;
	t.Begin();
#endif	
	int nCurSectionX = (int)(_fShowCenterX - (float)_nShowWidth / 2.0f) / _nSectionWidth;
	int nCurSectionY = (int)(_fShowCenterY - (float)_nShowHeight / 2.0f) / _nSectionHeight;

	int nEndSectionX = (int)(_fShowCenterX + (float)_nShowWidth / 2.0f) / _nSectionWidth;
	int nEndSectionY = (int)(_fShowCenterY + (float)_nShowHeight / 2.0f) / _nSectionHeight;

	int nShowSectionCntX = nEndSectionX - nCurSectionX;
	int nShowSectionCntY = nEndSectionY - nCurSectionY;

	if (_nShowWidth  % _nSectionWidth != 0)   nShowSectionCntX++;
	if (_nShowHeight % _nSectionHeight != 0)  nShowSectionCntY++;

	list<LEActiveMapSection*> _NewList;

	for (int y = 0; y < nShowSectionCntY; y++)
	{
		int nSectionY = nCurSectionY + y;

		if (nSectionY < 0 || nSectionY >= _nSectionCntY) continue;
		for (int x = 0; x < nShowSectionCntX; x++)
		{
			int nSectionX = nCurSectionX + x;

			if (nSectionX < 0 || nSectionX >= _nSectionCntX) continue;

			LEActiveMapSection *pSection = GetActiveSection(nSectionX, nSectionY);
			if (!pSection)
			{
				pSection = LoadSectionData(nSectionX, nSectionY);
				_NewList.push_back(pSection);
			}
			pSection->dwActiveTime = dwTimeParam;
		}
	}

	if ((int)(_ActiveSectionList.size()) >= _nSectionBufferSize)
	{
		static LEActiveMapSection *DelSectionList[MAX_DEL_SECTION];
		int n = 0;
		BOOL bDelFail = FALSE;
		for (list<LEActiveMapSection*>::iterator it = _ActiveSectionList.begin(); it != _ActiveSectionList.end(); it++)
		{
			LEActiveMapSection *pCur = (*it);
			if (pCur->dwActiveTime != dwTimeParam)
			{
				DelSectionList[n] = pCur;
				n++;
				if (n >= MAX_DEL_SECTION)
				{
					bDelFail = TRUE;
					break;
				}
			}
		}
		if (bDelFail) LG("map", "�ͷ�Section����ʱ, �������Buffer, n = %d\n", n);

		for (int i = 0; i < n; i++) // �������Ҫ��Section Tile Data
		{
			UpdateRender(TRUE);
			_ActiveSectionList.remove(DelSectionList[i]);
			if (DelSectionList[i]->dwDataOffset != 0)
			{
				_SaveSection(DelSectionList[i]);
			}
			if (_pfnProc)	_pfnProc(1, DelSectionList[i]->nX, DelSectionList[i]->nY, (DWORD)(DelSectionList[i]), this);
			_ActiveSectionArray[DelSectionList[i]->nY][DelSectionList[i]->nX] = NULL;
			SAFE_DELETE(DelSectionList[i]->pTileData);
			SAFE_DELETE(DelSectionList[i]);
		}
	}


	// ͳһִ��MapNotice
	for (list<LEActiveMapSection*>::iterator it = _NewList.begin(); it != _NewList.end(); it++)
	{
		LEActiveMapSection *pNewSection = (*it);
		if (_pfnProc) _pfnProc(0, pNewSection->nX, pNewSection->nY, (DWORD)(pNewSection), this);
		UpdateRender(TRUE);
	}

	_NewList.clear();

	// g_Render.Print(INFO_DEBUG, 10, 30, "Active Map Section = %d\n", _ActiveSectionList.size());



#ifdef __FPS_DEBUG__
	m_dwActiveSectionCnt = (DWORD)(_ActiveSectionList.size());
	DWORD dwLoadingTime = t.End();
	if (dwLoadingTime > m_dwMaxLoadingTime)
	{
		m_dwMaxLoadingTime = dwLoadingTime;
	}

	if (dwLoadingTime)
	{
		m_dwLoadingTime[_dwLoadingCnt] = dwLoadingTime;
		_dwLoadingCnt++;
		if (_dwLoadingCnt >= 3) _dwLoadingCnt = 0;
	}
#endif

	m_pBlock->GetBlockByRange(_fShowCenterX, _fShowCenterY, m_iRange);
}

void LEMap::ClearAllSection(BOOL bSaveFlag)
{
	for (list<LEActiveMapSection*>::iterator it = _ActiveSectionList.begin(); it != _ActiveSectionList.end(); it++)
	{
		LEActiveMapSection *pSection = (*it);
		if (pSection->dwDataOffset)
		{
			if (bSaveFlag) _SaveSection((*it));
		}
		if (bSaveFlag)
		{
			if (_pfnProc) _pfnProc(1, pSection->nX, pSection->nY, (DWORD)(pSection), this);
		}
		_ActiveSectionArray[pSection->nY][pSection->nX] = NULL;
		SAFE_DELETE(pSection->pTileData);
		SAFE_DELETE(pSection);
	}

	ClearSectionArray();
	_ActiveSectionList.clear();

	SAFE_DELETE_ARRAY(m_pMapData);
}

void LEMap::ClearSectionArray()
{
	memset(&_ActiveSectionArray, 0, 512 * 512 * 4);
}




