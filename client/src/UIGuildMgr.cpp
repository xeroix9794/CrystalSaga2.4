//------------------------------------------------------------------------
//	2005.4.27	Arcol	create this file
//------------------------------------------------------------------------


#include "stdafx.h"
#include "UIPage.h"
#include "netguild.h"
#include "UIFormMgr.h"
#include "UITextButton.h"
#include "GuildMemberData.h"
#include "GuildMembersMgr.h"
#include "RecruitMemberData.h"
#include "RecruitMembersMgr.h"
#include "UIBoxForm.h"
#include "UIListView.h"
#include "UILabel.h"
#include "UIEdit.h"
#include "GameApp.h"
#include "Character.h"
#include "uiguildmgr.h"
#include "commfunc.h"
#include "procirculate.h"
#include "packetcmd.h"

// FEATURE: GUILD_BANK
#include "UIGoodsGrid.h"
#include "UICheckBox.h"
#include "UIItemCommand.h"
#include "UIGlobalVar.h"
#include "UIBoatForm.h"


#define ATTR_BUTTON_CLICK_INTERVAL 10

CForm*	CUIGuildMgr::m_pGuildMgrForm=NULL;
CLabelEx*	CUIGuildMgr::m_plabGuildName=NULL;
CLabelEx*	CUIGuildMgr::m_plabGuildMottoName=NULL;
CLabelEx*	CUIGuildMgr::m_plabGuildType=NULL;
CLabelEx*	CUIGuildMgr::m_plabGuildMaster=NULL;
CLabelEx*	CUIGuildMgr::m_plabGuildMemberCount=NULL;
CLabelEx*	CUIGuildMgr::m_plabGuildExperience=NULL;
CLabelEx*	CUIGuildMgr::m_plabGuildMoney=NULL;
CLabelEx*	CUIGuildMgr::m_plabGuildState=NULL;
CLabelEx*	CUIGuildMgr::m_plabGuildRemainTime=NULL;
//CLabelEx*	CUIGuildMgr::m_plabGuildRank=NULL;
CListView*	CUIGuildMgr::m_plstGuildMember=NULL;
CListView*	CUIGuildMgr::m_plstRecruitMember=NULL;
CPage*		CUIGuildMgr::m_ppgeClass=NULL;
CTextButton*	CUIGuildMgr::m_pbtnGuildMottoEdit=NULL;
CTextButton*	CUIGuildMgr::m_pbtnGuildDismiss=NULL;
CTextButton*	CUIGuildMgr::m_pbtnMemberRecruit=NULL;
CTextButton*	CUIGuildMgr::m_pbtnMemberRefuse=NULL;
CTextButton*	CUIGuildMgr::m_pbtnMemberKick=NULL;



CForm*	CUIGuildMgr::m_pGuildMottoNameEditForm=NULL;
CForm*	CUIGuildMgr::guildperm = NULL;
CEdit*	CUIGuildMgr::m_pedtGuildMottoName=NULL;
CTextButton*	CUIGuildMgr::m_pbtnGuildMottoFormOK=NULL;

// FEATURE: GUILD_BANK
CLabelEx* CUIGuildMgr::m_plabGuildGold = NULL;

CTextButton* CUIGuildMgr::btnperm = NULL;

// FEATURE GUILD_PERM
CImage* CUIGuildMgr::imgVideoT = NULL;
CTextButton* CUIGuildMgr::btnClosePerm = NULL;
CTextButton* CUIGuildMgr::btnYesPerm = NULL;
CTextButton* CUIGuildMgr::btnNoPerm = NULL;
CCheckBox* CUIGuildMgr::perm1 = NULL;
CCheckBox* CUIGuildMgr::perm2 = NULL;
CCheckBox* CUIGuildMgr::perm3 = NULL;
CCheckBox* CUIGuildMgr::perm4 = NULL;
CTextButton* CUIGuildMgr::btnattr1 = NULL;
CTextButton* CUIGuildMgr::btnattr2 = NULL;
CTextButton* CUIGuildMgr::btnattr3 = NULL;
CTextButton* CUIGuildMgr::btnattr4 = NULL;
CTextButton* CUIGuildMgr::btnattr5 = NULL;
CTextButton* CUIGuildMgr::btnattr6 = NULL;
CTextButton* CUIGuildMgr::btnattr7 = NULL;
CTextButton* CUIGuildMgr::btnattr8 = NULL;
CTextButton* CUIGuildMgr::btnattr9 = NULL;
CTextButton* CUIGuildMgr::btnattr10 = NULL;
CTextButton* CUIGuildMgr::btnattr11 = NULL;

CUIGuildMgr::CUIGuildMgr(void)
{
}

CUIGuildMgr::~CUIGuildMgr(void)
{
}

void CUIGuildMgr::defaultAttr()
{
	for (int i = 1; i < 12; i++)
	{
		char buf[100];
		_snprintf_s(buf, _countof(buf), _TRUNCATE, "Val%d", i);
		CLabelEx* attr = (CLabelEx*)m_pGuildMgrForm->Find(buf);
		const char* caption = attr->GetCaption();
		string szData[2];
		Util_ResolveTextLine(caption, szData, 2, '/');
		if (atoi(szData[0].c_str()) >= atoi(szData[1].c_str()))
		{
			_snprintf_s(buf, _countof(buf), _TRUNCATE, "Max%d", i);
			CLabelEx* attributeMax = (CLabelEx*)m_pGuildMgrForm->Find(buf);
			attributeMax->SetIsShow(true);
			switch (i)
			{
			case 1:
			{
				_snprintf_s(buf, _countof(buf), _TRUNCATE, "attrPRBtn");
				CTextButton* attrBtn = (CTextButton*)m_pGuildMgrForm->Find(buf);
				attrBtn->SetIsShow(false);
				break;
			}
			case 2:
			{
				_snprintf_s(buf, _countof(buf), _TRUNCATE, "attrMSBtn");
				CTextButton* attrBtn = (CTextButton*)m_pGuildMgrForm->Find(buf);
				attrBtn->SetIsShow(false);
				break;
			}
			case 3:
			{
				_snprintf_s(buf, _countof(buf), _TRUNCATE, "attrASPDBtn");
				CTextButton* attrBtn = (CTextButton*)m_pGuildMgrForm->Find(buf);
				attrBtn->SetIsShow(false);
				break;
			}
			case 4:
			{
				_snprintf_s(buf, _countof(buf), _TRUNCATE, "attrMXBtn");
				CTextButton* attrBtn = (CTextButton*)m_pGuildMgrForm->Find(buf);
				attrBtn->SetIsShow(false);
				break;
			}
			case 5:
			{
				_snprintf_s(buf, _countof(buf), _TRUNCATE, "attrDEFBtn");
				CTextButton* attrBtn = (CTextButton*)m_pGuildMgrForm->Find(buf);
				attrBtn->SetIsShow(false);
				break;
			}
			case 6:
			{
				_snprintf_s(buf, _countof(buf), _TRUNCATE, "attrHITBtn");
				CTextButton* attrBtn = (CTextButton*)m_pGuildMgrForm->Find(buf);
				attrBtn->SetIsShow(false);
				break;
			}
			case 7:
			{
				_snprintf_s(buf, _countof(buf), _TRUNCATE, "attrFLEEBtn");
				CTextButton* attrBtn = (CTextButton*)m_pGuildMgrForm->Find(buf);
				attrBtn->SetIsShow(false);
				break;
			}
			case 8:
			{
				_snprintf_s(buf, _countof(buf), _TRUNCATE, "attrHPRBtn");
				CTextButton* attrBtn = (CTextButton*)m_pGuildMgrForm->Find(buf);
				attrBtn->SetIsShow(false);
				break;
			}
			case 9:
			{
				_snprintf_s(buf, _countof(buf), _TRUNCATE, "attrSPRBtn");
				CTextButton* attrBtn = (CTextButton*)m_pGuildMgrForm->Find(buf);
				attrBtn->SetIsShow(false);
				break;
			}
			case 10:
			{
				_snprintf_s(buf, _countof(buf), _TRUNCATE, "attrHPBtn");
				CTextButton* attrBtn = (CTextButton*)m_pGuildMgrForm->Find(buf);
				attrBtn->SetIsShow(false);
				break;
			}
			case 11:
			{
				_snprintf_s(buf, _countof(buf), _TRUNCATE, "attrSPBtn");
				CTextButton* attrBtn = (CTextButton*)m_pGuildMgrForm->Find(buf);
				attrBtn->SetIsShow(false);
				break;
			}
			default:
				break;
			}
		}
		else
		{
			_snprintf_s(buf, _countof(buf), _TRUNCATE, "Max%d", i);
			CLabelEx* attributeMax = (CLabelEx*)m_pGuildMgrForm->Find(buf);
			attributeMax->SetIsShow(false);
		}
	}
}

bool CUIGuildMgr::Init()
{
	FORM_LOADING_CHECK(m_pGuildMgrForm,"manage.clu","frmManage");
	m_pGuildMgrForm->evtBeforeShow=OnBeforeShow;
	FORM_CONTROL_LOADING_CHECK(m_plabGuildName,m_pGuildMgrForm,CLabelEx,"manage.clu","labName");
	FORM_CONTROL_LOADING_CHECK(m_plabGuildMottoName,m_pGuildMgrForm,CLabelEx,"manage.clu","labMaxim");
	FORM_CONTROL_LOADING_CHECK(m_plabGuildType,m_pGuildMgrForm,CLabelEx,"manage.clu","labState");
	FORM_CONTROL_LOADING_CHECK(m_plabGuildMaster,m_pGuildMgrForm,CLabelEx,"manage.clu","labPeople");
	FORM_CONTROL_LOADING_CHECK(m_plabGuildMemberCount,m_pGuildMgrForm,CLabelEx,"manage.clu","labNum");
	FORM_CONTROL_LOADING_CHECK(m_plabGuildExperience,m_pGuildMgrForm,CLabelEx,"manage.clu","labExp");
	FORM_CONTROL_LOADING_CHECK(m_plabGuildMoney,m_pGuildMgrForm,CLabelEx,"manage.clu","labMen");
	FORM_CONTROL_LOADING_CHECK(m_plabGuildState,m_pGuildMgrForm,CLabelEx,"manage.clu","labReason");
	FORM_CONTROL_LOADING_CHECK(m_plabGuildRemainTime,m_pGuildMgrForm,CLabelEx,"manage.clu","labRemain");
	FORM_CONTROL_LOADING_CHECK(m_pbtnGuildMottoEdit,m_pGuildMgrForm,CTextButton,"manage.clu","btnMaxim");
	FORM_CONTROL_LOADING_CHECK(m_pbtnGuildDismiss,m_pGuildMgrForm,CTextButton,"manage.clu","btnSan");
	FORM_CONTROL_LOADING_CHECK(m_pbtnMemberRecruit,m_pGuildMgrForm,CTextButton,"manage.clu","btnYes");
	FORM_CONTROL_LOADING_CHECK(m_pbtnMemberRefuse,m_pGuildMgrForm,CTextButton,"manage.clu","btnNo");
	FORM_CONTROL_LOADING_CHECK(m_pbtnMemberKick,m_pGuildMgrForm,CTextButton,"manage.clu","btnkick");
	FORM_CONTROL_LOADING_CHECK(m_plstGuildMember,m_pGuildMgrForm,CListView,"manage.clu","lstNum");
	FORM_CONTROL_LOADING_CHECK(m_plstRecruitMember,m_pGuildMgrForm,CListView,"manage.clu","lstAsk");
	FORM_CONTROL_LOADING_CHECK(m_ppgeClass,m_pGuildMgrForm,CPage,"manage.clu","pgePublic");
	FORM_CONTROL_LOADING_CHECK(btnperm, m_pGuildMgrForm, CTextButton, "manage.clu", "btnperm")
	FORM_CONTROL_LOADING_CHECK(btnattr1, m_pGuildMgrForm, CTextButton, "manage.clu", "attrPRBtn")
	FORM_CONTROL_LOADING_CHECK(btnattr2, m_pGuildMgrForm, CTextButton, "manage.clu", "attrMSBtn")
	FORM_CONTROL_LOADING_CHECK(btnattr3, m_pGuildMgrForm, CTextButton, "manage.clu", "attrASPDBtn")
	FORM_CONTROL_LOADING_CHECK(btnattr4, m_pGuildMgrForm, CTextButton, "manage.clu", "attrMXBtn")
	FORM_CONTROL_LOADING_CHECK(btnattr5, m_pGuildMgrForm, CTextButton, "manage.clu", "attrDEFBtn")
	FORM_CONTROL_LOADING_CHECK(btnattr6, m_pGuildMgrForm, CTextButton, "manage.clu", "attrHITBtn")
	FORM_CONTROL_LOADING_CHECK(btnattr7, m_pGuildMgrForm, CTextButton, "manage.clu", "attrFLEEBtn")
	FORM_CONTROL_LOADING_CHECK(btnattr8, m_pGuildMgrForm, CTextButton, "manage.clu", "attrHPRBtn")
	FORM_CONTROL_LOADING_CHECK(btnattr9, m_pGuildMgrForm, CTextButton, "manage.clu", "attrSPRBtn")
	FORM_CONTROL_LOADING_CHECK(btnattr10, m_pGuildMgrForm, CTextButton, "manage.clu", "attrHPBtn")
	FORM_CONTROL_LOADING_CHECK(btnattr11, m_pGuildMgrForm, CTextButton, "manage.clu", "attrSPBtn")

	// FEATURE: GUILD_BANK
	FORM_CONTROL_LOADING_CHECK(m_plabGuildGold, m_pGuildMgrForm, CLabelEx, "manage.clu", "labGuildMoney");


	m_pbtnGuildMottoEdit->evtMouseClick=_OnClickEditMottoName;
	m_pbtnGuildDismiss->evtMouseClick=_OnClickDismiss;
	m_pbtnMemberRecruit->evtMouseClick=_OnClickRecruit;
	m_pbtnMemberRefuse->evtMouseClick=_OnClickRefuse;
	m_pbtnMemberKick->evtMouseClick=_OnClickKick;
	m_ppgeClass->evtSelectPage=_OnClickSelectPage;
	m_pbtnMemberKick->SetIsEnabled(true);
	m_pbtnMemberRecruit->SetIsEnabled(false);
	m_pbtnMemberRefuse->SetIsEnabled(false);
	m_plabGuildName->SetIsCenter(true);
	m_plabGuildMottoName->SetIsCenter(true);
	m_plabGuildType->SetIsCenter(true);
	m_plabGuildMaster->SetIsCenter(true);
	m_plabGuildMemberCount->SetIsCenter(true);
	m_plabGuildExperience->SetIsCenter(true);
	m_plabGuildMoney->SetIsCenter(true);
	m_plabGuildState->SetIsCenter(true);

	// FEATURE: GUILD_BANK
	m_plabGuildGold->SetIsCenter(true);

	FORM_LOADING_CHECK(m_pGuildMottoNameEditForm,"manage.clu","frmEditMaxim");
	FORM_CONTROL_LOADING_CHECK(m_pedtGuildMottoName,m_pGuildMottoNameEditForm,CEdit,"manage.clu","edtMaxim");
	FORM_CONTROL_LOADING_CHECK(m_pbtnGuildMottoFormOK,m_pGuildMottoNameEditForm,CTextButton,"manage.clu","btnYes");
	FORM_CONTROL_LOADING_CHECK(m_pbtnGuildMottoFormOK, m_pGuildMottoNameEditForm, CTextButton, "manage.clu", "btnYes");

	m_pbtnGuildMottoFormOK->evtMouseClick=_OnClickMottoFormOK;

	btnperm->evtMouseClick = _OnClickPerm;

	FORM_LOADING_CHECK(guildperm, "manage.clu", "frmGuildPerm");
	FORM_CONTROL_LOADING_CHECK(imgVideoT, guildperm, CImage, "manage.clu", "imgVideoT");
	FORM_CONTROL_LOADING_CHECK(btnNoPerm, guildperm, CTextButton, "manage.clu", "btnNo");
	FORM_CONTROL_LOADING_CHECK(btnYesPerm, guildperm, CTextButton, "manage.clu", "btnYesPerm");
	FORM_CONTROL_LOADING_CHECK(btnClosePerm, guildperm, CTextButton, "manage.clu", "btnClose");
	btnYesPerm->evtMouseClick = _OnClickPermOK;

	btnattr1->evtMouseClick = _OnClickAttr1;
	btnattr2->evtMouseClick = _OnClickAttr2;
	btnattr3->evtMouseClick = _OnClickAttr3;
	btnattr4->evtMouseClick = _OnClickAttr4;
	btnattr5->evtMouseClick = _OnClickAttr5;
	btnattr6->evtMouseClick = _OnClickAttr6;
	btnattr7->evtMouseClick = _OnClickAttr7;
	btnattr8->evtMouseClick = _OnClickAttr8;
	btnattr9->evtMouseClick = _OnClickAttr9;
	btnattr10->evtMouseClick = _OnClickAttr10;
	btnattr11->evtMouseClick = _OnClickAttr11;


	// FEATURE: GUILD_BANK
	grdBank = dynamic_cast<CGoodsGrid*>(m_pGuildMgrForm->Find("guildBank"));
	if (!grdBank) {
		return Error(RES_STRING(CL_LANGUAGE_MATCH_439), m_pGuildMgrForm->GetName(), "guildBank");
	}

	grdBank->evtBeforeAccept = CUIInterface::_evtDragToGoodsEvent;
	grdBank->evtSwapItem = _evtBankToBank;
	m_pGuildMgrForm->evtEntrustMouseEvent = _MainMousePlayerTradeEvent;



	return true;
}

DWORD seconds2(unsigned long mseconds)
{
	return mseconds * 1000;
}

bool CanExecuteTimer(static DWORD& lastClick, int seconds)
{
	if (lastClick == 0)
	{
		lastClick = GetTickCount();
		return true;
	}
	else
	{
		if (GetTickCount() - lastClick > seconds2(seconds))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void CUIGuildMgr::SetGuildLevel(long level)
{
	CLabel* lbLevel = dynamic_cast<CLabel*>(m_pGuildMgrForm->Find("labLevel"));

	char buf[3];

	_snprintf_s(buf, _countof(buf), "%ld", level);

	lbLevel->SetCaption(buf);
}

void CUIGuildMgr::_OnClickAttr1(CGuiData *pSender, int x, int y, DWORD key)
{ 
	static DWORD lastClick = 0;
	char buf[100];
	CGuildMemberData* pSelfData = CGuildMembersMgr::GetSelfData();
	if (pSelfData && pSelfData->IsManager())
	{
			if (CanExecuteTimer(lastClick, ATTR_BUTTON_CLICK_INTERVAL))
			{
				g_NetIF->GetProCir()->UpdateGuildAttr(ATTR_PDEF);

				lastClick = GetTickCount();

				CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDATTR, NULL);
				CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDLEVEL, NULL);

			}
			else
			{
				sprintf_s(buf, _countof(buf), "Guild Attributes\nMust wait %d seconds before clicking again!", ATTR_BUTTON_CLICK_INTERVAL);

				CBoxMgr::ShowMsgBox(NULL, buf, true);
			}
	}
	else
	{
		CBoxMgr::ShowMsgBox(NULL, "Guild Attributes\nYou do not have permission to add attributes!", true);
	}
}
void CUIGuildMgr::_OnClickAttr2(CGuiData *pSender, int x, int y, DWORD key)
{
	static DWORD lastClick = 0;
	char buf[100];
	CGuildMemberData* pSelfData = CGuildMembersMgr::GetSelfData();
	if (pSelfData && pSelfData->IsManager())
	{
			if (CanExecuteTimer(lastClick, ATTR_BUTTON_CLICK_INTERVAL))
			{
				g_NetIF->GetProCir()->UpdateGuildAttr(ATTR_MSPD);

				lastClick = GetTickCount();

				CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDATTR, NULL);
				CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDLEVEL, NULL);

			}
			else
			{
				sprintf_s(buf, _countof(buf), "Guild Attributes\nMust wait %d seconds before clicking again!", ATTR_BUTTON_CLICK_INTERVAL);
				CBoxMgr::ShowMsgBox(NULL, buf, true);
			}
	}
	else
	{
		CBoxMgr::ShowMsgBox(NULL, "Guild Attributes\nMust be Guild leader to add attributes!", true);
	}
}

void CUIGuildMgr::_OnClickAttr3(CGuiData *pSender, int x, int y, DWORD key)
{
	static DWORD lastClick = 0;
	char buf[100];
	CGuildMemberData* pSelfData = CGuildMembersMgr::GetSelfData();
	if (pSelfData && pSelfData->IsManager())
	{
		if (CanExecuteTimer(lastClick, ATTR_BUTTON_CLICK_INTERVAL))
		{
			g_NetIF->GetProCir()->UpdateGuildAttr(ATTR_ASPD);

			lastClick = GetTickCount();

			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDATTR, NULL);
			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDLEVEL, NULL);

		}
		else
		{
			sprintf_s(buf, _countof(buf), "Guild Attributes\nMust wait %d seconds before clicking again!", ATTR_BUTTON_CLICK_INTERVAL);

			CBoxMgr::ShowMsgBox(NULL, buf, true);
		}

	}
	else
	{
		CBoxMgr::ShowMsgBox(NULL, "Guild Attributes\nMust be Guild leader to add attributes!", true);
	}
}

void CUIGuildMgr::_OnClickAttr4(CGuiData *pSender, int x, int y, DWORD key)
{
	static DWORD lastClick = 0;
	char buf[100];
	CGuildMemberData* pSelfData = CGuildMembersMgr::GetSelfData();
	if (pSelfData && pSelfData->IsManager())
	{
		if (CanExecuteTimer(lastClick, ATTR_BUTTON_CLICK_INTERVAL))
		{
			g_NetIF->GetProCir()->UpdateGuildAttr(ATTR_MXATK);

			lastClick = GetTickCount();

			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDATTR, NULL);
			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDLEVEL, NULL);

		}
		else
		{
			sprintf_s(buf, _countof(buf), "Guild Attributes\nMust wait %d seconds before clicking again!", ATTR_BUTTON_CLICK_INTERVAL);

			CBoxMgr::ShowMsgBox(NULL, buf, true);
		}

	}
	else
	{
		CBoxMgr::ShowMsgBox(NULL, "Guild Attributes\nMust be Guild leader to add attributes!", true);
	}
}

void CUIGuildMgr::_OnClickAttr5(CGuiData *pSender, int x, int y, DWORD key)
{
	static DWORD lastClick = 0;
	char buf[100];
	CGuildMemberData* pSelfData = CGuildMembersMgr::GetSelfData();
	if (pSelfData && pSelfData->IsManager())
	{
		if (CanExecuteTimer(lastClick, ATTR_BUTTON_CLICK_INTERVAL))
		{
			g_NetIF->GetProCir()->UpdateGuildAttr(ATTR_DEF);

			lastClick = GetTickCount();

			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDATTR, NULL);
			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDLEVEL, NULL);

		}
		else
		{
			sprintf_s(buf, _countof(buf), "Guild Attributes\nMust wait %d seconds before clicking again!", ATTR_BUTTON_CLICK_INTERVAL);

			CBoxMgr::ShowMsgBox(NULL, buf, true);
		}

	}
	else
	{
		CBoxMgr::ShowMsgBox(NULL, "Guild Attributes\nMust be Guild leader to add attributes!", true);
	}
}

void CUIGuildMgr::_OnClickAttr6(CGuiData *pSender, int x, int y, DWORD key)
{
	static DWORD lastClick = 0;
	char buf[100];
	CGuildMemberData* pSelfData = CGuildMembersMgr::GetSelfData();
	if (pSelfData && pSelfData->IsManager())
	{
		if (CanExecuteTimer(lastClick, ATTR_BUTTON_CLICK_INTERVAL))
		{
			g_NetIF->GetProCir()->UpdateGuildAttr(ATTR_HIT);

			lastClick = GetTickCount();

			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDATTR, NULL);
			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDLEVEL, NULL);

		}
		else
		{
			sprintf_s(buf, _countof(buf), "Guild Attributes\nMust wait %d seconds before clicking again!", ATTR_BUTTON_CLICK_INTERVAL);

			CBoxMgr::ShowMsgBox(NULL, buf, true);
		}

	}
	else
	{
		CBoxMgr::ShowMsgBox(NULL, "Guild Attributes\nMust be Guild leader to add attributes!", true);
	}
}

void CUIGuildMgr::_OnClickAttr7(CGuiData *pSender, int x, int y, DWORD key)
{
	static DWORD lastClick = 0;
	char buf[100];
	CGuildMemberData* pSelfData = CGuildMembersMgr::GetSelfData();
	if (pSelfData && pSelfData->IsManager())
	{
		if (CanExecuteTimer(lastClick, ATTR_BUTTON_CLICK_INTERVAL))
		{
			g_NetIF->GetProCir()->UpdateGuildAttr(ATTR_FLEE);

			lastClick = GetTickCount();

			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDATTR, NULL);
			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDLEVEL, NULL);

		}
		else
		{
			sprintf_s(buf, _countof(buf), "Guild Attributes\nMust wait %d seconds before clicking again!", ATTR_BUTTON_CLICK_INTERVAL);

			CBoxMgr::ShowMsgBox(NULL, buf, true);
		}

	}
	else
	{
		CBoxMgr::ShowMsgBox(NULL, "Guild Attributes\nMust be Guild leader to add attributes!", true);
	}
}
void CUIGuildMgr::_OnClickAttr8(CGuiData *pSender, int x, int y, DWORD key)
{
	static DWORD lastClick = 0;
	char buf[100];
	CGuildMemberData* pSelfData = CGuildMembersMgr::GetSelfData();
	if (pSelfData && pSelfData->IsManager())
	{
		if (CanExecuteTimer(lastClick, ATTR_BUTTON_CLICK_INTERVAL))
		{
			g_NetIF->GetProCir()->UpdateGuildAttr(ATTR_HREC);

			lastClick = GetTickCount();

			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDATTR, NULL);
			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDLEVEL, NULL);

		}
		else
		{
			sprintf_s(buf, _countof(buf), "Guild Attributes\nMust wait %d seconds before clicking again!", ATTR_BUTTON_CLICK_INTERVAL);

			CBoxMgr::ShowMsgBox(NULL, buf, true);
		}

	}
	else
	{
		CBoxMgr::ShowMsgBox(NULL, "Guild Attributes\nMust be Guild leader to add attributes!", true);
	}
}

void CUIGuildMgr::_OnClickAttr9(CGuiData *pSender, int x, int y, DWORD key)
{
	static DWORD lastClick = 0;
	char buf[100];
	CGuildMemberData* pSelfData = CGuildMembersMgr::GetSelfData();
	if (pSelfData && pSelfData->IsManager())
	{
		if (CanExecuteTimer(lastClick, ATTR_BUTTON_CLICK_INTERVAL))
		{
			g_NetIF->GetProCir()->UpdateGuildAttr(ATTR_SREC);

			lastClick = GetTickCount();

			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDATTR, NULL);
			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDLEVEL, NULL);

		}
		else
		{
			sprintf_s(buf, _countof(buf), "Guild Attributes\nMust wait %d seconds before clicking again!", ATTR_BUTTON_CLICK_INTERVAL);

			CBoxMgr::ShowMsgBox(NULL, buf, true);
		}

	}
	else
	{
		CBoxMgr::ShowMsgBox(NULL, "Guild Attributes\nMust be Guild leader to add attributes!", true);
	}
}

void CUIGuildMgr::_OnClickAttr10(CGuiData *pSender, int x, int y, DWORD key)
{
	static DWORD lastClick = 0;
	char buf[100];
	CGuildMemberData* pSelfData = CGuildMembersMgr::GetSelfData();
	if (pSelfData && pSelfData->IsManager())
	{
		if (CanExecuteTimer(lastClick, ATTR_BUTTON_CLICK_INTERVAL))
		{
			g_NetIF->GetProCir()->UpdateGuildAttr(ATTR_MXHP);

			lastClick = GetTickCount();

			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDATTR, NULL);
			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDLEVEL, NULL);

		}
		else
		{
			sprintf_s(buf, _countof(buf), "Guild Attributes\nMust wait %d seconds before clicking again!", ATTR_BUTTON_CLICK_INTERVAL);

			CBoxMgr::ShowMsgBox(NULL, buf, true);
		}

	}
	else
	{
		CBoxMgr::ShowMsgBox(NULL, "Guild Attributes\nMust be Guild leader to add attributes!", true);
	}
}

void CUIGuildMgr::_OnClickAttr11(CGuiData *pSender, int x, int y, DWORD key)
{
	static DWORD lastClick = 0;
	char buf[100];
	CGuildMemberData* pSelfData = CGuildMembersMgr::GetSelfData();
	if (pSelfData && pSelfData->IsManager())
	{
		if (CanExecuteTimer(lastClick, ATTR_BUTTON_CLICK_INTERVAL))
		{
			g_NetIF->GetProCir()->UpdateGuildAttr(ATTR_MXSP);

			lastClick = GetTickCount();

			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDATTR, NULL);
			CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDLEVEL, NULL);

		}
		else
		{
			sprintf_s(buf, _countof(buf), "Guild Attributes\nMust wait %d seconds before clicking again!", ATTR_BUTTON_CLICK_INTERVAL);

			CBoxMgr::ShowMsgBox(NULL, buf, true);
		}

	}
	else
	{
		CBoxMgr::ShowMsgBox(NULL, "Guild Attributes\nMust be Guild leader to add attributes!", true);
	}
}

void CUIGuildMgr::ShowForm()
{
	
	// FEATURE: GUILD_BANK
	CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDBANK, NULL);
	CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDATTR, NULL);
	CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDLEVEL, NULL);
	RefreshForm();
	m_pGuildMgrForm->nTag = 1;		// nTag==0时用于快捷键不显示
	m_pGuildMgrForm->Show();
}

void CUIGuildMgr::_OnClickEditMottoName(CGuiData *pSender, int x, int y, DWORD key)
{
	CGuildMemberData* pSelfData=CGuildMembersMgr::GetSelfData();
	if ( pSelfData && pSelfData->GetPerm() >= emGldPermMotto)
	{
		m_pedtGuildMottoName->SetCaption(CGuildData::GetGuildMottoName().c_str());
		m_pGuildMottoNameEditForm->ShowModal();
	}
	else
	{
		CBoxMgr::ShowMsgBox( NULL, RES_STRING(CL_LANGUAGE_MATCH_594), true );
	}
}


void CUIGuildMgr::GetAttrCost(int cost)
{
	char buf[50];
	for (int i = 1; i < 12; i++)
	{
		_snprintf_s(buf, _countof(buf), _TRUNCATE, "Val%d", i);
		CLabelEx* attr = (CLabelEx*)m_pGuildMgrForm->Find(buf);
		const char* caption = attr->GetCaption();
		string szData[2];
		Util_ResolveTextLine(caption, szData, 2, '/');

	}
}

void CUIGuildMgr::ResetGuildPermissions()
{
	char buf[100];
	for (int i = 1; i < 9; i++)
	{
		_snprintf_s(buf, _countof(buf), _TRUNCATE, "perm%d", i);
		CCheckBox* perm = (CCheckBox *)guildperm->Find(buf);
		perm->SetIsChecked(false);
	}
}

void CUIGuildMgr::_OnClickPerm(CGuiData *pSender, int x, int y, DWORD key)
{
	CGuildMemberData* pSelfData = CGuildMembersMgr::GetSelfData();
	ResetGuildPermissions();
	//char message[1024];
	//_snprintf_s(message, _countof(message), _TRUNCATE, "Permission : %d , Manager : %d", pSelfData->GetPerm(), pSelfData->IsManager());
	//CBoxMgr::ShowMsgBox(NULL, message, true);
	if (pSelfData && pSelfData->IsManager())
	{
		CItemRow *pRow = m_plstGuildMember->GetList()->GetSelectItem();
		if (!pRow)
		{
			CBoxMgr::ShowMsgBox(NULL, "Please select a player in the list!", true);
			return;
		}
		CGuildMemberData* pMemberData = static_cast<CGuildMemberData*>(pRow->GetPointer());

		if (pMemberData && pMemberData->GetName() != pSelfData->GetName() && pMemberData->IsManager() != true)
		{
			guildperm->ShowModal();
			int permission = pMemberData->GetPerm();
			CCheckBox* perm = NULL;
			switch (permission)
			{
			case emGldPermSpeak:
			{
				perm = (CCheckBox *)guildperm->Find("perm1");
				perm->SetIsChecked(true);
			}
				break;
			case emGldPermMgr:
			{
				perm = (CCheckBox *)guildperm->Find("perm2");
				perm->SetIsChecked(true);
			}
				break;
			case emGldPermViewBank: {
				perm = (CCheckBox *)guildperm->Find("perm3");
				perm->SetIsChecked(true);
			}
				break;
			case emGldPermDepoBank:
			{
				perm = (CCheckBox *)guildperm->Find("perm4");
				perm->SetIsChecked(true);
			}
			break;
			case emGldPermTakeBank:
			{
				perm = (CCheckBox *)guildperm->Find("perm5");
				perm->SetIsChecked(true);
			}
				break;
			case emGldPermRecruit:
			{
				perm = (CCheckBox *)guildperm->Find("perm6");
				perm->SetIsChecked(true);
			}
			break;
			case emGldPermKick:
			{
				perm = (CCheckBox *)guildperm->Find("perm7");
				perm->SetIsChecked(true);
			}
			break;
			case emGldPermMotto:
			{
				perm = (CCheckBox *)guildperm->Find("perm8");
				perm->SetIsChecked(true);
			}
			break;
			}
		}
	}
}

void CUIGuildMgr::AddGuildAttrs(DWORD attrs[11])
{
	char buf[100];
	char attrData[50];
	
	for (int i = 1; i < 12; i++)
	{
		_snprintf_s(buf, _countof(buf), _TRUNCATE, "Val%d", i);
		CLabelEx* attr = (CLabelEx*)m_pGuildMgrForm->Find(buf);
		const char* caption = attr->GetCaption();
		string szData[2];
		Util_ResolveTextLine(caption, szData, 2, '/');
		_snprintf_s(attrData, _countof(attrData), _TRUNCATE, "%d/%s", attrs[i], szData[1].c_str());
		attr->SetCaption(attrData);
	}
	for (int i = 1; i < 12; i++)
	{
		_snprintf_s(buf, _countof(buf), _TRUNCATE, "Total%d", i);
		CLabelEx* attra = (CLabelEx*)m_pGuildMgrForm->Find(buf);
		int total = 0;

		if (i == 1)
			total = attrs[1];
		else if (i > 1 && i < 8)
			total = attrs[i] * 5;
		else if (i >= 8 && i <= 9)
			total = attrs[i];
		else if (i > 9 && i < 12)
			total = attrs[i] * 50;

		_snprintf_s(attrData, _countof(attrData), _TRUNCATE, "+%d", total);
		attra->SetCaption(attrData);
	}
	CS_BeginAction(g_stUIBoat.GetHuman(), enumACTION_REQUESTGUILDLEVEL, NULL);

}

void CUIGuildMgr::_OnClickPermOK(CGuiData *pSender, int x, int y, DWORD key)
{
	CGuildMemberData* pSelfData = CGuildMembersMgr::GetSelfData();
	if (pSelfData && pSelfData->IsManager())
	{
		CItemRow *pRow = m_plstGuildMember->GetList()->GetSelectItem();
		if (!pRow)
		{
			CBoxMgr::ShowMsgBox(NULL, RES_STRING(CL_LANGUAGE_MATCH_597), true);
			return;
		}
		CGuildMemberData* pMemberData = static_cast<CGuildMemberData*>(pRow->GetPointer());
		if (pMemberData)
		{
			int characterId = pMemberData->GetID();
			char buf[100];
			for (int i = 1; i < 9; i++)
			{
				_snprintf_s(buf, _countof(buf), _TRUNCATE, "perm%d", i);
			     CCheckBox* perm = (CCheckBox *)guildperm->Find(buf);
				 if (perm->GetIsChecked())
				 {
					 
					 if(i == 1){
						 g_NetIF->GetProCir()->UpdateGuildPerm(characterId, emGldPermSpeak);
						 pMemberData->SetPerm(emGldPermSpeak);
					 }
					 else if(i == 2)
					 {
						 g_NetIF->GetProCir()->UpdateGuildPerm(characterId, emGldPermMgr);
						 pMemberData->SetPerm(emGldPermMgr);
					 }
					 else if (i == 3) {
						 g_NetIF->GetProCir()->UpdateGuildPerm(characterId, emGldPermViewBank);
						 pMemberData->SetPerm(emGldPermViewBank);

					 }
					 else if (i == 4)
					 {
						 g_NetIF->GetProCir()->UpdateGuildPerm(characterId, emGldPermDepoBank);
						 pMemberData->SetPerm(emGldPermDepoBank);

					 }
					 else if (i == 5)
					 {
						 g_NetIF->GetProCir()->UpdateGuildPerm(characterId, emGldPermTakeBank);
						 pMemberData->SetPerm(emGldPermTakeBank);

					 }
					 else if (i == 6)
					 {
						 g_NetIF->GetProCir()->UpdateGuildPerm(characterId, emGldPermRecruit);
						 pMemberData->SetPerm(emGldPermRecruit);
					 }
					 else if (i == 7)
					 {
						 g_NetIF->GetProCir()->UpdateGuildPerm(characterId, emGldPermKick);
						 pMemberData->SetPerm(emGldPermKick);

					 }
					 else if (i == 8)
					 {
						 g_NetIF->GetProCir()->UpdateGuildPerm(characterId, emGldPermMotto);
						 pMemberData->SetPerm(emGldPermMotto);
					 }
					 else{
						 
					 }
				 }
			}
		}
	}
}

void CUIGuildMgr::_OnClickDismiss(CGuiData *pSender, int x, int y, DWORD key)
{
	CGuildMemberData* pSelfData=CGuildMembersMgr::GetSelfData();
	if ( pSelfData && pSelfData->IsManager())
	{
		CBoxMgr::ShowPasswordBox(_OnPassDismiss, RES_STRING(CL_LANGUAGE_MATCH_595));
	}
	else
	{
		CBoxMgr::ShowSelectBox( _OnClickLeave, RES_STRING(CL_LANGUAGE_MATCH_596), true );
	}
}

void CUIGuildMgr::_OnClickRecruit(CGuiData *pSender, int x, int y, DWORD key)
{
	CGuildMemberData* pSelfData=CGuildMembersMgr::GetSelfData();
	if ( pSelfData && pSelfData->IsManager())
	{
		CItemRow *pRow=m_plstRecruitMember->GetList()->GetSelectItem();
		if (!pRow)
		{
			CBoxMgr::ShowMsgBox( NULL, RES_STRING(CL_LANGUAGE_MATCH_597), true );
			return;
		}
		CRecruitMemberData* pMemberData=static_cast<CRecruitMemberData*>(pRow->GetPointer());
		CM_GUILD_APPROVE(pMemberData->GetID());
		m_plstRecruitMember->GetList()->Del(pRow);
		CRecruitMembersMgr::DelRecruitMember(pMemberData);
	}
	else
	{
		CBoxMgr::ShowMsgBox( NULL, RES_STRING(CL_LANGUAGE_MATCH_598), true );
	}
}

void CUIGuildMgr::_OnClickRefuse(CGuiData *pSender, int x, int y, DWORD key)
{
	CGuildMemberData* pSelfData=CGuildMembersMgr::GetSelfData();
	if ( pSelfData && pSelfData->IsManager())
	{
		CItemRow *pRow=m_plstRecruitMember->GetList()->GetSelectItem();
		if (!pRow)
		{
			CBoxMgr::ShowMsgBox( NULL, RES_STRING(CL_LANGUAGE_MATCH_597), true );
			return;
		}
		CRecruitMemberData* pMemberData=static_cast<CRecruitMemberData*>(pRow->GetPointer());
		CM_GUILD_REJECT(pMemberData->GetID());
		m_plstRecruitMember->GetList()->Del(pRow);
		CRecruitMembersMgr::DelRecruitMember(pMemberData);
	}
	else
	{
		CBoxMgr::ShowMsgBox( NULL, RES_STRING(CL_LANGUAGE_MATCH_599), true );
	}
}

void CUIGuildMgr::_OnClickKick(CGuiData *pSender, int x, int y, DWORD key)
{
	CGuildMemberData* pSelfData=CGuildMembersMgr::GetSelfData();
	if ( pSelfData && pSelfData->IsManager())
	{
		CItemRow *pRow=m_plstGuildMember->GetList()->GetSelectItem();
		if (!pRow)
		{
			CBoxMgr::ShowMsgBox( NULL, RES_STRING(CL_LANGUAGE_MATCH_600), true );
			return;
		}
		CGuildMemberData* pMemberData=static_cast<CGuildMemberData*>(pRow->GetPointer());
		string str=RES_STRING(CL_LANGUAGE_MATCH_601)+pMemberData->GetName()+RES_STRING(CL_LANGUAGE_MATCH_602);
		CBoxMgr::ShowSelectBox(_OnPassKick,str.c_str(),true);
	}
	else
	{
		CBoxMgr::ShowMsgBox( NULL, RES_STRING(CL_LANGUAGE_MATCH_603), true );
	}
}

void CUIGuildMgr::_OnClickMottoFormOK(CGuiData *pSender, int x, int y, DWORD key)
{
	string name=m_pedtGuildMottoName->GetCaption();
	if (!CTextFilter::IsLegalText(CTextFilter::NAME_TABLE, name)
		|| !IsValidName(name.c_str(), (unsigned short)name.length())
		)
	{
		g_pGameApp->MsgBox(RES_STRING(CL_LANGUAGE_MATCH_51));
		return;
	}
	CM_GUILD_MOTTO(name.c_str());
	m_pGuildMottoNameEditForm->Hide();
}

void CUIGuildMgr::RefreshForm()
{
	RefreshAttribute();
	RefreshList();
}

void CUIGuildMgr::RefreshAttribute()
{
	char buf[50];
	m_plabGuildName->SetCaption(CGuildData::GetGuildName().c_str());
	m_plabGuildMaster->SetCaption(CGuildData::GetGuildMasterName().c_str());
	m_plabGuildMottoName->SetCaption(CGuildData::GetGuildMottoName().c_str());
	m_plabGuildType->SetCaption((CGuildData::GetGuildType()==CGuildData::navy)?RES_STRING(CL_LANGUAGE_MATCH_604):RES_STRING(CL_LANGUAGE_MATCH_605));
	_snprintf_s( buf, _countof( buf ), _TRUNCATE, "%d/%d",CGuildData::GetMemberCount(),CGuildData::GetMaxMembers());
	m_plabGuildMemberCount->SetCaption(buf);
	_i64toa_s(CGuildData::GetExperence(),buf,sizeof(buf) ,10);
	m_plabGuildExperience->SetCaption(buf);
	_i64toa_s(CGuildData::GetMoney(),buf,sizeof(buf) ,10);
	m_plabGuildMoney->SetCaption(buf);
	CGuildData::eState state=CGuildData::GetGuildState();
	string strState;
	if (state==CGuildData::normal)
	{
		strState=RES_STRING(CL_LANGUAGE_MATCH_606);
		m_plabGuildState->SetCaption(strState.c_str());
		m_plabGuildRemainTime->SetCaption("");
	}
	else
	{
		strState=RES_STRING(CL_LANGUAGE_MATCH_607);
		if (state&CGuildData::money)
		{
			strState+=RES_STRING(CL_LANGUAGE_MATCH_608);
		}
		if (state&CGuildData::repute)
		{
			strState+=RES_STRING(CL_LANGUAGE_MATCH_609);
		}
		if (state&CGuildData::member)
		{
			strState+=RES_STRING(CL_LANGUAGE_MATCH_610);
		}
		m_plabGuildState->SetCaption(strState.c_str());
		__int64 remain=CGuildData::GetRemainTime();
		if (remain>1440)
		{
			_snprintf_s( buf, _countof( buf ), _TRUNCATE, RES_STRING(CL_LANGUAGE_MATCH_611),remain/1440);
		}
		else if (remain>60)
		{
			_snprintf_s( buf, _countof( buf ), _TRUNCATE, RES_STRING(CL_LANGUAGE_MATCH_612),remain/60);
		}
		else
		{
			_snprintf_s( buf, _countof( buf ), _TRUNCATE, RES_STRING(CL_LANGUAGE_MATCH_613),remain);
		}
		m_plabGuildRemainTime->SetCaption(buf);
	}

	CGuildMemberData* pSelfData=CGuildMembersMgr::GetSelfData();
	//	Modify by alfred
	if ( pSelfData && pSelfData->IsManager())
	{
		m_pbtnGuildDismiss->GetImage()->LoadImage("texture/ui2009/guild4.tga",43,22,0,92,150);
		m_pbtnGuildDismiss->GetImage()->LoadImage("texture/ui2009/guild4.tga",43,22,1,92,194);
	}
	else
	{
		m_pbtnGuildDismiss->GetImage()->LoadImage("texture/ui2009/guild4.tga",43,22,0,135,62);
		m_pbtnGuildDismiss->GetImage()->LoadImage("texture/ui2009/guild4.tga",43,22,1,135,106);
	}
	m_pGuildMgrForm->Refresh();
}

void CUIGuildMgr::RefreshList()
{
	char buf[50];
	m_plstGuildMember->GetList()->GetItems()->Clear();
	for (DWORD i=0;i<CGuildMembersMgr::GetTotalGuildMembers();i++)
	{
		CGuildMemberData* pMemberData=CGuildMembersMgr::FindGuildMemberByIndex(i);
		if (!pMemberData) continue;
		CItemRow *pRow=m_plstGuildMember->GetList()->NewItem();
		CItem*	pMemberNameItem=new CItem(pMemberData->GetName().c_str(),COLOR_BLACK);
		CItem*	pMemberJobItem=new CItem(pMemberData->GetJob().c_str(),COLOR_BLACK);
		//CItem*	pMemberLevelItem=new CItem(_i64toa(pMemberData->GetLevel(),buf,10),COLOR_BLACK);
		_i64toa_s(pMemberData->GetLevel(),buf,sizeof(buf) ,10);
		CItem*	pMemberLevelItem=new CItem(buf,COLOR_BLACK);
		pRow->SetIndex(0,pMemberNameItem);
		pRow->SetIndex(1,pMemberJobItem);
		pRow->SetIndex(2,pMemberLevelItem);
		pRow->SetPointer(pMemberData);
	}

	m_plstRecruitMember->GetList()->GetItems()->Clear();
	for (DWORD i=0;i<CRecruitMembersMgr::GetTotalRecruitMembers();i++)
	{
		CRecruitMemberData* pMemberData=CRecruitMembersMgr::FindRecruitMemberByIndex(i);
		if (!pMemberData) continue;
		CItemRow *pRow=m_plstRecruitMember->GetList()->NewItem();
		CItem*	pMemberNameItem=new CItem(pMemberData->GetName().c_str(),COLOR_BLACK);
		CItem*	pMemberJobItem=new CItem(pMemberData->GetJob().c_str(),COLOR_BLACK);
		//CItem*	pMemberLevelItem=new CItem(_i64toa(pMemberData->GetLevel(),buf,10),COLOR_BLACK);
		_i64toa_s(pMemberData->GetLevel(),buf,sizeof(buf) ,10);
		CItem*	pMemberLevelItem=new CItem(buf,COLOR_BLACK);
		pRow->SetIndex(0,pMemberNameItem);
		pRow->SetIndex(1,pMemberJobItem);
		pRow->SetIndex(2,pMemberLevelItem);
		pRow->SetPointer(pMemberData);
	}
	m_pGuildMgrForm->Refresh();
}

void CUIGuildMgr::_OnClickSelectPage(CGuiData *pSender)
{
	int n=m_ppgeClass->GetIndex();
	if (n==0)	//成员列表
	{
		m_pbtnMemberKick->SetIsEnabled(true);
		m_pbtnMemberRecruit->SetIsEnabled(false);
		m_pbtnMemberRefuse->SetIsEnabled(false);
	}
	else if (n==1)	//申请人列表
	{
		m_pbtnMemberRecruit->SetIsEnabled(true);
		m_pbtnMemberRefuse->SetIsEnabled(true);
		m_pbtnMemberKick->SetIsEnabled(false);
	}
}

void CUIGuildMgr::RemoveForm()
{
	m_pGuildMgrForm->Close();
}

void CUIGuildMgr::_OnPassDismiss(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
{
	if( nMsgType!=CForm::mrYes ) return;
	stPasswordBox* pBox = (stPasswordBox*)pSender->GetForm()->GetPointer();
	if( !pBox ) return;
	string str=pBox->edtPassword->GetCaption();
	if (str.length()>0)
	{
		CM_GUILD_DISBAND(str.c_str());
	}
}

void CUIGuildMgr::_OnClickLeave(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
{
	if( nMsgType==CForm::mrYes )
	{
		CM_GUILD_LEAVE();
	}
}

void CUIGuildMgr::_OnPassKick(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
{
	if( nMsgType!=CForm::mrYes ) return;
	CItemRow *pRow=m_plstGuildMember->GetList()->GetSelectItem();
	if (!pRow)
	{
		CBoxMgr::ShowMsgBox( NULL, RES_STRING(CL_LANGUAGE_MATCH_614), true );
		return;
	}
	CGuildMemberData* pMemberData=static_cast<CGuildMemberData*>(pRow->GetPointer());
	CM_GUILD_KICK(pMemberData->GetID());
}

void CUIGuildMgr::OnBeforeShow(CForm* pForm,bool& IsShow)
{
	if( !pForm->nTag )
	{
		CM_GUILD_LISTTRYPLAYER();
		IsShow=false;
	}
	pForm->nTag=0;
}


// FEATURE: GUILD_BANK
bool CUIGuildMgr::PushToBank(CGoodsGrid& rkDrag, CGoodsGrid& rkSelf, int nGridID, CCommandObj& rkItem) {
	m_kNetBank.chSrcType = 0;
	m_kNetBank.sSrcID = rkDrag.GetDragIndex();
	m_kNetBank.chTarType = 1;
	m_kNetBank.sTarID = nGridID;

	CItemCommand* pkItemCmd = dynamic_cast<CItemCommand*>(&rkItem);
	if (!pkItemCmd) {
		return false;
	}

	CItemRecord* pkItemRecord = pkItemCmd->GetItemInfo();
	if (!pkItemRecord) {
		return false;
	}

	if (pkItemCmd->GetItemInfo()->GetIsPile() && pkItemCmd->GetTotalNum() > 1) {
		m_pkNumberBox = g_stUIBox.ShowNumberBox(_MoveItemsEvent, pkItemCmd->GetTotalNum(), RES_STRING(CL_LANGUAGE_MATCH_441), false);
		if (m_pkNumberBox->GetNumber() < pkItemCmd->GetTotalNum()) {
			return false;
		}
		else {
			return true;
		}
	}
	else {
		g_stUIGuildMgr.m_kNetBank.sSrcNum = 1;
		g_NetIF->GetProCir()->UpdateGuildBank(g_stUIBoat.GetHuman()->getHumanID(), &(g_stUIGuildMgr.m_kNetBank));
		return true;
	}
}

bool CUIGuildMgr::PopFromBank(CGoodsGrid& rkDrag, CGoodsGrid& rkSelf, int nGridID, CCommandObj& rkItem) {
	m_kNetBank.chSrcType = 1;
	m_kNetBank.sSrcID = rkDrag.GetDragIndex();
	m_kNetBank.chTarType = 0;
	m_kNetBank.sTarID = nGridID;
	CItemCommand* pkItemCmd = dynamic_cast<CItemCommand*>(&rkItem);
	if (!pkItemCmd) { return false; }

	if (pkItemCmd->GetItemInfo()->GetIsPile() && pkItemCmd->GetTotalNum() > 1) {
		m_pkNumberBox =
			g_stUIBox.ShowNumberBox(_MoveItemsEvent, pkItemCmd->GetTotalNum(), RES_STRING(CL_LANGUAGE_MATCH_442), false);

		if (m_pkNumberBox->GetNumber() < pkItemCmd->GetTotalNum()) {
			return false;
		}
		else {
			return true;
		}
	}
	else {
		g_stUIGuildMgr.m_kNetBank.sSrcNum = 1;
		g_NetIF->GetProCir()->UpdateGuildBank(g_stUIBoat.GetHuman()->getHumanID(), &(g_stUIGuildMgr.m_kNetBank));
		return true;
	}
}

void CUIGuildMgr::_MoveItemsEvent(CCompent* pSender, int nMsgType, int x, int y, DWORD dwKey) {
	if (nMsgType != CForm::mrYes) {
		return;
	}

	int num = g_stUIGuildMgr.m_pkNumberBox->GetNumber();
	if (num > 0) {
		g_stUIGuildMgr.m_kNetBank.sSrcNum = num;
		g_NetIF->GetProCir()->UpdateGuildBank(g_stUIBoat.GetHuman()->getHumanID(), &(g_stUIGuildMgr.m_kNetBank));
	}
}

void CUIGuildMgr::_MoveAItemEvent(CCompent* pSender, int nMsgType, int x, int y, DWORD dwKey) {
	if (nMsgType != CForm::mrYes) {
		return;
	}

	g_stUIGuildMgr.m_kNetBank.sSrcNum = 1;
	g_NetIF->GetProCir()->UpdateGuildBank(g_stUIBoat.GetHuman()->getHumanID(), &(g_stUIGuildMgr.m_kNetBank));
}

void CUIGuildMgr::_evtBankToBank(CGuiData* pSender, int nFirst, int nSecond, bool& isSwap) {
	isSwap = false;

	if (!g_stUIBoat.GetHuman()) {
		return;
	}

	g_stUIGuildMgr.m_kNetBank.chSrcType = 1;
	g_stUIGuildMgr.m_kNetBank.sSrcID = nSecond;
	g_stUIGuildMgr.m_kNetBank.sSrcNum = 0;
	g_stUIGuildMgr.m_kNetBank.chTarType = 1;
	g_stUIGuildMgr.m_kNetBank.sTarID = nFirst;

	g_NetIF->GetProCir()->UpdateGuildBank(g_stUIBoat.GetHuman()->getHumanID(), &(g_stUIGuildMgr.m_kNetBank));
}

void CUIGuildMgr::_MainMousePlayerTradeEvent(CCompent* pSender, int nMsgType, int x, int y, DWORD dwKey) {
	string name = pSender->GetName();
	if (name == "btngoldput") {
		g_stUIBox.ShowNumberBox(_evtGoldFormDepositEvent, (int)g_stUIBoat.GetHuman()->getGameAttr()->get(ATTR_GD), RES_STRING(CL_LANGUAGE_MATCH_781), false);
	}
	else if (name == "btngoldtake") {
		g_stUIBox.ShowNumberBox(_evtGoldFormWithdrawEvent, stoi(m_plabGuildGold->GetCaption()), "withdraw", false);
	}
}

void CUIGuildMgr::_evtGoldFormDepositEvent(CCompent* pSender, int nMsgType, int x, int y, DWORD dwKey) {

	if (nMsgType != CForm::mrYes) {
		return;
	}

	stNumBox* pBox = (stNumBox*)pSender->GetForm()->GetPointer();
	if (!pBox) {
		return;
	}

	int amount = pBox->GetNumber();

	if (amount > 0) {
		g_NetIF->GetProCir()->UpdateGuildBankGold(g_stUIBoat.GetHuman()->getHumanID(), 1, amount);

	}
}

void CUIGuildMgr::_evtGoldFormWithdrawEvent(CCompent* pSender, int nMsgType, int x, int y, DWORD dwKey) {

	if (nMsgType != CForm::mrYes) {
		return;
	}

	stNumBox* pBox = (stNumBox*)pSender->GetForm()->GetPointer();
	if (!pBox) {
		return;
	}

	int amount = pBox->GetNumber();

	if (amount > 0) {
		g_NetIF->GetProCir()->UpdateGuildBankGold(g_stUIBoat.GetHuman()->getHumanID(), 0, amount);

	}
}

