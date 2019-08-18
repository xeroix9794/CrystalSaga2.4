#pragma once

#include <string>

#define MAX_GATE 5
#define MAX_MAP  100
#define MAX_MAPNAME_LENGTH	32

class CGameConfig
{

public:	
	
    CGameConfig();
	
	bool	Load(char *pszFileName);
	void	SetDefault();
	
public:

	// ��ע��˴���Ҫ�����κηǹ̶��ڴ�ߴ�ı���, ����ģ��, string֮���,
	// ���ཫ��ֱ��д�̺Ͷ���
	char    m_szGateIP[MAX_GATE][64];  // GateIP��ַ�б�
    int     m_nGatePort[MAX_GATE];     // Gate Port�б�
    int     m_nGateCnt;                // Gate����
    char    m_szTradeIP[64];            // TradeServer IP
    int     m_nTradePort;               // TradeServer Port
	char	m_szTradePwd[33];		   // TradeServer��֤��
	int		m_nSection;				   // С����
    char    m_szMapList[MAX_MAP][MAX_MAPNAME_LENGTH];
	BYTE	m_btMapOK[MAX_MAP];		   // ��ͼ�Ƿ��ʼ���ɹ�
	int     m_nMapCnt;				   // ��ͼ����
	char	m_szEqument[MAX_MAPNAME_LENGTH];   // �����ػ���
    char    m_szName[64];			   // ����������
	char	m_szDBIP[64];			   // DB IP
	char	m_szDBUsr[32];			   // DB �û���
	char	m_szDBPass[32];			   // DB ����
	char	m_szDBName[32];
	// Add by lark.li 20080321 begin
	char	m_szTradeLogDBIP[64];			   // DB IP
	char	m_szTradeLogDBName[32];			   // DB IP
	char	m_szTradeLogDBUsr[32];			   // DB �û���
	char	m_szTradeLogDBPass[32];			   // DB ����

	BOOL	m_bTradeLogIsConfig;
	// End

	long	m_lSocketAlive;            // Socket���ֻ
	int		m_nMaxPly;                 // ��������
	int		m_nMaxShip;                // ���ֻ�� //��������GS��ռ�ý����ڴ棬��cfg����max_ship����(�����������Ĭ��ֵΪ��ǰ��1000) -Waiting Add 2009-03-23
	int		m_nMaxRes;		           // �����Դ�� //��������GS��ռ�ý����ڴ棬��cfg����max_ship����(�����������Ĭ��ֵΪ��ǰ��1000) -Waiting Add 2009-03-23
	int		m_nMaxCha;                 // ����ɫ��
	int		m_nMaxItem;                // ��������
	int		m_nMaxTNpc;                // ���Ի�NPC
	unsigned long	m_ulBaseID;        // ��������ID����
	long	m_lItemShowTime;           // ���ߴ���ʱ��
	long	m_lItemProtTime;           // ���߱���ʱ��
	long	m_lSayInterval;            // �������
	char	m_szResDir[255];		   // ������Դ���ڵ�Ŀ¼
	char	m_szLogDir[255];		   // Log���ڵ�Ŀ¼
	char	m_chMapMask;               // �Ƿ��ȡ���ͼ
	long	m_lDBSave;                 // ���ݿⶨʱ����ʱ����
	long	m_ItemInfoLimit;		
	long	m_CharInfoLimit;		
	long	m_SkilInfoLimit;		 
	long	m_SkillEffLimit;
	long	m_lSafeAdult;              // ������֪ͨʱ����

	BOOL	m_bLogAI;				   // �Ƿ��AI��log
	BOOL	m_bLogCha;				   // �Ƿ�򿪽�ɫ��log
	BOOL	m_bLogCal;				   // �Ƿ����ֵ�����log
	BOOL	m_bLogMission;			   // �Ƿ��Mission��log

	BOOL	m_bSuperCmd;
	BOOL	m_ResetFlag;				//�Ƿ����crystalTrade��
	
	// Add by lark.li 20080731 begin
	vector<int>	m_vGMCmd;
	// End
	
	BOOL	m_bLogDB;				   // �Ƿ�������Ϊ��¼�����ݿ�
};

#define	defCONFIG_FILE_NAME_LEN	512
extern char	szConfigFileN[defCONFIG_FILE_NAME_LEN];
extern CGameConfig g_Config;

