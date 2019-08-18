#pragma once
#include "Scene.h"
#include "UICursor.h"
#include "scenelight.h"
#include "LEFont.h"
#include "mousedown.h"
#include "uiguidata.h"
#include "NetProtocol.h"

class xShipMgr;
class xShipMgrCShipBuilder;
class CEvent;


class CWorldScene : public CGameScene
{
protected:
	virtual bool	_Init();
	virtual bool	_Clear();
	virtual void	_Render();
	virtual void	_FrameMove( DWORD dwTimeParam  );

    virtual bool	_HandleSuperKey();
	virtual void	_KeyDownEvent( int key );
	virtual bool	_MouseButtonDown(int nButton);
	virtual bool	_MouseButtonUp(int nButton);
	virtual bool	_MouseMove(int nOffsetX, int nOffsetY);
	virtual bool	_MouseButtonDB(int nButton);

public:
	virtual void	SetMainCha(int nChaID);
	virtual void	SetScreen( int w, int h, bool IsFull );
	static void		SetAttackChaColor( BYTE r, BYTE g, BYTE b );

    virtual void    LoadingCall();          // ��װ��loading��,ˢ��
	static CWorldScene& GetCurrScene();

public:
	CWorldScene(stSceneInitParam& param);
	~CWorldScene();

    AnimCtrlLight*	GetAnimLight(DWORD id)   { return (id>=_dwAnimLightNum) ? 0 : &_pAnimLightSeq[id]; }
    int				SwitchShipBuilder();

    xShipMgr*		GetShipMgr() { return _pShipMgr; }

    CCharacter*     HitSelectCharacter( int nScrX, int nScrY, int nSelect=0 );			// nSelectΪѡ��Χ,�ο�CCharacter.h-eSelectCha

	CMouseDown&		GetMouseDown()			{ return _cMouseDown;		}

	int				PickItem();
	void			ClearSelectUI();

protected:
    BOOL			_LoadAnimLight(const char* file);
    void			_SceneCursor();
	BOOL			_InitUI();
	bool			_IsBlock( CCharacter* pCha, int x, int y );

	static void         _SelChaFrmMouseEvent(CCompent *pSender, int nMsgType, 
		int x, int y, DWORD dwKey);

	static void         _evtCreateDoublePwdEvent(CCompent *pSender, int nMsgType, 
		int x, int y, DWORD dwKey);		// ѯ���Ƿ�Ҫ������������

	static void         _evtWelcomeNoticeEvent(CCompent *pSender, int nMsgType, 
		int x, int y, DWORD dwKey);		// ��ӭ���� �¼�����

	static void         _evtCreateOKNoticeEvent(CCompent *pSender, int nMsgType, 
		int x, int y, DWORD dwKey);		// �״δ�����ɫ�ɹ���ʾ���� �¼�����

	static void         _evtChaNameAlterMouseEvent(CCompent *pSender, int nMsgType, 
		int x, int y, DWORD dwKey);		// �״δ�����ɫ�ɹ���ʾ���� �¼�����

	//~ �߼�����: ============================================================

public:
	//����������Ϳ�ʼ��Ϸ��Ϣ
	void                SendBeginPlayToServer();
	//ɾ����ǰѡ�еĽ�ɫ
	void                DelCurrentSelCha();
	//��ʾ���н�ɫ
	bool                SelectCharacters(NetChaBehave* chabehave, int num);
	//������ɫ
	bool                CreateCha();


	//�����������ɾ����ɫ��Ϣ
	void SendDelChaToServer(const char szPassword2[]);

	// ���°�ť״̬
	void UpdateButton();

	// ��ý�ɫ����
	int GetChaCount();

	void ShowWelcomeNotice(bool bShow = true);
	
private:
    xShipMgr*		_pShipMgr;
    AnimCtrlLight*	_pAnimLightSeq;
    DWORD			_dwAnimLightNum;
	CLEFont			_cFont;

private:
	static BYTE		_bAttackRed, _bAttackGreen, _bAttackBlue;

	int             _nOldMainChaInArea;
	CMouseDown		_cMouseDown;
	bool			_IsShowSideLife;
	static bool		_IsShowPing;
	static bool		_IsAutoPick;
	static bool		_IsShowCameraInfo;
public:
	//UI
	CForm*			frmSelectCha;
	CForm*			frmRace;
	CForm*			frmmod;
	CTextButton*    btnDel;
	CTextButton*    btnYes;
	CTextButton*    btnCreate;
	CTextButton*    btnExit;
	CTextButton*    btnAlter;
	
	CTextButton*    btnrace1;
	CTextButton*    btnrace2;
	CTextButton*    btnrace3;

	CForm*          frmCheck;

	CTextButton*    btnModYes;
	CEdit*          edtModName;
	CTextButton*    btnModleft;
	CTextButton*    btnModright;

	CTextButton*    btnback;
	CTextButton*    btnleft;
	CTextButton*    btnright;

	//�߼�
	int             m_nCurChaIndex;

	//
	bool            m_isInit;

	bool			m_isCreateCha;

	CForm*			frmWelcomeNotice;	// ���延ӭ����  �ý�����ڵ�ǰ�ʺ����޽�ɫʱ����
	CForm*			frmCreateOKNotice;	// �����״δ�����ɫ�ɹ���ʾ����  �ý�����ڸ��ʺ������һ����ɫ�Ĵ������̺���ʾ
	CForm*			frmChaNameAlter;	// �����������  �ý������̨��汾��Ч�������ñ��ݹ���

public:
	// �ӵ����Ƿ���ʾ��ʾ
	static bool		_IsThrowItemHint;	
	CCharacter*		m_strChar[4];
	stNetChangeChaPart m_part;
};
