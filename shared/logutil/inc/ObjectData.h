#pragma once

#include "SectionData.h"

#define MAX_SECTION_OBJ 25

struct SSceneObjInfo {
	short sTypeID;		// High 2 is type (0: scene object, 1: effect object), the rest is ID
	unsigned long nX;
	unsigned long nY;
	short sHeightOff;
	short sYawAngle;
	short sScale;		// Reserved Not used

	short GetType() {
		return sTypeID >> (sizeof(short) * 8 - 2);
	}

	short GetID() {
		return ~(0x0003 << (sizeof(short) * 8 - 2)) & sTypeID;
	}

	SSceneObjInfo() : sTypeID(0), nX(0), nY(0) {
	}
};

class CSceneObjData : public CSectionDataMgr {
	struct SFileHead {
		char tcsTitle[16];	// "HF Object File!"
		int lVersion;
		long lFileSize;

		int iSectionCntX;	// The number of horizontal regions of the map
		int iSectionCntY;	// The number of vertical regions of the map
		int iSectionWidth;	// Area width (unit: Tile)
		int iSectionHeight; // The height of the area (unit: Tile)
		int iSectionObjNum;	// The maximum number of objects allowed in the area

		SFileHead() {
			strncpy_s(tcsTitle, sizeof(tcsTitle), "HF Object File!", _TRUNCATE);
			iSectionObjNum = MAX_SECTION_OBJ;
		}
	};

	struct SSectionIndex {

		// obj
		long lObjInfoPos;
		int	iObjNum;

		SSectionIndex() : lObjInfoPos(0), iObjNum(0) {
		}
	};

public:
	void TrimFile(const char *pszTarget);

	~CSceneObjData() {
		if (_fp) {
			_WriteFileHeader();
		}
	}

protected:
	int _GetSectionDataSize() { 
		return 20 * MAX_SECTION_OBJ; 
	}

	BOOL _ReadFileHeader();
	void _WriteFileHeader();
	DWORD _ReadSectionIdx(DWORD dwSectionNo);
	void _WriteSectionIdx(DWORD dwSectionNo, DWORD dwOffset);
	void _AfterReadSectionData(SDataSection *pSection, int nSectionX, int nSectionY);

	SFileHead _header;
};

// read the file header
// Subclasses can do some legitimacy judgments
inline BOOL CSceneObjData::_ReadFileHeader() {
	fread(&_header, sizeof(_header), 1, _fp);
	if (_header.lVersion != 600) {
		
		// LG(GetDataName(), "msg not legitimacy obj file!\n");
		return FALSE;
	}

	_nSectionCntX = _header.iSectionCntX;
	_nSectionCntY = _header.iSectionCntY;

	return TRUE;
}

// write the file header, for newly created files and other occasions
inline void CSceneObjData::_WriteFileHeader() {
	_header.iSectionCntX = _nSectionCntX;
	_header.iSectionCntY = _nSectionCntY;
	_header.iSectionWidth = 8;
	_header.iSectionHeight = 8;
	_header.lVersion = 600;			// Version 500
	fseek(_fp, 0, SEEK_END);
	_header.lFileSize = ftell(_fp);
	fseek(_fp, 0, SEEK_SET);
	fwrite(&_header, sizeof(_header), 1, _fp);
}

inline DWORD CSceneObjData::_ReadSectionIdx(DWORD dwSectionNo) {
	SSectionIndex idx;
	fseek(_fp, sizeof(SFileHead) + sizeof(idx) * dwSectionNo, SEEK_SET);
	fread(&idx, sizeof(idx), 1, _fp);

	SDataSection *pSection = _GetDataSection(dwSectionNo);
	if (pSection) {
		if (idx.iObjNum > MAX_SECTION_OBJ) {
			idx.iObjNum = MAX_SECTION_OBJ;
		}
		pSection->dwParam = idx.iObjNum;
	}
	return idx.lObjInfoPos;
}

inline void CSceneObjData::_WriteSectionIdx(DWORD dwSectionNo, DWORD dwOffset) {
	SSectionIndex idx;
	fseek(_fp, sizeof(SFileHead) + sizeof(idx) * dwSectionNo, SEEK_SET);
	fread(&idx, sizeof(idx), 1, _fp);
	idx.lObjInfoPos = dwOffset;
	SDataSection *pSection = _GetDataSection(dwSectionNo);
	if (pSection) {
		idx.iObjNum = pSection->dwParam;
		if (idx.iObjNum > MAX_SECTION_OBJ) idx.iObjNum = MAX_SECTION_OBJ;
	}
	
	// Was cleared
	if (dwOffset == 0) {
		idx.iObjNum = 0;
	}
	fseek(_fp, sizeof(SFileHead) + sizeof(idx) * dwSectionNo, SEEK_SET);
	fwrite(&idx, sizeof(idx), 1, _fp);
}

inline void CSceneObjData::_AfterReadSectionData(SDataSection *pSection, int nSectionX, int nSectionY) {
	{
		SSceneObjInfo *pObjArray = (SSceneObjInfo*)pSection->pData;
		SSceneObjInfo *pObjInfo = pObjArray + 0;
		for (DWORD i = 0; i < pSection->dwParam; i++) {
			short sID = (pObjInfo + i)->GetID();
			if (sID == 1398 || sID == 0) {
				LG("maptoolerr", "find unnormal ID %d\n", sID);
			}
		}
	}
}

inline void CSceneObjData::TrimFile(const char *pszTarget) {
	FILE *fp = NULL;
	if (fopen_s(&fp, pszTarget, "wb") != 0) {
		throw "TrimFile error";
	}
	fwrite(&_header, sizeof(_header), 1, fp);

	// Write content
	SSectionIndex *pIdx = new SSectionIndex[_nSectionCntX * _nSectionCntY];
	memset(pIdx, 0, sizeof(SSectionIndex) * _nSectionCntX * _nSectionCntY);
	SSectionIndex *pCurIdx = pIdx;

	fseek(fp, sizeof(_header) + sizeof(SSectionIndex) * _nSectionCntX * _nSectionCntY, SEEK_SET);
	for (int y = 0; y < GetSectionCntY(); y++) {
		for (int x = 0; x < GetSectionCntX(); x++) {
			SDataSection *pSection = GetSectionData(x, y);
			if (pSection && pSection->dwDataOffset) {
				pCurIdx->lObjInfoPos = ftell(fp);
				pCurIdx->iObjNum = pSection->dwParam;
				if (pCurIdx->iObjNum > MAX_SECTION_OBJ) {
					LG("objcnt", "section obj cnt = %d, location(%d %d)\n", pCurIdx->iObjNum, x * 8, y * 8);
					pCurIdx->iObjNum = MAX_SECTION_OBJ;
					pSection->dwParam = MAX_SECTION_OBJ;
				}
				fwrite(pSection->pData, 20 * MAX_SECTION_OBJ, 1, fp);
				_AfterReadSectionData(pSection, 0, 0);
			}
			pCurIdx++;
		}
	}

	int nFileSize = ftell(fp);
	int nLastSize = _header.lFileSize;
	_header.lFileSize = nFileSize;

	fseek(fp, 0, SEEK_SET);
	_header.iSectionObjNum = MAX_SECTION_OBJ;
	fwrite(&_header, sizeof(_header), 1, fp);
	_header.lFileSize = nLastSize;

	// Write to index area
	fseek(fp, sizeof(_header), SEEK_SET);
	fwrite(pIdx, sizeof(SSectionIndex), _nSectionCntX * _nSectionCntY, fp);

	fclose(fp);
	delete[] pIdx;
}
