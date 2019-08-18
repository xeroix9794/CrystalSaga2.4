//----------------------------------------------------------------------
// ����:����ͷ������
// ����:lh 2004-08-21
// ����޸�����:2004-10-09
//----------------------------------------------------------------------
#pragma once
#include "uiGuidata.h"

class CCharacter;

namespace GUI
{
const int FACE_MAX = 50;

class CGuiPic;
class CItemEx;

class CHeadSay
{
public:
    CHeadSay(CCharacter* p);
	~CHeadSay(); //��ȫ�ͷ��ڴ� by Waiting 2009-06-18

    static bool		Init();
    static bool		Clear();

	void			Reset()						{ _nShowTime=0; _dwFaceTime=0;	}
    void			AddItem( CItemEx* obj );
    void			Render( D3DXVECTOR3& pos );

    // ������ʾ��Ѫ��:numѪ����max��Ѫ��,attacknum������������
    void			SetLifeNum(int num, int max);
    void			SetIsShowLife( bool v )		{ _IsShowLife = v;		}
    bool			GetIsShowLife()				{ return _IsShowLife;	}

    void            SetIsShowName( bool v )     { _IsShowName = v;      }
    bool            GetIsShowName()             { return _IsShowName ;  } 
    void            SetNameColor( DWORD v )     { _dwNameColor=v;       }

    static void		SetMaxShowTime( int v )		{ _nMaxShowTime = v;	}

    static void		SetMaxShowLiftTime( int n )	{ _nMaxShowLifeTime = n;}
    static void		SetFaceFrequency( int n )	{ if(n>0) _nFaceFrequency=n;	}
    static CGuiPic* GetFacePic( unsigned int n ) { if( n>=FACE_MAX) return NULL; return &_pImgFace[n]; }
    static CGuiPic* GetShopPic( unsigned int n ) { if( n>=3) return NULL; return &_ImgShop[n]; }

    static CGuiPic* GetLifePic()			{ return _pImgLife;					}
    static CGuiPic* GetLeaderPic()			{ return _pImgTeamLeaderFlag;		}

	static void		SetBkgColor( DWORD v )	{ _dwBkgColor=v;					}

	bool			SetFaceID( unsigned int faceid );
    int             GetFaceID(){ return  _nCurFaceID ;}      //�õ���ǰ��ID.by billy
    void            SetRenderScale(float f ) { _fScale  = f  ;      }

    void			SetName( const char* name );

	bool			InShop( int MouseX, int MouseY );

	static void		RenderText( const char* szShopName, int x, int y );

private:
    static int		_nMaxShowTime;	// �������ʾ��ʱ��

private:
    int				_nShowTime;		// Ŀǰ��ʾ��ʱ��
    float           _fScale ;       //��ʾ�ı���
    CItemEx *       _pObj;
    string          _str ;          //����ʱ����ʾ��ͷ�������������Ϣ
    CCharacter*		_pOwn;

private:	// ��ʾ����Ч��ʱ��Ѫ���仯
    static int			_nMaxShowLifeTime;	// �����ʾ�೤ʱ��

    static CGuiPic*		_pImgLife;
    static CGuiPic*		_pImgTeamLeaderFlag;// ��ʾ�ڶӳ�ͷ�ϵı�־

    float				_fLifeW;

    static unsigned int	_nFaceFrequency;

    static CGuiPic*	_pImgFace;
	static DWORD	_dwBkgColor;

    int				_dwFaceTime;	// �����ʼʱ��
    int             _nCurFaceID ;   //Ŀǰ��ʾ�ı���ID��
    DWORD           _dwNameColor;

    CGuiPic*		_pCurFace;		// Ŀǰ��ʾ�������ű���
    unsigned int	_nCurFaceFrame;	// Ŀǰ��ʾ�ı���ڼ�֡
    unsigned int	_nCurFaceCycle;

    bool			_IsShowLife;	// �Ƿ���ʾѪ��
    bool            _IsShowName;    // �Ƿ���ʾ��ɫ�����֡�added by billy
    int				_nChaNameOffX;	// ������ֵ�Xƫ��

	// ���ֵĲ�����
	enum {	
		PRENAME_SEP1_INDEX = 0,			//	(
		PRENAME_INDEX = 1,				//	ǰ׺��
		PRENAME_SEP2_INDEX = 2,			//	)
		NAME_INDEX = 3,					//	��ɫ��
		MOTTO_NAME_SEP1_INDEX = 4,		//	(
		MOTTO_NAME_INDEX = 5,			//	������
		MOTTO_NAME_SEP2_INDEX = 6,		//	)
		BOAT_NAME_SEP1_INDEX = 7,		//	[
		BOAT_NAME_INDEX = 8,			//	����
		BOAT_NAME_SEP2_INDEX = 9,		//	]

		NAME_PART_NUM=10,			//	���ֵ���ɲ��ֵ���Ŀ
	};
	//����ǰ׺
	//������ʾ��ʽ (ǰ׺) �� ��ɫ�� + (������) �� [����]
	static char s_sNamePart[NAME_PART_NUM][64];
	//ǰһ����ǰ��ɫ,��һ������Ӱ��ɫ
	static DWORD	s_dwNamePartsColors[NAME_PART_NUM][2];
	//ȫ��
	static char s_szName[1024];
	//������
	static char s_szConsortiaNamePart[4][64];
	static char s_szConsortiaName[256];

	enum 
	{ 
		NAME_LENGTH = 64,
	};

private:	// ��̯
	static CGuiPic	_ImgShop[3];	// 0Ϊ��1Ϊ�п��������ͼƬ��2Ϊ��
	static int		_nShopFrameWidth;
	static int		_nShopFontYOff;
	void			_RenderShop( const char* szShopName, int x, int y );

	int	_nShopX0, _nShopY0, _nShopX1, _nShopY1;
};

inline bool CHeadSay::InShop( int nMouseX, int nMouseY )
{
	return nMouseX > _nShopX0 && nMouseX < _nShopX1 && nMouseY > _nShopY0 && nMouseY < _nShopY1;
}

}
