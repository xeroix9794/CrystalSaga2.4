#pragma once

#include "UIForm.h"
#include "UIGlobalVar.h"
#include "UITextButton.h"
#include "UICompent.h"
namespace GUI
{
	class CItemCommand;
	class CCombo;

	class AllButtonMgr : public CUIInterface
	{
	public:
		static	void _evtAllButtonMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
		virtual bool Init();
		static	void evtCloseCompany( CGuiData* pSender);
		static	void evtbtnClickDown(CGuiData *pSender,int x,int y ,DWORD key);

		void	ShowAllForm(bool bShow);
		void	GetLeaveTime(int iTime);
		void	CloseAllForm(bool bClose);
		static void	CloseAllFormButThis(string FormName);
	private:
		static int JudgeGost();

	private:
		CForm*			frmAllBtn;
		CForm*			frmElfCoin;
		//CTextButton*	btnTxt03;	//	�����ť������Ĳ�һ����
		CTextButton*	btnTxt01;
		CTextButton*	btnTxt02;
		CTextButton*	btnTxt03;
		static CCombo*			cboCion1;
		static CCombo*			cboCion2;
		static CCombo*			cboCion3;

		CTextButton*	btnHelp;

		// װ������Lv5����װ�ۺ�Lv5��װ����ĥLv5����ʯ�ϳ�Lv5��
		// ����ת��lv4��������Lv4����װǿ��Lv5��
		// ���߹һ�Lv1������Ӳ������Lv1��װ����������lv4��������lv1�����������lv1		
		
		static const int Max_Button = 12;
		CTextButton* btnCommand[Max_Button];
		int btnCommandCondition[Max_Button];
	};
}