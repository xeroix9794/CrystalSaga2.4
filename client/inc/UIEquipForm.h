#pragma once
#include "UIGlobalVar.h"
#include "NetProtocol.h"
#include "SkillRecord.h"

class CCharacter;
class CSkillStateRecord;
struct stNetSkillBag;

namespace GUI
{
class CItemCommand;

// ���ܣ����ߣ���ݷ�ʽ��װ��
class CEquipMgr : public CUIInterface
{
public:
	void			SynSkillBag(DWORD dwCharID, stNetSkillBag *pSSkillBag);

    void            UpdataEquip( const stNetChangeChaPart& SPart, CCharacter* pCha );
    void            UpdataEquipData( const stNetChangeChaPart& SPart, CCharacter* pCha );

    void            UpdateShortCut( stNetShortCut& stShortCut );		// ���¿����
	void			DelFastCommand( CCommandObj* pObj );				// ɾ����ǰ�����
    bool            ExecFastKey( int key );

	void            CloseAllForm();		// �رմ���

	// ���������������Ƚ�,�����������ͬ�Ŀ����,�����������Э����¿����,���ظ��µĸ���
	// ���ڵ����ڵ���������ʱ
	int				RefreshServerShortCut();							
	int				shortCutKeyNum;
	CGoodsGrid*		GetGoodsGrid()					{ return grdItem;				}	// �������������
	CForm*			GetItemForm()					{ return frmItem;				}   // �������߱�

	// ��װ����ж�ص��ߵ�������
	void			UnfixToGrid( CCommandObj* pItem, int nGridID, int nLink );

	CSkillRecord*	FindSkill( int nID );

	CItemCommand*	GetEquipItem( unsigned int nLink );

	bool			IsEquipCom( COneCommand* pCom );

	void			SetIsLock(bool bLock);
	bool			GetIsLock();

	// ��õ����ڵ�ǰ�������ܵĸ���
	int				GetItemCount(int nID);
	// ��õ����ڵ�ǰ�����е�λ��
	short			GetItemPos(int nID);

	static CGuiPic*		GetChargePic( unsigned int n );

    static void		evtThrowItemEvent(CGuiData *pSender,int id,bool& isThrow);					// �ӵ��������ӳ�����
    static void		evtSwapItemEvent(CGuiData *pSender,int nFirst, int nSecond, bool& isSwap); // �������н�������

protected:
	virtual bool Init();
    virtual void End();
	virtual void LoadingCall();
	virtual void FrameMove(DWORD dwTime);

private:
    CSkillList*		GetSkillList( char nType );
	void			RefreshUpgrade();
	void			RefreshSkillJob( int nJob );

	bool			_GetThrowPos( int& x, int& y );
	bool			_GetCommandShortCutType( CCommandObj* pItem, char& chType, short& sGridID );

	static void		_evtSkillFormShow(CGuiData *pSender);
	static void		_evtItemFormShow(CGuiData *pSender);
	static void		_evtItemFormClose( CForm *pForm, bool& IsClose );

	static void		_evtSkillUpgrade(CSkillList *pSender, CSkillCommand* pSkill);
    static void		_evtFastChange(CGuiData *pSender, CCommandObj* pItem, bool& isAccept);		// ����������仯
	static void		_evtEquipEvent(CGuiData *pSender, CCommandObj* pItem, bool& isAccept);		// �ӵ�������װ����װ����
	static void		_evtThrowEquipEvent(CGuiData *pSender, CCommandObj* pItem, bool& isThrow);  // ��װ�����ӳ�װ��

	static void		_evtButtonClickEvent( CGuiData *pSender, int x, int y, DWORD key);
	static void		_evtRMouseGridEvent(CGuiData *pSender,CCommandObj* pItem,int nGridID);

	bool			_UpdataEquip( SItemGrid& Item, int nLink );
    void			_ActiveFast( int num );         // ����ڼ���

	static void		_evtItemFormMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

	// ���� MSGBOX ȷ��
	static void		_CheckLockMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);


private:
	CForm*			frmSkill;

    CLabel*         labPoint;                   // ʣ�༼�ܵ���
    CLabel*         labPointLife;               // ʣ������ܵ���
    CSkillList*     lstFightSkill;              // ս��������
    CSkillList*     lstLifeSkill;               // �������
    CSkillList*     lstSailSkill;               // ����������

	CForm*			frmItem;					// ���߱�
	CGoodsGrid*     grdItem;					// ������
    stNetChangeChaPart    stEquip;
    stNetShortCut         _stShortCut;

    int				_nFastCur;					// �����Ŀǰ�ڵڼ�λ
	CLabel*			_pActiveFastLabel;			// ���ڼ���������ҳ��

	CLabel*			lblGold;					// ��Ǯ
	CImage*			imgItem4;					// ����ǰ4���ͼƬ

	CImage*			imgLock;
	CImage*			imgUnLock;
	CTextButton*    rotateLeft;
private:
	static void RenderCha(int x, int y);

	static void RenderModel(int x, int y, CCharacter* original, CCharacter* model, int rotation, bool refresh);
	static void ChaLeftRotate();
	static void ChaRightRotate();
	static void _evtChaLeftRotate(CGuiData *sender, int x, int y, DWORD key);
	static int m_rotate;
	static void _evtChaRightRotate(CGuiData *sender, int x, int y, DWORD key);

	static void _evtChaLeftContinueRotate(CGuiData *sender);

	static void _evtChaRightContinueRotate(CGuiData *sender);
	
	static bool refreshChaModel;
	static void _cha_render_event(C3DCompent *pSender, int x, int y);
    C3DCompent* ui3dCreateCha;

    CCharacter      * m_pCurrMainCha;
	static CCharacter* characterCopy;
	CLabel*         labChaName;
	CLabel*         labChaGuild;

public:
	COneCommand*    cnmEquip[enumEQUIP_NUM];    // װ����
	CFastCommand**  _pFastCommands;

private:
	struct stThrow
	{
		stThrow() : nX(0), nY(0), nGridID(0), pSelf(0) {}

		int				nX, nY;
		int				nGridID;
		CCharacter*		pSelf;
	};
	stThrow		_sThrow;
	static void _evtThrowDialogEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _evtThrowBoatDialogEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

	static void _evtThrowDeleteConfirm(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	void		_SendThrowData( const stThrow& sthrow, int nThrowNum=1 );

	struct stUnfix
	{
		stUnfix() : nLink(0), nGridID(0), nX(0), nY(0), pItem(NULL) {}
		void	Reset()	{ nGridID=-1; nLink=-1;	pItem=NULL;}

		int		nLink;
		int		nGridID;
		int		nX, nY;
		CCommandObj* pItem;
	};
	stUnfix		_sUnfix;

	static void _evtUnfixDialogEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	void		_SendUnfixData( const stUnfix& unfix, int nUnfixNum=1 );
	void		_StartUnfix( stUnfix& unfix );

	static void _evtLostYesNoEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	int			_nLostGridID;

	typedef	vector<CSkillRecord*>	vskill;
	vskill		_skills;

	typedef vector<int>	ints;
	vskill		_cancels;			// �ɱ�����ļ���״̬

	typedef vector<CSkillStateRecord*>  states;
	states		_charges;			// ���ļ���״̬

	static CGuiPic	_imgCharges[enumEQUIP_NUM];

	struct SSplitItem
	{
		static void		_evtSplitItemEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);	// ��ֵ��߻ص�
		int			nFirst;
		int			nSecond;
		CCharacter*	pSelf;
	};
	static SSplitItem	SSplit;

public:	// ����������
	void	ShowRepairMsg( const char* pItemName, long lMoney );
	void    SetIsShow(bool bShow);

private:
	static void		_evtRepairEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	
};

inline CSkillList* CEquipMgr::GetSkillList( char nType )	
{ 
	switch( nType )
	{
	case enumSKILL_SAIL: return lstSailSkill;
	case enumSKILL_FIGHT: return lstFightSkill;
	default: return lstLifeSkill;
	}
}	

}

