//----------------------------------------------------------------------
// ����:����ʵ���������
// ����:lh 2005-02-26
// ����޸�����:2004-10-20
//----------------------------------------------------------------------
#pragma once
#include "UIGlobalVar.h"
#include "netprotocol.h"
class CCharacter2D;

extern int SMALL_ICON_SIZE;
extern int BIG_ICON_SIZE;

namespace GUI
{
class CGuiData;
class CMenu;
class CTreeNodeObj;
class CItemObj;

enum eSendTeamMsg
{
    enumSTM_ADD_GROUP,  // ������,����1:CTeam*
    enumSTM_DEL_GROUP,  // ɾ����,����1:CTeam*
    enumSTM_ADD_MEMBER, // �������Ա,����:CMember*
    enumSTM_DEL_MEMBER, // ɾ�����Ա,����:CMember*
	enumSTM_AFTER_DEL_MEMBER,
    enumSTM_NODE_CHANGE,// ��ǰ�ڵ㷢���仯,����:CMember*
    enumSTM_NODE_DATA_CHANGE,   // �ڵ���չ���ݷ����仯,������ӵ�Ѫ,ħ��,����ȷ����仯ʱ֪ͨ,����:CMember*

	enumSTM_ADD_FRIEND_GROUP,	// ׷�����ѷ���
	enumSTM_DEL_FRIEND_GROUP,	// ɾ�����ѷ���
};

class CGuiTime;
class CNoteGraph;

//begin add by guojiangang 20080211
class PictureCChat
{
public:
	PictureCChat():m_AreaID(0), m_ServerID(0),m_ServerKey(NULL), m_UserName(NULL),
		m_VisitorName(NULL), m_VisitorCharacterID(0)
	{
	}
	void setAreaID(int AreaID) { m_AreaID = AreaID;}
    void setServerID(int ServerID)  {	m_ServerID = ServerID;}
    void setServerKey(char* ServerKey)	{  m_ServerKey = ServerKey;}
    void setUserName(char* UserName)	{  m_UserName = UserName;}
	void setVisitorName(char* VisitorName)  {  m_VisitorName = VisitorName;}
	void setVisitorCharacterID(int VisitorCharacterID){ m_VisitorCharacterID = VisitorCharacterID; }

	const int getAreaID()const{ return m_AreaID;}
	const int getServerID() const{ return m_ServerID;}
	const long int getVisitorCharacterID() const { return m_VisitorCharacterID;}
	const char* getServerKey()const { return m_ServerKey;}
	const char* getUserName()const { return m_UserName;}
	const char* getVisitorName()const { return m_VisitorName;}
	

protected:
private:
	int   m_AreaID;
	int   m_ServerID;
	char* m_ServerKey;
	char* m_UserName;
	char* m_VisitorName;
	DWORD m_VisitorCharacterID;
	

};
//end guojiangang20090211

class CChat : public CUIInterface
{
	friend class CChaQeryInfo;//add by guojiangang 20090211
public:
    enum 
    {
        MAX_MEMBER = 4,         // ����Ա��
    };

    CChat();

    bool Init();
    void End();

    void RefreshTeam();         // ˢ����ӽ���

	// ˢ����ӳ�Ա����
	void RefreshTeamData( CMember* pCurMember );		

    void ClearTeam();           // �����ӽ���
    int  TeamSend( DWORD dwMsg, void* pData=NULL, DWORD dwParam=0 );

	void SortOnlineFrnd();

	// Add by lark.li 20080806 begin
	void SortOnlineFrnd(CTreeGridNode* pNode);
	// End

	void MasterAsk(const char* szName, DWORD dwCharID);
	void PrenticeAsk(const char* szName, DWORD dwCharID);

	// Add by lark.li 20080707 begin
	// �������ӳ�ȷ��
	void CaptainConfirm(DWORD dwTeamID);
	// End

	// Add by drog 2008-8-18
	// /*�����null,����ԭ�������ݸ���*/
	void ResetSelfInfo(const stPersonInfo * _info);

public:
	CForm*			GetQQFrom()		{ return _frmQQ;		}
    CTreeView*      GetTeamView()   { return m_pQQTreeView;   }
	CTreeGridNode*  GetSessionNode(){ return _pSessionNode;	}
	CTreeGridNode*  GetGuildNode(){ return _pGuildNode;	}
    CTeamMgr*       GetTeamMgr()    { return _pTeamMgr;    }
	static bool		_UpdateFrndInfo(CMember *pMember);
	static void            setURLPicture(DWORD chaID);//add guojiangang 20090211
	static void           GB2312ToUTF8( const char* utf8, string &OutUTF8);//add guojiangang 20090211
	//static void		LoadFilterText(const char *text);
	static bool		_UpdateSelfInfo();
	static DWORD	_dwSelfID;			// �Լ���ID
	static DWORD	_dwSelfIcon;			// �Լ���ͷ��
	static string	_strSelfMottoName;		// �Լ���������
	static CMember* _curSelectMember;

	 PictureCChat _PictureCChat;//add by guo jiangang 20090211
	 static char m_URLPicture[512];//add by guo jiangang 20090211

	
	// Add by drog   2008-8-18  
	static stPersonInfo  _SelfInfo;
	// Add by lark.li 20080804 begin
	static CTreeGridNode* _curSelectNode;
	// End


public:
	CForm*          frmChatManage;          // Chat����
	//static CForm*	_frmDetails;

	//��ӽ���
	static CForm*          frmTeamMenber[MAX_MEMBER];		// nTag��¼HummanID
	static CProgressBar*   proTeamMenberHP[MAX_MEMBER];
	static CProgressBar*   proTeamMenberSP[MAX_MEMBER];
	static CLabelEx*       labMenberName[MAX_MEMBER];
	static CLabelEx*       labLv[MAX_MEMBER];
	static CImage*		   imgWork[MAX_MEMBER];
	static CImage*		   imgLeader[MAX_MEMBER];

	static bool _bForbid;
	static vector<char*>	_strFilterTxt;

private:
	static bool _Error(const char* strInfo, const char* strFormName, const char* strCompentName) {
		LG("gui", strInfo, strFormName, strCompentName );
		return false;
	}

    static void _RenderEvent(C3DCompent *pSender, int x, int y);
	static void _MemberMouseDownEvent(CGuiData *pSender, int x, int y, DWORD key);

public:
	// Add by lark.li 20080804 begin
	CTreeGridNode* GetFrndGroupNode(const char* szGroupName, CTreeGridNode* pNode = NULL);
	bool IsFrndGroupNode(CTreeGridNode* pNode);
	void AddFrndGroup(const char* szGroupName);
	void DelFrndGroup(const char* szGroupName);
	void ChangeFrndGroup(const char* szOldGroupName, const char* szNewGroupName);
	// End

	// Add by lark.li 20080811 begin
	void ShowDetailForm(CMember* pMember);
	// End

private:

	// Add by lark.li 20080804 begin
	static CForm*          _frmAddFrndGroup;
	static CForm*          _frmEditFrndGroup;
	// End

	static CForm*          _frmQQ;			 // QQ����

	//	Ů����������	Add by alfred 20080903	begin
	static CForm*			frmjieguo2;		 
	static CForm*			frmchainfo2;	
	//	End.

	static CForm*          _frmAddFriend;	 
	static CForm*          _frmChangeSytle;
	static CForm*			_frmEditMotto;
    static CTeamMgr*       _pTeamMgr;        // ��ӹ�����
	CTreeView*		m_pQQTreeView;
	CTreeGridNode*  _pFrndNode;              // ���ѽڵ�

	// Add by lark.li 20080802 begin
	CTreeGridNode*  _pFrndNodeAll;              // ���ѽڵ�ȫ����
	vector< pair<string, CTreeGridNode*> >  vFrndNode;          // ���ѽڵ㼯��

	CTreeGridNode* _pDropTreeNode;		// ׼��Ҫ�ŵķ���
	// End

    CTreeGridNode*  _pGroupNode;             // ����ڵ�
	//����·������

    //CTreeGridNode*  _pRoadNode;              // ·�˽ڵ�
	static CTreeGridNode*  _pGuildNode;      // ����ڵ�
	static CTreeGridNode*  _pSessionNode;
	static CTreeGridNode*  _pMasterNode;	// ʦ��
	static CTreeGridNode*  _pPrenticeNode;	// ͽ��

	static CCharacter2D*   _pCharacter[MAX_MEMBER]; // ������
	static CEdit*	_pEditFrndName;

	// Add by lark.li 20080804 begin
	static CEdit*	_pAddFrndGroupName;
	static CEdit*	_pEditFrndGroupName;
	// End

	static CEdit*	_pEditMotto;
	//static CForm*	_frmMouseRightFrnd;
	//static CForm*	_frmMouseRightGroup;
	//static CForm*	_frmMouseRightRoad;
	static CList*	_lstFrndPlayer;
	static CList*	_lstGroupPlayer;
	static CList*	_lstRoadPlayer;
	static CList*	_lstStyle;

	// Add by lark.li 20080804 begin
	static CMenu*	_frndGroupMouseRight;
	static CMenu*	_hitNothingMouseRight;
	// End

	static CMenu*	_frndMouseRight;
	static CMenu*	_groupMouseRight;
	static CMenu*	_roadMouseRight;
	static CMenu*	_sessMouseRight;
	static CMenu*	_MasterMouseRight;
	static CMenu*	_StudentMouseRight;
	static CMenu*	_styMenu;

	static CLabelEx*		_labNameOfMottoFrm;
	static CImage*			_imgFaceOfMottoFrm;
	static CCheckBox*		_chkForbidOfMottoFrm;

	// Add by lark.li 20080804 begin
	static void _OnMouseFrndGroupMenu(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnMouseAddFrndGroupMenu(CGuiData *pSender, int x, int y, DWORD key);
	// End

	static void _MainMouseClick(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnMouseFrndMenu(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnMouseGroupMenu(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnMouseRoadMenu(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnMouseSessMenu(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnMouseMasterMenu(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnMouseStudentMenu(CGuiData *pSender, int x, int y, DWORD key);

	static void _evtQQMainShow(CGuiData *pSender);
	static void _evtQQMainAddFrnd(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtQQMainChangeStyle(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtAddFrnd(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtChangeStyle(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtQQMainEditMotto(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtChangeMotto(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtRefreshInfo(CGuiData *pSender, int x, int y, DWORD key);

	// Add by lark.li 20090325 begin
	static void _evtGostMainEditMotto(CGuiData *pSender, int x, int y, DWORD key);
	// End

	//	Add by alfred.shi 20080903	begin
	static void _evtAddFriend(CGuiData *pSender, int x, int y, DWORD key);		
	static void _evtViewFriend(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtViewDetailInfo(CGuiData *pSender, int x, int y, DWORD key);
	//	End.

	static void _OnTimerFlash(CGuiTime *pSender);

	// Modify by lark.li 20080805 begin
	//static void _OnDragEnd(CForm* pTargetForm, CTreeView *pTree, CTreeNodeObj* pNode, CItemObj* pItem, int x, int y, DWORD key );
	static void _OnDragEnd(CForm* pTargetForm, CTreeView *pTree, CTreeNodeObj* pNode1, CTreeNodeObj* pNode2, CItemObj* pItem, int x, int y, DWORD key );	
	static void _OnDragMouseMove(CForm* pTargetForm, CTreeView *pTree, CTreeNodeObj* pNode1, CTreeNodeObj* pNode2, CItemObj* pItem, int x, int y, DWORD key );
	// End

	static void _OnFrndDeleteConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _OnMasterDeleteConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _OnStudentDeleteConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _OnStudentAskConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _OnMasterAskConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

	// Add by lark.li 20080804 begin
	static void _evtAddFrndGroup(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtEditFrndGroup(CGuiData *pSender, int x, int y, DWORD key);

	static void _evtAskDelGroupEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	// End

	// Add by lark.li 20080707 begin
	static void _OnCaptainAskConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	// End

	//begin add  guojiangang 20090209
	static void		_PhotoFrmDragBegin(CGuiData *pSender, int x, int y, DWORD key);
	static void		_PhotoFrmDragEnd(CGuiData *pSender, int x, int y, DWORD key);
	static void		_webPhotoFrmShow(CGuiData *pSender);
	static void		_webPhotoFrmHide(CGuiData *pSender);
    //end add  guojiangang 20090209

};

}

extern CChat            g_stUIChat;
