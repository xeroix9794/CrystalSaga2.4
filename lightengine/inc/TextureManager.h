#ifndef _TEXTUREMANAGER_
#define _TEXTUREMANAGER_

#include <map>
#include "Texture.h"
#include "LERender.h"

typedef DWORD TextureHandle;

#define	BASEHANDLE	5000
#define UPDATETIME	3000
#define MAXEXIST	5000
#define INVALID_HANDLE 0x00

//---------------------------------------------------
// ָ���ľ����Χ��1~5000,��ָ����Ϊ5000+
// ָ����Ϊ�ǹ�������,��ָ����Ϊ��������
// ����ͬ�ļ����Ķ������,�ͷ�ֻ�ṩ����������ṩ�ļ�������
// QueryHandleΪ��������,����ֵ5000+
//---------------------------------------------------

class ASE_DLL CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();

	TextureHandle	LoadTexture(const char *szFilename, MemoryMode mode = MM_AUTO, int mipLevel = 1);
	TextureHandle	LoadAMFTexture(const char *szFilename, DWORD index = 0, MemoryMode mode = MM_AUTO, int mipLevel = 1);
	TextureHandle	LoadTextureDesc(const char *szFilename, int mipLevel = 1);
	TextureHandle	LoadAMFTextureDesc(const char *szFilename, DWORD index = 0, int mipLevel = 1);
	TextureHandle	InsertTexture(CTexture *pTexture, MemoryMode mode = MM_AUTO);
	TextureHandle	LoadTextureFromMem(int width, int height, const DWORD *pData);

	bool	LoadTexture(TextureHandle hTex, const char *szFilename, MemoryMode mode = MM_AUTO, int mipLevel = 1);
	bool	LoadAMFTexture(TextureHandle hTex, const char *szFilename, DWORD index = 0, MemoryMode mode = MM_AUTO, int mipLevel = 1);
	bool	LoadTextureDesc(TextureHandle hTex, const char *szFilename, int mipLevel = 1);
	bool	LoadAMFTextureDesc(TextureHandle hTex, const char *szFilename, DWORD index = 0, int mipLevel = 1);
	bool	BatchLoad(const char *szIniFile);

	void	ReleaseTexture(TextureHandle texHandle);
	void	DestroyTexture(TextureHandle texHandle);
	void	DestroyAll(void);

	CTexture* GetTexture(TextureHandle texHandle);
	void	Update(void);

	bool	LockTexture(TextureHandle texHandle, D3DLOCKED_RECT *pLockedRect, RECT *pRc = NULL);
	void	UnLockTexture(TextureHandle texHandle);
	// D3DPOOL_DEFAULT����Դ�ڶ�ʧ�豸ʱҪ���¼���
private:
	TextureHandle CreateHandle(void);
	TextureHandle QueryHandle(const char *szFilename, DWORD index = 0);
private:
	std::map<TextureHandle, CTexture*>	m_TextureMap;
	DWORD			m_CurHandle;
	DWORD			m_UpdateTime;
};

extern LIGHTENGINE_API CTextureManager* g_pTextureManager;

// Texture Macro
#ifndef SET_TEXTURE_UNTYPE
#define SET_TEXTURE_UNTYPE(stage, handle)  \
	pTexture = g_pTextureManager->GetTexture(handle);	\
	if(!pTexture) return;	\
	if(pTexture->AutoLoad())	\
{	\
	g_Render.GetDevice()->SetTexture((stage), pTexture->GetTexture());	\
}
#endif // SET_TEXTURE_UNTYPE

#ifndef SET_TEXTURE_TYPE
#ifndef SET_TEXTURE_UNTYPE
#error (SET_TEXTURE_UNTYPE) macro undefine!
#endif
#define SET_TEXTURE_TYPE(stage, handle) CTexture* SET_TEXTURE_UNTYPE(stage, handle)
#endif // SET_TEXTURE_TYPE

#endif // _TEXTUREMANAGER_