#include "Stdafx.h"
#include "StdHeader.h"
#include "LERender.h"
#include "Texture.h"
#include "Log.h"
#include "AniFile.h"

#pragma comment (lib, "winmm.lib")

//---------------------------------------------------
//				CTexture��չ����
// 1,����LoadTextureӦ�ͷ�m_pTexture����m_IsAutoRelease״̬���ü���
// 2,AutoRemoveʱȷ��Handle��m_Name��Ϣ��Ч
// 3,ʹ��ʱ(��ʾ)Ҫ����ʱ��
// 4,D3DPOOL_DEFAULT����Դ�ڶ�ʧ�豸ʱҪ���¼���
// 5,�Զ����ʽ���Զ���������(OK,2006_12_16)
// 6,����ߴ���mipmap����.(OK,2006_12_21)
//---------------------------------------------------

//---------------------------------------------------
// 1, 3D�������ʹ�ö��mipmap, ������ߴ���Ҫ2��N�η�
// 2, 2D�õ�����ʹ��1��mipmap����, AMF��ʽ�Ϻ�
//---------------------------------------------------
CTexture::CTexture()
{
	m_Ref = 0;
	m_Width = 0;
	m_Height = 0;
	m_MemoryPool = D3DPOOL_MANAGED;
	m_Name = "";
	m_MipmapLevel = 1;
	m_pTexture = NULL;
	m_LastUseTime = 0;
	m_IsAutoRelease = false;
	m_MemoryMode = MM_AUTO;
	m_FileFlag = 0;
}

CTexture::~CTexture()
{
	Destroy();
}

//---------------------------------------------------
// ��������(�ı��ڴ�ģʽ����SetMemMode)
//---------------------------------------------------
bool CTexture::LoadTexture(const char *szFilename, int mipLevel /* = 1 */)
{
	if (szFilename == NULL || szFilename[0] == 0)
	{
		WriteLog(INFO_ERROR, "Invalidate parameter!");
		return false;
	}

	Destroy();

	HRESULT hr;
	D3DXIMAGE_INFO image_info;
	hr = D3DXGetImageInfoFromFile(szFilename, &image_info);
	if (FAILED(hr))
	{
		WriteLog(INFO_ERROR, "Can't get image info from file[%s]!", szFilename);
		return false;
	}
	// 	OutputDebugString("D3DXCreateTextureFromFileEx2:");
	// 	OutputDebugString(szFilename);
	// 	OutputDebugString("\n");
	hr = D3DXCreateTextureFromFileEx(g_Render.GetDevice(), szFilename, \
		image_info.Width, image_info.Height, mipLevel, 0, \
		//	image_info.Format, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 
		D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, \
		0, &image_info, NULL, &m_pTexture);

	if (FAILED(hr))
	{
		WriteLog(INFO_ERROR, "Can't create texture from file[%s]!", szFilename);
		return false;
	}

	m_Ref = 1;
	m_Width = image_info.Width;
	m_Height = image_info.Height;
	m_MemoryPool = D3DPOOL_MANAGED;
	m_Name = szFilename;
	m_MipmapLevel = mipLevel;
	m_LastUseTime = ::timeGetTime();

	// Output Load Texture Success Info
	// WriteLog(INFO_NORMAL, "Load texture[%s]!", szFilename);
	return true;
}

//---------------------------------------------------
// ��ȡ���������,��TextureManager����,
// ͨ��TextureManager�ӿ�ʹ������ʱ�ż�����������.
//---------------------------------------------------
bool CTexture::LoadTextureDesc(const char *szFilename, int mipLevel /* = 1 */)
{
	if (szFilename == NULL || szFilename[0] == 0)
	{
		WriteLog(INFO_ERROR, "Invalidate parameter!");
		return false;
	}
	Destroy();

	HRESULT hr;
	D3DXIMAGE_INFO image_info;
	hr = D3DXGetImageInfoFromFile(szFilename, &image_info);
	if (FAILED(hr))
	{
		WriteLog(INFO_ERROR, "Can't get image info from file[%s]!", szFilename);
		return false;
	}
	m_Ref = 1;
	m_Width = image_info.Width;
	m_Height = image_info.Height;
	m_MemoryPool = D3DPOOL_MANAGED;
	m_Name = szFilename;
	m_MipmapLevel = mipLevel;
	m_LastUseTime = ::timeGetTime();
	// ֻ����MM_AUTOģʽ
	m_IsAutoRelease = true;

	return true;
}

//---------------------------------------------------
// ��AMF��������
//---------------------------------------------------
bool CTexture::LoadAMFTexture(const char *szFilename, DWORD index /* = 0 */, int mipLevel /* = 1 */)
{
	if (szFilename == NULL || szFilename[0] == 0)
	{
		WriteLog(INFO_ERROR, "Invalidate parameter!");
		return false;
	}
	Destroy();
	if (!ReLoadAMFTexture(szFilename, index, mipLevel))
	{
		WriteLog(INFO_ERROR, "Can't Load AMF[%s]!", szFilename);
		return false;
	}

	m_Ref = 1;
	m_MemoryPool = D3DPOOL_MANAGED;
	m_Name = szFilename;
	m_MipmapLevel = mipLevel;
	m_LastUseTime = ::timeGetTime();
	// AMF��ʽ
	m_FileFlag = 0xff000000 | index;
	return true;
}

//---------------------------------------------------
// ��AMF�������������,��TextureManager����
//---------------------------------------------------
bool CTexture::LoadAMFTextureDesc(const char *szFilename, DWORD index /* =0 */, int mipLevel /* = 1 */)
{
	if (szFilename == NULL || szFilename[0] == 0)
	{
		WriteLog(INFO_ERROR, "Invalidate parameter!");
		return false;
	}
	Destroy();

	CAniFile anifile;
	FILE *fp = NULL;
	if (fopen_s(&fp, szFilename, "rb") != 0)
	{
		return false;
	}
	AMFHeader header;
	if (!anifile.ReadHeader(fp, &header))
	{
		return false;
	}

	if (index >= header.m_FrameCount)
	{
		return false;
	}
	AMFIndex picIndex;
	if (!anifile.ReadIndex(fp, index, &picIndex))
	{
		return false;
	}
	m_Width = picIndex.m_PictureWidth;
	m_Height = picIndex.m_PictureHeight;

	m_Ref = 1;
	m_MemoryPool = D3DPOOL_MANAGED;
	m_Name = szFilename;
	m_MipmapLevel = mipLevel;
	m_LastUseTime = ::timeGetTime();
	// ֻ����MM_AUTOģʽ
	m_IsAutoRelease = true;
	// AMF��ʽ
	m_FileFlag = 0xff000000 | index;

	fclose(fp);

	return true;
}

//---------------------------------------------------
// ���¼���AMF
//---------------------------------------------------
bool CTexture::ReLoadAMFTexture(const char *szFilename, DWORD index, int mipLevel /* = 1 */)
{
	if (szFilename == NULL || szFilename[0] == 0)
	{
		WriteLog(INFO_ERROR, "Invalidate parameter!");
		return false;
	}
	CAniFile anifile;
	FILE *fp = NULL;
	if (fopen_s(&fp, szFilename, "rb") != 0)
	{
		return false;
	}
	AMFHeader header;
	if (!anifile.ReadHeader(fp, &header))
	{
		return false;
	}

	if (index >= header.m_FrameCount)
	{
		return false;
	}

	AMFIndex picIndex;
	if (!anifile.ReadIndex(fp, index, &picIndex))
	{
		return false;
	}
	HRESULT hr;
	// 	OutputDebugString("D3DXCreateTexture3:");
	// 	OutputDebugString(szFilename);
	// 	OutputDebugString("\n");
	hr = D3DXCreateTexture(g_Render.GetDevice(), picIndex.m_PictureWidth, picIndex.m_PictureHeight, mipLevel, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture);
	if (FAILED(hr))
	{
		WriteLog(INFO_ERROR, "ERROR! Can't create texture.");
		return false;
	}
	m_Width = picIndex.m_PictureWidth;
	m_Height = picIndex.m_PictureHeight;
	if (mipLevel == 1)
	{
		// ��mipmap��ʽ����
		D3DLOCKED_RECT lrDst;
		m_pTexture->LockRect(0, &lrDst, 0, 0);
		if (!anifile.ReadData(fp, index, (void*)(lrDst.pBits)))
		{
			m_pTexture->UnlockRect(0);
			return false;
		}
		m_pTexture->UnlockRect(0);
	}
	else
	{	// mipmap��ʽ����
		DWORD* pData = new DWORD[m_Width*m_Height];
		if (pData == NULL)
		{
			return false;
		}
		if (!anifile.ReadData(fp, index, (void*)pData))
		{
			return false;
		}
		D3DLOCKED_RECT lrDst;
		m_pTexture->LockRect(0, &lrDst, 0, 0);
		DWORD* pDst = (DWORD*)(lrDst.pBits);
		for (int i = 0; i < m_Height; i++)
		{
			for (int j = 0; j < m_Width; j++)
			{
				pDst[i*(lrDst.Pitch / 4) + j] = pData[i*m_Width + j];
			}
		}
		m_pTexture->UnlockRect(0);
		delete[] pData;
		pData = NULL;
		D3DXFilterTexture(m_pTexture, 0, 0, D3DX_DEFAULT);
	}

	fclose(fp);
	return true;
}

//---------------------------------------------------
// ��������(��Ҫ���ڶ���֡, ��������, ������ʽ��֧��mipmap, 
// �Ҳ�֧��MM_AUTOģʽ)
//---------------------------------------------------
bool CTexture::CreateTexture(int width, int height, D3DFORMAT colorFmt, D3DPOOL mem /* = D3DPOOL_MANAGED */)
{
	Destroy();
	HRESULT hr;
	// 	OutputDebugString("D3DXCreateTexture4:");
	// 	OutputDebugString(this->m_Name.c_str());
	// 	OutputDebugString("\n");
	hr = D3DXCreateTexture(g_Render.GetDevice(), width, height, 1, 0, colorFmt, mem, &m_pTexture);
	if (FAILED(hr))
	{
		WriteLog(INFO_ERROR, "ERROR! Can't create texture.");
		return false;
	}
	m_Ref = 1;
	m_Width = width;
	m_Height = height;
	m_MemoryPool = mem;
	m_Name = "";
	m_LastUseTime = ::timeGetTime();
	// ֻ����MM_KEEPINMEMORYģʽ
	m_MemoryMode = MM_KEEPINMEMORY;

	return true;
}

//---------------------------------------------------
// ��������(��Ҫ����3D Compose����, ֧��mipmap, 
// ����֧��MM_AUTOģʽ)
//---------------------------------------------------
bool CTexture::CreateMipTexture(int width, int height, D3DFORMAT colorFmt, D3DPOOL mem /* = D3DPOOL_MANAGED */)
{
	Destroy();
	HRESULT hr;
	// 	OutputDebugString("D3DXCreateTexture5:");
	// 	OutputDebugString(this->m_Name.c_str());
	// 	OutputDebugString("\n");
	hr = D3DXCreateTexture(g_Render.GetDevice(), width, height, 0, 0, colorFmt, mem, &m_pTexture);
	if (FAILED(hr))
	{
		WriteLog(INFO_ERROR, "ERROR! Can't create texture.");
		return false;
	}
	m_Ref = 1;
	m_Width = width;
	m_Height = height;
	m_MemoryPool = mem;
	m_Name = "";
	m_LastUseTime = ::timeGetTime();
	// ֻ����MM_KEEPINMEMORYģʽ
	m_MemoryMode = MM_KEEPINMEMORY;

	return true;
}

//---------------------------------------------------
// �Զ�����ͼƬ����
//---------------------------------------------------
bool CTexture::AutoLoad(void)
{
	if (m_IsAutoRelease && !m_Name.empty())
	{
		if (m_FileFlag & 0xff000000)
		{
			if (!ReLoadAMFTexture(m_Name.c_str(), m_FileFlag & 0x00ffffff, m_MipmapLevel))
			{
				return false;
			}
		}
		else
		{
			HRESULT hr;
			D3DXIMAGE_INFO image_info;
			hr = D3DXGetImageInfoFromFile(m_Name.c_str(), &image_info);
			if (FAILED(hr))
			{
				WriteLog(INFO_ERROR, "Auto get image info fail[%s]!", m_Name.c_str());
				return false;
			}
			// 			OutputDebugString("D3DXCreateTextureFromFileEx6:");
			// 			OutputDebugString(m_Name.c_str());
			// 			OutputDebugString("\n");
			hr = D3DXCreateTextureFromFileEx(g_Render.GetDevice(), m_Name.c_str(), \
				image_info.Width, image_info.Height, m_MipmapLevel, 0, \
				//	image_info.Format, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 
				D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, \
				0, &image_info, NULL, &m_pTexture);

			if (FAILED(hr))
			{
				WriteLog(INFO_ERROR, "Auto load texture fail[%s]!", m_Name.c_str());
				return false;
			}
		}

		m_IsAutoRelease = false;
	}
	return true;
}

//---------------------------------------------------
// �Զ��ͷ�ͼƬ����
//---------------------------------------------------
int CTexture::AutoRemove(void)
{
	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
	m_IsAutoRelease = true;
	return m_Ref;
}

int CTexture::AddRef(void)
{
	return ++m_Ref;
}

int CTexture::Release(void)
{
	--m_Ref;
	if (m_Ref <= 0)
	{
		Destroy();
	}
	return m_Ref;
}

void CTexture::Destroy(void)
{
	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
	if (!m_Name.empty())
	{
		m_Name.clear();
	}
	m_MipmapLevel = 1;
	m_FileFlag = 0;
	m_IsAutoRelease = false;
}

// end of this file 2006.10.26 artsylee
