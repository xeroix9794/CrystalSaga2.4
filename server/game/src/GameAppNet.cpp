#include "Stdafx.h"
#include "GameApp.h"
#include "GameAppNet.h"
#include "SubMap.h"
#include "MapEntry.h"
#include "CharTrade.h"
#include "GameDB.h"
#include "Guild.h"
extern std::string g_strLogName;

//	2008-8-20	yangyinyu	add	begin!

//	������ͼ(������)��
void	lgtool_printCharacter(	const	char*	sType,	CCharacter *pCCha	)
{
	//
	static	char	sText[2048];
	static	char	sTextEx[2048];

	//_snprintf(	sText,	2048,	"%s%s;%d;%d;%d;%d;%d;%d;%d;%d;",
	//	sType,							//	���⡣
	//	pCCha->GetName(),				//	�������֡�
	//	pCCha->GetID(),					//	����ID��
	//	pCCha->GetLevel(),				//	����ȼ���
	//	pCCha->getAttr(	ATTR_STR	),	//	������������ԡ�
	//	pCCha->getAttr(	ATTR_DEX	),	//	������������ԡ�
	//	pCCha->getAttr(	ATTR_AGI	),	//	
	//	pCCha->getAttr(	ATTR_CON	),	//	
	//	pCCha->getAttr(	ATTR_STA	),	//	
	//	pCCha->getAttr( ATTR_AP	)		//	��ʾʣ�����Ե㡣
	//	);
	_snprintf_s(	sText,sizeof(sText),_TRUNCATE,	"%s%s;%d;%d;%d;%d;%d;%d;%d;%d;",
		sType,							//	���⡣
		pCCha->GetName(),				//	�������֡�
		pCCha->GetID(),					//	����ID��
		pCCha->GetLevel(),				//	����ȼ���
		(long)pCCha->getAttr(	ATTR_STR	),	//	������������ԡ�
		(long)pCCha->getAttr(	ATTR_DEX	),	//	������������ԡ�
		(long)pCCha->getAttr(	ATTR_AGI	),	//	
		(long)pCCha->getAttr(	ATTR_CON	),	//	
		(long)pCCha->getAttr(	ATTR_STA	),	//	
		(long)pCCha->getAttr( ATTR_AP	)		//	��ʾʣ�����Ե㡣
		);

	//	��ӡ�������ߡ�
	USHORT sNum =	pCCha->GetKitbag()->GetUseGridNum();

	for(	int	i	=	0;	i	<	sNum;	i	++	)
	{
		SItemGrid*	pGrid	=	pCCha->GetKitbag()->GetGridContByNum(	i	);
		if(	pGrid	)
		{
			CItemRecord*	pItem	=	GetItemRecordInfo(	pGrid->sID );
			if(	pItem	)
			{
				//_snprintf(	sTextEx,	2048,	"%s(%d),",	pItem->szName,	pGrid->sNum	);
				_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,	"%s(%d),",	pItem->szName,	pGrid->sNum	);
				//strncat(	sText,	sTextEx,	2048	);
				strncat_s( sText,sizeof(sText), sTextEx,_TRUNCATE);
			};
		};
	};
	//strncat(	sText,	";",	2048	);
	strncat_s( sText,sizeof(sText), ";",_TRUNCATE);

	//	��ӡ���е��ߡ�
	if(	pCCha->GetPlayer()	)
	{
		CKitbag*	bank	=	pCCha->GetPlayer()->GetBank();

		if(	bank	)
		{
			for(	int	i	=	0;	i	<	bank->GetUseGridNum();	i	++	)
			{
				SItemGrid*	pGrid	=	bank->GetGridContByNum(	i	);
				if(	pGrid	)
				{
					CItemRecord*	pItem	=	GetItemRecordInfo(	pGrid->sID );
					if(	pItem	)
					{
						if(	pGrid->dwDBID )
							//_snprintf(	sTextEx,	2048,	"%s-%d-%d,",	pItem->szName,	pGrid->sNum,	pGrid->dwDBID	);
							_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,"%s-%d-%d,",	pItem->szName,	pGrid->sNum,	(long)pGrid->dwDBID	);
						else
							//_snprintf(	sTextEx,	2048,	"%s-%d,",	pItem->szName,	pGrid->sNum	);
							_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,	"%s-%d,",	pItem->szName,	pGrid->sNum	);
						//strncat(	sText,	sTextEx,	2048	);
						strncat_s( sText,sizeof(sText), sTextEx,_TRUNCATE);
					};
				};
			};
		};
	};

	//	����ʱ��ӡ���С�
	//strncat(	sText,	"\n",	2048	);
	strncat_s( sText,sizeof(sText), "\n",_TRUNCATE);

	//
	LG(	"query_cha",	sText	);
};

//	��ӡ��ֻ�ĵ���( �����������죬���� )��
void	lgtool_printBoat(	const	char*	sType,	CCharacter*	pCCha	)
{
	//	ȡ�����ﵱǰ�Ĵ�ֻ��
	CCharacter*	pBoat	=	pCCha->GetBoat();

	if(	!pBoat	)
	{
		return;
	};

	//	��ӡ������Ϣ��
	static	char	sText[2048];
	static	char	sTextEx[2048];

	//_snprintf(	sText,	2048,	"%s%s;%s;%d;",
	//	sType,				//	���⡣
	//	pCCha->GetName(),	//	�������֡�
	//	pBoat->GetName(),	//	�����֡�
	//	pBoat->GetLevel()	//	���ȼ���
	//	);
	_snprintf_s(	sText,sizeof(sText),_TRUNCATE,	"%s%s;%s;%d;",
		sType,				//	���⡣
		pCCha->GetName(),	//	�������֡�
		pBoat->GetName(),	//	�����֡�
		pBoat->GetLevel()	//	���ȼ���
		);

	//	��ӡ���е��ߡ�
	USHORT sNum =	pBoat->GetKitbag()->GetUseGridNum();

	for(	int	i	=	0;	i	<	sNum;	i	++	)
	{
		SItemGrid*	pGrid	=	pBoat->GetKitbag()->GetGridContByNum(	i	);
		if(	pGrid	)
		{
			CItemRecord*	pItem	=	GetItemRecordInfo(	pGrid->sID );
			if(	pItem	)
			{
				//_snprintf(	sTextEx,	2048,	"%s(%d),",	
				//	pItem->szName,		//	�������֡�
				//	pGrid->sNum			//	����������
				//	);
				_snprintf_s(	sTextEx,sizeof(sTextEx),_TRUNCATE,	"%s(%d),",	
					pItem->szName,		//	�������֡�
					pGrid->sNum			//	����������
					);

				//strncat(	sText,	sTextEx,	2048	);
				strncat_s( sText,sizeof(sText), sTextEx,_TRUNCATE);
			};
		};
	};

	//strncat(	sText,	"\n",	2048	);
	strncat_s( sText,sizeof(sText), "\n",_TRUNCATE);

	//	дLOG��
	LG(	"query_boat",	sText	);

};
//	2008-8-20	yangyinyu	add	end!


// ��ʱ�˳��л�
//#define CHAEXIT_ONTIME

//  ��ȡ������Ϣ��Ψһ���
void CGameApp::ProcessNetMsg(int nMsgType, GateServer* pGate, RPACKET pkt)
{T_B
    switch (nMsgType)
    {
    case NETMSG_GATE_CONNECTED: // ������Gate
        {
	    LG("Connect", "Exec OnGateConnected()\n");
        OnGateConnected(pGate, pkt);
        break;}

    case NETMSG_GATE_DISCONNECT: // ��Gate�Ͽ�����
        {
			LG("Connect", "Exec OnGateDisconnect()\n");
        OnGateDisconnect(pGate, pkt);
        break;}

    case NETMSG_PACKET: // �յ���Ϣ��
        {
        ProcessPacket(pGate, pkt);
        break;}

    }
T_E}

void CGameApp::ProcessInfoMsg(pNetMessage msg, short sType, TradeServer *pInfo)
{T_B
    switch(sType)
    {
    case TradeServer::CMD_FM_CONNECTED:
        OnInfoConnected(pInfo);
        break;

    case TradeServer::CMD_FM_DISCONNECTED:
        OnInfoDisconnected(pInfo);
        break;

    case TradeServer::CMD_FM_MSG:
        ProcessMsg(msg, pInfo);
        break;

    default:
        break;
    }
T_E}

void CGameApp::OnInfoConnected(TradeServer *pInfo)
{T_B
	//��¼TradeServer
	pInfo->Login();
T_E}

void CGameApp::OnInfoDisconnected(TradeServer *pInfo)
{T_B
	// �̳�ϵͳ�ر�
	g_StoreSystem.InValid();
	pInfo->InValid();
T_E}

void CGameApp::ProcessMsg(pNetMessage msg, TradeServer *pInfo)
{T_B
	if(msg)
	{
		switch(msg->msgHead.msgID)
		{
		case INFO_LOGIN:		// ��¼TradeServer
			{
				if(msg->msgHead.subID == INFO_SUCCESS)
				{
					LG("Store_data", "TradeServer Login Success!\n");

					pInfo->SetValid();
					//g_StoreSystem.SetValid();

					//��TradeServer��ȡ�̳��б�͹����б�
					g_StoreSystem.GetItemList();
					g_StoreSystem.GetAfficheList();
				}
				else if(msg->msgHead.subID == INFO_FAILED)
				{
					LG("Store_data", "TradeServer Login Failed!\n");
					pInfo->InValid();
				}
				else
				{
					//LG("Store_data", "��¼TradeServer�������ݴ���!\n");
					LG("Store_data", "enter TradeServer message data error!\n");
				}
			}
			break;

		case INFO_REQUEST_ACCOUNT:	// ��ȡ�ʻ���Ϣ
			{
				if(msg->msgHead.subID == INFO_SUCCESS)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]��ȡ�ʻ���Ϣ�ɹ�!\n", lOrderID);
					LG("Store_data", "[%I64i]succeed to obtain account information!\n", lOrderID);

					RoleInfo *ChaInfo = (RoleInfo *)((char *)msg->msgBody + sizeof(long long));
					g_StoreSystem.AcceptRoleInfo(lOrderID, ChaInfo);
				}
				else if(msg->msgHead.subID == INFO_FAILED)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]��ȡ�ʻ���Ϣʧ��!\n", lOrderID);
					LG("Store_data", "[%I64i]obtain account information failed!\n", lOrderID);

					g_StoreSystem.CancelRoleInfo(lOrderID);
				}
				else
				{
					//LG("Store_data", "�ʻ���Ϣ�������ݴ���!\n");
					LG("Store_data", "account information message data error");
				}
			}
			break;

		case INFO_REQUEST_STORE:	// ��ȡ�̳���Ϣ
			{
				//LG("Store_data", "ȡ���̳��б�!\n");
				LG("Store_data", "get store list!\n");
				if(msg->msgHead.subID == INFO_SUCCESS)		// �̳���Ϣ����
				{
					//��Ʒ����
					short lComNum = LOWORD(msg->msgHead.msgExtend);
					//�������
					short lClassNum = HIWORD(msg->msgHead.msgExtend);
					//���÷����б�
					g_StoreSystem.SetItemClass((ClassInfo *)(msg->msgBody), lClassNum);
					//������Ʒ�б�
					g_StoreSystem.SetItemList((StoreStruct *)((char *)msg->msgBody + lClassNum * sizeof(ClassInfo)), lComNum);

					g_StoreSystem.SetValid();
				}
				else if(msg->msgHead.subID == INFO_FAILED) // �̳���Ϣ����
				{
					//��Ʒ����
					short lComNum = LOWORD(msg->msgHead.msgExtend);
					//�������
					short lClassNum = HIWORD(msg->msgHead.msgExtend);
					//���÷����б�
					g_StoreSystem.SetItemClass((ClassInfo *)(msg->msgBody), lClassNum);
					//������Ʒ�б�
					g_StoreSystem.SetItemList((StoreStruct *)((char *)msg->msgBody + lClassNum * sizeof(ClassInfo)), lComNum);
				}
				else
				{
					//LG("Store_data", "�̳��б������ݴ���!\n");
					LG("Store_data", "store list message data error!\n");
				}
			}
			break;

		case INFO_REQUEST_AFFICHE:		// ��ȡ������Ϣ
			{
				//LG("Store_data", "ȡ�ù�����Ϣ!\n");
				LG("Store_data", "get offiche information!\n");
				if(msg->msgHead.subID == INFO_SUCCESS) // ������Ϣ����
				{
					//�������
					long lAfficheNum = msg->msgHead.msgExtend;
					//���ù����б�
					g_StoreSystem.SetAfficheList((AfficheInfo *)msg->msgBody, lAfficheNum);
				}
				else if(msg->msgHead.subID == INFO_FAILED) // ������Ϣ����
				{
					//�������
					long lAfficheNum = msg->msgHead.msgExtend;
					//���ù����б�
					g_StoreSystem.SetAfficheList((AfficheInfo *)msg->msgBody, lAfficheNum);
				}
				else
				{
					//LG("Store_data", "������Ϣ�������ݴ���!\n");
					LG("Store_data", "offiche information message data error!\n");
				}
			}
			break;

			// Add by lark.li 20090218 begin
		case INFO_STORE_BUY_RETURN:
			{
				if(msg->msgHead.subID == INFO_SUCCESS) // ����ɹ�
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]��������˿�ɹ�!\n", lOrderID);
					LG("Store_data", "[%I64i]succeed to buy return item!\n", lOrderID);

					RoleInfo *ChaInfo = (RoleInfo *)((char *)msg->msgBody + sizeof(long long));
					g_StoreSystem.AcceptReturn(lOrderID, ChaInfo);
				}
				else if(msg->msgHead.subID == INFO_FAILED) // ����ʧ��
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]��������˿�ʧ��!\n", lOrderID);
					LG("Store_data", "[%I64i]buy item return failed!\n", lOrderID);

					RoleInfo *ChaInfo = (RoleInfo *)((char *)msg->msgBody + sizeof(long long));
					g_StoreSystem.AcceptReturn(lOrderID, ChaInfo);
				}
				else
				{
					//LG("Store_data", "���߹����˿�ȷ����Ϣ�������ݴ���!\n");
					LG("Store_data", "confirm information that buy item return message data error!\n");
				}
			}
			break;
			// End

		case INFO_STORE_BUY:		// �������
			{
				if(msg->msgHead.subID == INFO_SUCCESS) // ����ɹ�
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]������߳ɹ�!\n", lOrderID);
					LG("Store_data", "[%I64i]succeed to buy item!\n", lOrderID);

					RoleInfo *ChaInfo = (RoleInfo *)((char *)msg->msgBody + sizeof(long long));
					g_StoreSystem.Accept(lOrderID, ChaInfo);
				}
				else if(msg->msgHead.subID == INFO_FAILED) // ����ʧ��
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]�������ʧ��!\n", lOrderID);
					LG("Store_data", "[%I64i]buy item failed!\n", lOrderID);

					g_StoreSystem.Cancel(lOrderID);
				}
				else
				{
					//LG("Store_data", "���߹���ȷ����Ϣ�������ݴ���!\n");
					LG("Store_data", "confirm information that buy item message data error!\n");
				}
			}
			break;

		case INFO_REGISTER_VIP:
			{
				if(msg->msgHead.subID == INFO_SUCCESS)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]����VIP�ɹ�!\n", lOrderID);
					LG("Store_data", "[%I64i] buy VIP succeed !\n", lOrderID);

					RoleInfo *ChaInfo = (RoleInfo *)((char *)msg->msgBody + sizeof(long long));

					g_StoreSystem.AcceptVIP(lOrderID, ChaInfo, msg->msgHead.msgExtend);
				}
				else if(msg->msgHead.subID == INFO_FAILED)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]����VIPʧ��!\n", lOrderID);
					LG("Store_data", "[%I64i] buy VIP failed !\n", lOrderID);

					g_StoreSystem.CancelVIP(lOrderID);
				}
				else
				{
					//LG("Store_data", "����VIPȷ����Ϣ�������ݴ���!\n");
					LG("Store_data", "buy VIP confirm information message data error !\n");
				}
			}
			break;

		case INFO_EXCHANGE_MONEY:		// �һ�����
			{
				if(msg->msgHead.subID == INFO_SUCCESS)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]�һ����ҳɹ�!\n", lOrderID);
					LG("Store_data", "[%I64i]change token succeed !\n", lOrderID);

					RoleInfo *ChaInfo = (RoleInfo *)((char *)msg->msgBody + sizeof(long long));
					g_StoreSystem.AcceptChange(lOrderID, ChaInfo);
				}
				else if(msg->msgHead.subID == INFO_FAILED)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]�һ�����ʧ��!\n", lOrderID);
					LG("Store_data", "[%I64i]change token failed!\n", lOrderID);

					g_StoreSystem.CancelChange(lOrderID);
				}
				else
				{
					//LG("Store_data", "�һ�����ȷ����Ϣ�������ݴ���!\n");
					LG("Store_data", "change token confirm information message data error !\n");
				}
			}
			break;

		case INFO_REQUEST_HISTORY:		// ��ѯ���׼�¼
			{
				if(msg->msgHead.subID == INFO_SUCCESS)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]��ѯ���׼�¼�ɹ�!\n", lOrderID);
					LG("Store_data", "[%I64i]succeed to query trade note!\n", lOrderID);

					HistoryInfo *pRecord = (HistoryInfo *)((char *)msg->msgBody + sizeof(long long));
					g_StoreSystem.AcceptRecord(lOrderID, pRecord);
				}
				else if(msg->msgHead.subID == INFO_FAILED)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]��ѯ���׼�¼ʧ��!\n", lOrderID);
					LG("Store_data", "[%I64i]query trade note failed!\n", lOrderID);

					g_StoreSystem.CancelRecord(lOrderID);
				}
				else
				{
					//LG("Store_data", "���׼�¼��ѯӦ����Ϣ�������ݴ���!\n");
					LG("Store_data", "trade note query resoibsuib nessage data error!\n");
				}
			}
			break;

		case INFO_SND_GM_MAIL:
			{
				if(msg->msgHead.subID == INFO_SUCCESS)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]����GM�ʼ��ɹ�!\n", lOrderID);
					LG("Store_data", "[%I64i]send GM mail success!\n", lOrderID);

					long lMailID = msg->msgHead.msgExtend;
					g_StoreSystem.AcceptGMSend(lOrderID, lMailID);
				}
				else if(msg->msgHead.subID == INFO_FAILED)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]����GM�ʼ�ʧ��!\n", lOrderID);
					LG("Store_data", "[%I64i]send GM mail failed!\n", lOrderID);

					g_StoreSystem.CancelGMSend(lOrderID);
				}
				else
				{
					//LG("Store_data", "����GM�ʼ��������ݴ���!\n");
					LG("Store_data", "send GM mail message data error!\n");
				}
			}
			break;

		case INFO_RCV_GM_MAIL:
			{
				if(msg->msgHead.subID == INFO_SUCCESS)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]����GM�ʼ��ɹ�!\n", lOrderID);
					LG("Store_data", "[%I64i]receive GM mail success!\n", lOrderID);

					MailInfo *pMi = (MailInfo *)((char *)msg->msgBody + sizeof(long long));
					g_StoreSystem.AcceptGMRecv(lOrderID, pMi);
				}
				else if(msg->msgHead.subID == INFO_FAILED)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					//LG("Store_data", "[%I64i]����GM�ʼ�ʧ��!\n", lOrderID);
					LG("Store_data", "[%I64i]reciveGMmail failed!\n", lOrderID);

					g_StoreSystem.CancelGMRecv(lOrderID);
				}
				else
				{
					//LG("Store_data", "����GM�ʼ��������ݴ���!\n");
					LG("Store_data", "receive GM mail message data error!\n");
				}
			}
			break;

		case INFO_EXCEPTION_SERVICE:	//�ܾ�����
			{
				//LG("Store_data", "TradeServer�ܾ�����!\n");
				LG("Store_data", "TradeServer refuse serve!\n");
				g_StoreSystem.InValid();
				pInfo->InValid();
			}
			break;
//����2016ע�� ˮ���������Ʋ�������
#ifdef SHUI_JING
		case INFO_REQUEST_ACTINFO:
			{
				if(msg->msgHead.subID == INFO_SUCCESS)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					LG("Store_data", "[%I64i]succeed to obtain account information!\n", lOrderID);

					RoleInfo *ChaInfo = (RoleInfo *)((char *)msg->msgBody + sizeof(long long));
					g_StoreSystem.AcceptActInfo(lOrderID, ChaInfo);
				}
				else if(msg->msgHead.subID == INFO_FAILED)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					LG("Store_data", "[%I64i]obtain account information failed!\n", lOrderID);

					g_StoreSystem.CancelActInfo(lOrderID);
				}
				else
				{
					LG("Store_data", "account information message data error");
				}
			}
			break;
		case INFO_CRYSTAL_OP_ADD:
			{
				if(msg->msgHead.subID == INFO_SUCCESS)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					LG("Store_data", "[%I64i]succeed to obtain account information!\n", lOrderID);
					CrystalInfo* cryInfo = (CrystalInfo*)((char*)msg->msgBody + sizeof(long long));
					g_StoreSystem.AcceptCrystalAdd(lOrderID,cryInfo);
				}
				else if(msg->msgHead.subID == INFO_FAILED)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					LG("Store_data", "[%I64i]obtain account information failed!\n", lOrderID);
					CrystalInfo* cryInfo = (CrystalInfo*)((char*)msg->msgBody + sizeof(long long));
					g_StoreSystem.CancelCrystalAdd(lOrderID,cryInfo);
				}
				else
				{
					LG("Store_data", "AddCrystal information message data error");
				}			
			}
			break;
		case INFO_CRYSTAL_OP_DEL:
			{
				if(msg->msgHead.subID == INFO_SUCCESS)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					LG("Store_data", "[%I64i]succeed to obtain account information!\n", lOrderID);
					CrystalInfo* cryInfo = (CrystalInfo*)((char*)msg->msgBody + sizeof(long long));
					g_StoreSystem.AcceptCrystalDel(lOrderID,cryInfo);
				}
				else if(msg->msgHead.subID == INFO_FAILED)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					LG("Store_data", "[%I64i]obtain account information failed!\n", lOrderID);
					CrystalInfo* cryInfo = (CrystalInfo*)((char*)msg->msgBody + sizeof(long long));
					g_StoreSystem.CancelCrystalDel(lOrderID,cryInfo);
				}
				else
				{
					LG("Store_data", "DelCrystal information message data error");
				}			
			}
			break;

		case INFO_CRYSTAL_OP_ADD_RETURN:
			{
				if(msg->msgHead.subID == INFO_SUCCESS)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					LG("Store_data", "[%I64i]succeed to obtain account information!\n", lOrderID);
					CrystalInfo* cryInfo = (CrystalInfo*)((char*)msg->msgBody + sizeof(long long));
					g_StoreSystem.AcceptCrystalAddReturn(lOrderID,cryInfo);
				}
				else if(msg->msgHead.subID == INFO_FAILED)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					LG("Store_data", "[%I64i]obtain account information failed!\n", lOrderID);
					CrystalInfo* cryInfo = (CrystalInfo*)((char*)msg->msgBody + sizeof(long long));
					g_StoreSystem.CancelCrystalAddReturn(lOrderID,cryInfo);
				}
				else
				{
					LG("Store_data", "Addcrystal return  information message data error");
				}			
			}
			break;
		case INFO_CRYSTAL_OP_DEL_RETURN:
			{
				if(msg->msgHead.subID == INFO_SUCCESS)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					LG("Store_data", "[%I64i]succeed to obtain account information!\n", lOrderID);
					CrystalInfo* cryInfo = (CrystalInfo*)((char*)msg->msgBody + sizeof(long long));
					g_StoreSystem.AcceptCrystalDelReturn(lOrderID,cryInfo);
				}
				else if(msg->msgHead.subID == INFO_FAILED)
				{
					long long lOrderID = *(long long *)msg->msgBody;
					LG("Store_data", "[%I64i]obtain account information failed!\n", lOrderID);
					CrystalInfo* cryInfo = (CrystalInfo*)((char*)msg->msgBody + sizeof(long long));
					g_StoreSystem.CancelCrystalDelReturn(lOrderID,cryInfo);
				}
				else
				{
					LG("Store_data", "Delcrystal return information message data error");
				}			
			}
			break;
#endif
		default:
			{
				//LG("Store_data", "ȡ�õ���Ϣ���Ͳ���!\n");
				LG("Store_data", "get unknown information type!\n");
			}
			break;

		}

		FreeNetMessage(msg);
	}
T_E}

// ��Gate�����ϵĴ�����
void CGameApp::OnGateConnected(GateServer* pGate, RPACKET pkt)
{T_B
	// ��GateServerע�᱾GameServer
	WPACKET	wpk = GETWPACKET();
    WRITE_CMD(wpk, CMD_MT_LOGIN);
	WRITE_STRING(wpk, GETGMSVRNAME());
	WRITE_STRING(wpk, g_pGameApp->m_strMapNameList.c_str());

	LG("Connect", "[%s]\n", g_pGameApp->m_strMapNameList.c_str());

    pGate->SendData(wpk);
T_E}

// ��Gate�Ͽ����ӵĴ�����
void CGameApp::OnGateDisconnect(GateServer* pGate, RPACKET pkt)
{T_B
    bool ret = VALIDRPACKET(pkt);
    if (!ret) return; // ��Ч��packet

    GatePlayer* tmp = (GatePlayer *)MakePointer(READ_LONG(pkt));    

    while (tmp != NULL)
    {
		if (((CPlayer *)tmp)->IsValid())
		{
			GoOutGame((CPlayer *)tmp, true);
			tmp->OnLogoff();
		}

		tmp = tmp->Next;
	}

	pGate->Invalid();
T_E}

// ����������Ϣ��
void CGameApp::ProcessPacket(GateServer* pGate, RPACKET pkt)
{T_B
	CPlayer *l_player = 0;
    uShort cmd = READ_CMD(pkt);

	// ���������ô��룬������Ҫ 
	MONITOR_VALUE(MESSAGE_NAME(cmd))

	//DWORD dwLastTick = GetTickCount();

	//GameServerLG(g_strLogName.c_str(), "CGameApp::ProcessPacket (cmd = %d) Begin...\n", cmd);
	
	//LG("Thread", "ProcessPacket %d\n", ::GetCurrentThreadId());

	switch (cmd)
	{
	case CMD_TM_LOGIN_ACK:
		{
			short	sErrCode;
			if (sErrCode = READ_SHORT(pkt))
			{
				/*LG("GameLogin", "��¼ GateServer: %s:%dʧ��[%s], ע���ͼ[%s]\n",
					pGate->GetIP().c_str(), pGate->GetPort(), g_GameGateConnError(sErrCode),
                    g_pGameApp->m_strMapNameList.c_str());*/
				LG("GameLogin", "enter GateServer: %s:%d failed [%s], register map[%s]\n",
					pGate->GetIP().c_str(), pGate->GetPort(), g_GameGateConnError(sErrCode),
                    g_pGameApp->m_strMapNameList.c_str());
				DISCONNECT(pGate->GetDataSock());
			}
			else
			{
				pGate->GetName() = READ_STRING(pkt);
				if (!strcmp(pGate->GetName().c_str(), ""))
				{
					/*LG("GameLogin", "��¼ GateServer: [%s:%d]�ɹ� ��û���õ������֣������ж���������\n",
						pGate->GetName().c_str(), pGate->GetIP().c_str(), pGate->GetPort(),
						g_pGameApp->m_strMapNameList.c_str());*/
					LG("GameLogin", "entry GateServer: [%s:%d]success but do not get his name��so disconnection and entry again\n",
						pGate->GetName().c_str(), pGate->GetIP().c_str(), pGate->GetPort(),
						g_pGameApp->m_strMapNameList.c_str());

					DISCONNECT(pGate->GetDataSock());
				}
				else
				{
					/*LG("GameLogin", "��¼ GateServer: %s [%s:%d]�ɹ� [MapName:%s]\n",
						pGate->GetName().c_str(), pGate->GetIP().c_str(), pGate->GetPort(),
						g_pGameApp->m_strMapNameList.c_str());*/
					LG("GameLogin", "entry GateServer: %s [%s:%d]success [MapName:%s]\n",
						pGate->GetName().c_str(), pGate->GetIP().c_str(), pGate->GetPort(),
						g_pGameApp->m_strMapNameList.c_str());
				}
			}

			break;
		}
	//	// Add by lark.li 20080921 begin
	//case CMD_TM_DELETEMAP:
	//	{
	//		int reason = READ_LONG(pkt);

	//		LG("GameLogin", "Gate %s Ip %s %d deleted\r\n", pGate->GetName(), pGate->GetIP(), reason);

	//		pGate->Invalid();

	//		break;
	//	}
	//	//End

	case CMD_TM_ENTERMAP:
		{
			//LG("XXX", ",1\n");

			uLong   l_actid = READ_LONG(pkt);
			const char* pszPassword = READ_STRING(pkt);
			if(pszPassword == NULL)
				break;
			uLong	l_dbid = READ_LONG(pkt);
			uLong	l_worldid = READ_LONG(pkt);
			cChar *	l_map	= READ_STRING(pkt);
			if(l_map == NULL)
				break;
			Long	lMapCpyNO = READ_LONG(pkt);
			uLong	l_x		= READ_LONG(pkt);
			uLong	l_y		= READ_LONG(pkt);
			char	chLogin = READ_CHAR(pkt); // ��ɫ����(0)
			short	swiner  = READ_SHORT_R(pkt);
			uLong	l_gtaddr = READ_LONG_R(pkt);

			//�����ulChaDBId�����ͼ
			//LG("enter_map", "��ʼ�����ͼ cha_id = %d enter--------------------------\n", l_dbid);
			LG("enter_map", "start entry map cha_id = %d enter--------------------------\n", l_dbid);
			//EnterServerCall * pCall	= g_gmsvr->m_EnterProc.Get();
			//if(pCall)
			//{
			//	pCall->pGate = pGate;
			//	pCall->l_actid = l_actid;
			//	pCall->pszPassword = pszPassword;
			//	pCall->l_dbid = l_dbid;
			//	pCall->l_worldid = l_worldid;
			//	pCall->l_map = l_map;
			//	pCall->lMapCpyNO = lMapCpyNO;
			//	pCall->l_x=l_x ;
			//	pCall->l_y = l_y;
			//	pCall->chLogin = chLogin;
			//	pCall->swiner = swiner;
			//	pCall->l_gtaddr = l_gtaddr;
			//	g_gmsvr->GetProcessor()->AddTask(pCall);
			//}

			l_player = CreateGamePlayer(pszPassword, l_dbid, l_worldid, l_map, chLogin == 0 ? 0 : 1);			
            if (!l_player)
            {
				WPACKET pkret = GETWPACKET();
				WRITE_CMD(pkret, CMD_MC_ENTERMAP);
				WRITE_SHORT(pkret, ERR_MC_ENTER_ERROR);
				WRITE_LONG(pkret, l_dbid);
				WRITE_LONG(pkret, l_gtaddr);
				WRITE_SHORT(pkret, 1);
				pGate->SendData(pkret);
				//LG("enter_map", "��������ң�ID = %u��ʱ�������ڴ�ʧ�� \n", l_dbid);
				LG("error", "when create new palyer(ID = %u),assign memory failed \n", l_dbid);
                return ;
            }
			
            l_player->SetActLoginID(l_actid);
			l_player->SetGarnerWiner(swiner);
			l_player->GetLifeSkillinfo() = "";
			l_player->SetInLifeSkill(false);

			if (!chLogin) // ���ߣ��������л���ͼ��
				l_player->MisLogin();			

            //////////////////////////////////////////////////////////////////////////
            // ���gate server��Ӧ��ά����Ϣ
            ADDPLAYER(l_player, pGate, l_gtaddr);
            l_player->OnLogin();
            //////////////////////////////////////////////////////////////////////////
            
            CCharacter *pCCha = l_player->GetMainCha();
			if (pCCha->Cmd_EnterMap(l_map, lMapCpyNO, l_x, l_y, chLogin))
			{
				l_player->MisEnterMap();

				if (chLogin == 0) // ��ɫ����
				{
					NoticePlayerLogin(l_player);
				}
			}

			//LG("enter_map", "���������ͼ  [%s]================\n\n", pCCha->GetLogName());
			LG("enter_map", "end up entry map  [%s]================\n\n", pCCha->GetLogName());

			//	2008-8-20	yangyinyu	add	begin!
			//lgtool_printCharacter(	RES_STRING(GM_CHARACTER_CPP_00147),	pCCha	);
			//	2008-8-20	yangyinyu	add	end!

			//change by zcj 
			//game_db.SavePlayer(l_player, enumSAVE_TYPE_SWITCH);
			// ��Ϊ�����Ϊ�˷�ֹ���ƶ�����ģ��ٶ�������ֻ�ñ�������ؼ���
			game_db.OnlySavePosWhenBeSaved(l_player);

			// Add by lark.li 20090112 begin
			//extern ToMMS* g_ToMMS;
			//g_ToMMS->EnterMap(l_actid, pCCha->m_ID, l_map);
			// End
			break;
		}
	case CMD_TM_GOOUTMAP:
		{
			//LG("XXX", ",0\n");

			l_player = (CPlayer *)MakePointer(READ_LONG_R(pkt));
			DWORD	l_gateaddr = READ_LONG_R(pkt);

			if (!l_player)
			{
				LG("error", "CMD_TM_GOOUTMAP\n");
				break;
			}

			try
			{
				if (l_player->GetGateAddr() != l_gateaddr)
				{
					//LG("error", "���ݿ�ID: %u, ��ַ��ƥ�䣬����:%x, gate:%x,cmd=%d, ��Ч��(%d).\n", l_player->GetDBChaId(), l_player->GetGateAddr(), l_gateaddr,cmd, l_player->IsValidFlag());
					LG("error", "DB ID: %u, address not matching,local :%x, gate:%x,cmd=%d, validity(%d).\n", l_player->GetDBChaId(), l_player->GetGateAddr(), l_gateaddr,cmd, l_player->IsValidFlag());
					break;
				}
			}
			catch (...)
			{
				//LG("error", "===========================��Gate������ҵ�ַ����%p,cmd =%d\n", l_player, cmd);
				LG("error", "===========================from Gate plyaer's address error %p,cmd =%d\n", l_player, cmd);
				break;
			}
			if (!l_player->IsValid())
			{
				//LG("enter_map", "������Ѿ���Ч\n");
				LG("error", "this palyer already invalid\n");
				break;
			}
			if (l_player->GetMainCha()->GetPlayer() != l_player)
			{
				//LG("error", "����player��ƥ�䣨��ɫ����%s��Gate��ַ[����%p, ����%p]����cmd=%u\n", l_player->GetMainCha()->GetLogName(), l_player->GetMainCha()->GetPlayer(), l_player, cmd);
				LG("error", "two player not matching(character name:%s,Gate address [local %p, guest %p]),cmd=%u\n", l_player->GetMainCha()->GetLogName(), l_player->GetMainCha()->GetPlayer(), l_player, cmd);
			}

			
			//LG("enter_map", "��ʼ�뿪��ͼ--------\n");
			LG("enter_map", "start leave map--------\n");
			char	chOffLine = READ_CHAR(pkt); // ��ɫ����(0)
			
			//LG("enter_map", "���Player [%s]\n", l_player->GetMainCha()->GetLogName());
			LG("enter_map", "Delete Player [%s]\n", l_player->GetMainCha()->GetLogName());

			//	2008-8-20	yangyinyu	add	begin!
			//lgtool_printCharacter(	RES_STRING(GM_CHARACTER_CPP_00148),	l_player->GetMainCha()	);
			//	2008-8-20	yangyinyu	add	end!

#ifdef OUT_MUL_THREAD
			//GoOutServerCall * pCall	= g_gmsvr->m_GoOutProc.Get();
			//if(pCall)
			//{
			//	pCall->Init(l_player, !chOffLine);
			//	g_gmsvr->GetProcessor()->AddTask(pCall);
			//}
#else
			GoOutGame(l_player, !chOffLine);
#endif
			//LG("enter_map", "�����뿪��ͼ========\n\n");
			LG("enter_map", "end and leave the map========\n\n");

			// Add by lark.li 20090112 begin
			//extern ToMMS* g_ToMMS;
			//g_ToMMS->LeaveMap(l_player->GetActLoginID(), l_player->GetID());
			// End

			//LG("OutMap", "%s�뿪��ͼ\n", szLogName);

			break;
		}
	case CMD_PM_SAY2ALL:
		{
			uLong ulChaID = pkt.ReadLong();
			cChar *szContent = pkt.ReadString();
			long lChatMoney = pkt.ReadLong();

			CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(ulChaID);
			if(pPlayer)
			{
				CCharacter *pCha = pPlayer->GetMainCha();
				if(!pCha->HasMoney(lChatMoney))
				{
					//pCha->SystemNotice("���Ľ�Ǯ����,����������Ƶ������!");
					pCha->SystemNotice(RES_STRING(GM_GAMEAPPNET_CPP_00007));

					WPacket l_wpk = GETWPACKET();
					WRITE_CMD(l_wpk, CMD_MP_SAY2ALL);
					WRITE_CHAR(l_wpk, 0);
					pCha->ReflectINFof(pCha, l_wpk);

					break;
				}
				pCha->setAttr(ATTR_GD, (pCha->getAttr(ATTR_GD) - lChatMoney));
				pCha->SynAttr(enumATTRSYN_TASK);
				//pCha->SystemNotice("��������Ƶ��˵��һ�仰,������%ld�����!", lChatMoney);
				pCha->SystemNotice(RES_STRING(GM_GAMEAPPNET_CPP_00006), lChatMoney);

				WPacket l_wpk = GETWPACKET();
				WRITE_CMD(l_wpk, CMD_MP_SAY2ALL);
				WRITE_CHAR(l_wpk, 1);
				WRITE_STRING(l_wpk, pCha->GetName());
				WRITE_STRING(l_wpk, szContent);
				pCha->ReflectINFof(pCha, l_wpk);
			}
		}
		break;
	case CMD_PM_SAY2TRADE:
		{
			uLong ulChaID = pkt.ReadLong();
			cChar *szContent = pkt.ReadString();
			long lChatMoney = pkt.ReadLong();

			CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(ulChaID);
			if(pPlayer)
			{
				CCharacter *pCha = pPlayer->GetMainCha();
				if(!pCha->HasMoney(lChatMoney))
				{
					//pCha->SystemNotice("���Ľ�Ǯ����,�����ڽ���Ƶ������!");
					pCha->SystemNotice(RES_STRING(GM_GAMEAPPNET_CPP_00005));

					WPacket l_wpk = GETWPACKET();
					WRITE_CMD(l_wpk, CMD_MP_SAY2TRADE);
					WRITE_CHAR(l_wpk, 0);
					pCha->ReflectINFof(pCha, l_wpk);

					break;
				}
				pCha->setAttr(ATTR_GD, (pCha->getAttr(ATTR_GD) - lChatMoney));
				pCha->SynAttr(enumATTRSYN_TASK);
				//pCha->SystemNotice("���ڽ���Ƶ��˵��һ�仰,������%ld�����!", lChatMoney);
				pCha->SystemNotice(RES_STRING(GM_GAMEAPPNET_CPP_00004), lChatMoney);

				WPacket l_wpk = GETWPACKET();
				WRITE_CMD(l_wpk, CMD_MP_SAY2TRADE);
				WRITE_CHAR(l_wpk, 1);
				WRITE_STRING(l_wpk, pCha->GetName());
				WRITE_STRING(l_wpk, szContent);
				pCha->ReflectINFof(pCha, l_wpk);
			}
		}
		break;
	case CMD_PM_TEAM: // GroupServer֪ͨ�����Ϣ�仯
		{
			ProcessTeamMsg(pGate, pkt);
			break;
		}
	case CMD_PM_GUILDINFO:	// GroupServer֪ͨ������Ϣ�仯
		{
			ProcessGuildMsg(pGate, pkt);
			break;
		}
	case CMD_PM_GUILD_CHALLMONEY:
		{
			ProcessGuildChallMoney( pGate, pkt );
		}
		break;
	case CMD_PM_GUILD_CHALL_PRIZEMONEY:
		{
			//ProcessGuildChallPrizeMoney( pGate, pkt );

			WPACKET WtPk	=GETWPACKET();
			WRITE_CMD(WtPk, CMD_MM_GUILD_CHALL_PRIZEMONEY);
			WRITE_LONG(WtPk, 0);
			WRITE_LONG(WtPk, READ_LONG(pkt));
			WRITE_LONG(WtPk, READ_LONG(pkt));
			WRITE_SHORT(WtPk, 0);
			WRITE_LONG(WtPk, 0);
			WRITE_LONG(WtPk, 0);
			pGate->SendData(WtPk);
		}
		break;
	case CMD_PM_GUILD_INVITE:
		{
			uLong chaid = pkt.ReadLong();
			uLong l_inviter_chaid = pkt.ReadLong();
			CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(chaid);
			CPlayer *pinviter = g_pGameApp->GetPlayerByDBID(l_inviter_chaid);
			if( pPlayer && pinviter )
			{
				CCharacter* pCha = pPlayer->GetMainCha();
				CCharacter*	pinCha = pinviter->GetMainCha();
				if( pCha && pinCha )
				{
					uLong guildid = pinCha->GetGuildID();
					char l_gldtype	=game_db.GetGuildTypeByID( pinCha,guildid);
					pCha->SetGuildType( l_gldtype );
					pCha->GetPlayer()->m_GuildState.SetBit(emGuildReplaceOldTry);
					pCha->GetPlayer()->m_lTempGuildID = guildid;
					strncpy_s( pCha->GetPlayer()->m_szTempGuildName, sizeof(pCha->GetPlayer()->m_szTempGuildName),pinCha->GetPlayer()->m_szGuildName, _TRUNCATE );
					if( guildid > 0 && guildid < 199 )
					{
						Guild::cmd_GuildCancelTryFor( pCha );
						//Guild::cmd_GuildTryFor( pCha,guildid );
						Guild::cmd_GuildTryForComfirm( pCha, 1);
						Guild::cmd_GuildApprove( pinCha, chaid );
					}
				}
			}
			break;
		}
	case CMD_TM_MAPENTRY:
		{
			ProcessDynMapEntry(pGate, pkt);
			break;
		}
	case CMD_TM_MAP_ADMIN:
		{
			ProcessMapAdmin(pGate, pkt);
			break;
		}
	case CMD_TM_MAPENTRY_NOMAP:
		{
			break;
		}
	case CMD_PM_GARNER2_UPDATE:
		{
			ProcessGarner2Update(pkt);
			break;
		}
	case CMD_TM_STATE:
		{
			string str;

			uLong addr = READ_LONG_R(pkt);
			uLong dbID = READ_LONG_R(pkt);

			char buffer[256];
			CFormatParameter param(13);
			param.setLong( 0, this->m_pCPlySpace->GetMaxHoldPlyNum() );
			param.setLong( 1, this->m_pCEntSpace->GetMaxHoldChaNum() );
			param.setLong( 2, this->m_pCEntSpace->GetMaxHoldItemNum() );
			param.setLong( 3, this->m_pCEntSpace->GetMaxHoldTNpcNum() );
			
			param.setLong( 4, this->m_pCPlySpace->GetAllocPlyNum() );
			param.setLong( 5, this->m_pCEntSpace->GetAllocChaNum() );
			param.setLong( 6, this->m_pCEntSpace->GetAllocItemNum() );
			param.setLong( 7, this->m_pCEntSpace->GetAllocTNpcNum() );

			param.setLong( 8, this->m_pCPlySpace->GetHoldPlyNum() );
			param.setLong( 9, this->m_pCEntSpace->GetMaxHoldChaNum() );
			param.setLong( 10, this->m_pCEntSpace->GetMaxHoldItemNum() );
			param.setLong( 11, this->m_pCEntSpace->GetMaxHoldTNpcNum() );

			param.setString(12, g_Config.m_szName);

			CResourceBundleManage::Instance()->FormatString("{12} (Ply,Cha,Item,TNpc)[Max,Alloc,Hold] [{0} {1} {2} {3}] [{4} {5} {6} {7}] [{8} {9} {10} {11}]", param, buffer);
			str.append(buffer);

			WPACKET wpk	=GETWPACKET();
			WRITE_CMD(wpk, CMD_MT_STATE);
			WRITE_STRING(wpk, str.c_str());

			WRITE_LONG(wpk, dbID);
			WRITE_LONG(wpk, addr);

			pGate->SendData(wpk);
			
			break;
		};
    case CMD_PM_EXPSCALE:
        {
            //  ������
            uLong ulChaID = pkt.ReadLong();
            uLong ulTime = pkt.ReadLong();

            CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(ulChaID);

            if(pPlayer)
            {
                CCharacter *pCha = pPlayer->GetMainCha();

				if(pCha->IsScaleFlag())
				{
					break;
				}

				pCha->SetNoticeState(ulTime);

            }
        }  break;

	default:
		if (cmd/500 ==CMD_MM_BASE/500)
		{
			ProcessInterGameMsg(cmd, pGate, pkt);
		}else
		{
			l_player = (CPlayer *)MakePointer(READ_LONG_R(pkt));
			if (cmd/500 ==CMD_PM_BASE/500 && !l_player)
			{
				ProcessGroupBroadcast(cmd, pGate, pkt);
			}else
			{
				if (!l_player)
					break;
				try
				{
					DWORD	l_gateaddr = READ_LONG_R(pkt);
					if (l_player->GetGateAddr() != l_gateaddr)
					{
						/*LG("error", "���ݿ�ID:%u, ��ַ��ƥ�䣬����:%u, gate:%u,cmd=%d, ��Ч��(%d)\n", l_player->GetDBChaId(), l_player->GetGateAddr(),
							l_gateaddr,cmd, l_player->IsValidFlag() );*/
						LG("error", "DB ID:%u, address not matching,local :%u, gate:%u,cmd=%d, validity (%d)\n", l_player->GetDBChaId(), l_player->GetGateAddr(),
							l_gateaddr,cmd, l_player->IsValidFlag() );
						break;
					}
				}
				catch (...)
				{
					//LG("error", "===========================��Gate������ҵ�ַ����%p,cmd =%d\n", l_player, cmd);
					LG("error", "===========================Player address error that come from Gate %p,cmd =%d\n", l_player, cmd);
					break;
				}
				if (!l_player->IsValid())
					break;
				if (l_player->GetMainCha()->GetPlayer() != l_player)
				{
					//LG("error", "����player��ƥ�䣨��ɫ����%s��Gate��ַ[����%p, ����%p]����cmd=%u\n", l_player->GetMainCha()->GetLogName(), l_player->GetMainCha()->GetPlayer(), l_player, cmd);
					LG("error", "two player not matching(character name:%s,Gate address [local %p, guest %p]),cmd=%u\n", l_player->GetMainCha()->GetLogName(), l_player->GetMainCha()->GetPlayer(), l_player, cmd);
				}

				CCharacter	*pCCha = l_player->GetCtrlCha();
				if (!pCCha)
					break;
				if (g_pGameApp->IsValidEntity(pCCha->GetID(), pCCha->GetHandle()))
				{
					g_pNoticeChar = pCCha;
					
					g_ulCurID = pCCha->GetID();
					g_lCurHandle = pCCha->GetHandle();

					pCCha->ProcessPacket(cmd, pkt);
					
					g_ulCurID = defINVALID_CHA_ID;
					g_lCurHandle = defINVALID_CHA_HANDLE;

					g_pNoticeChar = NULL;
				}
				else
				{
					//LG("error", "�յ�CMD_CM_BASE����Ϣ[%d]ʱ, ���ֽ�ɫpCChaΪ��\n", cmd);
					LG("error", "when receive CMD_CM_BASE message[%d], find character pCCha is null\n", cmd);
				}
				break;
			}
		}
	}

	//DWORD dwTick = GetTickCount() - dwLastTick;
	//if(dwTick > 10)
	//	LG("TickCount", ",%d,%ld\n", cmd, dwTick);

	//GameServerLG(g_strLogName.c_str(), "CGameApp::ProcessPacket (cmd = %d) End!\n", cmd);
T_E}

// ������Ͷ����Ǯ
void CGameApp::ProcessGuildChallMoney( GateServer *pGate, RPACKET pkt )
{T_B
	DWORD dwChaDBID = READ_LONG(pkt);
	DWORD dwMoney  = READ_LONG(pkt);
	const char* pszGuild1 = READ_STRING(pkt);

	if(pszGuild1 == NULL)
		return;

	const char* pszGuild2 = READ_STRING(pkt);

	if(pszGuild2 == NULL)
		return;

	//	2007-8-4	yangyinyu	change	begin!	//	����ʱ���յ������Ϣ����������Ǯ������
	CPlayer* pPlayer = GetPlayerByDBID(dwChaDBID);
	if( pPlayer )
	{
		CCharacter* pCha = pPlayer->GetMainCha();
		//pCha->AddMoney( "ϵͳ", dwMoney );
		if(pCha)
		{
			pCha->AddMoney( RES_STRING(GM_GAMEAPPNET_CPP_00017), dwMoney );
			/*pCha->SystemNotice( "��Ͷ����빫�ᡶ%s��ս�����ᡶ%s��ȡ���ˣ�����Ͷ���(%u)�Ѿ��˻�������\n", pszGuild1, pszGuild2, dwMoney );
			LG( "��ս������", "��%s��Ͷ����빫�ᡶ%s��ս�����ᡶ%s��ȡ���ˣ�����Ͷ���(%u)�Ѿ��˻�������\n", pCha->GetGuildName(), pszGuild1, pszGuild2, dwMoney );*/
			//pCha->SystemNotice( RES_STRING(GM_GAMEAPPNET_CPP_00009), pszGuild1, pszGuild2, dwMoney );
			char szData[128];
			CFormatParameter param(3);
			param.setString( 0, pszGuild1 );
			param.setString( 1, pszGuild2 );
			param.setDouble( 2, dwMoney );
			RES_FORMAT_STRING( GM_GAMEAPPNET_CPP_00009, param, szData );
			pCha->SystemNotice( szData );
			LG( "challenge consortia result", "��%s��bidder and consortia��%s��battle was consortia��%s��replace,your consortia gold (%u)had back to you!\n", pCha->GetGuildName(), pszGuild1, pszGuild2, dwMoney );
		}
	}
	else
	{
		//LG( "��ս������", "δ���ֹ���᳤��Ϣָ�룬�޷���ǮDBID[%u],Ǯ��[%u].\n", dwChaDBID, dwMoney );
		LG( "challenge consortia result", "not find deacon information finger,cannot back gold DBID[%u],how much money[%u].\n", dwChaDBID, dwMoney );
	}
T_E}

void CGameApp::ProcessGuildChallPrizeMoney( GateServer *pGate, RPACKET pkt )
{T_B
	DWORD dwChaDBID = READ_LONG(pkt);
	DWORD dwMoney  = READ_LONG(pkt);
	CPlayer* pPlayer = GetPlayerByDBID(dwChaDBID);
	if( pPlayer )
	{
		CCharacter* pCha = pPlayer->GetMainCha();
		// pCha->AddMoney( "ϵͳ", dwMoney );
		pCha->AddMoney( RES_STRING(GM_CHARACTER_CPP_00012), dwMoney );
		/*pCha->SystemNotice( "��ϲ���쵼�Ĺ��ᡶ%s��ȡ���˹���ս��ʤ������ý�����%u����", pCha->GetGuildName(), dwMoney );
		LG( "��ս������", "��ϲ���쵼�Ĺ��ᡶ%s��ȡ���˹���ս��ʤ������ý�����%u����", pCha->GetGuildName(), dwMoney );*/
		//pCha->SystemNotice( RES_STRING(GM_GAMEAPPNET_CPP_00010), pCha->GetGuildName(), dwMoney );
		char szData[128];
		CFormatParameter param(2);
		param.setString( 0, pCha->GetGuildName() );
		param.setDouble( 1, dwMoney );
		RES_FORMAT_STRING( GM_GAMEAPPNET_CPP_00010, param, szData );
		pCha->SystemNotice( szData );
		LG( "challenge consortia result", "congratulate you have leading the consortia��%s��get win in consortia battle!gain bounty(%u)!", pCha->GetGuildName(), dwMoney );
	}
	else
	{
		//LG( "��ս������", "δ���ֹ���᳤��Ϣָ�룬�޷�����DBID[%u],Ǯ��[%u]", dwChaDBID, dwMoney );
		LG( "challenge consortia result", "cannot find deacon information finger,cannot hortation DBID[%u],how much money[%u]", dwChaDBID, dwMoney );
	}
T_E}

// ��������Ϣ
void CGameApp::ProcessGuildMsg(GateServer *pGate, RPACKET pkt)
{T_B
	DWORD dwChaDBID  = READ_LONG(pkt);
	CPlayer* pPlayer = GetPlayerByDBID(dwChaDBID);
	if( pPlayer )
	{
		CCharacter* pCha = pPlayer->GetCtrlCha();
		BYTE byType = READ_CHAR(pkt);
		DWORD dwGuildID = READ_LONG(pkt);
		DWORD dwLeaderID = READ_LONG(pkt);
		const char* pszGuildName = READ_STRING(pkt);
		const char* pszGuildMotto = READ_STRING(pkt);
		pCha->SetGuildType( byType );

		if(pszGuildName == NULL)
			pCha->SetGuildName( "" );
		else
			pCha->SetGuildName( pszGuildName );

		if(pszGuildMotto == NULL)
			pCha->SetGuildMotto( pszGuildMotto );
		else
			pCha->SetGuildMotto( "" );

		pCha->SyncGuildInfo();
	}
T_E}

// ��������й���Ϣ
void CGameApp::ProcessTeamMsg(GateServer *pGate, RPACKET pkt)
{T_B
	//LG("team", "��ʼ���������Ϣ\n");

	char cTeamMsgType = READ_CHAR(pkt);
	
	switch(cTeamMsgType)
	{
		case TEAM_MSG_ADD:     {	/*LG("team", "�յ���� [�¼Ӷ�Ա] ��Ϣ\n");*/ break; }
		case TEAM_MSG_LEAVE:   {	/*LG("team", "�յ���� [��Ա���] ��Ϣ\n");*/ break; }
		case TEAM_MSG_UPDATE:  {	/*LG("team", "�յ���� [��Աˢ��] ��Ϣ\n");*/ break; }
		default:
			//LG("team", "�յ���Ч��Team��Ϣ [%d]\n", cTeamMsgType);
			return;
	}
	
	char cMemberCnt = READ_CHAR(pkt);

	if(cMemberCnt > MAX_TEAM_MEMBER)
	{
		LG("team", "team count [%d] error\n", cMemberCnt);
		return;
	}
	//LG("team", "��ǰ��Ա������[%d]\n", cMemberCnt); // ����յ����� < 2���ʾGroupServer�Ķ�����Ϣ��������.

	uplayer Team[MAX_TEAM_MEMBER];
	CPlayer *PlayerList[MAX_TEAM_MEMBER];
	bool	CanSeenO[MAX_TEAM_MEMBER][2];
	bool	CanSeenN[MAX_TEAM_MEMBER][2];
			
	// ��ȡ����Ϣ�����ҳ����е�Player
	for(char i = 0; i < cMemberCnt; i++)
	{
		const char *pszGateName = READ_STRING(pkt);
		
		if(pszGateName == NULL)
			return;

		DWORD dwGateAddr = READ_LONG(pkt);
		DWORD dwChaDBID  = READ_LONG(pkt);
		Team[i].Init(pszGateName, dwGateAddr, dwChaDBID);
		if (!Team[i].pGate)
		{
			LG("team", "GameServer can't find matched Gate:%s, addr = 0x%X, chaid = %d.\n", pszGateName, dwGateAddr, dwChaDBID);
			//LG("team", "\tGameServer all Gate:\n");
			BEGINGETGATE();
			GateServer	*pGateServer;
			while (pGateServer = GETNEXTGATE())
			{
				LG("team", "\t%s\n", pGateServer->GetName().c_str());
			}
		}
			
		PlayerList[i] = GetPlayerByDBID(dwChaDBID);
		
		//LG("team", "��Ա: %s, %d %d ����Gate [%s]\n", PlayerList[i]!=NULL ? PlayerList[i]->GetCtrlCha()->GetLogName():"(���˲��ڱ�Server!)", dwChaDBID, dwGateAddr, pszGateName);
	}

	//RefreshTeamEyeshot(PlayerList, cMemberCnt, cTeamMsgType);
	CheckSeeWithTeamChange(CanSeenO, PlayerList, cMemberCnt);
	//if(PlayerList[0]==NULL)
	//{
	//	LG("team", "�ӳ����ڱ�game server����\n");
	//}

	int nLeftMember = cMemberCnt;
	if(cTeamMsgType == TEAM_MSG_LEAVE) // ��Ա��ӻ�����
	{
		nLeftMember-=1;
		CPlayer *pLeave = PlayerList[cMemberCnt - 1];
		if(pLeave)
		{
			pLeave->LeaveTeam();
		}
	}
	// �˴�֮����Ӷ���, ���cMember��ʱΪ1, ��ȼ��ڽ���˶���
	for(int i = 0; i < nLeftMember; i++)
	{
		if(PlayerList[i]==NULL) continue;
			
		PlayerList[i]->ClearTeamMember();
		for(int j = 0; j < nLeftMember; j++)
		{
			if(i==j) continue;
			PlayerList[i]->AddTeamMember(&Team[j]);
		}
		if (nLeftMember != 1)
		{
			PlayerList[i]->setTeamLeaderID(Team[0].m_dwDBChaId);
			PlayerList[i]->NoticeTeamLeaderID();
		}
	}

	CheckSeeWithTeamChange(CanSeenN, PlayerList, cMemberCnt);
	RefreshTeamEyeshot(CanSeenO, CanSeenN, PlayerList, cMemberCnt, cTeamMsgType);

	//add by jilinlee 2007/07/11

	for(char i = 0; i < cMemberCnt; i++)
	{
        if(i < 5)
        {
            if(PlayerList[i])
            {
		        CCharacter* pCtrlCha = PlayerList[i] ->GetCtrlCha();
		        if(pCtrlCha)
		        {
			        SubMap* pSubMap = pCtrlCha ->GetSubMap();
				        if(pSubMap && pSubMap ->GetMapRes())
				        {
					        if(!(pSubMap ->GetMapRes() ->CanTeam()))
					        {
								// pCtrlCha ->SystemNotice("�˵�ͼ������ӣ������߳��˵�ͼ��");
								pCtrlCha ->SystemNotice(RES_STRING(GM_GAMEAPPNET_CPP_00011));

								LG("team", "MoveCity()\n");
								//Modify by lark.li 20090114 begin
								//pCtrlCha ->MoveCity("garner");
								// ��ص������� 
								pCtrlCha ->MoveCity("");
								// End
					        }
				        }
		        }
            }
        }
	}

	//if(nLeftMember==1) LG("team", "nLeftMember==1, �����ɢ!\n");
	
	//LG("team", "�������������Ϣ\n\n");
T_E}

// ȷ����Ҽ��Ƿ�ɼ�
void CGameApp::CheckSeeWithTeamChange(bool CanSeen[][2], CPlayer **pCPlayerList, char chMemberCnt)
{T_B
	if (chMemberCnt <= 1)
		return;

	CPlayer	*pCProcPly = pCPlayerList[chMemberCnt - 1];
	if (!pCProcPly)
		return;

	CPlayer	*pCCurPly;
	CCharacter	*pCProcCha = pCProcPly->GetCtrlCha(), *pCCurCha;
	for (char i = 0; i < chMemberCnt - 1; i++)
	{
		pCCurPly = pCPlayerList[i];
		if (!pCCurPly)
			continue;
		pCCurCha = pCCurPly->GetCtrlCha();
		if (pCProcCha->IsInEyeshot(pCCurCha))
		{
			pCProcCha->CanSeen(pCCurCha) ? CanSeen[i][0] = true : CanSeen[i][0] = false;
			pCCurCha->CanSeen(pCProcCha) ? CanSeen[i][1] = true : CanSeen[i][1] = false;
		}
	}
T_E}

// ���ݵ�ǰ�Ƿ�ɼ���֮ǰ�Ľ��бȽ���ˢ����Ұ
void CGameApp::RefreshTeamEyeshot(bool CanSeenOld[][2], bool CanSeenNew[][2], CPlayer **pCPlayerList, char chMemberCnt, char chRefType)
{T_B
	if (chMemberCnt <= 1)
		return;

	CPlayer	*pCProcPly = pCPlayerList[chMemberCnt - 1];
	if (!pCProcPly)
		return;

	CPlayer	*pCCurPly;
	CCharacter	*pCProcCha = pCProcPly->GetCtrlCha(), *pCCurCha;
	for (char i = 0; i < chMemberCnt - 1; i++)
	{
		pCCurPly = pCPlayerList[i];
		if (!pCCurPly)
			continue;
		pCCurCha = pCCurPly->GetCtrlCha();
		if (pCProcCha->IsInEyeshot(pCCurCha))
		{
			if (chRefType == TEAM_MSG_ADD)
			{
				if (!CanSeenOld[i][0] && CanSeenNew[i][0])
					pCCurCha->BeginSee(pCProcCha);
				if (!CanSeenOld[i][1] && CanSeenNew[i][1])
					pCProcCha->BeginSee(pCCurCha);
			}
			else if (chRefType == TEAM_MSG_LEAVE)
			{
				if (CanSeenOld[i][0] && !CanSeenNew[i][0])
					pCCurCha->EndSee(pCProcCha);
				if (CanSeenOld[i][1] && !CanSeenNew[i][1])
					pCProcCha->EndSee(pCCurCha);
			}
		}
	}
T_E}

// ���ݵ�ǰ�Ƿ�ɼ���ˢ����Ұ
void CGameApp::RefreshTeamEyeshot(CPlayer **pCPlayerList, char chMemberCnt, char chRefType)
{T_B
	if (chMemberCnt <= 1)
		return;

	CPlayer	*pCProcPly = pCPlayerList[chMemberCnt - 1];
	if (!pCProcPly)
		return;

	CPlayer	*pCCurPly;
	CCharacter	*pCProcCha = pCProcPly->GetCtrlCha(), *pCCurCha;
	bool	bCurChaHide;
	bool	bProcChaHide = pCProcCha->IsHide();
	for (char i = 0; i < chMemberCnt - 1; i++)
	{
		pCCurPly = pCPlayerList[i];
		if (!pCCurPly)
			continue;
		pCCurCha = pCCurPly->GetCtrlCha();
		bCurChaHide = pCCurCha->IsHide();
		if (bProcChaHide || bCurChaHide) // ��������
		{
			if (pCProcCha->IsInEyeshot(pCCurCha))
			{
				if (chRefType == TEAM_MSG_ADD)
				{
					if (bProcChaHide)
						pCCurCha->BeginSee(pCProcCha);
					if (bCurChaHide)
						pCProcCha->BeginSee(pCCurCha);
				}
				else if (chRefType == TEAM_MSG_LEAVE)
				{
					if (bProcChaHide)
						pCCurCha->EndSee(pCProcCha);
					if (bCurChaHide)
						pCProcCha->EndSee(pCCurCha);
				}
			}
		}
	}
T_E}

BOOL CGameApp::AddVolunteer(CCharacter *pCha)
{T_B
	if(pCha->IsVolunteer())
		return false;
	pCha->SetVolunteer(true);

	SVolunteer volNode;
	volNode.lJob = (long)pCha->getAttr(ATTR_JOB);
	volNode.lLevel = pCha->GetLevel();
	volNode.ulID = pCha->GetID();
	//strcpy(volNode.szMapName, pCha->GetSubMap()->GetName());
	strncpy_s( volNode.szMapName, sizeof(volNode.szMapName), pCha->GetSubMap()->GetName(), _TRUNCATE );
	//strcpy(volNode.szName, pCha->GetName());
	strncpy_s( volNode.szName, sizeof(volNode.szName), pCha->GetName(), _TRUNCATE );

	m_vecVolunteerList.push_back(volNode);
	
	return true;
T_E}

BOOL CGameApp::DelVolunteer(CCharacter *pCha)
{T_B
	if(!pCha->IsVolunteer())
		return false;
	pCha->SetVolunteer(false);

	vector<SVolunteer>::iterator it;
	for(it = m_vecVolunteerList.begin(); it != m_vecVolunteerList.end(); it++)
	{
		if(!strcmp((*it).szName, pCha->GetName()))
		{
			m_vecVolunteerList.erase(it);
			return true;
		}
	}

	return false;
T_E}

int CGameApp::GetVolNum()
{T_B
	return (int)m_vecVolunteerList.size();
T_E}

SVolunteer* CGameApp::GetVolInfo(int nIndex)
{T_B
	if(nIndex < 0 || nIndex >= (int)m_vecVolunteerList.size())
		return NULL;
	
	return &m_vecVolunteerList[nIndex];
T_E}

SVolunteer* CGameApp::FindVolunteer(const char *szName)
{T_B
	vector<SVolunteer>::iterator it;
	for(it = m_vecVolunteerList.begin(); it != m_vecVolunteerList.end(); it++)
	{
		if(!strcmp((*it).szName, szName))
		{
			return (SVolunteer *)&(*it);
		}
	}
	return NULL;
T_E}

void CGameApp::ProcessInterGameMsg(unsigned short usCmd, GateServer *pGate, RPACKET pkt)
{T_B
	long	lSrcID = READ_LONG(pkt);
long	gTarget = 0;
long	gSender = 0;
long	gPermssion = 0;

if (usCmd == CMD_MM_UPDATEGUILDPERMISSONS)
{
	gTarget = READ_LONG(pkt);
	gSender = READ_LONG(pkt);
	gPermssion = READ_SHORT(pkt);
}

	short	sNum = READ_SHORT_R(pkt);
	long	lGatePlayerAddr = READ_LONG_R(pkt);
	long	lGatePlayerID = READ_LONG_R(pkt);

	switch (usCmd)
	{
		// FEATURE: GUILD_BANK
	case CMD_MM_UPDATEGUILDBANK: {
		int guildID = READ_LONG(pkt);

		BEGINGETGATE();
		CPlayer* pCPlayer;
		CCharacter* pCha = 0;
		GateServer* pGateServer;

		CKitbag bag;
		if (!game_db.GetGuildBank(guildID, &bag)) {
			return;
		}

		while (pGateServer = GETNEXTGATE()) {
			if (!BEGINGETPLAYER(pGateServer)) {
				continue;
			}

			int nCount = 0;
			while (pCPlayer = (CPlayer*)GETNEXTPLAYER(pGateServer)) {
				pCha = pCPlayer->GetMainCha();
				if (!pCha) {
					continue;
				}

				if (pCha->GetGuildID() == guildID) {
					pCPlayer->SynGuildBank(&bag, 0);
				}
			}
		}

		break;
	}
	case CMD_MM_UPDATEGUILDBANKGOLD: {
		int guildID = READ_LONG(pkt);

		BEGINGETGATE();

		CPlayer* pCPlayer;
		CCharacter* pCha = 0;
		GateServer* pGateServer;

		unsigned long long gold = game_db.GetGuildBankGold(guildID);

		WPACKET WtPk = GETWPACKET();
		WRITE_CMD(WtPk, CMD_MC_UPDATEGUILDBANKGOLD);
		WRITE_STRING(WtPk, to_string(gold).c_str());

		while (pGateServer = GETNEXTGATE()) {
			if (!BEGINGETPLAYER(pGateServer)) {
				continue;
			}

			int nCount = 0;
			while (pCPlayer = (CPlayer*)GETNEXTPLAYER(pGateServer)) {
				pCha = pCPlayer->GetMainCha();
				if (!pCha) {
					continue;
				}

				pCPlayer->GetGuildGold();


				//int canSeeBank = (pCha->guildPermission & emGldPermViewBank);
				//if (pCha->GetGuildID() == guildID && canSeeBank == emGldPermViewBank){
				//	pCha->ReflectINFof(pCha, WtPk);
				//}

			}
		}
		break;
	}
	case 	CMD_MM_UPDATEGUILDPERMISSONS: {
		int guildID = lSrcID;
		long  permission = game_db.GetGuildPermission(gTarget, guildID);

		BEGINGETGATE();

		CPlayer* pCPlayer;
		CCharacter* pCha = 0;
		GateServer* pGateServer;


		WPACKET WtPk = GETWPACKET();
		WRITE_CMD(WtPk, CMD_MC_UPDATEGUILDBANKGOLD);

		while (pGateServer = GETNEXTGATE()) {
			if (!BEGINGETPLAYER(pGateServer)) {
				continue;
			}

			int nCount = 0;
			while (pCPlayer = (CPlayer*)GETNEXTPLAYER(pGateServer)) {
				pCha = pCPlayer->GetMainCha();
				if (!pCha) {
					continue;
				}
				if (pCha->GetGuildID() == guildID && pCha->m_ID == gTarget)
				{
					pCPlayer->SystemNotice("[GUILD] Your permission has been changed!");
					pCha->guildPermission = permission;
					break;
				}
			}
		}
		break;
	}

	case CMD_MM_GUILD_MOTTO:
		{
			uLong	l_gldid		=lSrcID;
			cChar* pszMotto = READ_STRING( pkt );
			{//������FindPlayerChaByID

				if(pszMotto == NULL)
					pszMotto = "";

				BEGINGETGATE();
				CPlayer	*pCPlayer;
				CCharacter	*pCha = 0;
				GateServer	*pGateServer;
				while (pGateServer = GETNEXTGATE())
				{
					if (!BEGINGETPLAYER(pGateServer))
						continue;
					int nCount = 0;
					while (pCPlayer = (CPlayer *)GETNEXTPLAYER(pGateServer))
					{
						if (++nCount > GETPLAYERCOUNT(pGateServer))
						{
							//LG("����������", "�����Ŀ:%u, %s\n", GETPLAYERCOUNT(pGateServer), "ProcessInterGameMsg::CMD_MM_GUILD_DISBAND");
							LG("player list error", "player number:%u, %s\n", GETPLAYERCOUNT(pGateServer), "ProcessInterGameMsg::CMD_MM_GUILD_DISBAND");
							break;
						}
						pCha = pCPlayer->GetMainCha();
						if (!pCha)
							continue;
						if (pCha->GetGuildID() == l_gldid) // �ҵ���ɫ
						{							
							pCha->SetGuildMotto(pszMotto);
							pCha->SyncGuildInfo();
							//pCha->SystemNotice("�������������޸ģ�");
							pCha->SystemNotice(RES_STRING(GM_GAMEAPPNET_CPP_00012));
						}
					}
				}
			}
		}
		break;
	case CMD_MM_GUILD_DISBAND:
		{
			uLong	l_gldid		=lSrcID;
			{//������FindPlayerChaByID
				BEGINGETGATE();
				CPlayer	*pCPlayer;
				CCharacter	*pCha = 0;
				GateServer	*pGateServer;
				while (pGateServer = GETNEXTGATE())
				{
					if (!BEGINGETPLAYER(pGateServer))
						continue;
					int nCount = 0;
					while (pCPlayer = (CPlayer *)GETNEXTPLAYER(pGateServer))
					{
						if (++nCount > GETPLAYERCOUNT(pGateServer))
						{
							//LG("����������", "�����Ŀ:%u, %s\n", GETPLAYERCOUNT(pGateServer), "ProcessInterGameMsg::CMD_MM_GUILD_DISBAND");
							LG("player list error", "player number:%u, %s\n", GETPLAYERCOUNT(pGateServer), "ProcessInterGameMsg::CMD_MM_GUILD_DISBAND");
							break;
						}
						pCha = pCPlayer->GetMainCha();
						if (!pCha)
							continue;
						if (pCha->GetGuildID() == l_gldid) // �ҵ���ɫ
						{
							pCha->m_CChaAttr.ResetChangeFlag();

							pCha->SetGuildID( 0 );
							pCha->SetGuildState( 0 );
							pCha->SynAttr(enumATTRSYN_TRADE);
							
							pCha->SetGuildName("");
							pCha->SetGuildMotto("");
							pCha->SyncGuildInfo();
							//pCha->SystemNotice("�����Ѿ����᳤��ɢ");
							pCha->SystemNotice(RES_STRING(GM_GAMEAPPNET_CPP_00013));
						}
					}
				}
			}
		}
		break;
	case CMD_MM_GUILD_KICK:
		{
			uLong	l_chaid		=lSrcID;
			CCharacter	*pCha	=FindMainPlayerChaByID(l_chaid);
			if(pCha)
			{
				pCha->SetGuildName("");
				cChar * l_gldname =READ_STRING(pkt);
				if(l_gldname == NULL)
					l_gldname = "";

				pCha->SetGuildID( 0 );			//���ù���ID
				pCha->SetGuildType( 0 );		//���ù���Type
				pCha->SetGuildState( 0 );				
				pCha->SetGuildName( "" );
				pCha->SetGuildMotto( "" );
               // pCha->SystemNotice("���Ѿ�������������[%s].",l_gldname);
				 pCha->SystemNotice(RES_STRING(GM_GAMEAPPNET_CPP_00014),l_gldname);
				pCha->SyncGuildInfo();
			}
		}
		break;
	case CMD_MM_GUILD_APPROVE:
		{
			uLong	l_chaid		=lSrcID;
			CCharacter	*pCha	=FindMainPlayerChaByID(l_chaid);
			if(pCha)
			{
				pCha->SetGuildID( READ_LONG(pkt) );				//���ù���ID
				pCha->SetGuildType( READ_CHAR(pkt) );
				pCha->SetGuildState( 0 );	//���ù���Type
				cChar * l_gldname =READ_STRING(pkt);

				if(l_gldname == NULL)
					l_gldname = "";

				pCha->SetGuildName(l_gldname);
				cChar * l_gldmotto = READ_STRING(pkt);

				if(l_gldmotto == NULL)
					l_gldmotto = "";

				pCha->SetGuildMotto( l_gldmotto );

              //  pCha->SystemNotice("���Ѿ�����׼���빫��[%s].",l_gldname);
				  pCha->SystemNotice(RES_STRING(GM_GAMEAPPNET_CPP_00015),l_gldname);
				pCha->SyncGuildInfo();
			}
		}
		break;
	case CMD_MM_GUILD_REJECT:
		{
			uLong l_chaid		=lSrcID;
			CCharacter	*pCha	=FindMainPlayerChaByID(l_chaid);
			if(pCha)
			{
				pCha->SetGuildID( 0 );
				pCha->SetGuildState( 0 );
				pCha->SetGuildName("");				

                //pCha->SystemNotice("��Թ���[%s]�ļ������뱻�ܾ�.",READ_STRING(pkt));
				const char	*cszMsg = READ_STRING(pkt);
				if(cszMsg != NULL)
					pCha->SystemNotice(RES_STRING(GM_GAMEAPPNET_CPP_00016),cszMsg);
			}
		}
		break;
	case	CMD_MM_QUERY_CHAPING:
		{
			const char	*cszChaName = READ_STRING(pkt);
			
			if(cszChaName == NULL)
				break;

			CCharacter	*pCCha = FindPlayerChaByName(cszChaName);
			if (!pCCha)
				break;

			WPACKET WtPk	=GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_PING);
			WRITE_LONG(WtPk, GetTickCount());
			WRITE_LONG(WtPk, MakeULong(pGate));
			WRITE_LONG(WtPk, lSrcID);
			WRITE_LONG(WtPk, lGatePlayerID);
			WRITE_LONG(WtPk, lGatePlayerAddr);
			WRITE_SHORT(WtPk, 1);
			pCCha->ReflectINFof(pCCha,WtPk);

			break;
		}
	case	CMD_MM_QUERY_CHA:
		{
			const char	*cszChaName = READ_STRING(pkt);

			if(cszChaName == NULL)
				break;

			CCharacter	*pCCha = FindPlayerChaByName(cszChaName);
			if (!pCCha || !pCCha->GetSubMap())
				break;

			WPACKET WtPk	=GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_QUERY_CHA);
			WRITE_LONG(WtPk, lSrcID);
			WRITE_STRING(WtPk, pCCha->GetName());
			WRITE_STRING(WtPk, pCCha->GetSubMap()->GetName());
			WRITE_LONG(WtPk, pCCha->GetPos().x);
			WRITE_LONG(WtPk, pCCha->GetPos().y);
			WRITE_LONG(WtPk, pCCha->GetID());
			WRITE_LONG(WtPk, lGatePlayerID);
			WRITE_LONG(WtPk, lGatePlayerAddr);
			WRITE_SHORT(WtPk, 1);
			pGate->SendData(WtPk);

			break;
		}
	case	CMD_MM_QUERY_CHAITEM:
		{
			const char	*cszChaName = READ_STRING(pkt);

			if(cszChaName == NULL)
				break;

			CCharacter	*pCCha = FindPlayerChaByName(cszChaName);
			if (!pCCha)
				break;
			pCCha->GetKitbag()->SetChangeFlag();

			WPACKET WtPk	=GETWPACKET();
			WRITE_CMD(WtPk, CMD_MC_QUERY_CHA);
			WRITE_LONG(WtPk, lSrcID);
			pCCha->WriteKitbag(pCCha->GetKitbag(), WtPk, enumSYN_KITBAG_INIT);
			WRITE_LONG(WtPk, lGatePlayerID);
			WRITE_LONG(WtPk, lGatePlayerAddr);
			WRITE_SHORT(WtPk, 1);
			pGate->SendData(WtPk);

			break;
		}
	case	CMD_MM_CALL_CHA:
		{
			const char	*cszChaName = READ_STRING(pkt);
			
			if(cszChaName == NULL)
				break;

			CCharacter	*pCCha = FindPlayerChaByName(cszChaName);
			if (!pCCha || !pCCha->GetSubMap())
				break;
			bool	bTarIsBoat = READ_CHAR(pkt) ? true : false;
			if (bTarIsBoat != pCCha->IsBoat()) // ��ͬ��������
				break;
			const char	*cszMapName = READ_STRING(pkt);

			if(cszMapName == NULL)
				break;

			long	lPosX = READ_LONG(pkt);
			long	lPosY = READ_LONG(pkt);
			long	lCopyNO = READ_LONG(pkt);
			pCCha->SwitchMap(pCCha->GetSubMap(), cszMapName, lPosX, lPosY, true, enumSWITCHMAP_CARRY, lCopyNO);

			break;
		}
	case	CMD_MM_GOTO_CHA:
		{
			const char	*cszChaName = READ_STRING(pkt);

			if(cszChaName == NULL)
				break;

			CCharacter	*pCCha = FindPlayerChaByName(cszChaName);
			if (!pCCha || !pCCha->GetSubMap())
				break;
			switch (READ_CHAR(pkt))
			{
			case	1: // �������Ŀ���ɫ
				{
					const char	*cszSrcName = READ_STRING(pkt);

					if(cszSrcName == NULL)
						break;

					WPACKET WtPk	=GETWPACKET();
					WRITE_CMD(WtPk, CMD_MM_GOTO_CHA);
					WRITE_LONG(WtPk, lSrcID);
					WRITE_STRING(WtPk, cszSrcName);
					WRITE_CHAR(WtPk, 2);
					if (pCCha->IsBoat())
						WRITE_CHAR(WtPk, 1);
					else
						WRITE_CHAR(WtPk, 0);
					WRITE_STRING(WtPk, pCCha->GetSubMap()->GetName());
					WRITE_LONG(WtPk, pCCha->GetPos().x);
					WRITE_LONG(WtPk, pCCha->GetPos().y);
					WRITE_LONG(WtPk, pCCha->GetSubMap()->GetCopyNO());
					WRITE_LONG(WtPk, lGatePlayerID);
					WRITE_LONG(WtPk, lGatePlayerAddr);
					WRITE_SHORT(WtPk, 1);
					pGate->SendData(WtPk);

					break;
				}
			case	2: // �ҵ���Ŀ���ɫ��ԭ��ɫ������ת
				{
					bool	bTarIsBoat = READ_CHAR(pkt) ? true : false;
					if (bTarIsBoat != pCCha->IsBoat()) // ��ͬ��������
						break;
					const char	*cszMapName = READ_STRING(pkt);

					if(cszMapName == NULL)
						break;

					long	lPosX = READ_LONG(pkt);
					long	lPosY = READ_LONG(pkt);
					long	lCopyNO = READ_LONG(pkt);
					pCCha->SwitchMap(pCCha->GetSubMap(), cszMapName, lPosX, lPosY, true, enumSWITCHMAP_CARRY, lCopyNO);

					break;
				}
			}

			break;
		}
	case	CMD_MM_KICK_CHA:
		{
			const char	*cszChaName = READ_STRING(pkt);

			if(cszChaName == NULL)
				break;

			long	lTime = READ_LONG(pkt);
			CCharacter	*pCCha = FindPlayerChaByName(cszChaName);
			if (!pCCha || !pCCha->GetSubMap())
				break;

			KICKPLAYER(pCCha->GetPlayer(), lTime);
			g_pGameApp->GoOutGame(pCCha->GetPlayer(), true);

			break;
		}
	case	CMD_MM_NOTICE:
		{
			LocalNotice(READ_STRING(pkt));

			break;
		}
	case	CMD_MM_CHA_NOTICE:
		{
			const char	*cszNotiCont = READ_STRING(pkt);

			if(cszNotiCont == NULL)
				break;

			const char	*cszChaName = READ_STRING(pkt);

			if(cszChaName == NULL)
				break;

			if (!strcmp(cszChaName, ""))
				LocalNotice(cszNotiCont);
			else
			{
				CCharacter	*pCCha = FindPlayerChaByName(cszChaName);
				if (!pCCha)
					break;

				WPACKET wpk	= GETWPACKET();
				WRITE_CMD(wpk, CMD_MC_SYSINFO);
				WRITE_STRING(wpk, cszNotiCont);
				pCCha->ReflectINFof(pCCha, wpk);
			}

			break;
		}
	case	CMD_MM_DO_STRING:
		{
			lua_dostring(g_pLuaState, READ_STRING(pkt));
			break;
		}
	case	CMD_MM_LOGIN:
		{
			g_pGameApp->AfterPlayerLogin(READ_STRING(pkt));

			break;
		}
	case	CMD_MM_GUILD_CHALL_PRIZEMONEY:
		{
			DWORD dwChaDBID = READ_LONG(pkt);
			DWORD dwMoney  = READ_LONG(pkt);
			CPlayer* pPlayer = GetPlayerByDBID(dwChaDBID);
			if( pPlayer )
			{
				CCharacter* pCha = pPlayer->GetMainCha();
				/*pCha->AddMoney( "ϵͳ", dwMoney );
				pCha->SystemNotice( "��ϲ���쵼�Ĺ��ᡶ%s��ȡ���˹���ս��ʤ������ý�����%u����", pCha->GetGuildName(), dwMoney );
				LG( "��ս������", "��ϲ���쵼�Ĺ��ᡶ%s��ID��%u��ȡ���˹���ս��ʤ������ý�����%u����\n", pCha->GetGuildName(), 
					pCha->GetGuildID(), dwMoney );*/
				pCha->AddMoney( RES_STRING(GM_GAMEAPPNET_CPP_00017), dwMoney );
				pCha->SystemNotice( RES_STRING(GM_GAMEAPPNET_CPP_00010), pCha->GetGuildName(), dwMoney );
				LG( "challenge consortia result", "congratulate you leading consortia��%s��ID(%u)get win in consortia battle!gain bounty(%u)!\n", pCha->GetGuildName(), 
					pCha->GetGuildID(), dwMoney );
			}
			//else
			//{
			//	LG( "��ս������", "δ���ֹ���᳤��Ϣָ�룬�޷�����DBID[%u],Ǯ��[%u]\n", dwChaDBID, dwMoney );
			//}

			break;
		}
	case	CMD_MM_ADDCREDIT:
		{
			DWORD dwChaDBID = READ_LONG(pkt);
			long lCredit = READ_LONG(pkt);
			CPlayer* pPlayer = GetPlayerByDBID(dwChaDBID);
			if(pPlayer)
			{
				CCharacter* pCha = pPlayer->GetMainCha();
				pCha->SetCredit((long)pCha->GetCredit() + lCredit);
				pCha->SynAttr(enumATTRSYN_TASK);
			}
			break;
		}
	case	CMD_MM_STORE_BUY:
		{
			DWORD dwChaDBID = READ_LONG(pkt);
			long lComID = READ_LONG(pkt);
			//long lMoBean = READ_LONG(pkt);
			long lRplMoney = READ_LONG(pkt);
			CPlayer* pPlayer = GetPlayerByDBID(dwChaDBID);
			if(pPlayer)
			{
				CCharacter* pCha = pPlayer->GetMainCha();
				g_StoreSystem.Accept(pCha, lComID);
				//pCha->GetPlayer()->SetMoBean(lMoBean);
				pCha->GetPlayer()->SetRplMoney(lRplMoney);
			}
			break;
		}
	case CMD_MM_ADDMONEY:
		{
			DWORD dwChaID = READ_LONG(pkt);
			DWORD dwMoney = READ_LONG(pkt);

			CCharacter *pCha = NULL;
			CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(dwChaID);
			if(pPlayer)
			{
				pCha = pPlayer->GetMainCha();
			}
			if(pCha)
			{
				//pCha->AddMoney("ϵͳ", dwMoney);
				pCha->AddMoney(RES_STRING(GM_CHARACTERSUPERCMD_CPP_00023), dwMoney);
			}

			break;
		}
	case CMD_MM_AUCTION:
		//add by ALLEN 2007-10-19
		{
			
			DWORD dwChaID = READ_LONG(pkt);
			
			CCharacter *pCha = NULL;
			CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(dwChaID);
			if(pPlayer)
			{
				pCha = pPlayer->GetMainCha();
			}
			if(pCha)
			{
				g_CParser.DoString("AuctionEnd", enumSCRIPT_RETURN_NONE, 0, enumSCRIPT_PARAM_LIGHTUSERDATA, 1, pCha, DOSTRING_PARAM_END);
			}

			break;
		}
#ifdef SHUI_JING
	case CMD_MM_NOTICETOCHA:
		{
			long dwChaID = lSrcID;
			int type = READ_SHORT(pkt);
			DWORD Price = READ_LONG(pkt);
			DWORD Num = READ_LONG(pkt);
			DWORD tMoney = READ_LONG(pkt);
			CCharacter* pCha = NULL;
			CPlayer *pPlayer = g_pGameApp->GetPlayerByDBID(dwChaID);
			if( type == 1 )
			{
				if( pPlayer )
				{
					pCha = pPlayer->GetMainCha();
					if( pCha )
					{
						DWORD flatmoney = 0;
						//if( game_db.GetdwFlatMoney( pCha->GetName(), flatmoney ) )
						//{
						flatmoney = pCha->GetFlatMoney();
						pCha->SetFlatMoney( flatmoney + Price * Num );
						pCha->Cmd_TradeInfoAsr( type, true, Num, Price );
						pCha->Cmd_CrystalBuyAndSaleList();
					}
					//else
					//	game_db.SetdwFlatMoney( dwChaID, Price * Num );
				}
			}
			else if( type == 0 )
			{
				if( pPlayer )
				{
					pCha = pPlayer->GetMainCha();
					if( pCha )
					{
						pPlayer->SetRplMoney( tMoney );
						pCha->Cmd_TradeInfoAsr( type, true, Num, Price );
						pCha->Cmd_CrystalBuyAndSaleList();
					}
				}
			}
		break;
		}
#endif
	case CMD_MM_EXCHEXTERNVALUE:
		{
			const char* szScriptName = READ_STRING( pkt );
			long	m_lValue = READ_LONG( pkt );
			g_CParser.DoString( szScriptName,  enumSCRIPT_RETURN_NONE, 0, enumSCRIPT_PARAM_NUMBER, 1, m_lValue, DOSTRING_PARAM_END);
			break;
		}
	}

T_E}
void CGameApp::ProcessGroupBroadcast(unsigned short usCmd, GateServer *pGate, RPACKET pkt)
{T_B


T_E}
void CGameApp::ProcessGarner2Update(RPACKET pkt)//CMD_PM_GARNER2_UPDATE
{T_B
	long chaid[6];
	CPlayer * pplay;
//	CCharacter * pcha;
	chaid[0] = READ_LONG_R(pkt);
	chaid[1] = READ_LONG(pkt);
	chaid[2] = READ_LONG(pkt);
	chaid[3] = READ_LONG(pkt);
	chaid[4] = READ_LONG(pkt);
	chaid[5] = READ_LONG(pkt);
	if(0 != chaid[0])
	{
		pplay = FindPlayerByDBChaID(chaid[0]);
		if(pplay)
		{
			pplay->SetGarnerWiner(0);
		}
	}

	for(int i = 1;i<6 && chaid[i];i++)
	{
		pplay = FindPlayerByDBChaID(chaid[0]);
		if(pplay)
		{
			pplay->SetGarnerWiner(i);
		}
	}
T_E}

void CGameApp::ProcessMapAdmin(GateServer *pGate, RPACKET pkt)
{T_B
	
	LG("map_entrance_flow", "ProcessMapAdmin\n");

	if(pGate == NULL)
		return;

	cChar	*szMap = READ_STRING(pkt);

	if(szMap == NULL)
		return;

	CMapRes	*pCMapRes = FindMapByName(szMap);
	if (!pCMapRes)
	{
		return;
	}
	
	uShort id = READ_SHORT_R(pkt);
	uLong addr = READ_LONG_R(pkt);
	uLong dbID = READ_LONG_R(pkt);

	switch (READ_CHAR(pkt))
	{
	case enumMAPADMIN_OPEN_MAP:
		{
			pCMapRes->OpenMap();
		}
		break;
	case enumMAPADMIN_CLOSE_MAP:
		{
			pCMapRes->CloseMap();
		}
		break;
	case enumMAPADMIN_OPEN_MAP_ENTRY:
		{
			pCMapRes->OpenMapEntry();
		}
		break;
	case enumMAPADMIN_CLOSE_MAP_ENTRY:
		{
			pCMapRes->CloseMapEntry();
		}
		break;
	case enumMAPADMIN_CHECK_MAP:
		{
			string str;
			pCMapRes->CheckMapState(str);

			WPACKET	wpk	=GETWPACKET();
			WRITE_CMD(wpk, CMD_MC_SYSINFO);
			WRITE_STRING(wpk, str.c_str());

			WRITE_LONG(wpk, dbID);
			WRITE_LONG(wpk, addr);
			WRITE_SHORT(wpk, 1);

			pGate->SendData(wpk);
		}
		break;
	}

T_E}

void CGameApp::ProcessDynMapEntry(GateServer *pGate, RPACKET pkt)
{T_B
	cChar	*szTarMapN = READ_STRING(pkt);

	if(szTarMapN == NULL)
		return;

	cChar	*szSrcMapN = READ_STRING(pkt);

	if(szSrcMapN == NULL)
		return;

	switch (READ_CHAR(pkt))
	{
	case	enumMAPENTRY_CREATE:
		{
			CMapRes	*pCMapRes;
			SubMap	*pCMap;
			pCMapRes = FindMapByName(szTarMapN);
			if (!pCMapRes)
			{
				break;
			}
			pCMap = pCMapRes->GetCopy();
			Long	lPosX = READ_LONG(pkt);
			Long	lPosY = READ_LONG(pkt);
			Short	sMapCopyNum = READ_SHORT(pkt);
			Short	sCopyPlyNum = READ_SHORT(pkt);
			CDynMapEntryCell	CEntryCell;
			CEntryCell.SetMapName(szTarMapN);
			CEntryCell.SetTMapName(szSrcMapN);
			CEntryCell.SetEntiPos(lPosX, lPosY);
			CDynMapEntryCell	*pCEntry = g_CDMapEntry.Add(&CEntryCell);
			if (pCEntry)
			{
				if (g_cchLogMapEntry)
					//LG("��ͼ�������", "�յ������������λ�� %s --> %s[%u, %u]���ű����� %d\n", szSrcMapN, szTarMapN, lPosX, lPosY, sLineNum);
					LG("map_entrance_flow", "receive request to create entry:position %s --> %s[%u, %u]\n", szSrcMapN, szTarMapN, lPosX, lPosY);
				pCEntry->SetCopyNum(sMapCopyNum);
				pCEntry->SetCopyPlyNum(sCopyPlyNum);
				string	strScript;
				cChar	*cszSctLine;
				Short	sLineNum = READ_SHORT_R(pkt);
				while (--sLineNum >= 0)
				{
					cszSctLine = READ_STRING(pkt);
					strScript += cszSctLine;
					strScript += " ";
				}
				lua_dostring(g_pLuaState, strScript.c_str());
				g_CParser.DoString("config_entry", enumSCRIPT_RETURN_NONE, 0, enumSCRIPT_PARAM_LIGHTUSERDATA, 1, pCEntry, DOSTRING_PARAM_END);

				if (pCEntry->GetEntiID() > 0)
				{
					SItemGrid	SItemCont;
					SItemCont.sID = (Short)pCEntry->GetEntiID();
					SItemCont.sNum = 1;
					SItemCont.SetDBParam(-1, 0);
					SItemCont.chForgeLv = 0;
					SItemCont.SetInstAttrInvalid();
					CItem *pCItem = pCMap->ItemSpawn(&SItemCont, lPosX, lPosY, enumITEM_APPE_NATURAL, 0, g_pCSystemCha->GetID(), g_pCSystemCha->GetHandle(), -1, -1,
						pCEntry->GetEvent());
					if (pCItem)
					{
						pCItem->SetOnTick(0);
						pCEntry->SetEnti(pCItem);
					}
					else
					{
						if (g_cchLogMapEntry)
							//LG("��ͼ�������", "�������ʧ�ܣ�λ�� %s --> %s[%u, %u]������ %u ����ʧ��\n", szSrcMapN, szTarMapN, lPosX, lPosY, SItemCont.sID);
							LG("map_entrance_flow", "create entry failed:position %s --> %s[%u, %u],item %u create failed\n", szSrcMapN, szTarMapN, lPosX, lPosY, SItemCont.sID);
						g_CDMapEntry.Del(pCEntry);
						break;
					}
				}
				// ֪ͨԴ���������ɹ�
				WPACKET	wpk	=GETWPACKET();
				WRITE_CMD(wpk, CMD_MT_MAPENTRY);
				WRITE_STRING(wpk, szSrcMapN);
				WRITE_STRING(wpk, szTarMapN);
				WRITE_CHAR(wpk, enumMAPENTRY_RETURN);
				WRITE_CHAR(wpk, enumMAPENTRYO_CREATE_SUC);

				BEGINGETGATE();
				GateServer	*pGateServer = NULL;
				while (pGateServer = GETNEXTGATE())
				{
					pGateServer->SendData(wpk);
					break;
				}
				if (g_cchLogMapEntry)
					//LG("��ͼ�������", "������ڳɹ���λ�� %s --> %s[%u, %u] \n", szSrcMapN, szTarMapN, lPosX, lPosY);
					LG("map_entrance_flow", "create entry success:position %s --> %s[%u, %u] \n", szSrcMapN, szTarMapN, lPosX, lPosY);

				g_CParser.DoString("after_create_entry", enumSCRIPT_RETURN_NONE, 0, enumSCRIPT_PARAM_LIGHTUSERDATA, 1, pCEntry, DOSTRING_PARAM_END);
			}
			else
			{
				if (g_cchLogMapEntry)
					//LG("��ͼ�������", "������ڳɹ���λ�� %s --> %s[%u, %u] \n", szSrcMapN, szTarMapN, lPosX, lPosY);
					LG("map_entrance_flow", "create entry error:position %s --> %s[%u, %u] \n", szSrcMapN, szTarMapN, lPosX, lPosY);
			}
		}
		break;
	case	enumMAPENTRY_SUBPLAYER:
		{
			Short	sCopyNO = READ_SHORT(pkt);
			Short	sSubNum = READ_SHORT(pkt);

			CDynMapEntryCell	*pCEntry = g_CDMapEntry.GetEntry(szSrcMapN);
			if (pCEntry)
			{
				CMapEntryCopyCell	*pCCopyInfo = pCEntry->GetCopy(sCopyNO);
				if (pCCopyInfo)
					pCCopyInfo->AddCurPlyNum(-1 * sSubNum);
			}
		}
		break;
	case	enumMAPENTRY_SUBCOPY:
		{
			Short	sCopyNO = READ_SHORT(pkt);

			CDynMapEntryCell	*pCEntry = g_CDMapEntry.GetEntry(szSrcMapN);
			if (pCEntry)
			{
				pCEntry->ReleaseCopy(sCopyNO);
			}
			// ֪ͨԴ���������رճɹ�
			WPACKET	wpk	=GETWPACKET();
			WRITE_CMD(wpk, CMD_MT_MAPENTRY);
			WRITE_STRING(wpk, szSrcMapN);
			WRITE_STRING(wpk, szTarMapN);
			WRITE_CHAR(wpk, enumMAPENTRY_RETURN);
			WRITE_CHAR(wpk, enumMAPENTRYO_COPY_CLOSE_SUC);
			WRITE_SHORT(wpk, sCopyNO);

			BEGINGETGATE();
			GateServer	*pGateServer = NULL;
			while (pGateServer = GETNEXTGATE())
			{
				pGateServer->SendData(wpk);
				break;
			}
		}
		break;
	case	enumMAPENTRY_DESTROY:
		{
			CDynMapEntryCell	*pCEntry = g_CDMapEntry.GetEntry(szSrcMapN);
			if (g_cchLogMapEntry)
				//LG("��ͼ�������", "�յ������������λ�� %s --> %s\n", szSrcMapN, szTarMapN);
				LG("map_entrance_flow", "receive request to destroy entry:position %s --> %s\n", szSrcMapN, szTarMapN);
			if (pCEntry)
			{
				string	strScript = "after_destroy_entry_";
				strScript += szSrcMapN;
				g_CParser.DoString(strScript.c_str(), enumSCRIPT_RETURN_NONE, 0, enumSCRIPT_PARAM_LIGHTUSERDATA, 1, pCEntry, DOSTRING_PARAM_END);
				g_CDMapEntry.Del(pCEntry);

				// ֪ͨԴ�������ٳɹ�
				WPACKET	wpk	=GETWPACKET();
				WRITE_CMD(wpk, CMD_MT_MAPENTRY);
				WRITE_STRING(wpk, szSrcMapN);
				WRITE_STRING(wpk, szTarMapN);
				WRITE_CHAR(wpk, enumMAPENTRY_RETURN);
				WRITE_CHAR(wpk, enumMAPENTRYO_DESTROY_SUC);

				BEGINGETGATE();
				GateServer	*pGateServer = NULL;
				while (pGateServer = GETNEXTGATE())
				{
					pGateServer->SendData(wpk);
					break;
				}
				if (g_cchLogMapEntry)
					//LG("��ͼ�������", "������ڳɹ���λ�� %s --> %s\n", szSrcMapN, szTarMapN);
					LG("map_entrance_flow", "destroy entry success:position %s --> %s\n", szSrcMapN, szTarMapN);
			}
			else
			{
				LG("map_entrance_flow", "destroy entry error:position %s --> %s\n", szSrcMapN, szTarMapN);
			}
		}
		break;
	case	enumMAPENTRY_COPYPARAM:
		{
			CMapRes	*pCMapRes;
			SubMap	*pCMap;
			pCMapRes = FindMapByName(szTarMapN);
			if (!pCMapRes)
				break;
			pCMap = pCMapRes->GetCopy(READ_SHORT(pkt));
			if (!pCMap)
				break;
			for (dbc::Char i = 0; i < defMAPCOPY_INFO_PARAM_NUM; i++)
				pCMap->SetInfoParam(i, READ_LONG(pkt));
		}
		break;
	case	enumMAPENTRY_COPYRUN:
		{
			CMapRes	*pCMapRes;
			SubMap	*pCMap;
			pCMapRes = FindMapByName(szTarMapN);
			if (!pCMapRes)
				break;
			pCMap = pCMapRes->GetCopy(READ_SHORT(pkt));
			if (!pCMap)
				break;

			Char	chType = READ_CHAR(pkt);
			Long	lVal = READ_LONG(pkt);
			pCMapRes->SetCopyStartCondition(chType, lVal);
		}
		break;
	case	enumMAPENTRY_RETURN:
		{
			CMapRes	*pCMap = FindMapByName(szTarMapN, true);
			if (!pCMap)
				break;
			switch (READ_CHAR(pkt))
			{
			case	enumMAPENTRYO_CREATE_SUC:
				{
					pCMap->CheckEntryState(enumMAPENTRY_STATE_OPEN);
					if (g_cchLogMapEntry)
						//LG("��ͼ�������", "�յ���ڳɹ�������λ�� %s --> %s\n", szSrcMapN, szTarMapN);
						LG("map_entrance_flow", "receive entry create success :position %s --> %s\n", szSrcMapN, szTarMapN);
				}
				break;
			case	enumMAPENTRYO_DESTROY_SUC:
				{
					if (g_cchLogMapEntry)
						//LG("��ͼ�������", "�յ���ڳɹ����٣�λ�� %s --> %s\n", szSrcMapN, szTarMapN);
						LG("map_entrance_flow", "receive entry destroy success:position %s --> %s\n", szSrcMapN, szTarMapN);
					pCMap->CheckEntryState(enumMAPENTRY_STATE_CLOSE_SUC);
				}
				break;
			case	enumMAPENTRYO_COPY_CLOSE_SUC:
				{
					pCMap->CopyClose(READ_SHORT(pkt));
				}
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	};
T_E}
