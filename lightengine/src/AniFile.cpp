#include "Stdafx.h"
#include "StdHeader.h"
#include "AniFile.h"
#include "Frames.h"

CAniFile::CAniFile()
{
	m_Delay = 100;
}

CAniFile::~CAniFile()
{
}

//---------------------------------------------------
//				base information
// 1, �����ļ���С����, AMF�ļ��е�ͼ��֧�ֶ��Mipmap
//---------------------------------------------------

//---------------------------------------------------
// ���浽AMF�ļ�(�༭��ʹ��)
//---------------------------------------------------
bool CAniFile::SaveAMF(const char *szFileName, CFrames *pFrame) const
{
	if (pFrame == NULL || szFileName == NULL)
	{
		return false;
	}
	FILE* fp = NULL;
	if (fopen_s(&fp, szFileName, "wb") != 0)
	{
		return false;
	}
	AMFHeader header;
	memset(&header, 0, sizeof(AMFHeader));
	header.m_Signature = 'HFMA';
	header.m_Width = pFrame->GetWidth();
	header.m_Height = pFrame->GetHeight();
	header.m_FrameCount = pFrame->GetFrameCount();
	header.m_Delay = (WORD)m_Delay;
	header.m_Type = pFrame->GetType();;
	header.m_Version = 1000;
	fwrite(&header, sizeof(AMFHeader), 1, fp);
	fseek(fp, sizeof(AMFIndex)*header.m_FrameCount, SEEK_CUR);
	AMFIndex picIndex;
	picIndex.m_PictureIndex = ftell(fp);
	for (int i = 0; i < header.m_FrameCount; i++)
	{
		CTexture* pCTexture = pFrame->GetTexture(i);
		if (pCTexture == NULL)	return false;
		DWORD Width = pCTexture->GetWidth();
		DWORD Height = pCTexture->GetHeight();
		picIndex.m_PictureWidth = Width;
		picIndex.m_PictureHeight = Height;

		IDirect3DTextureX* pTexture = pCTexture->GetTexture();
		if (pTexture == NULL)	return false;
		D3DLOCKED_RECT d3dlr;
		pTexture->LockRect(0, &d3dlr, 0, 0);
		DWORD dstSize = 0;
		// �����ռ�
		BYTE* pData = new BYTE[Width * Height * sizeof(DWORD) * 2];
		if (pData == NULL)
			return false;
		CompressData(d3dlr.pBits, pData, Width * Height * sizeof(DWORD), dstSize);
		pTexture->UnlockRect(0);
		picIndex.m_PictureSize = dstSize;
		fseek(fp, sizeof(AMFIndex)*i + sizeof(AMFHeader), SEEK_SET);
		fwrite(&picIndex, sizeof(AMFIndex), 1, fp);

		fseek(fp, picIndex.m_PictureIndex, SEEK_SET);
		fwrite(pData, dstSize, 1, fp);
		picIndex.m_PictureIndex = ftell(fp);
		delete[] pData;
		pData = NULL;
	}

	fclose(fp);

	return true;
}

//---------------------------------------------------
// ��ȡ�ļ�ͷ
//---------------------------------------------------
bool CAniFile::ReadHeader(const char *szFileName, AMFHeader *head) const
{
	if (szFileName == NULL || head == NULL)
	{
		return false;
	}
	FILE* fp = NULL;
	if (fopen_s(&fp, szFileName, "rb") != 0)
	{
		return false;
	}

	fread(head, sizeof(AMFHeader), 1, fp);
	if (head->m_Signature != 'HFMA')
	{
		MessageBox(NULL, "Invalid AMF File!", NULL, MB_OK);
		return false;
	}
	fclose(fp);
	return true;
}

//---------------------------------------------------
// ��ȡ�ļ�ͷ
//---------------------------------------------------
bool CAniFile::ReadHeader(FILE *pFile, AMFHeader *head) const
{
	if (pFile == NULL || head == NULL)
	{
		return false;
	}
	fseek(pFile, 0, SEEK_SET);
	fread(head, sizeof(AMFHeader), 1, pFile);
	if (head->m_Signature != 'HFMA')
	{
		MessageBox(NULL, "Invalid AMF File!", NULL, MB_OK);
		return false;
	}
	return true;
}

//---------------------------------------------------
// ��ȡ���ݿ�����
//---------------------------------------------------
bool CAniFile::ReadIndex(const char *szFileName, const DWORD &index, AMFIndex *amfindex) const
{
	if (szFileName == NULL || amfindex == NULL)
	{
		return false;
	}
	FILE* fp = NULL;
	if (fopen_s(&fp, szFileName, "rb") != 0)
	{
		return false;
	}
	AMFHeader header;
	fread(&header, sizeof(AMFHeader), 1, fp);
	if (header.m_Signature != 'HFMA')
	{
		MessageBox(NULL, "Invalid AMF File!", NULL, MB_OK);
		return false;
	}

	if (header.m_FrameCount == 0)
		return false;
	if (index >= header.m_FrameCount)
		return false;

	fseek(fp, sizeof(AMFIndex) * index + sizeof(AMFHeader), SEEK_SET);
	fread(amfindex, sizeof(AMFIndex), 1, fp);

	fclose(fp);
	return true;
}

//---------------------------------------------------
// ��ȡ���ݿ�����
//---------------------------------------------------
bool CAniFile::ReadIndex(FILE *pFile, const DWORD &index, AMFIndex *amfindex) const
{
	if (pFile == NULL || amfindex == NULL)
	{
		return false;
	}

	AMFHeader header;
	fseek(pFile, 0, SEEK_SET);
	fread(&header, sizeof(AMFHeader), 1, pFile);
	if (header.m_Signature != 'HFMA')
	{
		MessageBox(NULL, "Invalid AMF File!", NULL, MB_OK);
		return false;
	}

	if (header.m_FrameCount == 0)
		return false;
	if (index >= header.m_FrameCount)
		return false;

	fseek(pFile, sizeof(AMFIndex) * index + sizeof(AMFHeader), SEEK_SET);
	fread(amfindex, sizeof(AMFIndex), 1, pFile);

	return true;
}

//---------------------------------------------------
// ��ȡ���ݿ�
//---------------------------------------------------
bool CAniFile::ReadData(const char *szFileName, const DWORD &index, void *pData) const
{
	if (szFileName == NULL || pData == NULL)
	{
		return false;
	}
	FILE* fp = NULL;
	if (fopen_s(&fp, szFileName, "rb") != 0)
	{
		return false;
	}
	AMFHeader header;
	fread(&header, sizeof(AMFHeader), 1, fp);
	if (header.m_Signature != 'HFMA')
	{
		MessageBox(NULL, "Invalid AMF File!", NULL, MB_OK);
		return false;
	}

	if (header.m_FrameCount == 0)
		return false;
	if (index >= header.m_FrameCount)
		return false;

	fseek(fp, sizeof(AMFIndex) * index + sizeof(AMFHeader), SEEK_SET);
	AMFIndex amfindex;
	fread(&amfindex, sizeof(AMFIndex), 1, fp);
	fseek(fp, amfindex.m_PictureIndex, SEEK_SET);
	BYTE* pTempData = new BYTE[amfindex.m_PictureSize];
	if (pTempData == NULL)
		return false;

	fread(pTempData, amfindex.m_PictureSize, 1, fp);
	DWORD dstSize = 0;
	DeCompressData(pTempData, pData, amfindex.m_PictureSize, dstSize);
	delete[] pTempData;
	pTempData = NULL;

	fclose(fp);
	return true;
}

//---------------------------------------------------
// ��ȡ���ݿ�
//---------------------------------------------------
bool CAniFile::ReadData(FILE *pFile, const DWORD &index, void *pData) const
{
	if (pFile == NULL || pData == NULL)
	{
		return false;
	}

	AMFHeader header;
	fseek(pFile, 0, SEEK_SET);
	fread(&header, sizeof(AMFHeader), 1, pFile);
	if (header.m_Signature != 'HFMA')
	{
		MessageBox(NULL, "Invalid AMF File!", NULL, MB_OK);
		return false;
	}

	if (header.m_FrameCount == 0)
		return false;
	if (index >= header.m_FrameCount)
		return false;

	fseek(pFile, sizeof(AMFIndex) * index + sizeof(AMFHeader), SEEK_SET);
	AMFIndex amfindex;
	fread(&amfindex, sizeof(AMFIndex), 1, pFile);
	fseek(pFile, amfindex.m_PictureIndex, SEEK_SET);
	BYTE* pTempData = new BYTE[amfindex.m_PictureSize];
	if (pTempData == NULL)
		return false;

	fread(pTempData, amfindex.m_PictureSize, 1, pFile);
	DWORD dstSize = 0;
	DeCompressData(pTempData, pData, amfindex.m_PictureSize, dstSize);
	delete[] pTempData;
	pTempData = NULL;

	return true;
}

//---------------------------------------------------
// RLEѹ���㷨(����dstSize����srcSize)
//---------------------------------------------------
void CAniFile::CompressData(const void *pSrc, void *pDst, const DWORD &srcSize, DWORD &dstSize) const
{
	if ((pSrc == NULL) || (pDst == NULL) || srcSize <= 0)
	{
		return;
	}

	int totalCompress = 0;
	int repCount = 1;
	DWORD	blockHead = 0;

	DWORD *pComData = (DWORD *)pDst;
	DWORD *pRawData = (DWORD *)pSrc;

	int len = srcSize >> 2;
	dstSize = 0;

	while (totalCompress < len)
	{
		if (!(*(pRawData + repCount - 1) & 0xff000000) && !(*(pRawData + repCount) & 0xff000000))
		{	// ͨ��Ϊ0x00
			while (!(*(pRawData + repCount - 1) & 0xff000000) && !(*(pRawData + repCount) & 0xff000000)
				&& ((totalCompress + repCount) < len))
			{
				++repCount;
			}

			blockHead = 0x11000000 | repCount;
			*pComData++ = blockHead;

			dstSize += 1;

		}
		else if (*pRawData == *(pRawData + repCount))
		{	// ��ɫ��ͬ
			while (*pRawData == *(pRawData + repCount)
				&& ((totalCompress + repCount) < len))
			{
				++repCount;
			}
			blockHead = 0x10000000 | repCount;
			*pComData++ = blockHead;
			*pComData++ = *pRawData;

			dstSize += 2;
		}
		else
		{
			// ��ɫ��ͬ
			while (*(pRawData + repCount - 1) != *(pRawData + repCount)
				&& ((totalCompress + repCount) < len))
			{
				if (!(*(pRawData + repCount - 1) & 0xff000000) && !(*(pRawData + repCount) & 0xff000000))
				{
					break;
				}
				++repCount;
			}

			blockHead = 0x00000000 | repCount;
			*pComData++ = blockHead;
			memcpy(pComData, pRawData, sizeof(DWORD)*repCount);
			pComData += repCount;

			dstSize += repCount + 1;
		}
		pRawData += repCount;
		totalCompress += repCount;
		repCount = 1;
	}

	dstSize *= 4;
}

//---------------------------------------------------
// RLE��ѹ���㷨
//---------------------------------------------------
bool CAniFile::DeCompressData(const void *pSrc, void *pDst, const DWORD &srcSize, DWORD &dstSize) const
{
	if ((pSrc == NULL) || (pDst == NULL) || srcSize <= 0)
	{
		return false;
	}
	DWORD totalSize = srcSize >> 2;
	DWORD *pDSrc = (DWORD *)pSrc;
	DWORD *pDstData = (DWORD *)pDst;

	dstSize = 0;
	DWORD deCompress = 0;
	DWORD nowSize = 0;
	//-----------------
	// C++�汾
	//-----------------
	/*
	while(nowSize < totalSize)
	{
		DWORD blockData = 0;
		blockData = *pDSrc++;
		if(!(blockData & 0xff000000))
		{
			int len = blockData & 0x00ffffff;
			memcpy(pDstData, pDSrc, sizeof(DWORD)*len);
			pDstData += len;
			pDSrc += len;
			nowSize += len+1;

			dstSize += len;
		}
		else
		{
			int len = blockData & 0x00ffffff;
			DWORD CompData = 0;
			if(!(blockData & 0x0f000000))
			{
				CompData = *pDSrc++;
				nowSize += 1;
			}
			for(int i=0; i<len; i++)
			{
				*pDstData++ = CompData;
			}
			nowSize += 1;

			dstSize += len;
		}
	}
	dstSize *= 4;
	*/
	//-----------------
	// asm�汾
	//-----------------
	__asm
	{
		mov  esi, pDSrc
		mov  edi, pDstData
		loop_start :
		lodsd; ��block��ȡ��eax��
			mov  ebx, eax
			and  ebx, 0x00ffffff; ebx ������ݵĳ�����Ϣ
			and  eax, 0xff000000
			jnz  compress
			; ԭʼ����
			mov  ecx, ebx

			cld
			rep  movsd

			mov  edx, ebx; ��ȡlength + 1
			inc  edx
			jmp  new_block

			compress :
		mov  ecx, ebx; ѹ������
			mov  edx, 0x01

			and eax, 0x0f000000
			jnz  loadzero
			lodsd
			inc	 edx
			jmp  notloadzero
			loadzero :
		mov	 eax, 0
			notloadzero :
			cld
			rep  stosd

			new_block :
		add	 deCompress, ebx; �����ܵĳ�����Ϣ
			add  nowSize, edx; ���㵱ǰ�Ѿ���ѹ�ĳ���

			mov  eax, nowSize
			cmp  eax, totalSize
			jnz  loop_start
	};
	dstSize = 4 * deCompress;

	return true;
}
