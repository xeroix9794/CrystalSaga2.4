#pragma once
#include "uiglobalvar.h"
#include "GuildData.h"
#include "UIPage.h"

// FEATURE: GUILD_BANK
#include "NetProtocol.h"
#include "UIBoxForm.h"

namespace GUI
{

class CUIGuildMgr :
	public CUIInterface
{
public:
	CUIGuildMgr(void);
	~CUIGuildMgr(void);
	static void ShowForm();
	static void RefreshList();
	static void RefreshAttribute();
	static void RefreshForm();
	static void RemoveForm();

	// FEATURE: GUILD_BANK
	CGoodsGrid* GetBankGoodsGrid() {
		return grdBank;
	}

	bool PushToBank(CGoodsGrid& rkDrag, CGoodsGrid& rkSelf, int nGridID, CCommandObj& rkItem);
	bool PopFromBank(CGoodsGrid& rkDrag, CGoodsGrid& rkSelf, int nGridID, CCommandObj& rkItem);
	static void AddGuildAttrs(DWORD attrs[11]);
	static void defaultAttr();
	static void SetGuildLevel(long level);
protected:
	virtual bool Init();

private:
	static CForm*		m_pGuildMgrForm;
	static CLabelEx*	m_plabGuildName;
	static CLabelEx*	m_plabGuildMottoName;
	static CLabelEx*	m_plabGuildType;
	static CLabelEx*	m_plabGuildMaster;
	static CLabelEx*	m_plabGuildMemberCount;
	static CLabelEx*	m_plabGuildExperience;
	static CLabelEx*	m_plabGuildMoney;
	static CLabelEx*	m_plabGuildState;
	static CLabelEx*	m_plabGuildRemainTime;
	//static CLabelEx*	m_plabGuildRank;
	static CListView*	m_plstGuildMember;
	static CListView*	m_plstRecruitMember;
	static CTextButton*	m_pbtnGuildMottoEdit;
	static CTextButton*	m_pbtnGuildDismiss;
	static CTextButton*	m_pbtnMemberRecruit;
	static CTextButton*	m_pbtnMemberRefuse;
	static CTextButton*	m_pbtnMemberKick;
	static CPage*		m_ppgeClass;
	static CImage*		imgVideoT;
	static CTextButton* btnClosePerm;
	static CTextButton* btnYesPerm;
	static CTextButton* btnNoPerm;


	static void GetAttrCost(int attr);
	static void CalculateCost(int index);

	static void _OnClickEditMottoName(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickDismiss(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickRecruit(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickRefuse(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickKick(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickPerm(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickPermOK(CGuiData *pSender, int x, int y, DWORD key);

	static void _OnClickAttr1(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickAttr2(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickAttr3(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickAttr4(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickAttr5(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickAttr6(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickAttr7(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickAttr8(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickAttr9(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickAttr10(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnClickAttr11(CGuiData *pSender, int x, int y, DWORD key);
	
	static void _OnClickSelectPage(CGuiData *pSender);
	static void _OnClickLeave(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _OnPassKick(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _OnPassDismiss(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

	static CForm*		m_pGuildMottoNameEditForm;
	static CForm*		guildperm;
	static CEdit*		m_pedtGuildMottoName;
	static CTextButton*	m_pbtnGuildMottoFormOK;
	static void _OnClickMottoFormOK(CGuiData *pSender, int x, int y, DWORD key);

	static void OnBeforeShow(CForm* pForm,bool& IsShow);
	static void ResetGuildPermissions();
	// FEATURE: GUILD_BANK

	stNumBox* m_pkNumberBox;
	stNetBank m_kNetBank;
	CGoodsGrid* grdBank;

	static CCheckBox* perm1;
	static CCheckBox* perm2;
	static CCheckBox* perm3;
	static CCheckBox* perm4;
	static CCheckBox* perm5;
	static CCheckBox* perm6;
	static CCheckBox* perm7;
	static CCheckBox* perm8;

	static void _MoveItemsEvent(CCompent* pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _MoveAItemEvent(CCompent* pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _evtBankToBank(CGuiData* pSender, int nFirst, int nSecond, bool& isSwap);

	static CTextButton* m_pbtnGoldDeposit;
	static CTextButton* m_pbtnGoldWithdraw;

	static void _MainMousePlayerTradeEvent(CCompent* pSender, int nMsgType, int x, int y, DWORD dwKey);

	static void _evtGoldFormDepositEvent(CCompent* pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _evtGoldFormWithdrawEvent(CCompent* pSender, int nMsgType, int x, int y, DWORD dwKey);

	// FEATURE GUILD_ATTR
	static CTextButton*   btnperm;
	static CTextButton*   btnattr1;
	static CTextButton*   btnattr2;
	static CTextButton*   btnattr3;
	static CTextButton*   btnattr4;
	static CTextButton*   btnattr5;
	static CTextButton*   btnattr6;
	static CTextButton*   btnattr7;
	static CTextButton*   btnattr8;
	static CTextButton*   btnattr9;
	static CTextButton*   btnattr10;
	static CTextButton*   btnattr11;

	static CLabelEx*	attr1;
	static CLabelEx*	attr2;
	static CLabelEx*	attr3;
	static CLabelEx*	attr4;
	static CLabelEx*	attr5;
	static CLabelEx*	attr6;
	static CLabelEx*	attr7;
	static CLabelEx*	attr8;
	static CLabelEx*	attr9;
	static CLabelEx*	attr10;
	static CLabelEx*	attr11;

public:
	static CLabelEx* m_plabGuildGold;

};

}
