#pragma once

#include "Util.h"

// MapData data management class
// Support data dynamic read, do not limit the data type
// such as the surface, object save files can use this class
// Support data between the block copy, you can easily cut the map, the combination


struct SDataSection {
	void* pData;				// Point to the actual data
	DWORD dwDataOffset;			// The file data pointer position = 0 indicates that there is no data
	DWORD dwActiveTime;			// The last time of use
	DWORD dwParam;				// parameter

	SDataSection() : pData(NULL), dwActiveTime(0), dwDataOffset(0), dwParam(0) {
	}
};

class CSectionDataMgr;
typedef long (CALLBACK* MAP_LOADING_PROC)(int nFlag, int nSectionX, int nSectionY, unsigned long dwParam, CSectionDataMgr* pMapData);

class CSectionDataMgr {
public:

	~CSectionDataMgr();

	// Read from the file, and decide whether or not an editing operation is required
	BOOL CreateFromFile(const char *pszMapName, BOOL bEdit = FALSE);

	// create, will generate the file
	BOOL Create(int nSectionCntX, int nSectionCntY, const char *pszMapName);

	// Read the disk, you can read one-time, you can also dynamically read
	void Load(BOOL bFull, DWORD dwTimeParam = 0);

	// Read the callback function for Section data
	void SetLoadSectionCallback(MAP_LOADING_PROC pfn)	{ _pfnProc = pfn; }

	// copy the Section data
	void CopySectionData(SDataSection* pDest, SDataSection *pSrc);

	// Save the Section data to the file
	void SaveSectionData(int nSectionX, int nSectionY);

	// Clear the data for the specified Section, and the file index will be cleared internally
	void ClearSectionData(int nSectionX, int nSectionY);

	// Get the Section data, or empty if it is not in memory
	SDataSection* GetSectionData(int nSectionX, int nSectionY);

	// Read the data for the specified Section
	SDataSection* LoadSectionData(int nSectionX, int nSectionY);


	// Get the map description
	int GetSectionCntX()		{ return _nSectionCntX; }
	int GetSectionCntY()		{ return _nSectionCntY; }
	int GetSectionCnt()			{ return _nSectionCntX * _nSectionCntY; }
	char* GetFileName()			{ return _szFileName; }
	char* GetDataName()			{ return _szDataName; }

	// void SetDataName(const char *pszName) { strcpy(_szDataName, pszName); }   
	void SetDataName(const char *pszName) { strncpy_s(_szDataName, sizeof(_szDataName), pszName, _TRUNCATE); }

	// Calculate valid data
	// int CalValidSectionCnt();

	// Set the center position and range of dynamic read
	void SetLoadSize(int nWidth, int nHeight)	{ _nLoadWidth = nWidth; _nLoadHeight = nHeight; }
	int GetLoadWidth()	{ return _nLoadWidth; }
	int	GetLoadHeight()	{ return _nLoadHeight; }
	void SetLoadCenter(int nX, int nY)	{ _nLoadCenterX = nX; _nLoadCenterY = nY; }
	int GetLoadCenterX() { return _nLoadCenterX; }
	int GetLoadCenterY() { return _nLoadCenterY; }
	BOOL IsValidLocation(int nX, int nY);
	int GetSectionDataSize() { return _GetSectionDataSize(); }

protected:
	CSectionDataMgr();

	virtual int _GetSectionDataSize() = 0;
	virtual BOOL _ReadFileHeader() = 0;
	virtual void _WriteFileHeader() = 0;
	virtual DWORD _ReadSectionIdx(DWORD dwSectionNo) = 0;
	virtual void _WriteSectionIdx(DWORD dwSectionNo, DWORD dwDataOffset) = 0;
	virtual void _AfterReadSectionData(SDataSection *pSection, int nSectionX, int nSectionY) {
	}

protected:
	SDataSection* _GetDataSection(DWORD dwSectionNo) {
		return *(_SectionArray + dwSectionNo);
	}

	char _szFileName[255];		// Contains the name of the full path
	char _szDataName[32];		// Map name
	int _nSectionCntX;			// The number of blocks in the horizontal direction of the map
	int _nSectionCntY;			// The number of blocks in the vertical direction of the map
	int _nLoadCenterX;			// The center of the dynamic read
	int _nLoadCenterY;
	int _nLoadWidth;			// Visual range
	int _nLoadHeight;

	int _nSectionStartX;
	int _nSectionStartY;

	MAP_LOADING_PROC _pfnProc; // The map dynamically reads the callback notification function

	FILE* _fp;		// Map file handle
	BOOL _bEdit;	// Whether it can be edited

	BOOL _bDebug;

	std::list<SDataSection*> _SectionList;
	SDataSection** _SectionArray;
};

inline SDataSection* CSectionDataMgr::GetSectionData(int nSectionX, int nSectionY) {
	if (!IsValidLocation(nSectionX, nSectionY)) {
		return NULL;
	}
	DWORD dwLoc = (nSectionY * _nSectionCntX + nSectionX);
	return (SDataSection*)(*(_SectionArray + dwLoc));
}

inline BOOL CSectionDataMgr::IsValidLocation(int nX, int nY) {
	if (nX < 0 || nX >= _nSectionCntX || nY < 0 || nY >= _nSectionCntY) {
		return FALSE;
	}
	return TRUE;
}

