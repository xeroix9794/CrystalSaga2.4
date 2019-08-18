#include <stdio.h>

#include "TgaCore.h"

#include <assert.h>

#include "blk.h"
#include "MyMemory.h"

#define UL_TGA_LR 0x10
#define UL_TGA_BT 0x20

#define UL_GFX_SWAP_RGB 0x0001
#define UL_GFX_PAD 0x0004
#define UL_GFX_COLERESSED 0x0008
#define UL_GFX_SIGNEDPAD 0x0010
#define UL_GFX_INVERT_VERT 0x0020
#define UL_GFX_INVERT_HORIZ 0x0040

#define UL_GFX_565 0x00020000

#ifndef bread
#define bread() ((BYTE)fgetc(f))
#endif

#ifndef wread
#define wread() ( ((BYTE)bread()) | (((BYTE)bread())<<8) )
#endif

CGraphicsFileData::CGraphicsFileData(void) {
	m_pFileData = NULL;
	memset(&m_FileHeader, 0, sizeof(m_FileHeader));
}

CGraphicsFileData::~CGraphicsFileData(void) {
	if (m_pFileData) {
		delete[] m_pFileData;
	}
}

int CGraphicsFileData::GetImageSize(int* width, int* height, const char* file) {
	FILE* fp;
	fopen_s(&fp, file, "rb");
	if (fp == 0) { return 0; }

	TGAFILEHEADER tga_head;
	memset(&tga_head, 0, sizeof(tga_head));

	fread(&tga_head, sizeof(tga_head), 1, fp);

	*width = tga_head.Width;
	*height = tga_head.Height;

	fclose(fp);

	return 1;
}

int CGraphicsFileData::LoadTGAFile2(const char* filename) {
	FILE* fp;
	fopen_s(&fp, filename, "rb");
	if (fp == 0) { return 0; }

	fseek(fp, 0, SEEK_END);
	long len = ftell(fp);

	BYTE* pBuf = new BYTE[len];
	fseek(fp, 0, SEEK_SET);
	fread(pBuf, sizeof(BYTE)*len, 1, fp);
	LoadTGAFile(pBuf);
	delete[] pBuf;

	fclose(fp);

	return 1;
}


void CGraphicsFileData::Draw(HDC hDC, RECT * pRect, COLORREF ct) {
	int x, y;

	float fW = (float)GetWidth() / (pRect->right - pRect->left);
	float fH = (float)GetHeight() / (pRect->bottom - pRect->top);

	for (y = pRect->top; y <= pRect->bottom; y++) {
		for (x = pRect->left; x <= pRect->right; x++) {
			int x1, y1;
			x1 = (int)(x * fW);
			y1 = (int)(y * fH);
			int index = y1 * GetWidth() + x1;
			float f1, f2;
			f1 = m_pstFileData[index].A / 255.0f;
			f2 = 1 - f1;
			COLORREF c1 = ::GetPixel(hDC, x, y);
			COLORREF color = RGB(this->m_pstFileData[index].R*f1 + GetRValue(c1)*f2,
				this->m_pstFileData[index].G*f1 + GetGValue(c1)*f2,
				this->m_pstFileData[index].B*f1 + GetBValue(c1)*f2);
			::SetPixel(hDC, x, y, color);
		}
	}
}

HBITMAP CGraphicsFileData::GetBitmap(COLORREF ct) {
	HBITMAP ret = NULL;
	if (m_pFileData == NULL) { return ret; }

	HDC hdc = GetDC(NULL);

	int cx = GetWidth();
	int cy = GetHeight();

	int bitWidth = cx * 3;

	if (bitWidth % 4) {
		bitWidth += (4 - (bitWidth % 4));
	}

	BYTE* pBmp = new BYTE[bitWidth * cy];

	int x, y;
	int index = 0;
	int ix = 0;
	for (y = 0; y < cy; y++) {
		int ix1 = ix;
		for (x = 0; x < cx; x++) {
			UINT f1, f2;
			f1 = m_pstFileData[index].A;
			f2 = 255 - f1;
			pBmp[ix1 + 0] = this->m_pstFileData[index].B * f1 / 255 + GetBValue(ct) *f2 / 255;
			pBmp[ix1 + 1] = this->m_pstFileData[index].G * f1 / 255 + GetGValue(ct) *f2 / 255;
			pBmp[ix1 + 2] = this->m_pstFileData[index].R * f1 / 255 + GetRValue(ct) *f2 / 255;
			index++;
			ix1 += 3;
		}
		ix += bitWidth;
	}

	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = cx;
	
	// top-down image
	bmi.bmiHeader.biHeight = -cy;
	
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;

	ret = CreateDIBitmap(hdc, &bmi.bmiHeader, CBM_INIT, (void*)pBmp, &bmi, DIB_RGB_COLORS);

	delete[] pBmp;
	::ReleaseDC(NULL, hdc);
	return ret;
}

void CGraphicsFileData::LoadTGAFile(BYTE* pBuf) {
	if (NULL == pBuf) { return; }

	int i;

	TGAFILEHEADER * ptga = (TGAFILEHEADER*)pBuf;
	pBuf += sizeof(TGAFILEHEADER);
	memcpy(&m_FileHeader, ptga, sizeof(m_FileHeader));

	// Only TRUE color, non-mapped images are supported
	if ((0 != ptga->ColormapType && ptga->ImageType != 1) || (ptga->ImageType != 10 && ptga->ImageType != 2 && ptga->ImageType != 1)) {
		return;
	}

	m_BytesPerPixel = ptga->PixelDepth / 8;

	// Skip the ID field. The first byte of the header is the length of this field
	if (ptga->IDLength) { pBuf += ptga->IDLength; }

	_RGB aPal[256];
	int pal_count = *(WORD*)&ptga->ColormapSpecification[2];

	if (ptga->ImageType == 1) {
		for (i = 0; i < pal_count; i++) {
			aPal[i].b = pBuf[i * 3 + 0];
			aPal[i].g = pBuf[i * 3 + 1];
			aPal[i].r = pBuf[i * 3 + 2];
		}
	}

	pBuf += pal_count * 3;
	
	DWORD m_dwBPP = ptga->PixelDepth;

	delete m_pFileData;

	m_pFileData = (unsigned char *) new unsigned long[m_FileHeader.Width * m_FileHeader.Height];
	assert(m_pFileData);

	BYTE* p1 = m_pFileData;
	BYTE* p2;

	if (0 == (ptga->ImageDescriptor & 0x0020)) {
		p1 += m_FileHeader.Height * m_FileHeader.Width * 4;
		for (DWORD y = 0; y < m_FileHeader.Height; y++) {
			p1 -= m_FileHeader.Width * 4;
			DWORD dwOffset = y * m_FileHeader.Width;

			if (0 == (ptga->ImageDescriptor & 0x0020)) {
				dwOffset = (m_FileHeader.Height - y - 1)*m_FileHeader.Width;
			}

			p2 = p1;
			for (DWORD x = 0; x < m_FileHeader.Width; x) {
				if (ptga->ImageType == 10) {
					BYTE PacketInfo = *pBuf;
					++pBuf;

					WORD PacketType = 0x80 & PacketInfo;
					WORD PixelCount = (0x007f & PacketInfo) + 1;
					if (PacketType) {
						DWORD dwTemp;
						if (m_dwBPP == 32) {
							dwTemp = *(DWORD*)pBuf;
							pBuf += 4;
						}
						else if (m_dwBPP == 8) {
							dwTemp = aPal[pBuf[0]] | 0xff000000;
							pBuf += 1;
						}
						else {
							dwTemp = 0xff000000 | (pBuf[2] << 16) | (pBuf[1] << 8) | (pBuf[0]);
							pBuf += 3;
						}

						while (PixelCount--) {
							*(DWORD*)p2 = dwTemp;
							p2 += 4;
							x++;
						}
					}
					else {
						while (PixelCount--) {
							if (m_dwBPP == 32) {
								*(DWORD*)p2 = *(DWORD*)pBuf;
								pBuf += 4;
							}
							else if (m_dwBPP == 8) {
								*(DWORD*)p2 = aPal[pBuf[0]] | 0xff000000;
								pBuf++;
							}
							else {
								*(DWORD*)p2 = 0xff000000 | (pBuf[2] << 16) | (pBuf[1] << 8) | (pBuf[0]);
								pBuf += 3;
							}
							p2 += 4;
							x++;
						}
					}
				}
				else {
					if (m_dwBPP == 32) {
						*(DWORD*)p2 = *(DWORD*)pBuf;
						pBuf += 4;
					}
					else if (m_dwBPP == 8) {
						*(DWORD*)p2 = aPal[pBuf[0]] | 0xff000000;
						pBuf++;
					}
					else {
						*(DWORD*)p2 = 0xff000000 | (pBuf[2] << 16) | (pBuf[1] << 8) | (pBuf[0]);
						pBuf += 3;
					}
					p2 += 4;
					x++;
				}
			}
		}
	}
	else {
		for (DWORD y = 0; y < m_FileHeader.Height; y++) {
			p2 = p1;
			for (DWORD x = 0; x < m_FileHeader.Width; x) {
				if (ptga->ImageType == 10) {
					BYTE PacketInfo = *pBuf;
					++pBuf;

					WORD PacketType = 0x80 & PacketInfo;
					WORD PixelCount = (0x007f & PacketInfo) + 1;
					if (PacketType) {
						DWORD dwTemp;
						if (m_dwBPP == 32) {
							dwTemp = *(DWORD*)pBuf;
							pBuf += 4;
						}
						else if (m_dwBPP == 8) {
							dwTemp = aPal[pBuf[0]] | 0xff000000;
							pBuf += 1;
						}
						else {
							dwTemp = 0xff000000 | (pBuf[2] << 16) | (pBuf[1] << 8) | (pBuf[0]);
							pBuf += 3;
						}

						while (PixelCount--) {
							*(DWORD*)p2 = dwTemp;
							p2 += 4;
							x++;
						}
					}
					else {
						while (PixelCount--) {
							if (m_dwBPP == 32) {
								*(DWORD*)p2 = *(DWORD*)pBuf;
								pBuf += 4;
							}
							else if (m_dwBPP == 8) {
								*(DWORD*)p2 = aPal[pBuf[0]] | 0xff000000;
								pBuf++;
							}
							else {
								*(DWORD*)p2 = 0xff000000 | (pBuf[2] << 16) | (pBuf[1] << 8) | (pBuf[0]);
								pBuf += 3;
							}
							p2 += 4;
							x++;
						}
					}
				}
				else {
					if (m_dwBPP == 32) {
						*(DWORD*)p2 = *(DWORD*)pBuf;
						pBuf += 4;
					}
					else if (m_dwBPP == 8) {
						*(DWORD*)p2 = aPal[pBuf[0]] | 0xff000000;
						pBuf++;
					}
					else {
						*(DWORD*)p2 = 0xff000000 | (pBuf[2] << 16) | (pBuf[1] << 8) | (pBuf[0]);
						pBuf += 3;
					}
					p2 += 4;
					x++;
				}
			}
			p1 += m_FileHeader.Width * 4;
		}
	}
}
