#include "StdAfx.h"
#include "UIGlobalVar.h"
#include "uitextparse.h"
#include "uiEditor.h"
#include "UIFormMgr.h"
#include "uiheadsay.h"
#include "UITeam.h"
#include "uitreeview.h"
#include "UIChat.h"
#include "uiboxform.h"
#include "uitradeform.h"
#include "uiequipform.h"
#include "uiminimapform.h"
#include "uisystemform.h"
#include "uimissionform.h"
#include "UIMisLogForm.h"
#include "uiforgeform.h"
#include "uinpctradeform.h"
#include "uistartform.h"
#include "uistateform.h"
#include "uiCozeform.h"
#include "uifastcommand.h"
#include "gameapp.h"
#include "scene.h"
#include "uigoodsgrid.h"
#include "uinpctalkform.h"
#include "worldscene.h"
#include "uiguildlist.h"
#include "UIGuildMgr.h"
#include "uiguildapply.h"
#include "uiboatform.h"
#include "uibourseform.h"
#include "UIBankForm.h"
#include "UIBoothForm.h"
#include "UIHaircutForm.h"
#include "UIPKDialog.h"
#include "UIMakeEquipForm.h"
#include "UIGuildChallengeForm.h"
#include "UIDoublePwdForm.h"
#include "UIStoreForm.h"
#include "uiCozeform.h"
#include "UISpiritForm.h"
#include "UIPurifyForm.h"
#include "UIPKSilverForm.h"
#include "UIFindTeamForm.h"
#include "UIComposeForm.h"
#include "UIBreakForm.h"
#include "UIFoundForm.h"
#include "UICookingForm.h"
#include "UIMailForm.h"
#include "UINumAnswer.h"
#include "UIChurchChallenge.h"
#include "UIPortalTime.h"
#include "UILotteryForm.h"	//Add by lark.li 20080514
#include "UIAmphitheaterForm.h" //Add by sunny.sun20080716

#include "UIChaInfo.h"
#include "UIAllButtonForm.h"
#include "UIChaInfo.h"
#include "uibakghostform.h"
#include "uiitemreform.h"
#include "uiforgemoveform.h"
#include "uijewelryupform.h"
#include "uirumdealform.h"
#include "uirumbuyform.h"
#include "uirumsellform.h"
#include "uirumpurform.h"
#include "uirumsaleform.h"
#include "uirumgetform.h"
#include "uihelpinfoform.h"
#include "GameErrorReport.h"

namespace GUI
{
	CTextParse		g_TextParse;
}
using namespace GUI;

extern void UI_ReleaseItemScript();
bool UIMainInit( CFormMgr* pSender )
{
    if( !CHeadSay::Init() )
    {
        LG( "gui", "CHeadSay::Init failed" );
        return false;
    }

	return CUIInterface::All_Init();
}

bool UIClear()
{
    if( !CHeadSay::Clear() )
    {
        LG( "gui", "CHeadSay::Clear failed" );
        return false;
    }

    //°²È«ÊÍ·ÅÄÚ´æ by Waiting 2009-06-18
	CGuiTime::RemoveAllTimer();
	CItemBar::Clear();
	UI_ReleaseItemScript();
    return true;
}

//---------------------------------------------------------------------------
// calss CUIInterface
//---------------------------------------------------------------------------
CUIInterface::allmgr CUIInterface::all(0);

#ifdef __EDITOR__
CEditor			g_stUIEditor;
#endif
CChat           g_stUIChat;
CBoxMgr			g_stUIBox;
CTradeMgr		g_stUITrade;
CEquipMgr		g_stUIEquip;
CMiniMapMgr		g_stUIMap;
CSystemMgr		g_stUISystem;
CMissionMgr		g_stUIMission;
CMisLogForm		g_stUIMisLog;
CForgeMgr		g_stUIForge;
CPortalTime g_stUIPortalTime;
CLotteryMgr		g_stUILottery;	//Add by lark.li 20080514
CAmphitheaterForm g_stUIAmphitheater;//Add by sunny.sun20080715

CNpcTradeMgr	g_stUINpcTrade;
CStartMgr		g_stUIStart;
CStateMgr		g_stUIState;
CNpcTalkMgr		g_stUINpcTalk;
CUIGuildList	g_stUIGuildList;
CUIGuildMgr		g_stUIGuildMgr;
CUIGuildApply	g_stUIGuildApply;
CBoatMgr		g_stUIBoat;
CBourseMgr		g_stUIBourse;
CBankMgr		g_stUIBank;
CBoothMgr		g_stUIBooth;
CHaircutMgr		g_stUIHaircut;
CPkDialog		g_stUIPKDialog;
CMakeEquipMgr	g_stUIMakeEquip;
CGuildChallengeMgr g_stGuildChallenge;
CDoublePwdMgr	g_stUIDoublePwd;
CStoreMgr		g_stUIStore;
CSpiritMgr		g_stUISpirit;
CPurifyMgr		g_stUIPurify;
CBlackTradeMgr  g_stUIBlackTrade;
CPKSilverMgr    g_stUIPKSilver;
CFindTeamMgr	g_stUIFindTeam;
CComposeMgr     g_stUICompose;
CBreakMgr       g_stUIBreak;
CFoundMgr       g_stUIFound;
CCookingMgr     g_stUICooking;
CMailMgr		g_stUIMail;
CNumAnswerMgr	g_stUINumAnswer;
CChurchChallengeMgr g_stChurchChallenge;

CCozeForm          g_stUICozeForm;
CChannelSwitchForm g_stUIChannelSwitch;

AllButtonMgr	g_stAllButton;
 CBakGhostMgr	g_stBakGhostForm;
 CItemReMgr		g_stItemReForm;
CForgeMoveMgr	g_stForgeMoveForm;
CJewelryUpMgr	g_stJewelryUpForm;
CRumDealMgr		g_stRumDealForm;
CRumBuyMgr		g_stRumBuyForm;
CRumSellMgr		g_stRumSellForm;
CRumPurMgr		g_stRumPurForm;
CRumSaleMgr		g_stRumSaleForm;
CRumGetMgr		g_stRumGetForm;
CHelpInfoMgr	g_FormHelpInfo;

CChaQeryInfo g_ChaQeryInfo;

bool CUIInterface::_IsAllInit = false;

CUIInterface::CUIInterface()
{
	all.push_back( this );
}

bool CUIInterface::All_Init()
{
	//	int size =  all.size();
	size_t size =  all.size();

	for( allmgr::iterator it=all.begin(); it!=all.end(); it++ )
	{
		CUIInterface* pUI = dynamic_cast<CUIInterface*>(*it);
		if( !pUI->Init() )
		{
			const type_info& info = typeid(**it);
			ErrorReport("msg[%s] Init failed", info.name() );
			return false;
		}
	}

	CFormMgr& mgr = CFormMgr::s_Mgr;
	mgr.AddKeyCharEvent( _evtESCKey );
	_IsAllInit = true;
	return true;
}

void CUIInterface::All_End()
{
	if( !_IsAllInit ) return;

	for( allmgr::iterator it=all.begin(); it!=all.end(); it++ )
	{
		(*it)->End();
	}
//	g_stUIEquip.End();
}

void CUIInterface::All_FrameMove( DWORD dwTime )
{
	for( allmgr::iterator it=all.begin(); it!=all.end(); it++ )
	{
		(*it)->FrameMove(dwTime);
	}
}

void CUIInterface::All_LoadingCall()
{
	for( allmgr::iterator it=all.begin(); it!=all.end(); it++ )
	{
		(*it)->LoadingCall();
	}
}

void CUIInterface::All_SwitchMap()
{
	for( allmgr::iterator it=all.begin(); it!=all.end(); it++ )
	{
		(*it)->SwitchMap();
	}
}

void CUIInterface::MainChaMove()
{
	for( allmgr::iterator it=all.begin(); it!=all.end(); it++ )
		(*it)->CloseForm();
}

CForm* CUIInterface::_FindForm(const char * frmName)
{
	CForm*	form = CFormMgr::s_Mgr.Find( frmName );
//	if( !form )	LG("gui", RES_STRING(CL_LANGUAGE_MATCH_464), frmName );
	return form;
}

void CUIInterface::_evtDragToGoodsEvent(CGuiData *pSender, CCommandObj* pItem, int nGridID, bool& isAccept)
{
    isAccept = false;

	if( !CGameScene::GetMainCha() ) return;

	if( !CGameApp::GetCurScene() ) return;

	CGoodsGrid* pSelf = dynamic_cast<CGoodsGrid*>(pSender);
	if( !pSelf ) return;

    COneCommand* pCom = dynamic_cast<COneCommand*>(CDrag::GetParent());
    if( pCom )
	{
        int iIndex = -1;
        if( g_stUIEquip.IsEquipCom( pCom ) && pSelf==g_stUIEquip.GetGoodsGrid() )
		{
			// ´Ó×°±¸À¸Ð¶ÔØµÀ¾ßµ½µÀ¾ßÀ¸
			g_stUIEquip.UnfixToGrid( pCom->GetCommand(), nGridID, pCom->nTag );
			return;
		}

		/* µÀ¾ß×°±¸¾«Á¶ */
		// ´Ó¾«Á¶½çÃæÍÏ¶¯µ½ÎïÆ·À¸
		iIndex = g_stUIForge.GetForgeComIndex(pCom);
		if (pSelf == g_stUIEquip.GetGoodsGrid()	&& -1 != iIndex)
		{
			g_stUIForge.DragToEquipGrid(iIndex);
			return;
		}

		/* µÀ¾ß×°±¸ºÏ³É */
		// ´ÓºÏ³É½çÃæÍÏ¶¯µ½ÎïÆ·À¸
		if (pSelf == g_stUIEquip.GetGoodsGrid()	&& g_stUIMakeEquip.IsAllCommand(pCom))
		{
			if (g_stUIMakeEquip.IsRouleauCommand(pCom))
				g_stUIMakeEquip.DragRouleauToEquipGrid();
			else
			{
				/*int*/ iIndex = g_stUIMakeEquip.GetItemComIndex(pCom);
				if (iIndex != -1)
					g_stUIMakeEquip.DragItemToEquipGrid(iIndex);
			}
			return;
		}

		// Ìá´¿
		// ´ÓÌá´¿½çÃæÍÏµ½ÎïÆ·À¸
		int nPurifyIndex = g_stUIPurify.GetItemComIndex(pCom);
		if (pSelf == g_stUIEquip.GetGoodsGrid()	&& -1 != nPurifyIndex)
		{
			g_stUIPurify.DragItemToEquipGrid(nPurifyIndex);
		}

        //  ÖÆÔì
        iIndex = g_stUICompose.GetComIndex(pCom);
        if(pSelf == g_stUIEquip.GetGoodsGrid() && -1 != iIndex)
        {
            if(1 == iIndex)
            {
                g_stUICompose.ClearCommand();
            }
            else
            {
                g_stUICompose.DragToEquipGrid(iIndex);
            }
            return;
        }

        //  ¶ÍÔì
        iIndex = g_stUIFound.GetComIndex(pCom);
        if(pSelf == g_stUIEquip.GetGoodsGrid() && -1 != iIndex)
        {
            if(1 == iIndex)
            {
                g_stUIFound.ClearCommand();
            }
            else
            {
                g_stUIFound.DragToEquipGrid(iIndex);
            }
            return;
        }

        //  Åëâ¿
        iIndex = g_stUICooking.GetComIndex(pCom);
        if(pSelf == g_stUIEquip.GetGoodsGrid() && -1 != iIndex)
        {
            if(1 == iIndex)
            {
                g_stUICooking.ClearCommand();
            }
            else
            {
                g_stUICooking.DragToEquipGrid(iIndex);
            }
            return;
        }

        //  ·Ö½â
        iIndex = g_stUIBreak.GetComIndex(pCom);
        if(pSelf == g_stUIEquip.GetGoodsGrid() && -1 != iIndex)
        {
            g_stUIBreak.DragToEquipGrid(iIndex);
            return;
        }

		return;
	}

    CGoodsGrid* pDrag = dynamic_cast<CGoodsGrid*>(CDrag::GetParent());
	if( !pDrag ) return;

	// ´ÓNPC´¦ÂòµÀ¾ß
	if( pSelf==g_stUIEquip.GetGoodsGrid() 
		&& g_stUINpcTrade.IsNpcGoods( pDrag ) )
	{		
		g_stUINpcTrade.LocalBuyFromNpc( pDrag, pSelf, nGridID, pItem );
		return;
	}

	// ÂôµÀ¾ß¸øNPC
	if( pDrag==g_stUIEquip.GetGoodsGrid() 
		&& g_stUINpcTrade.IsNpcGoods( pSelf ) )
	{
		g_stUINpcTrade.LocalSaleToNpc( pSelf, pDrag, nGridID, pItem );
		return;
	}

	// ´Ó½»Ò×ËùÂò¶«Î÷µ½´¬²Õ Michael Chen (2005-05-27)
	if ( pDrag==g_stUIBourse.GetBuyGoodsGrid() && 
		 pSelf == g_stUIBourse.GetShipRoomGoodsGrid())
	{			
		g_stUIBourse.BuyItem(*pSelf, *pItem, nGridID);
		return;
	}

	// Íæ¼Ò¼ä½»Ò×:¼ÓÈëÒ»¸öÎïÆ·ÍÏ¶¯
	if (pDrag == g_stUITrade.GetRequestGrid() &&
		pSelf==g_stUITrade.GetPlayertradeSaleGrid())
	{
		g_stUITrade.LocalSaleItem( pSelf, pDrag, nGridID, pItem );
		return;
	}

	// Íæ¼Ò¼ä½»Ò×:È¡ÏûÒ»¸öÎïÆ·½»Ò×
	if( pSelf==g_stUITrade.GetRequestGrid() 
		&&  pDrag==g_stUITrade.GetPlayertradeSaleGrid() )
	{
		g_stUITrade.LocalCancelItem( pDrag, pSelf, nGridID, pItem );
		return;
	}

	// ´ÓÒøÐÐÈ¡³öÎïÆ·
	if ( pSelf == g_stUIEquip.GetGoodsGrid()
		&& pDrag == g_stUIBank.GetBankGoodsGrid())
	{
		g_stUIBank.PopFromBank(*pDrag, *pSelf, nGridID, *pItem);
	}

	// ½«ÎïÆ·´æµ½ÒøÐÐ
	if ( pSelf == g_stUIBank.GetBankGoodsGrid()
		&& pDrag == g_stUIEquip.GetGoodsGrid())
	{
		g_stUIBank.PushToBank(*pDrag, *pSelf, nGridID, *pItem);
	}

	// FEATURE: GUILD_BANK
	if (pSelf == g_stUIEquip.GetGoodsGrid()
		&& pDrag == g_stUIGuildMgr.GetBankGoodsGrid())
	{
		g_stUIGuildMgr.PopFromBank(*pDrag, *pSelf, nGridID, *pItem);
	}

	if (pSelf == g_stUIGuildMgr.GetBankGoodsGrid()
		&& pDrag == g_stUIEquip.GetGoodsGrid())
	{
		g_stUIGuildMgr.PushToBank(*pDrag, *pSelf, nGridID, *pItem);
	}


	/* °ÚÌ¯Ïà¹Ø */
	// ´ÓÌ¯Î»ÍÏ¶¯µ½ÎïÆ·À¸
	if (pSelf == g_stUIEquip.GetGoodsGrid()
		&& pDrag == g_stUIBooth.GetBoothItemsGrid())
	{
		g_stUIBooth.PopFromBooth(*pDrag, *pSelf, nGridID, *pItem);
	}

	// ´ÓÎïÆ·À¸ÍÏ¶¯µ½Ì¯Î»
	if (pSelf == g_stUIBooth.GetBoothItemsGrid()
		&& pDrag == g_stUIEquip.GetGoodsGrid())
	{
		g_stUIBooth.PushToBooth(*pDrag, *pSelf, nGridID, *pItem);
	}

	// ´ÓÁÙÊ±±³°üµ½±³°ü
	if (pSelf == g_stUIEquip.GetGoodsGrid() &&
		pDrag == g_stUIStore.GetTempKitbagGrid())
	{
		g_stUIStore.PopFromTempKitbag(*pDrag, *pSelf, nGridID, *pItem);
	}

	// ºÚÊÐ½»Ò×
	if (pSelf == g_stUIBlackTrade.GetBuyGoodsGrid() &&
		pDrag == g_stUIBlackTrade.GetSaleGoodsGrid())
	{
		g_stUIBlackTrade.SailToBuy(*pDrag, *pSelf, nGridID, *pItem);
	}

}

bool CUIInterface::_evtESCKey(char& key)
{
	if( key==VK_ESCAPE )
	{
		if( g_pGameApp->IsCtrlPress() ) return false;		// ÆÁ±Î°´¼üCtrl+[

		if( !dynamic_cast<CWorldScene*>(CGameApp::GetCurScene()) ) return false;

		g_stUIStart.CloseForm();

		if( g_stUITrade.IsTrading() ) return false;

		CForm* frm = CFormMgr::s_Mgr.FindESCForm();
		if( frm ) 
		{
			GuiFormEscCloseEvent	evtEscClose = frm->evtEscClose;
			if( evtEscClose)
			{
				evtEscClose( frm );
			}
			else
			{
				frm->Close();
			}
			return true;
		}

		if( CFormMgr::s_Mgr.ModalFormNum()==0 && CFormMgr::s_Mgr.GetEnableHotKey() )
		{
			frm = g_stUISystem.GetSystemForm();
			if( frm )
			{
				frm->Show();
				return true;
			}
		}
	}
	return false;
}

bool CUIInterface::Error(const char* strInfo, const char* strFormName, const char* strCompentName) 
{
	string strFormat = strInfo;
	if( strFormat.substr( 0, 3 )!="msg" )
		strFormat = "msg" + strFormat;
	
	static BOOL s_bErrorShowed = FALSE;
	if( !s_bErrorShowed )
	{
		s_bErrorShowed = TRUE;
		char buf[1000];
		_snprintf_s(buf, _countof(buf), _TRUNCATE, strFormat.c_str(), strFormName, strCompentName);
		MessageBox(NULL, buf, "Error", MB_OK | MB_ICONERROR);
	}

	GameErrorLog(strFormat.c_str(), strFormName, strCompentName);
	LG("gui", strFormat.c_str(), strFormName, strCompentName );
	return false;
}
