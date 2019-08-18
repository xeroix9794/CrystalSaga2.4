#include "util.h"
#include "MapUtil.h"
#include "TerrainData.h"
#include "ObjectData.h"
#include "MyMemory.h"

// map tool function, according to the file name, read the map data
BOOL MU_LoadMapData(CSectionDataMgr *pData, const char *pszDataName) {
	if (!pData->CreateFromFile(pszDataName, TRUE)) {
		LG("maputil", "msg读取数据文件%s失败!\n", pszDataName);
		return FALSE;
	}
	pData->Load(TRUE);
	return TRUE;
}

// map patch tool
// Main working principle:
// 1. Traverse the two maps, find out the different sections, generate these sections of the map data patch file
// 2. When the game client starts and finds that there is a map patch file, it reads and sucks it into the map file and then deletes the map patch file
BOOL MU_CreateMapPatch(const char *pszOld, const char *pszNew) {
	
	// Open the old map file
	MPTerrainData oldmap, newmap;
	if (!MU_LoadMapData(&oldmap, pszOld)) {
		return FALSE;
	}

	// Open a new map file
	if (!MU_LoadMapData(&newmap, pszNew)) {
		return FALSE;
	}

	if (oldmap.GetSectionCntX() != newmap.GetSectionCntX() || oldmap.GetSectionCntY() != newmap.GetSectionCntY()) {
		LG("mappatch", "msg map size can't match, can't go on!\n");
		return FALSE;
	}

	string strOldName = pszOld;
	strOldName = strOldName.substr(0, strOldName.size() - 4);
	char szPatchName[255];
	_snprintf_s(szPatchName, sizeof(szPatchName), _TRUNCATE, "%s.pch", strOldName.c_str());

	FILE *fp = NULL;
	if (fopen_s(&fp, szPatchName, "wb") != 0) {
		LG("mappatch", "msg map size can't match, can't go on!\n");
		return FALSE;
	}

	SMapPatchHeader header;
	strncpy_s(header.szMapName, sizeof(header.szMapName), oldmap.GetDataName(), _TRUNCATE);

	header.sSectionCntX = oldmap.GetSectionCntX();
	header.sSectionCntY = oldmap.GetSectionCntY();

	fwrite(&header, sizeof SMapPatchHeader, 1, fp);
	DWORD dwCnt = 0;
	for (int y = 0; y < oldmap.GetSectionCntY(); y++) {
		for (int x = 0; x < newmap.GetSectionCntX(); x++) {
			SDataSection *pOld = oldmap.GetSectionData(x, y);
			SDataSection *pNew = newmap.GetSectionData(x, y);

			if (pOld == NULL && pNew == NULL) {
				continue;
			}

			BYTE btType = 0; // Not updated

			if (pNew == NULL && pOld != NULL) {
				btType = 2;  // Clear the previous section of the original map
			}
			else {
				if (pOld == NULL || memcmp(pOld->pData, pNew->pData, oldmap.GetSectionDataSize()) != 0) {
					btType = 1; // need to be updated
				}
			}

			if (btType > 0) {
				fwrite(&btType, 1, 1, fp);
				fwrite(&x, 4, 1, fp);
				fwrite(&y, 4, 1, fp);
				if (btType == 1) {
					fwrite(pNew->pData, newmap.GetSectionDataSize(), 1, fp);
				}
				dwCnt++;
			}
		}

	}
	header.dwUpdateCnt = dwCnt;
	fseek(fp, SEEK_SET, 0);
	fwrite(&header, sizeof SMapPatchHeader, 1, fp);
	fclose(fp);

	if (dwCnt == 0) {
		LG("mappatch", "msg the two file is the same!\n");
		DeleteFile(szPatchName);
	}
	else {
		LG("mappatch", "msg map patch create ok, totle update %d area, patch file is [%s]\n", dwCnt, szPatchName);
	}
	return TRUE;
}

// Make a patch into a map file
BOOL MU_PatchMapFile(const char *pszMap, const char *pszPatch) {

	// Open the patch file
	FILE *fp = NULL;
	if (fopen_s(&fp, pszPatch, "rb") != 0) {
		LG("mappatch", "msg open patch map file [%s] failed!\n", pszPatch);
		return FALSE;
	}

	if (fp == NULL) {
		LG("mappatch", "msg open patch map file [%s] failed!\n", pszPatch);
		return FALSE;
	}

	SMapPatchHeader header;
	fread(&header, sizeof SMapPatchHeader, 1, fp);

	LG("mappatch", "msg totle update [%d] area\n", header.dwUpdateCnt);

	MPTerrainData oldmap; MU_LoadMapData(&oldmap, pszMap);

	DWORD dwDataSize = oldmap.GetSectionDataSize();
	BYTE *pBuf = new BYTE[dwDataSize];
	DWORD dwCnt = 0;
	for (DWORD i = 0; i < header.dwUpdateCnt; i++) {
		BYTE btType; fread(&btType, 1, 1, fp);
		DWORD x, y;  fread(&x, 4, 1, fp);
		fread(&y, 4, 1, fp);

		SDataSection *pSec = oldmap.GetSectionData(x, y);
		
		// There are data updates
		if (btType == 1) {
			fread(pBuf, dwDataSize, 1, fp);
			if (pSec == NULL) {
				pSec = oldmap.LoadSectionData(x, y);
			}

			if (pSec->dwDataOffset == 0) {
				pSec->pData = new BYTE[dwDataSize];
			}

			memcpy(pSec->pData, pBuf, dwDataSize);
		}

		// Clear Section
		else if (btType == 2) {
			oldmap.ClearSectionData(x, y);
		}
		oldmap.SaveSectionData(x, y);
		dwCnt++;
	}

	fclose(fp);
	delete[] pBuf;

	LG("mappatch", "msg map patch create ok, totle update [%d] area\n", dwCnt);
	return TRUE;
}
