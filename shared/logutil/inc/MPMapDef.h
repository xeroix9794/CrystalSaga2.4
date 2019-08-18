#pragma once

// version description
// The current version is converted to map 780625 version -> 780626 version
// obj 600 version -> 700 version
// The purpose is to further reduce the size of the map file and the obj file
// This version uses short to represent the surface vertex color, byte to represent the surface vertex height
// Reduce the file size by reducing the precision, and the new version of the file will be reduced by one-third

#define MP_MAP_FLAG			780624		// Map file version number starts
#define LAST_VERSION_NO		780626		// The previous version needs to support the conversion	
#define CUR_VERSION_NO		780627		// The currently used version number

// Map file header definition
struct MPMapFileHeader {
	int nMapFlag;
	int nWidth;
	int nHeight;
	int nSectionWidth;
	int nSectionHeight;
};

#pragma pack(push)
#pragma pack(1)

// A total of 15 bytes
struct SNewFileTile {
	DWORD dwTileInfo;		// Save tets and alpha no for 3 layers
	BYTE btTileInfo;		// The bottom tex no
	short sColor;			// 32bit color saved as 565
	char cHeight;			// Every 10cm for a level to save height
	short sRegion;
	BYTE btIsland;
	BYTE btBlock[4];

	SNewFileTile() : sRegion(0), btIsland(0) {
		btBlock[0] = 0;
		btBlock[1] = 0;
		btBlock[2] = 0;
		btBlock[3] = 0;
	}
};

struct SFileTile {
	BYTE  t[8];
	short sHeight;		// The height of the surface vertex in centimeters
	DWORD dwColor;
	short sRegion;
	BYTE  btIsland;
	BYTE  btBlock[4];	// 4 x 1 obstacles and height records

	SFileTile() : sRegion(0), btIsland(0) {
		btBlock[0] = 0;
		btBlock[1] = 0;
		btBlock[2] = 0;
		btBlock[3] = 0;
	}
};

#pragma pack(pop)

// The original 8 bytes of Tile information, converted to 5 bytes
// Each texture number is 6 digits, from 1 to 63, up to 63, except 0
// Each alpha number occupies 4 bits, 0 ~ 15, and up to 16 styles
inline void TileInfo_8To5(BYTE *pbtTile, DWORD &dwNew, BYTE &btNew) {
	DWORD dwTex1 = *(pbtTile + 2);
	DWORD dwTex2 = *(pbtTile + 4);
	DWORD dwTex3 = *(pbtTile + 6);
	DWORD dwAlpha1 = *(pbtTile + 3);
	DWORD dwAlpha2 = *(pbtTile + 5);
	DWORD dwAlpha3 = *(pbtTile + 7);
	dwNew = dwTex1 << 26 | dwAlpha1 << 22 | dwTex2 << 16 | dwAlpha2 << 12 | dwTex3 << 6 | dwAlpha3 << 2;
	btNew = *(pbtTile + 0);
}

// 5 bytes to 8 bytes of reverse conversion
inline void TileInfo_5To8(DWORD dwNew, BYTE btNew, BYTE *pbtTile) {
	*(pbtTile + 0) = btNew;
	*(pbtTile + 1) = 15;
	*(pbtTile + 2) = (BYTE)(dwNew >> 26);
	*(pbtTile + 3) = (BYTE)(dwNew >> 22) & 0x000f;
	*(pbtTile + 4) = (BYTE)(dwNew >> 16) & 63;
	*(pbtTile + 5) = (BYTE)(dwNew >> 12) & 0x000f;
	*(pbtTile + 6) = (BYTE)(dwNew >> 6) & 63;
	*(pbtTile + 7) = (BYTE)(dwNew >> 2) & 0x000f;
}

#define NEW_VERSION  // MindPower map read whether to use the new version
