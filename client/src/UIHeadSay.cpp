#include "StdAfx.h"
#include "uiheadsay.h"
#include "GameApp.h"
#include "Character.h"
#include "uiitem.h"
#include "UILabel.h"
#include "UIProgressBar.h"
#include "SkillStateRecord.h"
#include "chastate.h"
#include "uiboatform.h"
#include "uiMiniMapForm.h"
#include "UIStartForm.h"
using namespace GUI;

bool g_IsShowShop = true;
//---------------------------------------------------------------------------
// calss CHeadSay
//---------------------------------------------------------------------------
int CHeadSay::_nMaxShowTime = 100;
CGuiPic* CHeadSay::_pImgLife = new CGuiPic(NULL, 2);
CGuiPic* CHeadSay::_pImgMana = new CGuiPic(NULL, 2);

DWORD CHeadSay::_dwBkgColor = 0x90000000;

unsigned int	CHeadSay::_nFaceFrequency = 3;
CGuiPic* CHeadSay::_pImgFace = new CGuiPic[FACE_MAX];

CGuiPic* CHeadSay::_pImgTeamLeaderFlag = new CGuiPic;

CGuiPic CHeadSay::_ImgShop[3];
int		CHeadSay::_nShopFrameWidth = 0;
int		CHeadSay::_nShopFontYOff = 0;

//������ʾ��ʽ (ǰ׺) �� ��ɫ�� + (������) �� [����]
char CHeadSay::s_sNamePart[NAME_PART_NUM][64] = { 0 };
//{
//	"",							//	(
//	"",							//	ǰ׺��
//	"",							//	)
//	"",							//	��ɫ��
//	"",							//	(
//	"",							//	������
//	"",							//	)
//	"",							//	[
//	"",							//	����
//	"",							//	]
//};

char CHeadSay::s_szName[1024] = { 0 };

DWORD CHeadSay::s_dwNamePartsColors[NAME_PART_NUM][2] =			//	ǰһ����ǰ��ɫ,��һ������Ӱ��ɫ
{
	COLOR_WHITE,			COLOR_BLACK,		//	(
	0,						COLOR_BLACK,		//	ǰ׺��
	COLOR_WHITE,			COLOR_BLACK,		//	)
	0,						COLOR_BLACK,		//	��ɫ��
	COLOR_BLACK,			0,		//	(
	COLOR_BLACK,			0,		//	������
	COLOR_BLACK,			0,		//	)
	COLOR_WHITE,			COLOR_BLACK,		//	[
	COLOR_SHIP_NAME,		COLOR_BLACK,		//	����
	COLOR_WHITE,			COLOR_BLACK,		//	]
};

char CHeadSay::s_szConsortiaNamePart[4][64] = { 0 };
//{
//	"",							//������
//	"",							//"("
//	"",							//����������
//	"",							//")"
//};

char CHeadSay::s_szConsortiaName[256] = { 0 };


struct ColoredNames {
	int itemId;
	int colorId;
};

// Member Methods---------------------------------------------------------
bool CHeadSay::Init()
{
	_pImgTeamLeaderFlag->LoadImage("texture/ui/flag.tga", 12, 12, 0, 0, 0, 1.0, 1.0);

	_nShopFrameWidth = _ImgShop[0].GetWidth() - 1;
	_nShopFontYOff = (_ImgShop[0].GetHeight() - CGuiFont::s_Font.GetHeight(RES_STRING(CL_LANGUAGE_MATCH_489))) / 2;
	return true;
}

bool CHeadSay::Clear()
{
	if (_pImgLife)
	{
		delete _pImgLife;
		_pImgLife = NULL;
	}

	if (_pImgFace)
	{
		delete[] _pImgFace;
		_pImgFace = NULL;
	}

	if (_pImgTeamLeaderFlag)
	{
		delete _pImgTeamLeaderFlag;
		_pImgTeamLeaderFlag = NULL;
	}
	return true;
}

CHeadSay::CHeadSay(CCharacter* p)
	: _pOwn(p), _nShowTime(0), _pObj(NULL), _fLifeW(1.0f), _fManaW(1.0f), _dwFaceTime(0), _pCurFace(NULL), _IsShowLife(false)
	, _IsShowName(false), _nChaNameOffX(0), _fScale(0.1f), _dwNameColor(COLOR_WHITE), _ShowMPBar(true)
{
}

//��ȫ�ͷ��ڴ� by Waiting 2009-06-18
CHeadSay::~CHeadSay()
{
	SAFE_DELETE(_pObj);
}

void CHeadSay::AddItem(CItemEx* obj)
{
	if (_pObj) delete _pObj;

	_pObj = obj;

	_nShowTime = _nMaxShowTime;
	_fScale = 0.1f;
}

void CHeadSay::SetName(const char* name)
{
	_nChaNameOffX = 0 - CGuiFont::s_Font.GetWidth(name) / 2;
}

double HPPrecent(double hp, double maxhp)
{
	double precentage = (hp / maxhp) * 100.0f;

	return precentage;
}

void CHeadSay::Render(D3DXVECTOR3& pos)
{
	static int x = 0, y = 0;
	static int nSayTotalWidth = 32 * CGuiFont::s_Font.GetWidth("a"); //32���ַ��ĳ���
	float fHeight = _pOwn->GetDefaultChaInfo()->fHeight;
	g_Render.WorldToScreen(pos.x, pos.y, pos.z + fHeight, &x, &y);

	if (_IsShowLife)  //Ѫ��
	{
		char temp[32];
		DWORD color = 0xFF00FF00;
		CCharacter* pMain = CGameScene::GetMainCha();

		double precent = HPPrecent((double)_pOwn->getGameAttr()->get(ATTR_HP), (double)_pOwn->getGameAttr()->get(ATTR_MXHP));

		if (precent < 50)

			color = 0xFFFFA500;

		if (precent < 30)
			color = 0xFFDC143C;

		static int x1 = 0, y1 = 0;
		g_Render.WorldToScreen(pos.x, pos.y, pos.z, &x1, &y1);

		int nLifeWidth = _pImgLife->GetWidth();
		int nManaWidth = _pImgMana->GetWidth();
		_pImgLife->SetScaleW(1, _fLifeW);
		_pImgMana->SetScaleW(1, _fManaW);

		int nOffset = (int)((x - g_Render.GetScrWidth() / 2) * 0.02f);


		if (_pOwn->pChaMount)
			_pOwn->pChaMount->SetShowHP(false);

		if(_pOwn->GetShowHP())
			_pImgLife->RenderAll(x - nLifeWidth / 2 - nOffset, y1 + 20, (DWORD)0xFFFAFA9A);

		if (_pOwn != pMain->pChaMount && _pOwn->GetShowHP())
			_snprintf_s(temp, _countof(temp), _TRUNCATE, "HP : %lld/%lld (%ld%%)", _pOwn->getGameAttr()->get(ATTR_HP), _pOwn->getGameAttr()->get(ATTR_MXHP), (long)precent);


		if (_pOwn->IsMonster() && precent > 0)
			CGuiFont::s_Font.BRender(temp, x - nLifeWidth / 2 - nOffset, y1 + 25, (DWORD)color, 0xFF000000);

		DWORD spcolour = 0xA0FF0000;
		if (_fManaW > 0) {
			int red = 255 - (_fManaW * 255);
			int blue = _fManaW * 255;
			spcolour = (DWORD)0xA0000000 + blue + (65536 * red);

			if (_pOwn->GetShowSP() || _pOwn->IsPlayer()) {

				int yOffset = (_ShowMPBar) ? y1 + 15 : y1 + 35;

				_pImgMana->RenderAll(x - nManaWidth / 2 - nOffset, y1 + 28, spcolour);
			}
		}
	}

	if (g_stUIMap.IsPKSilver())
	{
		// ���Ҷ������������ͷ�ϲ���ʾ�κ����ݣ�����ʾ��һ�ֵ�Ѫ��
		return;
	}

	if (_pOwn->GetIsFly())
		y -= 30;


	if (_pOwn->IsTeamLeader())
	{
		y -= 20;
		GetLeaderPic()->Render(x - 6, y);
	}

	if (_pOwn->IsShop())
	{
		_RenderShop(_pOwn->getShopName(), x, y);

		static int nImageHeight = _ImgShop[0].GetHeight();
		y -= nImageHeight;
	}
	if (_nShowTime <= 0)
	{
#ifdef _LOG_NAME_                     //��ɫ����
		const int LINE_HEIGHT_STEP = 14;		//һ�еĸ߶�
		int iNameHeightStep = LINE_HEIGHT_STEP;	//��ɫ�������ĸ߶�

		// ������־�� Ctrl+F1+N ��ʾWorldID��buf����
		/*if (CCharacter::IsShowLogName)
		{
			char szLogName[128];
			_snprintf_s(szLogName, _countof(szLogName), _TRUNCATE, "%s (%u)", _pOwn->getName(), _pOwn->getAttachID());
			int  nNameLength = 0 - CGuiFont::s_Font.GetWidth(szLogName) / 2;
			CGuiFont::s_Font.BRender(szLogName, x + nNameLength, y - iNameHeightStep, _dwNameColor, COLOR_BLACK);
		}
		else*/ if (_IsShowName)
		{
			// ��������
			if (*(_pOwn->getGuildName()))
			{
				iNameHeightStep += LINE_HEIGHT_STEP;	//��������һ��
				int iGuildNameHeightStep = LINE_HEIGHT_STEP;

				//������
				if (*(_pOwn->getGuildName()))
				{
					//strncpy( s_szConsortiaNamePart[0], _pOwn->getGuildName(), NAME_LENGTH );			

					strncpy_s(s_szConsortiaNamePart[0], NAME_LENGTH, _pOwn->getGuildName(), _TRUNCATE);
				}
				else
				{
					//strncpy( s_szConsortiaNamePart[0], "", NAME_LENGTH );					
					strncpy_s(s_szConsortiaNamePart[0], NAME_LENGTH, "", _TRUNCATE);
				}
				//����������
				if (*(_pOwn->getGuildMotto()))
				{
					strncpy_s(s_szConsortiaNamePart[1], _countof(s_szConsortiaNamePart[1]), "(", _TRUNCATE);
					strncpy_s(s_szConsortiaNamePart[2], NAME_LENGTH, _pOwn->getGuildMotto(), _TRUNCATE);
					strncpy_s(s_szConsortiaNamePart[3], _countof(s_szConsortiaNamePart[3]), ")", _TRUNCATE);
				}
				else
				{
					strncpy_s(s_szConsortiaNamePart[1], NAME_LENGTH, "", _TRUNCATE);
					strncpy_s(s_szConsortiaNamePart[2], NAME_LENGTH, "", _TRUNCATE);
					strncpy_s(s_szConsortiaNamePart[3], NAME_LENGTH, "", _TRUNCATE);
				}

				//ZeroMemory(s_szConsortiaName, sizeof(s_szConsortiaName));
				s_szConsortiaName[0] = '\0';
				for (int i(0); i < 4; i++)
				{
					strncat_s(s_szConsortiaName, NAME_LENGTH, s_szConsortiaNamePart[i], _TRUNCATE);
				}
				int nNameLength = 0 - CGuiFont::s_Font.GetWidth(s_szConsortiaName) / 2;
				int iStartPosX = 0 - CGuiFont::s_Font.GetWidth(s_szConsortiaName) / 2;
				if (_pOwn->IsPlayer())
				{
					if (!_pOwn->IsNPC())
					{
						//g_pGameApp->SysInfo("%l", _pOwn->getGuildDwrd());
						//int ATTR_COLOR = Colors[_pOwn->getGameAttr()->get(ATTR_EXTEND9)];
						//_dwNameColor = ATTR_COLOR;


						s_dwNamePartsColors[NAME_INDEX][0] = _dwNameColor;
						char szBuf[NAME_LENGTH] = { 0 };
						_snprintf_s(szBuf, _countof(szBuf), _TRUNCATE, "%s | Lv %llu", _pOwn->getName(), _pOwn->getGameAttr()->get(ATTR_LV));
						strncpy_s(s_sNamePart[NAME_INDEX], NAME_LENGTH, szBuf, _TRUNCATE);
					}
				}
				// renderÿһ������
				for (int i(0); i < 4; i++)
				{

					CGuiFont::s_Font.BRender(s_szConsortiaNamePart[i], x + iStartPosX, y - iGuildNameHeightStep, _pOwn->getGuildColor(), COLOR_BLACK);
					iStartPosX += CGuiFont::s_Font.GetWidth(s_szConsortiaNamePart[i]);
				}

			}
			//	Modify Notice Please Attention, Thank you
			//	strncpy(str1,str2,sizeof(str1)) Modified to strncpy_s(str1,sizeof(str1),str2,_TRUNCATE).
			// ǰ׺��
			if (*(_pOwn->GetPreName()))
			{
				strncpy_s(s_sNamePart[PRENAME_SEP1_INDEX], _countof(s_sNamePart[PRENAME_SEP1_INDEX]), "(", _TRUNCATE);
				strncpy_s(s_sNamePart[PRENAME_INDEX], NAME_LENGTH, _pOwn->GetPreName(), _TRUNCATE);
				strncpy_s(s_sNamePart[PRENAME_SEP2_INDEX], _countof(s_sNamePart[PRENAME_SEP2_INDEX]), ")", _TRUNCATE);
				s_dwNamePartsColors[PRENAME_INDEX][0] = _pOwn->GetPreColor();
			}
			else
			{
				strncpy_s(s_sNamePart[PRENAME_SEP1_INDEX], NAME_LENGTH, "", _TRUNCATE);
				strncpy_s(s_sNamePart[PRENAME_INDEX], NAME_LENGTH, "", _TRUNCATE);
				strncpy_s(s_sNamePart[PRENAME_SEP2_INDEX], NAME_LENGTH, "", _TRUNCATE);
				s_dwNamePartsColors[PRENAME_INDEX][0] = 0;
			}

			//������
			if (*(_pOwn->getSecondName()))
			{
				strncpy_s(s_sNamePart[MOTTO_NAME_SEP1_INDEX], _countof(s_sNamePart[MOTTO_NAME_SEP1_INDEX]), "(", _TRUNCATE);
				strncpy_s(s_sNamePart[MOTTO_NAME_INDEX], NAME_LENGTH, _pOwn->getSecondName(), _TRUNCATE);
				strncpy_s(s_sNamePart[MOTTO_NAME_SEP2_INDEX], _countof(s_sNamePart[MOTTO_NAME_SEP2_INDEX]), ")", _TRUNCATE);
			}
			else
			{
				strncpy_s(s_sNamePart[MOTTO_NAME_SEP1_INDEX], NAME_LENGTH, "", _TRUNCATE);
				strncpy_s(s_sNamePart[MOTTO_NAME_INDEX], NAME_LENGTH, "", _TRUNCATE);
				strncpy_s(s_sNamePart[MOTTO_NAME_SEP2_INDEX], NAME_LENGTH, "", _TRUNCATE);
			}

			s_dwNamePartsColors[NAME_INDEX][0] = _dwNameColor;

			if (_pOwn->IsBoat())
			{
				strncpy_s(s_sNamePart[NAME_INDEX], NAME_LENGTH, _pOwn->getHumanName(), _TRUNCATE);
				strncpy_s(s_sNamePart[BOAT_NAME_SEP1_INDEX], _countof(s_sNamePart[BOAT_NAME_SEP1_INDEX]), "[", _TRUNCATE);
				strncpy_s(s_sNamePart[BOAT_NAME_INDEX], NAME_LENGTH, _pOwn->getName(), _TRUNCATE);
				strncpy_s(s_sNamePart[BOAT_NAME_SEP2_INDEX], _countof(s_sNamePart[BOAT_NAME_SEP2_INDEX]), "]", _TRUNCATE);
			}
			else
			{
				strncpy_s(s_sNamePart[BOAT_NAME_SEP1_INDEX], NAME_LENGTH, "", _TRUNCATE);
				strncpy_s(s_sNamePart[BOAT_NAME_INDEX], NAME_LENGTH, "", _TRUNCATE);
				strncpy_s(s_sNamePart[BOAT_NAME_SEP2_INDEX], NAME_LENGTH, "", _TRUNCATE);
				strncpy_s(s_sNamePart[NAME_INDEX], NAME_LENGTH, _pOwn->getName(), _TRUNCATE);
				if (_pOwn->IsPlayer())
				{
					if (!_pOwn->IsNPC())
					{
						//g_pGameApp->SysInfo("%l", _pOwn->getGuildDwrd());
						//int ATTR_COLOR = Colors[_pOwn->getGameAttr()->get(ATTR_EXTEND9)];
						//s_dwNamePartsColors[NAME_INDEX][0] = ATTR_COLOR;
						char szBuf[NAME_LENGTH] = { 0 };
						_snprintf_s(szBuf, _countof(szBuf), _TRUNCATE, "%s | Lv %llu", _pOwn->getName(), _pOwn->getGameAttr()->get(ATTR_LV));
						strncpy_s(s_sNamePart[NAME_INDEX], NAME_LENGTH, szBuf, _TRUNCATE);
					}
					else
					{

					}
				}
				if (_pOwn->IsMonster())
				{	//����ǹ���ҵȼ���������10������ʾ����ȼ�
					//g_pGameApp->SysInfo("ATTR COLOR: %d", ATTR_COLOR);

					// ����ȼ�
					static int nMainLevel(0);
					if (g_stUIBoat.GetHuman())
					{
						//	nMainLevel = g_stUIBoat.GetHuman()->getGameAttr()->get(ATTR_LV);
						nMainLevel = (int)g_stUIBoat.GetHuman()->getGameAttr()->get(ATTR_LV);
					}

					// ����ȼ�
					static int nMonsterLevel(0);
					//	nMonsterLevel = _pOwn->getGameAttr()->get(ATTR_LV);
					nMonsterLevel = (int)_pOwn->getGameAttr()->get(ATTR_LV);

					static char szBuf[NAME_LENGTH] = { 0 };
					if (nMonsterLevel - nMainLevel <= 10)
					{	//��ʾ�ȼ�
						_snprintf_s(szBuf, _countof(szBuf), _TRUNCATE, "Lv%d %s", nMonsterLevel, _pOwn->getName());
					}
					else
					{
						_snprintf_s(szBuf, _countof(szBuf), _TRUNCATE, "??? %s", _pOwn->getName());
					}
					//strncpy( s_sNamePart[NAME_INDEX], szBuf, NAME_LENGTH );
					strncpy_s(s_sNamePart[NAME_INDEX], NAME_LENGTH, szBuf, _TRUNCATE);
				}
			}

			// �õ����ֵ��ܳ�
			s_szName[0] = '\0';
			//ZeroMemory( s_szName, sizeof(s_szName) );
			for (int i(0); i < NAME_PART_NUM; i++)
			{
				strncat_s(s_szName, NAME_LENGTH, s_sNamePart[i], _TRUNCATE);
			}
			int nNameLength = 0 - CGuiFont::s_Font.GetWidth(s_szName) / 2;
			int iStartPosX = 0 - CGuiFont::s_Font.GetWidth(s_szName) / 2;

			if (_pOwn->getCharNameColor() == -1)
				s_dwNamePartsColors[NAME_INDEX][0] = COLOR_WHITE;
			// renderÿһ������
			for (int i(0); i < NAME_PART_NUM; i++)
			{
				if (s_dwNamePartsColors[i][1])
				{
					if (_pOwn->getCharNameColor() == -1 || _pOwn->getCharNameColor() == NULL || _pOwn->IsNPC() || _pOwn->IsResource())
						CGuiFont::s_Font.BRender(s_sNamePart[i], x + iStartPosX, y - iNameHeightStep, COLOR_WHITE, s_dwNamePartsColors[i][1]);
					else
						CGuiFont::s_Font.BRender(s_sNamePart[i], x + iStartPosX, y - iNameHeightStep, _pOwn->getCharNameColor(), s_dwNamePartsColors[i][1]);
				}
				else
				{
					if (_pOwn->getCharNameColor() == -1 || _pOwn->getCharNameColor() == NULL)
						CGuiFont::s_Font.Render(s_sNamePart[i], x + iStartPosX, y - iNameHeightStep, _pOwn->getCharNameColor());
					else
						CGuiFont::s_Font.Render(s_sNamePart[i], x + iStartPosX, y - iNameHeightStep, COLOR_WHITE);
				}
				if (_pOwn->IsMonster())
				{
					static int nMainLevel(0);
					if (g_stUIBoat.GetHuman())
					{
						//	nMainLevel = g_stUIBoat.GetHuman()->getGameAttr()->get(ATTR_LV);
						nMainLevel = (int)g_stUIBoat.GetHuman()->getGameAttr()->get(ATTR_LV);
					}

					// ����ȼ�
					static int nMonsterLevel(0);
					//	nMonsterLevel = _pOwn->getGameAttr()->get(ATTR_LV);
					nMonsterLevel = (int)_pOwn->getGameAttr()->get(ATTR_LV);

					static char szBuf[NAME_LENGTH] = { 0 };
					if (nMonsterLevel - nMainLevel <= 10)
					{	//��ʾ�ȼ�
						_snprintf_s(szBuf, _countof(szBuf), _TRUNCATE, "Lv%d %s", nMonsterLevel, _pOwn->getName());
					}
					else
					{
						_snprintf_s(szBuf, _countof(szBuf), _TRUNCATE, "??? %s", _pOwn->getName());
					}
					//strncpy( s_sNamePart[NAME_INDEX], szBuf, NAME_LENGTH );
					strncpy_s(s_sNamePart[NAME_INDEX], NAME_LENGTH, szBuf, _TRUNCATE);
					CGuiFont::s_Font.BRender(s_sNamePart[i], x + iStartPosX, y - iNameHeightStep, _dwNameColor, s_dwNamePartsColors[i][1]);
				}
				iStartPosX += CGuiFont::s_Font.GetWidth(s_sNamePart[i]);
			}

		}
#else
		if (_IsShowName)
		{
			if (_pOwn->IsBoat())	//��ʾ��ɫ��+����
			{
				static string sNameBuf;
				sNameBuf = _pOwn->getHumanName();
				sNameBuf += "--";
				sNameBuf += _pOwn->getName();

				int nNameLength = 0 - CGuiFont::s_Font.GetWidth(sNameBuf.c_str()) / 2;
				CGuiFont::s_Font.BRender(sNameBuf.c_str(), x + nNameLength, y - LINE_HEIGHT_STEP, _dwNameColor, COLOR_BLACK);
				if (_pOwn->IsShowSecondName())
				{
					if (strlen(_pOwn->getSecondName()) > 0)
					{
						string strSec = "(";
						strSec += _pOwn->getSecondName();
						strSec += ")";
						CGuiFont::s_Font.Render(strSec.c_str(), x - nNameLength, y - LINE_HEIGHT_STEP, COLOR_BLACK);
					}
				}
			}
			else	//��ʾ��ɫ��
			{
				int  nNameLength = 0 - CGuiFont::s_Font.GetWidth(_pOwn->getName()) / 2;
				CGuiFont::s_Font.BRender(_pOwn->getName(), x + nNameLength, y - LINE_HEIGHT_STEP, _dwNameColor, COLOR_BLACK);
				if (_pOwn->IsShowSecondName())
				{
					if (strlen(_pOwn->getSecondName()) > 0)
					{
						string strSec = "(";
						strSec += _pOwn->getSecondName();
						strSec += ")";
						CGuiFont::s_Font.Render(strSec.c_str(), x - nNameLength, y - LINE_HEIGHT_STEP, COLOR_RED);
					}
				}

			}	//end of if (_pOwn->IsBoat())
		}
#endif
	}
	else if (_nShowTime > 0)                        //����
	{
		_nShowTime--;

		if (_pObj)
		{
			int  left = x - (int)(_fScale * _pObj->GetWidth() / 2);
			int  nLine = _pObj->GetLineNum();

			string str = _pObj->GetString();

			if (str.find("#") != -1)
			{
				if (nLine == 1)
				{
					GetRender().FillFrame(left - 2, y - 20 - 32, left + (int)(_fScale * _pObj->GetWidth()) + 4, y - 14, _dwBkgColor);
					_pObj->RenderEx(left, y - 18 - 32, 32, _fScale);
				}
				else if (nLine == 2)
				{
					GetRender().FillFrame(left - 2, y - 20 - 44, left + (int)(_fScale * _pObj->GetWidth()) + 4, y - 14, _dwBkgColor);
					_pObj->RenderEx(left, y - 18 - 44, 24, _fScale);

				}
				else if (nLine == 3)
				{
					GetRender().FillFrame(left - 2, y - 20 - 76, left + (int)(_fScale * _pObj->GetWidth()) + 4, y - 14, _dwBkgColor);
					_pObj->RenderEx(left, y - 18 - 76, 24, _fScale);
				}
			}
			else
			{
				if (nLine == 1)
				{
					GetRender().FillFrame(left - 2, y - 20 - 18, left + (int)(_fScale * _pObj->GetWidth()) + 4, y - 14, _dwBkgColor);
					_pObj->RenderEx(left, y - 18 - 18, 12, _fScale);
				}
				else if (nLine == 2)
				{
					GetRender().FillFrame(left - 2, y - 20 - 36, left + (int)(_fScale * _pObj->GetWidth()) + 4, y - 14, _dwBkgColor);
					_pObj->RenderEx(left, y - 18 - 36, 12, _fScale);
				}
				else if (nLine == 3)
				{
					GetRender().FillFrame(left - 2, y - 20 - 54, left + (int)(_fScale * _pObj->GetWidth()) + 4, y - 14, _dwBkgColor);
					_pObj->RenderEx(left, y - 18 - 54, 12, _fScale);
				}
			}
			_fScale += 0.2f;
			if (_fScale > 1.0f)
				_fScale = 1.0f;

			if (_nShowTime == 4)
				_fScale = 0.8f;
			else if (_nShowTime == 3)
				_fScale = 0.6f;
			else if (_nShowTime == 2)
				_fScale = 0.4f;
			else if (_nShowTime == 1)
				_fScale = 0.2f;
		}
	}

	//ÿ�β��� _dwFaceTime(100)*33
	//ÿ֡���� _nFaceFrequency(3)*33
#define FFQ 60
	if (_pCurFace && _dwFaceTime)   //����
	{
		if (_dwFaceTime == 1)
			_dwFaceTime = g_dwCurFrameTick;

		DWORD dwElaped = g_dwCurFrameTick - _dwFaceTime;
		y -= 20;
		if (dwElaped > _nMaxShowTime*FFQ / 2)
		{
			_dwFaceTime = 0;
		}
		else if (_pCurFace->GetMax())
		{
			DWORD dwCurFrame = (dwElaped / (_nFaceFrequency*FFQ)) % _pCurFace->GetMax();
			_pCurFace->SetFrame(dwCurFrame);
			_pCurFace->Render(x - _pCurFace->GetWidth() / 2, y - 20);
		}
	}
	/*if (CGameApp::GetCurScene()->GetSceneTypeID() == enumWorldScene) {
		int nTotalState = CSkillStateRecordSet::I()->GetLastID() + 1;
		CSkillStateRecord* pState;
		for (int i = 0; i < nTotalState; i++) {
			if (_pOwn->GetStateMgr()->HasSkillState(i) && _pOwn->IsPlayer()) {
				MessageBoxA(0, "test", "", 0);
				if (_pOwn->icons.HasIcon(_pOwn->GetStateMgr()->GetSkillState(i)->chID)) {
					g_stUIStart.AddStateImageByID(i, _pOwn->icons.GetIcon(_pOwn->GetStateMgr()->GetSkillState(i)->chID), i);
				}
			}
			else {
				g_stUIStart.HideStateImgByID(i);
			}
		}
	}*/
#ifdef _LOG_NAME_                     //��ɫ����
	if (CCharacter::IsShowLogName)
	{
		CChaStateMgr* pState = _pOwn->GetStateMgr();
		int nCount = pState->GetSkillStateNum();
		for (int i = 0; i < nCount; i++)
		{
			y -= 20;
			CGuiFont::s_Font.Render(pState->GetSkillState(i)->szName, x, y, COLOR_BLUE);
		}
	}
#endif
}

bool CHeadSay::SetFaceID(unsigned int faceid)
{
	if (faceid >= FACE_MAX) return false;

	_nCurFaceFrame = 0;
	_nCurFaceCycle = 0;
	_dwFaceTime = 1;
	_pCurFace = &_pImgFace[faceid];
	return true;
}

void CHeadSay::SetSPNum(int num, int max)
{
	if (max == 0) return;

	if (num < 0)   num = 0;
	if (num > max) num = max;
	if (_pImgMana)
	{
		_fManaW = (float)num / (float)max;
	}
}

void CHeadSay::SetLifeNum(int num, int max)
{
	if (max == 0) return;

	if (num < 0)   num = 0;
	if (num > max) num = max;
	if (_pImgLife)
	{
		_fLifeW = (float)num / (float)max;
	}
}

void CHeadSay::_RenderShop(const char* szShopName, int x, int y)
{
	static int nImageHeight = _ImgShop[0].GetHeight();
	static int nImageWidth = _ImgShop[0].GetWidth();
	y -= nImageHeight;

	static int nFontWidth = 0;
	nFontWidth = CGuiFont::s_Font.GetWidth(szShopName);
	const DWORD dwImgColor = 0xffffffff;

	if (nFontWidth < 40)
	{
		static int nFontTrueWidth = 0;
		nFontTrueWidth = nFontWidth;
		nFontWidth = 40;

		x -= _nShopFrameWidth + nFontWidth / 2;

		_nShopX0 = x;
		_nShopY0 = y;
		_nShopX1 = x + nFontWidth + nImageWidth;
		_nShopY1 = y + nImageHeight;

		_ImgShop[0].Render(x, y, dwImgColor);
		x += _nShopFrameWidth;

		_ImgShop[2].Render(x + nFontWidth, y, dwImgColor);

		_ImgShop[1].SetScaleW(nFontWidth);
		_ImgShop[1].Render(x, y, dwImgColor);
		//CGuiFont::s_Font.BRender( szShopName, x + (nFontWidth - nFontTrueWidth) / 2, y+_nShopFontYOff, COLOR_BLACK, COLOR_WHITE );
		CGuiFont::s_Font.Render(szShopName, x + (nFontWidth - nFontTrueWidth) / 2, y + _nShopFontYOff, COLOR_BLACK);
	}
	else
	{
		x -= _nShopFrameWidth + nFontWidth / 2;

		_nShopX0 = x;
		_nShopY0 = y;
		_nShopX1 = x + nFontWidth + nImageWidth;
		_nShopY1 = y + nImageHeight;

		_ImgShop[0].Render(x, y, dwImgColor);
		x += _nShopFrameWidth;

		_ImgShop[2].Render(x + nFontWidth, y, dwImgColor);

		_ImgShop[1].SetScaleW(nFontWidth);
		_ImgShop[1].Render(x, y, dwImgColor);
		//CGuiFont::s_Font.BRender( szShopName, x, y+_nShopFontYOff, COLOR_BLACK, COLOR_WHITE );
		CGuiFont::s_Font.Render(szShopName, x, y + _nShopFontYOff, COLOR_BLACK);
	}
}

void CHeadSay::RenderText(const char* szShopName, int x, int y)
{
	static int nImageHeight = _ImgShop[0].GetHeight();
	static int nImageWidth = _ImgShop[0].GetWidth();
	// y -= nImageHeight;
	y -= 100;

	static int nFontWidth = 0;
	nFontWidth = CGuiFont::s_Font.GetWidth(szShopName);
	const DWORD dwImgColor = 0xffffffff;

	if (nFontWidth < 40)
	{
		static int nFontTrueWidth = 0;
		nFontTrueWidth = nFontWidth;
		nFontWidth = 40;

		x -= _nShopFrameWidth + nFontWidth / 2;

		_ImgShop[0].Render(x, y, dwImgColor);
		x += _nShopFrameWidth;

		_ImgShop[2].Render(x + nFontWidth, y, dwImgColor);

		_ImgShop[1].SetScaleW(nFontWidth);
		_ImgShop[1].Render(x, y, dwImgColor);
		CGuiFont::s_Font.BRender(szShopName, x + (nFontWidth - nFontTrueWidth) / 2, y + _nShopFontYOff, COLOR_BLACK, COLOR_WHITE);
	}
	else
	{
		x -= _nShopFrameWidth + nFontWidth / 2;

		_ImgShop[0].Render(x, y, dwImgColor);
		x += _nShopFrameWidth;

		_ImgShop[2].Render(x + nFontWidth, y, dwImgColor);

		_ImgShop[1].SetScaleW(nFontWidth);
		_ImgShop[1].Render(x, y, dwImgColor);
		CGuiFont::s_Font.BRender(szShopName, x, y + _nShopFontYOff, COLOR_BLACK, COLOR_WHITE);
	}
}

