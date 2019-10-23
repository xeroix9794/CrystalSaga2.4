//----------------------------------------------------------------------
// 名称:人物头上泡泡
// 作者:lh 2004-08-21
// 最后修改日期:2004-10-09
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
	~CHeadSay(); //安全释放内存 by Waiting 2009-06-18

    static bool		Init();
    static bool		Clear();

	void			Reset()						{ _nShowTime=0; _dwFaceTime=0;	}
    void			AddItem( CItemEx* obj );
    void			Render( D3DXVECTOR3& pos );

    // 设置显示的血条:num血量，max总血量,attacknum被攻击的数字
    void			SetLifeNum(int num, int max);
	void			SetSPNum(int num, int max);

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
	static CGuiPic* GetSPPic() { return _pImgMana; }
    static CGuiPic* GetLeaderPic()			{ return _pImgTeamLeaderFlag;		}
	static int	stateCount;
	static vector<int> allStates;
	static int	refreshCount;
	struct uiState {
		char iconID;
		short stateID;
		string stateName;
		string iconName;
		bool enabled;
		uiState(short id, string name, string ico) {
			stateID = id;
			stateName = name;
			iconName = ico;
			enabled = false;
			iconID = -1;
		}
		void SetValidID(int id) { this->iconID = id; }
		short GetStateID() { return stateID; }
		string GetStateName() { return stateName; }
		string GetIcon() { return string("texture\\stateicon\\") + iconName; }
		bool isEnabled() { return enabled; }
	};
	struct TempStates {
		int stateID[13];
		int isActive[13];

		TempStates() {
			memset(stateID, 0, _countof(stateID));
			memset(isActive, 0, _countof(stateID));
		}

		int GetState(int index) {
			return stateID[index];
		}
		bool IsActive(int index) {
			return (isActive[index] == 0) ? false : true;
		}

		void Reset() {
			memset(stateID, 0, _countof(stateID));
			memset(isActive, 0, _countof(isActive));
		}
	};
	struct StateIcons {
		vector<uiState> states;
		vector<int> activeStates;
		StateIcons() {
			ifstream file("scripts/txt/states.txt");
			string line;
			string data[3];
			if (file.is_open())
			{
				while (getline(file, line)) {
					// load data
					Util_ResolveTextLine(line.c_str(), data, 3, '\t');
					// Save to structure
					uiState uiStateIt(Str2Int(data[0]), data[1], data[2]);
					states.push_back(uiStateIt);
				}
			}
		}

		string GetIcon(int id) {
			string icon;
			for (auto it = states.begin(); it != states.end(); ++it) {
				if (it->GetStateID() == id)
					icon = it->GetIcon();
			}
			return icon;
		}

		bool activeState(int id)
		{
			bool ret = false;
			for (auto it = activeStates.begin(); it != activeStates.end(); ++it)
			{
				if (*it == id)
				{
					ret = true;
				}
			}
		}

		void removeActiveState(int id)
		{
			for (auto it = activeStates.begin(); it != activeStates.end(); ++it)
			{
				if (*it == id)
				{
					activeStates.erase(it);
				}
			}
		}

		int GetStateID(int id) {
			int getID;
			for (auto it = states.begin(); it != states.end(); ++it) {
				if (it->GetStateID() == id)
					getID = it->GetStateID();
			}
			return getID;
		}

		string GetIcon(string name) {
			string icon;
			for (auto it = states.begin(); it != states.end(); ++it) {
				if (it->GetStateName() == name)
					icon = it->GetIcon();
			}
			return icon;
		}

		void SetValid(int id, bool isValid, int IconID)
		{
			for (auto it = states.begin(); it != states.end(); ++it) {
				if (it->GetStateID() == id)
				{
					it->enabled = isValid;
					it->iconID = IconID;
					if (isValid == true) it->SetValidID(IconID);
					else it->SetValidID(-1);// = -1;
				}
			}
		}

		char GetValidIconID(int state) {
			char ret = -1;
			for (auto it = states.begin(); it != states.end(); ++it) {
				if (it->GetStateID() == state) {
					ret = it->iconID;
				}
			}
			return ret;
		}

		vector<int> GetIconsEnabled() {
			vector<int> ret;
			for (auto it = states.begin(); it != states.end(); ++it) {
				if (it->isEnabled())
				{
					ret.push_back(it->GetStateID());
				}
			}
			return ret;
		}

	bool IsValid(int id) {
		bool valid = false;
		for (auto it = states.begin(); it != states.end(); ++it) {
			if (it->GetStateID() == id)
			{
				valid = (it->enabled == true) ? true : false;
			}
		}
		return valid;
	}

	bool HasIcon(int id)
	{
		bool ret = false;
		for (auto it = states.begin(); it != states.end(); ++it) {
			if (it->GetStateID() == id)
				ret = true;
		}
		return ret;
	}
	};
	StateIcons icons;
	TempStates tmpStates;
	static void		SetBkgColor( DWORD v )	{ _dwBkgColor=v;					}

	bool			SetFaceID( unsigned int faceid );
    int             GetFaceID(){ return  _nCurFaceID ;}      //得到当前的ID.by billy
    void            SetRenderScale(float f ) { _fScale  = f  ;      }

    void			SetName( const char* name );

	bool			InShop( int MouseX, int MouseY );

	static void		RenderText( const char* szShopName, int x, int y );

private:
    static int		_nMaxShowTime;	// 最大能显示的时间

private:
    int				_nShowTime;		// 目前显示的时间
    float           _fScale ;       //显示的比例
    CItemEx *       _pObj;
    string          _str ;          //聊天时候显示在头像上面的文字信息
    CCharacter*		_pOwn;

private:	// 显示攻击效果时，血量变化
    static int			_nMaxShowLifeTime;	// 最大显示多长时间

    static CGuiPic*		_pImgLife;
	static CGuiPic*		_pImgMana;
    static CGuiPic*		_pImgTeamLeaderFlag;// 显示在队长头上的标志

    float				_fLifeW;
	float			    _fManaW;
	bool				_ShowHPMPText;
	bool				_ShowMPBar;
    static unsigned int	_nFaceFrequency;

    static CGuiPic*	_pImgFace;
	static DWORD	_dwBkgColor;

    int				_dwFaceTime;	// 表情初始时间
    int             _nCurFaceID ;   //目前显示的表情ID号
    DWORD           _dwNameColor;

    CGuiPic*		_pCurFace;		// 目前显示的是那张表情
    unsigned int	_nCurFaceFrame;	// 目前显示的表情第几帧
    unsigned int	_nCurFaceCycle;

    bool			_IsShowLife;	// 是否显示血条
    bool            _IsShowName;    // 是否显示角色的名字。added by billy
    int				_nChaNameOffX;	// 玩家名字的X偏移

	// 名字的部分数
	enum {	
		PRENAME_SEP1_INDEX = 0,			//	(
		PRENAME_INDEX = 1,				//	前缀名
		PRENAME_SEP2_INDEX = 2,			//	)
		NAME_INDEX = 3,					//	角色名
		MOTTO_NAME_SEP1_INDEX = 4,		//	(
		MOTTO_NAME_INDEX = 5,			//	座右铭
		MOTTO_NAME_SEP2_INDEX = 6,		//	)
		BOAT_NAME_SEP1_INDEX = 7,		//	[
		BOAT_NAME_INDEX = 8,			//	船名
		BOAT_NAME_SEP2_INDEX = 9,		//	]

		NAME_PART_NUM=10,			//	名字的组成部分的数目
	};
	//名字前缀
	//名字显示方式 (前缀) ＋ 角色名 + (座右铭) ＋ [船名]
	static char s_sNamePart[NAME_PART_NUM][64];
	//前一列是前景色,后一列是阴影颜色
	static DWORD	s_dwNamePartsColors[NAME_PART_NUM][2];
	//全名
	static char s_szName[1024];
	//公会名
	static char s_szConsortiaNamePart[4][64];
	static char s_szConsortiaName[256];

	enum 
	{ 
		NAME_LENGTH = 64,
	};

private:	// 摆摊
	static CGuiPic	_ImgShop[3];	// 0为左，1为中可以拉伸的图片，2为右
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
