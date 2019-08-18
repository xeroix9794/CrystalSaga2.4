#include "StdAfx.h"
#include "UIChat.h"
#include "UITeam.h"
#include "uiformmgr.h"
#include "uitreeview.h"
#include "uiitem.h"
#include "uiprogressbar.h"
#include "uilabel.h"
#include "uigraph.h"
#include "uiboatform.h"
#include "smallmap.h"
#include "ui3dcompent.h"
#include "GameApp.h"
#include "netcommand.h"

#include "teaminviteformmgr.h"
#include "frndinviteformmgr.h"
#include "Character.h"
#include "uilist.h"
#include "PacketCmd.h"
#include "UICheckBox.h"
#include "UIEdit.h"
#include "UIForm.h"
#include "NetChat.h"
#include "UIMenu.h"
#include "chaticonset.h"
#include "talksessionformmgr.h"
#include "GameConfig.h"
#include <fstream>
#include "uiboxform.h"
#include "UITemplete.h"
#include "worldscene.h"
#include "GuildMemberData.h"
#include "GuildMembersMgr.h"
#include "UIStartForm.h"
#include "GuildData.h"
#include "autoattack.h"
#include "UIAllButtonForm.h"
#include "UICozeForm.h"

using namespace GUI;

int SMALL_ICON_SIZE = 16;
int BIG_ICON_SIZE = 32;

CTeamMgr* CChat::_pTeamMgr=NULL;
CForm* CChat::_frmQQ=NULL;

CForm* CChat::_frmAddFriend=NULL;

CForm* CChat::_frmChangeSytle=NULL;
CForm* CChat::_frmEditMotto=NULL;
CForm* CChat::_frmDetails=NULL;
CList* CChat::_lstFrndPlayer=NULL;
CList* CChat::_lstGroupPlayer=NULL;
CList* CChat::_lstRoadPlayer=NULL;
CList* CChat::_lstStyle=NULL;
CMember* CChat::_curSelectMember=NULL;
CEdit* CChat::_pEditFrndName=NULL;

CEdit* CChat::_pEditMotto=NULL;
DWORD CChat::_dwSelfID=0;
DWORD CChat::_dwSelfIcon=0;
string CChat::_strSelfMottoName="";
stPersonInfo CChat::_SelfInfo;
CTreeGridNode* CChat::_pGuildNode    = NULL;
CTreeGridNode* CChat::_pSessionNode  = NULL;
CTreeGridNode* CChat::_pMasterNode   = NULL;
CTreeGridNode* CChat::_pPrenticeNode = NULL;

CMenu*	CChat::_frndMouseRight    = NULL;
CMenu*	CChat::_groupMouseRight   = NULL;
CMenu*	CChat::_roadMouseRight    = NULL;
CMenu*	CChat::_sessMouseRight    = NULL;
CMenu*	CChat::_MasterMouseRight  = NULL;
CMenu*	CChat::_StudentMouseRight = NULL;
CMenu*	CChat::_styMenu           = NULL;

CLabelEx* CChat::_labNameOfMottoFrm=NULL;
CImage*	CChat::_imgFaceOfMottoFrm=NULL;
CCheckBox*	CChat::_chkForbidOfMottoFrm=NULL;
vector<char*>	CChat::_strFilterTxt;

bool CChat::_bForbid=false;

CCharacter2D*	CChat::_pCharacter[MAX_MEMBER] = { 0 };
CLabelEx*		CChat::labMenberName[MAX_MEMBER] = { 0 };
CForm*			CChat::frmTeamMenber[MAX_MEMBER] = { 0 };
CProgressBar*   CChat::proTeamMenberHP[MAX_MEMBER] = { 0 };
CProgressBar*   CChat::proTeamMenberSP[MAX_MEMBER] = { 0 };
CLabelEx*       CChat::labLv[MAX_MEMBER] = { 0 };
CImage*		    CChat::imgWork[MAX_MEMBER] = { 0 };
CImage*		    CChat::imgLeader[MAX_MEMBER] = { 0 };

//---------------------------------------------------------------------------
// class CChat
//---------------------------------------------------------------------------

CChat::CChat()
:m_pQQTreeView(NULL), _pGroupNode(NULL), _pFrndNode(NULL)
{
	frmChatManage = NULL;

}

bool CChat::Init()
{
	//ifstream filterTxt("scripts\\table\\filter.txt",ios::in);
	//if (filterTxt.is_open())
	//{
	//	char buf[500]={0};
	//	filterTxt.getline(buf,500);
	//	while (!filterTxt.fail())
	//	{
	//		char *pText=new char[strlen(buf)+2];
	//		strcpy(pText,buf);
	//		_strFilterTxt.push_back(pText);
	//		filterTxt.getline(buf,500);
	//	}
	//	filterTxt.close();
	//}
	CFormMgr::s_Mgr.AddHotKeyEvent(CTalkSessionFormMgr::OnHotKeyShow);

    _pTeamMgr = new CTeamMgr;

    CFormMgr& mgr = CFormMgr::s_Mgr;

    // ³õÊ¼»¯×é¶Ó¿Ø¼þ
    char szBuf[80] = { 0 };
    for( int i=0; i<MAX_MEMBER; i++ )
    {
		_snprintf_s(szBuf, _countof(szBuf), _TRUNCATE, "frmTeamMenber%d", i + 1);
		frmTeamMenber[i] = mgr.Find(szBuf);
		if (!frmTeamMenber[i]) return false;
		frmTeamMenber[i]->SetIsShow(false);
		frmTeamMenber[i]->Refresh();

		_snprintf_s(szBuf, _countof(szBuf), _TRUNCATE, "proTeamMenber%dHP", i + 1);
		proTeamMenberHP[i] = dynamic_cast<CProgressBar*> (frmTeamMenber[i]->Find(szBuf));
		if (!proTeamMenberHP[i]) return _Error(RES_STRING(CL_LANGUAGE_MATCH_45), frmTeamMenber[i]->GetName(), szBuf);
		proTeamMenberHP[i]->SetRange(0.0f, 1.0f);

		_snprintf_s(szBuf, _countof(szBuf), _TRUNCATE, "proTeamMenber%dSP", i + 1);
		proTeamMenberSP[i] = dynamic_cast<CProgressBar*> (frmTeamMenber[i]->Find(szBuf));
		if (!proTeamMenberSP[i]) return _Error(RES_STRING(CL_LANGUAGE_MATCH_45), frmTeamMenber[i]->GetName(), szBuf);
		proTeamMenberSP[i]->SetRange(0.0f, 1.0f);

		_snprintf_s(szBuf, _countof(szBuf), _TRUNCATE, "labMenber%dName", i + 1);
		labMenberName[i] = dynamic_cast<CLabelEx*> (frmTeamMenber[i]->Find(szBuf));
		if (!labMenberName[i]) return _Error(RES_STRING(CL_LANGUAGE_MATCH_45), frmTeamMenber[i]->GetName(), szBuf);

		labLv[i] = dynamic_cast<CLabelEx*> (frmTeamMenber[i]->Find("labFLv"));
		if (!labLv[i]) return _Error(RES_STRING(CL_LANGUAGE_MATCH_45), frmTeamMenber[i]->GetName(), "labFLv");

		imgWork[i] = dynamic_cast<CImage*> (frmTeamMenber[i]->Find("imgobj"));
		if (!imgWork[i]) return _Error(RES_STRING(CL_LANGUAGE_MATCH_45), frmTeamMenber[i]->GetName(), "imgobj");

		imgLeader[i] = dynamic_cast<CImage*> (frmTeamMenber[i]->Find("imgLeader"));
		if (!imgLeader[i]) return _Error(RES_STRING(CL_LANGUAGE_MATCH_45), frmTeamMenber[i]->GetName(), "imgLeader");

		C3DCompent* p3DDown = dynamic_cast<C3DCompent*>(frmTeamMenber[i]->Find("d3dDown"));
		if (!p3DDown) return _Error(RES_STRING(CL_LANGUAGE_MATCH_45), frmTeamMenber[i]->GetName(), "d3dDown");
		p3DDown->nTag = i;
		p3DDown->evtMouseDown = _MemberMouseDownEvent;
		p3DDown->SetMouseAction(enumMA_Skill);

		C3DCompent* p3D = dynamic_cast<C3DCompent*>(frmTeamMenber[i]->Find("d3dHead"));
		if (!p3D) return _Error(RES_STRING(CL_LANGUAGE_MATCH_45), frmTeamMenber[i]->GetName(), "d3dHead");
		p3D->SetRenderEvent(_RenderEvent);
		p3D->nTag = i;

		RECT rt;
		rt.left = p3D->GetX();
		rt.right = p3D->GetX2();
		rt.top = p3D->GetY();
		rt.bottom = p3D->GetY2();

		_pCharacter[i] = new CCharacter2D;
		_pCharacter[i]->Create(rt);
	}

	// ³õÊ¼»¯QQ½çÃæ
	_frmQQ = mgr.Find("frmQQ");
	if (!_frmQQ)
	{
		::Error(RES_STRING(CL_LANGUAGE_MATCH_412));
		return false;
	}

	_frmQQ->SetIsShow(true);
	_frmQQ->evtShow = _evtQQMainShow;
	CGuiTime::Create(300, _OnTimerFlash);

	//	初始化	Add by alfred.shi 20080903	begin

	//	End.

	CTextButton* pbtnAdd = dynamic_cast<CTextButton*>(_frmQQ->Find("btnAdd"));
	if (!pbtnAdd) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "btnAdd");
	pbtnAdd->evtMouseClick = _evtQQMainAddFrnd;


	pbtnAdd = dynamic_cast<CTextButton*>(_frmQQ->Find("btnEdit"));
	if (!pbtnAdd) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "btnEdit");
	pbtnAdd->evtMouseClick = _evtQQMainEditMotto;

	pbtnAdd = dynamic_cast<CTextButton*>(_frmQQ->Find("btnscr"));
	if (!pbtnAdd) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "btnscr");
	pbtnAdd->evtMouseClick = _evtQQMainEditMotto;

	//	Add by alfred.shi 20080903 begin
	/*CTextButton* btnAddFriend = dynamic_cast<CTextButton*>(frmjieguo2->Find("btnAddFriend"));
	if (!btnAddFriend) return false;
	btnAddFriend->evtMouseClick = _evtAddFriend;

	CTextButton* btnViewFriend = dynamic_cast<CTextButton*>(frmjieguo2->Find("btnViewFriend"));
	if (!btnViewFriend) return false;
	btnViewFriend->evtMouseClick = _evtViewFriend;

	CTextButton* btnInfo = dynamic_cast<CTextButton*>(frmchainfo2->Find("btnInfo"));
	if (!btnInfo) return false;
	btnInfo->evtMouseClick = _evtViewDetailInfo;

	CForm*	frmDetail = _FindForm("frmDetail");
	CTextButton* btnshow = dynamic_cast<CTextButton*>(frmDetail->Find("btnshow"));
	//if(btnshow)	btnshow->SetFlashCycle();

	if (btnshow)
		btnshow->evtMouseClick = _evtGostMainEditMotto;
	//	End*/
	CForm*	frmDetail = _FindForm("frmDetail");
	CTextButton* btnshow = dynamic_cast<CTextButton*>(frmDetail->Find("btnshow"));
	btnshow->evtMouseClick = _evtGostMainEditMotto;

	m_pQQTreeView = dynamic_cast<CTreeView*>(_frmQQ->Find("trvEditor"));
	if (!m_pQQTreeView) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "trvEditor");

	CLabelEx* pMineName = dynamic_cast<CLabelEx*>(_frmQQ->Find("labMineName"));
	if (!pMineName)  return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "labMineName");

	// Add by lark.li 20080804 begin
	/*_frndGroupMouseRight = CMenu::FindMenu("frndGroupRight");
	if (!_frndGroupMouseRight)  return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "frndGroupRight");
	_frndGroupMouseRight->evtListMouseDown = _OnMouseFrndGroupMenu;

	_hitNothingMouseRight = CMenu::FindMenu("hitNothingRight");
	if (!_frndGroupMouseRight)  return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "frndGroupRight");
	_hitNothingMouseRight->evtListMouseDown = _OnMouseAddFrndGroupMenu;
	// End
	*/
	_frndMouseRight = CMenu::FindMenu("frndMouseRight");
	if (!_frndMouseRight)  return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "frndMouseRight");
	_frndMouseRight->evtListMouseDown = _OnMouseFrndMenu;

	_groupMouseRight = CMenu::FindMenu("groupMouseRight");
	if (!_groupMouseRight)  return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "groupMouseRight");
	_groupMouseRight->evtListMouseDown = _OnMouseGroupMenu;

	_roadMouseRight = CMenu::FindMenu("roadMouseRight");
	if (!_roadMouseRight)  return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "roadMouseRight");
	_roadMouseRight->evtListMouseDown = _OnMouseRoadMenu;

	_sessMouseRight = CMenu::FindMenu("ChatMouseRight");
	if (!_sessMouseRight)  return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "ChatMouseRight");
	_sessMouseRight->evtListMouseDown = _OnMouseSessMenu;

	_MasterMouseRight = CMenu::FindMenu("MasterRight");
	if (!_MasterMouseRight)  return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "MasterRight");
	_MasterMouseRight->evtListMouseDown = _OnMouseMasterMenu;

	_StudentMouseRight = CMenu::FindMenu("StudentRight");
	if (!_StudentMouseRight)  return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "StudentRight");
	_StudentMouseRight->evtListMouseDown = _OnMouseStudentMenu;

	m_pQQTreeView->evtMouseDown=_MainMouseClick;
	m_pQQTreeView->evtMouseDB=_MainMouseClick;
	//m_pQQTreeView->evtMouseDragEnd = _OnDragEnd;

	m_pQQTreeView->SetSelectColor( 0 );
	int nWidth = m_pQQTreeView->GetWidth() - m_pQQTreeView->GetScroll()->GetWidth()-28;
	int nHeight = SMALL_ICON_SIZE+3;

	_pSessionNode = dynamic_cast<GUI::CTreeGridNode*>(m_pQQTreeView->GetRootNode()->FindNode(RES_STRING(CL_LANGUAGE_MATCH_465)));
	_pSessionNode->GetItem()->SetColor( COLOR_BLACK );
	_pSessionNode->GetUpImage()->UnLoadImage();
	_pSessionNode->GetDownImage()->UnLoadImage();
	_pSessionNode->SetUnitSize(nWidth, nHeight );
	_pSessionNode->SetColMaxNum(1);

	_pFrndNode = dynamic_cast<GUI::CTreeGridNode*>(m_pQQTreeView->GetRootNode()->FindNode(RES_STRING(CL_LANGUAGE_MATCH_466)));
	_pFrndNode->GetItem()->SetColor(COLOR_BLACK);
	_pFrndNode->GetUpImage()->UnLoadImage();
	_pFrndNode->GetDownImage()->UnLoadImage();
	_pFrndNode->SetUnitSize(nWidth, nHeight);
	_pFrndNode->SetColMaxNum(1);



	_pGroupNode = dynamic_cast<GUI::CTreeGridNode*>(m_pQQTreeView->GetRootNode()->FindNode(RES_STRING(CL_LANGUAGE_MATCH_467)));
	_pGroupNode->GetItem()->SetColor(COLOR_BLACK);
	_pGroupNode->GetUpImage()->UnLoadImage();
	_pGroupNode->GetDownImage()->UnLoadImage();
	_pGroupNode->SetUnitSize(nWidth, nHeight);
	_pGroupNode->SetColMaxNum(1);

	_pGuildNode = dynamic_cast<GUI::CTreeGridNode*>(m_pQQTreeView->GetRootNode()->FindNode(RES_STRING(CL_LANGUAGE_MATCH_468)));
	_pGuildNode->GetItem()->SetColor(COLOR_BLACK);
	_pGuildNode->GetUpImage()->UnLoadImage();
	_pGuildNode->GetDownImage()->UnLoadImage();
	_pGuildNode->SetUnitSize(nWidth, nHeight);
	_pGuildNode->SetColMaxNum(1);

	_pMasterNode = dynamic_cast<GUI::CTreeGridNode*>(m_pQQTreeView->GetRootNode()->FindNode(RES_STRING(CL_LANGUAGE_MATCH_850)));
	_pMasterNode->GetItem()->SetColor(COLOR_BLACK);
	_pMasterNode->GetUpImage()->UnLoadImage();
	_pMasterNode->GetDownImage()->UnLoadImage();
	_pMasterNode->SetUnitSize(nWidth, nHeight);
	_pMasterNode->SetColMaxNum(1);

	_pPrenticeNode = dynamic_cast<GUI::CTreeGridNode*>(m_pQQTreeView->GetRootNode()->FindNode(RES_STRING(CL_LANGUAGE_MATCH_851)));
	_pPrenticeNode->GetItem()->SetColor(COLOR_BLACK);
	_pPrenticeNode->GetUpImage()->UnLoadImage();
	_pPrenticeNode->GetDownImage()->UnLoadImage();
	_pPrenticeNode->SetUnitSize(nWidth, nHeight);
	_pPrenticeNode->SetColMaxNum(1);

	_pTeamMgr->Find( enumTeamFrnd )->SetPointer( _pFrndNode );
	_pTeamMgr->Find( enumTeamGroup )->SetPointer( _pGroupNode );
	_pTeamMgr->Find( enumTeamMaster)->SetPointer( _pMasterNode );
	_pTeamMgr->Find( enumTeamPrentice )->SetPointer(_pPrenticeNode );

	_frmAddFriend = mgr.Find("frmAddFriend");
	if (!_frmAddFriend)
	{
		::Error(RES_STRING(CL_LANGUAGE_MATCH_470));
		return false;
	}
	_frmAddFriend->SetIsShow(true);
	pbtnAdd = dynamic_cast<CTextButton*>(_frmAddFriend->Find("btnYes"));
	if (!pbtnAdd) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmAddFriend->GetName(), "btnYes");
	pbtnAdd->evtMouseClick = _evtAddFrnd;
	_pEditFrndName = dynamic_cast<CEdit*>(_frmAddFriend->Find("edtTradeGold"));
	if (!_pEditFrndName) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmAddFriend->GetName(), "edtTradeGold");

	_styMenu = CMenu::FindMenu("styMenu");
	if (!_styMenu) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmQQ->GetName(), "styMenu");
	_styMenu->evtListMouseDown = _evtChangeStyle;

	_frmEditMotto = mgr.Find("frmEdit");
	if (!_frmEditMotto)
	{
		::Error(RES_STRING(CL_LANGUAGE_MATCH_471));
		return false;
	}
	_frmEditMotto->SetIsShow(true);
	pbtnAdd = dynamic_cast<CTextButton*>(_frmEditMotto->Find("btnYes"));
	if (!pbtnAdd) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmEditMotto->GetName(), "btnYes");
	pbtnAdd->evtMouseClick = _evtChangeMotto;
	pbtnAdd = dynamic_cast<CTextButton*>(_frmEditMotto->Find("btnFlower"));
	if (!pbtnAdd) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmEditMotto->GetName(), "btnFlower");
	pbtnAdd->evtMouseClick = _evtChangeMotto;
	pbtnAdd = dynamic_cast<CTextButton*>(_frmEditMotto->Find("btnEgg"));
	if (!pbtnAdd) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmEditMotto->GetName(), "btnEgg");
	pbtnAdd->evtMouseClick = _evtChangeMotto;

	pbtnAdd = dynamic_cast<CTextButton*>(_frmEditMotto->Find("btnxiangce01"));
	if (!pbtnAdd) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmEditMotto->GetName(), "btnxiangce01");
	pbtnAdd->evtMouseClick = _evtChangeMotto;

	pbtnAdd = dynamic_cast<CTextButton*>(_frmEditMotto->Find("btnxiangce02"));
	if (!pbtnAdd) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmEditMotto->GetName(), "btnxiangce02");
	pbtnAdd->evtMouseClick = _evtChangeMotto;

	_pEditMotto = dynamic_cast<CEdit*>(_frmEditMotto->Find("edtTradeGold"));
	if (!_pEditMotto) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmEditMotto->GetName(), "edtTradeGold");
	_labNameOfMottoFrm = dynamic_cast<CLabelEx*>(_frmEditMotto->Find("labName"));
	if (!_labNameOfMottoFrm) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmEditMotto->GetName(), "labName");
	_imgFaceOfMottoFrm = dynamic_cast<CImage*>(_frmEditMotto->Find("imgMhead"));
	if (!_imgFaceOfMottoFrm) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmEditMotto->GetName(), "imgMhead");
	_chkForbidOfMottoFrm = dynamic_cast<CCheckBox*>(_frmEditMotto->Find("chkChat"));
	if (!_chkForbidOfMottoFrm) return _Error(RES_STRING(CL_LANGUAGE_MATCH_411), _frmEditMotto->GetName(), "chkChat");

	//_frmDetails = mgr.Find("frmdetails");
	//if( !_frmDetails )
	//{
	//	::Error(RES_STRING(CL_LANGUAGE_MATCH_472));
	//	return false;
	//}
	//_frmDetails->SetIsShow(true);
	//pbtnAdd = dynamic_cast<CTextButton*>(_frmDetails->Find("btnRefurbish"));
	//if( !pbtnAdd ) return _Error(RES_STRING(CL_LANGUAGE_MATCH_473), _frmDetails->GetName(), "btnRefurbish" );
	//pbtnAdd->evtMouseClick=_evtRefreshInfo;

	return true;
}

void CChat::End()
{
    m_pQQTreeView = NULL;
    //delete _pTeamMgr;
	SAFE_DELETE(_pTeamMgr); // UIµ±»ú´¦Àí

    for( int i=0; i<MAX_MEMBER; i++ )
    {
        //delete _pCharacter[i];
        //_pCharacter[i] = 0;
		SAFE_DELETE(_pCharacter[i]); // UIµ±»ú´¦Àí
    }
}

void CChat::ClearTeam()
{
	for( int i=0; i<MAX_MEMBER; i++)
    {
		frmTeamMenber[i]->SetIsShow(false);
    }
}

void CChat::RefreshTeamData( CMember* pCurMember )
{
	if( !pCurMember || !pCurMember->GetPointer() ) return;

	CMemberData *pCurMemData = pCurMember->GetData();	
	DWORD i = pCurMemData->GetIndex();

    proTeamMenberHP[i]->SetRange( 0.0f, (float)pCurMemData->GetMaxHP() );
    proTeamMenberHP[i]->SetPosition( (float)pCurMemData->GetHP() );

	static char szBuf[32] = { 0 };
	sprintf( szBuf, "%d", pCurMemData->GetLV() );
	labLv[i]->SetCaption( szBuf );

	if (pCurMemData->GetFace()) _pCharacter[i]->UpdataFace(*pCurMemData->GetFace());
}

void CChat::_evtGostMainEditMotto(CGuiData *pSender, int x, int y, DWORD key)
{
	if (!g_pGameApp->GetCurScene()) return;
	if (!g_pGameApp->GetCurScene()->GetMainCha()) return;

	g_stAllButton.ShowAllForm(true);
}

void CChat::RefreshTeam()
{
    CTeam* pTeam = _pTeamMgr->Find(enumTeamGroup);
	if( !dynamic_cast<CWorldScene*>(CGameApp::GetCurScene()) )
	{
		for( int i=0; i<MAX_MEMBER; i++)
		{	
			frmTeamMenber[i]->SetIsShow(false);
		}
		return;
	}

	if( CGameScene::GetMainCha() && CGameScene::GetMainCha()->getHumanID()==CTeamMgr::GetTeamLeaderID() )
	{
		// ÔÚ×ÔÒÑµÄÍ·ÉÏÏÔÊ¾¶Ó³¤±êÖ¾
		g_stUIStart.SetIsLeader( true );
	}
	else
	{
		g_stUIStart.SetIsLeader( false );
	}

	int Num = pTeam->GetCount();
	for( int i=MAX_MEMBER-1; i>=0; i--)
	{	
		if ( i<Num )
		{
			CMember* pCurMember = pTeam->GetMember( i );
			if( pCurMember && pCurMember->GetPointer())
            {
			    CMemberData *pCurMemData = pCurMember->GetData();
				pCurMemData->SetIndex( i );

				labMenberName[i]->SetCaption( pCurMember->GetName() );

				RefreshTeamData( pCurMember );
			}

			frmTeamMenber[i]->nTag = pCurMember->GetID();

			imgLeader[i]->SetIsShow( pCurMember->GetID()==CTeamMgr::GetTeamLeaderID() );
			frmTeamMenber[i]->SetIsShow( true );
		}
		else
		{
			frmTeamMenber[i]->SetIsShow( false );
		}
	}
}

int CChat::TeamSend( DWORD dwMsg, void* pData, DWORD dwParam )
{
    CTreeView* pTree = GetTeamView();
    if( !pTree ) return -1;

    switch( dwMsg )
    {
    case enumSTM_ADD_GROUP:
        {
            CTeam* pTeam = (CTeam*)pData;
            
            CTreeGridNode* pNode = (CTreeGridNode*)pTeam->GetPointer();
            if( !pNode ) return -1;

            pNode->Clear();
            if( pNode->GetIsExpand() )
                pTree->Refresh();

            if( enumTeamGroup==dwParam )
                ClearTeam();
        }
        break;
    case enumSTM_DEL_GROUP:
        {
            CTeam* pTeam = (CTeam*)pData;
            CTreeGridNode* pNode = (CTreeGridNode*)pTeam->GetPointer();
            if( !pNode ) return -1;

            pNode->Clear();
            if( pNode->GetIsExpand() )
                pTree->Refresh();

            if( enumTeamGroup==dwParam )
                ClearTeam();
        }
        break;
    case enumSTM_ADD_MEMBER:
        {
            CMember* pMember = (CMember*)pData;
            CTreeGridNode* pNode = (CTreeGridNode*)pMember->GetTeam()->GetPointer();
            if( !pNode ) return -1;

			CTextGraph* pItem = new CTextGraph(2);
            pItem->SetString( pMember->GetShowName() );
			pItem->SetPointer(pMember);
            pNode->AddItem( pItem );
            pMember->SetPointer( pItem );
			CChatIconInfo *pIconInfo=GetChatIconInfo(pMember->GetIconID());
			if (pIconInfo)
			{
				CGuiPic* pPic=pItem->GetImage();
				string strPath="texture/ui/HEAD/";
				if (pMember->IsOnline())
				{
					pPic->LoadImage((strPath+pIconInfo->szSmall).c_str(),SMALL_ICON_SIZE,SMALL_ICON_SIZE,0,pIconInfo->nSmallX,pIconInfo->nSmallY);
					pPic->SetFrame( 0 );
					pItem->SetColor(0xfffc2f20);
				}
				else
				{
					pPic->LoadImage((strPath+pIconInfo->szSmallOff).c_str(),SMALL_ICON_SIZE,SMALL_ICON_SIZE,0,pIconInfo->nSmallX,pIconInfo->nSmallY);
					pPic->SetFrame( 0 );
					pItem->SetColor(0xff000000);
				}
			}
			string _strHint=pMember->GetName();
			string strMotto=pMember->GetMotto();
			if (!strMotto.empty())
			{
				_strHint+="("+strMotto+")";
			}
			pItem->SetHint(_strHint.c_str());

			SortOnlineFrnd();

			if( pNode->GetIsExpand() )
				pTree->Refresh();
            if( enumTeamGroup==dwParam )
                RefreshTeam();
        }
        break;
    case enumSTM_DEL_MEMBER:
        {
            CMember* pMember = (CMember*)pData;
			if (pMember==_curSelectMember)
			{
				_curSelectMember=NULL;
				_frmQQ->PopMenu(NULL);
			}
            CTreeGridNode* pNode = (CTreeGridNode*)pMember->GetTeam()->GetPointer();
            if( !pNode ) return -1;

			pNode->DelItem((CItemObj*)pMember->GetPointer());
			pMember->SetPointer(NULL);
            if( pNode->GetIsExpand() )
                pTree->Refresh();

            if( dwParam==enumTeamGroup )
                RefreshTeam();
        }
        break;
    case enumSTM_NODE_CHANGE:
        {
            CMember* pMember = (CMember*)pData;
            CTextGraph* pItem = (CTextGraph*)(pMember->GetPointer());
			CChatIconInfo *pIconInfo=GetChatIconInfo(pMember->GetIconID());
			if (pIconInfo && pItem)	// modify by Philip.Wu  2006-08-13  ÐÞ¸ÄÈ¥³ýÂ·ÈËºóµÄµ±»ú
			{
				CGuiPic* pPic=pItem->GetImage();
				string strPath="texture/ui/HEAD/";
				if (pMember->IsOnline())
				{
					pPic->LoadImage((strPath+pIconInfo->szSmall).c_str(),SMALL_ICON_SIZE,SMALL_ICON_SIZE,0,pIconInfo->nSmallX,pIconInfo->nSmallY);
					pPic->SetFrame( 0 );
					pItem->SetColor(0xfffc2f20);
				}
				else
				{
					pPic->LoadImage((strPath+pIconInfo->szSmallOff).c_str(),SMALL_ICON_SIZE,SMALL_ICON_SIZE,0,pIconInfo->nSmallX,pIconInfo->nSmallY);
					pPic->SetFrame( 0 );
					pItem->SetColor(0xff000000);
				}
			}
			if( pItem )
            {
				pItem->SetString( pMember->GetShowName() );
				string _strHint=pMember->GetName();
				string strMotto=pMember->GetMotto();
				if (!strMotto.empty())
				{
					_strHint+="("+strMotto+")";
				}
				pItem->SetHint(_strHint.c_str());
			}
			
			SortOnlineFrnd();
			pTree->Refresh();

            if( dwParam==enumTeamGroup )
                RefreshTeam();
        }
        break;
    case enumSTM_NODE_DATA_CHANGE:
        {
            if( dwParam==enumTeamGroup )
                RefreshTeamData( (CMember*)pData );
        }
        break;
	case enumSTM_AFTER_DEL_MEMBER:
        {
            if( dwParam==enumTeamGroup )
				RefreshTeam();
        }
        break;
    }
    return 0;
}

void CChat::SortOnlineFrnd()
{
	DWORD nCount=_pFrndNode->GetItemCount();
	DWORD i=0;
	DWORD j=0;
	CTextGraph *pItem1,*pItem2;
	CMember *pMember1,*pMember2;
	for (;i<nCount;i++)
	{
		pItem1=dynamic_cast<CTextGraph*>(_pFrndNode->GetItemByIndex(i));
		pMember1=(CMember*)(pItem1->GetPointer());
		if (pMember1->IsOnline())
		{
			continue;
		}
		if (j<=i)
		{
			j=i+1;
		}
		for (;j<nCount;j++)
		{
			pItem2=dynamic_cast<CTextGraph*>(_pFrndNode->GetItemByIndex(j));
			pMember2=(CMember*)(pItem2->GetPointer());
			if (pMember2->IsOnline())
			{
				//1ºÍ2Ïî¶Ô»»
				pMember1->SetPointer(pItem2);
				pMember2->SetPointer(pItem1);
				pItem1->SetPointer(pMember2);
				pItem2->SetPointer(pMember1);
				CChatIconInfo *pIconInfo1=GetChatIconInfo(pMember1->GetIconID());
				CChatIconInfo *pIconInfo2=GetChatIconInfo(pMember2->GetIconID());
				if (!pIconInfo1 || !pIconInfo2)
				{
					continue;
				}
				CGuiPic* pPic2=pItem2->GetImage();
				string strPath="texture/ui/HEAD/";
				pPic2->LoadImage((strPath+pIconInfo1->szSmallOff).c_str(),SMALL_ICON_SIZE,SMALL_ICON_SIZE,0,pIconInfo1->nSmallX,pIconInfo1->nSmallY);
				pPic2->SetFrame( 0 );
				pItem2->SetColor(0xff000000);
				CGuiPic* pPic1=pItem1->GetImage();
				pPic1->LoadImage((strPath+pIconInfo2->szSmall).c_str(),SMALL_ICON_SIZE,SMALL_ICON_SIZE,0,pIconInfo2->nSmallX,pIconInfo2->nSmallY);
				pPic1->SetFrame( 0 );
				pItem1->SetColor(0xfffc2f20);
				string _strHint1=pMember1->GetName();
				string strMotto1=pMember1->GetMotto();
				if (!strMotto1.empty())
				{
					_strHint1+="("+strMotto1+")";
				}
				pItem2->SetString(pMember1->GetShowName());
				pItem2->SetHint(_strHint1.c_str());
				string _strHint2=pMember2->GetName();
				string strMotto2=pMember2->GetMotto();
				if (!strMotto2.empty())
				{
					_strHint2+="("+strMotto2+")";
				}
				pItem1->SetString(pMember2->GetShowName());
				pItem1->SetHint(_strHint2.c_str());
				break;
			}
		}
	}
}

void CChat::_RenderEvent(C3DCompent *pSender, int x, int y)
{
	_pCharacter[pSender->nTag]->Render();
}

void CChat::_MemberMouseDownEvent(CGuiData *pSender, int x, int y, DWORD key)
{	
	if( key & Mouse_LDown )
	{
		CWorldScene* pScene = dynamic_cast<CWorldScene*>(CGameApp::GetCurScene());
		if( !pScene ) return;

		CCharacter* pCha = pScene->SearchByHumanName( labMenberName[pSender->nTag]->GetCaption() );
		if( !pCha )
		{
			g_pGameApp->SysInfo(RES_STRING(CL_LANGUAGE_MATCH_474), labMenberName[pSender->nTag]->GetCaption());
			return;
		}

		if( g_pGameApp->IsAltPress() )
			pScene->GetMouseDown().GetAutoAttack()->Follow( CGameScene::GetMainCha(), pCha );
		else
			pScene->GetMouseDown().ActCha( CGameScene::GetMainCha(), pCha );
		return;
	}
	
	if( key & Mouse_RDown )
	{
		if( _groupMouseRight )
		{
			CTeam *pTeam = _pTeamMgr->Find( enumTeamGroup );
			CMember *pMember = pTeam->Find( frmTeamMenber[pSender->nTag]->nTag );
			if( pMember )
			{
				_curSelectMember = pMember;
				CMenuItem* pItem = _groupMouseRight->GetMenuItem(3);
				if (pItem && _stricmp(pItem->GetString(), RES_STRING(CL_LANGUAGE_MATCH_475)) == 0)
				{
					if (g_stUIStart.GetIsLeader())
					{
						pItem->SetIsEnabled(true);
					}
					else
					{
						pItem->SetIsEnabled(false);
					}
				}
				pSender->GetForm()->PopMenu(_groupMouseRight,x,y);
				return;
			}
		}
		g_pGameApp->SysInfo(RES_STRING(CL_LANGUAGE_MATCH_476), labMenberName[pSender->nTag]->GetCaption());
	}
}

void CChat::_MainMouseClick(CGuiData *pSender, int x, int y, DWORD key)
{
	CTreeView* pTree = dynamic_cast<CTreeView*>(pSender);
	if( !pTree ) return;

	CTextGraph *pSelectItem=dynamic_cast<CTextGraph*>(pTree->GetHitItem(x,y));
	if (!pSelectItem)
		return;

	CTreeGridNode *pSelectNode=dynamic_cast<CTreeGridNode*>(pTree->GetSelectNode());
	if (!pSelectNode)
		return;
		
	int sx=x;
	int sy=y;

	if (pSelectNode==(_pTeamMgr->Find( enumTeamFrnd ))->GetPointer())
	{
		CTeam *pTeam=_pTeamMgr->Find( enumTeamFrnd );
		for (DWORD i=0;i<pTeam->GetCount();i++)
		{
			CMember *pMember=pTeam->GetMember(i);
			if (pMember->GetPointer()==pSelectItem)
			{
				_curSelectMember=pMember;
				if (key & Mouse_LDB)
				{
					CTalkSessionFormMgr::ApplySession( &_curSelectMember );
				}
				else if ( key & Mouse_RDown )
				{
					if (x+_frndMouseRight->GetWidth()>g_pGameApp->GetWindowWidth())
						sx=g_pGameApp->GetWindowWidth()-_frndMouseRight->GetWidth();
					if (y+_frndMouseRight->GetHeight()>g_pGameApp->GetWindowHeight())
						sy=g_pGameApp->GetWindowHeight()-_frndMouseRight->GetHeight();
					_frmQQ->PopMenu(_frndMouseRight,sx,sy);
				}
				break;
			}
		}
	}
	else if (pSelectNode==_pTeamMgr->Find( enumTeamGroup )->GetPointer())
	{
		CTeam *pTeam=_pTeamMgr->Find( enumTeamGroup );
		for (DWORD i=0;i<pTeam->GetCount();i++)
		{
			CMember *pMember=pTeam->GetMember(i);
			if (pMember->GetPointer()==pSelectItem)
			{
				_curSelectMember=pMember;
				if (key & Mouse_LDB)
				{
					CTalkSessionFormMgr::ApplySession(&_curSelectMember);
				}
				else if (key & Mouse_RDown)
				{
					if (x+_groupMouseRight->GetWidth()>g_pGameApp->GetWindowWidth())
						sx=g_pGameApp->GetWindowWidth()-_groupMouseRight->GetWidth();
					if (y+_groupMouseRight->GetHeight()>g_pGameApp->GetWindowHeight())
						sy=g_pGameApp->GetWindowHeight()-_groupMouseRight->GetHeight();
					_frmQQ->PopMenu(_groupMouseRight,sx,sy);
				}
				break;
			}
		}
	}
	else if (pSelectNode==_pTeamMgr->Find( enumTeamRoad )->GetPointer())
	{
		CTeam *pTeam=_pTeamMgr->Find( enumTeamRoad );
		for (DWORD i=0;i<pTeam->GetCount();i++)
		{
			CMember *pMember=pTeam->GetMember(i);
			if (pMember->GetPointer()==pSelectItem)
			{
				_curSelectMember=pMember;
				if (key & Mouse_LDB)
				{
					CTalkSessionFormMgr::ApplySession(&_curSelectMember);
				}
				else if (key & Mouse_RDown)
				{
					if (x+_roadMouseRight->GetWidth()>g_pGameApp->GetWindowWidth())
						sx=g_pGameApp->GetWindowWidth()-_roadMouseRight->GetWidth();
					if (y+_roadMouseRight->GetHeight()>g_pGameApp->GetWindowHeight())
						sy=g_pGameApp->GetWindowHeight()-_roadMouseRight->GetHeight();
					_frmQQ->PopMenu(_roadMouseRight,sx,sy);
				}
				break;
			}
		}
	}
	else if (pSelectNode == _pTeamMgr->Find( enumTeamMaster )->GetPointer())	// µ¼Ê¦
	{
		CTeam *pTeam=_pTeamMgr->Find( enumTeamMaster );
		for (DWORD i=0;i<pTeam->GetCount();i++)
		{
			CMember *pMember=pTeam->GetMember(i);
			if (pMember->GetPointer()==pSelectItem)
			{
				_curSelectMember=pMember;
				if (key & Mouse_LDB)
				{
					CTalkSessionFormMgr::ApplySession(&_curSelectMember);
				}
				else if (key & Mouse_RDown)
				{
					if (x+_roadMouseRight->GetWidth()>g_pGameApp->GetWindowWidth())
						sx=g_pGameApp->GetWindowWidth()-_roadMouseRight->GetWidth();
					if (y+_roadMouseRight->GetHeight()>g_pGameApp->GetWindowHeight())
						sy=g_pGameApp->GetWindowHeight()-_roadMouseRight->GetHeight();
					_frmQQ->PopMenu(_MasterMouseRight,sx,sy);
				}
				break;
			}
		}
	}
	else if (pSelectNode == _pTeamMgr->Find( enumTeamPrentice )->GetPointer())		// Ñ§Í½
	{
		CTeam *pTeam=_pTeamMgr->Find( enumTeamPrentice );
		for (DWORD i=0;i<pTeam->GetCount();i++)
		{
			CMember *pMember=pTeam->GetMember(i);
			if (pMember->GetPointer()==pSelectItem)
			{
				_curSelectMember=pMember;
				if (key & Mouse_LDB)
				{
					CTalkSessionFormMgr::ApplySession(&_curSelectMember);
				}
				else if (key & Mouse_RDown)
				{
					if (x+_roadMouseRight->GetWidth()>g_pGameApp->GetWindowWidth())
						sx=g_pGameApp->GetWindowWidth()-_roadMouseRight->GetWidth();
					if (y+_roadMouseRight->GetHeight()>g_pGameApp->GetWindowHeight())
						sy=g_pGameApp->GetWindowHeight()-_roadMouseRight->GetHeight();
					_frmQQ->PopMenu(_StudentMouseRight,sx,sy);
				}
				break;
			}
		}
	}
	else if (pSelectNode==_pSessionNode)
	{
		if (key & Mouse_LDB)
		{
			CTalkSessionFormMgr::OnClickOpenSession(pSelectItem);
		}
		else if (key & Mouse_RDown)
		{
			CTalkSessionForm* sessForm=static_cast<CTalkSessionForm*>(pSelectItem->GetPointer());
			if (sessForm)
			{
				_sessMouseRight->SetPointer(sessForm);
				if (x+_sessMouseRight->GetWidth()>g_pGameApp->GetWindowWidth())
					sx=g_pGameApp->GetWindowWidth()-_sessMouseRight->GetWidth();
				if (y+_sessMouseRight->GetHeight()>g_pGameApp->GetWindowHeight())
					sy=g_pGameApp->GetWindowHeight()-_sessMouseRight->GetHeight();
				_frmQQ->PopMenu(_sessMouseRight,sx,sy);
			}
		}
	}
	else if (pSelectNode==_pGuildNode)
	{
		int i=0;
		CGuildMemberData* pMemberData=NULL;
		while(pMemberData=CGuildMembersMgr::FindGuildMemberByIndex(i))
		{
			if (pMemberData->GetPointer()==pSelectItem)
			{
				if (key & Mouse_LDB)
				{
					CTalkSessionFormMgr::sApplyMember *pMember=new CTalkSessionFormMgr::sApplyMember[1];
					pMember[0].name=pMemberData->GetName();
					pMember[0].numbers=1;
					CTalkSessionFormMgr::ApplySession(pMember);
				}
				else if (key & Mouse_RDown)
				{
				}
			}
			i++;
		}
	}
	else
	{
		g_pGameApp->SysInfo("Unknow Hit! File: UIChat.cpp   Class: CChat   Code: Arcol");
	}
}

void CChat::_evtQQMainShow(CGuiData *pSender)
{
	CCharacter* pMain = CGameScene::GetMainCha();
	if( pMain )
	{
		CLabelEx* pMineName=dynamic_cast<CLabelEx*>(CFormMgr::s_Mgr.Find("frmQQ")->Find("labMineName"));
		if (pMineName)
		{
			pMineName->SetCaption(pMain->getHumanName());
		}
		else
		{
			_Error(RES_STRING(CL_LANGUAGE_MATCH_473), _frmQQ->GetName(), "labMineName");
		}
	}
}

void CChat::_OnMouseFrndMenu(CGuiData *pSender, int x, int y, DWORD key)
{
	_frndMouseRight->SetIsShow(false);
	if (!_curSelectMember) return;
	CMenuItem* pItem=_frndMouseRight->GetSelectMenu();
	if (!pItem) return;
	string str=pItem->GetString();
	if (str == RES_STRING(CL_LANGUAGE_MATCH_477))
	{
		_UpdateFrndInfo(_curSelectMember);
		CP_Frnd_Refresh_Info(_curSelectMember->GetID());
		_frmDetails->Show();
	}
	else if (str == RES_STRING(CL_LANGUAGE_MATCH_478))
	{
	 	CTalkSessionFormMgr::ApplySession(&_curSelectMember);
	}
	else if (str == RES_STRING(CL_LANGUAGE_MATCH_479))
	{
		string str = RES_STRING(CL_LANGUAGE_MATCH_480);
		str+=_curSelectMember->GetName();
		stSelectBox *pSelectBox=g_stUIBox.ShowSelectBox(_OnFrndDeleteConfirm,str.c_str(),true);
		pSelectBox->dwTag=_curSelectMember->GetID();
	}
	else if (str == RES_STRING(CL_LANGUAGE_MATCH_481))
	{
		CCozeForm::GetInstance()->OnPrivateNameSet(_curSelectMember->GetName());
	}
	_frmQQ->Refresh();
}

void CChat::_OnMouseGroupMenu(CGuiData *pSender, int x, int y, DWORD key)
{
	_groupMouseRight->SetIsShow(false);
	if (!_curSelectMember) return;
	CMenuItem* pItem = _groupMouseRight->GetSelectMenu();
	if (!pItem) return;
	string str = pItem->GetString();
	if (str == RES_STRING(CL_LANGUAGE_MATCH_482))
	{
		CS_Frnd_Invite(_curSelectMember->GetName());
	}
	else if (str == RES_STRING(CL_LANGUAGE_MATCH_478))
	{
		CTalkSessionFormMgr::ApplySession(&_curSelectMember);
	}
	else if (str == RES_STRING(CL_LANGUAGE_MATCH_483))
	{
		CS_Team_Leave();
	}
	else if (str == RES_STRING(CL_LANGUAGE_MATCH_481))
	{
		CCozeForm::GetInstance()->OnPrivateNameSet(_curSelectMember->GetName());
	}
	else if (str == RES_STRING(CL_LANGUAGE_MATCH_475))
	{
		CS_Team_Kick(_curSelectMember->GetID());
	}
	_frmQQ->Refresh();

}

void CChat::_OnMouseRoadMenu(CGuiData *pSender, int x, int y, DWORD key)
{
	_roadMouseRight->SetIsShow(false);
	if (!_curSelectMember) return;

	CMenuItem* pItem = _roadMouseRight->GetSelectMenu();
	if (!pItem) return;
	string str = pItem->GetString();
	if (str == RES_STRING(CL_LANGUAGE_MATCH_482))
	{
		CS_Frnd_Invite(_curSelectMember->GetName());
	}
	else if (str == RES_STRING(CL_LANGUAGE_MATCH_484))
	{
		CS_Team_Invite(_curSelectMember->GetName());
	}
	else if (str == RES_STRING(CL_LANGUAGE_MATCH_478))
	{
		CTalkSessionFormMgr::ApplySession(&_curSelectMember);
	}
	else if (str == RES_STRING(CL_LANGUAGE_MATCH_481))
	{
		CCozeForm::GetInstance()->OnPrivateNameSet(_curSelectMember->GetName());
	}
	_frmQQ->Refresh();
}

void CChat::_OnMouseSessMenu(CGuiData *pSender, int x, int y, DWORD key)
{
	_frmQQ->PopMenu(NULL);
	CMenuItem* pItem = _sessMouseRight->GetSelectMenu();
	if (!pItem) return;
	string str = pItem->GetString();
	if (str == RES_STRING(CL_LANGUAGE_MATCH_485))	// 密语对方
	{
		CTalkSessionFormMgr::OnClickCloseSession(static_cast<CTalkSessionForm*>(_sessMouseRight->GetPointer()));
	}

	_frmQQ->Refresh();
}

// µ¼Ê¦²Ëµ¥
void CChat::_OnMouseMasterMenu(CGuiData *pSender, int x, int y, DWORD key)
{
	_frmQQ->PopMenu(FALSE);

	CMenuItem* pItem = _MasterMouseRight->GetSelectMenu();
	if(! pItem) return;

	string strCommand = pItem->GetString();

	if (strCommand == RES_STRING(CL_LANGUAGE_MATCH_482))			// 添加好友
	{
		CCharacter* pMainCha = CGameScene::GetMainCha();
		if(pMainCha && pMainCha->getGameAttr() && pMainCha->getGameAttr()->get(ATTR_LV) >= 7)
		{
			CS_Frnd_Invite(_curSelectMember->GetName());
		}
		else
		{
			// Æß¼¶ÒÔÏÂ½ûÖ¹Ìí¼ÓºÃÓÑ
			g_pGameApp->SysInfo(RES_STRING(CL_LANGUAGE_MATCH_865));
		}
	}
	else if (strCommand == RES_STRING(CL_LANGUAGE_MATCH_484))	// 邀请组队
	{
		CCharacter* pMainCha = CGameScene::GetMainCha();
		if(pMainCha && (pMainCha->IsBoat() || (pMainCha->getGameAttr() && pMainCha->getGameAttr()->get(ATTR_LV) >= 8)))
		{
			CS_Team_Invite(_curSelectMember->GetName());
		}
		else
		{
			// °Ë¼¶ÒÔÏÂ½ûÖ¹×é¶Ó
			g_pGameApp->SysInfo(RES_STRING(CL_LANGUAGE_MATCH_866));
		}
	}
	else if (strCommand == RES_STRING(CL_LANGUAGE_MATCH_481))	// 密语对方
	{
		CCozeForm::GetInstance()->OnPrivateNameSet(_curSelectMember->GetName());
	}
	else if(strCommand == RES_STRING(CL_LANGUAGE_MATCH_478))	// 发送消息
	{
		CTalkSessionFormMgr::ApplySession(&_curSelectMember);
	}
	else if (strCommand == RES_STRING(CL_LANGUAGE_MATCH_854))	// 解除关系（徒弟解除师傅）
	{
		CCharacter* pMain = CGameScene::GetMainCha();
		if( pMain && pMain->getGameAttr() )
		{
			long nLevel = pMain->getGameAttr()->get(ATTR_LV);

			char szBuffer[256] = {0};
			_snprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE, RES_STRING(CL_LANGUAGE_MATCH_889), _curSelectMember->GetName(), nLevel * 500); // "解除师傅 %s 您将扣除\n金钱:%d\n确定吗？"

			stSelectBox* pSelectBox = g_stUIBox.ShowSelectBox(_OnMasterDeleteConfirm, szBuffer, true);
			pSelectBox->dwTag = _curSelectMember->GetID();
			pSelectBox->pointer = (void*)_curSelectMember->GetName();
		}
	}

	_frmQQ->Refresh();
}

// Ñ§Í½²Ëµ¥
void CChat::_OnMouseStudentMenu(CGuiData *pSender, int x, int y, DWORD key)
{
	_frmQQ->PopMenu(FALSE);

	CMenuItem* pItem = _StudentMouseRight->GetSelectMenu();
	if(! pItem) return;

	string strCommand = pItem->GetString();

	if (strCommand == RES_STRING(CL_LANGUAGE_MATCH_482))			// 添加好友
	{
		CS_Frnd_Invite(_curSelectMember->GetName());
	}
	else if (strCommand == RES_STRING(CL_LANGUAGE_MATCH_484))	// 邀请组队
	{
		if(_curSelectMember->GetLevel() >= 8)
		{
			CS_Team_Invite(_curSelectMember->GetName());
		}
		else
		{
			// °Ë¼¶ÒÔÏÂ½ûÖ¹×é¶Ó
			g_pGameApp->SysInfo(RES_STRING(CL_LANGUAGE_MATCH_866));
		}
	}
	else if (strCommand == RES_STRING(CL_LANGUAGE_MATCH_481))	// 密语对方
	{
		CCozeForm::GetInstance()->OnPrivateNameSet(_curSelectMember->GetName());
	}
	else if (strCommand == RES_STRING(CL_LANGUAGE_MATCH_478))	// 发送消息
	{
		CTalkSessionFormMgr::ApplySession(&_curSelectMember);
	}
	else if (strCommand == RES_STRING(CL_LANGUAGE_MATCH_854))	// 解除关系（师傅解除徒弟）
	{
		CCharacter* pMain = CGameScene::GetMainCha();
		if (pMain && pMain->getGameAttr())
		{
			//long nLevel = pMain->getGameAttr()->get(ATTR_LV);
			long nLevel = (long)pMain->getGameAttr()->get(ATTR_LV);

			char szBuffer[256] = { 0 };
			_snprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE, RES_STRING(CL_LANGUAGE_MATCH_856), _curSelectMember->GetName(), nLevel * 5); // 解除徒弟 %s 您将扣除\n金钱:%d  声望:%d\n确定吗？

			stSelectBox* pSelectBox = g_stUIBox.ShowSelectBox(_OnStudentDeleteConfirm, szBuffer, true);
			pSelectBox->dwTag = _curSelectMember->GetID();
			pSelectBox->pointer = (void*)_curSelectMember->GetName();
		}
	}

	_frmQQ->Refresh();
}


void CChat::_evtQQMainAddFrnd(CGuiData *pSender, int x, int y, DWORD key)
{
	_frmAddFriend->Refresh();
	_frmAddFriend->Show();
}

void CChat::_evtAddFrnd(CGuiData *pSender, int x, int y, DWORD key)
{
	string name=_pEditFrndName->GetCaption();
	if (!name.empty())
	{
		if ( IsValidName(name.c_str(), (unsigned short)name.length() )
			)
		{
			_frmAddFriend->Close();
			CS_Frnd_Invite(name.c_str());
		}
		else
		{
			g_pGameApp->MsgBox(RES_STRING(CL_LANGUAGE_MATCH_51));
		}
	}
	else
	{
		_frmAddFriend->Close();
	}
}

void CChat::_evtQQMainChangeStyle(CGuiData *pSender, int x, int y, DWORD key)
{
	int sx=x;
	int sy=y;
	if (x+_styMenu->GetWidth()>g_pGameApp->GetWindowWidth())
		sx=g_pGameApp->GetWindowWidth()-_styMenu->GetWidth();
	if (y+_styMenu->GetHeight()>g_pGameApp->GetWindowHeight())
		sy=g_pGameApp->GetWindowHeight()-_styMenu->GetHeight();
	_frmQQ->PopMenu(_styMenu,sx,sy);
}

void CChat::_evtChangeStyle(CGuiData *pSender, int x, int y, DWORD key)
{
	_frmQQ->PopMenu(NULL);

	CMenuItem* pItem = _styMenu->GetSelectMenu();
	if (!pItem) return;
	string str = pItem->GetString();
	if (str == RES_STRING(CL_LANGUAGE_MATCH_486))
	{
		_pTeamMgr->ChangeStyle(enumShowQQName);
	}
	else if (str == RES_STRING(CL_LANGUAGE_MATCH_487))
	{
		_pTeamMgr->ChangeStyle(enumShowQQMotto);
	}
	_frmQQ->Refresh();
}

void CChat::_evtQQMainEditMotto(CGuiData *pSender, int x, int y, DWORD key)
{
	if (!g_pGameApp->GetCurScene()) return;
	if (!g_pGameApp->GetCurScene()->GetMainCha()) return;

	_pEditMotto->SetCaption(_strSelfMottoName.c_str());
	_labNameOfMottoFrm->SetCaption(g_pGameApp->GetCurScene()->GetMainCha()->getHumanName());

	CChatIconInfo* pIconInfo = GetChatIconInfo(_dwSelfIcon);
	if (pIconInfo)
	{
		CGuiPic* pPic = _imgFaceOfMottoFrm->GetImage();
		std::string strPath = "texture/ui/HEAD/";
		pPic->LoadImage((strPath+pIconInfo->szBig).c_str(), BIG_ICON_SIZE, BIG_ICON_SIZE, 0, pIconInfo->nBigX, pIconInfo->nBigY);
	}

	_chkForbidOfMottoFrm->SetIsChecked(_bForbid);
	_frmEditMotto->Refresh();
	_frmEditMotto->Show();
}

void CChat::_evtChangeMotto(CGuiData *pSender, int x, int y, DWORD key)
{
	CTextButton* pbtnAdd = dynamic_cast<CTextButton*>(_frmEditMotto->Find("btnYes"));
	CTextButton*pbtnAdd1 = dynamic_cast<CTextButton*>(_frmEditMotto->Find("btnEgg"));
	CTextButton*pbtnAdd2 = dynamic_cast<CTextButton*>(_frmEditMotto->Find("btnFlower"));
	CTextButton*pbtnAdd3 = dynamic_cast<CTextButton*>(_frmEditMotto->Find("btnxiangce01"));
	CTextButton*pbtnAdd4 = dynamic_cast<CTextButton*>(_frmEditMotto->Find("btnxiangce02"));
	if (pSender == pbtnAdd)
	{
		stPersonInfo m_ShowInfo;
		char birthmonth[20] = "";
		char birthday[20] = "";
		char tradeGold[128] = "";
		_W64 unsigned int len = 0;
		CForm* m_pFrmSInfo = _frmEditMotto;

		CEdit* m_pEdtTradeGold = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("edtTradeGold"));

		CEdit* m_pChaSex = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaxingbie"));

		CEdit* m_pChaAge = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chanianling"));

		CEdit*m_pChaName = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaxingming"));

		CEdit*m_pAnimalZodiac = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chashengxiao"));

		CEdit*m_pBloodType = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaxuexing"));

		CEdit*m_pBirthMonth = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chashengriyue"));

		CEdit*m_pBirthDay = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chashengriri"));

		CEdit*m_pState = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chashengfen"));

		CEdit*m_pCity = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaquyu"));

		CEdit*m_pConstellation = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaxingzuo"));

		CEdit*m_pJob = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chajob"));

		CEdit*m_pIdiograph = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaqianming"));

		CCheckBox*m_pStopChat = dynamic_cast<CCheckBox*>(m_pFrmSInfo->Find("chkChat"));

		CLabel* m_pSupport = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("chaxianhua"));

		CLabel*  m_pOppose = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("chajidan"));

		len = sizeof(tradeGold) > strlen(m_pEdtTradeGold->GetCaption()) ? strlen(m_pEdtTradeGold->GetCaption()) : sizeof(tradeGold);
		memcpy(tradeGold, m_pEdtTradeGold->GetCaption(), len);

		len = sizeof(m_ShowInfo.szMotto) > strlen(m_pIdiograph->GetCaption()) ? strlen(m_pIdiograph->GetCaption()) : sizeof(m_ShowInfo.szMotto);
		memcpy(m_ShowInfo.szMotto, m_pIdiograph->GetCaption(), len);

		m_ShowInfo.bShowMotto = len > 0 ? true : false;

		len = sizeof(m_ShowInfo.szSex) > strlen(m_pChaSex->GetCaption()) ? strlen(m_pChaSex->GetCaption()) : sizeof(m_ShowInfo.szSex);
		memcpy(m_ShowInfo.szSex, m_pChaSex->GetCaption(), len);
		m_ShowInfo.szSex[9] = '\0';
		// modify by ning.yan 2008-11-18 begin
		// if(strcmp(m_ShowInfo.szSex,"男")!=0  && strcmp(m_ShowInfo.szSex,"女")!=0)
			//g_pGameApp->MsgBox("性别必须是:男或女!");
		if (strcmp(m_ShowInfo.szSex, RES_STRING(CL_LANGUAGE_MATCH_966)) != 0 && strcmp(m_ShowInfo.szSex, RES_STRING(CL_LANGUAGE_MATCH_967)) != 0)
			g_pGameApp->MsgBox(RES_STRING(CL_LANGUAGE_MATCH_968));
		// end
			//MessageBox( NULL, "性别必须是:男或女!", RES_STRING(CL_LANGUAGE_MATCH_25), MB_OK );

		m_ShowInfo.sAge = atoi(m_pChaAge->GetCaption());

		len = sizeof(m_ShowInfo.szName) > strlen(m_pChaName->GetCaption()) ? strlen(m_pChaName->GetCaption()) : sizeof(m_ShowInfo.szName);
		memcpy(m_ShowInfo.szName, m_pChaName->GetCaption(), len);

		len = sizeof(m_ShowInfo.szAnimalZodiac) > strlen(m_pAnimalZodiac->GetCaption()) ? strlen(m_pAnimalZodiac->GetCaption()) : sizeof(m_ShowInfo.szAnimalZodiac);
		memcpy(m_ShowInfo.szAnimalZodiac, m_pAnimalZodiac->GetCaption(), len);

		len = sizeof(m_ShowInfo.szBloodType) > strlen(m_pBloodType->GetCaption()) ? strlen(m_pBloodType->GetCaption()) : sizeof(m_ShowInfo.szBloodType);
		memcpy(m_ShowInfo.szBloodType, m_pBloodType->GetCaption(), len);

		len = sizeof(birthmonth) > strlen(m_pBirthMonth->GetCaption()) ? strlen(m_pBirthMonth->GetCaption()) : sizeof(birthmonth);
		memcpy(birthmonth, m_pBirthMonth->GetCaption(), len);

		len = sizeof(birthday) > strlen(m_pBirthDay->GetCaption()) ? strlen(m_pBirthDay->GetCaption()) : sizeof(birthday);
		memcpy(birthday, m_pBirthDay->GetCaption(), len);

		//m_ShowInfo.iBirthday = atoi(birthmonth)<<16 |atoi(birthday);
		m_ShowInfo.iBirthday = (atoi(birthmonth) << 5) + atoi(birthday);//Modify by sunny.sun 20080825

		len = sizeof(m_ShowInfo.szState) > strlen(m_pState->GetCaption()) ? strlen(m_pState->GetCaption()) : sizeof(m_ShowInfo.szState);
		memcpy(m_ShowInfo.szState, m_pState->GetCaption(), len);

		len = sizeof(m_ShowInfo.szCity) > strlen(m_pCity->GetCaption()) ? strlen(m_pCity->GetCaption()) : sizeof(m_ShowInfo.szCity);
		memcpy(m_ShowInfo.szCity, m_pCity->GetCaption(), len);

		len = sizeof(m_ShowInfo.szConstellation) > strlen(m_pConstellation->GetCaption()) ? strlen(m_pConstellation->GetCaption()) : sizeof(m_ShowInfo.szConstellation);
		memcpy(m_ShowInfo.szConstellation, m_pConstellation->GetCaption(), len);

		len = sizeof(m_ShowInfo.szCareer) > strlen(m_pJob->GetCaption()) ? strlen(m_pJob->GetCaption()) : sizeof(m_ShowInfo.szCareer);
		memcpy(m_ShowInfo.szCareer, m_pJob->GetCaption(), len);


		m_ShowInfo.iSupport = atoi(m_pSupport->GetCaption());
		m_ShowInfo.iOppose = atoi(m_pSupport->GetCaption());
		m_ShowInfo.iSize = 1;
		m_ShowInfo.bPprevent = m_pStopChat->GetIsChecked();
		memset(m_ShowInfo.pAvatar, 0, sizeof(m_ShowInfo.pAvatar));

		CP_Change_PersonInfo(&m_ShowInfo, tradeGold);
		_frmEditMotto->Close();

	}
	else if (pSender == pbtnAdd1)  // 鸡蛋
	{
		CLabel*  m_pOppose = dynamic_cast<CLabel*>(_frmEditMotto->Find("chajidan"));
		int content = atoi(m_pOppose->GetCaption());
		char strcontent[128] = "";
		_snprintf_s(strcontent, _countof(strcontent), _TRUNCATE, "%d", content + 1);
		m_pOppose->SetCaption(strcontent);



	}
	else if (pSender == pbtnAdd2)  // 鲜花
	{
		CLabel* m_pSupport = dynamic_cast<CLabel*>(_frmEditMotto->Find("chaxianhua"));
		int content = atoi(m_pSupport->GetCaption());
		char strcontent[128] = "";
		_snprintf_s(strcontent, _countof(strcontent), _TRUNCATE, "%d", content + 1);
		m_pSupport->SetCaption(strcontent);
	}
	else if (pSender == pbtnAdd3)
	{
		//CFormMgr& mgr = CFormMgr::s_Mgr;
		//CForm* frmPhoto = mgr.Find("frmPhoto");
		//if( !frmPhoto )
		//{
		//	assert(0);
		//	return ;
		//}
		////add begin guojiangang 20090116
		//g_stUIChat.setURLPicture(g_stUIChat._dwSelfID);
		//frmPhoto->evtMouseDragBegin = _PhotoFrmDragBegin;
		//frmPhoto->evtMouseDragEnd = _PhotoFrmDragEnd;
		//frmPhoto->evtShow = _webPhotoFrmShow;
		//frmPhoto->evtHide = _webPhotoFrmHide;
		//frmPhoto->ShowModal();
		////end begin guojiangang 20090116
	}
}

void CChat::_evtRefreshInfo(CGuiData *pSender, int x, int y, DWORD key)
{
	CP_Frnd_Refresh_Info(_frmDetails->nTag);
}

bool CChat::_UpdateFrndInfo(CMember *pMember)
{
	if (!pMember) return false;
	
	CLabelEx* pDeName = dynamic_cast<CLabelEx*>(_frmDetails->Find("labDeName"));
	if (pDeName)
	{
		pDeName->SetCaption(pMember->GetName());
	}
	else
	{
		_Error(RES_STRING(CL_LANGUAGE_MATCH_473), _frmDetails->GetName(), "labDeName");
	}

	CLabelEx* pJob = dynamic_cast<CLabelEx*>(_frmDetails->Find("labJob"));
	if (pJob)
	{
		pJob->SetCaption(pMember->GetJobName());
	}
	else
	{
		_Error(RES_STRING(CL_LANGUAGE_MATCH_473), _frmDetails->GetName(), "labJob");
	}

	CLabelEx* pLv = dynamic_cast<CLabelEx*>(_frmDetails->Find("labLv"));
	if (pLv)
	{
		char buf[32] = {0};
		sprintf(buf, "%d", pMember->GetLevel());
		pLv->SetCaption(buf);
	}
	else
	{
		_Error(RES_STRING(CL_LANGUAGE_MATCH_473), _frmDetails->GetName(), "labLv");
	}

	CLabelEx* pConsortia = dynamic_cast<CLabelEx*>(_frmDetails->Find("labConsortia"));
	if (pConsortia)
	{
		pConsortia->SetCaption(pMember->GetGuildName());
	}

	CLabelEx* pName = dynamic_cast<CLabelEx*>(_frmDetails->Find("labName"));
	if (!pName)
	{
		_Error(RES_STRING(CL_LANGUAGE_MATCH_473), _frmDetails->GetName(), "labName");
	}

	CImage* pDeHead = dynamic_cast<CImage*>(_frmDetails->Find("imgDeHead"));
	if (pDeHead)
	{
		CChatIconInfo* pIconInfo = GetChatIconInfo(pMember->GetIconID());
		if (pIconInfo)
		{
			CGuiPic* pPic = pDeHead->GetImage();
			std::string strPath = "texture/ui/HEAD/";
			pPic->LoadImage((strPath+pIconInfo->szBig).c_str(), BIG_ICON_SIZE, BIG_ICON_SIZE, 0, pIconInfo->nBigX, pIconInfo->nBigY);
		}
	}
	else
	{
		_Error(RES_STRING(CL_LANGUAGE_MATCH_473), _frmDetails->GetName(), "imgDeHead");
	}

	_frmDetails->nTag = pMember->GetID();
	_frmDetails->Refresh();

	return true;
}

bool CChat::_UpdateSelfInfo()
{
	CForm* frmQQ = CFormMgr::s_Mgr.Find("frmQQ", 1);
	CLabelEx* pMineName = dynamic_cast<CLabelEx*>(frmQQ->Find("labMineName"));
	if (pMineName)
	{
		pMineName->SetHint(_strSelfMottoName.c_str());
	}
	else
	{
		_Error(RES_STRING(CL_LANGUAGE_MATCH_473), _frmQQ->GetName(), "labMineName");
	}

	CImage* pImage = dynamic_cast<CImage*>(_frmQQ->Find("imgHead"));
	if (pImage)
	{
		CChatIconInfo* pIconInfo = GetChatIconInfo(_dwSelfIcon);
		if (pIconInfo)
		{
			CGuiPic* pPic = pImage->GetImage();
			std::string strPath = "texture/ui/HEAD/";
			pPic->LoadImage((strPath+pIconInfo->szBig).c_str(), BIG_ICON_SIZE, BIG_ICON_SIZE, 0, pIconInfo->nBigX, pIconInfo->nBigY);
		}
	}
	else
	{
		_Error(RES_STRING(CL_LANGUAGE_MATCH_473), _frmQQ->GetName(), "imgHead");
	}

	return true;

}

void CChat::_OnTimerFlash(CGuiTime *pSender)
{
	CTextButton *pBtn=CStartMgr::GetShowQQButton();
	if (!pBtn) return;
	if (CTalkSessionFormMgr::hasFlashSession())
	{
		if (!_frmQQ->GetIsShow())
		{
			static int i=0;
			pBtn->GetImage()->SetFrame(i);
			i=(i==0)?3:0;
		}
		else
		{
			pBtn->GetImage()->SetFrame(0);
		}
		if (!_pSessionNode->GetIsExpand())
		{
			static bool bColor=false;
			_pSessionNode->GetItem()->SetColor((bColor)?0xff00ff00:COLOR_BLACK);
			bColor=!bColor;
		}
		else
		{
			_pSessionNode->GetItem()->SetColor(COLOR_BLACK);
		}
	}
	else
	{
		pBtn->GetImage()->SetFrame(0);
		_pSessionNode->GetItem()->SetColor(COLOR_BLACK);
	}
	CTalkSessionFormMgr::OnFlashSession();
}

void CChat::_OnDragEnd(CForm* pTargetForm, CTreeView *pTree, CTreeNodeObj* pNode, CItemObj* pItem, int x, int y, DWORD key  )
{
	CTalkSessionForm* pSessForm;
	pSessForm=CTalkSessionFormMgr::GetSessionFormByForm(pTargetForm);
	if (!pSessForm) return;
	string strCaption=pNode->GetCaption();
	if (strCaption== RES_STRING(CL_LANGUAGE_MATCH_465)) return;
	CTextGraph* pText=dynamic_cast<CTextGraph*>(pItem);
	if (pText)
	{
		CMember* pMember=static_cast<CMember*>(pText->GetPointer());
		if (!pMember) return;
		if (pSessForm->IsActiveSession())
		{
			CS_Sess_Add(pSessForm->GetSessionID(),pMember->GetName());
		}
		else
		{
			if (string(pMember->GetName())==pSessForm->GetMemberByIndex(0)->GetName())
			{
				g_pGameApp->SysInfo(RES_STRING(CL_LANGUAGE_MATCH_488));
				return;
			}
			pSessForm->AddMemberToBuffer(pMember->GetName());
			const char **pArrName=new const char *[1];
			pArrName[0]=pSessForm->GetMemberByIndex(0)->GetName();
			CS_Sess_Create(pArrName,1);
			SAFE_DELETE_ARRAY(pArrName);
		}
	}
	else
	{
		CTreeGridNode* pGridNode = dynamic_cast<CTreeGridNode*>(pNode);
		if (!pGridNode) return;
		int nCount=0;
		for (DWORD i=0;i<pGridNode->GetItemCount();i++)
		{
			pText=dynamic_cast<CTextGraph*>(pGridNode->GetItemByIndex(i));
			if (!pText) continue;
			CMember* pMember=static_cast<CMember*>(pText->GetPointer());
			if (!pMember) continue;
			string strName=pMember->GetName();
			if (pSessForm->hasMember(strName)) continue;
			nCount++;
			if (pSessForm->IsActiveSession())
			{
				CS_Sess_Add(pSessForm->GetSessionID(),strName.c_str());
			}
			else
			{
				pSessForm->AddMemberToBuffer(strName);
			}
		}
		if (nCount>0 && !pSessForm->IsActiveSession())
		{
			const char **pArrName=new const char *[1];
			pArrName[0]=pSessForm->GetMemberByIndex(0)->GetName();
			CS_Sess_Create(pArrName,1);
			SAFE_DELETE_ARRAY(pArrName);
		}
	}
}

void CChat::_OnFrndDeleteConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
{
	if( nMsgType!=CForm::mrYes ) return;

	stSelectBox* pSelect=static_cast<stSelectBox*>(pSender->GetForm()->GetPointer());
	if (pSelect)
	{
		CS_Frnd_Delete(pSelect->dwTag);
	}
}


void CChat::_OnMasterDeleteConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
{
	if( nMsgType != CForm::mrYes ) return;

	stSelectBox* pSelect=static_cast<stSelectBox*>(pSender->GetForm()->GetPointer());
	if (pSelect && pSelect->pointer)
	{
		CS_MasterDel((const char*)pSelect->pointer, pSelect->dwTag);
	}
}

void CChat::_OnStudentDeleteConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
{
	if( nMsgType != CForm::mrYes ) return;

	stSelectBox* pSelect = static_cast<stSelectBox*>(pSender->GetForm()->GetPointer());
	if (pSelect && pSelect->pointer)
	{
		CS_PrenticeDel((const char*)pSelect->pointer, pSelect->dwTag);
	}
}


void CChat::MasterAsk(const char* szName, DWORD dwCharID)
{
	char szBuffer[512] = {0};
	sprintf(szBuffer, RES_STRING(CL_LANGUAGE_MATCH_860), szName);

	stSelectBox *pSelectBox = g_stUIBox.ShowSelectBox(_OnStudentAskConfirm, szBuffer, true);
	pSelectBox->pointer = (void*)szName;
	pSelectBox->dwTag	= dwCharID;
}

void CChat::_OnStudentAskConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
{
	stSelectBox* pSelect = static_cast<stSelectBox*>(pSender->GetForm()->GetPointer());
	if (pSelect && pSelect->pointer)
	{
		CS_MasterAsr(nMsgType == CForm::mrYes, (const char*)pSelect->pointer, pSelect->dwTag);
	}
}


void CChat::PrenticeAsk(const char* szName, DWORD dwCharID)
{
	char szBuffer[512] = {0};
	sprintf(szBuffer, RES_STRING(CL_LANGUAGE_MATCH_861), szName);

	stSelectBox *pSelectBox = g_stUIBox.ShowSelectBox(_OnMasterAskConfirm, szBuffer, true);
	pSelectBox->pointer = (void*)szName;
	pSelectBox->dwTag	= dwCharID;
}

void CChat::_OnMasterAskConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
{
	stSelectBox* pSelect = static_cast<stSelectBox*>(pSender->GetForm()->GetPointer());
	if (pSelect && pSelect->pointer)
	{
		CS_PrenticeAsr(nMsgType == CForm::mrYes, (const char*)pSelect->pointer, pSelect->dwTag);
	}
}
/*
bool CChat::_UpdateSelfInfo()
{
	char  pszCha[256] = { 0 };
	string  strGuildName = "";
	char  ChCharaterName[256] = "";
	char  ChCharaterNickName[256] = "";   //称号
	char  ChCharaterJob[256] = "";
	char  ChCharaterLev[256] = "";
	char  ChCharaterFame[256] = "";

	CCharacter* pCha = g_stUIBoat.GetHuman();
	if (!pCha)			return false;

	SGameAttr* pCChaAttr = pCha->getGameAttr();
	if (!pCChaAttr)	return false;

	//玩家名称
	_snprintf_s(ChCharaterName, _countof(ChCharaterName), _TRUNCATE, "%s", pCha->getName());

	_snprintf_s(ChCharaterNickName, _countof(ChCharaterNickName), _TRUNCATE, "%s", pCChaAttr->get(ATTR_TITLE));

	//公会
	if (CGuildData::GetGuildID())
	{
		strGuildName = CGuildData::GetGuildName().c_str();
	}
	if (strGuildName == "")
	{
		//strGuildName="[无]";// ning.yan modify
		strGuildName = RES_STRING(CL_LANGUAGE_MATCH_969);
	}
	//玩家职业
	_snprintf_s(ChCharaterJob, _countof(ChCharaterJob), _TRUNCATE, "%s", g_GetJobName((short)pCChaAttr->get(ATTR_JOB)));

	//玩家等级
	_snprintf_s(ChCharaterLev, _countof(ChCharaterLev), _TRUNCATE, "%lld", pCChaAttr->get(ATTR_LV));

	// 声望
	_snprintf_s(ChCharaterFame, _countof(ChCharaterFame), _TRUNCATE, "%lld", pCChaAttr->get(ATTR_FAME));

	CForm* m_pFrmSInfo = _frmEditMotto;

	CLabel* m_pLabName = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("labName"));
	CLabel* m_pChaguild = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("chaguild"));
	CLabel* m_pChaLv = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("chaLv"));
	CLabel* m_pChaJob = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("chazhiye"));
	CLabel* m_pReputation = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("chashengwang"));
	CLabel* m_pChaNickName = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("chachenghao"));
	CImage* m_pChaPhoto = dynamic_cast<CImage*>(m_pFrmSInfo->Find("imgMhead"));
	CEdit* m_pEdtTradeGold = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("edtTradeGold"));

	m_pEdtTradeGold->SetCaption(pCha->getSecondName());
	m_pLabName->SetCaption(ChCharaterName);
	m_pChaguild->SetCaption(strGuildName.c_str());
	m_pChaLv->SetCaption(ChCharaterLev);
	m_pChaJob->SetCaption(ChCharaterJob);
	// 声望不知道是什么
	m_pReputation->SetCaption(ChCharaterFame);

	m_pChaNickName->SetCaption(ChCharaterNickName);

	m_pChaPhoto->GetImage()->UnLoadImage();

	// 清空数据
	CLabel* m_pAttention = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("chaguanzhu"));
	m_pAttention->SetCaption("");

	CEdit* m_pChaSex = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaxingbie"));
	m_pChaSex->SetCaption("");

	CEdit* m_pChaAge = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chanianling"));
	m_pChaAge->SetCaption("");

	CEdit*m_pChaName = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaxingming"));
	m_pChaName->SetCaption("");

	CEdit*m_pAnimalZodiac = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chashengxiao"));
	m_pAnimalZodiac->SetCaption("");

	CEdit*m_pBloodType = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaxuexing"));
	m_pBloodType->SetCaption("");

	CEdit*m_pBirthMonth = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chashengriyue"));
	m_pBirthMonth->SetCaption("");
	CEdit*m_pBirthDay = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chashengriri"));
	m_pBirthDay->SetCaption("");
	CEdit*m_pState = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chashengfen"));
	m_pState->SetCaption("");
	CEdit*m_pCity = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaquyu"));
	m_pCity->SetCaption("");
	CEdit*m_pConstellation = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaxingzuo"));
	m_pConstellation->SetCaption("");
	CEdit*m_pJob = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chajob"));
	m_pJob->SetCaption("");
	CEdit*m_pIdiograph = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaqianming"));
	m_pIdiograph->SetCaption("");
	CCheckBox*m_pStopChat = dynamic_cast<CCheckBox*>(m_pFrmSInfo->Find("chkChat"));

	CLabel* m_pSupport = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("chaxianhua"));
	m_pSupport->SetCaption("");
	CLabel*  m_pOppose = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("chajidan"));
	m_pOppose->SetCaption("");
	return true;

}*/

void CChat::CaptainConfirm(DWORD dwTeamID)
{
	stMsgTimeBox *pBox = g_stUIBox.ShowMsgTime(_OnCaptainAskConfirm, RES_STRING(CL_UI_CHAT_CPP_00001), 5);
	//pBox->teamID= dwTeamID;
	pBox->teamID = (short)dwTeamID;
}
void CChat::_OnCaptainAskConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
{
	stMsgTimeBox* pBox = static_cast<stMsgTimeBox*>(pSender->GetForm()->GetPointer());
	if (pBox)
	{
		pBox->frmDialog->Close();
		CS_CaptainConfirmAsr(true, pBox->teamID);
	}
}

void CChat::ChangeFrndGroup(const char* szOldGroupName, const char* szNewGroupName)
{
	if (strcmp("Friend", szOldGroupName) == 0)
	{
		return;
	}

	for (vector< pair<string, CTreeGridNode*> >::iterator it = vFrndNode.begin(); it != vFrndNode.end(); it++)
	{
		if (strcmp(it->first.c_str(), szOldGroupName) == 0)
		{
			if (m_pQQTreeView)
			{
				it->first = szNewGroupName;
				it->second->SetCaption(szNewGroupName);
				it->second->RefreshNode();
			}
			break;
		}
	}

	if (m_pQQTreeView)
		m_pQQTreeView->Refresh();
}

void CChat::DelFrndGroup(const char* szGroupName)
{
	if (strcmp("Friend", szGroupName) == 0)
	{
		_pFrndNode->Clear();
		if (_pFrndNode->GetIsExpand())
			_pFrndNode->RefreshNode();

		return;
	}

	for (vector< pair<string, CTreeGridNode*> >::iterator it = vFrndNode.begin(); it != vFrndNode.end(); it++)
	{
		if (strcmp(it->first.c_str(), szGroupName) == 0)
		{
			if (m_pQQTreeView)
			{
				CTreeNodeObj* parent = m_pQQTreeView->GetRootNode();
				if (parent)
				{
					it->second->ClearAllChild();
					if (it->second->GetIsExpand())
						it->second->RefreshNode();

					parent->DelNode(it->second);
				}
			}

			vFrndNode.erase(it);
			break;
		}
	}

	if (m_pQQTreeView)
		m_pQQTreeView->Refresh();
}

void CChat::ResetSelfInfo(const stPersonInfo *_info)
{
	// _info不可能为NULL，因为外边是引用传过来的
	// 更新自己的信息
	if (!_info)
		stPersonInfo *_info = &_SelfInfo;
	else
		_SelfInfo = *_info;
	// 更新基本属性
	_UpdateSelfInfo();

	char birthmonth[20] = "";
	char birthday[20] = "";
	char tradeGold[128] = "";
	_W64 unsigned int len = 0;

	CForm* m_pFrmSInfo = _frmEditMotto;

	CEdit* m_pEdtTradeGold = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("edtTradeGold"));

	CLabel* m_pAttention = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("chaguanzhu"));

	CEdit* m_pChaSex = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaxingbie"));

	CEdit* m_pChaAge = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chanianling"));

	CEdit*m_pChaName = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaxingming"));

	CEdit*m_pAnimalZodiac = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chashengxiao"));

	CEdit*m_pBloodType = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaxuexing"));

	CEdit*m_pBirthMonth = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chashengriyue"));

	CEdit*m_pBirthDay = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chashengriri"));

	CEdit*m_pState = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chashengfen"));

	CEdit*m_pCity = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaquyu"));

	CEdit*m_pConstellation = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaxingzuo"));

	CEdit*m_pJob = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chajob"));

	CEdit*m_pIdiograph = dynamic_cast<CEdit*>(m_pFrmSInfo->Find("chaqianming"));

	CCheckBox*m_pStopChat = dynamic_cast<CCheckBox*>(m_pFrmSInfo->Find("chkChat"));

	CLabel* m_pSupport = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("chaxianhua"));

	CLabel*  m_pOppose = dynamic_cast<CLabel*>(m_pFrmSInfo->Find("chajidan"));

	char buff[64] = "";

	//m_pEdtTradeGold->SetCaption(_info->szMotto);

	_snprintf_s(buff, _countof(buff), _TRUNCATE, "%d", _info->iSupport);
	m_pSupport->SetCaption(buff);
	_snprintf_s(buff, _countof(buff), _TRUNCATE, "%d", _info->iOppose);
	m_pOppose->SetCaption(buff);

	// 关注
	//srand(time(NULL));
	srand((unsigned int)time(NULL));
	_snprintf_s(buff, _countof(buff), _TRUNCATE, "%d", rand() % 1000);
	m_pAttention->SetCaption(buff);

	m_pChaSex->SetCaption(_info->szSex);
	_snprintf_s(buff, _countof(buff), _TRUNCATE, "%d", _info->sAge);
	m_pChaAge->SetCaption(buff);
	m_pChaName->SetCaption(_info->szName);
	m_pAnimalZodiac->SetCaption(_info->szAnimalZodiac);
	m_pBloodType->SetCaption(_info->szBloodType);

	//_snprintf_s( buff, _countof( buff ), _TRUNCATE, "%d",(_info->iBirthday &&0xffff0000));
	//m_pBirthMonth->SetCaption(buff);
	//_snprintf_s( buff, _countof( buff ), _TRUNCATE, "%d",(_info->iBirthday &&0xffff));
	//Modify by sunny.sun 20080825
	_snprintf_s(buff, _countof(buff), _TRUNCATE, "%d", (_info->iBirthday >> 5));
	m_pBirthMonth->SetCaption(buff);
	_snprintf_s(buff, _countof(buff), _TRUNCATE, "%d", (_info->iBirthday - (_info->iBirthday >> 5 << 5)));
	m_pBirthDay->SetCaption(buff);

	m_pState->SetCaption(_info->szState);
	m_pCity->SetCaption(_info->szCity);
	m_pConstellation->SetCaption(_info->szConstellation);
	m_pJob->SetCaption(_info->szCareer);
	m_pIdiograph->SetCaption(_info->szMotto);

	//if(_info->bShowMotto)
	//	m_pStopChat->SetCaption("有");
	//else 
	//	m_pStopChat->SetCaption("没");


	CImage* m_pChaPhoto = dynamic_cast<CImage*>(m_pFrmSInfo->Find("imgMhead"));
	CChatIconInfo *pIconInfo = GetChatIconInfo(g_stUIChat._dwSelfIcon);
	if (pIconInfo)
	{
		CGuiPic* pPic = m_pChaPhoto->GetImage();
		string strPath = "texture/ui/HEAD/";
		//if(strcmp(_info->szSex,"女")==0)
		if (strcmp(_info->szSex, RES_STRING(CL_LANGUAGE_MATCH_967)) == 0) // ning.yan modify
			pPic->LoadImage((strPath + "1.bmp").c_str(), 100, 100, 0, pIconInfo->nBigX, pIconInfo->nBigY);
		else
			pPic->LoadImage((strPath + "2.bmp").c_str(), 100, 100, 0, pIconInfo->nBigX, pIconInfo->nBigY);

		//pPic->LoadImage((strPath+pIconInfo->szBig).c_str(),BIG_ICON_SIZE,BIG_ICON_SIZE,0,pIconInfo->nBigX,pIconInfo->nBigY);
	}

	return;
}