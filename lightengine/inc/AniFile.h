//--------------------------------------------------
//  Desc: Animation File(AMF)
//  Date: 2006.12.11 /update
//  Author: artsylee
//
//  Copyright (C) 2006 artsylee
//
//  ��չ: OpenAMF, �����ṩFrames����(2006_12_15)
//		  ���ֶ�ȡAMF�ļ���ʽ(2006_12_15)
//		  �����ṹ������Ҫ��ʾ��ѹ��Ĵ�С(ѹ������ܱ�ԭ�ļ���)
//		  (����Ҫ)
//
//--------------------------------------------------

#ifndef _ANIFILE_
#define _ANIFILE_

#include <Windows.h>
#include <string>
#include <stdio.h>

class CFrames;

// artsylee's media file header
// 24 bytes total
struct AMFHeader
{
	DWORD	m_Signature;	// "AMFH"
	DWORD	m_Width;
	DWORD	m_Height;
	WORD	m_FrameCount;	// Picture count
	WORD	m_Delay;
	WORD	m_Type;
	WORD	m_Version;
	DWORD	m_Reserved;
};

// picture index (16 bytes)
struct AMFIndex
{
	DWORD	m_PictureIndex;
	DWORD	m_PictureSize;
	DWORD	m_PictureWidth;
	DWORD	m_PictureHeight;
};

class ASE_DLL CAniFile
{
public:
	CAniFile();
	~CAniFile();

	bool	SaveAMF(const char *szFileName, CFrames *pFrame) const;
	// �ļ���ʽ��ȡ
	bool	ReadHeader(const char *szFileName, AMFHeader *head) const;
	bool	ReadIndex(const char *szFileName, const DWORD &index, AMFIndex *amfindex) const;
	bool	ReadData(const char *szFileName, const DWORD &index, void *pData) const;
	// �ļ�ָ�뷽ʽ��ȡ
	bool	ReadHeader(FILE *pFile, AMFHeader *head) const;
	bool	ReadIndex(FILE *pFile, const DWORD &index, AMFIndex *amfindex) const;
	bool	ReadData(FILE *pFile, const DWORD &index, void *pData) const;

	DWORD	GetDelay() const { return m_Delay; }
	void	SetDelay(DWORD d) { m_Delay = d; }
private:
	void	CompressData(const void *pSrc, void *pDst, const DWORD &srcSize, DWORD &dstSize) const;
	bool	DeCompressData(const void *pSrc, void *pDst, const DWORD &srcSize, DWORD &dstSize) const;

private:
	DWORD	 m_Delay;
};




#endif // _ANIFILE_