//----------------------------------------------------------------------
// ����:����ʵ��������Ϣ����
// ����:zcj 2008-8-7
// ����޸�����:2008-8-7
//----------------------------------------------------------------------
#pragma once
#include "UIGlobalVar.h"
#include <list>
class CCharacter2D;

#include "netprotocol.h"

extern int SMALL_ICON_SIZE;
extern int BIG_ICON_SIZE;

namespace GUI
{
class CGuiData;
class CMenu;
class CTreeNodeObj;
class CItemObj;
class CGuiTime;
class CTextButton;
class CEdit;
class CLabel;
class CMember;


//struct stPersonInfo
//{
//	char	szMotto[40];				// ǩ��
//	bool	bShowMotto;					// ��ʾǩ������
//	char	szSex[10];					// �Ա�
//	short	sAge;						// ����
//	char	szName[50];					// ����
//	char	szAnimalZodiac[50];			// ����
//	char	szBloodType[50];			// Ѫ��
//	int		iBirthday;					// ���գ���
//	char	szState[50];				// �ݣ�ʡ��
//	char	szCity[50];					// ���У�����
//	char	szConstellation[50];		// ����
//	char	szCareer[50];				// ְҵ
//	int		iSize;						// ͷ���С([8 * 1024])
//	char	pAvatar[8096];				// ͷ��
//	bool	bPprevent;					// �Ƿ���ֹ��Ϣ
//	int		iSupport;					// �ʻ���
//	int		iOppose;					// ��������
//	stPersonInfo&  operator =(const stPersonInfo & _info)
//	{
//		strcpy(szMotto,_info.szMotto);
//		bShowMotto = _info.bShowMotto;
//		strcpy(szSex,_info.szSex);
//		sAge = _info.sAge;
//		strcpy(szName,_info.szName);
//		strcpy(szAnimalZodiac,_info.szAnimalZodiac);
//		strcpy(szBloodType,_info.szBloodType);
//		iBirthday = _info.iBirthday;
//		strcpy(szState,_info.szState);
//		strcpy(szCity,_info.szCity);
//		strcpy(szConstellation,_info.szConstellation);
//		strcpy(szCareer,_info.szCareer);
//		iSize = _info.iSize;
//		strcpy(pAvatar,_info.pAvatar);
//		bPprevent = _info.bPprevent;
//		iSupport = _info.iSupport;
//		iOppose = _info.iOppose;
//		return *this;
//	}
//
//
//};
//struct stQueryPersonInfo
//{
//	char	sChaName[64];			// ����
//	bool	bHavePic;				// �Ƿ���ʾͷ��
//	char	cSex[4];				// �Ա�	  1���� 0��Ů��
//	int		nMinAge[2];				// �����ѯ��m_nMinAge[1]����m_nMinAge[0]��Сֵ
//	char	szAnimalZodiac[4];		// ��Ф
//	int		iBirthday[2];			// ��ѯ�������m_nBirth[1]����m_nBirth[0]��Сֵ
//									 0xffff0000&month   0xffff&day
//	char	szState[32];			// ʡ��
//	char	szCity[32];				// ����
//	char	szConstellation[16];	// ����		
//	char	szCareer[32];			// ����
//	int		nCurPage;				// ��ǰҳ��
//	stQueryPersonInfo& operator =(const stQueryPersonInfo& _info)
//	{
//		strcpy(sChaName,_info.sChaName);
//		bHavePic = _info.bHavePic;
//		strcpy(cSex,_info.cSex);
//		nMinAge[0] = _info.nMinAge[0];
//		nMinAge[1] = _info.nMinAge[1];
//		strcpy(szAnimalZodiac,_info.szAnimalZodiac);
//		iBirthday[0] = _info.iBirthday[0];
//		iBirthday[1] = _info.iBirthday[1];
//		strcpy(szState,_info.szState);
//		strcpy(szCity,_info.szCity);
//		strcpy(szConstellation,_info.szConstellation);
//		strcpy(szCareer,_info.szCareer);
//		nCurPage = _info.nCurPage;
//		return *this;
//	}
//
//};
//struct stQueryResoultPersonInfo
//{
//	char	sChaName[64];		// ����
//	int		nMinAge;			// ����
//	char	cSex[4];			// �Ա�	  1���� 0��Ů��
//	char	szState[32];		// ʡ��
//	char	nCity[32];			// ����
//	char	pAvatar[8096];		// ͷ��ͼƬ
//	stQueryResoultPersonInfo& operator =(const stQueryResoultPersonInfo& _info)
//	{
//		strcpy(sChaName,_info.sChaName);
//		nMinAge = _info.nMinAge;
//		strcpy(cSex,_info.cSex);
//		strcpy(szState,_info.szState);
//		strcpy(nCity,_info.nCity);
//		strcpy(pAvatar,_info.pAvatar);
//		return   *this;
//	}
//};
//
class CChaQeryInfo : public CUIInterface
{
public:
   

    CChaQeryInfo();

    bool Init();
    void End();

    void RefreshInfo();								 // ˢ���������
	
	void RefreshInfoData( CMember* pCurMember );	// ˢ����������		

    void ClearInfo();								// ����������

	void SetShowChaInfo(bool _b);

	void SetShowQChaInfo(bool _b);

	void SetShowQRChaInfo(bool _b);

	bool SetSearchFrmData(stPersonInfo* _info,CMember* _m);

	stPersonInfo* GetSearchFrmData();

	bool SetQueryFrmData(stQueryPersonInfo* _info);

	stQueryPersonInfo* GetQueryFrmData();

	bool SetQueryRFrmData(stQueryResoultPersonInfo* _info,int num);

	stQueryResoultPersonInfo* GetQueryRFrmData();

	void  ClearQueryRFrmData();			// ��ղ�ѯ�������ϴε�����

	void UpdateInfo();

	void FrameMove(DWORD dwTime);
 

private:
	static bool _Error(const char* strInfo, const char* strFormName, const char* strCompentName) {
		LG("gui", strInfo, strFormName, strCompentName );
		return false;
	}

    static void _RenderEvent(C3DCompent *pSender, int x, int y);
	static void _MemberMouseDownEvent(CGuiData *pSender, int x, int y, DWORD key);

	static void _evSFrm(CGuiData *pSender, int x, int y, DWORD key);
	static void _evQFrm(CGuiData *pSender, int x, int y, DWORD key);

public:
	// data
	stPersonInfo				m_ShowInfo;		// �����Ϣ
	stQueryPersonInfo			m_QeryInfo;		// ��ѯ��Ϣ
	stQueryResoultPersonInfo	m_ResoultInfo;	// ��ѯ�����Ϣ
	typedef std::list<stQueryResoultPersonInfo>  ResoultList;
	ResoultList					m_ResoultInfoList;// ��ѯ�����Ϣ�б�

	//UI
	static CForm*         m_pFrmSInfo;
	static CForm*		  m_pFrmQInfo;	
	static CForm*		  m_pFrmQRInfo;

	// form   search info
	CLabel*				m_pEdtTradeGold;			//	������
	CLabel*				m_pLabName;					//  �û�����
	CLabel*				m_pChaguild;				//  ��������
	CLabel*				m_pChaLv;					//  ��ɫ�ȼ�
	CLabel*				m_pChaJob;					//	ְҵ
	CLabel*				m_pReputation;				//  ����
	CLabel*				m_pChaNickName;				//   ��ɫ�ƺ�
	CLabel*				m_pSupport;					//  ֧�ֶ�(�ʻ�)
	CLabel*				m_pOppose;					//	���ԣ���������
	CLabel*				m_pAttention;				//  ��ע��
	CLabel*				m_pChaSex;					//   ��ɫ�Ա�
	CLabel*				m_pChaAge;					//   ��ɫ����
	CLabel*				m_pChaName;					//	 �������
	CLabel*				m_pAnimalZodiac;			//	 ����
	CLabel*				m_pBloodType;				//	 Ѫ��
	CLabel*				m_pBirthMonth;				//	 ������
	CLabel*				m_pBirthDay;				//	 ������
	CLabel*				m_pState;					//	 ����ʡ
	CLabel*				m_pCity;					//	 ������
	CLabel*				m_pConstellation;			//	 ����
	CLabel*				m_pJob;						//	 ���������ְҵ
	CLabel*				m_pIdiograph;				//	 ���ǩ��
	CTextButton*		m_pAddFlowerBt;				//	 �ʻ����
	CTextButton*		m_pAddEggBt;				//	 ���������
	CTextButton*		m_pChangePhoto;				//	 ����ͷ��Ť
	CImage*				m_pChaPhoto;				//	 ͷ��
	//CCheckBox*			m_pStopChat;				//	 ����
	CTextButton*		m_pBtYes;					//	 ȷ����Ť

	// form query info
	CEdit*				m_pQchaName;				//   ��ɫ����
	CEdit*				m_pQHasPic;					//   ��ɫ�Ƿ�����Ƭ
	CEdit*				m_pQSex;					//	 ��ɫ�Ա�
	CEdit*				m_pQminAge;					//	 ��ɫ������С
	CEdit*				m_pQmaxAge;					//	 ��ɫ�������
	CEdit*				m_pQAnimalZodiac;			//	 ��ɫ����
	CEdit*				m_pQBirthmonth0;			//	 ��ɫ���գ��£���һ��
	CEdit*				m_pQBirthmonth1;			//	 ��ɫ���գ��£��ڶ���
	CEdit*				m_pQBirthday0;				//	 ��ɫ���գ��գ���һ��
	CEdit*				m_pQBirthday1;				//	 ��ɫ���գ��գ��ڶ���
	CEdit*				m_pQstate;					//	 ��ɫʡ��
	CEdit*				m_pQcity;					//	 ��ɫ����
	CEdit*				m_pQConstellation;			//	 ��ɫ����
	CEdit*				m_pQjob;					//	 ��ɫְҵ
	CTextButton*		m_pQYes;					//	 ��ɫȷ��
	CTextButton*		m_pPicBox1;					//	 ���1
	CTextButton*		m_pPicBox2;					//	 ���2
//	CTextButton*		m_pAddPicBox;				//	 �������


	// for  resoult query info
	CList*				m_pQRList;					//	 ��ѯ��ʾ����б�
};

}
extern CChaQeryInfo g_ChaQeryInfo;


