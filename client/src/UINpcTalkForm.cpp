#include "StdAfx.h"
#include "uinpctalkform.h"
#include "uiequipform.h"
#include "uigoodsgrid.h"
#include "uiitemcommand.h"
#include "uipage.h"
#include "packetcmd.h"
#include "gameapp.h"
#include "scene.h"
#include "character.h"
#include "uiboxform.h"
#include "uiedit.h"
#include "STNpcTalk.h"
#include "uiformmgr.h"
#include "uimemo.h"
#include "worldscene.h"
#include <shellapi.h>

using namespace GUI;

static BYTE		_byIndex = - 1; 
static BYTE		_byPage = - 1;       
static DWORD	_npcID = -1;       //npc��id ��
static BYTE		_byCmd = 0;

BYTE CNpcTalkMgr::_byTalkStyle = 0;

//---------------------------------------------------------------------------
// class CNpcTalkMgr
//---------------------------------------------------------------------------
bool CNpcTalkMgr::Init()
{
	m_bIsNpcTalk = false;
	CFormMgr &mgr = CFormMgr::s_Mgr;

	//��ʼ��npc�Ի���
	frmNPCchat = _FindForm("frmNPCchat");   // ���߱�
	if ( !frmNPCchat ) return false;	
	frmNPCchat->evtEntrustMouseEvent = _MainMouseNPCEvent;

	memCtrl = dynamic_cast<CMemo*> (frmNPCchat->Find("memCtrl")) ;
	if( !memCtrl ) return Error(RES_STRING(CL_LANGUAGE_MATCH_45), frmNPCchat->GetName(), "memCtrl");	
	memCtrl->evtSelectChange = _evtMemSelectChange;
	return true;
}

void CNpcTalkMgr::End()
{
}

DWORD CNpcTalkMgr::GetNpcId()
{
	return _npcID;
}

void CNpcTalkMgr::ShowFuncPage( BYTE byFuncPage , BYTE byCount,BYTE byMisNum, const NET_FUNCPAGE& FuncArray, DWORD dwNpcID ) 
{
	m_bIsNpcTalk = true;

	//ɾ��Lst�ؼ�ԭ����Item
	if( !memCtrl ) return;

    memCtrl->Init();
	memCtrl->reset();

	memCtrl->SetCaption( FuncArray.szTalk );

	if ( byCount >0 )   //��ȡѡ������
	{
		memCtrl->SetIsHaveItem(true);			
		memCtrl->SetItemRowNum( byCount);
		for (int i =0 ; i<byCount ; i++)
		{
			if( FuncArray.FuncItem[i].szFunc[0]=='@' )
			{
				string item = &FuncArray.FuncItem[i].szFunc[1];
				size_t pos = item.find( "http" );
				if( pos==string::npos )
				{
					memCtrl->AddItemRowContent( i, item.c_str() );
				}
				else
				{
					string http = item.substr( pos, item.length() );
					item = item.substr( 0, pos );
					memCtrl->AddItemRowContent( i, item.c_str(), http.c_str() );
				}
			}
			else
			{
				memCtrl->AddItemRowContent( i, FuncArray.FuncItem[i].szFunc );
			}
		}			
	}
	if ( byMisNum >0 )
	{
		memCtrl->SetIsHaveMis(true);		
		memCtrl->SetMisRowNum( byMisNum);
		for (int i =0 ; i<byMisNum ; i++)
		{
			if(FuncArray.MisItem[i].byState == ROLE_MIS_ACCEPT )
			{
				CGraph *p = new CGraph("texture/ui/SystemBotton3.tga", 7, 15, 170,119,1);
				memCtrl->AddIcon( i, p);
			
			}
			else if (FuncArray.MisItem[i].byState == ROLE_MIS_DELIVERY )
			{
				CGraph *p = new CGraph("texture/ui/SystemBotton3.tga", 7, 15, 177,119, 1);
				memCtrl->AddIcon( i, p);
			}
			else if (FuncArray.MisItem[i].byState == ROLE_MIS_PENDING)
			{
				CGraph *p = new CGraph("texture/ui/SystemBotton3.tga", 7,15,184,119,1);
				memCtrl->AddIcon( i, p);
			}

			memCtrl->AddMisRowContent( i , FuncArray.MisItem[i].szMis);
		}			
	}
	memCtrl->ProcessCaption();
	
	if ( frmNPCchat )
	{
		frmNPCchat->Show();
		_byIndex  = -1;
	}

	_byPage = byFuncPage;
	_npcID = dwNpcID;
	return;
}

void CNpcTalkMgr::CloseForm()			
{ 
	if( m_bIsNpcTalk && frmNPCchat->GetIsShow() )
	{
		frmNPCchat->Close();	
	}
}

void CNpcTalkMgr::_MainMouseNPCEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
{
	string name = pSender->GetName();

	if( name=="btnNo"  || name == "btnClose" )  //������˳���ť,��رոñ�
	{		
		pSender->GetForm()->Close();
		pSender->GetForm()->Find("memCtrl")->SetCaption("");			
		return;
	}		
	return;
}

void CNpcTalkMgr::_evtMemSelectChange(CGuiData *pSender)
{
	CMemo* memo = dynamic_cast<CMemo*>(pSender);
	if( !memo ) return;

	if (!memo->GetIsHaveItem() &&!memo->GetIsHaveMis() )
		return;

	int nMis= -1;
	int nItem = -1;

	nMis = memo->GetSelectMis();
	nItem = memo->GetSelectItem();

	if( nMis==-1 && nItem ==-1 )
		return;

	if ( nMis >=0  )
	{
		_byTalkStyle = 1;
		_byIndex =nMis ;
	}

	if (nItem >=0 )
	{
		_byTalkStyle = 0; 
		_byIndex =nItem ;
	}	
	
	switch( _byTalkStyle )
	{
	case 0:
		{
			string item, itemex;
			memo->GetSelectItemText( item, itemex );
			if( itemex.empty() )
			{
				CS_SelFunction( _npcID, _byPage, _byIndex) ;
			}
			else
			{
				::ShellExecute( NULL, "open", 
					itemex.c_str(),
					NULL, NULL, SW_SHOW);
			}
		}
		break;
	case 1:
		CS_SelMission( _npcID, _byIndex );
	
		break;
	case 2:
		CS_SelMissionFunc( _npcID, _byPage, _byIndex) ;
		break;
	}			
	pSender->GetForm()->Close();
}

void CNpcTalkMgr::FrameMove(DWORD dwTime)
{	
	static CTimeWork time(100);
	if( !time.IsTimeOut( dwTime ) ) return;

	if( frmNPCchat && !frmNPCchat->GetIsShow() )
	{
		if( m_HelpInfoList.size() > 0 )
		{
			HELP_LIST::iterator pos = m_HelpInfoList.begin();
			if( pos == m_HelpInfoList.end() )
				return;

			ShowHelpInfo( *pos );
			m_HelpInfoList.erase( pos );
		}
	}
}

void CNpcTalkMgr::LoadingCall()
{
}

void CNpcTalkMgr::SwitchMap()
{
	if( !(dynamic_cast<CWorldScene*>( CGameApp::GetCurScene() )) ) return;

	// ����
	static bool IsFirstWorldScene = true;
	if( IsFirstWorldScene )
	{
		IsFirstWorldScene = false;
		FILE* fp;
		fopen_s( &fp, "./scripts/txt/HelpInfo.tx", "rt" );
		if( fp )
		{
			if( fseek(fp, 0, SEEK_END) == 0 )
			{
				long size = ftell( fp );
				fseek(fp, 0, SEEK_SET);

				NET_HELPINFO Info;
				memset( &Info, 0, sizeof(NET_HELPINFO) );
				//fread(Info.szDesp, size > ROLE_MAXNUM_DESPSIZE - 1 ? ROLE_MAXNUM_DESPSIZE - 1 : size, 1, fp);
				fread(Info.szDesp, size > HELPINFO_DESPSIZE - 1 ? HELPINFO_DESPSIZE - 1 : size, 1, fp);
				Info.byType = mission::MIS_HELP_DESP;
				AddHelpInfo( Info );			
			}
			fclose( fp );
		}
		return;
	}
}

void CNpcTalkMgr::ShowHelpInfo(const NET_HELPINFO &Info)
{
	if( Info.byType == mission::MIS_HELP_DESP || Info.byType == mission::MIS_HELP_IMAGE )
	{
		m_bIsNpcTalk = false;
		if ( memCtrl )
		{
			memCtrl->Init();
			memCtrl->SetCaption( Info.szDesp );
			memCtrl->ProcessCaption();
			if( frmNPCchat )
			{
				frmNPCchat->Show();
			}
			else
			{

			}
		}
		else
		{
		}			
	}
	else if( Info.byType == mission::MIS_HELP_SOUND )
	{
		//Info.sID;
	}
	else
	{
		return ;
	}
	return;
}

void CNpcTalkMgr::AddHelpInfo(const NET_HELPINFO &Info)
{
	m_HelpInfoList.push_back( Info );
}

void CNpcTalkMgr::ShowTalkPage( const char *content, BYTE command, DWORD npcID )
{
	if ( memCtrl )
	{
		memCtrl->Init();
		memCtrl->SetCaption( content );
		memCtrl->ProcessCaption();
	}
	if ( frmNPCchat )
	{
		m_bIsNpcTalk = true;
		frmNPCchat->Show();
	}

	_byCmd = command;
	_npcID = npcID;
}

void CNpcTalkMgr::CloseTalk( DWORD dwNpcID )
{
	if( frmNPCchat )
	{
		frmNPCchat->Close();
		memCtrl->SetCaption("") ;
	}
}
