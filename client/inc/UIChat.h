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

class PictureCChat
{
public:
	PictureCChat() :m_AreaID(0), m_ServerID(0), m_ServerKey(NULL), m_UserName(NULL),
		m_VisitorName(NULL), m_VisitorCharacterID(0)
	{
	}
	void setAreaID(int AreaID) { m_AreaID = AreaID; }
	void setServerID(int ServerID) { m_ServerID = ServerID; }
	void setServerKey(char* ServerKey) { m_ServerKey = ServerKey; }
	void setUserName(char* UserName) { m_UserName = UserName; }
	void setVisitorName(char* VisitorName) { m_VisitorName = VisitorName; }
	void setVisitorCharacterID(int VisitorCharacterID) { m_VisitorCharacterID = VisitorCharacterID; }

	const int getAreaID()const { return m_AreaID; }
	const int getServerID() const { return m_ServerID; }
	const long int getVisitorCharacterID() const { return m_VisitorCharacterID; }
	const char* getServerKey()const { return m_ServerKey; }
	const char* getUserName()const { return m_UserName; }
	const char* getVisitorName()const { return m_VisitorName; }

	void ChangeFrndGroup(const char* character, const char* szcontent);
protected:
private:
	int   m_AreaID;
	int   m_ServerID;
	char* m_ServerKey;
	char* m_UserName;
	char* m_VisitorName;
	DWORD m_VisitorCharacterID;


};

class CChat : public CUIInterface
{
	friend class CChaQeryInfo;
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

	void MasterAsk(const char* szName, DWORD dwCharID);
	void PrenticeAsk(const char* szName, DWORD dwCharID);
	void CaptainConfirm(DWORD dwTeamID);
	// End

	// Add by drog 2008-8-18
	// /*�����null,����ԭ�������ݸ���*/
	void ResetSelfInfo(const stPersonInfo * _info);
public:
	CForm*			GetQQFrom()		{ return _frmQQ;		}
	CForm*          GetDetailsForm(){ return _frmDetails;     }
    CTreeView*      GetTeamView()   { return m_pQQTreeView;   }
	CTreeGridNode*  GetSessionNode(){ return _pSessionNode;	}
	CTreeGridNode*  GetGuildNode(){ return _pGuildNode;	}
    CTeamMgr*       GetTeamMgr()    { return _pTeamMgr;    }
	static bool		_UpdateFrndInfo(CMember *pMember);
	static bool		_UpdateSelfInfo();
	static DWORD	_dwSelfID;			// �Լ���ID
	static DWORD	_dwSelfIcon;			// �Լ���ͷ��
	static string	_strSelfMottoName;		// �Լ���������
	static CMember* _curSelectMember;
	PictureCChat _PictureCChat;
	static stPersonInfo  _SelfInfo;
public:
	CForm*          frmChatManage;          // Chat����

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
	void ShowDetailForm(CMember* pMember);
	bool IsFrndGroup(CTreeGridNode* pNode);
	// End

private:

	static CForm*          _frmQQ;			 // QQ����

	static CForm*          _frmAddFriend;	 
	static CForm*          _frmChangeSytle;
	static CForm*			_frmEditMotto;
	static CForm*          _frmDetails;
    static CTeamMgr*       _pTeamMgr;        // ��ӹ�����
	CTreeView*		m_pQQTreeView;
	CTreeGridNode*  _pFrndNode;              // ���ѽڵ�

    CTreeGridNode*  _pGroupNode;             // ����ڵ�
	vector< pair<string, CTreeGridNode*> >  vFrndNode;
	//����·������

    //CTreeGridNode*  _pRoadNode;              // ·�˽ڵ�
	static CTreeGridNode*  _pGuildNode;      // ����ڵ�
	static CTreeGridNode*  _pSessionNode;
	static CTreeGridNode*  _pMasterNode;	// ʦ��
	static CTreeGridNode*  _pPrenticeNode;	// ͽ��

	static CCharacter2D*   _pCharacter[MAX_MEMBER]; // ������
	static CEdit*	_pEditFrndName;

	static CEdit*	_pEditMotto;
	//static CForm*	_frmMouseRightFrnd;
	//static CForm*	_frmMouseRightGroup;
	//static CForm*	_frmMouseRightRoad;
	static CList*	_lstFrndPlayer;
	static CList*	_lstGroupPlayer;
	static CList*	_lstRoadPlayer;
	static CList*	_lstStyle;

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

	static void _MainMouseClick(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnMouseFrndMenu(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnMouseGroupMenu(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnMouseRoadMenu(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnMouseSessMenu(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnMouseMasterMenu(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnMouseStudentMenu(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnCaptainAskConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _evtQQMainShow(CGuiData *pSender);
	static void _evtQQMainAddFrnd(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtQQMainChangeStyle(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtAddFrnd(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtChangeStyle(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtQQMainEditMotto(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtChangeMotto(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtRefreshInfo(CGuiData *pSender, int x, int y, DWORD key);
	static void _evtGostMainEditMotto(CGuiData *pSender, int x, int y, DWORD key);
	static void _OnTimerFlash(CGuiTime *pSender);

	static void _OnDragEnd(CForm* pTargetForm, CTreeView *pTree, CTreeNodeObj* pNode, CItemObj* pItem, int x, int y, DWORD key );

	static void _OnFrndDeleteConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _OnMasterDeleteConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _OnStudentDeleteConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _OnStudentAskConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _OnMasterAskConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
};

}

extern CChat            g_stUIChat;
