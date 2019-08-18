/**************************************************************************************************************
*
*			ͨ��Э������(cmd)�����ļ�											(Created by Andor.Zhang in 2004.11)
*
**************************************************************************************************************/
#ifndef NETCOMMAND_H
#define NETCOMMAND_H

#define NET_PROTOCOL_ENCRYPT   1	// ����Э���Ƿ���ܣ������ҵ��޸ģ�
#define CMD_INVALID		       0	//��Ч����
//��������������������������������������������������������������������������������������
//                 Э�������(16bit)
//			����崫�䷽���ʶ(��CMD_CM_XXX�е�CM)��
//				CM	(C)lient		->Ga(m)eServer
//				MC	Ga(m)eServer	->(C)lient
//						......
//				(������"Э�������������"����)
//	(ע��ÿ�����������ռ���Ԥ��500��������ÿ�������1��ʼ����CMD_CM_BASE+1��ʼ����)
//	������Э�鶨���Ƽ�ʹ�á�MSG_����ʼ���磺MSG_TEAM_CANCLE_TIMEOUT;���Ϊ1�ֽ�(8bit)��ʹ��WriteChar/ReadChar����
//��������������������������������������������������������������������������������������
/*=====================================================================================================
*					Э������������壺
*/
#define CMD_CM_BASE			   0    // (C)lient       -> Ga(m)eServer
#define CMD_MC_BASE			 500    // Ga(m)eServer   -> (C)lient
#define CMD_TM_BASE			1000	// Ga(t)eServer   -> Ga(m)eServer
#define CMD_MT_BASE			1500	// Ga(m)eServer   -> Ga(t)eServer
#define CMD_TP_BASE         2000    // Ga(t)eServer   -> Grou(p)Server
#define CMD_PT_BASE         2500    // Grou(p)Server  -> Ga(t)eServer
#define CMD_PA_BASE         3000    // Grou(p)Server  -> (A)ccountServer
#define CMD_AP_BASE         3500    // (A)ccoutServer -> Grou(p)Server
#define CMD_MM_BASE			4000	// Ga(m)eServer	  -> Ga(m)eServer
#define CMD_PM_BASE         4500    // Grou(p)Server  -> Ga(m)eServer
#define	CMD_PC_BASE			5000	// Grou(p)Server  -> (C)lient
#define CMD_MP_BASE			5500	// Ga(m)eServer   -> Grou(p)Server
#define CMD_CP_BASE			6000	// (C)lient		  -> Grou(p)Server

// Add by lark.li 20081209 begin
#define CMD_OS_BASE			6500	// M(o)nitor	  -> (S)erver
#define CMD_SO_BASE			7000	//(S)erver		  ->  M(o)nitor
// End

// Add by lark.li 20090601 begin
#define CMD_AB_BASE			7500	// (A)ccoutServer -> (B)illingServer
#define CMD_BA_BASE			8000	// (B)illingServer-> (A)ccoutServer
// End

#include <map>
#include <string>
#include <utility>
using namespace std;

extern map<int, pair<string, string>> MsgMap;

inline void AddMessage(int id, const char* name, const char* description)
{
	if(MsgMap.find(id) != MsgMap.end())
		throw "The Same Message ID";

	MsgMap.insert(map<int, pair<string, string>>::value_type(id, make_pair<string, string>(name, description)));
}

inline string& GetMessageName(int id)
{
	map<int, pair<string, string>>::iterator it = MsgMap.find(id);

	if(it == MsgMap.end())
		throw "The Message ID not exsit";
	
	return it->second.first;
}

#define MESSAGE_DEF(name, id, description)		const int name = id;
#define MESSAGE_ADD(name, id, description)		AddMessage(id, "" #name  "", "" #description  "");
#define MESSAGE_NAME(id)						MsgMap[id].first
#define MESSAGE_DESC(id)						MsgMap[id].second
#define MESSAGE_NUM								MsgMap.size()

/*=====================================================================================================
*				(C)lient -> Ga(m)eServer
*/
MESSAGE_DEF(	CMD_CM_ROLEBASE,				CMD_CM_BASE + 300,	(300-400)����ϵͳ�������)
MESSAGE_DEF(	CMD_CM_GULDBASE,				CMD_CM_BASE	+ 400,	(400-430)���������ַ)
MESSAGE_DEF(	CMD_CM_CHARBASE,				CMD_CM_BASE	+ 430,	(430-450)��ɫ�������)

MESSAGE_DEF(	CMD_CM_SAY,						CMD_CM_BASE + 1,	��Ұ��˵��)
MESSAGE_DEF(	CMD_CM_BEGINACTION,				CMD_CM_BASE + 6,	��)
	//char	�ж����ͣ�1���ƶ���2��ʹ�ü��ܣ�3����װ��4�������ӣ�5���ϴ���
	//	����1���ƶ�
	//		long	ƽ��pingֵ
	//		Point	�����У�����player�ĵ�ǰλ�ã�
	//	����2��ʹ�ü���
	//		char	���������ͣ�1��ֱ��ʹ�ã�2�����ƶ���ʹ�ã�
	//			����1��ֱ��ʹ��
	//				long	���ܱ�ţ�0��������������������ID��
	//				���ܲ��������ݼ��ܵ����ͣ����壬��Χ��ѡ�����в���
	//				long	Ŀ��ID
	//				Point	Ŀ��λ��
	//			����2�����ƶ���ʹ��
	//				long	ƽ��pingֵ
	//				Point	�����У�����player�ĵ�ǰλ�ã�
	//				long	���ܱ�ţ�0��������������������ID��
	//				���ܲ��������ݼ��ܵ����ͣ����壬��Χ��ѡ�����в���
	//				long	Ŀ��ID
	//				Point	Ŀ��λ��
	//	����3����װ
	//	����4��
	//	����5���ϴ�
	//		ulong	����ID�����Ϊ0����ʾ�ϴ����ɹ�
	//	����6���´�
	//		char	0���´����ɹ���1���´��ɹ�

MESSAGE_DEF(	CMD_CM_ENDACTION,				CMD_CM_BASE + 7,	��)
MESSAGE_DEF(	CMD_CM_SYNATTR,					CMD_CM_BASE + 8,	ͬ����ɫ���ԣ�ͬ�����Ե�����ֵ��)
MESSAGE_DEF(	CMD_CM_SYNSKILLBAG,				CMD_CM_BASE + 9,	ͬ��������)
MESSAGE_DEF(	CMD_CM_DIE_RETURN,				CMD_CM_BASE + 10,	��������)
MESSAGE_DEF(	CMD_CM_SKILLUPGRADE,			CMD_CM_BASE + 11,	��)
MESSAGE_DEF(	CMD_CM_PING,					CMD_CM_BASE + 15,	��)
MESSAGE_DEF(	CMD_CM_REFRESH_DATA,			CMD_CM_BASE + 16,	��)
MESSAGE_DEF(	CMD_CM_CHECK_PING,				CMD_CM_BASE + 17,	����Ping)
MESSAGE_DEF(	CMD_CM_MAP_MASK,				CMD_CM_BASE + 18,	��)

MESSAGE_DEF(	CMD_CM_UPDATEHAIR,				CMD_CM_BASE + 20,	�ͻ��������������)
MESSAGE_DEF(	CMD_CM_TEAM_FIGHT_ASK,			CMD_CM_BASE + 21,	������ս����)
MESSAGE_DEF(	CMD_CM_TEAM_FIGHT_ASR,			CMD_CM_BASE + 22,	������սӦ��(ANSWER))
MESSAGE_DEF(	CMD_CM_ITEM_REPAIR_ASK,			CMD_CM_BASE + 23,	������������)
MESSAGE_DEF(	CMD_CM_ITEM_REPAIR_ASR,			CMD_CM_BASE + 24,	��������Ӧ��)
MESSAGE_DEF(	CMD_CM_ITEM_FORGE_ASK,			CMD_CM_BASE + 25,	���߾�������)
MESSAGE_DEF(	CMD_CM_ITEM_FORGE_ASR,			CMD_CM_BASE + 26,	���߾���Ӧ��)
MESSAGE_DEF(	CMD_CM_TIGER_START,				CMD_CM_BASE + 27,	�����ϻ���)
MESSAGE_DEF(	CMD_CM_TIGER_STOP,				CMD_CM_BASE + 28,	ֹͣ�ϻ���)
//�ж�
MESSAGE_DEF(	CMD_CM_ITEM_FORGE_CANACTION,	CMD_CM_BASE + 29,	��������������ɣ�����������¡�)
//��������
MESSAGE_DEF(	CMD_CM_KITBAG_LOCK,				CMD_CM_BASE + 31,	������������)
MESSAGE_DEF(	CMD_CM_KITBAG_UNLOCK,			CMD_CM_BASE + 32,	������������)
MESSAGE_DEF(	CMD_CM_KITBAG_CHECK,			CMD_CM_BASE + 33,	��鱳������״̬)
MESSAGE_DEF(	CMD_CM_KITBAG_AUTOLOCK,			CMD_CM_BASE + 34,	�����Զ�����)
MESSAGE_DEF(	CMD_CM_KITBAGTEMP_SYNC,			CMD_CM_BASE + 35,	ͬ����ʱ����)

//�̳�ϵͳ
MESSAGE_DEF(	CMD_CM_STORE_OPEN_ASK,			CMD_CM_BASE + 41,	���̳�)
MESSAGE_DEF(	CMD_CM_STORE_LIST_ASK,			CMD_CM_BASE + 42,	�̳��б�)
MESSAGE_DEF(	CMD_CM_STORE_BUY_ASK,			CMD_CM_BASE + 43,	�������)
MESSAGE_DEF(	CMD_CM_STORE_CHANGE_ASK,		CMD_CM_BASE + 44,	�һ�����)
MESSAGE_DEF(	CMD_CM_STORE_QUERY,				CMD_CM_BASE + 45,	���׼�¼��ѯ)
MESSAGE_DEF(	CMD_CM_STORE_VIP,				CMD_CM_BASE + 46,	VIP)
MESSAGE_DEF(	CMD_CM_STORE_AFFICHE,			CMD_CM_BASE + 47,	������Ʒ)
MESSAGE_DEF(	CMD_CM_STORE_CLOSE,				CMD_CM_BASE + 48,	��)

//��������
MESSAGE_DEF(	CMD_CM_BLACKMARKET_EXCHANGE_REQ,CMD_CM_BASE + 51,	���жһ�)
MESSAGE_DEF(	CMD_CM_CHEAT_CHECK,				CMD_CM_BASE + 52,	��)

//���ִ�����
MESSAGE_DEF(	CMD_CM_VOLUNTER_LIST,			CMD_CM_BASE + 61,	��ʾ־Ը���б�)
MESSAGE_DEF(	CMD_CM_VOLUNTER_ADD,			CMD_CM_BASE + 62,	��ӵ�־Ը���б�)
MESSAGE_DEF(	CMD_CM_VOLUNTER_DEL,			CMD_CM_BASE + 63,	���б���ɾ��)
MESSAGE_DEF(	CMD_CM_VOLUNTER_SEL,			CMD_CM_BASE + 64,	�������)
MESSAGE_DEF(	CMD_CM_VOLUNTER_OPEN,			CMD_CM_BASE + 65,	��־Ը�����)
MESSAGE_DEF(	CMD_CM_VOLUNTER_ASR,			CMD_CM_BASE + 66,	־Ը�����Ӧ��)

//��ʦ
MESSAGE_DEF(	CMD_CM_MASTER_INVITE,			CMD_CM_BASE + 71,	��ʦ)
MESSAGE_DEF(	CMD_CM_MASTER_ASR,				CMD_CM_BASE + 72,	��)
MESSAGE_DEF(	CMD_CM_MASTER_DEL,				CMD_CM_BASE + 73,	��)
MESSAGE_DEF(	CMD_CM_PRENTICE_DEL,			CMD_CM_BASE + 74,	��)
MESSAGE_DEF(	CMD_CM_PRENTICE_INVITE,			CMD_CM_BASE + 75,	��)
MESSAGE_DEF(	CMD_CM_PRENTICE_ASR,			CMD_CM_BASE + 76,	��)

//�����
MESSAGE_DEF(	CMD_CM_LIFESKILL_ASR,			CMD_CM_BASE + 80,	��������ˣ��ϳɣ��ֽ�ȵ�����)
MESSAGE_DEF(	CMD_CM_LIFESKILL_ASK,			CMD_CM_BASE + 81,	��������ˣ��ϳɣ��ֽ�ȵ�ȷ��)

MESSAGE_DEF(	CMD_CM_BIDUP,					CMD_CM_BASE + 86,	��)

MESSAGE_DEF(	CMD_CM_SAY2CAMP,				CMD_CM_BASE + 91,	��)
MESSAGE_DEF(	CMD_CM_GM_SEND,					CMD_CM_BASE + 92,	��)
MESSAGE_DEF(	CMD_CM_GM_RECV,					CMD_CM_BASE + 93,	��)
MESSAGE_DEF(	CMD_CM_PK_CTRL,					CMD_CM_BASE + 94,	��)

// Add by lark.li 20080514 begin
MESSAGE_DEF(	CMD_CM_ITEM_LOTTERY_ASK,		CMD_CM_BASE + 95,	���߾�������)
MESSAGE_DEF(	CMD_CM_ITEM_LOTTERY_ASR,		CMD_CM_BASE + 96,	��Ʊ�趨Ӧ��)
// End

// Add by lark.li 20080707 begin
MESSAGE_DEF(	CMD_CM_CAPTAIN_CONFIRM_ASR,		CMD_CM_BASE + 97,	�������ӳ�Ӧ��)
// End

MESSAGE_DEF(	CMD_CM_ITEM_AMPHITHEATER_ASK,	CMD_CM_BASE + 98,	ͶƱ����)

//	2008-8-29	yangyinyu	add	begin!
MESSAGE_DEF(	CMD_CM_ITEM_LOCK_ASK,			CMD_CM_BASE	+ 99,	��)
//	2008-8-29	yangyinyu	add	end!
MESSAGE_DEF(	CMD_CM_ITEM_UNLOCK_ASK,			CMD_CM_BASE + 100,	��)

MESSAGE_DEF(    CMD_CM_SYNCHAR, CMD_CM_BASE + 101, SynChar)
MESSAGE_DEF(CMD_CM_GETORIGNAMECOLOR, CMD_CM_BASE + 102, GetOrigcolor)
MESSAGE_DEF(CMD_CM_CHANGETITLE, CMD_CM_BASE + 103, SetTitle)
MESSAGE_DEF(CMD_CM_SYNGUILDCOLOR, CMD_CM_BASE + 104, SynGuildColor)
//Begin ���������
	MESSAGE_DEF(	CMD_CM_GUILD_PUTNAME,		CMD_CM_GULDBASE	+ 1,	�ͻ������빫����)
	MESSAGE_DEF(	CMD_CM_GUILD_TRYFOR,		CMD_CM_GULDBASE	+ 2,	������빫��)
	MESSAGE_DEF(	CMD_CM_GUILD_TRYFORCFM,		CMD_CM_GULDBASE	+ 3,	ȷ���Ƿ񸲸���ǰ����)
	MESSAGE_DEF(	CMD_CM_GUILD_LISTTRYPLAYER,	CMD_CM_GULDBASE	+ 4,	�г����д��������)

	MESSAGE_DEF(	CMD_CM_GUILD_APPROVE,		CMD_CM_GULDBASE	+ 5,	������׼�����˼���)
	MESSAGE_DEF(	CMD_CM_GUILD_REJECT,		CMD_CM_GULDBASE	+ 6,	����ܾ������˼���)
	MESSAGE_DEF(	CMD_CM_GUILD_KICK,			CMD_CM_GULDBASE	+ 7,	��)
	MESSAGE_DEF(	CMD_CM_GUILD_LEAVE,			CMD_CM_GULDBASE	+ 8,	��)
	MESSAGE_DEF(	CMD_CM_GUILD_DISBAND,		CMD_CM_GULDBASE	+ 9,	��)
	MESSAGE_DEF(	CMD_CM_GUILD_MOTTO,			CMD_CM_GULDBASE	+ 10,	��)
	MESSAGE_DEF(	CMD_CM_GUILD_CHALLENGE,		CMD_CM_GULDBASE + 11,	��ս����)
	MESSAGE_DEF(	CMD_CM_GUILD_LEIZHU,		CMD_CM_GULDBASE + 12,	������)
	MESSAGE_DEF(	CMD_CM_GUILD_PERM,			CMD_CM_GULDBASE + 13,	Guild Perm)
//End ���������

//Begin ��ɫ�����͵�¼����CMD_CM_CHARBASE
	MESSAGE_DEF(	CMD_CM_LOGIN,				CMD_CM_CHARBASE + 1,	�û��ʺŵ���(�û���/�������֤))
	MESSAGE_DEF(	CMD_CM_LOGOUT,				CMD_CM_CHARBASE + 2,	�û��ʺŵǳ�)
	MESSAGE_DEF(	CMD_CM_BGNPLAY,				CMD_CM_CHARBASE + 3,	�û�ѡ���ɫ��ʼ����Ϸ)
	MESSAGE_DEF(	CMD_CM_ENDPLAY,				CMD_CM_CHARBASE + 4,	�û���������Ϸ����ѡ/��/ɾ��ɫ����)
	MESSAGE_DEF(	CMD_CM_NEWCHA,				CMD_CM_CHARBASE	+ 5,	�û��½���ɫ)
	MESSAGE_DEF(	CMD_CM_DELCHA,				CMD_CM_CHARBASE	+ 6,	�û�ɾ����ɫ)
	MESSAGE_DEF(	CMD_CM_CANCELEXIT,			CMD_CM_CHARBASE + 7,	�û�Ҫ��ȡ���˳�)
//End ��ɫ�����͵�¼����

//Begin����ϵͳָ��
	MESSAGE_DEF(	CMD_CM_REQUESTNPC,			CMD_CM_ROLEBASE + 1,	�ͻ��������NPC����)
	MESSAGE_DEF(	CMD_CM_REQUESTTALK,			CMD_CM_ROLEBASE + 1,	�ͻ���������NPC�Ի�����)
	MESSAGE_DEF(	CMD_CM_TALKPAGE,			CMD_CM_ROLEBASE + 2,	�ͻ���������NPC�Ի�ҳ����)
	MESSAGE_DEF(	CMD_CM_FUNCITEM,			CMD_CM_ROLEBASE + 3,	�ͻ�������ָ������ѡ�����)

	MESSAGE_DEF(	CMD_CM_REQUESTTRADE,		CMD_CM_ROLEBASE + 8,	�ͻ���������н���)
	MESSAGE_DEF(	CMD_CM_TRADEITEM,			CMD_CM_ROLEBASE + 9,	�ͻ�����������Ʒ)
	MESSAGE_DEF(	CMD_CM_REQUESTAGENCY,		CMD_CM_ROLEBASE + 10,	�ͻ�������npc������)

	//��ɫ����ָ��
	MESSAGE_DEF(	CMD_CM_CHARTRADE_REQUEST,	CMD_CM_ROLEBASE + 12,	����������)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_ACCEPT,	CMD_CM_ROLEBASE + 13,	���ܽ�������)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_REJECT,	CMD_CM_ROLEBASE + 14,	�ܾ���������)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_CANCEL,	CMD_CM_ROLEBASE + 15,	ȡ������)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_ITEM,		CMD_CM_ROLEBASE + 16,	��ɫ������Ʒ)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_VALIDATEDATA,CMD_CM_ROLEBASE + 17,	ȷ�Ͻ�������)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_VALIDATE,	CMD_CM_ROLEBASE + 18,	ȷ�Ͻ���)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_MONEY,		CMD_CM_ROLEBASE + 19,	�϶���Ǯ)

	// ����
	MESSAGE_DEF(	CMD_CM_MISSION,				CMD_CM_ROLEBASE + 22,	�ͻ�������������)
	MESSAGE_DEF(	CMD_CM_MISSIONLIST,			CMD_CM_ROLEBASE + 23,	�����б�)
	MESSAGE_DEF(	CMD_CM_MISSIONTALK,			CMD_CM_ROLEBASE + 24,	����Ի�)
	MESSAGE_DEF(	CMD_CM_MISLOG,				CMD_CM_ROLEBASE + 25,	������־)
	MESSAGE_DEF(	CMD_CM_MISLOGINFO,			CMD_CM_ROLEBASE + 26,	����������ϸ��Ϣ)
	MESSAGE_DEF(	CMD_CM_MISLOG_CLEAR,		CMD_CM_ROLEBASE + 27,	����ɾ��һ������)
	
	// ��̯
	MESSAGE_DEF(	CMD_CM_STALL_ALLDATA,		CMD_CM_ROLEBASE + 30,	�����̯������Ϣ)
	MESSAGE_DEF(	CMD_CM_STALL_OPEN,			CMD_CM_ROLEBASE + 31,	����򿪰�̯������Ϣ)
	MESSAGE_DEF(	CMD_CM_STALL_BUY,			CMD_CM_ROLEBASE + 32,	�������̯����)
	MESSAGE_DEF(	CMD_CM_STALL_CLOSE,			CMD_CM_ROLEBASE + 33,	������̯)

	// ����
// 	MESSAGE_DEF(	CMD_CM_FORGE			CMD_CM_ROLEBASE + 35 // ��������Ʒ //������ -Waiting Mark 2009-03-25

    
	// ��ֻ
	MESSAGE_DEF(	CMD_CM_CREATE_BOAT,			CMD_CM_ROLEBASE + 38,	���󴴽���)
	MESSAGE_DEF(	CMD_CM_UPDATEBOAT_PART,		CMD_CM_ROLEBASE + 39,	���������ֻ����)
	MESSAGE_DEF(	CMD_CM_BOAT_CANCEL,			CMD_CM_ROLEBASE + 40,	����ȡ�����촬ֻ)
	MESSAGE_DEF(	CMD_CM_BOAT_LUANCH,			CMD_CM_ROLEBASE + 41,	ѡ��ֻ����)
	MESSAGE_DEF(	CMD_CM_BOAT_BAGSEL,			CMD_CM_ROLEBASE + 42,	ѡ��һ�Ҵ�ֻ���մ�����л���)
	MESSAGE_DEF(	CMD_CM_BOAT_SELECT,			CMD_CM_ROLEBASE + 43,	ѡ��ֻ)
	MESSAGE_DEF(	CMD_CM_BOAT_GETINFO,		CMD_CM_ROLEBASE + 44,	��ȡ��ֻ��Ϣ)

	// ʵ��
	MESSAGE_DEF(	CMD_CM_ENTITY_EVENT,		CMD_CM_ROLEBASE + 45,	����ʵ���¼�)

	// ��������Э��
	MESSAGE_DEF(	CMD_CM_CREATE_PASSWORD2,	CMD_CM_ROLEBASE + 46,	������������)
	MESSAGE_DEF(	CMD_CM_UPDATE_PASSWORD2,	CMD_CM_ROLEBASE + 47,	���¶�������)

    //����
    MESSAGE_DEF(	CMD_CM_READBOOK_START,		CMD_CM_ROLEBASE + 48,	��ʼ����)
	MESSAGE_DEF(	CMD_CM_READBOOK_CLOSE,		CMD_CM_ROLEBASE + 49,	��������)

	MESSAGE_DEF(	CMD_CM_SELECT_TIME_ASK,		CMD_CM_ROLEBASE + 50,	ѡ�������ʱ��)
	MESSAGE_DEF(	CMD_CM_BEHINDGENIUS_ASK,	CMD_CM_ROLEBASE + 51,	�������챳����)
	MESSAGE_DEF(	CMD_CM_GET_OUTTIMEEXP,		CMD_CM_ROLEBASE + 52,	��)
	MESSAGE_DEF(	CMD_CM_SELECT_COIN_ASK,		CMD_CM_ROLEBASE + 53,	��)
	MESSAGE_DEF(	CMD_CM_REQUEST_ANYTIMETRADE,CMD_CM_ROLEBASE + 54,	��ʱ����)
	MESSAGE_DEF(	CMD_CM_REQUEST_JEWELRYUP_ASK,CMD_CM_ROLEBASE + 55,	��ʯ����)

	//ˮ������ƽ̨ Add by sunny.sun 20090409 
	MESSAGE_DEF(	CMD_CM_OPEN_CRYSTALTRADE_ASK,CMD_CM_ROLEBASE + 56,	��ˮ������ƽ̨)
	MESSAGE_DEF(	CMD_CM_REQUEST_BUY_ASK,		CMD_CM_ROLEBASE + 57,	����ҵ�)
	MESSAGE_DEF(	CMD_CM_REQUEST_SALE_ASK,	CMD_CM_ROLEBASE + 58,	�����ҵ�)
	MESSAGE_DEF(	CMD_CM_REQUEST_CANCEL_ASK,	CMD_CM_ROLEBASE + 59,	ȡ���ҵ�)
	MESSAGE_DEF(	CMD_CM_REQUEST_GETFLATMONEY,CMD_CM_ROLEBASE + 60,	ȡ��ƽ̨�˺��ϵ�Ǯ)
	MESSAGE_DEF(	CMD_CM_REQUEST_REFRESH_ASK,	CMD_CM_ROLEBASE + 61,	����ˢ��)
	MESSAGE_DEF(	CMD_CM_CHA_CONFIRM_CRYSTALTRADE,CMD_CM_ROLEBASE + 62,	���ȷ�Ͻ�����Ϣ)
//End ����ϵͳָ��

//Begin�Ҷ�����������ϵͳ
MESSAGE_DEF(	CMD_CM_GARNER2_REORDER,			CMD_CM_ROLEBASE + 71,	�ͻ�������������)
//End�Ҷ�����������ϵͳ


MESSAGE_DEF(	CMD_CM_ANTIINDULGENCE,			CMD_CM_ROLEBASE + 72,	�������ƽ�)
//Add by sunny.sun20090728
MESSAGE_DEF(	XTRAP_CMD_STEP_TWO,				CMD_CM_ROLEBASE + 73,	��)
MESSAGE_DEF(	XTRAP_MAP_FILE_CHANGE,			CMD_CM_ROLEBASE + 74,	��)
MESSAGE_DEF(	CMD_CM_OPEN_CYCELMISSION,		CMD_CM_ROLEBASE + 75,	����ѭ������)//Add by sunny.sun 20090831

/*=====================================================================================================
*		Ga(m)eServer -> (C)lient
*/
MESSAGE_DEF(	CMD_MC_ROLEBASE,				CMD_MC_BASE	+ 300,	(300-400)����ϵͳ�����ַ)
MESSAGE_DEF(	CMD_MC_GULDBASE,				CMD_MC_BASE	+ 400,	(400-430)���������ַ)
MESSAGE_DEF(	CMD_MC_CHARBASE,				CMD_MC_BASE	+ 430,	(430-450)��ɫ�����ַ)

MESSAGE_DEF(	CMD_MC_SAY,						CMD_MC_BASE + 1,	��Ұ��˵��)
MESSAGE_DEF(	CMD_MC_MAPCRASH,				CMD_MC_BASE	+ 3,	��)
MESSAGE_DEF(	CMD_MC_CHABEGINSEE,				CMD_MC_BASE + 4,	��)
MESSAGE_DEF(	CMD_MC_CHAENDSEE,				CMD_MC_BASE + 5,	��)
MESSAGE_DEF(	CMD_MC_ITEMBEGINSEE,			CMD_MC_BASE + 6,	��)
MESSAGE_DEF(	CMD_MC_ITEMENDSEE,				CMD_MC_BASE + 7,	��)

MESSAGE_DEF(	CMD_MC_NOTIACTION,				CMD_MC_BASE + 8,	��)
	//long	��ɫID
	//char	�ж����ͣ�1���ƶ���2��ʹ�ü��ܣ�3������Ӱ�죩
	//	����1���ƶ�
	//		char	״̬��0x00���ƶ��С�0x01������Ŀ���ֹͣ��0x02�����ϰ�ֹͣ��0x04���ͻ���Ҫ��ֹͣ��0x08������Ҫ��ķ�Χ(�繥����Χ)ֹͣ����
	//		Point	�����У�����player�ĵ�ǰλ�ã�
	//	����2��ʹ�ü���
	//		char	״̬��0x00��ʹ���С�0x01������������ֹͣ��0x02�������뿪ʹ�÷�Χ��ֹͣ��0x04���ﵽ��ʹ��Ŀ�Ķ�ֹͣ����������0x08���ͻ�Ҫ��ֹͣ����
	//		long	���ܱ�ţ�0��������������������ID��
	//		���ܲ��������ݼ��ܵ����ͣ����壬��Χ��ѡ�����в���
	//		long	Ŀ��ID
	//		Point	Ŀ��λ��
	//	����3������Ӱ��
	//		long	ʹ�÷��Ľ�ɫID
	//		long	ʹ�õļ���ID
	//		�������
	//		long	Ӱ�������ID
	//		long	Ӱ�����ֵ
	//	����4����װ
MESSAGE_DEF(	CMD_MC_SYNATTR,					CMD_MC_BASE + 9,	ͬ����ɫ���ԣ�ͬ�����Ե�����ֵ��)
MESSAGE_DEF(	CMD_MC_SYNSKILLBAG,				CMD_MC_BASE + 10,	ͬ��������)
MESSAGE_DEF(	CMD_MC_SYNASKILLSTATE,			CMD_MC_BASE + 11,	ͬ����ɫ״̬)
MESSAGE_DEF(	CMD_MC_PING,					CMD_MC_BASE + 15,	��)
MESSAGE_DEF(	CMD_MC_ENTERMAP,				CMD_MC_BASE	+ 16,	GameServer֪ͨClient�����ͼ��������ҵ�������Ϣ���丽���ṹ)
MESSAGE_DEF(	CMD_MC_SYSINFO,					CMD_MC_BASE	+ 17,	ϵͳ��Ϣ��ʾ�������Ҳ����ߣ��㻹û�����ʲô����ȵȡ�)
MESSAGE_DEF(	CMD_MC_ALARM,					CMD_MC_BASE	+ 18,	��)
MESSAGE_DEF(	CMD_MC_TEAM,					CMD_MC_BASE + 19,	GameServer֪ͨ�ͻ���ĳ����Ա������ˢ��(�������, HP,SP))
MESSAGE_DEF(	CMD_MC_FAILEDACTION,			CMD_MC_BASE + 20,	������ж�ִ��ʧ��)
MESSAGE_DEF(	CMD_MC_MESSAGE,					CMD_MC_BASE + 21,	��ʾ��Ϣ)
MESSAGE_DEF(	CMD_MC_ASTATEBEGINSEE,			CMD_MC_BASE + 22,	����״̬)
MESSAGE_DEF(	CMD_MC_ASTATEENDSEE,			CMD_MC_BASE + 23,	��)
MESSAGE_DEF(	CMD_MC_TLEADER_ID,				CMD_MC_BASE + 24,	GameServer֪ͨ�ͻ���ĳ����Ա�Ķӳ���Ϣ)
MESSAGE_DEF(	CMD_MC_CHA_EMOTION,				CMD_MC_BASE + 25,	GameServer����ָ����ɫ������   )

MESSAGE_DEF(	CMD_MC_QUERY_CHA,				CMD_MC_BASE + 26,	��ѯ��ɫ������Ϣ)
MESSAGE_DEF(	CMD_MC_QUERY_CHAITEM,			CMD_MC_BASE + 27,	��ѯ��ɫ����)
MESSAGE_DEF(	CMD_MC_CALL_CHA,				CMD_MC_BASE + 28,	����ɫ�ٻ����Լ����)
MESSAGE_DEF(	CMD_MC_GOTO_CHA,				CMD_MC_BASE + 29,	���Լ����͵���ɫ���)
MESSAGE_DEF(	CMD_MC_KICK_CHA,				CMD_MC_BASE + 30,	����ɫ������)
MESSAGE_DEF(	CMD_MC_SYNDEFAULTSKILL,			CMD_MC_BASE + 31,	ͬ��Ĭ�ϼ���)
MESSAGE_DEF(	CMD_MC_ADD_ITEM_CHA,			CMD_MC_BASE + 32,	��ӵ��߽�ɫ)
MESSAGE_DEF(	CMD_MC_DEL_ITEM_CHA,			CMD_MC_BASE + 33,	ɾ�����߽�ɫ)
MESSAGE_DEF(	CMD_MC_QUERY_CHAPING,			CMD_MC_BASE + 34,	��ѯ��ɫ��GameServer�߼����pingֵ)
MESSAGE_DEF(	CMD_MC_QUERY_RELIVE,			CMD_MC_BASE + 35,	��ѯ�Ƿ����ԭ�ظ���)
MESSAGE_DEF(	CMD_MC_PREMOVE_TIME,			CMD_MC_BASE + 36,	Ԥ�ƶ�ʱ��)
MESSAGE_DEF(	CMD_MC_CHECK_PING,				CMD_MC_BASE + 37,	����Ping)
MESSAGE_DEF(	CMD_MC_MAP_MASK,				CMD_MC_BASE + 38,	��)

MESSAGE_DEF(	CMD_MC_OPENHAIR,				CMD_MC_BASE + 39,	��������)
MESSAGE_DEF(	CMD_MC_UPDATEHAIR_RES,			CMD_MC_BASE + 40,	�������͵Ľ��)
MESSAGE_DEF(	CMD_MC_EVENT_INFO,				CMD_MC_BASE + 41,	ͬ���¼���Ϣ)
MESSAGE_DEF(	CMD_MC_SIDE_INFO,				CMD_MC_BASE + 42,	ͬ���ֱ���Ϣ)
MESSAGE_DEF(	CMD_MC_TEAM_FIGHT_ASK,			CMD_MC_BASE + 43,	������ս����)
MESSAGE_DEF(	CMD_MC_ITEM_REPAIR_ASK,			CMD_MC_BASE + 44,	������������)
MESSAGE_DEF(	CMD_MC_ITEM_REPAIR_ASR,			CMD_MC_BASE + 45,	��������Ӧ��)
MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_REPAIR,		CMD_MC_BASE + 46,	��ʼ��������)
MESSAGE_DEF(	CMD_MC_APPEND_LOOK,				CMD_MC_BASE + 47,	�������)
MESSAGE_DEF(	CMD_MC_ITEM_FORGE_ASK,			CMD_MC_BASE + 48,	���߾�������)
MESSAGE_DEF(	CMD_MC_ITEM_FORGE_ASR,			CMD_MC_BASE + 49,	���߾���Ӧ��)
MESSAGE_DEF(	CMD_MC_ITEM_USE_SUC,			CMD_MC_BASE + 50,	����ʹ�óɹ�)


MESSAGE_DEF(	CMD_MC_KITBAG_CAPACITY,			CMD_MC_BASE + 51,	��������)
MESSAGE_DEF(	CMD_MC_ESPE_ITEM,				CMD_MC_BASE + 52,	�������)

//��������
MESSAGE_DEF(	CMD_MC_KITBAG_CHECK_ASR,		CMD_MC_BASE + 53,	��������״̬��Ӧ��)
MESSAGE_DEF(	CMD_MC_KITBAGTEMP_SYNC,			CMD_MC_BASE + 54,	ͬ����ʱ����)

//�̳�ϵͳ
MESSAGE_DEF(	CMD_MC_STORE_OPEN_ASR,			CMD_MC_BASE + 61,	���̳�)
MESSAGE_DEF(	CMD_MC_STORE_LIST_ASR,			CMD_MC_BASE + 62,	�̳��б�)
MESSAGE_DEF(	CMD_MC_STORE_BUY_ASR,			CMD_MC_BASE + 63,	�������)
MESSAGE_DEF(	CMD_MC_STORE_CHANGE_ASR,		CMD_MC_BASE + 64,	�һ�����)
MESSAGE_DEF(	CMD_MC_STORE_QUERY,				CMD_MC_BASE + 65,	���׼�¼��ѯ)
MESSAGE_DEF(	CMD_MC_STORE_VIP,				CMD_MC_BASE + 66,	VIP)
MESSAGE_DEF(	CMD_MC_STORE_AFFICHE,			CMD_MC_BASE + 67,	������Ʒ)
MESSAGE_DEF(	CMD_MC_POPUP_NOTICE,			CMD_MC_BASE + 68,	��)

//���н���
MESSAGE_DEF(	CMD_MC_BLACKMARKET_EXCHANGEDATA,	CMD_MC_BASE + 71,	���жһ�����)
MESSAGE_DEF(	CMD_MC_BLACKMARKET_EXCHANGE_ASR,	CMD_MC_BASE + 72,	���жһ�Ӧ��)
MESSAGE_DEF(	CMD_MC_BLACKMARKET_EXCHANGEUPDATE,	CMD_MC_BASE + 73,	���жһ�����)
MESSAGE_DEF(	CMD_MC_BLACKMARKET_TRADEUPDATE,	CMD_MC_BASE + 74,	���н��׸���)
MESSAGE_DEF(	CMD_MC_EXCHANGEDATA,			CMD_MC_BASE + 75,	��ͨ�һ�����)

//���ִ�����
MESSAGE_DEF(	CMD_MC_VOLUNTER_LIST,			CMD_MC_BASE + 81,	��ʾ־Ը���б�)
MESSAGE_DEF(	CMD_MC_VOLUNTER_STATE,			CMD_MC_BASE + 82,	־Ը��״̬)
MESSAGE_DEF(	CMD_MC_VOLUNTER_SEL,			CMD_MC_BASE + 83,	�������)
MESSAGE_DEF(	CMD_MC_VOLUNTER_OPEN,			CMD_MC_BASE + 84,	��־Ը�����)
MESSAGE_DEF(	CMD_MC_VOLUNTER_ASK,			CMD_MC_BASE + 85,	־Ը���������)

MESSAGE_DEF(	CMD_MC_LISTAUCTION,				CMD_MC_BASE + 86,	��)

//��ʦ
MESSAGE_DEF(	CMD_MC_MASTER_ASK,				CMD_MC_BASE + 91,	��)
MESSAGE_DEF(	CMD_MC_PRENTICE_ASK,			CMD_MC_BASE + 92,	��)
MESSAGE_DEF(	CMD_MC_CHAPLAYEFFECT,			CMD_MC_BASE + 93,	��)

MESSAGE_DEF(	CMD_MC_SAY2CAMP,				CMD_MC_BASE + 96,	��)
MESSAGE_DEF(	CMD_MC_GM_MAIL,					CMD_MC_BASE + 97,	��)
MESSAGE_DEF(	CMD_MC_CHEAT_CHECK,				CMD_MC_BASE + 98,	��)

//	2008-8-29	yangyinyu	add	begin!
MESSAGE_DEF(	CMD_CM_ITEM_LOCK_ASR,			CMD_MC_BASE	+	99,	��)
//	2008-8-29	yangyinyu	add	end!

MESSAGE_DEF(	CMD_MC_ITEM_UNLOCK_ASR,			CMD_MC_BASE	+	95,	��)

// FEATURE: GUILD_BANK
MESSAGE_DEF(CMD_MC_UPDATEGUILDBANKGOLD, CMD_MC_BASE + 120, Update Guild Bank Gold)

//Begin ���������
	MESSAGE_DEF(	CMD_MC_GUILD_GETNAME,		CMD_MC_GULDBASE	+ 1,	����ͻ������빫����)
	MESSAGE_DEF(	CMD_MC_LISTGUILD,			CMD_MC_GULDBASE	+ 2,	����ͻ��˴�������봰�ڲ������л��б�)
	MESSAGE_DEF(	CMD_MC_GUILD_TRYFORCFM,		CMD_MC_GULDBASE	+ 3,	ȷ���Ƿ񸲸���ǰ����)
	MESSAGE_DEF(	CMD_MC_GUILD_LISTTRYPLAYER,	CMD_MC_GULDBASE	+ 4,	�г����д��������)
	MESSAGE_DEF(	CMD_MC_GUILD_LISTCHALL,		CMD_MC_GULDBASE + 5,	��ѯ������ս��Ϣ)
	MESSAGE_DEF(	CMD_MC_GUILD_MOTTO,			CMD_MC_GULDBASE	+ 10,	�޸�������)
	MESSAGE_DEF(	CMD_MC_GUILD_LEAVE,			CMD_MC_GULDBASE + 11,	�뿪����)
	MESSAGE_DEF(	CMD_MC_GUILD_KICK,			CMD_MC_GULDBASE + 12,	���߳�����)
	MESSAGE_DEF(	CMD_MC_GUILD_INFO,			CMD_MC_GULDBASE + 13,	ͬ��������Ϣ)
//End ���������

//Begin ��ɫ�����͵�¼����CMD_MC_CHARBASE
	MESSAGE_DEF(	CMD_MC_LOGIN,				CMD_MC_CHARBASE + 1,	��)
	MESSAGE_DEF(	CMD_MC_LOGOUT,				CMD_MC_CHARBASE + 2,	��)
	MESSAGE_DEF(	CMD_MC_BGNPLAY,				CMD_MC_CHARBASE + 3,	��)
	MESSAGE_DEF(	CMD_MC_ENDPLAY,				CMD_MC_CHARBASE + 4,	��)
	MESSAGE_DEF(	CMD_MC_NEWCHA,				CMD_MC_CHARBASE	+ 5,	��)
	MESSAGE_DEF(	CMD_MC_DELCHA,				CMD_MC_CHARBASE	+ 6,	��)
	MESSAGE_DEF(	CMD_MC_STARTEXIT,			CMD_MC_CHARBASE + 7	,	��ʼ�˳�����ʱ)
	MESSAGE_DEF(	CMD_MC_CANCELEXIT,			CMD_MC_CHARBASE + 8	,	ȡ���˳�����ʱ)
	MESSAGE_DEF(	CMD_MC_CHAPSTR,				CMD_MC_CHARBASE	+ 10,	��)
	MESSAGE_DEF(	CMD_MC_CREATE_PASSWORD2,	CMD_MC_CHARBASE + 11,	��)
	MESSAGE_DEF(	CMD_MC_UPDATE_PASSWORD2,	CMD_MC_CHARBASE + 12,	��)
	MESSAGE_DEF(    CMD_MC_SYNCCHAR, CMD_MC_CHARBASE + 13, ��)
	MESSAGE_DEF(CMD_MC_GETORIGCOLOR, CMD_MC_CHARBASE + 14, GetOrigColor)
	MESSAGE_DEF(	CMD_MC_CHANGETITLE, CMD_MC_CHARBASE + 15, Changetitle)
	MESSAGE_DEF(CMD_MC_SYNGUILDCOLOR, CMD_MC_CHARBASE + 16, SynGuildColor)

//End ��ɫ�����͵�¼����

//Begin ����ϵͳָ��
	MESSAGE_DEF(	CMD_MC_TALKPAGE,			CMD_MC_ROLEBASE + 1,	��������ͻ��˷��ͶԻ���Ϣ)
	MESSAGE_DEF(	CMD_MC_FUNCPAGE,			CMD_MC_ROLEBASE + 2,	��������ͻ��˷��͹���ѡ��ҳ��Ϣ)
	MESSAGE_DEF(	CMD_MC_CLOSETALK,			CMD_MC_ROLEBASE + 3,	��������ͻ��˷��͹رնԻ���Ϣ)
	MESSAGE_DEF(	CMD_MC_HELPINFO,			CMD_MC_ROLEBASE + 4,	��������ͻ��˷��Ͱ�����Ϣ)
	MESSAGE_DEF(	CMD_MC_TRADEPAGE,			CMD_MC_ROLEBASE + 5,	��������ͻ��˷��ͽ�����Ϣ)
	MESSAGE_DEF(	CMD_MC_TRADERESULT,			CMD_MC_ROLEBASE + 6,	��������ͻ��˷��ͽ����������Ϣ)
	MESSAGE_DEF(	CMD_MC_TRADE_DATA,			CMD_MC_ROLEBASE + 7,	NPC����������Ϣ����)
	MESSAGE_DEF(	CMD_MC_TRADE_ALLDATA,		CMD_MC_ROLEBASE + 8,	NPC����������Ϣ����)
	MESSAGE_DEF(	CMD_MC_TRADE_UPDATE,		CMD_MC_ROLEBASE + 9,	�������˽�����Ϣ����)
	MESSAGE_DEF(	CMD_MC_EXCHANGE_UPDATE,		CMD_MC_ROLEBASE + 10,	�������˶һ���Ϣ����)

	//��ɫ����ָ��
	MESSAGE_DEF(	CMD_MC_CHARTRADE,			CMD_MC_ROLEBASE + 11,	��������ͻ��˷��ͽ�����Ϣ )
	MESSAGE_DEF(	CMD_MC_CHARTRADE_REQUEST,	CMD_MC_ROLEBASE + 12,	��������ͻ��˷���������ɫ���뽻����Ϣ)
	MESSAGE_DEF(	CMD_MC_CHARTRADE_CANCEL,	CMD_MC_ROLEBASE + 13,	��������ͻ��˷��� )
	MESSAGE_DEF(	CMD_MC_CHARTRADE_ITEM,		CMD_MC_ROLEBASE + 14,	��������ͻ��˽�ɫ���ͽ��׵���Ʒ��Ϣ)
	MESSAGE_DEF(	CMD_MC_CHARTRADE_PAGE,		CMD_MC_ROLEBASE + 15,	��������ͻ��˷��Ϳ�ʼ���׽�����Ϣ)
	MESSAGE_DEF(	CMD_MC_CHARTRADE_VALIDATEDATA,	CMD_MC_ROLEBASE + 16,	ȷ�Ͻ�������)
	MESSAGE_DEF(	CMD_MC_CHARTRADE_VALIDATE,	CMD_MC_ROLEBASE + 17,	ȷ�Ͻ���)
	MESSAGE_DEF(	CMD_MC_CHARTRADE_RESULT,	CMD_MC_ROLEBASE + 18,	ȷ�Ͻ��׳ɹ�)
	MESSAGE_DEF(	CMD_MC_CHARTRADE_MONEY,		CMD_MC_ROLEBASE + 19,	�϶���Ǯ����)
	MESSAGE_DEF(	CMD_MC_MISSION,				CMD_MC_ROLEBASE + 22,	��������ͻ��˷��������б�ҳ��Ϣ)
	MESSAGE_DEF(	CMD_MC_MISSIONLIST,			CMD_MC_ROLEBASE + 23,	�����б�)
	MESSAGE_DEF(	CMD_MC_MISSIONTALK,			CMD_MC_ROLEBASE + 24,	����Ի�)
	MESSAGE_DEF(	CMD_MC_NPCSTATECHG,			CMD_MC_ROLEBASE + 25,	�����л�npc״̬)
	MESSAGE_DEF(	CMD_MC_TRIGGER_ACTION,		CMD_MC_ROLEBASE + 26,	����������)
	MESSAGE_DEF(	CMD_MC_MISPAGE,				CMD_MC_ROLEBASE + 27,	����ҳ��Ϣ)
	MESSAGE_DEF(	CMD_MC_MISLOG,				CMD_MC_ROLEBASE + 28,	������־)
	MESSAGE_DEF(	CMD_MC_MISLOGINFO,			CMD_MC_ROLEBASE + 29,	����������ϸ��Ϣ)
	MESSAGE_DEF(	CMD_MC_MISLOG_CHANGE,		CMD_MC_ROLEBASE + 30,	����״̬�仯)
	MESSAGE_DEF(	CMD_MC_MISLOG_CLEAR,		CMD_MC_ROLEBASE + 31,	���һ������)
	MESSAGE_DEF(	CMD_MC_MISLOG_ADD,			CMD_MC_ROLEBASE + 32,	���һ������)

	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_FUSION,	CMD_MC_ROLEBASE + 33	,	��ʼ�ۺϵ���)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_UPGRADE,	CMD_MC_ROLEBASE + 34	,	��ʼ��������)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_FORGE,	CMD_MC_ROLEBASE + 35	,	��ʼ���߾���)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_UNITE,	CMD_MC_ROLEBASE + 36	,	��ʼ���ߺϳ�)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_MILLING,	CMD_MC_ROLEBASE + 37	,	��ʼ���ߴ�ĥ)

	MESSAGE_DEF(	CMD_MC_CREATEBOAT,			CMD_MC_ROLEBASE + 38,	������ֻ)
	MESSAGE_DEF(	CMD_MC_UPDATEBOAT,			CMD_MC_ROLEBASE + 39,	���촬ֻ)
	MESSAGE_DEF(	CMD_MC_UPDATEBOAT_PART,		CMD_MC_ROLEBASE + 40,	���촬ֻ�Ĳ���)
	MESSAGE_DEF(	CMD_MC_BERTH_LIST,			CMD_MC_ROLEBASE + 41,	��ֻͣ���б�)
	MESSAGE_DEF(	CMD_MC_BOAT_LIST,			CMD_MC_ROLEBASE + 42,	ͬ����ɫӵ�е����д�ֻ����)
	MESSAGE_DEF(	CMD_MC_BOAT_ADD,			CMD_MC_ROLEBASE + 43,	���һ�Ҵ�)
	MESSAGE_DEF(	CMD_MC_BOAT_CLEAR,			CMD_MC_ROLEBASE + 44,	���һ�Ҵ�)
	MESSAGE_DEF(	CMD_MC_BOATINFO,			CMD_MC_ROLEBASE + 45,	ͬ����ֻ������Ϣ)
	MESSAGE_DEF(	CMD_MC_BOAT_BAGLIST,		CMD_MC_ROLEBASE + 46,	��ʾ���Դ���Ĵ�ֻ�б�)

	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_EIDOLON_METEMPSYCHOSIS,	CMD_MC_ROLEBASE + 47,	��ʼ����ת��)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_EIDOLON_FUSION,	CMD_MC_ROLEBASE + 48,	��ʼ�����ں�)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_PURIFY,	CMD_MC_ROLEBASE + 49	,	��ʼװ���ᴿ)
	
	// �¼�ʵ�彻��Э������
	MESSAGE_DEF(	CMD_MC_ENTITY_BEGINESEE,	CMD_MC_ROLEBASE + 50,	ͬ����Ұʵ����Ϣ)
	MESSAGE_DEF(	CMD_MC_ENTITY_ENDSEE,		CMD_MC_ROLEBASE + 51,	ʵ����Ϣ������Ұ)
	MESSAGE_DEF(	CMD_MC_ENTITY_CHGSTATE,		CMD_MC_ROLEBASE + 52,	ʵ��״̬�ı�)

	// ��̯
	MESSAGE_DEF(	CMD_MC_STALL_ALLDATA,		CMD_MC_ROLEBASE + 54,	�鿴��̯������Ϣ)
	MESSAGE_DEF(	CMD_MC_STALL_UPDATE,		CMD_MC_ROLEBASE + 55,	��̯���������Ϣ)
	MESSAGE_DEF(	CMD_MC_STALL_DELGOODS,		CMD_MC_ROLEBASE + 56,	ɾ��ָ��λ�õĻ���)
	MESSAGE_DEF(	CMD_MC_STALL_CLOSE,			CMD_MC_ROLEBASE + 57,	̯����̯)
	MESSAGE_DEF(	CMD_MC_STALL_START,			CMD_MC_ROLEBASE + 58,	�ɹ���̯)
	MESSAGE_DEF(	CMD_MC_STALL_NAME,			CMD_MC_ROLEBASE + 59,	��̯��)

	// ����������Ϣ
	MESSAGE_DEF(	CMD_MC_BICKER_NOTICE,		CMD_MC_ROLEBASE + 60,	����������Ϣ֪ͨ)

	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_ENERGY,	CMD_MC_ROLEBASE + 71	,	��ʼ���ǳ��)
	MESSAGE_DEF(	CMD_MC_BEGIN_GET_STONE,		CMD_MC_ROLEBASE + 72	,	��ʼ��ȡ��ʯ)
	MESSAGE_DEF(	CMD_MC_BEGIN_TIGER,			CMD_MC_ROLEBASE + 73	,	�����ϻ�������)
	MESSAGE_DEF(	CMD_MC_TIGER_ITEM_ID,		CMD_MC_ROLEBASE + 74	,	�ϻ�������ID)
	MESSAGE_DEF(	CMD_MC_TIGER_STOP,			CMD_MC_ROLEBASE + 75	,	�ϻ�������)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_FIX,		CMD_MC_ROLEBASE + 76	,	�����ƹ�)
	MESSAGE_DEF(	CMD_MC_BEGIN_GM_SEND,		CMD_MC_ROLEBASE + 77	,	����GM�ʼ�)
	MESSAGE_DEF(	CMD_MC_BEGIN_GM_RECV,		CMD_MC_ROLEBASE + 78	,	����GM�ʼ�)

	// Add by lark.li 20080514 begin
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_LOTTERY,	CMD_MC_ROLEBASE + 79	,	��ʼ���ò�Ʊ)
	// End
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_AMPHITHEATER,	CMD_MC_ROLEBASE + 80,	�򿪾��������а�)
	//Add by sunny.sun 20090306 begin
	MESSAGE_DEF(	CMD_MC_LEAVE_TIME_ASR,		CMD_MC_ROLEBASE + 81,	��ɫ����ʱ��)
	MESSAGE_DEF(	CMD_MC_SELECT_TIME_ASR,		CMD_MC_ROLEBASE + 82,	��ȡ���߾�����Ϣ�ظ�)
	MESSAGE_DEF(	CMD_MC_BEHINDGENIUS_ASR,	CMD_MC_ROLEBASE + 83	,	����������Ӧ��)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_JEWELRYUP,CMD_MC_ROLEBASE + 84	,	������ʯ��������)
	//Add by sunny.sun 20090414
	MESSAGE_DEF(	CMD_MC_OPEN_CRYSTALTRADE_ASR,CMD_MC_ROLEBASE + 85	,	��ˮ��ƽ̨Ӧ��)
	MESSAGE_DEF(	CMD_MC_REQUEST_BUY_ASR,		CMD_MC_ROLEBASE + 86 ,	����ҵ�Ӧ��)
	MESSAGE_DEF(	CMD_MC_REQUEST_SALE_ASR,	CMD_MC_ROLEBASE + 87 ,	�����ҵ�Ӧ��)
	MESSAGE_DEF(	CMD_MC_REQUEST_CANCEL_ASR,	CMD_MC_ROLEBASE + 88 ,	ȡ���ҵ�Ӧ��)
	MESSAGE_DEF(	CMD_MC_REQUEST_GETFLATMONEY,CMD_MC_ROLEBASE + 89 ,	ȡ��ƽ̨�ϵ�ǮӦ��)
	MESSAGE_DEF(	CMD_MC_REQUEST_REFRESH_ASR,	CMD_MC_ROLEBASE + 90 ,	����ˢ��Ӧ��)
	MESSAGE_DEF(	CMD_MC_GET_CRYSTALSTATE,	CMD_MC_ROLEBASE + 91 ,	��ȡ����״̬)
	//End

//�����
MESSAGE_DEF(	CMD_MC_LIFESKILL_BGING,			CMD_MC_BASE + 100	,	����ܿ�ʼ)
MESSAGE_DEF(	CMD_MC_LIFESKILL_ASR,			CMD_MC_BASE + 101	,	��������ˣ��ϳɣ��ֽ�ȵ�����)
MESSAGE_DEF(	CMD_MC_LIFESKILL_ASK,			CMD_MC_BASE + 102	,	��������ˣ��ϳɣ��ֽ�ȵ�ȷ��)

// Add by lark.li 20080515 begin
MESSAGE_DEF(	CMD_MC_ITEM_LOTTERY_ASK,		CMD_MC_BASE + 103,	��Ʊ����)
MESSAGE_DEF(	CMD_MC_ITEM_LOTTERY_ASR,		CMD_MC_BASE + 104,	��ƱӦ��)
// End

// Add by lark.li 20080707 begin
MESSAGE_DEF(	CMD_MC_CAPTAIN_ASK,				CMD_MC_BASE + 105,	�������ӳ�ȷ������)
// End

MESSAGE_DEF(	CMD_MC_ITEM_AMPHITHEATER_ASR,	CMD_MC_BASE + 106, ��)
MESSAGE_DEF(	CMD_MC_REQUEST_JEWELRYUP_ASR,	CMD_MC_BASE + 107, ��)

MESSAGE_DEF(	XTRAP_CMD_STEP_ONE,				CMD_MC_BASE + 108, ��)

//End ����ϵͳָ��

/*=====================================================================================================
*					Ga(t)eServer <->Ga(m)eServer
*/
MESSAGE_DEF(	CMD_TM_ENTERMAP,				CMD_TM_BASE	+ 3	,	����ͼ��������ɫID�������ͼ�����֡�λ��(x,y))
MESSAGE_DEF(	CMD_TM_GOOUTMAP,				CMD_TM_BASE + 4		,	����ͼ)
MESSAGE_DEF(	CMD_TM_LOGIN_ACK,				CMD_TM_BASE + 5, ��)
MESSAGE_DEF(	CMD_TM_CHANGE_PERSONINFO,		CMD_TM_BASE	+6	,	�޸ĸ�����Ϣ)
MESSAGE_DEF(	CMD_TM_MAPENTRY,				CMD_TM_BASE + 7, ��)
MESSAGE_DEF(	CMD_TM_MAPENTRY_NOMAP,			CMD_TM_BASE + 8, ��)
MESSAGE_DEF(	CMD_TM_MAP_ADMIN,				CMD_TM_BASE + 9	,	����Ա�õ�ͼ��������)
MESSAGE_DEF(	CMD_TM_STATE,					CMD_TM_BASE + 10,	��������״̬�������ڴ�������汾��)
//---------------------------------------------------

MESSAGE_DEF(	CMD_MT_LOGIN,					CMD_MT_BASE + 1		,	GameServer��½GateServer)
MESSAGE_DEF(	CMD_MT_SWITCHMAP,				CMD_MT_BASE + 4		,	��ʼ�л���ͼ����)
MESSAGE_DEF(	CMD_MT_KICKUSER,				CMD_MT_BASE + 5		,	GameServer�ߵ�ĳ���û�)
MESSAGE_DEF(	CMD_MT_MAPENTRY,				CMD_MT_BASE + 6		,	��ͼ���)
MESSAGE_DEF(	CMD_MT_PALYEREXIT,				CMD_MT_BASE + 8		,	֪ͨGate�ͷŸ��û���Դ)
MESSAGE_DEF(	CMD_MT_MAP_ADMIN,				CMD_MT_BASE + 9	,	����Ա�õ�ͼ��������)
MESSAGE_DEF(	CMD_MT_STATE,					CMD_MT_BASE + 10,	��������״̬�������ڴ�������汾��)

/*=====================================================================================================
*					Grou(p)eServer <->Ga(m)eServer 
*/
MESSAGE_DEF(	CMD_PM_TEAM,					CMD_PM_BASE + 1    ,	GroupServer�����������Ϣ����)
//CMD_PM_TEAM�����Ͷ���Begin
	MESSAGE_DEF(	TEAM_MSG_ADD,				1                  ,	������: �����¶�Ա)
	MESSAGE_DEF(	TEAM_MSG_LEAVE,				2                  ,	������: ��Ա�뿪)
	MESSAGE_DEF(	TEAM_MSG_UPDATE,			3                  ,	������: ˢ�¶��� ֻ�÷���������תʱ��ˢ�µ�GameServerʹ��)	
	MESSAGE_DEF(	TEAM_MSG_GROUP_ADD,			4                  ,	��������)
	MESSAGE_DEF(	TEAM_MSG_GROUP_BREAK,		5                  ,	��ɢ����)
	MESSAGE_DEF(	TEAM_MSG_KICK,				6				,	�߳�����ĳ����Ա)
MESSAGE_DEF(	CMD_PM_GUILD_DISBAND,			CMD_PM_BASE	+ 2		,	��ɢ����)
//CMD_PM_TEAM�����Ͷ���End

MESSAGE_DEF(	CMD_PM_GUILDINFO,				CMD_PM_BASE + 4	,	GroupServer�����Ĺ���ͬ����Ϣ)
MESSAGE_DEF(	CMD_PM_GUILD_CHALLMONEY,		CMD_PM_BASE + 5    ,	������սͶ���Ǯ�˸��᳤)
MESSAGE_DEF(	CMD_PM_GUILD_CHALL_PRIZEMONEY,	CMD_PM_BASE + 6    ,	������սս������Ǯ���᳤)
//Begin�Ҷ�����������ϵͳ
MESSAGE_DEF(	CMD_PM_GARNER2_UPDATE,			CMD_PM_BASE +7, ��)
//End�Ҷ�����������ϵͳ
MESSAGE_DEF(	CMD_PM_TEAM_CREATE,				CMD_PM_BASE + 8, ��)
MESSAGE_DEF(	CMD_PM_SAY2ALL,					CMD_PM_BASE + 9, ��)
MESSAGE_DEF(	CMD_PM_SAY2TRADE,				CMD_PM_BASE + 10, ��)
MESSAGE_DEF(	CMD_PM_EXPSCALE,				CMD_PM_BASE + 11       ,	 ������ϵͳ  Ӱ����Ҿ��鼰������)
MESSAGE_DEF(	CMD_PM_GUILD_INVITE,			CMD_PM_BASE + 12		,	��������)

// FEATURE: GUILD_BANK
MESSAGE_DEF(CMD_PM_GUILDBANK, CMD_PM_BASE + 13, Guild Bank)

//---------------------------------------------------
MESSAGE_DEF(	CMD_MP_ENTERMAP,				CMD_MP_BASE	+ 1, ��)
MESSAGE_DEF(	CMD_MP_GUILD_APPROVE,			CMD_MP_BASE	+ 2, ��)
MESSAGE_DEF(	CMD_MP_GUILD_CREATE,			CMD_MP_BASE	+ 3, ��)
MESSAGE_DEF(	CMD_MP_GUILD_KICK,				CMD_MP_BASE	+ 4, ��)
MESSAGE_DEF(	CMD_MP_GUILD_LEAVE,				CMD_MP_BASE	+ 5, ��)
MESSAGE_DEF(	CMD_MP_GUILD_DISBAND,			CMD_MP_BASE	+ 6, ��)
MESSAGE_DEF(	CMD_MP_GUILD_MOTTO,				CMD_MP_BASE	+ 10, ��)
MESSAGE_DEF(	CMD_MP_GUILD_CHALLMONEY,		CMD_MP_BASE + 13			,	��Ͷ�깫���Ǯ�˻��᳤��ɫ)
MESSAGE_DEF(	CMD_MP_GUILD_CHALL_PRIZEMONEY,	CMD_MP_BASE + 14    ,	������ս�����ĵ�Ǯ���᳤��ɫ)

//Begin�Ҷ�����������ϵͳ
MESSAGE_DEF(	CMD_MP_GARNER2_UPDATE,			CMD_MP_BASE +7, ��)
MESSAGE_DEF(	CMD_MP_GARNER2_CGETORDER,		CMD_MP_BASE+15, ��)
//End�Ҷ�����������ϵͳ

MESSAGE_DEF(	CMD_MP_TEAM_CREATE,				CMD_MP_BASE + 16, ��)

MESSAGE_DEF(	CMD_MP_MASTER_CREATE,			CMD_MP_BASE + 17, ��)
MESSAGE_DEF(	CMD_MP_MASTER_DEL,				CMD_MP_BASE + 18, ��)
MESSAGE_DEF(	CMD_MP_MASTER_FINISH,			CMD_MP_BASE + 19, ��)
MESSAGE_DEF(	CMD_MP_SAY2ALL,					CMD_MP_BASE + 20, ��)
MESSAGE_DEF(	CMD_MP_SAY2TRADE,				CMD_MP_BASE + 21, ��)
MESSAGE_DEF(	CMD_MP_GM1SAY1,					CMD_MP_BASE + 22, ��)
MESSAGE_DEF(	CMD_MP_GM1SAY,					CMD_MP_BASE + 23, ��)
MESSAGE_DEF(	CMD_MP_GMBANACCOUNT,			CMD_MP_BASE	+ 24, GM��ͣˢ���˺�)

// FEATURE: GUILD_BANK
MESSAGE_DEF(CMD_MP_GUILDBANK, CMD_MP_BASE + 25, Guild Bank)
MESSAGE_DEF(CMD_MP_GUILDADDEXP, CMD_MP_BASE + 26, Guild Attr)
MESSAGE_DEF(CMD_MP_GUILD_PERM, CMD_MP_BASE + 27, Guild Perm)
MESSAGE_DEF(CMD_MP_GUILDNAMECOLOR, CMD_MP_BASE + 28, Guild Color)
/*=====================================================================================================
*					Ga(t)eServer <->Grou(p)Server 
*/
MESSAGE_DEF(	CMD_TP_LOGIN,					CMD_TP_BASE + 1		,	GateServer��GroupServer���̼��½)
MESSAGE_DEF(	CMD_TP_LOGOUT,					CMD_TP_BASE + 2		,	GateServer��GroupServer���̼�ǳ�)
MESSAGE_DEF(	CMD_TP_USER_LOGIN,				CMD_TP_BASE	+ 3		,	�û�����)
MESSAGE_DEF(	CMD_TP_USER_LOGOUT,				CMD_TP_BASE + 4		,	�û��ǳ�)
MESSAGE_DEF(	CMD_TP_BGNPLAY,					CMD_TP_BASE	+ 5		,	�û�ѡ���ɫ��ʼ����Ϸ)
MESSAGE_DEF(	CMD_TP_ENDPLAY,					CMD_TP_BASE	+ 6		,	�û���������Ϸ����ѡ/��/ɾ��ɫ����)
MESSAGE_DEF(	CMD_TP_NEWCHA,					CMD_TP_BASE	+ 7		,	�½���ɫ)
MESSAGE_DEF(	CMD_TP_DELCHA,					CMD_TP_BASE	+ 8		,	ɾ����ɫ)
MESSAGE_DEF(	CMD_TP_PLAYEREXIT,				CMD_TP_BASE + 9		,	�˳���ɫ�ͷ���Դ)
MESSAGE_DEF(	CMD_TP_REQPLYLST,				CMD_TP_BASE	+ 10	,	����GateServer�ϵ���ҵ�ַ�б�)
MESSAGE_DEF(	CMD_TP_DISC,					CMD_TP_BASE + 11, ��)
MESSAGE_DEF(	CMD_TP_ESTOPUSER_CHECK,			CMD_TP_BASE + 12, ��)
MESSAGE_DEF(	CMD_TP_CREATE_PASSWORD2,		CMD_TP_BASE + 13,	������������)
MESSAGE_DEF(	CMD_TP_UPDATE_PASSWORD2,		CMD_TP_BASE + 14,	���¶�������)

// Add by lark.li 20081119 begin
MESSAGE_DEF(	CMD_TP_SYNC_PLYLST,				CMD_TP_BASE	+ 15	,	���ͱ�GateServer�ϵ���ҵ�ַ�б�)

// End

//---------------------------------------------------
MESSAGE_DEF(	CMD_PT_KICKUSER,				CMD_PT_BASE	+ 11, ��)
MESSAGE_DEF(	CMD_PT_ESTOPUSER,				CMD_PT_BASE + 12,	���Խ�ɫ)
MESSAGE_DEF(	CMD_PT_DEL_ESTOPUSER,			CMD_PT_BASE + 13,	�������)

/*=====================================================================================================
*					Grou(p)Server <-> (A)ccountServer
*/
MESSAGE_DEF(	CMD_PA_LOGIN,					CMD_PA_BASE + 2, ��)
MESSAGE_DEF(	CMD_PA_USER_LOGIN,				CMD_PA_BASE + 4, ��)
MESSAGE_DEF(	CMD_PA_USER_LOGOUT,				CMD_PA_BASE + 5, ��)
MESSAGE_DEF(	CMD_PA_USER_DISABLE,			CMD_PA_BASE + 6, ��)
MESSAGE_DEF(	CMD_PA_USER_LOGIN1,				CMD_PA_BASE + 13, ��)
MESSAGE_DEF(	CMD_PA_USER_LOGIN2,				CMD_PA_BASE + 14, ��)
MESSAGE_DEF(	CMD_PA_USER_BILLBGN,			CMD_PA_BASE + 20, ��)
MESSAGE_DEF(	CMD_PA_USER_BILLEND,			CMD_PA_BASE + 21, ��)
MESSAGE_DEF(	CMD_PA_GROUP_BILLEND_AND_LOGOUT,CMD_PA_BASE + 22, ��)
MESSAGE_DEF(	CMD_PA_LOGOUT,					CMD_PA_BASE + 23, ��)
MESSAGE_DEF(	CMD_PA_GMBANACCOUNT,			CMD_PA_BASE + 24, GM��ͣ�˺�)//Add by sunny.sun 20090828
//---------------------------------------------------
MESSAGE_DEF(	CMD_AP_LOGIN,					CMD_AP_BASE + 2, ��)
MESSAGE_DEF(	CMD_AP_USER_LOGIN,				CMD_AP_BASE + 3, ��)
MESSAGE_DEF(	CMD_AP_RELOGIN,					CMD_AP_BASE + 4, ��)
MESSAGE_DEF(	CMD_AP_KICKUSER,				CMD_AP_BASE	+ 11, ��)
MESSAGE_DEF(	CMD_AP_USER_LOGIN1,				CMD_AP_BASE + 12, ��)
MESSAGE_DEF(	CMD_AP_USER_LOGIN2,				CMD_AP_BASE + 13, ��)
MESSAGE_DEF(	CMD_AP_EXPSCALE,				CMD_AP_BASE + 14, ������)
/*=====================================================================================================
*					Ga(m)eServer <-> Ga(m)eServer
*
*/
MESSAGE_DEF(	CMD_MM_GATE_RELEASE,			CMD_MM_BASE + 1, ��)
MESSAGE_DEF(	CMD_MM_GATE_CONNECT,			CMD_MM_BASE + 2, ��)
MESSAGE_DEF(	CMD_MM_QUERY_CHA,				CMD_MM_BASE + 3	,	��ѯ��ɫ������Ϣ)
MESSAGE_DEF(	CMD_MM_QUERY_CHAITEM,			CMD_MM_BASE + 4	,	��ѯ��ɫ������Ϣ)
MESSAGE_DEF(	CMD_MM_CALL_CHA,				CMD_MM_BASE + 5	,	����ɫ�ٻ����Լ����)
MESSAGE_DEF(	CMD_MM_GOTO_CHA,				CMD_MM_BASE + 6	,	���Լ����͵���ɫ���)
MESSAGE_DEF(	CMD_MM_KICK_CHA,				CMD_MM_BASE + 7	,	����ɫ������)
MESSAGE_DEF(	CMD_MM_GUILD_REJECT,			CMD_MM_BASE + 8	,	�ܾ����빫������֪ͨ����)
MESSAGE_DEF(	CMD_MM_GUILD_APPROVE,			CMD_MM_BASE + 9	,	�ܾ����빫������֪ͨ����)
MESSAGE_DEF(	CMD_MM_GUILD_KICK,				CMD_MM_BASE	+ 10,	�ӹ��������޳�ĳ��)
MESSAGE_DEF(	CMD_MM_GUILD_DISBAND,			CMD_MM_BASE	+ 11,	��ɢ����)
MESSAGE_DEF(	CMD_MM_QUERY_CHAPING,			CMD_MM_BASE + 12,	��ѯ��ɫ��GameServer�߼����pingֵ)
MESSAGE_DEF(	CMD_MM_NOTICE,					CMD_MM_BASE + 13,	ϵͳͨ��)
MESSAGE_DEF(	CMD_MM_GUILD_MOTTO,				CMD_MM_BASE + 14,	���¹���������)
MESSAGE_DEF(	CMD_MM_DO_STRING,				CMD_MM_BASE + 15,	ִ�нű�)
MESSAGE_DEF(	CMD_MM_CHA_NOTICE,				CMD_MM_BASE + 16,	��ָ����ҵ�ϵͳͨ��)
MESSAGE_DEF(	CMD_MM_LOGIN,					CMD_MM_BASE + 17,	��ҵ�½��Ϸ)
MESSAGE_DEF(	CMD_MM_GUILD_CHALL_PRIZEMONEY,	CMD_MM_BASE + 18,	�˻�����ս�ķ���)
MESSAGE_DEF(	CMD_MM_ADDCREDIT,				CMD_MM_BASE + 19,	��������)
MESSAGE_DEF(	CMD_MM_STORE_BUY,				CMD_MM_BASE + 20,	�����̳���Ʒ)
MESSAGE_DEF(	CMD_MM_ADDMONEY,				CMD_MM_BASE + 21, ��)
MESSAGE_DEF(	CMD_MM_AUCTION,					CMD_MM_BASE + 22, ��)
MESSAGE_DEF(	CMD_MM_NOTICETOCHA,				CMD_MM_BASE + 23,	ˮ������֪ͨ����ͬһ�������Ľ�����)
MESSAGE_DEF(	CMD_MM_EXCHEXTERNVALUE,			CMD_MM_BASE + 24,	�ű��ı�ȫ�ֱ���)
MESSAGE_DEF(	CMD_MM_NOTICETRADER,			CMD_MM_BASE + 25,	֪ͨ������)

// FEATURE: GUILD_BANK
MESSAGE_DEF(CMD_MM_UPDATEGUILDBANK, CMD_MM_BASE + 26, Update Guild Bank)
MESSAGE_DEF(CMD_MM_UPDATEGUILDBANKGOLD, CMD_MM_BASE + 27, Update Guild Bank Gold)

/*=====================================================================================================
*					Grou(p)Server <-> (C)lient
*/
//��������BEGIN
	MESSAGE_DEF(	CMD_CP_TEAM_INVITE,			CMD_CP_BASE	+ 1		,	�������)
	MESSAGE_DEF(	CMD_CP_TEAM_ACCEPT,			CMD_CP_BASE	+ 2		,	ȷ�����)
	MESSAGE_DEF(	CMD_CP_TEAM_REFUSE,			CMD_CP_BASE	+ 3		,	�ܾ����)
	MESSAGE_DEF(	CMD_CP_TEAM_LEAVE,			CMD_CP_BASE	+ 4		,	�뿪���)
	MESSAGE_DEF(	CMD_CP_TEAM_KICK,			CMD_CP_BASE + 5		,	�߳����)
//��������END
//���������BEGIN
	MESSAGE_DEF(	CMD_CP_FRND_INVITE,			CMD_CP_BASE	+ 11	,	��������)
	MESSAGE_DEF(	CMD_CP_FRND_ACCEPT,			CMD_CP_BASE	+ 12	,	���ѽ���)
	MESSAGE_DEF(	CMD_CP_FRND_REFUSE,			CMD_CP_BASE	+ 13	,	���Ѿܾ�)
	MESSAGE_DEF(	CMD_CP_FRND_DELETE,			CMD_CP_BASE	+ 14	,	����ɾ��)
	MESSAGE_DEF(	CMD_CP_FRND_MOVE_GROUP,		CMD_CP_BASE	+ 15	,	�ı������������)
	MESSAGE_DEF(	CMD_CP_FRND_REFRESH_INFO,	CMD_CP_BASE	+ 16	,	���Լ��Ŀͻ���ˢ�±��ˣ����ѡ����ѡ�ͬ�š�·�ˡ�İ���ˣ��ĸ�����Ϣ)
	MESSAGE_DEF(	CMD_CP_CHANGE_PERSONINFO,	CMD_CP_BASE	+ 17	,	�ı�����Լ���ͷ��ȸ�����Ϣ)

// Add by lark.li 20080804 begin
	MESSAGE_DEF(	CMD_CP_FRND_DEL_GROUP,		CMD_CP_BASE	+ 18, ��)
	MESSAGE_DEF(	CMD_CP_FRND_ADD_GROUP,		CMD_CP_BASE	+ 19, ��)
	MESSAGE_DEF(	CMD_CP_FRND_CHANGE_GROUP,	CMD_CP_BASE	+ 20, ��)
// End

// Add by lark.li 20080808 begin
	MESSAGE_DEF(	CMD_CP_QUERY_PERSONINFO,	CMD_CP_BASE	+ 21, ��)
// End

//���������END

// Modify by lark.li 20080806 begin
//MESSAGE_DEF(	CMD_CP_PING				CMD_CP_BASE	+ 20
MESSAGE_DEF(	CMD_CP_PING,					CMD_CP_BASE	+ 25, ��)

// ���
//MESSAGE_DEF(	CMD_CP_REPORT_WG		CMD_CP_BASE + 21,	���淢��ʹ�����
MESSAGE_DEF(	CMD_CP_REPORT_WG,				CMD_CP_BASE + 26,	���淢��ʹ�����)
// End

//��ʦ�����BEGIN
MESSAGE_DEF(	CMD_CP_MASTER_REFRESH_INFO,		CMD_CP_BASE	+ 31	,	���Լ��Ŀͻ���ˢ��ʦ���ĸ�����Ϣ)
MESSAGE_DEF(	CMD_CP_PRENTICE_REFRESH_INFO,	CMD_CP_BASE	+ 32	,	���Լ��Ŀͻ���ˢ��ͽ�ܵĸ�����Ϣ)
//��ʦ�����END

//Add by sunny.sun 20090605 for Guild Invite 
MESSAGE_DEF(	CMD_CP_GUILD_INVITE,			CMD_CP_BASE + 33, ��)
MESSAGE_DEF(	CMD_CP_GUILD_ACCEPT,			CMD_CP_BASE + 34, ��)
MESSAGE_DEF(	CMD_CP_GUILD_REFUSE,			CMD_CP_BASE + 35, ��)


//���������Begin

//���������End
//Begin ����ָ��
	MESSAGE_DEF(	CMD_CP_GM1SAY,				CMD_CP_BASE	+ 400		,	GM��ȫ���㲥)
	MESSAGE_DEF(	CMD_CP_SAY2TRADE,			CMD_CP_BASE	+ 401		,	����Ƶ��)
	MESSAGE_DEF(	CMD_CP_SAY2ALL,				CMD_CP_BASE + 402		,	������Ϸ����㲥)
	MESSAGE_DEF(	CMD_CP_SAY2YOU,				CMD_CP_BASE	+ 403		,	P2P����(��Ҷ����))
	MESSAGE_DEF(	CMD_CP_SAY2TEM,				CMD_CP_BASE	+ 404		,	�������)
	MESSAGE_DEF(	CMD_CP_SAY2GUD,				CMD_CP_BASE	+ 405		,	�л�(Guild)����)

	MESSAGE_DEF(	CMD_CP_SESS_CREATE,			CMD_CP_BASE	+ 406		,	�Ự����)
	MESSAGE_DEF(	CMD_CP_SESS_SAY,			CMD_CP_BASE	+ 407		,	�Ự˵��)
	MESSAGE_DEF(	CMD_CP_SESS_ADD,			CMD_CP_BASE	+ 408		,	�Ự�����Ա)
	MESSAGE_DEF(	CMD_CP_SESS_LEAVE,			CMD_CP_BASE	+ 409		,	�뿪�Ự)
	MESSAGE_DEF(	CMD_CP_REFUSETOME,			CMD_CP_BASE	+ 410		,	�򿪽���˽�Ŀ���)
	MESSAGE_DEF(	CMD_CP_GM1SAY1,				CMD_CP_BASE	+ 411		,	�����㲥)
	MESSAGE_DEF(	CMD_CP_GMSAY,				CMD_CP_BASE	+ 412		,	��������ִ��)

	// FEATURE: GUILD_BANK
	MESSAGE_DEF(CMD_CP_GUILDBANK, CMD_CP_BASE + 414, Guild Bank)

	// GUILD_ATTR
	MESSAGE_DEF(CMD_CP_GUILDATTR, CMD_CP_BASE + 416, Guild Attr)
	MESSAGE_DEF(CMD_CP_GUILDNAMECOLOR, CMD_CP_BASE + 417, Guild Color)
//End ����ָ��
//Begin�Ҷ�����������ϵͳ
//End�Ҷ�����������ϵͳ
//---------------------------------------------------
//��������BEGIN
	MESSAGE_DEF(	CMD_PC_TEAM_INVITE,			CMD_PC_BASE	+ 1			,	֪ͨ�ͻ��������������)
	MESSAGE_DEF(	CMD_PC_TEAM_REFRESH,		CMD_PC_BASE	+ 2			,	֪ͨ�ͻ�����ӱ仯����Ϣ��ʹ��CMD_PM_TEAM�����������)
	MESSAGE_DEF(	CMD_PC_TEAM_CANCEL,			CMD_PC_BASE	+ 3			,	��������Ϊ��ʱ���Ա��������ԭ��֪ͨ�����뷽ȡ����������)
	//CMD_PC_TEAM_CANCEL�����Ͷ���Begin
		MESSAGE_DEF(	MSG_TEAM_CANCLE_BUSY,	1	,	�Լ���������з�æ)
		MESSAGE_DEF(	MSG_TEAM_CANCLE_TIMEOUT,2	,	�������볬ʱ)
		MESSAGE_DEF(	MSG_TEAM_CANCLE_OFFLINE,3	,	���뷽������)
		MESSAGE_DEF(	MSG_TEAM_CANCLE_ISFULL,	4	,	���뷽�����Ա������)
		MESSAGE_DEF(	MSG_TEAM_CANCLE_CANCEL,	5	,	�����뷽������/��������ӻ����뷽�����˱��˴�������Ӷ�ȡ���˱�������)
	//CMD_PC_TEAM_CANCEL�����Ͷ���End
//��������END
//���������BEGIN
	MESSAGE_DEF(	CMD_PC_FRND_INVITE,			CMD_PC_BASE	+ 11, ��)
	MESSAGE_DEF(	CMD_PC_FRND_REFRESH,		CMD_PC_BASE	+ 12, ��)
	//CMD_PC_FRND_REFRESH�����Ͷ���Begin
		MESSAGE_DEF(	MSG_FRND_REFRESH_START,	1  ,	������: ������ʱ����ĺ����б�)
		MESSAGE_DEF(	MSG_FRND_REFRESH_ADD,	2  ,	������: ��������)
		MESSAGE_DEF(	MSG_FRND_REFRESH_DEL,	3  ,	������: ����ɾ��)
		MESSAGE_DEF(	MSG_FRND_REFRESH_ONLINE,4,	������: ��������)
		MESSAGE_DEF(	MSG_FRND_REFRESH_OFFLINE,5  ,	������: ��������)
	//CMD_PC_FRND_REFRESH�����Ͷ���End
	MESSAGE_DEF(	CMD_PC_FRND_CANCEL,			CMD_PC_BASE	+ 13, ��)
	//CMD_PC_FRND_CANCEL�����Ͷ���Begin
		MESSAGE_DEF(	MSG_FRND_CANCLE_BUSY,	1	,	�Լ���������з�æ)
		MESSAGE_DEF(	MSG_FRND_CANCLE_TIMEOUT,2	,	�������볬ʱ)
		MESSAGE_DEF(	MSG_FRND_CANCLE_OFFLINE,3	,	���뷽������)
		MESSAGE_DEF(	MSG_FRND_CANCLE_INVITER_ISFULL,	4	,	���뷽�ĺ��ѳ�Ա������)
		MESSAGE_DEF(	MSG_FRND_CANCLE_SELF_ISFULL,	5	,	�Լ��ĺ���������)
		MESSAGE_DEF(	MSG_FRND_CANCLE_CANCEL,	6	,	���뷽ȡ���˱�������)
	//CMD_PC_FRND_CANCEL�����Ͷ���End
	MESSAGE_DEF(	CMD_PC_FRND_MOVE_GROUP,		CMD_PC_BASE + 15	,	�ı���ѷ���ɹ�)
	MESSAGE_DEF(	CMD_PC_FRND_REFRESH_INFO,	CMD_PC_BASE	+ 16	,	ˢ�º��Ѹ�����Ϣ)
	MESSAGE_DEF(	CMD_PC_CHANGE_PERSONINFO,	CMD_PC_BASE	+ 17	,	�ı�����Լ���ͷ��ȸ�����Ϣ)

// Add by lark.li 20080804 begin
	MESSAGE_DEF(	CMD_PC_FRND_DEL_GROUP,		CMD_PC_BASE + 18	,	ɾ�����ѷ���ɹ�)
	MESSAGE_DEF(	CMD_PC_FRND_ADD_GROUP,		CMD_PC_BASE + 19	,	�������ѷ���ɹ�)
	MESSAGE_DEF(	CMD_PC_FRND_CHANGE_GROUP,	CMD_PC_BASE + 20	,	�ı���ѷ���ɹ�)
// End

// Add by lark.li 20080808 begin
MESSAGE_DEF(	CMD_PC_QUERY_PERSONINFO,		CMD_PC_BASE	+ 21	,	�ı�����Լ���ͷ��ȸ�����Ϣ)
// End
	//
//���������END

// Modify by lark.li 20080806
//MESSAGE_DEF(	CMD_PC_PING				CMD_PC_BASE	+ 20
MESSAGE_DEF(	CMD_PC_PING,					CMD_PC_BASE	+ 25, ��)
// End

//���������Begin
	MESSAGE_DEF(	CMD_PC_GUILD,				CMD_PC_BASE	+ 30, ��)
	MESSAGE_DEF(	CMD_PC_GUILD_PERM,			CMD_PC_BASE + 31, GuildPerm)
	MESSAGE_DEF(CMD_PC_GUILDNAMECOLOR, CMD_MP_BASE + 32, Guild Color)

	//CMD_PC_GUILD�����Ͷ���Begin
		MESSAGE_DEF(	MSG_GUILD_START,	1, ��)
		MESSAGE_DEF(	MSG_GUILD_ADD,		2, ��)
		MESSAGE_DEF(	MSG_GUILD_DEL,		3, ��)
		MESSAGE_DEF(	MSG_GUILD_ONLINE,	4, ��)
		MESSAGE_DEF(	MSG_GUILD_OFFLINE,	5, ��)
		MESSAGE_DEF(	MSG_GUILD_STOP,		6, ��)
	//CMD_PC_GUILD�����Ͷ���End
//���������End

	MESSAGE_DEF(	CMD_PC_GUILD_INVITE,		CMD_PC_BASE	+ 31, ��)
	//MESSAGE_DEF(	CMD_PC_GUILD_REFRESH		CMD_PC_BASE	+ 32
//Add by sunny.sun 20090608
MESSAGE_DEF(	CMD_PC_GUILD_CANCEL,		CMD_PC_BASE	+ 33, ��)
	MESSAGE_DEF(	MSG_GUILD_CANCLE_BUSY,			1	,	�Լ���������з�æ)
	MESSAGE_DEF(	MSG_GUILD_CANCLE_TIMEOUT,		2	,	�������볬ʱ)
	MESSAGE_DEF(	MSG_GUILD_CANCLE_OFFLINE,		3	,	�Է�������)
	MESSAGE_DEF(	MSG_GUILD_CANCLE_SELF_ISFULL,	4	,	�Է��Ĺ�����Ա������)
	MESSAGE_DEF(	MSG_GUILD_CANCLE_CANCEL,		5	,	�Է�ȡ���˱��ι�������)
	MESSAGE_DEF(	MSG_GUILD_CANCLE_SELFCANCEL,	6	,	�Լ�ȡ��������)
//End
//��ʦ�����BEGIN
MESSAGE_DEF(	CMD_PC_MASTER_REFRESH,	CMD_PC_BASE	+ 41, ��)
	//CMD_PC_MASTER_REFRESH�����Ͷ���Begin
	MESSAGE_DEF(	MSG_MASTER_REFRESH_START,		1  ,	������: ������ʱ�����ʦ���б�)
	MESSAGE_DEF(	MSG_MASTER_REFRESH_ADD,			2  ,	������: ʦ������)
	MESSAGE_DEF(	MSG_MASTER_REFRESH_DEL,			3  ,	������: ʦ��ɾ��)
	MESSAGE_DEF(	MSG_MASTER_REFRESH_ONLINE,		4	 ,	������: ʦ������)
	MESSAGE_DEF(	MSG_MASTER_REFRESH_OFFLINE,		5  ,	������: ʦ������)
	MESSAGE_DEF(	MSG_PRENTICE_REFRESH_START,		6  ,	������: ������ʱ�����ͽ���б�)
	MESSAGE_DEF(	MSG_PRENTICE_REFRESH_ADD,		7  ,	������: ͽ������)
	MESSAGE_DEF(	MSG_PRENTICE_REFRESH_DEL,		8  ,	������: ͽ��ɾ��)
	MESSAGE_DEF(	MSG_PRENTICE_REFRESH_ONLINE,	9  ,	������: ͽ������)
	MESSAGE_DEF(	MSG_PRENTICE_REFRESH_OFFLINE,	10  ,	������: ͽ������)
	//CMD_PC_MASTER_REFRESH�����Ͷ���End
MESSAGE_DEF(	CMD_PC_MASTER_CANCEL,	CMD_PC_BASE	+ 42, ��)
	//CMD_PC_MASTER_CANCEL�����Ͷ���Begin
	MESSAGE_DEF(	MSG_MASTER_CANCLE_BUSY,				1	,	�Լ��İ�ʦ���з�æ)
	MESSAGE_DEF(	MSG_MASTER_CANCLE_TIMEOUT,			2	,	���ΰ�ʦ��ʱ)
	MESSAGE_DEF(	MSG_MASTER_CANCLE_OFFLINE,			3	,	�Է�������)
	MESSAGE_DEF(	MSG_MASTER_CANCLE_INVITER_ISFULL,	4	,	�Է���ͽ��������)
	MESSAGE_DEF(	MSG_MASTER_CANCLE_SELF_ISFULL,		5	,	�Լ���ʦ��������)
	MESSAGE_DEF(	MSG_MASTER_CANCLE_CANCEL,			6	,	�Է�ȡ���˱��ΰ�ʦ)
	//CMD_PC_MASTER_CANCEL�����Ͷ���End
MESSAGE_DEF(	CMD_PC_MASTER_REFRESH_INFO,			CMD_PC_BASE	+ 43	,	ˢ��ʦ��������Ϣ)
MESSAGE_DEF(	CMD_PC_PRENTICE_REFRESH_INFO,		CMD_PC_BASE	+ 44	,	ˢ��ͽ�ܸ�����Ϣ)
//
//��ʦ�����END

//Begin ����ָ��
	MESSAGE_DEF(	CMD_PC_GM1SAY,		CMD_PC_BASE	+ 400		,	GM��ȫ���㲥)
	MESSAGE_DEF(	CMD_PC_SAY2TRADE,	CMD_PC_BASE	+ 401		,	����Ƶ��)
	MESSAGE_DEF(	CMD_PC_SAY2ALL,		CMD_PC_BASE + 402		,	������Ϸ����㲥)
	MESSAGE_DEF(	CMD_PC_SAY2YOU,		CMD_PC_BASE	+ 403		,	P2P����(��Ҷ����))
	MESSAGE_DEF(	CMD_PC_SAY2TEM,		CMD_PC_BASE	+ 404		,	�������)
	MESSAGE_DEF(	CMD_PC_SAY2GUD,		CMD_PC_BASE	+ 405		,	�л�(Guild)����)

	MESSAGE_DEF(	CMD_PC_SESS_CREATE,	CMD_PC_BASE	+ 406		,	�Ự����)
	MESSAGE_DEF(	CMD_PC_SESS_SAY,	CMD_PC_BASE	+ 407		,	�Ự˵��)
	MESSAGE_DEF(	CMD_PC_SESS_ADD,	CMD_PC_BASE	+ 408		,	�Ự�����Ա)
	MESSAGE_DEF(	CMD_PC_SESS_LEAVE,	CMD_PC_BASE	+ 409		,	�뿪�Ự)
	MESSAGE_DEF(	CMD_PC_GM1SAY1,	CMD_PC_BASE	+ 411			,	�����㲥)

	// FEATURE: GUILD_BANK
	MESSAGE_DEF(CMD_PC_ERRMSG, CMD_PC_BASE + 414, Error Message)
	MESSAGE_DEF(CMD_PC_UPDATEGUILDATTR, CMD_PC_BASE + 415,	Guild Attr)

//End ����ָ��
//Begin�Ҷ�����������ϵͳ
	MESSAGE_DEF(	CMD_PC_GARNER2_ORDER,	CMD_PC_BASE +101, ��)
//End�Ҷ�����������ϵͳ

// Add by lark.li 20081119 begin
MESSAGE_DEF(	CMD_OS_LOGIN,		CMD_OS_BASE + 1			,	��¼)
MESSAGE_DEF(	CMD_OS_PING,		CMD_OS_BASE + 2			,	�������)
MESSAGE_DEF(	CMD_SO_LOGIN,		CMD_SO_BASE + 1			,	��¼����)
MESSAGE_DEF(	CMD_SO_PING,		CMD_SO_BASE + 2			,	�������)
MESSAGE_DEF(	CMD_SO_WARING,		CMD_SO_BASE + 3			,	��������)
MESSAGE_DEF(	CMD_SO_EXCEPTION,	CMD_SO_BASE + 4		,	�쳣����)

MESSAGE_DEF(	CMD_SO_ON_LINE,		CMD_SO_BASE + 5			,	��������)
MESSAGE_DEF(	CMD_SO_OFF_LINE,	CMD_SO_BASE + 6			,	��������)
MESSAGE_DEF(	CMD_SO_ENTER_MAP,	CMD_SO_BASE + 7			,	�����ͼ����)
MESSAGE_DEF(	CMD_SO_LEAVE_MAP,	CMD_SO_BASE + 8			,	�뿪��ͼ����)

// End

// Add by lark.li 20090601 begin
MESSAGE_DEF(	CMD_AB_LOGIN,		CMD_AB_BASE + 1			,	��¼)
MESSAGE_DEF(	CMD_AB_LOGOUT,		CMD_AB_BASE + 2			,	�ǳ�)

MESSAGE_DEF(	CMD_AB_USER_LOGINT,	CMD_AB_BASE + 3			,	��ҵ�¼)
MESSAGE_DEF(	CMD_AB_USER_LOGOUT,	CMD_AB_BASE + 4			,	��ҵǳ�)

MESSAGE_DEF(	CMD_BA_USER_TIME,	CMD_BA_BASE + 1			,	���ʱ��֪ͨ)
MESSAGE_DEF(	CMD_BA_KILL_USER,	CMD_BA_BASE + 2			,	�ߵ���ң�Ŀǰ�������1.����ʱ����Ѿ�����5Сʱ����ʱ��2.�����֤ʧ�ܣ�)

// End

inline static void InitMessage()
{
	MESSAGE_ADD(	CMD_CM_ROLEBASE,				CMD_CM_BASE + 300,	(300-400)����ϵͳ�������)
	MESSAGE_ADD(	CMD_CM_GULDBASE,				CMD_CM_BASE	+ 400,	(400-430)���������ַ)
	MESSAGE_ADD(	CMD_CM_CHARBASE,				CMD_CM_BASE	+ 430,	(430-450)��ɫ�������)
	MESSAGE_ADD(	CMD_CM_SAY,						CMD_CM_BASE + 1,	��Ұ��˵��)
	MESSAGE_ADD(	CMD_CM_BEGINACTION,				CMD_CM_BASE + 6,	��)
	MESSAGE_ADD(	CMD_CM_ENDACTION,				CMD_CM_BASE + 7,	��)
	MESSAGE_ADD(	CMD_CM_SYNATTR,					CMD_CM_BASE + 8,	ͬ����ɫ���ԣ�ͬ�����Ե�����ֵ��)
	MESSAGE_ADD(	CMD_CM_SYNSKILLBAG,				CMD_CM_BASE + 9,	ͬ��������)
	MESSAGE_ADD(	CMD_CM_DIE_RETURN,				CMD_CM_BASE + 10,	��������)
	MESSAGE_ADD(	CMD_CM_SKILLUPGRADE,			CMD_CM_BASE + 11,	��)
	MESSAGE_ADD(	CMD_CM_PING,					CMD_CM_BASE + 15,	��)
	MESSAGE_ADD(	CMD_CM_REFRESH_DATA,			CMD_CM_BASE + 16,	��)
	MESSAGE_ADD(	CMD_CM_CHECK_PING,				CMD_CM_BASE + 17,	����Ping)
	MESSAGE_ADD(	CMD_CM_MAP_MASK,				CMD_CM_BASE + 18,	��)
	MESSAGE_ADD(	CMD_CM_UPDATEHAIR,				CMD_CM_BASE + 20,	�ͻ��������������)
	MESSAGE_ADD(	CMD_CM_TEAM_FIGHT_ASK,			CMD_CM_BASE + 21,	������ս����)
	MESSAGE_ADD(	CMD_CM_TEAM_FIGHT_ASR,			CMD_CM_BASE + 22,	������սӦ��(ANSWER))
	MESSAGE_ADD(	CMD_CM_ITEM_REPAIR_ASK,			CMD_CM_BASE + 23,	������������)
	MESSAGE_ADD(	CMD_CM_ITEM_REPAIR_ASR,			CMD_CM_BASE + 24,	��������Ӧ��)
	MESSAGE_ADD(	CMD_CM_ITEM_FORGE_ASK,			CMD_CM_BASE + 25,	���߾�������)
	MESSAGE_ADD(	CMD_CM_ITEM_FORGE_ASR,			CMD_CM_BASE + 26,	���߾���Ӧ��)
	MESSAGE_ADD(	CMD_CM_TIGER_START,				CMD_CM_BASE + 27,	�����ϻ���)
	MESSAGE_ADD(	CMD_CM_TIGER_STOP,				CMD_CM_BASE + 28,	ֹͣ�ϻ���)
	MESSAGE_ADD(	CMD_CM_ITEM_FORGE_CANACTION,	CMD_CM_BASE + 29,	��������������ɣ�����������¡�)
	MESSAGE_ADD(	CMD_CM_KITBAG_LOCK,				CMD_CM_BASE + 31,	������������)
	MESSAGE_ADD(	CMD_CM_KITBAG_UNLOCK,			CMD_CM_BASE + 32,	������������)
	MESSAGE_ADD(	CMD_CM_KITBAG_CHECK,			CMD_CM_BASE + 33,	��鱳������״̬)
	MESSAGE_ADD(	CMD_CM_KITBAG_AUTOLOCK,			CMD_CM_BASE + 34,	�����Զ�����)
	MESSAGE_ADD(	CMD_CM_KITBAGTEMP_SYNC,			CMD_CM_BASE + 35,	ͬ����ʱ����)
	MESSAGE_ADD(	CMD_CM_STORE_OPEN_ASK,			CMD_CM_BASE + 41,	���̳�)
	MESSAGE_ADD(	CMD_CM_STORE_LIST_ASK,			CMD_CM_BASE + 42,	�̳��б�)
	MESSAGE_ADD(	CMD_CM_STORE_BUY_ASK,			CMD_CM_BASE + 43,	�������)
	MESSAGE_ADD(	CMD_CM_STORE_CHANGE_ASK,		CMD_CM_BASE + 44,	�һ�����)
	MESSAGE_ADD(	CMD_CM_STORE_QUERY,				CMD_CM_BASE + 45,	���׼�¼��ѯ)
	MESSAGE_ADD(	CMD_CM_STORE_VIP,				CMD_CM_BASE + 46,	VIP)
	MESSAGE_ADD(	CMD_CM_STORE_AFFICHE,			CMD_CM_BASE + 47,	������Ʒ)
	MESSAGE_ADD(	CMD_CM_STORE_CLOSE,				CMD_CM_BASE + 48,	��)
	MESSAGE_ADD(	CMD_CM_BLACKMARKET_EXCHANGE_REQ,CMD_CM_BASE + 51,	���жһ�)
	MESSAGE_ADD(	CMD_CM_CHEAT_CHECK,				CMD_CM_BASE + 52,	��)
	MESSAGE_ADD(	CMD_CM_VOLUNTER_LIST,			CMD_CM_BASE + 61,	��ʾ־Ը���б�)
	MESSAGE_ADD(	CMD_CM_VOLUNTER_ADD,			CMD_CM_BASE + 62,	��ӵ�־Ը���б�)
	MESSAGE_ADD(	CMD_CM_VOLUNTER_DEL,			CMD_CM_BASE + 63,	���б���ɾ��)
	MESSAGE_ADD(	CMD_CM_VOLUNTER_SEL,			CMD_CM_BASE + 64,	�������)
	MESSAGE_ADD(	CMD_CM_VOLUNTER_OPEN,			CMD_CM_BASE + 65,	��־Ը�����)
	MESSAGE_ADD(	CMD_CM_VOLUNTER_ASR,			CMD_CM_BASE + 66,	־Ը�����Ӧ��)
	MESSAGE_ADD(	CMD_CM_MASTER_INVITE,			CMD_CM_BASE + 71,	��ʦ)
	MESSAGE_ADD(	CMD_CM_MASTER_ASR,				CMD_CM_BASE + 72,	��)
	MESSAGE_ADD(	CMD_CM_MASTER_DEL,				CMD_CM_BASE + 73,	��)
	MESSAGE_ADD(	CMD_CM_PRENTICE_DEL,			CMD_CM_BASE + 74,	��)
	MESSAGE_ADD(	CMD_CM_PRENTICE_INVITE,			CMD_CM_BASE + 75,	��)
	MESSAGE_ADD(	CMD_CM_PRENTICE_ASR,			CMD_CM_BASE + 76,	��)
	MESSAGE_ADD(	CMD_CM_LIFESKILL_ASR,			CMD_CM_BASE + 80,	��������ˣ��ϳɣ��ֽ�ȵ�����)
	MESSAGE_ADD(	CMD_CM_LIFESKILL_ASK,			CMD_CM_BASE + 81,	��������ˣ��ϳɣ��ֽ�ȵ�ȷ��)
	MESSAGE_ADD(	CMD_CM_BIDUP,					CMD_CM_BASE + 86,	��)
	MESSAGE_ADD(	CMD_CM_SAY2CAMP,				CMD_CM_BASE + 91,	��)
	MESSAGE_ADD(	CMD_CM_GM_SEND,					CMD_CM_BASE + 92,	��)
	MESSAGE_ADD(	CMD_CM_GM_RECV,					CMD_CM_BASE + 93,	��)
	MESSAGE_ADD(	CMD_CM_PK_CTRL,					CMD_CM_BASE + 94,	��)
	MESSAGE_ADD(	CMD_CM_ITEM_LOTTERY_ASK,		CMD_CM_BASE + 95,	���߾�������)
	MESSAGE_ADD(	CMD_CM_ITEM_LOTTERY_ASR,		CMD_CM_BASE + 96,	��Ʊ�趨Ӧ��)
	MESSAGE_ADD(	CMD_CM_CAPTAIN_CONFIRM_ASR,		CMD_CM_BASE + 97,	�������ӳ�Ӧ��)
	MESSAGE_ADD(	CMD_CM_ITEM_AMPHITHEATER_ASK,	CMD_CM_BASE + 98,	ͶƱ����)
	MESSAGE_ADD(	CMD_CM_ITEM_LOCK_ASK,			CMD_CM_BASE	+ 99,	��)
	MESSAGE_ADD(	CMD_CM_ITEM_UNLOCK_ASK,			CMD_CM_BASE + 100,	��)
	MESSAGE_ADD(	CMD_CM_SYNCHAR,					CMD_CM_BASE + 101, sync)
	MESSAGE_ADD(    CMD_CM_GETORIGNAMECOLOR,        CMD_CM_BASE + 102, GetOldColor)
	MESSAGE_ADD(	CMD_CM_CHANGETITLE,				CMD_CM_BASE + 103, ChangeTitle)
	MESSAGE_ADD(	CMD_CM_SYNGUILDCOLOR,			CMD_CM_BASE + 104, SynGuildColor)
	MESSAGE_ADD(	CMD_CM_GUILD_PUTNAME,		CMD_CM_GULDBASE	+ 1,	�ͻ������빫����)
	MESSAGE_ADD(	CMD_CM_GUILD_TRYFOR,		CMD_CM_GULDBASE	+ 2,	������빫��)
	MESSAGE_ADD(	CMD_CM_GUILD_TRYFORCFM,		CMD_CM_GULDBASE	+ 3,	ȷ���Ƿ񸲸���ǰ����)
	MESSAGE_ADD(	CMD_CM_GUILD_LISTTRYPLAYER,	CMD_CM_GULDBASE	+ 4,	�г����д��������)
	MESSAGE_ADD(	CMD_CM_GUILD_APPROVE,		CMD_CM_GULDBASE	+ 5,	������׼�����˼���)
	MESSAGE_ADD(	CMD_CM_GUILD_REJECT,		CMD_CM_GULDBASE	+ 6,	����ܾ������˼���)
	MESSAGE_ADD(	CMD_CM_GUILD_KICK,			CMD_CM_GULDBASE	+ 7,	��)
	MESSAGE_ADD(	CMD_CM_GUILD_LEAVE,			CMD_CM_GULDBASE	+ 8,	��)
	MESSAGE_ADD(	CMD_CM_GUILD_DISBAND,		CMD_CM_GULDBASE	+ 9,	��)
	MESSAGE_ADD(	CMD_CM_GUILD_MOTTO,			CMD_CM_GULDBASE	+ 10,	��)
	MESSAGE_ADD(	CMD_CM_GUILD_CHALLENGE,		CMD_CM_GULDBASE + 11,	��ս����)
	MESSAGE_ADD(	CMD_CM_GUILD_LEIZHU,		CMD_CM_GULDBASE + 12,	������)
	MESSAGE_DEF(CMD_CM_GUILD_PERM, CMD_CM_GULDBASE + 13, Guild Perm)
	MESSAGE_ADD(	CMD_CM_LOGIN,				CMD_CM_CHARBASE + 1,	�û��ʺŵ���(�û���/�������֤))
	MESSAGE_ADD(	CMD_CM_LOGOUT,				CMD_CM_CHARBASE + 2,	�û��ʺŵǳ�)
	MESSAGE_ADD(	CMD_CM_BGNPLAY,				CMD_CM_CHARBASE + 3,	�û�ѡ���ɫ��ʼ����Ϸ)
	MESSAGE_ADD(	CMD_CM_ENDPLAY,				CMD_CM_CHARBASE + 4,	�û���������Ϸ����ѡ/��/ɾ��ɫ����)
	MESSAGE_ADD(	CMD_CM_NEWCHA,				CMD_CM_CHARBASE	+ 5,	�û��½���ɫ)
	MESSAGE_ADD(	CMD_CM_DELCHA,				CMD_CM_CHARBASE	+ 6,	�û�ɾ����ɫ)
	MESSAGE_ADD(	CMD_CM_CANCELEXIT,			CMD_CM_CHARBASE + 7,	�û�Ҫ��ȡ���˳�)
	MESSAGE_ADD(	CMD_CM_REQUESTNPC,			CMD_CM_ROLEBASE + 1,	�ͻ��������NPC����)
	MESSAGE_ADD(	CMD_CM_TALKPAGE,			CMD_CM_ROLEBASE + 2,	�ͻ���������NPC�Ի�ҳ����)
	MESSAGE_ADD(	CMD_CM_FUNCITEM,			CMD_CM_ROLEBASE + 3,	�ͻ�������ָ������ѡ�����)
	MESSAGE_ADD(	CMD_CM_REQUESTTRADE,		CMD_CM_ROLEBASE + 8,	�ͻ���������н���)
	MESSAGE_ADD(	CMD_CM_TRADEITEM,			CMD_CM_ROLEBASE + 9,	�ͻ�����������Ʒ)
	MESSAGE_ADD(	CMD_CM_REQUESTAGENCY,		CMD_CM_ROLEBASE + 10,	�ͻ�������npc������)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_REQUEST,	CMD_CM_ROLEBASE + 12,	����������)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_ACCEPT,	CMD_CM_ROLEBASE + 13,	���ܽ�������)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_REJECT,	CMD_CM_ROLEBASE + 14,	�ܾ���������)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_CANCEL,	CMD_CM_ROLEBASE + 15,	ȡ������)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_ITEM,		CMD_CM_ROLEBASE + 16,	��ɫ������Ʒ)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_VALIDATEDATA,CMD_CM_ROLEBASE + 17,	ȷ�Ͻ�������)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_VALIDATE,	CMD_CM_ROLEBASE + 18,	ȷ�Ͻ���)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_MONEY,		CMD_CM_ROLEBASE + 19,	�϶���Ǯ)
	MESSAGE_ADD(	CMD_CM_MISSION,				CMD_CM_ROLEBASE + 22,	�ͻ�������������)
	MESSAGE_ADD(	CMD_CM_MISSIONLIST,			CMD_CM_ROLEBASE + 23,	�����б�)
	MESSAGE_ADD(	CMD_CM_MISSIONTALK,			CMD_CM_ROLEBASE + 24,	����Ի�)
	MESSAGE_ADD(	CMD_CM_MISLOG,				CMD_CM_ROLEBASE + 25,	������־)
	MESSAGE_ADD(	CMD_CM_MISLOGINFO,			CMD_CM_ROLEBASE + 26,	����������ϸ��Ϣ)
	MESSAGE_ADD(	CMD_CM_MISLOG_CLEAR,		CMD_CM_ROLEBASE + 27,	����ɾ��һ������)
	MESSAGE_ADD(	CMD_CM_STALL_ALLDATA,		CMD_CM_ROLEBASE + 30,	�����̯������Ϣ)
	MESSAGE_ADD(	CMD_CM_STALL_OPEN,			CMD_CM_ROLEBASE + 31,	����򿪰�̯������Ϣ)
	MESSAGE_ADD(	CMD_CM_STALL_BUY,			CMD_CM_ROLEBASE + 32,	�������̯����)
	MESSAGE_ADD(	CMD_CM_STALL_CLOSE,			CMD_CM_ROLEBASE + 33,	������̯)
	MESSAGE_ADD(	CMD_CM_CREATE_BOAT,			CMD_CM_ROLEBASE + 38,	���󴴽���)
	MESSAGE_ADD(	CMD_CM_UPDATEBOAT_PART,		CMD_CM_ROLEBASE + 39,	���������ֻ����)
	MESSAGE_ADD(	CMD_CM_BOAT_CANCEL,			CMD_CM_ROLEBASE + 40,	����ȡ�����촬ֻ)
	MESSAGE_ADD(	CMD_CM_BOAT_LUANCH,			CMD_CM_ROLEBASE + 41,	ѡ��ֻ����)
	MESSAGE_ADD(	CMD_CM_BOAT_BAGSEL,			CMD_CM_ROLEBASE + 42,	ѡ��һ�Ҵ�ֻ���մ�����л���)
	MESSAGE_ADD(	CMD_CM_BOAT_SELECT,			CMD_CM_ROLEBASE + 43,	ѡ��ֻ)
	MESSAGE_ADD(	CMD_CM_BOAT_GETINFO,		CMD_CM_ROLEBASE + 44,	��ȡ��ֻ��Ϣ)
	MESSAGE_ADD(	CMD_CM_ENTITY_EVENT,		CMD_CM_ROLEBASE + 45,	����ʵ���¼�)
	MESSAGE_ADD(	CMD_CM_CREATE_PASSWORD2,	CMD_CM_ROLEBASE + 46,	������������)
	MESSAGE_ADD(	CMD_CM_UPDATE_PASSWORD2,	CMD_CM_ROLEBASE + 47,	���¶�������)
	MESSAGE_ADD(	CMD_CM_READBOOK_START,		CMD_CM_ROLEBASE + 48,	��ʼ����)
	MESSAGE_ADD(	CMD_CM_READBOOK_CLOSE,		CMD_CM_ROLEBASE + 49,	��������)
	MESSAGE_ADD(	CMD_CM_SELECT_TIME_ASK,		CMD_CM_ROLEBASE + 50,	ѡ�������ʱ��)
	MESSAGE_ADD(	CMD_CM_BEHINDGENIUS_ASK,	CMD_CM_ROLEBASE + 51,	�������챳����)
	MESSAGE_ADD(	CMD_CM_GET_OUTTIMEEXP,		CMD_CM_ROLEBASE + 52,	��)
	MESSAGE_ADD(	CMD_CM_SELECT_COIN_ASK,		CMD_CM_ROLEBASE + 53,	��)
	MESSAGE_ADD(	CMD_CM_REQUEST_ANYTIMETRADE,CMD_CM_ROLEBASE + 54,	��ʱ����)
	MESSAGE_ADD(	CMD_CM_REQUEST_JEWELRYUP_ASK,CMD_CM_ROLEBASE + 55,	��ʯ����)
	MESSAGE_ADD(	CMD_CM_OPEN_CRYSTALTRADE_ASK,CMD_CM_ROLEBASE + 56,	��ˮ������ƽ̨)
	MESSAGE_ADD(	CMD_CM_REQUEST_BUY_ASK,		CMD_CM_ROLEBASE + 57,	����ҵ�)
	MESSAGE_ADD(	CMD_CM_REQUEST_SALE_ASK,	CMD_CM_ROLEBASE + 58,	�����ҵ�)
	MESSAGE_ADD(	CMD_CM_REQUEST_CANCEL_ASK,	CMD_CM_ROLEBASE + 59,	ȡ���ҵ�)
	MESSAGE_ADD(	CMD_CM_REQUEST_GETFLATMONEY,CMD_CM_ROLEBASE + 60,	ȡ��ƽ̨�˺��ϵ�Ǯ)
	MESSAGE_ADD(	CMD_CM_REQUEST_REFRESH_ASK,	CMD_CM_ROLEBASE + 61,	����ˢ��)
	MESSAGE_ADD(	CMD_CM_CHA_CONFIRM_CRYSTALTRADE,CMD_CM_ROLEBASE + 62,	���ȷ�Ͻ�����Ϣ)
	MESSAGE_ADD(	CMD_CM_GARNER2_REORDER,			CMD_CM_ROLEBASE + 71,	�ͻ�������������)
	MESSAGE_ADD(	CMD_CM_ANTIINDULGENCE,			CMD_CM_ROLEBASE + 72,	�������ƽ�)
	MESSAGE_ADD(	XTRAP_CMD_STEP_TWO,				CMD_CM_ROLEBASE + 73,	��)
	MESSAGE_ADD(	XTRAP_MAP_FILE_CHANGE,			CMD_CM_ROLEBASE + 74,	��)
	MESSAGE_ADD(	CMD_CM_OPEN_CYCELMISSION,		CMD_CM_ROLEBASE + 75,	����ѭ������)//Add by sunny.sun 20090831
	MESSAGE_ADD(	CMD_MC_ROLEBASE,				CMD_MC_BASE	+ 300,	(300-400)����ϵͳ�����ַ)
	MESSAGE_ADD(	CMD_MC_GULDBASE,				CMD_MC_BASE	+ 400,	(400-430)���������ַ)
	MESSAGE_ADD(	CMD_MC_CHARBASE,				CMD_MC_BASE	+ 430,	(430-450)��ɫ�����ַ)
	MESSAGE_ADD(	CMD_MC_SAY,						CMD_MC_BASE + 1,	��Ұ��˵��)
	MESSAGE_ADD(	CMD_MC_MAPCRASH,				CMD_MC_BASE	+ 3,	��)
	MESSAGE_ADD(	CMD_MC_CHABEGINSEE,				CMD_MC_BASE + 4,	��)
	MESSAGE_ADD(	CMD_MC_CHAENDSEE,				CMD_MC_BASE + 5,	��)
	MESSAGE_ADD(	CMD_MC_ITEMBEGINSEE,			CMD_MC_BASE + 6,	��)
	MESSAGE_ADD(	CMD_MC_ITEMENDSEE,				CMD_MC_BASE + 7,	��)
	MESSAGE_ADD(	CMD_MC_NOTIACTION,				CMD_MC_BASE + 8,	��)
	MESSAGE_ADD(	CMD_MC_SYNATTR,					CMD_MC_BASE + 9,	ͬ����ɫ���ԣ�ͬ�����Ե�����ֵ��)
	MESSAGE_ADD(	CMD_MC_SYNSKILLBAG,				CMD_MC_BASE + 10,	ͬ��������)
	MESSAGE_ADD(	CMD_MC_SYNASKILLSTATE,			CMD_MC_BASE + 11,	ͬ����ɫ״̬)
	MESSAGE_ADD(	CMD_MC_PING,					CMD_MC_BASE + 15,	��)
	MESSAGE_ADD(	CMD_MC_ENTERMAP,				CMD_MC_BASE	+ 16,	GameServer֪ͨClient�����ͼ��������ҵ�������Ϣ���丽���ṹ)
	MESSAGE_ADD(	CMD_MC_SYSINFO,					CMD_MC_BASE	+ 17,	ϵͳ��Ϣ��ʾ�������Ҳ����ߣ��㻹û�����ʲô����ȵȡ�)
	MESSAGE_ADD(	CMD_MC_ALARM,					CMD_MC_BASE	+ 18,	��)
	MESSAGE_ADD(	CMD_MC_TEAM,					CMD_MC_BASE + 19,	GameServer֪ͨ�ͻ���ĳ����Ա������ˢ��(�������, HP,SP))
	MESSAGE_ADD(	CMD_MC_FAILEDACTION,			CMD_MC_BASE + 20,	������ж�ִ��ʧ��)
	MESSAGE_ADD(	CMD_MC_MESSAGE,					CMD_MC_BASE + 21,	��ʾ��Ϣ)
	MESSAGE_ADD(	CMD_MC_ASTATEBEGINSEE,			CMD_MC_BASE + 22,	����״̬)
	MESSAGE_ADD(	CMD_MC_ASTATEENDSEE,			CMD_MC_BASE + 23,	��)
	MESSAGE_ADD(	CMD_MC_TLEADER_ID,				CMD_MC_BASE + 24,	GameServer֪ͨ�ͻ���ĳ����Ա�Ķӳ���Ϣ)
	MESSAGE_ADD(	CMD_MC_CHA_EMOTION,				CMD_MC_BASE + 25,	GameServer����ָ����ɫ������   )
	MESSAGE_ADD(	CMD_MC_QUERY_CHA,				CMD_MC_BASE + 26,	��ѯ��ɫ������Ϣ)
	MESSAGE_ADD(	CMD_MC_QUERY_CHAITEM,			CMD_MC_BASE + 27,	��ѯ��ɫ����)
	MESSAGE_ADD(	CMD_MC_CALL_CHA,				CMD_MC_BASE + 28,	����ɫ�ٻ����Լ����)
	MESSAGE_ADD(	CMD_MC_GOTO_CHA,				CMD_MC_BASE + 29,	���Լ����͵���ɫ���)
	MESSAGE_ADD(	CMD_MC_KICK_CHA,				CMD_MC_BASE + 30,	����ɫ������)
	MESSAGE_ADD(	CMD_MC_SYNDEFAULTSKILL,			CMD_MC_BASE + 31,	ͬ��Ĭ�ϼ���)
	MESSAGE_ADD(	CMD_MC_ADD_ITEM_CHA,			CMD_MC_BASE + 32,	��ӵ��߽�ɫ)
	MESSAGE_ADD(	CMD_MC_DEL_ITEM_CHA,			CMD_MC_BASE + 33,	ɾ�����߽�ɫ)
	MESSAGE_ADD(	CMD_MC_QUERY_CHAPING,			CMD_MC_BASE + 34,	��ѯ��ɫ��GameServer�߼����pingֵ)
	MESSAGE_ADD(	CMD_MC_QUERY_RELIVE,			CMD_MC_BASE + 35,	��ѯ�Ƿ����ԭ�ظ���)
	MESSAGE_ADD(	CMD_MC_PREMOVE_TIME,			CMD_MC_BASE + 36,	Ԥ�ƶ�ʱ��)
	MESSAGE_ADD(	CMD_MC_CHECK_PING,				CMD_MC_BASE + 37,	����Ping)
	MESSAGE_ADD(	CMD_MC_MAP_MASK,				CMD_MC_BASE + 38,	��)
	MESSAGE_ADD(	CMD_MC_OPENHAIR,				CMD_MC_BASE + 39,	��������)
	MESSAGE_ADD(	CMD_MC_UPDATEHAIR_RES,			CMD_MC_BASE + 40,	�������͵Ľ��)
	MESSAGE_ADD(	CMD_MC_EVENT_INFO,				CMD_MC_BASE + 41,	ͬ���¼���Ϣ)
	MESSAGE_ADD(	CMD_MC_SIDE_INFO,				CMD_MC_BASE + 42,	ͬ���ֱ���Ϣ)
	MESSAGE_ADD(	CMD_MC_TEAM_FIGHT_ASK,			CMD_MC_BASE + 43,	������ս����)
	MESSAGE_ADD(	CMD_MC_ITEM_REPAIR_ASK,			CMD_MC_BASE + 44,	������������)
	MESSAGE_ADD(	CMD_MC_ITEM_REPAIR_ASR,			CMD_MC_BASE + 45,	��������Ӧ��)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_REPAIR,		CMD_MC_BASE + 46,	��ʼ��������)
	MESSAGE_ADD(	CMD_MC_APPEND_LOOK,				CMD_MC_BASE + 47,	�������)
	MESSAGE_ADD(	CMD_MC_ITEM_FORGE_ASK,			CMD_MC_BASE + 48,	���߾�������)
	MESSAGE_ADD(	CMD_MC_ITEM_FORGE_ASR,			CMD_MC_BASE + 49,	���߾���Ӧ��)
	MESSAGE_ADD(	CMD_MC_ITEM_USE_SUC,			CMD_MC_BASE + 50,	����ʹ�óɹ�)
	MESSAGE_ADD(	CMD_MC_KITBAG_CAPACITY,			CMD_MC_BASE + 51,	��������)
	MESSAGE_ADD(	CMD_MC_ESPE_ITEM,				CMD_MC_BASE + 52,	�������)
	MESSAGE_ADD(	CMD_MC_KITBAG_CHECK_ASR,		CMD_MC_BASE + 53,	��������״̬��Ӧ��)
	MESSAGE_ADD(	CMD_MC_KITBAGTEMP_SYNC,			CMD_MC_BASE + 54,	ͬ����ʱ����)
	MESSAGE_ADD(	CMD_MC_STORE_OPEN_ASR,			CMD_MC_BASE + 61,	���̳�)
	MESSAGE_ADD(	CMD_MC_STORE_LIST_ASR,			CMD_MC_BASE + 62,	�̳��б�)
	MESSAGE_ADD(	CMD_MC_STORE_BUY_ASR,			CMD_MC_BASE + 63,	�������)
	MESSAGE_ADD(	CMD_MC_STORE_CHANGE_ASR,		CMD_MC_BASE + 64,	�һ�����)
	MESSAGE_ADD(	CMD_MC_STORE_QUERY,				CMD_MC_BASE + 65,	���׼�¼��ѯ)
	MESSAGE_ADD(	CMD_MC_STORE_VIP,				CMD_MC_BASE + 66,	VIP)
	MESSAGE_ADD(	CMD_MC_STORE_AFFICHE,			CMD_MC_BASE + 67,	������Ʒ)
	MESSAGE_ADD(	CMD_MC_POPUP_NOTICE,			CMD_MC_BASE + 68,	��)
	MESSAGE_ADD(	CMD_MC_BLACKMARKET_EXCHANGEDATA,	CMD_MC_BASE + 71,	���жһ�����)
	MESSAGE_ADD(	CMD_MC_BLACKMARKET_EXCHANGE_ASR,	CMD_MC_BASE + 72,	���жһ�Ӧ��)
	MESSAGE_ADD(	CMD_MC_BLACKMARKET_EXCHANGEUPDATE,	CMD_MC_BASE + 73,	���жһ�����)
	MESSAGE_ADD(	CMD_MC_BLACKMARKET_TRADEUPDATE,	CMD_MC_BASE + 74,	���н��׸���)
	MESSAGE_ADD(	CMD_MC_EXCHANGEDATA,			CMD_MC_BASE + 75,	��ͨ�һ�����)
	MESSAGE_ADD(	CMD_MC_VOLUNTER_LIST,			CMD_MC_BASE + 81,	��ʾ־Ը���б�)
	MESSAGE_ADD(	CMD_MC_VOLUNTER_STATE,			CMD_MC_BASE + 82,	־Ը��״̬)
	MESSAGE_ADD(	CMD_MC_VOLUNTER_SEL,			CMD_MC_BASE + 83,	�������)
	MESSAGE_ADD(	CMD_MC_VOLUNTER_OPEN,			CMD_MC_BASE + 84,	��־Ը�����)
	MESSAGE_ADD(	CMD_MC_VOLUNTER_ASK,			CMD_MC_BASE + 85,	־Ը���������)
	MESSAGE_ADD(	CMD_MC_LISTAUCTION,				CMD_MC_BASE + 86,	��)
	MESSAGE_ADD(	CMD_MC_MASTER_ASK,				CMD_MC_BASE + 91,	��)
	MESSAGE_ADD(	CMD_MC_PRENTICE_ASK,			CMD_MC_BASE + 92,	��)
	MESSAGE_ADD(	CMD_MC_CHAPLAYEFFECT,			CMD_MC_BASE + 93,	��)
	MESSAGE_ADD(	CMD_MC_SAY2CAMP,				CMD_MC_BASE + 96,	��)
	MESSAGE_ADD(	CMD_MC_GM_MAIL,					CMD_MC_BASE + 97,	��)
	MESSAGE_ADD(	CMD_MC_CHEAT_CHECK,				CMD_MC_BASE + 98,	��)
	MESSAGE_ADD(	CMD_CM_ITEM_LOCK_ASR,			CMD_MC_BASE	+	99,	��)
	MESSAGE_ADD(	CMD_MC_ITEM_UNLOCK_ASR,			CMD_MC_BASE	+	95,	��)

	// FEATURE: GUILD_BANL
	MESSAGE_ADD(CMD_MC_UPDATEGUILDBANKGOLD, CMD_MC_BASE + 120, Update Guild Bank Gold)

	MESSAGE_ADD(	CMD_MC_GUILD_GETNAME,		CMD_MC_GULDBASE	+ 1,	����ͻ������빫����)
	MESSAGE_ADD(	CMD_MC_LISTGUILD,			CMD_MC_GULDBASE	+ 2,	����ͻ��˴�������봰�ڲ������л��б�)
	MESSAGE_ADD(	CMD_MC_GUILD_TRYFORCFM,		CMD_MC_GULDBASE	+ 3,	ȷ���Ƿ񸲸���ǰ����)
	MESSAGE_ADD(	CMD_MC_GUILD_LISTTRYPLAYER,	CMD_MC_GULDBASE	+ 4,	�г����д��������)
	MESSAGE_ADD(	CMD_MC_GUILD_LISTCHALL,		CMD_MC_GULDBASE + 5,	��ѯ������ս��Ϣ)
	MESSAGE_ADD(	CMD_MC_GUILD_MOTTO,			CMD_MC_GULDBASE	+ 10,	�޸�������)
	MESSAGE_ADD(	CMD_MC_GUILD_LEAVE,			CMD_MC_GULDBASE + 11,	�뿪����)
	MESSAGE_ADD(	CMD_MC_GUILD_KICK,			CMD_MC_GULDBASE + 12,	���߳�����)
	MESSAGE_ADD(	CMD_MC_GUILD_INFO,			CMD_MC_GULDBASE + 13,	ͬ��������Ϣ)
	MESSAGE_ADD(	CMD_MC_LOGIN,				CMD_MC_CHARBASE + 1,	��)
	MESSAGE_ADD(	CMD_MC_LOGOUT,				CMD_MC_CHARBASE + 2,	��)
	MESSAGE_ADD(	CMD_MC_BGNPLAY,				CMD_MC_CHARBASE + 3,	��)
	MESSAGE_ADD(	CMD_MC_ENDPLAY,				CMD_MC_CHARBASE + 4,	��)
	MESSAGE_ADD(	CMD_MC_NEWCHA,				CMD_MC_CHARBASE	+ 5,	��)
	MESSAGE_ADD(	CMD_MC_DELCHA,				CMD_MC_CHARBASE	+ 6,	��)
	MESSAGE_ADD(	CMD_MC_STARTEXIT,			CMD_MC_CHARBASE + 7	,	��ʼ�˳�����ʱ)
	MESSAGE_ADD(	CMD_MC_CANCELEXIT,			CMD_MC_CHARBASE + 8	,	ȡ���˳�����ʱ)
	MESSAGE_ADD(	CMD_MC_CHAPSTR,				CMD_MC_CHARBASE	+ 10,	��)
	MESSAGE_ADD(	CMD_MC_CREATE_PASSWORD2,	CMD_MC_CHARBASE + 11,	��)
	MESSAGE_ADD(	CMD_MC_UPDATE_PASSWORD2,	CMD_MC_CHARBASE + 12,	��)
	MESSAGE_ADD(	CMD_MC_SYNCCHAR, CMD_MC_CHARBASE + 13, ��)
	MESSAGE_ADD(    CMD_MC_GETORIGCOLOR, CMD_MC_CHARBASE + 14, GetOrigColor)
	MESSAGE_ADD(    CMD_MC_CHANGETITLE,	CMD_MC_CHARBASE + 15, SetTitle)
	MESSAGE_ADD(	CMD_MC_SYNGUILDCOLOR, CMD_MC_CHARBASE + 16, SynGuildColor)
	MESSAGE_ADD(	CMD_MC_TALKPAGE,			CMD_MC_ROLEBASE + 1,	��������ͻ��˷��ͶԻ���Ϣ)
	MESSAGE_ADD(	CMD_MC_FUNCPAGE,			CMD_MC_ROLEBASE + 2,	��������ͻ��˷��͹���ѡ��ҳ��Ϣ)
	MESSAGE_ADD(	CMD_MC_CLOSETALK,			CMD_MC_ROLEBASE + 3,	��������ͻ��˷��͹رնԻ���Ϣ)
	MESSAGE_ADD(	CMD_MC_HELPINFO,			CMD_MC_ROLEBASE + 4,	��������ͻ��˷��Ͱ�����Ϣ)
	MESSAGE_ADD(	CMD_MC_TRADEPAGE,			CMD_MC_ROLEBASE + 5,	��������ͻ��˷��ͽ�����Ϣ)
	MESSAGE_ADD(	CMD_MC_TRADERESULT,			CMD_MC_ROLEBASE + 6,	��������ͻ��˷��ͽ����������Ϣ)
	MESSAGE_ADD(	CMD_MC_TRADE_DATA,			CMD_MC_ROLEBASE + 7,	NPC����������Ϣ����)
	MESSAGE_ADD(	CMD_MC_TRADE_ALLDATA,		CMD_MC_ROLEBASE + 8,	NPC����������Ϣ����)
	MESSAGE_ADD(	CMD_MC_TRADE_UPDATE,		CMD_MC_ROLEBASE + 9,	�������˽�����Ϣ����)
	MESSAGE_ADD(	CMD_MC_EXCHANGE_UPDATE,		CMD_MC_ROLEBASE + 10,	�������˶һ���Ϣ����)
	MESSAGE_ADD(	CMD_MC_CHARTRADE,			CMD_MC_ROLEBASE + 11,	��������ͻ��˷��ͽ�����Ϣ )
	MESSAGE_ADD(	CMD_MC_CHARTRADE_REQUEST,	CMD_MC_ROLEBASE + 12,	��������ͻ��˷���������ɫ���뽻����Ϣ)
	MESSAGE_ADD(	CMD_MC_CHARTRADE_CANCEL,	CMD_MC_ROLEBASE + 13,	��������ͻ��˷��� )
	MESSAGE_ADD(	CMD_MC_CHARTRADE_ITEM,		CMD_MC_ROLEBASE + 14,	��������ͻ��˽�ɫ���ͽ��׵���Ʒ��Ϣ)
	MESSAGE_ADD(	CMD_MC_CHARTRADE_PAGE,		CMD_MC_ROLEBASE + 15,	��������ͻ��˷��Ϳ�ʼ���׽�����Ϣ)
	MESSAGE_ADD(	CMD_MC_CHARTRADE_VALIDATEDATA,	CMD_MC_ROLEBASE + 16,	ȷ�Ͻ�������)
	MESSAGE_ADD(	CMD_MC_CHARTRADE_VALIDATE,	CMD_MC_ROLEBASE + 17,	ȷ�Ͻ���)
	MESSAGE_ADD(	CMD_MC_CHARTRADE_RESULT,	CMD_MC_ROLEBASE + 18,	ȷ�Ͻ��׳ɹ�)
	MESSAGE_ADD(	CMD_MC_CHARTRADE_MONEY,		CMD_MC_ROLEBASE + 19,	�϶���Ǯ����)
	MESSAGE_ADD(	CMD_MC_MISSION,				CMD_MC_ROLEBASE + 22,	��������ͻ��˷��������б�ҳ��Ϣ)
	MESSAGE_ADD(	CMD_MC_MISSIONLIST,			CMD_MC_ROLEBASE + 23,	�����б�)
	MESSAGE_ADD(	CMD_MC_MISSIONTALK,			CMD_MC_ROLEBASE + 24,	����Ի�)
	MESSAGE_ADD(	CMD_MC_NPCSTATECHG,			CMD_MC_ROLEBASE + 25,	�����л�npc״̬)
	MESSAGE_ADD(	CMD_MC_TRIGGER_ACTION,		CMD_MC_ROLEBASE + 26,	����������)
	MESSAGE_ADD(	CMD_MC_MISPAGE,				CMD_MC_ROLEBASE + 27,	����ҳ��Ϣ)
	MESSAGE_ADD(	CMD_MC_MISLOG,				CMD_MC_ROLEBASE + 28,	������־)
	MESSAGE_ADD(	CMD_MC_MISLOGINFO,			CMD_MC_ROLEBASE + 29,	����������ϸ��Ϣ)
	MESSAGE_ADD(	CMD_MC_MISLOG_CHANGE,		CMD_MC_ROLEBASE + 30,	����״̬�仯)
	MESSAGE_ADD(	CMD_MC_MISLOG_CLEAR,		CMD_MC_ROLEBASE + 31,	���һ������)
	MESSAGE_ADD(	CMD_MC_MISLOG_ADD,			CMD_MC_ROLEBASE + 32,	���һ������)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_FUSION,	CMD_MC_ROLEBASE + 33	,	��ʼ�ۺϵ���)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_UPGRADE,	CMD_MC_ROLEBASE + 34	,	��ʼ��������)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_FORGE,	CMD_MC_ROLEBASE + 35	,	��ʼ���߾���)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_UNITE,	CMD_MC_ROLEBASE + 36	,	��ʼ���ߺϳ�)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_MILLING,	CMD_MC_ROLEBASE + 37	,	��ʼ���ߴ�ĥ)
	MESSAGE_ADD(	CMD_MC_CREATEBOAT,			CMD_MC_ROLEBASE + 38,	������ֻ)
	MESSAGE_ADD(	CMD_MC_UPDATEBOAT,			CMD_MC_ROLEBASE + 39,	���촬ֻ)
	MESSAGE_ADD(	CMD_MC_UPDATEBOAT_PART,		CMD_MC_ROLEBASE + 40,	���촬ֻ�Ĳ���)
	MESSAGE_ADD(	CMD_MC_BERTH_LIST,			CMD_MC_ROLEBASE + 41,	��ֻͣ���б�)
	MESSAGE_ADD(	CMD_MC_BOAT_LIST,			CMD_MC_ROLEBASE + 42,	ͬ����ɫӵ�е����д�ֻ����)
	MESSAGE_ADD(	CMD_MC_BOAT_ADD,			CMD_MC_ROLEBASE + 43,	���һ�Ҵ�)
	MESSAGE_ADD(	CMD_MC_BOAT_CLEAR,			CMD_MC_ROLEBASE + 44,	���һ�Ҵ�)
	MESSAGE_ADD(	CMD_MC_BOATINFO,			CMD_MC_ROLEBASE + 45,	ͬ����ֻ������Ϣ)
	MESSAGE_ADD(	CMD_MC_BOAT_BAGLIST,		CMD_MC_ROLEBASE + 46,	��ʾ���Դ���Ĵ�ֻ�б�)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_EIDOLON_METEMPSYCHOSIS,	CMD_MC_ROLEBASE + 47,	��ʼ����ת��)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_EIDOLON_FUSION,	CMD_MC_ROLEBASE + 48,	��ʼ�����ں�)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_PURIFY,	CMD_MC_ROLEBASE + 49	,	��ʼװ���ᴿ)
	MESSAGE_ADD(	CMD_MC_ENTITY_BEGINESEE,	CMD_MC_ROLEBASE + 50,	ͬ����Ұʵ����Ϣ)
	MESSAGE_ADD(	CMD_MC_ENTITY_ENDSEE,		CMD_MC_ROLEBASE + 51,	ʵ����Ϣ������Ұ)
	MESSAGE_ADD(	CMD_MC_ENTITY_CHGSTATE,		CMD_MC_ROLEBASE + 52,	ʵ��״̬�ı�)
	MESSAGE_ADD(	CMD_MC_STALL_ALLDATA,		CMD_MC_ROLEBASE + 54,	�鿴��̯������Ϣ)
	MESSAGE_ADD(	CMD_MC_STALL_UPDATE,		CMD_MC_ROLEBASE + 55,	��̯���������Ϣ)
	MESSAGE_ADD(	CMD_MC_STALL_DELGOODS,		CMD_MC_ROLEBASE + 56,	ɾ��ָ��λ�õĻ���)
	MESSAGE_ADD(	CMD_MC_STALL_CLOSE,			CMD_MC_ROLEBASE + 57,	̯����̯)
	MESSAGE_ADD(	CMD_MC_STALL_START,			CMD_MC_ROLEBASE + 58,	�ɹ���̯)
	MESSAGE_ADD(	CMD_MC_STALL_NAME,			CMD_MC_ROLEBASE + 59,	��̯��)
	MESSAGE_ADD(	CMD_MC_BICKER_NOTICE,		CMD_MC_ROLEBASE + 60,	����������Ϣ֪ͨ)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_ENERGY,	CMD_MC_ROLEBASE + 71	,	��ʼ���ǳ��)
	MESSAGE_ADD(	CMD_MC_BEGIN_GET_STONE,		CMD_MC_ROLEBASE + 72	,	��ʼ��ȡ��ʯ)
	MESSAGE_ADD(	CMD_MC_BEGIN_TIGER,			CMD_MC_ROLEBASE + 73	,	�����ϻ�������)
	MESSAGE_ADD(	CMD_MC_TIGER_ITEM_ID,		CMD_MC_ROLEBASE + 74	,	�ϻ�������ID)
	MESSAGE_ADD(	CMD_MC_TIGER_STOP,			CMD_MC_ROLEBASE + 75	,	�ϻ�������)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_FIX,		CMD_MC_ROLEBASE + 76	,	�����ƹ�)
	MESSAGE_ADD(	CMD_MC_BEGIN_GM_SEND,		CMD_MC_ROLEBASE + 77	,	����GM�ʼ�)
	MESSAGE_ADD(	CMD_MC_BEGIN_GM_RECV,		CMD_MC_ROLEBASE + 78	,	����GM�ʼ�)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_LOTTERY,	CMD_MC_ROLEBASE + 79	,	��ʼ���ò�Ʊ)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_AMPHITHEATER,	CMD_MC_ROLEBASE + 80,	�򿪾��������а�)
	MESSAGE_ADD(	CMD_MC_LEAVE_TIME_ASR,		CMD_MC_ROLEBASE + 81,	��ɫ����ʱ��)
	MESSAGE_ADD(	CMD_MC_SELECT_TIME_ASR,		CMD_MC_ROLEBASE + 82,	��ȡ���߾�����Ϣ�ظ�)
	MESSAGE_ADD(	CMD_MC_BEHINDGENIUS_ASR,	CMD_MC_ROLEBASE + 83	,	����������Ӧ��)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_JEWELRYUP,CMD_MC_ROLEBASE + 84	,	������ʯ��������)
	MESSAGE_ADD(	CMD_MC_OPEN_CRYSTALTRADE_ASR,CMD_MC_ROLEBASE + 85	,	��ˮ��ƽ̨Ӧ��)
	MESSAGE_ADD(	CMD_MC_REQUEST_BUY_ASR,		CMD_MC_ROLEBASE + 86 ,	����ҵ�Ӧ��)
	MESSAGE_ADD(	CMD_MC_REQUEST_SALE_ASR,	CMD_MC_ROLEBASE + 87 ,	�����ҵ�Ӧ��)
	MESSAGE_ADD(	CMD_MC_REQUEST_CANCEL_ASR,	CMD_MC_ROLEBASE + 88 ,	ȡ���ҵ�Ӧ��)
	MESSAGE_ADD(	CMD_MC_REQUEST_GETFLATMONEY,CMD_MC_ROLEBASE + 89 ,	ȡ��ƽ̨�ϵ�ǮӦ��)
	MESSAGE_ADD(	CMD_MC_REQUEST_REFRESH_ASR,	CMD_MC_ROLEBASE + 90 ,	����ˢ��Ӧ��)
	MESSAGE_ADD(	CMD_MC_GET_CRYSTALSTATE,	CMD_MC_ROLEBASE + 91 ,	��ȡ����״̬)
	MESSAGE_ADD(	CMD_MC_LIFESKILL_BGING,			CMD_MC_BASE + 100	,	����ܿ�ʼ)
	MESSAGE_ADD(	CMD_MC_LIFESKILL_ASR,			CMD_MC_BASE + 101	,	��������ˣ��ϳɣ��ֽ�ȵ�����)
	MESSAGE_ADD(	CMD_MC_LIFESKILL_ASK,			CMD_MC_BASE + 102	,	��������ˣ��ϳɣ��ֽ�ȵ�ȷ��)
	MESSAGE_ADD(	CMD_MC_ITEM_LOTTERY_ASK,		CMD_MC_BASE + 103,	��Ʊ����)
	MESSAGE_ADD(	CMD_MC_ITEM_LOTTERY_ASR,		CMD_MC_BASE + 104,	��ƱӦ��)
	MESSAGE_ADD(	CMD_MC_CAPTAIN_ASK,				CMD_MC_BASE + 105,	�������ӳ�ȷ������)
	MESSAGE_ADD(	CMD_MC_ITEM_AMPHITHEATER_ASR,	CMD_MC_BASE + 106, ��)
	MESSAGE_ADD(	CMD_MC_REQUEST_JEWELRYUP_ASR,	CMD_MC_BASE + 107, ��)
	MESSAGE_ADD(	XTRAP_CMD_STEP_ONE,				CMD_MC_BASE + 108, ��)
	MESSAGE_ADD(	CMD_TM_ENTERMAP,				CMD_TM_BASE	+ 3	,	����ͼ��������ɫID�������ͼ�����֡�λ��(x,y))
	MESSAGE_ADD(	CMD_TM_GOOUTMAP,				CMD_TM_BASE + 4		,	����ͼ)
	MESSAGE_ADD(	CMD_TM_LOGIN_ACK,				CMD_TM_BASE + 5, ��)
	MESSAGE_ADD(	CMD_TM_CHANGE_PERSONINFO,		CMD_TM_BASE	+6	,	�޸ĸ�����Ϣ)
	MESSAGE_ADD(	CMD_TM_MAPENTRY,				CMD_TM_BASE + 7, ��)
	MESSAGE_ADD(	CMD_TM_MAPENTRY_NOMAP,			CMD_TM_BASE + 8, ��)
	MESSAGE_ADD(	CMD_TM_MAP_ADMIN,				CMD_TM_BASE + 9	,	����Ա�õ�ͼ��������)
	MESSAGE_ADD(	CMD_TM_STATE,					CMD_TM_BASE + 10,	��������״̬�������ڴ�������汾��)
	MESSAGE_ADD(	CMD_MT_LOGIN,					CMD_MT_BASE + 1		,	GameServer��½GateServer)
	MESSAGE_ADD(	CMD_MT_SWITCHMAP,				CMD_MT_BASE + 4		,	��ʼ�л���ͼ����)
	MESSAGE_ADD(	CMD_MT_KICKUSER,				CMD_MT_BASE + 5		,	GameServer�ߵ�ĳ���û�)
	MESSAGE_ADD(	CMD_MT_MAPENTRY,				CMD_MT_BASE + 6		,	��ͼ���)
	MESSAGE_ADD(	CMD_MT_PALYEREXIT,				CMD_MT_BASE + 8		,	֪ͨGate�ͷŸ��û���Դ)
	MESSAGE_ADD(	CMD_MT_MAP_ADMIN,				CMD_MT_BASE + 9	,	����Ա�õ�ͼ��������)
	MESSAGE_ADD(	CMD_MT_STATE,					CMD_MT_BASE + 10,	��������״̬�������ڴ�������汾��)
	MESSAGE_ADD(	CMD_PM_TEAM,					CMD_PM_BASE + 1    ,	GroupServer�����������Ϣ����)
	MESSAGE_ADD(	CMD_PM_GUILD_DISBAND,			CMD_PM_BASE	+ 2		,	��ɢ����)
	MESSAGE_ADD(	CMD_PM_GUILDINFO,				CMD_PM_BASE + 4	,	GroupServer�����Ĺ���ͬ����Ϣ)
	MESSAGE_ADD(	CMD_PM_GUILD_CHALLMONEY,		CMD_PM_BASE + 5    ,	������սͶ���Ǯ�˸��᳤)
	MESSAGE_ADD(	CMD_PM_GUILD_CHALL_PRIZEMONEY,	CMD_PM_BASE + 6    ,	������սս������Ǯ���᳤)
	MESSAGE_ADD(	CMD_PM_GARNER2_UPDATE,			CMD_PM_BASE +7, ��)
	MESSAGE_ADD(	CMD_PM_TEAM_CREATE,				CMD_PM_BASE + 8, ��)
	MESSAGE_ADD(	CMD_PM_SAY2ALL,					CMD_PM_BASE + 9, ��)
	MESSAGE_ADD(	CMD_PM_SAY2TRADE,				CMD_PM_BASE + 10, ��)
	MESSAGE_ADD(	CMD_PM_EXPSCALE,				CMD_PM_BASE + 11       ,	 ������ϵͳ  Ӱ����Ҿ��鼰������)
	MESSAGE_ADD(	CMD_PM_GUILD_INVITE,			CMD_PM_BASE + 12		,	��������)

	// FEATURE: GUILD_BANK
	MESSAGE_ADD(CMD_PM_GUILDBANK, CMD_PM_BASE + 13, Guild Bank)


	MESSAGE_ADD(	CMD_MP_ENTERMAP,				CMD_MP_BASE	+ 1, ��)
	MESSAGE_ADD(	CMD_MP_GUILD_APPROVE,			CMD_MP_BASE	+ 2, ��)
	MESSAGE_ADD(	CMD_MP_GUILD_CREATE,			CMD_MP_BASE	+ 3, ��)
	MESSAGE_ADD(	CMD_MP_GUILD_KICK,				CMD_MP_BASE	+ 4, ��)
	MESSAGE_ADD(	CMD_MP_GUILD_LEAVE,				CMD_MP_BASE	+ 5, ��)
	MESSAGE_ADD(	CMD_MP_GUILD_DISBAND,			CMD_MP_BASE	+ 6, ��)
	MESSAGE_ADD(	CMD_MP_GUILD_MOTTO,				CMD_MP_BASE	+ 10, ��)
	MESSAGE_ADD(	CMD_MP_GUILD_CHALLMONEY,		CMD_MP_BASE + 13			,	��Ͷ�깫���Ǯ�˻��᳤��ɫ)
	MESSAGE_ADD(	CMD_MP_GUILD_CHALL_PRIZEMONEY,	CMD_MP_BASE + 14    ,	������ս�����ĵ�Ǯ���᳤��ɫ)
	MESSAGE_ADD(	CMD_MP_GARNER2_UPDATE,			CMD_MP_BASE + 7, ��)
	MESSAGE_ADD(	CMD_MP_GARNER2_CGETORDER,		CMD_MP_BASE + 15, ��)
	MESSAGE_ADD(	CMD_MP_TEAM_CREATE,				CMD_MP_BASE + 16, ��)
	MESSAGE_ADD(	CMD_MP_MASTER_CREATE,			CMD_MP_BASE + 17, ��)
	MESSAGE_ADD(	CMD_MP_MASTER_DEL,				CMD_MP_BASE + 18, ��)
	MESSAGE_ADD(	CMD_MP_MASTER_FINISH,			CMD_MP_BASE + 19, ��)
	MESSAGE_ADD(	CMD_MP_SAY2ALL,					CMD_MP_BASE + 20, ��)
	MESSAGE_ADD(	CMD_MP_SAY2TRADE,				CMD_MP_BASE + 21, ��)
	MESSAGE_ADD(	CMD_MP_GM1SAY1,					CMD_MP_BASE + 22, ��)
	MESSAGE_ADD(	CMD_MP_GM1SAY,					CMD_MP_BASE + 23, ��)
	MESSAGE_ADD(	CMD_MP_GMBANACCOUNT,			CMD_MP_BASE + 24, GM��ͣˢ���˺�)//Add by sunny.sun 20090828
	
	// FEATURE: GUILD_BANK
	MESSAGE_ADD(CMD_MP_GUILDBANK, CMD_MP_BASE + 25, Guild Bank)
	MESSAGE_ADD(CMD_MP_GUILDADDEXP, CMD_MP_BASE + 26, Guild Attr)
	MESSAGE_ADD(CMD_MP_GUILD_PERM, CMD_MP_BASE + 27, Guild Perm)

	MESSAGE_ADD(	CMD_TP_LOGIN,					CMD_TP_BASE + 1		,	GateServer��GroupServer���̼��½)
	MESSAGE_ADD(	CMD_TP_LOGOUT,					CMD_TP_BASE + 2		,	GateServer��GroupServer���̼�ǳ�)
	MESSAGE_ADD(	CMD_TP_USER_LOGIN,				CMD_TP_BASE	+ 3		,	�û�����)
	MESSAGE_ADD(	CMD_TP_USER_LOGOUT,				CMD_TP_BASE + 4		,	�û��ǳ�)
	MESSAGE_ADD(	CMD_TP_BGNPLAY,					CMD_TP_BASE	+ 5		,	�û�ѡ���ɫ��ʼ����Ϸ)
	MESSAGE_ADD(	CMD_TP_ENDPLAY,					CMD_TP_BASE	+ 6		,	�û���������Ϸ����ѡ/��/ɾ��ɫ����)
	MESSAGE_ADD(	CMD_TP_NEWCHA,					CMD_TP_BASE	+ 7		,	�½���ɫ)
	MESSAGE_ADD(	CMD_TP_DELCHA,					CMD_TP_BASE	+ 8		,	ɾ����ɫ)
	MESSAGE_ADD(	CMD_TP_PLAYEREXIT,				CMD_TP_BASE + 9		,	�˳���ɫ�ͷ���Դ)
	MESSAGE_ADD(	CMD_TP_REQPLYLST,				CMD_TP_BASE	+ 10	,	����GateServer�ϵ���ҵ�ַ�б�)
	MESSAGE_ADD(	CMD_TP_DISC,					CMD_TP_BASE + 11, ��)
	MESSAGE_ADD(	CMD_TP_ESTOPUSER_CHECK,			CMD_TP_BASE + 12, ��)
	MESSAGE_ADD(	CMD_TP_CREATE_PASSWORD2,		CMD_TP_BASE + 13,	������������)
	MESSAGE_ADD(	CMD_TP_UPDATE_PASSWORD2,		CMD_TP_BASE + 14,	���¶�������)
	MESSAGE_ADD(	CMD_TP_SYNC_PLYLST,				CMD_TP_BASE	+ 15	,	���ͱ�GateServer�ϵ���ҵ�ַ�б�)
	MESSAGE_ADD(	CMD_PT_KICKUSER,				CMD_PT_BASE	+ 11, ��)
	MESSAGE_ADD(	CMD_PT_ESTOPUSER,				CMD_PT_BASE + 12,	���Խ�ɫ)
	MESSAGE_ADD(	CMD_PT_DEL_ESTOPUSER,			CMD_PT_BASE + 13,	�������)

	MESSAGE_ADD(	CMD_PA_LOGIN,					CMD_PA_BASE + 2, ��)
	MESSAGE_ADD(	CMD_PA_USER_LOGIN,				CMD_PA_BASE + 4, ��)
	MESSAGE_ADD(	CMD_PA_USER_LOGOUT,				CMD_PA_BASE + 5, ��)
	MESSAGE_ADD(	CMD_PA_USER_DISABLE,			CMD_PA_BASE + 6, ��)
	MESSAGE_ADD(	CMD_PA_USER_LOGIN1,				CMD_PA_BASE + 13, ��)
	MESSAGE_ADD(	CMD_PA_USER_LOGIN2,				CMD_PA_BASE + 14, ��)
	MESSAGE_ADD(	CMD_PA_USER_BILLBGN,			CMD_PA_BASE + 20, ��)
	MESSAGE_ADD(	CMD_PA_USER_BILLEND,			CMD_PA_BASE + 21, ��)
	MESSAGE_ADD(	CMD_PA_GROUP_BILLEND_AND_LOGOUT,CMD_PA_BASE + 22, ��)
	MESSAGE_ADD(	CMD_PA_LOGOUT,					CMD_PA_BASE + 23, ��)
	MESSAGE_ADD(	CMD_PA_GMBANACCOUNT,			CMD_PA_BASE + 24, GM��ͣ�˺�)//Add by sunny.sun 20090828

	MESSAGE_ADD(	CMD_AP_LOGIN,					CMD_AP_BASE + 2, ��)
	MESSAGE_ADD(	CMD_AP_USER_LOGIN,				CMD_AP_BASE + 3, ��)
	MESSAGE_ADD(	CMD_AP_RELOGIN,					CMD_AP_BASE + 4, ��)
	MESSAGE_ADD(	CMD_AP_KICKUSER,				CMD_AP_BASE	+ 11, ��)
	MESSAGE_ADD(	CMD_AP_USER_LOGIN1,				CMD_AP_BASE + 12, ��)
	MESSAGE_ADD(	CMD_AP_USER_LOGIN2,				CMD_AP_BASE + 13, ��)
	MESSAGE_ADD(	CMD_AP_EXPSCALE,				CMD_AP_BASE + 14, ������)
	MESSAGE_ADD(	CMD_MM_GATE_RELEASE,			CMD_MM_BASE + 1, ��)
	MESSAGE_ADD(	CMD_MM_GATE_CONNECT,			CMD_MM_BASE + 2, ��)
	MESSAGE_ADD(	CMD_MM_QUERY_CHA,				CMD_MM_BASE + 3	,	��ѯ��ɫ������Ϣ)
	MESSAGE_ADD(	CMD_MM_QUERY_CHAITEM,			CMD_MM_BASE + 4	,	��ѯ��ɫ������Ϣ)
	MESSAGE_ADD(	CMD_MM_CALL_CHA,				CMD_MM_BASE + 5	,	����ɫ�ٻ����Լ����)
	MESSAGE_ADD(	CMD_MM_GOTO_CHA,				CMD_MM_BASE + 6	,	���Լ����͵���ɫ���)
	MESSAGE_ADD(	CMD_MM_KICK_CHA,				CMD_MM_BASE + 7	,	����ɫ������)
	MESSAGE_ADD(	CMD_MM_GUILD_REJECT,			CMD_MM_BASE + 8	,	�ܾ����빫������֪ͨ����)
	MESSAGE_ADD(	CMD_MM_GUILD_APPROVE,			CMD_MM_BASE + 9	,	�ܾ����빫������֪ͨ����)
	MESSAGE_ADD(	CMD_MM_GUILD_KICK,				CMD_MM_BASE	+ 10,	�ӹ��������޳�ĳ��)
	MESSAGE_ADD(	CMD_MM_GUILD_DISBAND,			CMD_MM_BASE	+ 11,	��ɢ����)
	MESSAGE_ADD(	CMD_MM_QUERY_CHAPING,			CMD_MM_BASE + 12,	��ѯ��ɫ��GameServer�߼����pingֵ)
	MESSAGE_ADD(	CMD_MM_NOTICE,					CMD_MM_BASE + 13,	ϵͳͨ��)
	MESSAGE_ADD(	CMD_MM_GUILD_MOTTO,				CMD_MM_BASE + 14,	���¹���������)
	MESSAGE_ADD(	CMD_MM_DO_STRING,				CMD_MM_BASE + 15,	ִ�нű�)
	MESSAGE_ADD(	CMD_MM_CHA_NOTICE,				CMD_MM_BASE + 16,	��ָ����ҵ�ϵͳͨ��)
	MESSAGE_ADD(	CMD_MM_LOGIN,					CMD_MM_BASE + 17,	��ҵ�½��Ϸ)
	MESSAGE_ADD(	CMD_MM_GUILD_CHALL_PRIZEMONEY,	CMD_MM_BASE + 18,	�˻�����ս�ķ���)
	MESSAGE_ADD(	CMD_MM_ADDCREDIT,				CMD_MM_BASE + 19,	��������)
	MESSAGE_ADD(	CMD_MM_STORE_BUY,				CMD_MM_BASE + 20,	�����̳���Ʒ)
	MESSAGE_ADD(	CMD_MM_ADDMONEY,				CMD_MM_BASE + 21, ��)
	MESSAGE_ADD(	CMD_MM_AUCTION,					CMD_MM_BASE + 22, ��)
	MESSAGE_ADD(	CMD_MM_NOTICETOCHA,				CMD_MM_BASE + 23,	ˮ������֪ͨ����ͬһ�������Ľ�����)
	MESSAGE_ADD(	CMD_MM_EXCHEXTERNVALUE,			CMD_MM_BASE + 24,	�ű��ı�ȫ�ֱ���)
	MESSAGE_ADD(	CMD_MM_NOTICETRADER,			CMD_MM_BASE + 25,	֪ͨ������)

	// FEATURE: GUILD_BANK
	MESSAGE_ADD(CMD_MM_UPDATEGUILDBANK, CMD_MM_BASE + 26, Update Guild Bank)
	MESSAGE_ADD(CMD_MM_UPDATEGUILDBANKGOLD, CMD_MM_BASE + 27, Update Guild Bank Gold)

	MESSAGE_ADD(	CMD_CP_TEAM_INVITE,			CMD_CP_BASE	+ 1		,	�������)
	MESSAGE_ADD(	CMD_CP_TEAM_ACCEPT,			CMD_CP_BASE	+ 2		,	ȷ�����)
	MESSAGE_ADD(	CMD_CP_TEAM_REFUSE,			CMD_CP_BASE	+ 3		,	�ܾ����)
	MESSAGE_ADD(	CMD_CP_TEAM_LEAVE,			CMD_CP_BASE	+ 4		,	�뿪���)
	MESSAGE_ADD(	CMD_CP_TEAM_KICK,			CMD_CP_BASE + 5		,	�߳����)
	MESSAGE_ADD(	CMD_CP_FRND_INVITE,			CMD_CP_BASE	+ 11	,	��������)
	MESSAGE_ADD(	CMD_CP_FRND_ACCEPT,			CMD_CP_BASE	+ 12	,	���ѽ���)
	MESSAGE_ADD(	CMD_CP_FRND_REFUSE,			CMD_CP_BASE	+ 13	,	���Ѿܾ�)
	MESSAGE_ADD(	CMD_CP_FRND_DELETE,			CMD_CP_BASE	+ 14	,	����ɾ��)
	MESSAGE_ADD(	CMD_CP_FRND_MOVE_GROUP,		CMD_CP_BASE	+ 15	,	�ı������������)
	MESSAGE_ADD(	CMD_CP_FRND_REFRESH_INFO,	CMD_CP_BASE	+ 16	,	���Լ��Ŀͻ���ˢ�±��ˣ����ѡ����ѡ�ͬ�š�·�ˡ�İ���ˣ��ĸ�����Ϣ)
	MESSAGE_ADD(	CMD_CP_CHANGE_PERSONINFO,	CMD_CP_BASE	+ 17	,	�ı�����Լ���ͷ��ȸ�����Ϣ)
	MESSAGE_ADD(	CMD_CP_FRND_DEL_GROUP,		CMD_CP_BASE	+ 18, ��)
	MESSAGE_ADD(	CMD_CP_FRND_ADD_GROUP,		CMD_CP_BASE	+ 19, ��)
	MESSAGE_ADD(	CMD_CP_FRND_CHANGE_GROUP,	CMD_CP_BASE	+ 20, ��)
	MESSAGE_ADD(	CMD_CP_QUERY_PERSONINFO,	CMD_CP_BASE	+ 21, ��)
	MESSAGE_ADD(	CMD_CP_PING,					CMD_CP_BASE	+ 25, ��)
	MESSAGE_ADD(	CMD_CP_REPORT_WG,				CMD_CP_BASE + 26,	���淢��ʹ�����)
	MESSAGE_ADD(	CMD_CP_MASTER_REFRESH_INFO,		CMD_CP_BASE	+ 31	,	���Լ��Ŀͻ���ˢ��ʦ���ĸ�����Ϣ)
	MESSAGE_ADD(	CMD_CP_PRENTICE_REFRESH_INFO,	CMD_CP_BASE	+ 32	,	���Լ��Ŀͻ���ˢ��ͽ�ܵĸ�����Ϣ)
	MESSAGE_ADD(	CMD_CP_GUILD_INVITE,			CMD_CP_BASE + 33, ��)
	MESSAGE_ADD(	CMD_CP_GUILD_ACCEPT,			CMD_CP_BASE + 34, ��)
	MESSAGE_ADD(	CMD_CP_GUILD_REFUSE,			CMD_CP_BASE + 35, ��)
	MESSAGE_ADD(	CMD_CP_GM1SAY,				CMD_CP_BASE	+ 400		,	GM��ȫ���㲥)
	MESSAGE_ADD(	CMD_CP_SAY2TRADE,			CMD_CP_BASE	+ 401		,	����Ƶ��)
	MESSAGE_ADD(	CMD_CP_SAY2ALL,				CMD_CP_BASE + 402		,	������Ϸ����㲥)
	MESSAGE_ADD(	CMD_CP_SAY2YOU,				CMD_CP_BASE	+ 403		,	P2P����(��Ҷ����))
	MESSAGE_ADD(	CMD_CP_SAY2TEM,				CMD_CP_BASE	+ 404		,	�������)
	MESSAGE_ADD(	CMD_CP_SAY2GUD,				CMD_CP_BASE	+ 405		,	�л�(Guild)����)
	MESSAGE_ADD(	CMD_CP_SESS_CREATE,			CMD_CP_BASE	+ 406		,	�Ự����)
	MESSAGE_ADD(	CMD_CP_SESS_SAY,			CMD_CP_BASE	+ 407		,	�Ự˵��)
	MESSAGE_ADD(	CMD_CP_SESS_ADD,			CMD_CP_BASE	+ 408		,	�Ự�����Ա)
	MESSAGE_ADD(	CMD_CP_SESS_LEAVE,			CMD_CP_BASE	+ 409		,	�뿪�Ự)
	MESSAGE_ADD(	CMD_CP_REFUSETOME,			CMD_CP_BASE	+ 410		,	�򿪽���˽�Ŀ���)
	MESSAGE_ADD(	CMD_CP_GM1SAY1,				CMD_CP_BASE	+ 411		,	�����㲥)
	MESSAGE_ADD(	CMD_CP_GMSAY,				CMD_CP_BASE	+ 412		,	��������ִ��)

	// FEATURE: GUILD_BANK
	MESSAGE_ADD(CMD_CP_GUILDBANK, CMD_CP_BASE + 414, Guild Bank)
	MESSAGE_ADD(CMD_CP_GUILDATTR, CMD_CP_BASE + 416, Guild Attr)
	MESSAGE_ADD(CMD_CP_GUILDNAMECOLOR, CMD_CP_BASE + 417, Guild Color)


	MESSAGE_ADD(	CMD_PC_TEAM_INVITE,			CMD_PC_BASE	+ 1			,	֪ͨ�ͻ��������������)
	MESSAGE_ADD(	CMD_PC_TEAM_REFRESH,		CMD_PC_BASE	+ 2			,	֪ͨ�ͻ�����ӱ仯����Ϣ��ʹ��CMD_PM_TEAM�����������)
	MESSAGE_ADD(	CMD_PC_TEAM_CANCEL,			CMD_PC_BASE	+ 3			,	��������Ϊ��ʱ���Ա��������ԭ��֪ͨ�����뷽ȡ����������)
	MESSAGE_ADD(	CMD_PC_FRND_INVITE,			CMD_PC_BASE	+ 11, ��)
	MESSAGE_ADD(	CMD_PC_FRND_REFRESH,		CMD_PC_BASE	+ 12, ��)
	MESSAGE_ADD(	CMD_PC_FRND_CANCEL,			CMD_PC_BASE	+ 13, ��)
	MESSAGE_ADD(	CMD_PC_FRND_MOVE_GROUP,		CMD_PC_BASE + 15	,	�ı���ѷ���ɹ�)
	MESSAGE_ADD(	CMD_PC_FRND_REFRESH_INFO,	CMD_PC_BASE	+ 16	,	ˢ�º��Ѹ�����Ϣ)
	MESSAGE_ADD(	CMD_PC_CHANGE_PERSONINFO,	CMD_PC_BASE	+ 17	,	�ı�����Լ���ͷ��ȸ�����Ϣ)
	MESSAGE_ADD(	CMD_PC_FRND_DEL_GROUP,		CMD_PC_BASE + 18	,	ɾ�����ѷ���ɹ�)
	MESSAGE_ADD(	CMD_PC_FRND_ADD_GROUP,		CMD_PC_BASE + 19	,	�������ѷ���ɹ�)
	MESSAGE_ADD(	CMD_PC_FRND_CHANGE_GROUP,	CMD_PC_BASE + 20	,	�ı���ѷ���ɹ�)
	MESSAGE_ADD(	CMD_PC_QUERY_PERSONINFO,	CMD_PC_BASE	+ 21	,	�ı�����Լ���ͷ��ȸ�����Ϣ)
	MESSAGE_ADD(	CMD_PC_PING,				CMD_PC_BASE	+ 25, ��)
	MESSAGE_ADD(	CMD_PC_GUILD,				CMD_PC_BASE	+ 30, ��)
	MESSAGE_ADD(	CMD_PC_GUILD_INVITE,		CMD_PC_BASE	+ 31, ��)
	MESSAGE_ADD(	CMD_PC_GUILD_CANCEL,		CMD_PC_BASE	+ 33, ��)
	MESSAGE_ADD(	CMD_PC_MASTER_REFRESH,	CMD_PC_BASE	+ 41, ��)
	MESSAGE_ADD(	CMD_PC_MASTER_CANCEL,	CMD_PC_BASE	+ 42, ��)
	MESSAGE_ADD(	CMD_PC_MASTER_REFRESH_INFO,	CMD_PC_BASE	+ 43	,	ˢ��ʦ��������Ϣ)
	MESSAGE_ADD(	CMD_PC_PRENTICE_REFRESH_INFO,CMD_PC_BASE	+ 44	,	ˢ��ͽ�ܸ�����Ϣ)
	MESSAGE_ADD(	CMD_PC_GM1SAY,				CMD_PC_BASE	+ 400		,	GM��ȫ���㲥)
	MESSAGE_ADD(	CMD_PC_SAY2TRADE,			CMD_PC_BASE	+ 401		,	����Ƶ��)
	MESSAGE_ADD(	CMD_PC_SAY2ALL,				CMD_PC_BASE + 402		,	������Ϸ����㲥)
	MESSAGE_ADD(	CMD_PC_SAY2YOU,				CMD_PC_BASE	+ 403		,	P2P����(��Ҷ����))
	MESSAGE_ADD(	CMD_PC_SAY2TEM,				CMD_PC_BASE	+ 404		,	�������)
	MESSAGE_ADD(	CMD_PC_SAY2GUD,				CMD_PC_BASE	+ 405		,	�л�(Guild)����)
	MESSAGE_ADD(	CMD_PC_SESS_CREATE,			CMD_PC_BASE	+ 406		,	�Ự����)
	MESSAGE_ADD(	CMD_PC_SESS_SAY,			CMD_PC_BASE	+ 407		,	�Ự˵��)
	MESSAGE_ADD(	CMD_PC_SESS_ADD,			CMD_PC_BASE	+ 408		,	�Ự�����Ա)
	MESSAGE_ADD(	CMD_PC_SESS_LEAVE,			CMD_PC_BASE	+ 409		,	�뿪�Ự)
	MESSAGE_ADD(	CMD_PC_GM1SAY1,				CMD_PC_BASE	+ 411		,	�����㲥)

	// FEATURE: GUILD_BANK
	MESSAGE_ADD(CMD_PC_ERRMSG, CMD_PC_BASE + 414, Error Message)
	MESSAGE_ADD(CMD_PC_UPDATEGUILDATTR, CMD_PC_BASE + 416, Guild Perm)
	MESSAGE_ADD(CMD_PC_GUILDNAMECOLOR, CMD_PC_BASE + 417, Guild Color)

	MESSAGE_ADD(	CMD_PC_GARNER2_ORDER,		CMD_PC_BASE +101, ��)
	MESSAGE_ADD(	CMD_OS_LOGIN,				CMD_OS_BASE + 1			,	��¼)
	MESSAGE_ADD(	CMD_OS_PING,				CMD_OS_BASE + 2			,	�������)
	MESSAGE_ADD(	CMD_SO_LOGIN,				CMD_SO_BASE + 1			,	��¼����)
	MESSAGE_ADD(	CMD_SO_PING,				CMD_SO_BASE + 2			,	�������)
	MESSAGE_ADD(	CMD_SO_WARING,				CMD_SO_BASE + 3			,	��������)
	MESSAGE_ADD(	CMD_SO_EXCEPTION,			CMD_SO_BASE + 4		,	�쳣����)
	MESSAGE_ADD(	CMD_SO_ON_LINE,				CMD_SO_BASE + 5			,	��������)
	MESSAGE_ADD(	CMD_SO_OFF_LINE,			CMD_SO_BASE + 6			,	��������)
	MESSAGE_ADD(	CMD_SO_ENTER_MAP,			CMD_SO_BASE + 7			,	�����ͼ����)
	MESSAGE_ADD(	CMD_SO_LEAVE_MAP,			CMD_SO_BASE + 8			,	�뿪��ͼ����)
	MESSAGE_ADD(	CMD_AB_LOGIN,				CMD_AB_BASE + 1			,	��¼)
	MESSAGE_ADD(	CMD_AB_LOGOUT,				CMD_AB_BASE + 2			,	�ǳ�)
	MESSAGE_ADD(	CMD_AB_USER_LOGINT,			CMD_AB_BASE + 3			,	��ҵ�¼)
	MESSAGE_ADD(	CMD_AB_USER_LOGOUT,			CMD_AB_BASE + 4			,	��ҵǳ�)
	MESSAGE_ADD(	CMD_BA_USER_TIME,			CMD_BA_BASE + 1			,	���ʱ��֪ͨ)
	MESSAGE_ADD(	CMD_BA_KILL_USER,			CMD_BA_BASE + 2			,	�ߵ���ң�Ŀǰ�������1.����ʱ����Ѿ�����5Сʱ����ʱ��2.�����֤ʧ�ܣ�)
}

inline static void ClearMessage()
{
	MsgMap.clear();
}

#endif
