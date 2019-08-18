#include "stdafx.h"
#include "LoginScene.h"

#include "GameApp.h"
#include "Character.h"
#include "SceneObj.h"
#include "UiFormMgr.h"
#include "UiTextButton.h"
#include "CharacterAction.h"
#include "SceneItem.h"
#include "ItemRecord.h"
#include "PacketCmd.h"
#include "GameConfig.h"

#include "Character.h"
#include "caLua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "UIRender.h"
#include "UIEdit.h"
#include "UILabel.h"
#include "uiformmgr.h"
#include "uitextbutton.h"
#include "uilabel.h"
#include "uiprogressbar.h"
#include "uiscroll.h"
#include "uilist.h"
#include "uicombo.h"
#include "uiimage.h"
#include "UICheckBox.h"
#include "uiimeinput.h"
#include "uigrid.h"
#include "uilistview.h"
#include "uipage.h"
#include "uitreeview.h"
#include "uiimage.h"
#include "UILabel.h"
#include "RenderStateMgr.h"
#include "cameractrl.h"
#include "UIListView.h"

#include "UIMemo.h"
#include "caLua.h"

#include "Connection.h"
#include "ServerSet.h"
#include "GameAppMsg.h"


#include "UI3DCompent.h"
#include "UIForm.h"
#include "UITemplete.h"
#include "commfunc.h"
#include "uiboxform.h"
#include "bill.h"

#include <shellapi.h>

//#define USE_STATUS		// ��Э����󲨵������г�ͻ���ر� lh by 2006-3-22

#ifdef USE_STATUS
#include "UdpClient.h"

using namespace client_udp;
#endif

#pragma comment( lib, "shell32.lib" )

#include "xmlwriter.h"

//#ifdef KOP_TOM.
TOM_SERVER	g_TomServer;
//#endif
Cooperate   g_cooperate;    //  add by jampe
 

int CLoginScene::nSelectChaType  = 0; // ����ѡ�н�ɫ����Ϣ
int CLoginScene::nSelectChaPart[enumEQUIP_PART_NUM] = { 0 };

CForm*			CLoginScene::frmServer   = NULL;
CForm*			CLoginScene::frmRegion   = NULL;
CForm*			CLoginScene::frmAccount  = NULL;
CForm*          CLoginScene::frmLOGO     = NULL;
CForm*          CLoginScene::frmAnnounce = NULL;
CForm*          CLoginScene::frmKeyboard = NULL;	// add by Philip.Wu  ����̽���  2006-06-05
CForm*          CLoginScene::frmPathLogo = NULL;

CCheckBox*      CLoginScene::chkID       = NULL;

CList*			CLoginScene::lstRegion[NUM_REGIN_LIST];

CEdit*			CLoginScene::edtID       = NULL;
CEdit*          CLoginScene::edtPassword = NULL;
CEdit*			CLoginScene::edtFocus    = NULL;	// add by Philip.Wu  ��꼤��ı༭��  2006-06-06
CCheckBox*      CLoginScene::chkShift    = NULL;	// add by Philip.Wu  ������ϵ� Shift  2006-06-09

CImage*         CLoginScene::imgLogo1    = NULL;
CImage*         CLoginScene::imgLogo2    = NULL;


//static			CCharacterBuilder* __character_scene = 0;
static			CCharacter*  _pCntCha[3] = { 0 };
static			CCharacter*  pPxCha[2] = { 0 };  //��½������3���з
static			CCharacter * _pSelectCha = 0;

static	int		iSelectIndex = 0;				//�����е�3D�ؼ������
static	int		iRotateCha      = 360;			// �ڴ�����ɫ��ʱ�򣬵�����Ұ�ť��ת
static	int		iMiniRotateCha  = 360;

static CLoginScene* g_login_scene = 0;

static void	_GoBack(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
{
	CLoginScene* pLogin = dynamic_cast<CLoginScene*>(CGameApp::GetCurScene());
	if( pLogin )
	{
		if( g_cooperate.code )
		{
			pLogin->InitRegionList();
			pLogin->frmRegion->SetIsShow( true );
		}
		else
		{
			g_pGameApp->m_bPlayFlash = TRUE;
			pLogin->UpdateBounds();
			pLogin->GotoFrame( 1 );

		}
	}
}

#define MAX_SERVER_NUM	8

extern char* GetFlashDir( int UpLayers );

CFlashPlayer* GFlashPlayer = NULL;

CLoginScene::CLoginScene(stSceneInitParam& param):
	CGameScene(param), 
	CFlashPlayer(),
	_eState(enumInit), 
    m_bPasswordError(false), 
	m_sPassport("nobill"), 
	m_sUsername(""), 
	m_sPassword("")
{
	for( int index = 0; index < NUM_REGIN_LIST; index ++ )
		lstRegion[index] = NULL;

//	LG( "scene memory", "CLoginScene Create\n" );
	_loadtex_flag = 9; _loadmesh_flag = 9;

	GFlashPlayer = this;
	g_pGameApp->m_bPlayFlash = FALSE;
	m_iCurSelRegionIndex = 1;
	m_iCurSelServerIndex = 0;
}

CLoginScene::~CLoginScene()
{
//	LG( "scene memory", "CLoginScene Destroy\n" );

	for (int i =0 ; i<3 ; i++)
		_pCntCha[i] = NULL ;
	_pSelectCha = NULL;
}

bool CLoginScene::_Init()
{
    g_login_scene = this;

    _IsUseSound = false;

    _eState = enumInit;

    { // save loading res mt flag
        drIByteSet* res_bs = g_Render.GetInterfaceMgr()->res_mgr->GetByteSet();
        _loadtex_flag = res_bs->GetValue(OPT_RESMGR_LOADTEXTURE_MT);
        _loadmesh_flag = res_bs->GetValue(OPT_RESMGR_LOADMESH_MT);
        res_bs->SetValue(OPT_RESMGR_LOADTEXTURE_MT, 0);
        res_bs->SetValue(OPT_RESMGR_LOADMESH_MT, 0);
    }

	CGameScene::_Init();

	SetFocus( g_pGameApp->GetHWND()) ;

	SetFocus( g_pGameApp->GetHWND()) ;

	m_sUsername = "player";
	m_sPassword = "";

	if (!_InitUI())
	{
		LG("login_ini", RES_STRING(CL_LANGUAGE_MATCH_168));

		return false;
	}

	CFormMgr::s_Mgr.SetEnabled(TRUE);    
   	_pMainCha = NULL;

	// ɾ���Զ����³���ĸ���
	char szUpdateFileName[] = "_Update.exe";
	SetFileAttributes(szUpdateFileName, FILE_ATTRIBUTE_NORMAL);
  	DeleteFile(szUpdateFileName);

	PlayWhalePose();

	CFlashPlayer::Init();
	CreateBackgroundSurface( "Background.bmp" );

	Activate( TRUE );

	static bool isFirst = true;
	if (g_TomServer.bEnable && isFirst )
	{
		isFirst = false;
		m_sUsername = g_TomServer.szUser;
		m_sPassword = g_TomServer.szPassword;
		m_sPassport = g_TomServer.szPassport;
		_Connect();
		return true;
	}


	return true;
}

void CLoginScene::PlayWhalePose()
{
}

bool CLoginScene::_Clear()
{
    CGameScene::_Clear();

//    g_Render.SetRenderState(D3DRS_FOGENABLE, 0);
    { // reset loading res mt flag
        if(_loadtex_flag != 9 && _loadmesh_flag != 9)
        {
            drIByteSet* res_bs = g_Render.GetInterfaceMgr()->res_mgr->GetByteSet();
            res_bs->SetValue(OPT_RESMGR_LOADTEXTURE_MT, _loadtex_flag);
            res_bs->SetValue(OPT_RESMGR_LOADMESH_MT, _loadmesh_flag);
        }
    }

    g_Render.SetClip(1.F, 1000.0f);

	Activate( FALSE );

    return 1;
}

void CLoginScene::ShowLoginForm()
{
	chkID->SetIsChecked( m_bSaveAccount );
	edtID->SetCaption( m_sSaveAccount.c_str());
	edtPassword->SetCaption("");
	frmAccount->Show();
	
	// add by Philip.Wu  2006-07-03  ��ʾ�����ʱͬʱ��ʾ�����
	frmKeyboard->SetIsShow(true);
	imgLogo1->SetIsShow(false);
	imgLogo2->SetIsShow(false);

	if (m_sSaveAccount == "")
	{
		edtID->SetActive(edtID);
	}
	else
	{
		edtPassword->SetActive(edtPassword);
	}
}

void CLoginScene::_FrameMove(DWORD dwTimeParam)
{
	if( g_pGameApp->m_bPlayFlash )
		Update();

    //CGameScene::_FrameMove(dwTimeParam);
	int x = g_pGameApp->GetMouseX();
	int y = g_pGameApp->GetMouseY();
	GetRender().ScreenConvert( x, y );

	if (frmServer->GetIsShow())
	{
//#ifdef USE_STATUS
//		CLoginScene* pkScene = dynamic_cast<CLoginScene*>(CGameApp::GetCurScene());
//		if (!pkScene) return;
//
//		GetRegionMgr()->FrameMove( dwTimeParam );
//
//		int iRegionIndex = pkScene->GetCurSelRegionIndex();
//		const char* szServerGroupName;
//		CGraph* pGraph(0);
//		int iServIconIndex(0);
//
//		static int nFlash = -5;
//		nFlash++;
//		bool IsFlash = false;
//		if( nFlash>=5 ) 
//		{
//			nFlash=-5;
//		}
//		for( int i = 0; i< GetCurServerGroupCnt(iRegionIndex); i++ )
//		{
//			szServerGroupName = GetCurServerGroupName(iRegionIndex, i);
//			iServIconIndex = pkScene->GetServIconIndex( GetRegionMgr()->GetGroupNum(szServerGroupName) );
//
//			if (i%2 == 0)
//				pGraph = dynamic_cast<CGraph*>(lvServer0->GetItemObj(i/2, IMAGE_INDEX));
//			else
//				pGraph = dynamic_cast<CGraph*>(lvServer1->GetItemObj(i/2, IMAGE_INDEX));
//			if ( pGraph)
//				pGraph->GetImage()->SetFrame(iServIconIndex);
//
//			if( nFlash>=0 )
//				pGraph->GetImage()->SetAlpha( 150 );
//			else
//				pGraph->GetImage()->SetAlpha( 255 );
//		}
//#endif
	}

	if (frmRegion->GetIsShow())
	{
		for( int index = 0; index < NUM_REGIN_LIST; index ++ )
		{
			if( !lstRegion[index]->InRect(x, y) )
				lstRegion[index]->GetItems()->GetSelect()->SetNoSelect();
		}
	}

	if( _eState==enumConnect )
    {
        switch( g_NetIF->GetConnStat() )
        {
		case Connection::CNST_CONNECTING:
			return ;
        case Connection::CNST_INVALID:
        case Connection::CNST_FAILURE: 
            {
				if (g_TomServer.bEnable)
				{
					MessageBox( 0, RES_STRING(CL_LANGUAGE_MATCH_169), "Info", 0 );
					g_pGameApp->SetIsRun( false );
					return;
				}

                // ���ص�ѡ�����������
	             _eState = enumInit;
                CGameApp::Waiting(false);

				ShowKeyboard(false);
				frmRegion->SetIsShow(false);
				frmAccount->SetIsShow(false);
				g_stUIBox.ShowMsgBox( _GoBack, RES_STRING(CL_LANGUAGE_MATCH_169) );
            }
            return;
        case Connection::CNST_CONNECTED:
            CGameApp::Waiting(false);

			//���ӳɹ�,���͵�½��Ϣ
			_Login();
			return;
		case Connection::CNST_TIMEOUT:
            _eState = enumInit;
			g_pGameApp->SendMessage(APP_NET_DISCONNECT,1000);
			return;
        }
        return;
    }
}

void CLoginScene::_Render()
{	
	if( g_pGameApp->m_bPlayFlash )
	{
		CFlashPlayer::Render();
		return;
	}

	static bool IsLoad = false;
	static CGuiPic LoginPic;
	if( !IsLoad )
	{
		LoginPic.LoadImage( "texture/ui/new_login.jpg", 1024, 768, 0, 0, 0, 1.F, 1.F );
		IsLoad = true;
	}
	LoginPic.SetScale( 0, GetRender().GetScreenWidth(), GetRender().GetScreenHeight() );
	LoginPic.Render( 0, 0 );

#ifdef USE_STATUS
	if (frmServer->GetIsShow())
	{
		CGraph* pGraph = NULL;
		CItemRow* pRow = NULL;
		if( pRow = lvServer0->GetList()->GetItems()->GetSelect()->GetItem() )
		{
			pGraph = dynamic_cast<CGraph*>( pRow->GetBegin() );
		}
		else if( pRow = lvServer1->GetList()->GetItems()->GetSelect()->GetItem() )
		{
			pGraph = dynamic_cast<CGraph*>( pRow->GetBegin() );
		}		
		if( pGraph )
		{
			pGraph->GetImage()->SetAlpha( 255 );
			int n = pGraph->GetImage()->GetFrame();
			char szBuf[256] = { 0 };
			switch( n )
			{
			case 0: 
				_snprintf_s( szBuf, _countof( szBuf ), _TRUNCATE,  RES_STRING(CL_LANGUAGE_MATCH_170), pRow->GetIndex(1)->GetString() );
				break;
			case 1:
				_snprintf_s( szBuf, _countof( szBuf ), _TRUNCATE,  RES_STRING(CL_LANGUAGE_MATCH_171), pRow->GetIndex(1)->GetString() );
				break;
			case 2: 
				_snprintf_s( szBuf, _countof( szBuf ), _TRUNCATE,  RES_STRING(CL_LANGUAGE_MATCH_172), pRow->GetIndex(1)->GetString() );
				break;
			case 3:
				_snprintf_s( szBuf, _countof( szBuf ), _TRUNCATE,  RES_STRING(CL_LANGUAGE_MATCH_173), pRow->GetIndex(1)->GetString() );
				break;
			}
			
			g_pGameApp->ShowHint( g_pGameApp->GetMouseX(), g_pGameApp->GetMouseY(), szBuf, COLOR_GREEN );
		}
	}
#endif
}


void CLoginScene::LoadingCall()
{
	g_pGameApp->PlayMusic( 1 );
}

// Xu Qin Added for Flash Control
BOOL CLoginScene::FlashProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( g_pGameApp->m_bPlayFlash )
	{
		MessageProc( message, wParam, lParam );
		return TRUE;
	}

	return FALSE;
}

VOID CLoginScene::FSCommand( char* szParam1, char* szParam2 )
{
	CLoginScene* pScene = dynamic_cast<CLoginScene*>( CGameApp::GetCurScene() );
	//	if( szParam1 && stricmp( szParam1, "return" ) == 0 )
	if( szParam1 && _stricmp( szParam1, "return" ) == 0 )
	{
		g_pGameApp->m_bPlayFlash = FALSE;
		pScene->InitRegionList();
		frmRegion->SetIsShow( true );
	}
	else if( szParam2 && strstr( szParam2, "00" ) )
	{
		char strIndex = szParam2[2];
		m_iCurSelServerIndex = strIndex - '1';
		g_pGameApp->m_bPlayFlash = FALSE;

		char szFilename[_MAX_PATH] = { 0 };
		const char* dir = GetFlashDir( 2 );
		_snprintf_s( szFilename, _countof( szFilename ), _TRUNCATE,  "%s\\flash\\serverzone%d.txt", dir, m_iCurSelRegionIndex );
		FILE* fp;
		fopen_s( &fp, szFilename, "wb" );
		if( fp )
		{
			fwrite( &m_iCurSelServerIndex, sizeof( int ), 1, fp );
			fclose( fp );
		}

		if( g_cooperate.code )
			pScene->LoginFlow();//ѡ���˷�����	  
		else
			pScene->ShowLoginForm();
	}
	else if( szParam2 && _stricmp( szParam2, "enter" ) == 0 )
	{
		char szFilename[_MAX_PATH] = { 0 };
		const char* dir = GetFlashDir( 2 );
		_snprintf_s( szFilename, _countof( szFilename ), _TRUNCATE,  "%s\\flash\\serverzone%d.txt", dir, m_iCurSelRegionIndex );
		FILE* fp;
		fopen_s( &fp, szFilename, "rb" );
		if( fp )
		{
			fread( &m_iCurSelServerIndex, sizeof( int ), 1, fp );
			fclose( fp );
		}

		g_pGameApp->m_bPlayFlash = FALSE;
		if( g_cooperate.code )
			pScene->LoginFlow();//ѡ���˷�����	  
		else
			pScene->ShowLoginForm();
	}
}


//-----------------
// �������Routines
//-----------------
void CLoginScene::CallbackUIEvent_LoginScene( CCompent *pSender, int state, int x, int y, DWORD key)
{
	string strName = (const char*)pSender->GetName();
	CLoginScene* pScene = dynamic_cast<CLoginScene*>(CGameApp::GetCurScene());
	if( !pScene )
		return;

	if ( _stricmp("frmAnnounce", pSender->GetForm()->GetName())==0 )
	{
		if ( strName == "btnYes")
		{
			//�ر�ͨ���
			pSender->GetForm()->SetIsShow(false);
			//������Ϸ�����б���ʾ��Ϸ����ѡ���
			pScene->InitRegionList();
			frmRegion->SetIsShow(true);
			//frmServer->Show();
		}
	}
	else if ( _stricmp ("frmServer" , pSender->GetForm()->GetName() ) == 0 )
	{
		if(strName=="btnYes")
		{         
            pSender->GetForm()->Hide();						
            pScene->LoginFlow();//ѡ���˷�����	  
		}
		else if(strName=="btnNo")
		{
			g_pGameApp->SetIsRun( false );
			return;
		}
	}
	//	else if ( stricmp ("frmAccount" , pSender->GetForm()->GetName() ) == 0 )
	else if ( _stricmp ("frmAccount" , pSender->GetForm()->GetName() ) == 0 )
	{
		if(strName=="btnYes")
		{	
			pScene->LoginFlow();
		}
		else if(strName=="btnNo")
		{
			if( g_NetIF->IsConnected() )
			{
				CS_Disconnect(DS_DISCONN);				
			}
			pSender->GetForm()->Hide();
			return;
		}
	}
}

void CLoginScene::_evtRegionFrm(CCompent *pSender, int state, int x, int y, DWORD key)
{
	CLoginScene* pkScene = dynamic_cast<CLoginScene*>(CGameApp::GetCurScene());
	if (!pkScene) return;

	string strName = (const char*)pSender->GetName();
	if (strName == "btnNo")
	{
		//�رշ������б��,��ʾͨ���
		pSender->GetForm()->SetIsShow(false);

		g_pGameApp->SetIsRun( false );
		return;
	}

}

void CLoginScene::_evtLoginFrm(CCompent *pSender, int state, int x, int y, DWORD key)
{
	CLoginScene* pkScene = dynamic_cast<CLoginScene*>(CGameApp::GetCurScene());
	if (!pkScene) return;

	string strName = (const char*)pSender->GetName();
	if(strName=="btnYes")
	{
		// �ȹر������
		if(frmKeyboard->GetIsShow())
		{
			frmKeyboard->SetIsShow(false);

			imgLogo1->SetIsShow(true);
			imgLogo2->SetIsShow(true);
		}

		// ���ӷ�����
		pkScene->LoginFlow();
	}
	else if(strName=="btnNo")
	{
		// �ȹر������
		if(frmKeyboard->GetIsShow())
		{
			frmKeyboard->SetIsShow(false);

			imgLogo1->SetIsShow(true);
			imgLogo2->SetIsShow(true);
		}

		//�ر�����
		if( g_NetIF->IsConnected() )
		{
			CS_Disconnect(DS_DISCONN);				
		}
		//�رյ�½��,��ʾ��һ����ʾ�ķ������б��
		pSender->GetForm()->SetIsShow(false);
//	Xuqin modified for flash login
//		pkScene->InitServerList(pkScene->GetCurSelRegionIndex());
//		frmServer->SetIsShow(true);
		g_pGameApp->m_bPlayFlash = TRUE;
		pkScene->UpdateBounds();
		pkScene->GotoFrame( 1 );
		return;
	}
	else if(strName == "btnKeyboard")
	{
		// �������ʾ��ر�  add by Philip  2006-06-20
		// ���������̡�������ʾ LOGO ͼƬ��LOGO ��������ƴ�Ӷ��ɵģ�
		// ��Ȼ���н����ͻ������������� LOGO ������޷����
		bool bShow = frmKeyboard->GetIsShow();

		ShowKeyboard(! bShow);
		//imgLogo1->SetIsShow(bShow);
		//imgLogo2->SetIsShow(bShow);
		//frmKeyboard->SetIsShow(! bShow);
	}
}

//-----------------------------------------------------------------------------
void CLoginScene::_evtServerFrmBeforeShow(CForm* pForm, bool& IsShow)
{
//#ifdef USE_STATUS
//	CLoginScene* pkScene = dynamic_cast<CLoginScene*>(CGameApp::GetCurScene());
//	if (!pkScene) return;
//
//	GetRegionMgr()->EnterRegion(GetCurRegionName(pkScene->GetCurSelRegionIndex()));
//#endif

}
//-----------------------------------------------------------------------------
void CLoginScene::_evtServerFrmOnClose(CForm* pForm, bool& IsClose)
{
//#ifdef USE_STATUS
//	CLoginScene* pkScene = dynamic_cast<CLoginScene*>(CGameApp::GetCurScene());
//	if (!pkScene) return;
//
//	pkScene->m_szSelServIp = NULL;
//	CRegionInfo* pRegion = GetRegionMgr()->GetActiovRegion();
//	if( pRegion )
//	{
//		CGroupInfo* pGroup = pRegion ->Find( GetCurServerGroupName(pkScene->m_iCurSelRegionIndex,pkScene->m_iCurSelServerIndex) );
//		if (pGroup)
//		{
//			CGateInfo* pGate = pGroup->GetMinGate();
//			if (pGate)
//			{
//				pkScene->m_szSelServIp = pGate->szName;
//			}
//		}
//	}
//	if (!pkScene->m_szSelServIp)
//		pkScene->m_szSelServIp = SelectGroupIP(pkScene->m_iCurSelRegionIndex, pkScene->m_iCurSelServerIndex);
//
//	GetRegionMgr()->ExitRegion();
//#endif
}

void CLoginScene::_evtRegionLDBDown(CGuiData *pSender, int x, int y, DWORD key)
{
	CLoginScene* pkScene = dynamic_cast<CLoginScene*>(CGameApp::GetCurScene());
	if (!pkScene) return;

	CList* pkRegionList = dynamic_cast<CList*>(pSender);
	if (!pkRegionList) return;

	for( int index = 0; index < NUM_REGIN_LIST; index ++ )
	{
		if( pkRegionList == lstRegion[index])
		{
			const char* ReginText = pkRegionList->GetItems()->GetSelect()->GetItem()->GetBegin()->GetString();
			int ReginIndex = GetReginIndex( ReginText );
			int ServerGroupCnt = GetCurServerGroupCnt( ReginIndex );
			if( !ServerGroupCnt )
				break;

			char szFilename[_MAX_PATH] = { 0 }, szFlashXml[_MAX_PATH] = { 0 }, szPngfile[_MAX_PATH] = { 0 };
			_snprintf_s( szFilename, _countof( szFilename ), _TRUNCATE,  "flash%d.swf", ServerGroupCnt );
			GFlashPlayer->SetupFlash( szFilename );

			pkScene->SetCurSelRegionIndex( ReginIndex );
			
			string ServerNames[MAX_SERVER_NUM] = { "", "", "", "", "", "", "", "" },
				ServerComment[MAX_SERVER_NUM] = { "", "", "", "", "", "", "", "" },
				ServerState[MAX_SERVER_NUM] = { "", "", "", "", "", "", "", "" };
			for( int m = 0; m < ServerGroupCnt; m ++ )
			{
				ServerNames[m] = GetCurServerGroupName( ReginIndex, m );
				ServerComment[m] = GetGroupComment( ReginIndex, m );
				ServerState[m] = GetGroupState( ReginIndex, m );
			}

			if( GFlashPlayer )
			{
				int TotalFrames = GFlashPlayer->GetTotalFrames();
				GFlashPlayer->GotoFrame( TotalFrames, FALSE );
			}

			const char* dir = GetFlashDir( 2 );
			_snprintf_s( szFlashXml,  _countof( szFlashXml ), _TRUNCATE,  "%s\\flash\\%s", dir, "flash.xml" );
			_snprintf_s( szFilename, _countof( szFilename ), _TRUNCATE,  "%s\\flash\\%s", dir, "text01.xml" );
			
			ofstream file( szFilename );
			XmlStream xml( file );

			ofstream ofile( szFlashXml );
			XmlStream Xml( ofile );
	
			xml << prolog() // write XML file declaration
				<< tag("data1") // root tag
				<< attr("name") << "imagebear";

			Xml << prolog() // write XML file declaration
				<< tag("data1") // root tag
				<< attr("name") << "imagebear";

			for( int n = 0; n < MAX_SERVER_NUM; n ++ )
			{
				xml << newlinetag("data2") // child tag
					<< attr("con") << ( ServerNames[n] == "" ? "δ��ͨ" : ServerNames[n] )
					<< endtag();

				_snprintf_s( szPngfile, _countof( szPngfile ), _TRUNCATE, "%d%d%d%d.png", n + 1, n + 1, n + 1, n + 1 );
				Xml << newlinetag("data2") // child tag
					<< attr("pic") << szPngfile
					<< attr("con1") << ServerComment[n]
					<< attr("con2") << ( ServerNames[n] == "" ? "δ��ͨ" : ServerNames[n] )
					<< attr("con3") << ServerState[n]
					<< endtag();
			}

			xml	<< endtag(); // close current tag

			Xml	<< endtag(); // close current tag

			_snprintf_s( szFilename, _countof( szFilename ), _TRUNCATE,  "%s\\flash\\%s", dir, "111.xml" );
			ofstream Recordfile( szFilename );

			_snprintf_s( szFilename, _countof( szFilename ), _TRUNCATE,  "%s\\flash\\serverzone%d.txt", dir, ReginIndex );
			FILE* fp;
			fopen_s( &fp, szFilename, "rb" );
			if( fp )
			{
				int ServerIndex = -1;
				fread( &ServerIndex, sizeof( int ), 1, fp );
				fclose( fp );

				XmlStream RecordXml( Recordfile );

				_snprintf_s( szPngfile, _countof( szPngfile ), _TRUNCATE, "%d%d%d%d.png", ServerIndex + 1, ServerIndex + 1,
					ServerIndex + 1, ServerIndex + 1 );

				RecordXml << prolog() // write XML file declaration
					<< tag("data1") // root tag
					<< attr("name") << "imagebear"
					<< tag("data2") // child tag
					<< attr("pic") << szPngfile
					<< attr("con1") << ServerComment[ServerIndex]
					<< attr("con2") << ( ServerNames[ServerIndex] == "" ? "δ��ͨ" : ServerNames[ServerIndex] )
					<< attr("con3") << ServerState[ServerIndex]
					<< endtag() // close current tag
					<< endtag(); // close current tag
			}

			if( GFlashPlayer )
				GFlashPlayer->GotoFrame( 1, TRUE );

			break;
		} 
	}

	if (key & Mouse_LDown)
	{
		pSender->GetForm()->SetIsShow(false);
		// Xu qin modified for flash login
//		pkScene->InitServerList(pkScene->GetCurSelRegionIndex());
//		frmServer->SetIsShow(true);
		g_pGameApp->m_bPlayFlash = TRUE;
		pkScene->UpdateBounds();
	}
}

void  CLoginScene::_evtEnter(CGuiData *pSender)			// added by billy 
{
    CLoginScene* pScene = dynamic_cast<CLoginScene*>(CGameApp::GetCurScene());
    if( !pScene )     return;
    pScene->LoginFlow();
}

void CLoginScene::InitRegionList()
{
	for( int index = 0; index < NUM_REGIN_LIST; index ++ )
		lstRegion[index]->GetItems()->Clear();

	ReginListMap& ListMap = GetReginListMap();

	for( int Index = 1; Index <= NUM_REGIN_LIST; Index ++ )
	{
		ReginListMap::iterator iter = ListMap.find( Index );
		if( iter != ListMap.end() )
		{
			ReginList& reginList = iter->second;
			for( ReginList::iterator ReginIter = reginList.begin(); ReginIter != reginList.end(); ReginIter ++ )
				lstRegion[Index - 1]->Add(  ( * ReginIter ).c_str() );
		}
	}

	//ѡ��Ĭ�ϵĵ�һ��ѡ��
	SetCurSelRegionIndex(0);
	CListItems *items = lstRegion[0]->GetItems() ;
	if ( !items ) return;
	items->Select( GetCurSelRegionIndex() );
}



BOOL CLoginScene::_InitUI()
{

	/*��Ϸ������ѡ���*/
	{	
		frmServer = CFormMgr::s_Mgr.Find( "frmServer" );	
		if(!frmServer) return false;	
#ifdef USE_STATUS
		frmServer->evtBeforeShow = _evtServerFrmBeforeShow;
		frmServer->evtClose = _evtServerFrmOnClose;
#endif

		imgServerIcons = dynamic_cast<CImage*>(frmServer->Find("imgServerIcon0"));
		if (!imgServerIcons) return false;
		imgServerIcons->SetIsShow(false);
	}

	/*��Ϸ����ѡ���*/
	{	
		frmRegion = CFormMgr::s_Mgr.Find("frmArea");
		if (!frmRegion) return false;
		frmRegion->evtEntrustMouseEvent = _evtRegionFrm;

		const char* strRegin[] = 
		{
			"lstRegion0",
			"lstRegion1",
			"lstRegion2",
			"lstRegion3",
			"lstRegion4",
			"lstRegion5"
		};

		for( int index = 0; index < NUM_REGIN_LIST; index ++ )
		{
			lstRegion[index] = dynamic_cast<CList *> (frmRegion->Find( strRegin[index] ));
			if( lstRegion[index] )
				lstRegion[index]->evtListMouseDown = _evtRegionLDBDown;
		}

		//��ȡ��Ϸ�����б�
		InitRegionList();
		if (!g_TomServer.bEnable)
			frmRegion->SetIsShow(true);
		
	}

	frmPathLogo = CFormMgr::s_Mgr.Find("frmPathLogo");
	if(! frmPathLogo) return false;
	frmPathLogo->SetIsShow(true);

	frmAccount = CFormMgr::s_Mgr.Find( "frmAccount" );
	if(!frmAccount) return false;

	frmAccount->evtEntrustMouseEvent = _evtLoginFrm;
    
	chkID  =( CCheckBox *)frmAccount->Find( "chkID" );
	m_bSaveAccount = false;
	if(!chkID)  return false;
	//����ϴ�ʱ��ѡ�����˺�
	char szChkID[128] ={0};
	string strChkID;
	ifstream inCheck("user\\checkid.txt");
	if( inCheck.is_open())
	{
		while(!inCheck.eof())
		{
			inCheck.getline(szChkID, 128);
			strChkID = szChkID ;
			int nCheck = Str2Int(strChkID);
			m_bSaveAccount = (nCheck ==1)?true:false;
			chkID->SetIsChecked( m_bSaveAccount );
		}		
	}
	else
	{
		m_bSaveAccount = true;
		chkID->SetIsChecked( m_bSaveAccount );
	}

	edtID = dynamic_cast<CEdit*>(frmAccount->Find( "edtID" ));
	
	if(!edtID )    return false;
	m_sSaveAccount = "";
	//��������, ���ϴ�chkID����ʱ�� �����ʺ�д�뵽��username.txt�ļ��� ���ڶ�����ʺ�
	char szName[128]= {0};
	ifstream in("user\\username.txt");

	_bAutoInputAct = FALSE;
	if(in.is_open())
	{
		while(!in.eof())
		{
			in.getline(szName, 128);
		}		
		_bAutoInputAct = TRUE;
	}
	m_sSaveAccount = string(szName);
	edtID->SetCaption( m_sSaveAccount.c_str());

	if( edtID )
	{
		edtID->evtEnter = _evtEnter;
		edtID->SetIsWrap(true);

	}

	edtPassword = dynamic_cast<CEdit*>(frmAccount->Find( "edtPassword" ));
	if( edtPassword )
	{
		edtPassword->SetCaption("");
		edtPassword->SetIsPassWord( true );
		edtPassword->SetIsWrap(true);
		edtPassword->evtEnter = _evtEnter;
	}

	// �������̽���
	frmKeyboard = CFormMgr::s_Mgr.Find("frmKeyboard");
	if (!frmKeyboard) return false;

	chkShift = ( CCheckBox *)frmKeyboard->Find("chkShift");
	if(!chkShift) return false;

	// ��������̽���������¼�����
	frmKeyboard->evtEntrustMouseEvent = _evtKeyboardFromMouseEvent;

	if(edtID)
		edtID->evtActive       = _evtAccountFocus;

	if(edtPassword)
		edtPassword->evtActive = _evtAccountFocus;

	// add by Philip.Wu  2006-06-20
	// ��� LOGO ͼƬ
	imgLogo1 = (CImage*) frmAccount->Find("imgLogo1");
	if(!imgLogo1) return false;

	imgLogo2 = (CImage*) frmAccount->Find("imgLogo2");
	if(!imgLogo2) return false;

	return TRUE;
}

void CLoginScene::CloseNewChaFrm()
{
}

bool CLoginScene::IsValidCheckChaName(const char *name)
{
	if( !::IsValidName( name, (unsigned short)strlen(name) ) )
	{
		g_pGameApp->MsgBox( RES_STRING(CL_LANGUAGE_MATCH_51));
		return false;
	}
	return true;

	const char* s = name ;
	int len = (int)strlen(s) ;
	bool bOk  = true;

	for ( int i = 0; i< len ; i++)
	{
		if ( s[i]&0x80 )
		{
			if (!(s[i]==-93) )  //���ڴ����Ƿ���˫�ֽڵ���ĸ
			{
				i++;
			}
			else
			{
				bOk = false;
				i++;
				break;
			}
		}
		else
		{
			if (  !( isdigit(s[i])||isalpha(s[i]) ) )
			{
				bOk = false;			
				break;
			}
		}	
	}

	if (!bOk )
		g_pGameApp->MsgBox( RES_STRING(CL_LANGUAGE_MATCH_52));

	return bOk;
}

bool CLoginScene::_CheckAccount()
{
	// ���ؼ���û���������
	if (strlen(edtID->GetCaption()) == 0)
	{
		g_pGameApp->MsgBox(RES_STRING(CL_LANGUAGE_MATCH_174));
		return false;
	}
	if(!IsValidCheckChaName(edtID->GetCaption()))
		return false;

	if (strlen(edtPassword->GetCaption()) <= 4)
	{
		g_pGameApp->MsgBox(RES_STRING(CL_LANGUAGE_MATCH_175));
		return false;
	}

	// �����û���
	SaveUserName(*chkID, *edtID);

	m_sUsername = edtID->GetCaption();
	m_sPassword = edtPassword->GetCaption();

	return true;
}

bool CLoginScene::_Bill()
{

	m_sPassport = "nobill";
	return true;
}

void CLoginScene::_Connect()
{
	CGameApp::Waiting(true);

	//PlayWhalePose();	//���㶯����Ϊ��ʼ��ʱ�Ͳ���(Michael Chen 2005-06-03)

	_eState = enumConnect;	

	if (g_TomServer.bEnable)
	{
		CS_Connect(g_TomServer.szServerIP.c_str(), g_TomServer.nPort, g_Config.m_nConnectTimeOut);
		return;
	}

//	LG("connect", RES_STRING(CL_LANGUAGE_MATCH_179), m_iCurSelRegionIndex, m_iCurSelServerIndex);
	//int nSelRegionNo = 0;
	//int nNO = lstServer->GetItems()->GetSelect()->GetIndex();

	const char *pszSelectGateIP(0);

#ifdef USE_STATUS
	//�õ��������������ٵ�Gate��Ip
	CRegionInfo* pRegion = GetRegionMgr()->GetActiovRegion();
	if( pRegion )
	{
		CGroupInfo* pGroup = pRegion ->Find( GetCurServerGroupName(m_iCurSelRegionIndex,m_iCurSelServerIndex) );
		if (pGroup)
		{
			CGateInfo* pGate = pGroup->GetMinGate();
			if (pGate)
			{
				pszSelectGateIP = pGate->szName;
			}
		}
	}
#endif

	//���û�еõ����ѡ��Gate��Ip
	if (!pszSelectGateIP) pszSelectGateIP = SelectGroupIP(m_iCurSelRegionIndex, m_iCurSelServerIndex );

	if (!pszSelectGateIP)
	{
		LG("connect", RES_STRING(CL_LANGUAGE_MATCH_180), m_iCurSelRegionIndex, m_iCurSelServerIndex);
	}
	else
	{
		CS_Connect(pszSelectGateIP, 1973, g_Config.m_nConnectTimeOut );
	}
//#endif

}


void CLoginScene::LoginFlow()
{
    ////////////////////////////////////////
    //
    //  By Jampe
    //  �������û������봦��
    //  2006/5/19
    //
    switch(g_cooperate.code)
    {
    case COP_OURGAME:
    case COP_SINA:
    case COP_CGA:
        {
            m_sUsername = g_cooperate.uid;
            m_sPassword = g_cooperate.pwd;
        }  break;
    case 0:
    default:
        {
	        if (!_CheckAccount())
		        return;
        }   break;
    }
    //  end
	if (!_Bill())
		return;
	_Connect();
}

void CLoginScene::Select( int num, NetChaBehave chabehave[] )
{
}

void CLoginScene::NewCha()
{
}

void CLoginScene::DelCha()
{
}

void CLoginScene::_Login()
{
    _eState = enumAccount;

	if( m_sUsername.size() != 0 && m_sPassword.size() != 0 )
    {
			//���ӷ�����
			
            //ȷ��������Ϣ
            CS_Login( m_sUsername.c_str(), m_sPassword.c_str(), m_sPassport.c_str() );

			CGameApp::Waiting();
    }
}

void CLoginScene::SaveUserName(CCheckBox& chkID, CEdit& edtID)
{
	//�������ļ���
	if (!CreateDirectory("user", NULL)) 
	{ 
	} 

	m_bSaveAccount = chkID.GetIsChecked();
	m_sSaveAccount = string(edtID.GetCaption());

	//д�ļ�
	FILE *fchk;
	fopen_s( &fchk, "user\\checkid.txt","wb");
	if ( fchk )
	{
		fwrite( m_bSaveAccount?"1":"0" , strlen("1"), 1,  fchk); 
		fclose(fchk);             
	}

	FILE *fp;
	fopen_s( &fp, "user\\username.txt", "wb");
	if( fp )
	{
		if (m_bSaveAccount)
			fwrite(m_sSaveAccount.c_str() , m_sSaveAccount.size(), 1, fp);
		else 
			fwrite("" ,0,1, fp); 

		fclose(fp);
	}
}


void CLoginScene::BeginPlay()
{
}

void  CLoginScene::_evtVerErrorFrm(CCompent *pSender, int nMsgType, int x, int y, DWORD key)
{
	g_pGameApp->SetIsRun( false );

	if( nMsgType!=CForm::mrYes )
	{
		// ����һ����ҳ
		if( strlen( g_Config.m_szVerErrorHTTP )==0 )
			return;

		::ShellExecute( NULL, "open", 
			g_Config.m_szVerErrorHTTP,
			NULL, NULL, SW_SHOW);
		return;
	}

	// �Զ�����
	extern bool g_IsAutoUpdate;
	g_IsAutoUpdate = true;
}

void CLoginScene::Error( int error_no, const char* error_info )
{
    CGameApp::Waiting( false );
    LG( "error", "%s Error, Code:%d, Info: %s", error_info, error_no, g_GetServerError(error_no) );

	if( ERR_MC_VER_ERROR==error_no && !g_TomServer.bEnable )
	{		
		CBoxMgr::ShowSelectBox( _evtVerErrorFrm, RES_STRING(CL_LANGUAGE_MATCH_181), true );
		return;
	}

    g_pGameApp->MsgBox( "%s", g_GetServerError(error_no) );
}

void CLoginScene::ReSetNewCha()
{
	_pSelectCha = CGameApp::GetCurScene()->AddCharacter(CLoginScene::nSelectChaType );
	for(int i = 0; i <enumEQUIP_PART_NUM; i++)
	{
		_pSelectCha->ChangePart(i, CLoginScene::nSelectChaPart[i]);		
	}	
}

void CLoginScene::ShowChaList()
{
	if( frmAccount )
    {
        frmAccount->Hide();
    }	
	if( frmAnnounce )
    {
        frmAnnounce->Hide();
    }
    if( frmServer )
    {
		frmServer->Hide();
	}
}

void CLoginScene::ShowServerList()
{
	CS_Disconnect(DS_DISCONN);

	if( frmKeyboard)		// add by Philip.Wu  2006-07-21
		ShowKeyboard(false);

	if( frmAccount )
    {
        frmAccount->Hide();
    }	
	if( frmAnnounce )
    {
        frmAnnounce->Hide();
    }
    if( frmServer )
    {
		frmServer->Show();
	}
}

void CLoginScene::ShowRegionList()
{
	CS_Disconnect(DS_DISCONN);

	if( frmKeyboard)		// add by Philip.Wu  2006-06-05
		ShowKeyboard(false);

	if( frmAccount )
		frmAccount->SetIsShow(true);

	if( frmAnnounce )
		frmAnnounce->SetIsShow(false);

	if( frmServer )
		frmServer->SetIsShow(false);

	InitRegionList();
	/*if (frmRegion)
		frmRegion->SetIsShow(true);*/
	if (frmRegion)
		frmRegion->SetIsShow(false);

}

int  CLoginScene::GetServIconIndex(int iNum)
{
	if( iNum < 0 ) return 0;
	if( iNum>ServIconNum ) return ServIconNum;
	return iNum;
}


// add by Philip.Wu  2006-06-05
// ����̵��������ť�¼�
void CLoginScene::_evtKeyboardFromMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
{
	if(!edtFocus) return;

	CLoginScene* pLoginScene = dynamic_cast<CLoginScene*>(CGameApp::GetCurScene());
	if(!pLoginScene) return;

	string strText = edtFocus->GetCaption();
	string strName = pSender->GetName();
	if(strName.size() <= 0) return;

	// ������Ϣ�Ĵ���
	if(strName == "btnClose" || strName == "btnYes")	// �ر������
	{
		if(frmKeyboard->GetIsShow())
		{
			ShowKeyboard(false);
		}
	}
	else if(strName == "btnDel")	// ɾ�����һ���ַ�
	{
		if(strText.size() > 0)
		{
			strText.resize(strText.size() - 1);
			edtFocus->SetCaption(strText.c_str());
		}
	}
	else if(strName == "chkShift")	// ��Сת��
	{
		
	}
	else if(strName == "btnOther101")	// ��һ���ر��ַ���
	{
		strText += '~';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther102")
	{
		strText += '!';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther103")
	{
		strText += '@';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther104")
	{
		strText += '#';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther105")
	{
		strText += '$';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther106")
	{
		strText += '%';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther107")
	{
		strText += '^';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther108")
	{
		strText += '&';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther109")
	{
		strText += '*';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther110")
	{
		strText += '(';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther111")
	{
		strText += ')';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther112")
	{
		strText += '_';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther113")
	{
		strText += '+';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther114")
	{
		strText += '|';
		edtFocus->SetCaption(strText.c_str());
	}

	else if(strName == "btnOther201")	// �ڶ����ر��ַ���
	{
		strText += '`';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther202")
	{
		strText += '-';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther203")
	{
		strText += '=';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther204")
	{
		strText += '\\';
		edtFocus->SetCaption(strText.c_str());
	}

	else if(strName == "btnOther301")	// �������ر��ַ���
	{
		strText += '{';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther302")
	{
		strText += '}';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther303")
	{
		strText += '[';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther304")
	{
		strText += ']';
		edtFocus->SetCaption(strText.c_str());
	}

	else if(strName == "btnOther401")	// �������ر��ַ���
	{
		strText += ':';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther402")
	{
		strText += '\"';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther403")
	{
		strText += ';';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther404")
	{
		strText += '\'';
		edtFocus->SetCaption(strText.c_str());
	}

	else if(strName == "btnOther501")	// �������ر��ַ���
	{
		strText += '<';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther502")
	{
		strText += '>';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther503")
	{
		strText += '?';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther504")
	{
		strText += ',';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther505")
	{
		strText += '.';
		edtFocus->SetCaption(strText.c_str());
	}
	else if(strName == "btnOther506")
	{
		strText += '/';
		edtFocus->SetCaption(strText.c_str());
	}

	else	// ���ܼ�ȫ���ų���ʣ�µ��������ַ�������
	{
		char cAdd = strName.at(strName.size() - 1);

		// �ж��Ƿ������ֻ���ĸ����ʱ͵����ʽ����
		if( ('0' <= cAdd && cAdd <= '9') )
		{
			strText += cAdd;
			edtFocus->SetCaption(strText.c_str());
		}
		else if( 'A' <= cAdd && cAdd <= 'Z')
		{
			if(chkShift->GetIsChecked())
			{
				// ��д
				strText += cAdd;
			}
			else
			{
				// Сд
				strText += cAdd + 32;
			}
			edtFocus->SetCaption(strText.c_str());
		}
	}
}


// add by Philip.Wu  2006-06-07
// �༭�򼤻��¼��������¼���ı༭��
void CLoginScene::_evtAccountFocus(CGuiData* pSender)
{
	CEdit* edtTemp = dynamic_cast<CEdit*>(pSender);

	if(edtTemp)
	{
		edtFocus = edtTemp;
	}
}


void CLoginScene::ShowKeyboard(bool bShow)
{
	frmKeyboard->SetIsShow(bShow);

	imgLogo1->SetIsShow(! bShow);
	imgLogo2->SetIsShow(! bShow);
}


void CLoginScene::ShowPathLogo(int isShow)
{
	frmPathLogo->SetIsShow(isShow ? true : false);
}
