#pragma once
#include "UIGlobalVar.h"
#include "uipage.h"
#include "TableData.h"
#include <vector>
#include "UIList.h"
#include "UIForm.h"
#include "UIScroll.h"


namespace GUI
{
	typedef void(* MouseFUN)(CGuiData *pSender, int x, int y, DWORD key);  

	struct TeamProUnit //����������Ϣ
	{
		char PromCapName[50];//�ӳ���
		int  PromID;		 //����ID
		int  WinNum;		 //Ӯ����

		TeamProUnit()
		{
			memset(PromCapName,0,sizeof(PromCapName));
			PromID = 0;
			WinNum = 0;
		}
	};
	struct TeamRelUnit //���������Ϣ
	{
		char  RelCapName[50];//�ӳ���
		int   Ballot;		 //����Ʊ��
		int	  RelID;		 //����ID
		TeamRelUnit()
		{
			memset(RelCapName,0,sizeof(RelCapName));
			Ballot = 0;
			RelID = 0;
		}
	};

	//���������а�����
	class AmphitheaterData 
	{
	public:
		AmphitheaterData();
		~AmphitheaterData();
	public:
		// add
		void AddTeamPro(const TeamProUnit* _tp);
		void AddTeamRel(const TeamRelUnit* _tr);
		// delete
		void DelTeamPro(int TeamID);
		void DelTeamRel(int TeamID);

		void DelTeamProAll();
		void DelTeamRelAll();
		// get
		TeamProUnit * GetTeamPro(int index);
		TeamRelUnit * GetTeamRel(int index);

		std::vector<TeamProUnit> GetTeamProList(){return TeamProList;}
		std::vector<TeamRelUnit> GetTeamRelList(){return TeamRelList;}


		// update
		void Update(){};

	private:
		std::vector<TeamProUnit> TeamProList;	// �����������ݱ�
		std::vector<TeamRelUnit> TeamRelList;	// ����������ݱ�

	};

	//�����������б�
	class CAmphitheaterList:public CList
	{
	public:
		CAmphitheaterList(CForm& frmOwn);
		CAmphitheaterList(const CAmphitheaterList&  p);
		CAmphitheaterList& operator =(const CAmphitheaterList& p);
		GUI_CLONE(CAmphitheaterList);//clone button

		virtual ~CAmphitheaterList();
		virtual void		Render();
		virtual void		Refresh();
	
		
		virtual bool MouseScroll( int nScroll );
		virtual bool MouseRun( int x, int y, DWORD key );
	

		CTextButton*	GetButton()			{ return _buttonPip;}
		//void SetCloneButtonPic();
		//static void MouseClick(CGuiData *pSender, int x, int y, DWORD key);

		void SetMouseFun(MouseFUN pf);

	public:
		CTextButton*		_button[20];

	protected:
		int             _nSelectIndex;  // ѡ�������
		bool            _IsShowUpgrade;

		int             _nUnitHeight, _nUnitWidth;   // ��ԪͼƬ���
	private:
		int					_ButtonLen;
		CTextButton*		_buttonPip;
		

		static CAmphitheaterList* g_pAmphiList;

	};

	//���������а���
	class CAmphitheaterForm : public CUIInterface
	{
	public:
		static CAmphitheaterForm * m_pStaticAmphiForm;


	public:
		void RefreshAmphitheaterData(int index);//�л�ҳ��ˢ������
		void RefreshAmphitheaterData(AmphitheaterData& data);//д������

		static void ListMouseRun(CGuiData *pSender, int x, int y, DWORD key);

	public:
		void ShowAmphitheaterFrom( bool bShow = true );
	protected:
		virtual bool Init();
		virtual void CloseForm();
		virtual void LoadingCall();
		virtual void Clear();
		virtual void FrameMove(DWORD dwTime){};

	private:
		CForm       *frmNPCAmphitheater;

		CAmphitheaterList*		m_pSecPromotionList;                // ������
		CAmphitheaterList*		m_pSecReliveList;					// ������
		AmphitheaterData        m_pAmphitheaterData;
		CTextButton		*		m_pSecExListbt;


		CPage*			listPage;

		void			RefreshUpgrade();
	private:
		static void		_evtPageIndexChange(CGuiData *pSender);

	};
}
