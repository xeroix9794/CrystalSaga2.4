#pragma once

#include <windows.h>

struct IMG_HEADER {
	DWORD m_dwMarks;			// Sign bit 'RYO'
	WORD m_LibType;				// Packaged image type
	WORD m_wPackedDirectory;	// Number of directories packed in the image package, maximum allowable number 65535
	WORD m_wGridX;				// The position of the center of gravity of all the images in the package
	WORD m_wGridY;
	WORD m_wVer;				// Version number
	BYTE m_bAlphaInfo;			// Alpha Information
	BYTE m_bShadowInfo;			// Shadow information
};

struct IMG_BLOCK {
	BYTE m_byPixelDepth;		// Pixel depth
	BYTE m_byUseRle;			// Whether to use RLE
	BYTE m_byUseAlpha;			// Is there an alpha channel?
	BYTE m_byNone1;

	WORD m_wPalCount;			// The number of color disks used in the block
	WORD m_wWidth;				// Image width
	WORD m_wHeight;				// Image Height

	WORD m_wTotalWidthBlock;
	WORD m_wTotalHeightBlock;
	WORD m_wTailBlockWidth;
	WORD m_wTailBlockHeight;

	BYTE m_byNone2;
	BYTE m_byNone3;

	RECT m_rClip;
};

struct _RGB {
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
	operator DWORD() { return *(DWORD*)this; }

	bool operator == (_RGB& v) { return ((0x00ffffff & this->operator DWORD()) == (0x00ffffff & v.operator DWORD())); }
	_RGB& operator = (DWORD dwRGB) { (*this) = (*(_RGB*)&dwRGB); return *this; }

	// Returns the difference between the two colors
	unsigned int Difference(_RGB& v) {
		int dr, dg, db;
		dr = (int)r - (int)v.r;
		dg = (int)g - (int)v.g;
		db = (int)b - (int)v.b;

		if (dr < 0) dr = -dr;
		if (dg < 0) dg = -dg;
		if (db < 0) db = -db;
		return (dr + dg + db);
	}
};

#define	MAX_TEXTURE_WIDTH 256

class CBlk {
public:
	CBlk(void);
	~CBlk(void);

	static BOOL Compress_BLK_RLE(const char* filename, DWORD* pARGB, int width, int height, BOOL bAlpha = TRUE);
	static BOOL Compress_BLK(const char* filename, DWORD* pARGB, int width, int height);
	static BOOL CompressPal(const DWORD* pARGB, int width, int height, _RGB* pPal, int & crCount);

	static int SearchPal(_RGB* pPal, int nCount, _RGB color);
	static int GetColorIndex(_RGB* pPal, int nCount, _RGB color);

	static RECT FoundRect(DWORD* pARGB, int width, int height);
	static int UseableSize(int size);
	static void ProcessLine(BYTE* pData, int width, BYTE* pRetData, int& nRetLen);

	void LoadBlk(const char* filename);
	void LoadBlk(BYTE* pBuf);

	typedef struct _stFileData {
		BYTE B;
		BYTE G;
		BYTE R;
		BYTE A;
	} stFileData;

	union {
		stFileData* m_pstFileData;
		DWORD *m_pFileData;
	};

	int m_iWidth;
	int m_iHeight;
	
	static int DecodeLine(BYTE* pData, int width, BYTE* pRetData, int & nRetLen);
	static void DecodeLine(BYTE* pData, int width, _stFileData* pRetData, int & nRetLen, _RGB* pal);
	static void DecodeAlphaLine(BYTE* pData, int width, _stFileData* pRetData, int & nRetLen);
	
	HBITMAP GetBitmap(COLORREF ct);
	
	static void PalTo565(_RGB* pal, int pal_count, WORD* pRet);
	static void _565ToPal(WORD* p565, int pal_count, _RGB* pal);
};
