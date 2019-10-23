/**************************************************************************************************************
*
*			Í¨ÐÅÐ­ÒéÃüÁî(cmd)¶¨ÒåÎÄ¼þ											(Created by Andor.Zhang in 2004.11)
*
**************************************************************************************************************/
#ifndef NETCOMMAND_H
#define NETCOMMAND_H

#define NET_PROTOCOL_ENCRYPT   1	// ÍøÂçÐ­ÒéÊÇ·ñ¼ÓÃÜ£¨Õë¶ÔÍâ¹ÒµÄÐÞ¸Ä£©
#define CMD_INVALID		       0	//ÎÞÐ§ÃüÁî
//£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½
//                 Ð­ÒéÃüÁî¶¨Òå(16bit)
//			ÃüÁî¶¨Òå´«Êä·½Ïò±êÊ¶(ÈçCMD_CM_XXXÖÐµÄCM)£º
//				CM	(C)lient		->Ga(m)eServer
//				MC	Ga(m)eServer	->(C)lient
//						......
//				(ÆäËü¼û"Ð­ÒéÃüÁî»ùÊý¶¨Òå"²¿·Ö)
//	(×¢£ºÃ¿¸ö·½ÏòµÄÃüÁî¿Õ¼äÇëÔ¤Áô500¸ö£¬²¢ÇÒÃ¿¸öÇø¼ä´Ó1¿ªÊ¼£¬ÈçCMD_CM_BASE+1¿ªÊ¼¶¨Òå)
//	¶þ¼¶×ÓÐ­Òé¶¨ÒåÍÆ¼öÊ¹ÓÃ¡°MSG_¡±¿ªÊ¼£¬Èç£ºMSG_TEAM_CANCLE_TIMEOUT;¿í¶ÈÎª1×Ö½Ú(8bit)£¬Ê¹ÓÃWriteChar/ReadChar²Ù×÷
//£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½£½
/*=====================================================================================================
*					Ð­ÒéÃüÁî»ùÊý¶¨Òå£º
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
MESSAGE_DEF(	CMD_CM_ROLEBASE,				CMD_CM_BASE + 300,	(300-400)ÈÎÎñÏµÍ³ÃüÁî»ùÊý)
MESSAGE_DEF(	CMD_CM_GULDBASE,				CMD_CM_BASE	+ 400,	(400-430)¹«»áÃüÁî»ùÖ·)
MESSAGE_DEF(	CMD_CM_CHARBASE,				CMD_CM_BASE	+ 430,	(430-450)½ÇÉ«ÃüÁî»ùÊý)

MESSAGE_DEF(	CMD_CM_SAY,						CMD_CM_BASE + 1,	ÊÓÒ°ÄÚËµ»°)
MESSAGE_DEF(	CMD_CM_BEGINACTION,				CMD_CM_BASE + 6,	ÎÞ)
	//char	ÐÐ¶¯ÀàÐÍ£¨1£¬ÒÆ¶¯£»2£¬Ê¹ÓÃ¼¼ÄÜ£»3£¬»»×°£»4£¬¿¿ÒÎ×Ó£»5£¬ÉÏ´¬£©
	//	ÀàÐÍ1£ºÒÆ¶¯
	//		long	Æ½¾ùpingÖµ
	//		Point	µãÐòÁÐ£¨°üº¬playerµÄµ±Ç°Î»ÖÃ£©
	//	ÀàÐÍ2£ºÊ¹ÓÃ¼¼ÄÜ
	//		char	¼¼ÄÜ×ÓÀàÐÍ£¨1£¬Ö±½ÓÊ¹ÓÃ£»2£¬ÏÈÒÆ¶¯ÔÙÊ¹ÓÃ£©
	//			ÀàÐÍ1£ºÖ±½ÓÊ¹ÓÃ
	//				long	¼¼ÄÜ±àºÅ£¨0£¬ÎïÀí¹¥»÷£»ÆäËû£¬¼¼ÄÜID£©
	//				¼¼ÄÜ²ÎÊý£¬¸ù¾Ý¼¼ÄÜµÄÀàÐÍ£¨¸öÌå£¬·¶Î§£©Ñ¡ÔñÏÂÁÐ²ÎÊý
	//				long	Ä¿±êID
	//				Point	Ä¿±êÎ»ÖÃ
	//			ÀàÐÍ2£ºÏÈÒÆ¶¯ÔÙÊ¹ÓÃ
	//				long	Æ½¾ùpingÖµ
	//				Point	µãÐòÁÐ£¨°üº¬playerµÄµ±Ç°Î»ÖÃ£©
	//				long	¼¼ÄÜ±àºÅ£¨0£¬ÎïÀí¹¥»÷£»ÆäËû£¬¼¼ÄÜID£©
	//				¼¼ÄÜ²ÎÊý£¬¸ù¾Ý¼¼ÄÜµÄÀàÐÍ£¨¸öÌå£¬·¶Î§£©Ñ¡ÔñÏÂÁÐ²ÎÊý
	//				long	Ä¿±êID
	//				Point	Ä¿±êÎ»ÖÃ
	//	ÀàÐÍ3£º»»×°
	//	ÀàÐÍ4£º
	//	ÀàÐÍ5£ºÉÏ´¬
	//		ulong	´¬µÄID£¬Èç¹ûÎª0£¬±íÊ¾ÉÏ´¬²»³É¹¦
	//	ÀàÐÍ6£ºÏÂ´¬
	//		char	0£¬ÏÂ´¬²»³É¹¦¡£1£¬ÏÂ´¬³É¹¦

MESSAGE_DEF(	CMD_CM_ENDACTION,				CMD_CM_BASE + 7,	ÎÞ)
MESSAGE_DEF(	CMD_CM_SYNATTR,					CMD_CM_BASE + 8,	Í¬²½½ÇÉ«ÊôÐÔ£¨Í¬²½ÊôÐÔµÄ×îÖÕÖµ£©)
MESSAGE_DEF(	CMD_CM_SYNSKILLBAG,				CMD_CM_BASE + 9,	Í¬²½¼¼ÄÜÀ¸)
MESSAGE_DEF(	CMD_CM_DIE_RETURN,				CMD_CM_BASE + 10,	ËÀÍö¸´»î)
MESSAGE_DEF(	CMD_CM_SKILLUPGRADE,			CMD_CM_BASE + 11,	ÎÞ)
MESSAGE_DEF(	CMD_CM_PING,					CMD_CM_BASE + 15,	ÎÞ)
MESSAGE_DEF(	CMD_CM_REFRESH_DATA,			CMD_CM_BASE + 16,	ÎÞ)
MESSAGE_DEF(	CMD_CM_CHECK_PING,				CMD_CM_BASE + 17,	¼ÆËãPing)
MESSAGE_DEF(	CMD_CM_MAP_MASK,				CMD_CM_BASE + 18,	ÎÞ)

MESSAGE_DEF(	CMD_CM_UPDATEHAIR,				CMD_CM_BASE + 20,	¿Í»§¶ËÇëÇó¸ü»»·¢ÐÍ)
MESSAGE_DEF(	CMD_CM_TEAM_FIGHT_ASK,			CMD_CM_BASE + 21,	¶ÓÎéÌôÕ½ÇëÇó)
MESSAGE_DEF(	CMD_CM_TEAM_FIGHT_ASR,			CMD_CM_BASE + 22,	¶ÓÎéÌôÕ½Ó¦´ð(ANSWER))
MESSAGE_DEF(	CMD_CM_ITEM_REPAIR_ASK,			CMD_CM_BASE + 23,	µÀ¾ßÐÞÀíÇëÇó)
MESSAGE_DEF(	CMD_CM_ITEM_REPAIR_ASR,			CMD_CM_BASE + 24,	µÀ¾ßÐÞÀíÓ¦´ð)
MESSAGE_DEF(	CMD_CM_ITEM_FORGE_ASK,			CMD_CM_BASE + 25,	µÀ¾ß¾«Á¶ÇëÇó)
MESSAGE_DEF(	CMD_CM_ITEM_FORGE_ASR,			CMD_CM_BASE + 26,	µÀ¾ß¾«Á¶Ó¦´ð)
MESSAGE_DEF(	CMD_CM_TIGER_START,				CMD_CM_BASE + 27,	¿ªÆôÀÏ»¢»ú)
MESSAGE_DEF(	CMD_CM_TIGER_STOP,				CMD_CM_BASE + 28,	Í£Ö¹ÀÏ»¢»ú)
//ÐÐ¶¯
MESSAGE_DEF(	CMD_CM_ITEM_FORGE_CANACTION,	CMD_CM_BASE + 29,	ÓÐÌØÊâÈÎÎñÒÑÍê³É£¬¿ÉÒÔ×ö±ðµÄÊÂ¡£)
//±³°üËø¶¨
MESSAGE_DEF(	CMD_CM_KITBAG_LOCK,				CMD_CM_BASE + 31,	±³°üËø¶¨ÇëÇó)
MESSAGE_DEF(	CMD_CM_KITBAG_UNLOCK,			CMD_CM_BASE + 32,	±³°ü½âËøÇëÇó)
MESSAGE_DEF(	CMD_CM_KITBAG_CHECK,			CMD_CM_BASE + 33,	¼ì²é±³°üËø¶¨×´Ì¬)
MESSAGE_DEF(	CMD_CM_KITBAG_AUTOLOCK,			CMD_CM_BASE + 34,	ÏÂÏß×Ô¶¯Ëø¶¨)
MESSAGE_DEF(	CMD_CM_KITBAGTEMP_SYNC,			CMD_CM_BASE + 35,	Í¬²½ÁÙÊ±±³°ü)

//ÉÌ³ÇÏµÍ³
MESSAGE_DEF(	CMD_CM_STORE_OPEN_ASK,			CMD_CM_BASE + 41,	´ò¿ªÉÌ³Ç)
MESSAGE_DEF(	CMD_CM_STORE_LIST_ASK,			CMD_CM_BASE + 42,	ÉÌ³ÇÁÐ±í)
MESSAGE_DEF(	CMD_CM_STORE_BUY_ASK,			CMD_CM_BASE + 43,	¹ºÂòµÀ¾ß)
MESSAGE_DEF(	CMD_CM_STORE_CHANGE_ASK,		CMD_CM_BASE + 44,	¶Ò»»´ú±Ò)
MESSAGE_DEF(	CMD_CM_STORE_QUERY,				CMD_CM_BASE + 45,	½»Ò×¼ÇÂ¼²éÑ¯)
MESSAGE_DEF(	CMD_CM_STORE_VIP,				CMD_CM_BASE + 46,	VIP)
MESSAGE_DEF(	CMD_CM_STORE_AFFICHE,			CMD_CM_BASE + 47,	¹«¸æÉÌÆ·)
MESSAGE_DEF(	CMD_CM_STORE_CLOSE,				CMD_CM_BASE + 48,	ÎÞ)

//ºÚÊÐÉÌÈË
MESSAGE_DEF(	CMD_CM_BLACKMARKET_EXCHANGE_REQ,CMD_CM_BASE + 51,	ºÚÊÐ¶Ò»»)
MESSAGE_DEF(	CMD_CM_CHEAT_CHECK,				CMD_CM_BASE + 52,	ÎÞ)

//ÀÏÊÖ´øÐÂÊÖ
MESSAGE_DEF(	CMD_CM_VOLUNTER_LIST,			CMD_CM_BASE + 61,	ÏÔÊ¾Ö¾Ô¸ÕßÁÐ±í)
MESSAGE_DEF(	CMD_CM_VOLUNTER_ADD,			CMD_CM_BASE + 62,	Ìí¼Óµ½Ö¾Ô¸ÕßÁÐ±í)
MESSAGE_DEF(	CMD_CM_VOLUNTER_DEL,			CMD_CM_BASE + 63,	´ÓÁÐ±íÖÐÉ¾³ý)
MESSAGE_DEF(	CMD_CM_VOLUNTER_SEL,			CMD_CM_BASE + 64,	ÉêÇë×é¶Ó)
MESSAGE_DEF(	CMD_CM_VOLUNTER_OPEN,			CMD_CM_BASE + 65,	´ò¿ªÖ¾Ô¸ÕßÃæ°å)
MESSAGE_DEF(	CMD_CM_VOLUNTER_ASR,			CMD_CM_BASE + 66,	Ö¾Ô¸Õß×é¶ÓÓ¦´ð)

//°ÝÊ¦
MESSAGE_DEF(	CMD_CM_MASTER_INVITE,			CMD_CM_BASE + 71,	°ÝÊ¦)
MESSAGE_DEF(	CMD_CM_MASTER_ASR,				CMD_CM_BASE + 72,	ÎÞ)
MESSAGE_DEF(	CMD_CM_MASTER_DEL,				CMD_CM_BASE + 73,	ÎÞ)
MESSAGE_DEF(	CMD_CM_PRENTICE_DEL,			CMD_CM_BASE + 74,	ÎÞ)
MESSAGE_DEF(	CMD_CM_PRENTICE_INVITE,			CMD_CM_BASE + 75,	ÎÞ)
MESSAGE_DEF(	CMD_CM_PRENTICE_ASR,			CMD_CM_BASE + 76,	ÎÞ)

//Éú»î¼¼ÄÜ
MESSAGE_DEF(	CMD_CM_LIFESKILL_ASR,			CMD_CM_BASE + 80,	Éú»î¼¼ÄÜÓÃÔË£¬ºÏ³É£¬·Ö½âµÈµÄÇëÇó)
MESSAGE_DEF(	CMD_CM_LIFESKILL_ASK,			CMD_CM_BASE + 81,	Éú»î¼¼ÄÜÓÃÔË£¬ºÏ³É£¬·Ö½âµÈµÄÈ·ÈÏ)

MESSAGE_DEF(	CMD_CM_BIDUP,					CMD_CM_BASE + 86,	ÎÞ)

MESSAGE_DEF(	CMD_CM_SAY2CAMP,				CMD_CM_BASE + 91,	ÎÞ)
MESSAGE_DEF(	CMD_CM_GM_SEND,					CMD_CM_BASE + 92,	ÎÞ)
MESSAGE_DEF(	CMD_CM_GM_RECV,					CMD_CM_BASE + 93,	ÎÞ)
MESSAGE_DEF(	CMD_CM_PK_CTRL,					CMD_CM_BASE + 94,	ÎÞ)

// Add by lark.li 20080514 begin
MESSAGE_DEF(	CMD_CM_ITEM_LOTTERY_ASK,		CMD_CM_BASE + 95,	µÀ¾ß¾«Á¶ÇëÇó)
MESSAGE_DEF(	CMD_CM_ITEM_LOTTERY_ASR,		CMD_CM_BASE + 96,	²ÊÆ±Éè¶¨Ó¦´ð)
// End

// Add by lark.li 20080707 begin
MESSAGE_DEF(	CMD_CM_CAPTAIN_CONFIRM_ASR,		CMD_CM_BASE + 97,	¾º¼¼³¡¶Ó³¤Ó¦´ð)
// End

MESSAGE_DEF(	CMD_CM_ITEM_AMPHITHEATER_ASK,	CMD_CM_BASE + 98,	Í¶Æ±·¢ËÍ)

//	2008-8-29	yangyinyu	add	begin!
MESSAGE_DEF(	CMD_CM_ITEM_LOCK_ASK,			CMD_CM_BASE	+ 99,	ÎÞ)
//	2008-8-29	yangyinyu	add	end!
MESSAGE_DEF(	CMD_CM_ITEM_UNLOCK_ASK,			CMD_CM_BASE + 100,	ÎÞ)

MESSAGE_DEF(    CMD_CM_SYNCHAR, CMD_CM_BASE + 101, SynChar)
MESSAGE_DEF(CMD_CM_GETORIGNAMECOLOR, CMD_CM_BASE + 102, GetOrigcolor)
MESSAGE_DEF(CMD_CM_CHANGETITLE, CMD_CM_BASE + 103, SetTitle)
MESSAGE_DEF(CMD_CM_SYNGUILDCOLOR, CMD_CM_BASE + 104, SynGuildColor)
//Begin ¹«»áÃüÁî¶¨Òå
	MESSAGE_DEF(	CMD_CM_GUILD_PUTNAME,		CMD_CM_GULDBASE	+ 1,	¿Í»§¶ËÊäÈë¹«»áÃû)
	MESSAGE_DEF(	CMD_CM_GUILD_TRYFOR,		CMD_CM_GULDBASE	+ 2,	ÉêÇë¼ÓÈë¹«»á)
	MESSAGE_DEF(	CMD_CM_GUILD_TRYFORCFM,		CMD_CM_GULDBASE	+ 3,	È·ÈÏÊÇ·ñ¸²¸ÇÒÔÇ°ÉêÇë)
	MESSAGE_DEF(	CMD_CM_GUILD_LISTTRYPLAYER,	CMD_CM_GULDBASE	+ 4,	ÁÐ³öËùÓÐ´ýÅú¸´Íæ¼Ò)

	MESSAGE_DEF(	CMD_CM_GUILD_APPROVE,		CMD_CM_GULDBASE	+ 5,	ÃÅÖíÅú×¼ÉêÇëÈË¼ÓÈë)
	MESSAGE_DEF(	CMD_CM_GUILD_REJECT,		CMD_CM_GULDBASE	+ 6,	ÃÅÖí¾Ü¾øÉêÇëÈË¼ÓÈë)
	MESSAGE_DEF(	CMD_CM_GUILD_KICK,			CMD_CM_GULDBASE	+ 7,	ÎÞ)
	MESSAGE_DEF(	CMD_CM_GUILD_LEAVE,			CMD_CM_GULDBASE	+ 8,	ÎÞ)
	MESSAGE_DEF(	CMD_CM_GUILD_DISBAND,		CMD_CM_GULDBASE	+ 9,	ÎÞ)
	MESSAGE_DEF(	CMD_CM_GUILD_MOTTO,			CMD_CM_GULDBASE	+ 10,	ÎÞ)
	MESSAGE_DEF(	CMD_CM_GUILD_CHALLENGE,		CMD_CM_GULDBASE + 11,	ÌôÕ½¹«»á)
	MESSAGE_DEF(	CMD_CM_GUILD_LEIZHU,		CMD_CM_GULDBASE + 12,	ÇÀÀÞÖ÷)
	MESSAGE_DEF(	CMD_CM_GUILD_PERM,			CMD_CM_GULDBASE + 13,	Guild Perm)
//End ¹«»áÃüÁî¶¨Òå

//Begin ½ÇÉ«²Ù×÷ºÍµÇÂ¼ÃüÁîCMD_CM_CHARBASE
	MESSAGE_DEF(	CMD_CM_LOGIN,				CMD_CM_CHARBASE + 1,	ÓÃ»§ÕÊºÅµÇÈë(ÓÃ»§Ãû/ÃÜÂë¶ÔÈÏÖ¤))
	MESSAGE_DEF(	CMD_CM_LOGOUT,				CMD_CM_CHARBASE + 2,	ÓÃ»§ÕÊºÅµÇ³ö)
	MESSAGE_DEF(	CMD_CM_BGNPLAY,				CMD_CM_CHARBASE + 3,	ÓÃ»§Ñ¡Ôñ½ÇÉ«¿ªÊ¼ÍæÓÎÏ·)
	MESSAGE_DEF(	CMD_CM_ENDPLAY,				CMD_CM_CHARBASE + 4,	ÓÃ»§½áÊøÍæÓÎÏ·½øÈëÑ¡/½¨/É¾½ÇÉ«»­Ãæ)
	MESSAGE_DEF(	CMD_CM_NEWCHA,				CMD_CM_CHARBASE	+ 5,	ÓÃ»§ÐÂ½¨½ÇÉ«)
	MESSAGE_DEF(	CMD_CM_DELCHA,				CMD_CM_CHARBASE	+ 6,	ÓÃ»§É¾³ý½ÇÉ«)
	MESSAGE_DEF(	CMD_CM_CANCELEXIT,			CMD_CM_CHARBASE + 7,	ÓÃ»§ÒªÇóÈ¡ÏûÍË³ö)
//End ½ÇÉ«²Ù×÷ºÍµÇÂ¼ÃüÁî

//BeginÈÎÎñÏµÍ³Ö¸Áî
	MESSAGE_DEF(	CMD_CM_REQUESTNPC,			CMD_CM_ROLEBASE + 1,	¿Í»§¶ËÇëÇóºÍNPC»¥¶¯)
	MESSAGE_DEF(	CMD_CM_REQUESTTALK,			CMD_CM_ROLEBASE + 1,	¿Í»§¶ËÇëÇóÓëNPC¶Ô»°ÃüÁî)
	MESSAGE_DEF(	CMD_CM_TALKPAGE,			CMD_CM_ROLEBASE + 2,	¿Í»§¶ËÇëÇóÓëNPC¶Ô»°Ò³ÃüÁî)
	MESSAGE_DEF(	CMD_CM_FUNCITEM,			CMD_CM_ROLEBASE + 3,	¿Í»§¶ËÇëÇóÖ¸¶¨¹¦ÄÜÑ¡Ïî²Ù×÷)

	MESSAGE_DEF(	CMD_CM_REQUESTTRADE,		CMD_CM_ROLEBASE + 8,	¿Í»§¶ËÇëÇó½øÐÐ½»Ò×)
	MESSAGE_DEF(	CMD_CM_TRADEITEM,			CMD_CM_ROLEBASE + 9,	¿Í»§¶ËÇëÇó½»Ò×ÎïÆ·)
	MESSAGE_DEF(	CMD_CM_REQUESTAGENCY,		CMD_CM_ROLEBASE + 10,	¿Í»§¶ËÇëÇónpc´úÀí½»Ò×)

	//½ÇÉ«½»Ò×Ö¸Áî
	MESSAGE_DEF(	CMD_CM_CHARTRADE_REQUEST,	CMD_CM_ROLEBASE + 12,	·¢Æð½»Ò×ÑûÇë)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_ACCEPT,	CMD_CM_ROLEBASE + 13,	½ÓÊÜ½»Ò×ÑûÇë)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_REJECT,	CMD_CM_ROLEBASE + 14,	¾Ü¾ø½»Ò×ÑûÇë)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_CANCEL,	CMD_CM_ROLEBASE + 15,	È¡Ïû½»Ò×)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_ITEM,		CMD_CM_ROLEBASE + 16,	½ÇÉ«½»Ò×ÎïÆ·)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_VALIDATEDATA,CMD_CM_ROLEBASE + 17,	È·ÈÏ½»Ò×Êý¾Ý)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_VALIDATE,	CMD_CM_ROLEBASE + 18,	È·ÈÏ½»Ò×)
	MESSAGE_DEF(	CMD_CM_CHARTRADE_MONEY,		CMD_CM_ROLEBASE + 19,	ÍÏ¶¯½ðÇ®)

	// ÈÎÎñ
	MESSAGE_DEF(	CMD_CM_MISSION,				CMD_CM_ROLEBASE + 22,	¿Í»§¶ËÇëÇóÈÎÎñ´¦Àí)
	MESSAGE_DEF(	CMD_CM_MISSIONLIST,			CMD_CM_ROLEBASE + 23,	ÈÎÎñÁÐ±í)
	MESSAGE_DEF(	CMD_CM_MISSIONTALK,			CMD_CM_ROLEBASE + 24,	ÈÎÎñ¶Ô»°)
	MESSAGE_DEF(	CMD_CM_MISLOG,				CMD_CM_ROLEBASE + 25,	ÈÎÎñÈÕÖ¾)
	MESSAGE_DEF(	CMD_CM_MISLOGINFO,			CMD_CM_ROLEBASE + 26,	µ¥¸öÈÎÎñÏêÏ¸ÐÅÏ¢)
	MESSAGE_DEF(	CMD_CM_MISLOG_CLEAR,		CMD_CM_ROLEBASE + 27,	Ç×ÇóÉ¾³ýÒ»¸öÈÎÎñ)
	
	// °ÚÌ¯
	MESSAGE_DEF(	CMD_CM_STALL_ALLDATA,		CMD_CM_ROLEBASE + 30,	ÇëÇó°ÚÌ¯Êý¾ÝÐÅÏ¢)
	MESSAGE_DEF(	CMD_CM_STALL_OPEN,			CMD_CM_ROLEBASE + 31,	ÇëÇó´ò¿ª°ÚÌ¯»õÎïÐÅÏ¢)
	MESSAGE_DEF(	CMD_CM_STALL_BUY,			CMD_CM_ROLEBASE + 32,	ÇëÇó¹ºÂò°ÚÌ¯»õÎï)
	MESSAGE_DEF(	CMD_CM_STALL_CLOSE,			CMD_CM_ROLEBASE + 33,	ÇëÇóÊÕÌ¯)

	// ¾«Á·
// 	MESSAGE_DEF(	CMD_CM_FORGE			CMD_CM_ROLEBASE + 35 // ÇëÇó¾«Á·ÎïÆ· //ÒÑÎÞÓÃ -Waiting Mark 2009-03-25

    
	// ´¬Ö»
	MESSAGE_DEF(	CMD_CM_CREATE_BOAT,			CMD_CM_ROLEBASE + 38,	ÇëÇó´´½¨´¬)
	MESSAGE_DEF(	CMD_CM_UPDATEBOAT_PART,		CMD_CM_ROLEBASE + 39,	ÇëÇó¸ü»»´¬Ö»²¿¼þ)
	MESSAGE_DEF(	CMD_CM_BOAT_CANCEL,			CMD_CM_ROLEBASE + 40,	ÇëÇóÈ¡Ïû½¨Ôì´¬Ö»)
	MESSAGE_DEF(	CMD_CM_BOAT_LUANCH,			CMD_CM_ROLEBASE + 41,	Ñ¡Ôñ´¬Ö»³öº£)
	MESSAGE_DEF(	CMD_CM_BOAT_BAGSEL,			CMD_CM_ROLEBASE + 42,	Ñ¡ÔñÒ»ËÒ´¬Ö»»õ²Õ´ò°üÏÖÓÐ»õÎï)
	MESSAGE_DEF(	CMD_CM_BOAT_SELECT,			CMD_CM_ROLEBASE + 43,	Ñ¡Ôñ´¬Ö»)
	MESSAGE_DEF(	CMD_CM_BOAT_GETINFO,		CMD_CM_ROLEBASE + 44,	»ñÈ¡´¬Ö»ÐÅÏ¢)

	// ÊµÌå
	MESSAGE_DEF(	CMD_CM_ENTITY_EVENT,		CMD_CM_ROLEBASE + 45,	´¥·¢ÊµÌåÊÂ¼þ)

	// ¶þ´ÎÃÜÂëÐ­Òé
	MESSAGE_DEF(	CMD_CM_CREATE_PASSWORD2,	CMD_CM_ROLEBASE + 46,	´´½¨¶þ´ÎÃÜÂë)
	MESSAGE_DEF(	CMD_CM_UPDATE_PASSWORD2,	CMD_CM_ROLEBASE + 47,	¸üÐÂ¶þ´ÎÃÜÂë)

    //¶ÁÊé
    MESSAGE_DEF(	CMD_CM_READBOOK_START,		CMD_CM_ROLEBASE + 48,	¿ªÊ¼¶ÁÊé)
	MESSAGE_DEF(	CMD_CM_READBOOK_CLOSE,		CMD_CM_ROLEBASE + 49,	½áÊø¶ÁÊé)

	MESSAGE_DEF(	CMD_CM_SELECT_TIME_ASK,		CMD_CM_ROLEBASE + 50,	Ñ¡ÔñµÄÀëÏßÊ±¼ä)
	MESSAGE_DEF(	CMD_CM_BEHINDGENIUS_ASK,	CMD_CM_ROLEBASE + 51,	ÇëÇóÖÆÔì±³ºóÁé)
	MESSAGE_DEF(	CMD_CM_GET_OUTTIMEEXP,		CMD_CM_ROLEBASE + 52,	ÎÞ)
	MESSAGE_DEF(	CMD_CM_SELECT_COIN_ASK,		CMD_CM_ROLEBASE + 53,	ÎÞ)
	MESSAGE_DEF(	CMD_CM_REQUEST_ANYTIMETRADE,CMD_CM_ROLEBASE + 54,	ËæÊ±½»Ò×)
	MESSAGE_DEF(	CMD_CM_REQUEST_JEWELRYUP_ASK,CMD_CM_ROLEBASE + 55,	±¦Ê¯Éý¼¶)

	//Ë®¾§½»Ò×Æ½Ì¨ Add by sunny.sun 20090409 
	MESSAGE_DEF(	CMD_CM_OPEN_CRYSTALTRADE_ASK,CMD_CM_ROLEBASE + 56,	´ò¿ªË®¾§½»Ò×Æ½Ì¨)
	MESSAGE_DEF(	CMD_CM_REQUEST_BUY_ASK,		CMD_CM_ROLEBASE + 57,	ÂòÈë¹Òµ¥)
	MESSAGE_DEF(	CMD_CM_REQUEST_SALE_ASK,	CMD_CM_ROLEBASE + 58,	Âô³ö¹Òµ¥)
	MESSAGE_DEF(	CMD_CM_REQUEST_CANCEL_ASK,	CMD_CM_ROLEBASE + 59,	È¡Ïû¹Òµ¥)
	MESSAGE_DEF(	CMD_CM_REQUEST_GETFLATMONEY,CMD_CM_ROLEBASE + 60,	È¡³öÆ½Ì¨ÕËºÅÉÏµÄÇ®)
	MESSAGE_DEF(	CMD_CM_REQUEST_REFRESH_ASK,	CMD_CM_ROLEBASE + 61,	ÇëÇóË¢ÐÂ)
	MESSAGE_DEF(	CMD_CM_CHA_CONFIRM_CRYSTALTRADE,CMD_CM_ROLEBASE + 62,	Íæ¼ÒÈ·ÈÏ½»Ò×ÐÅÏ¢)
//End ÈÎÎñÏµÍ³Ö¸Áî

//BeginÂÒ¶·°×Òø³ÇÅÅÃûÏµÍ³
MESSAGE_DEF(	CMD_CM_GARNER2_REORDER,			CMD_CM_ROLEBASE + 71,	¿Í»§¶ËÇëÇóÖØÅÅÃû)
//EndÂÒ¶·°×Òø³ÇÅÅÃûÏµÍ³


MESSAGE_DEF(	CMD_CM_ANTIINDULGENCE,			CMD_CM_ROLEBASE + 72,	·À³ÁÃÔÆÆ½â)
//Add by sunny.sun20090728
MESSAGE_DEF(	XTRAP_CMD_STEP_TWO,				CMD_CM_ROLEBASE + 73,	ÎÞ)
MESSAGE_DEF(	XTRAP_MAP_FILE_CHANGE,			CMD_CM_ROLEBASE + 74,	ÎÞ)
MESSAGE_DEF(	CMD_CM_OPEN_CYCELMISSION,		CMD_CM_ROLEBASE + 75,	¹«»áÑ­»·ÈÎÎñ)//Add by sunny.sun 20090831

/*=====================================================================================================
*		Ga(m)eServer -> (C)lient
*/
MESSAGE_DEF(	CMD_MC_ROLEBASE,				CMD_MC_BASE	+ 300,	(300-400)ÈÎÎñÏµÍ³ÃüÁî»ùÖ·)
MESSAGE_DEF(	CMD_MC_GULDBASE,				CMD_MC_BASE	+ 400,	(400-430)¹«»áÃüÁî»ùÖ·)
MESSAGE_DEF(	CMD_MC_CHARBASE,				CMD_MC_BASE	+ 430,	(430-450)½ÇÉ«ÃüÁî»ùÖ·)

MESSAGE_DEF(	CMD_MC_SAY,						CMD_MC_BASE + 1,	ÊÓÒ°ÄÚËµ»°)
MESSAGE_DEF(	CMD_MC_MAPCRASH,				CMD_MC_BASE	+ 3,	ÎÞ)
MESSAGE_DEF(	CMD_MC_CHABEGINSEE,				CMD_MC_BASE + 4,	ÎÞ)
MESSAGE_DEF(	CMD_MC_CHAENDSEE,				CMD_MC_BASE + 5,	ÎÞ)
MESSAGE_DEF(	CMD_MC_ITEMBEGINSEE,			CMD_MC_BASE + 6,	ÎÞ)
MESSAGE_DEF(	CMD_MC_ITEMENDSEE,				CMD_MC_BASE + 7,	ÎÞ)

MESSAGE_DEF(	CMD_MC_NOTIACTION,				CMD_MC_BASE + 8,	ÎÞ)
	//long	½ÇÉ«ID
	//char	ÐÐ¶¯ÀàÐÍ£¨1£¬ÒÆ¶¯£»2£¬Ê¹ÓÃ¼¼ÄÜ£»3£¬¼¼ÄÜÓ°Ïì£©
	//	ÀàÐÍ1£ºÒÆ¶¯
	//		char	×´Ì¬£¨0x00£¬ÒÆ¶¯ÖÐ¡£0x01£¬µ½´ïÄ¿±êµãÍ£Ö¹¡£0x02Óöµ½ÕÏ°­Í£Ö¹¡£0x04£¬¿Í»§¶ËÒªÇóÍ£Ö¹¡£0x08£¬µ½´ïÒªÇóµÄ·¶Î§(Èç¹¥»÷·¶Î§)Í£Ö¹¡££©
	//		Point	µãÐòÁÐ£¨°üº¬playerµÄµ±Ç°Î»ÖÃ£©
	//	ÀàÐÍ2£ºÊ¹ÓÃ¼¼ÄÜ
	//		char	×´Ì¬£¨0x00£¬Ê¹ÓÃÖÐ¡£0x01£¬¶ÔÏóËÀÍö¶øÍ£Ö¹¡£0x02£¬¶ÔÏóÀë¿ªÊ¹ÓÃ·¶Î§¶øÍ£Ö¹¡£0x04£¬´ïµ½ÁËÊ¹ÓÃÄ¿µÄ¶øÍ£Ö¹£¨µ¥»÷£©¡£0x08£¬¿Í»§ÒªÇóÍ£Ö¹¡££©
	//		long	¼¼ÄÜ±àºÅ£¨0£¬ÎïÀí¹¥»÷£»ÆäËû£¬¼¼ÄÜID£©
	//		¼¼ÄÜ²ÎÊý£¬¸ù¾Ý¼¼ÄÜµÄÀàÐÍ£¨¸öÌå£¬·¶Î§£©Ñ¡ÔñÏÂÁÐ²ÎÊý
	//		long	Ä¿±êID
	//		Point	Ä¿±êÎ»ÖÃ
	//	ÀàÐÍ3£º¼¼ÄÜÓ°Ïì
	//		long	Ê¹ÓÃ·½µÄ½ÇÉ«ID
	//		long	Ê¹ÓÃµÄ¼¼ÄÜID
	//		½á¹ûÐòÁÐ
	//		long	Ó°ÏìµÄÊôÐÔID
	//		long	Ó°ÏìµÄÊýÖµ
	//	ÀàÐÍ4£º»»×°
MESSAGE_DEF(	CMD_MC_SYNATTR,					CMD_MC_BASE + 9,	Í¬²½½ÇÉ«ÊôÐÔ£¨Í¬²½ÊôÐÔµÄ×îÖÕÖµ£©)
MESSAGE_DEF(	CMD_MC_SYNSKILLBAG,				CMD_MC_BASE + 10,	Í¬²½¼¼ÄÜÀ¸)
MESSAGE_DEF(	CMD_MC_SYNASKILLSTATE,			CMD_MC_BASE + 11,	Í¬²½½ÇÉ«×´Ì¬)
MESSAGE_DEF(	CMD_MC_PING,					CMD_MC_BASE + 15,	ÎÞ)
MESSAGE_DEF(	CMD_MC_ENTERMAP,				CMD_MC_BASE	+ 16,	GameServerÍ¨ÖªClient½øÈëµØÍ¼£¬¸½´øÍæ¼ÒµÄËùÓÐÐÅÏ¢¼°Æä¸½Êô½á¹¹)
MESSAGE_DEF(	CMD_MC_SYSINFO,					CMD_MC_BASE	+ 17,	ÏµÍ³ÏûÏ¢ÌáÊ¾ÃüÁî£¬ÈçÍæ¼Ò²»ÔÚÏß£¬Äã»¹Ã»ÓÐÍê³ÉÊ²Ã´ÈÎÎñµÈµÈ¡£)
MESSAGE_DEF(	CMD_MC_ALARM,					CMD_MC_BASE	+ 18,	ÎÞ)
MESSAGE_DEF(	CMD_MC_TEAM,					CMD_MC_BASE + 19,	GameServerÍ¨Öª¿Í»§¶ËÄ³¸ö¶ÓÔ±µÄÊôÐÔË¢ÐÂ(±ÈÈçÍâ¹Û, HP,SP))
MESSAGE_DEF(	CMD_MC_FAILEDACTION,			CMD_MC_BASE + 20,	ÇëÇóµÄÐÐ¶¯Ö´ÐÐÊ§°Ü)
MESSAGE_DEF(	CMD_MC_MESSAGE,					CMD_MC_BASE + 21,	ÌáÊ¾ÐÅÏ¢)
MESSAGE_DEF(	CMD_MC_ASTATEBEGINSEE,			CMD_MC_BASE + 22,	µØÃæ×´Ì¬)
MESSAGE_DEF(	CMD_MC_ASTATEENDSEE,			CMD_MC_BASE + 23,	ÎÞ)
MESSAGE_DEF(	CMD_MC_TLEADER_ID,				CMD_MC_BASE + 24,	GameServerÍ¨Öª¿Í»§¶ËÄ³¸ö¶ÓÔ±µÄ¶Ó³¤ÐÅÏ¢)
MESSAGE_DEF(	CMD_MC_CHA_EMOTION,				CMD_MC_BASE + 25,	GameServer´¥·¢Ö¸¶¨½ÇÉ«×ö±íÇé   )

MESSAGE_DEF(	CMD_MC_QUERY_CHA,				CMD_MC_BASE + 26,	²éÑ¯½ÇÉ«»ù±¾ÐÅÏ¢)
MESSAGE_DEF(	CMD_MC_QUERY_CHAITEM,			CMD_MC_BASE + 27,	²éÑ¯½ÇÉ«µÀ¾ß)
MESSAGE_DEF(	CMD_MC_CALL_CHA,				CMD_MC_BASE + 28,	½«½ÇÉ«ÕÙ»½µ½×Ô¼ºÉí±ß)
MESSAGE_DEF(	CMD_MC_GOTO_CHA,				CMD_MC_BASE + 29,	½«×Ô¼º´«ËÍµ½½ÇÉ«Éí±ß)
MESSAGE_DEF(	CMD_MC_KICK_CHA,				CMD_MC_BASE + 30,	½«½ÇÉ«ÌßÏÂÏß)
MESSAGE_DEF(	CMD_MC_SYNDEFAULTSKILL,			CMD_MC_BASE + 31,	Í¬²½Ä¬ÈÏ¼¼ÄÜ)
MESSAGE_DEF(	CMD_MC_ADD_ITEM_CHA,			CMD_MC_BASE + 32,	Ìí¼ÓµÀ¾ß½ÇÉ«)
MESSAGE_DEF(	CMD_MC_DEL_ITEM_CHA,			CMD_MC_BASE + 33,	É¾³ýµÀ¾ß½ÇÉ«)
MESSAGE_DEF(	CMD_MC_QUERY_CHAPING,			CMD_MC_BASE + 34,	²éÑ¯½ÇÉ«µ½GameServerÂß¼­²ãµÄpingÖµ)
MESSAGE_DEF(	CMD_MC_QUERY_RELIVE,			CMD_MC_BASE + 35,	×ÉÑ¯ÊÇ·ñ½ÓÊÜÔ­µØ¸´»î)
MESSAGE_DEF(	CMD_MC_PREMOVE_TIME,			CMD_MC_BASE + 36,	Ô¤ÒÆ¶¯Ê±¼ä)
MESSAGE_DEF(	CMD_MC_CHECK_PING,				CMD_MC_BASE + 37,	¼ÆËãPing)
MESSAGE_DEF(	CMD_MC_MAP_MASK,				CMD_MC_BASE + 38,	ÎÞ)

MESSAGE_DEF(	CMD_MC_OPENHAIR,				CMD_MC_BASE + 39,	´ò¿ªÀí·¢½çÃæ)
MESSAGE_DEF(	CMD_MC_UPDATEHAIR_RES,			CMD_MC_BASE + 40,	¸ü»»·¢ÐÍµÄ½á¹û)
MESSAGE_DEF(	CMD_MC_EVENT_INFO,				CMD_MC_BASE + 41,	Í¬²½ÊÂ¼þÐÅÏ¢)
MESSAGE_DEF(	CMD_MC_SIDE_INFO,				CMD_MC_BASE + 42,	Í¬²½·Ö±ßÐÅÏ¢)
MESSAGE_DEF(	CMD_MC_TEAM_FIGHT_ASK,			CMD_MC_BASE + 43,	¶ÓÎéÌôÕ½ÇëÇó)
MESSAGE_DEF(	CMD_MC_ITEM_REPAIR_ASK,			CMD_MC_BASE + 44,	µÀ¾ßÐÞÀíÇëÇó)
MESSAGE_DEF(	CMD_MC_ITEM_REPAIR_ASR,			CMD_MC_BASE + 45,	µÀ¾ßÐÞÀíÓ¦´ð)
MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_REPAIR,		CMD_MC_BASE + 46,	¿ªÊ¼µÀ¾ßÐÞÀí)
MESSAGE_DEF(	CMD_MC_APPEND_LOOK,				CMD_MC_BASE + 47,	¸½¼ÓÍâ¹Û)
MESSAGE_DEF(	CMD_MC_ITEM_FORGE_ASK,			CMD_MC_BASE + 48,	µÀ¾ß¾«Á¶ÇëÇó)
MESSAGE_DEF(	CMD_MC_ITEM_FORGE_ASR,			CMD_MC_BASE + 49,	µÀ¾ß¾«Á¶Ó¦´ð)
MESSAGE_DEF(	CMD_MC_ITEM_USE_SUC,			CMD_MC_BASE + 50,	µÀ¾ßÊ¹ÓÃ³É¹¦)


MESSAGE_DEF(	CMD_MC_KITBAG_CAPACITY,			CMD_MC_BASE + 51,	±³°üÈÝÁ¿)
MESSAGE_DEF(	CMD_MC_ESPE_ITEM,				CMD_MC_BASE + 52,	ÌØÊâµÀ¾ß)

//±³°üËø¶¨
MESSAGE_DEF(	CMD_MC_KITBAG_CHECK_ASR,		CMD_MC_BASE + 53,	±³°üËø¶¨×´Ì¬µÄÓ¦´ð)
MESSAGE_DEF(	CMD_MC_KITBAGTEMP_SYNC,			CMD_MC_BASE + 54,	Í¬²½ÁÙÊ±±³°ü)

//ÉÌ³ÇÏµÍ³
MESSAGE_DEF(	CMD_MC_STORE_OPEN_ASR,			CMD_MC_BASE + 61,	´ò¿ªÉÌ³Ç)
MESSAGE_DEF(	CMD_MC_STORE_LIST_ASR,			CMD_MC_BASE + 62,	ÉÌ³ÇÁÐ±í)
MESSAGE_DEF(	CMD_MC_STORE_BUY_ASR,			CMD_MC_BASE + 63,	¹ºÂòµÀ¾ß)
MESSAGE_DEF(	CMD_MC_STORE_CHANGE_ASR,		CMD_MC_BASE + 64,	¶Ò»»´ú±Ò)
MESSAGE_DEF(	CMD_MC_STORE_QUERY,				CMD_MC_BASE + 65,	½»Ò×¼ÇÂ¼²éÑ¯)
MESSAGE_DEF(	CMD_MC_STORE_VIP,				CMD_MC_BASE + 66,	VIP)
MESSAGE_DEF(	CMD_MC_STORE_AFFICHE,			CMD_MC_BASE + 67,	¹«¸æÉÌÆ·)
MESSAGE_DEF(	CMD_MC_POPUP_NOTICE,			CMD_MC_BASE + 68,	ÎÞ)

//ºÚÊÐ½»Ò×
MESSAGE_DEF(	CMD_MC_BLACKMARKET_EXCHANGEDATA,	CMD_MC_BASE + 71,	ºÚÊÐ¶Ò»»Êý¾Ý)
MESSAGE_DEF(	CMD_MC_BLACKMARKET_EXCHANGE_ASR,	CMD_MC_BASE + 72,	ºÚÊÐ¶Ò»»Ó¦´ð)
MESSAGE_DEF(	CMD_MC_BLACKMARKET_EXCHANGEUPDATE,	CMD_MC_BASE + 73,	ºÚÊÐ¶Ò»»¸üÐÂ)
MESSAGE_DEF(	CMD_MC_BLACKMARKET_TRADEUPDATE,	CMD_MC_BASE + 74,	ºÚÊÐ½»Ò×¸üÐÂ)
MESSAGE_DEF(	CMD_MC_EXCHANGEDATA,			CMD_MC_BASE + 75,	ÆÕÍ¨¶Ò»»Êý¾Ý)

//ÀÏÊÖ´øÐÂÊÖ
MESSAGE_DEF(	CMD_MC_VOLUNTER_LIST,			CMD_MC_BASE + 81,	ÏÔÊ¾Ö¾Ô¸ÕßÁÐ±í)
MESSAGE_DEF(	CMD_MC_VOLUNTER_STATE,			CMD_MC_BASE + 82,	Ö¾Ô¸Õß×´Ì¬)
MESSAGE_DEF(	CMD_MC_VOLUNTER_SEL,			CMD_MC_BASE + 83,	ÉêÇë×é¶Ó)
MESSAGE_DEF(	CMD_MC_VOLUNTER_OPEN,			CMD_MC_BASE + 84,	´ò¿ªÖ¾Ô¸ÕßÃæ°å)
MESSAGE_DEF(	CMD_MC_VOLUNTER_ASK,			CMD_MC_BASE + 85,	Ö¾Ô¸Õß×é¶ÓÉêÇë)

MESSAGE_DEF(	CMD_MC_LISTAUCTION,				CMD_MC_BASE + 86,	ÎÞ)

//°ÝÊ¦
MESSAGE_DEF(	CMD_MC_MASTER_ASK,				CMD_MC_BASE + 91,	ÎÞ)
MESSAGE_DEF(	CMD_MC_PRENTICE_ASK,			CMD_MC_BASE + 92,	ÎÞ)
MESSAGE_DEF(	CMD_MC_CHAPLAYEFFECT,			CMD_MC_BASE + 93,	ÎÞ)

MESSAGE_DEF(	CMD_MC_SAY2CAMP,				CMD_MC_BASE + 96,	ÎÞ)
MESSAGE_DEF(	CMD_MC_GM_MAIL,					CMD_MC_BASE + 97,	ÎÞ)
MESSAGE_DEF(	CMD_MC_CHEAT_CHECK,				CMD_MC_BASE + 98,	ÎÞ)
MESSAGE_DEF(	CMD_MC_PORTALTIMES﻿,				CMD_MC_BASE + 109, Portal)

//	2008-8-29	yangyinyu	add	begin!
MESSAGE_DEF(	CMD_CM_ITEM_LOCK_ASR,			CMD_MC_BASE	+	99,	ÎÞ)
//	2008-8-29	yangyinyu	add	end!

MESSAGE_DEF(	CMD_MC_ITEM_UNLOCK_ASR,			CMD_MC_BASE	+	95,	ÎÞ)

// FEATURE: GUILD_BANK
MESSAGE_DEF(CMD_MC_UPDATEGUILDBANKGOLD, CMD_MC_BASE + 120, Update Guild Bank Gold)

//Begin ¹«»áÃüÁî¶¨Òå
	MESSAGE_DEF(	CMD_MC_GUILD_GETNAME,		CMD_MC_GULDBASE	+ 1,	ÇëÇó¿Í»§¶ËÊäÈë¹«»áÃû)
	MESSAGE_DEF(	CMD_MC_LISTGUILD,			CMD_MC_GULDBASE	+ 2,	ÃüÁî¿Í»§¶Ë´ò¿ªÉêÇë¼ÓÈë´°¿Ú²¢¼ÓÈëÐÐ»áÁÐ±í)
	MESSAGE_DEF(	CMD_MC_GUILD_TRYFORCFM,		CMD_MC_GULDBASE	+ 3,	È·ÈÏÊÇ·ñ¸²¸ÇÒÔÇ°ÉêÇë)
	MESSAGE_DEF(	CMD_MC_GUILD_LISTTRYPLAYER,	CMD_MC_GULDBASE	+ 4,	ÁÐ³öËùÓÐ´ýÅú¸´Íæ¼Ò)
	MESSAGE_DEF(	CMD_MC_GUILD_LISTCHALL,		CMD_MC_GULDBASE + 5,	²éÑ¯¹«»áÌôÕ½ÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_GUILD_MOTTO,			CMD_MC_GULDBASE	+ 10,	ÐÞ¸Ä×ùÓÒÃú)
	MESSAGE_DEF(	CMD_MC_GUILD_LEAVE,			CMD_MC_GULDBASE + 11,	Àë¿ª¹«»á)
	MESSAGE_DEF(	CMD_MC_GUILD_KICK,			CMD_MC_GULDBASE + 12,	±»Ìß³ö¹«»á)
	MESSAGE_DEF(	CMD_MC_GUILD_INFO,			CMD_MC_GULDBASE + 13,	Í¬²½¹«»áÐÅÏ¢)
//End ¹«»áÃüÁî¶¨Òå

//Begin ½ÇÉ«²Ù×÷ºÍµÇÂ¼ÃüÁîCMD_MC_CHARBASE
	MESSAGE_DEF(	CMD_MC_LOGIN,				CMD_MC_CHARBASE + 1,	ÎÞ)
	MESSAGE_DEF(	CMD_MC_LOGOUT,				CMD_MC_CHARBASE + 2,	ÎÞ)
	MESSAGE_DEF(	CMD_MC_BGNPLAY,				CMD_MC_CHARBASE + 3,	ÎÞ)
	MESSAGE_DEF(	CMD_MC_ENDPLAY,				CMD_MC_CHARBASE + 4,	ÎÞ)
	MESSAGE_DEF(	CMD_MC_NEWCHA,				CMD_MC_CHARBASE	+ 5,	ÎÞ)
	MESSAGE_DEF(	CMD_MC_DELCHA,				CMD_MC_CHARBASE	+ 6,	ÎÞ)
	MESSAGE_DEF(	CMD_MC_STARTEXIT,			CMD_MC_CHARBASE + 7	,	¿ªÊ¼ÍË³öµ¹¼ÆÊ±)
	MESSAGE_DEF(	CMD_MC_CANCELEXIT,			CMD_MC_CHARBASE + 8	,	È¡ÏûÍË³öµ¹¼ÆÊ±)
	MESSAGE_DEF(	CMD_MC_CHAPSTR,				CMD_MC_CHARBASE	+ 10,	ÎÞ)
	MESSAGE_DEF(	CMD_MC_CREATE_PASSWORD2,	CMD_MC_CHARBASE + 11,	ÎÞ)
	MESSAGE_DEF(	CMD_MC_UPDATE_PASSWORD2,	CMD_MC_CHARBASE + 12,	ÎÞ)
	MESSAGE_DEF(    CMD_MC_SYNCCHAR, CMD_MC_CHARBASE + 13, ÎÞ)
	MESSAGE_DEF(CMD_MC_GETORIGCOLOR, CMD_MC_CHARBASE + 14, GetOrigColor)
	MESSAGE_DEF(	CMD_MC_CHANGETITLE, CMD_MC_CHARBASE + 15, Changetitle)
	MESSAGE_DEF(CMD_MC_SYNGUILDCOLOR, CMD_MC_CHARBASE + 16, SynGuildColor)

//End ½ÇÉ«²Ù×÷ºÍµÇÂ¼ÃüÁî

//Begin ÈÎÎñÏµÍ³Ö¸Áî
	MESSAGE_DEF(	CMD_MC_TALKPAGE,			CMD_MC_ROLEBASE + 1,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ¶Ô»°ÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_FUNCPAGE,			CMD_MC_ROLEBASE + 2,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ¹¦ÄÜÑ¡ÏîÒ³ÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_CLOSETALK,			CMD_MC_ROLEBASE + 3,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ¹Ø±Õ¶Ô»°ÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_HELPINFO,			CMD_MC_ROLEBASE + 4,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ°ïÖúÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_TRADEPAGE,			CMD_MC_ROLEBASE + 5,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ½»Ò×ÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_TRADERESULT,			CMD_MC_ROLEBASE + 6,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ½»Ò×ÇëÇó´ð¸´ÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_TRADE_DATA,			CMD_MC_ROLEBASE + 7,	NPC½»Ò×Êý¾ÝÐÅÏ¢¸üÐÂ)
	MESSAGE_DEF(	CMD_MC_TRADE_ALLDATA,		CMD_MC_ROLEBASE + 8,	NPC½»Ò×Êý¾ÝÐÅÏ¢¸üÐÂ)
	MESSAGE_DEF(	CMD_MC_TRADE_UPDATE,		CMD_MC_ROLEBASE + 9,	ºÚÊÐÉÌÈË½»Ò×ÐÅÏ¢¸üÐÂ)
	MESSAGE_DEF(	CMD_MC_EXCHANGE_UPDATE,		CMD_MC_ROLEBASE + 10,	ºÚÊÐÉÌÈË¶Ò»»ÐÅÏ¢¸üÐÂ)

	//½ÇÉ«½»Ò×Ö¸Áî
	MESSAGE_DEF(	CMD_MC_CHARTRADE,			CMD_MC_ROLEBASE + 11,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ½»Ò×ÐÅÏ¢ )
	MESSAGE_DEF(	CMD_MC_CHARTRADE_REQUEST,	CMD_MC_ROLEBASE + 12,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍÆäËû½ÇÉ«ÑûÇë½»Ò×ÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_CHARTRADE_CANCEL,	CMD_MC_ROLEBASE + 13,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ )
	MESSAGE_DEF(	CMD_MC_CHARTRADE_ITEM,		CMD_MC_ROLEBASE + 14,	·þÎñÆ÷Ïò¿Í»§¶Ë½ÇÉ«·¢ËÍ½»Ò×µÄÎïÆ·ÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_CHARTRADE_PAGE,		CMD_MC_ROLEBASE + 15,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ¿ªÊ¼½»Ò×½çÃæÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_CHARTRADE_VALIDATEDATA,	CMD_MC_ROLEBASE + 16,	È·ÈÏ½»Ò×Êý¾Ý)
	MESSAGE_DEF(	CMD_MC_CHARTRADE_VALIDATE,	CMD_MC_ROLEBASE + 17,	È·ÈÏ½»Ò×)
	MESSAGE_DEF(	CMD_MC_CHARTRADE_RESULT,	CMD_MC_ROLEBASE + 18,	È·ÈÏ½»Ò×³É¹¦)
	MESSAGE_DEF(	CMD_MC_CHARTRADE_MONEY,		CMD_MC_ROLEBASE + 19,	ÍÏ¶¯½ðÇ®²Ù×÷)
	MESSAGE_DEF(	CMD_MC_MISSION,				CMD_MC_ROLEBASE + 22,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍÈÎÎñÁÐ±íÒ³ÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_MISSIONLIST,			CMD_MC_ROLEBASE + 23,	ÈÎÎñÁÐ±í)
	MESSAGE_DEF(	CMD_MC_MISSIONTALK,			CMD_MC_ROLEBASE + 24,	ÈÎÎñ¶Ô»°)
	MESSAGE_DEF(	CMD_MC_NPCSTATECHG,			CMD_MC_ROLEBASE + 25,	ÇëÇóÇÐ»»npc×´Ì¬)
	MESSAGE_DEF(	CMD_MC_TRIGGER_ACTION,		CMD_MC_ROLEBASE + 26,	´¥·¢Æ÷¶¯×÷)
	MESSAGE_DEF(	CMD_MC_MISPAGE,				CMD_MC_ROLEBASE + 27,	ÈÎÎñÒ³ÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_MISLOG,				CMD_MC_ROLEBASE + 28,	ÈÎÎñÈÕÖ¾)
	MESSAGE_DEF(	CMD_MC_MISLOGINFO,			CMD_MC_ROLEBASE + 29,	µ¥¸öÈÎÎñÏêÏ¸ÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_MISLOG_CHANGE,		CMD_MC_ROLEBASE + 30,	ÈÎÎñ×´Ì¬±ä»¯)
	MESSAGE_DEF(	CMD_MC_MISLOG_CLEAR,		CMD_MC_ROLEBASE + 31,	Çå³ýÒ»¸öÈÎÎñ)
	MESSAGE_DEF(	CMD_MC_MISLOG_ADD,			CMD_MC_ROLEBASE + 32,	Ìí¼ÓÒ»¸öÈÎÎñ)

	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_FUSION,	CMD_MC_ROLEBASE + 33	,	¿ªÊ¼ÈÛºÏµÀ¾ß)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_UPGRADE,	CMD_MC_ROLEBASE + 34	,	¿ªÊ¼µÀ¾ßÉý¼¶)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_FORGE,	CMD_MC_ROLEBASE + 35	,	¿ªÊ¼µÀ¾ß¾«Á¶)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_UNITE,	CMD_MC_ROLEBASE + 36	,	¿ªÊ¼µÀ¾ßºÏ³É)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_MILLING,	CMD_MC_ROLEBASE + 37	,	¿ªÊ¼µÀ¾ß´òÄ¥)

	MESSAGE_DEF(	CMD_MC_CREATEBOAT,			CMD_MC_ROLEBASE + 38,	´´½¨´¬Ö»)
	MESSAGE_DEF(	CMD_MC_UPDATEBOAT,			CMD_MC_ROLEBASE + 39,	¸ÄÔì´¬Ö»)
	MESSAGE_DEF(	CMD_MC_UPDATEBOAT_PART,		CMD_MC_ROLEBASE + 40,	¸ÄÔì´¬Ö»µÄ²¿¼þ)
	MESSAGE_DEF(	CMD_MC_BERTH_LIST,			CMD_MC_ROLEBASE + 41,	´¬Ö»Í£²´ÁÐ±í)
	MESSAGE_DEF(	CMD_MC_BOAT_LIST,			CMD_MC_ROLEBASE + 42,	Í¬²½½ÇÉ«ÓµÓÐµÄËùÓÐ´¬Ö»ÊôÐÔ)
	MESSAGE_DEF(	CMD_MC_BOAT_ADD,			CMD_MC_ROLEBASE + 43,	Ìí¼ÓÒ»ËÒ´¬)
	MESSAGE_DEF(	CMD_MC_BOAT_CLEAR,			CMD_MC_ROLEBASE + 44,	Çå³ýÒ»ËÒ´¬)
	MESSAGE_DEF(	CMD_MC_BOATINFO,			CMD_MC_ROLEBASE + 45,	Í¬²½´¬Ö»ÊôÐÔÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_BOAT_BAGLIST,		CMD_MC_ROLEBASE + 46,	ÏÔÊ¾¿ÉÒÔ´ò°üµÄ´¬Ö»ÁÐ±í)

	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_EIDOLON_METEMPSYCHOSIS,	CMD_MC_ROLEBASE + 47,	¿ªÊ¼¾«Áé×ªÉú)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_EIDOLON_FUSION,	CMD_MC_ROLEBASE + 48,	¿ªÊ¼¾«ÁéÈÚºÏ)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_PURIFY,	CMD_MC_ROLEBASE + 49	,	¿ªÊ¼×°±¸Ìá´¿)
	
	// ÊÂ¼þÊµÌå½»»¥Ð­ÒéÃüÁî
	MESSAGE_DEF(	CMD_MC_ENTITY_BEGINESEE,	CMD_MC_ROLEBASE + 50,	Í¬²½ÊÓÒ°ÊµÌåÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_ENTITY_ENDSEE,		CMD_MC_ROLEBASE + 51,	ÊµÌåÐÅÏ¢³¬³öÊÓÒ°)
	MESSAGE_DEF(	CMD_MC_ENTITY_CHGSTATE,		CMD_MC_ROLEBASE + 52,	ÊµÌå×´Ì¬¸Ä±ä)

	// °ÚÌ¯
	MESSAGE_DEF(	CMD_MC_STALL_ALLDATA,		CMD_MC_ROLEBASE + 54,	²é¿´°ÚÌ¯ËùÓÐÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_STALL_UPDATE,		CMD_MC_ROLEBASE + 55,	°ÚÌ¯»õÎï¸üÐÂÐÅÏ¢)
	MESSAGE_DEF(	CMD_MC_STALL_DELGOODS,		CMD_MC_ROLEBASE + 56,	É¾³ýÖ¸¶¨Î»ÖÃµÄ»õÎï)
	MESSAGE_DEF(	CMD_MC_STALL_CLOSE,			CMD_MC_ROLEBASE + 57,	Ì¯Ö÷ÊÕÌ¯)
	MESSAGE_DEF(	CMD_MC_STALL_START,			CMD_MC_ROLEBASE + 58,	³É¹¦°ÚÌ¯)
	MESSAGE_DEF(	CMD_MC_STALL_NAME,			CMD_MC_ROLEBASE + 59,	°ÚÌ¯Ãû)

	// ÉÁ¶¯×ÖÌåÐÅÏ¢
	MESSAGE_DEF(	CMD_MC_BICKER_NOTICE,		CMD_MC_ROLEBASE + 60,	ÉÁ¶¯×ÖÌåÐÅÏ¢Í¨Öª)

	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_ENERGY,	CMD_MC_ROLEBASE + 71	,	¿ªÊ¼±´¿Ç³äµç)
	MESSAGE_DEF(	CMD_MC_BEGIN_GET_STONE,		CMD_MC_ROLEBASE + 72	,	¿ªÊ¼ÌáÈ¡±¦Ê¯)
	MESSAGE_DEF(	CMD_MC_BEGIN_TIGER,			CMD_MC_ROLEBASE + 73	,	¾«ÁéÀÏ»¢»ú½çÃæ)
	MESSAGE_DEF(	CMD_MC_TIGER_ITEM_ID,		CMD_MC_ROLEBASE + 74	,	ÀÏ»¢»úµÀ¾ßID)
	MESSAGE_DEF(	CMD_MC_TIGER_STOP,			CMD_MC_ROLEBASE + 75	,	ÀÏ»¢»ú½áÊø)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_FIX,		CMD_MC_ROLEBASE + 76	,	ÐÞÀíÆÆ¹ø)
	MESSAGE_DEF(	CMD_MC_BEGIN_GM_SEND,		CMD_MC_ROLEBASE + 77	,	·¢ËÍGMÓÊ¼þ)
	MESSAGE_DEF(	CMD_MC_BEGIN_GM_RECV,		CMD_MC_ROLEBASE + 78	,	½ÓÊÕGMÓÊ¼þ)

	// Add by lark.li 20080514 begin
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_LOTTERY,	CMD_MC_ROLEBASE + 79	,	¿ªÊ¼ÉèÖÃ²ÊÆ±)
	// End
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_AMPHITHEATER,	CMD_MC_ROLEBASE + 80,	´ò¿ª¾º¼¼³¡ÅÅÐÐ°ñ)
	//Add by sunny.sun 20090306 begin
	MESSAGE_DEF(	CMD_MC_LEAVE_TIME_ASR,		CMD_MC_ROLEBASE + 81,	½ÇÉ«ÀëÏßÊ±¼ä)
	MESSAGE_DEF(	CMD_MC_SELECT_TIME_ASR,		CMD_MC_ROLEBASE + 82,	»ñÈ¡ÀëÏß¾­ÑéÏûÏ¢»Ø¸´)
	MESSAGE_DEF(	CMD_MC_BEHINDGENIUS_ASR,	CMD_MC_ROLEBASE + 83	,	±³ºóÁéÖÆÔìÓ¦´ð)
	MESSAGE_DEF(	CMD_MC_BEGIN_ITEM_JEWELRYUP,CMD_MC_ROLEBASE + 84	,	µ¯¿ª±¦Ê¯Éý¼¶½çÃæ)
	//Add by sunny.sun 20090414
	MESSAGE_DEF(	CMD_MC_OPEN_CRYSTALTRADE_ASR,CMD_MC_ROLEBASE + 85	,	´ò¿ªË®¾§Æ½Ì¨Ó¦´ð)
	MESSAGE_DEF(	CMD_MC_REQUEST_BUY_ASR,		CMD_MC_ROLEBASE + 86 ,	ÂòÈë¹Òµ¥Ó¦´ð)
	MESSAGE_DEF(	CMD_MC_REQUEST_SALE_ASR,	CMD_MC_ROLEBASE + 87 ,	Âô³ö¹Òµ¥Ó¦´ð)
	MESSAGE_DEF(	CMD_MC_REQUEST_CANCEL_ASR,	CMD_MC_ROLEBASE + 88 ,	È¡Ïû¹Òµ¥Ó¦´ð)
	MESSAGE_DEF(	CMD_MC_REQUEST_GETFLATMONEY,CMD_MC_ROLEBASE + 89 ,	È¡³öÆ½Ì¨ÉÏµÄÇ®Ó¦´ð)
	MESSAGE_DEF(	CMD_MC_REQUEST_REFRESH_ASR,	CMD_MC_ROLEBASE + 90 ,	ÇëÇóË¢ÐÂÓ¦´ð)
	MESSAGE_DEF(	CMD_MC_GET_CRYSTALSTATE,	CMD_MC_ROLEBASE + 91 ,	»ñÈ¡½»Ò××´Ì¬)
	//End

//Éú»î¼¼ÄÜ
MESSAGE_DEF(	CMD_MC_LIFESKILL_BGING,			CMD_MC_BASE + 100	,	Éú»î¼¼ÄÜ¿ªÊ¼)
MESSAGE_DEF(	CMD_MC_LIFESKILL_ASR,			CMD_MC_BASE + 101	,	Éú»î¼¼ÄÜÓÃÔË£¬ºÏ³É£¬·Ö½âµÈµÄÇëÇó)
MESSAGE_DEF(	CMD_MC_LIFESKILL_ASK,			CMD_MC_BASE + 102	,	Éú»î¼¼ÄÜÓÃÔË£¬ºÏ³É£¬·Ö½âµÈµÄÈ·ÈÏ)

// Add by lark.li 20080515 begin
MESSAGE_DEF(	CMD_MC_ITEM_LOTTERY_ASK,		CMD_MC_BASE + 103,	²ÊÆ±ÇëÇó)
MESSAGE_DEF(	CMD_MC_ITEM_LOTTERY_ASR,		CMD_MC_BASE + 104,	²ÊÆ±Ó¦´ð)
// End

// Add by lark.li 20080707 begin
MESSAGE_DEF(	CMD_MC_CAPTAIN_ASK,				CMD_MC_BASE + 105,	¾º¼¼³¡¶Ó³¤È·ÈÏÇëÇó)
// End

MESSAGE_DEF(	CMD_MC_ITEM_AMPHITHEATER_ASR,	CMD_MC_BASE + 106, ÎÞ)
MESSAGE_DEF(	CMD_MC_REQUEST_JEWELRYUP_ASR,	CMD_MC_BASE + 107, ÎÞ)

MESSAGE_DEF(	XTRAP_CMD_STEP_ONE,				CMD_MC_BASE + 108, ÎÞ)

//End ÈÎÎñÏµÍ³Ö¸Áî

/*=====================================================================================================
*					Ga(t)eServer <->Ga(m)eServer
*/
MESSAGE_DEF(	CMD_TM_ENTERMAP,				CMD_TM_BASE	+ 3	,	½øµØÍ¼£¬¸½´ø½ÇÉ«ID¡¢½øÈëµØÍ¼µÄÃû×Ö¡¢Î»ÖÃ(x,y))
MESSAGE_DEF(	CMD_TM_GOOUTMAP,				CMD_TM_BASE + 4		,	³öµØÍ¼)
MESSAGE_DEF(	CMD_TM_LOGIN_ACK,				CMD_TM_BASE + 5, ÎÞ)
MESSAGE_DEF(	CMD_TM_CHANGE_PERSONINFO,		CMD_TM_BASE	+6	,	ÐÞ¸Ä¸öÈËÐÅÏ¢)
MESSAGE_DEF(	CMD_TM_MAPENTRY,				CMD_TM_BASE + 7, ÎÞ)
MESSAGE_DEF(	CMD_TM_MAPENTRY_NOMAP,			CMD_TM_BASE + 8, ÎÞ)
MESSAGE_DEF(	CMD_TM_MAP_ADMIN,				CMD_TM_BASE + 9	,	¹ÜÀíÔ±ÓÃµØÍ¼¿ØÖÆÃüÁî)
MESSAGE_DEF(	CMD_TM_STATE,					CMD_TM_BASE + 10,	·þÎñÆ÷µÄ×´Ì¬£¨ÔËÐÐÄÚ´æÇé¿ö£¬°æ±¾£©)
MESSAGE_DEF(	CMD_TM_PORTALTIMES, CMD_TM_BASE + 11, Portal time opening  closing)
//---------------------------------------------------

MESSAGE_DEF(	CMD_MT_LOGIN,					CMD_MT_BASE + 1		,	GameServerµÇÂ½GateServer)
MESSAGE_DEF(	CMD_MT_SWITCHMAP,				CMD_MT_BASE + 4		,	¿ªÊ¼ÇÐ»»µØÍ¼ÃüÁî)
MESSAGE_DEF(	CMD_MT_KICKUSER,				CMD_MT_BASE + 5		,	GameServerÌßµôÄ³¸öÓÃ»§)
MESSAGE_DEF(	CMD_MT_MAPENTRY,				CMD_MT_BASE + 6		,	µØÍ¼Èë¿Ú)
MESSAGE_DEF(	CMD_MT_PALYEREXIT,				CMD_MT_BASE + 8		,	Í¨ÖªGateÊÍ·Å¸ÃÓÃ»§×ÊÔ´)
MESSAGE_DEF(	CMD_MT_MAP_ADMIN,				CMD_MT_BASE + 9	,	¹ÜÀíÔ±ÓÃµØÍ¼¿ØÖÆÃüÁî)
MESSAGE_DEF(	CMD_MT_STATE,					CMD_MT_BASE + 10,	·þÎñÆ÷µÄ×´Ì¬£¨ÔËÐÐÄÚ´æÇé¿ö£¬°æ±¾£©)

/*=====================================================================================================
*					Grou(p)eServer <->Ga(m)eServer 
*/
MESSAGE_DEF(	CMD_PM_TEAM,					CMD_PM_BASE + 1    ,	GroupServer·¢À´µÄ×é¶ÓÐÅÏ¢¸üÐÂ)
//CMD_PM_TEAM×ÓÀàÐÍ¶¨ÒåBegin
	MESSAGE_DEF(	TEAM_MSG_ADD,				1                  ,	×ÓÀàÐÍ: Ôö¼ÓÐÂ¶ÓÔ±)
	MESSAGE_DEF(	TEAM_MSG_LEAVE,				2                  ,	×ÓÀàÐÍ: ¶ÓÔ±Àë¿ª)
	MESSAGE_DEF(	TEAM_MSG_UPDATE,			3                  ,	×ÓÀàÐÍ: Ë¢ÐÂ¶ÓÎé Ö»ÓÃ·þÎñÆ÷¼äÌø×ªÊ±ºòË¢ÐÂµ½GameServerÊ¹ÓÃ)	
	MESSAGE_DEF(	TEAM_MSG_GROUP_ADD,			4                  ,	³ÉÁ¢¶ÓÎé)
	MESSAGE_DEF(	TEAM_MSG_GROUP_BREAK,		5                  ,	½âÉ¢¶ÓÎé)
	MESSAGE_DEF(	TEAM_MSG_KICK,				6				,	Ìß³ö¶ÓÎéÄ³¸ö³ÉÔ±)
MESSAGE_DEF(	CMD_PM_GUILD_DISBAND,			CMD_PM_BASE	+ 2		,	½âÉ¢¹«»á)
//CMD_PM_TEAM×ÓÀàÐÍ¶¨ÒåEnd

MESSAGE_DEF(	CMD_PM_GUILDINFO,				CMD_PM_BASE + 4	,	GroupServer·¢À´µÄ¹«»áÍ¬²½ÐÅÏ¢)
MESSAGE_DEF(	CMD_PM_GUILD_CHALLMONEY,		CMD_PM_BASE + 5    ,	½«¹«»áÕ½Í¶±êµÄÇ®ÍË¸ø»á³¤)
MESSAGE_DEF(	CMD_PM_GUILD_CHALL_PRIZEMONEY,	CMD_PM_BASE + 6    ,	½«¹«»áÕ½Õ½½±ÀøµÄÇ®¸ø»á³¤)
//BeginÂÒ¶·°×Òø³ÇÅÅÃûÏµÍ³
MESSAGE_DEF(	CMD_PM_GARNER2_UPDATE,			CMD_PM_BASE +7, ÎÞ)
//EndÂÒ¶·°×Òø³ÇÅÅÃûÏµÍ³
MESSAGE_DEF(	CMD_PM_TEAM_CREATE,				CMD_PM_BASE + 8, ÎÞ)
MESSAGE_DEF(	CMD_PM_SAY2ALL,					CMD_PM_BASE + 9, ÎÞ)
MESSAGE_DEF(	CMD_PM_SAY2TRADE,				CMD_PM_BASE + 10, ÎÞ)
MESSAGE_DEF(	CMD_PM_EXPSCALE,				CMD_PM_BASE + 11       ,	 ·À³ÁÃÔÏµÍ³  Ó°ÏìÍæ¼Ò¾­Ñé¼°µô±¦ÂÊ)
MESSAGE_DEF(	CMD_PM_GUILD_INVITE,			CMD_PM_BASE + 12		,	¹«»áÑûÇë)

// FEATURE: GUILD_BANK
MESSAGE_DEF(CMD_PM_GUILDBANK, CMD_PM_BASE + 13, Guild Bank)

//---------------------------------------------------
MESSAGE_DEF(	CMD_MP_ENTERMAP,				CMD_MP_BASE	+ 1, ÎÞ)
MESSAGE_DEF(	CMD_MP_GUILD_APPROVE,			CMD_MP_BASE	+ 2, ÎÞ)
MESSAGE_DEF(	CMD_MP_GUILD_CREATE,			CMD_MP_BASE	+ 3, ÎÞ)
MESSAGE_DEF(	CMD_MP_GUILD_KICK,				CMD_MP_BASE	+ 4, ÎÞ)
MESSAGE_DEF(	CMD_MP_GUILD_LEAVE,				CMD_MP_BASE	+ 5, ÎÞ)
MESSAGE_DEF(	CMD_MP_GUILD_DISBAND,			CMD_MP_BASE	+ 6, ÎÞ)
MESSAGE_DEF(	CMD_MP_GUILD_MOTTO,				CMD_MP_BASE	+ 10, ÎÞ)
MESSAGE_DEF(	CMD_MP_GUILD_CHALLMONEY,		CMD_MP_BASE + 13			,	½«Í¶±ê¹«»áµÄÇ®ÍË»¹»á³¤½ÇÉ«)
MESSAGE_DEF(	CMD_MP_GUILD_CHALL_PRIZEMONEY,	CMD_MP_BASE + 14    ,	½«¹«»áÕ½½±ÀøµÄµÄÇ®¸ø»á³¤½ÇÉ«)

//BeginÂÒ¶·°×Òø³ÇÅÅÃûÏµÍ³
MESSAGE_DEF(	CMD_MP_GARNER2_UPDATE,			CMD_MP_BASE +7, ÎÞ)
MESSAGE_DEF(	CMD_MP_GARNER2_CGETORDER,		CMD_MP_BASE+15, ÎÞ)
//EndÂÒ¶·°×Òø³ÇÅÅÃûÏµÍ³

MESSAGE_DEF(	CMD_MP_TEAM_CREATE,				CMD_MP_BASE + 16, ÎÞ)

MESSAGE_DEF(	CMD_MP_MASTER_CREATE,			CMD_MP_BASE + 17, ÎÞ)
MESSAGE_DEF(	CMD_MP_MASTER_DEL,				CMD_MP_BASE + 18, ÎÞ)
MESSAGE_DEF(	CMD_MP_MASTER_FINISH,			CMD_MP_BASE + 19, ÎÞ)
MESSAGE_DEF(	CMD_MP_SAY2ALL,					CMD_MP_BASE + 20, ÎÞ)
MESSAGE_DEF(	CMD_MP_SAY2TRADE,				CMD_MP_BASE + 21, ÎÞ)
MESSAGE_DEF(	CMD_MP_GM1SAY1,					CMD_MP_BASE + 22, ÎÞ)
MESSAGE_DEF(	CMD_MP_GM1SAY,					CMD_MP_BASE + 23, ÎÞ)
MESSAGE_DEF(	CMD_MP_GMBANACCOUNT,			CMD_MP_BASE	+ 24, GM·âÍ£Ë¢ÆÁÕËºÅ)

// FEATURE: GUILD_BANK
MESSAGE_DEF(CMD_MP_GUILDBANK, CMD_MP_BASE + 25, Guild Bank)
MESSAGE_DEF(CMD_MP_GUILDADDEXP, CMD_MP_BASE + 26, Guild Attr)
MESSAGE_DEF(CMD_MP_GUILD_PERM, CMD_MP_BASE + 27, Guild Perm)
MESSAGE_DEF(CMD_MP_GUILDNAMECOLOR, CMD_MP_BASE + 28, Guild Color)
MESSAGE_DEF(CMD_MP_GUILDNOTICE, CMD_MP_BASE + 29, Guild Notice)
/*=====================================================================================================
*					Ga(t)eServer <->Grou(p)Server 
*/
MESSAGE_DEF(	CMD_TP_LOGIN,					CMD_TP_BASE + 1		,	GateServerµ½GroupServer½ø³Ì¼äµÇÂ½)
MESSAGE_DEF(	CMD_TP_LOGOUT,					CMD_TP_BASE + 2		,	GateServerµ½GroupServer½ø³Ì¼äµÇ³ö)
MESSAGE_DEF(	CMD_TP_USER_LOGIN,				CMD_TP_BASE	+ 3		,	ÓÃ»§µÇÈë)
MESSAGE_DEF(	CMD_TP_USER_LOGOUT,				CMD_TP_BASE + 4		,	ÓÃ»§µÇ³ö)
MESSAGE_DEF(	CMD_TP_BGNPLAY,					CMD_TP_BASE	+ 5		,	ÓÃ»§Ñ¡Ôñ½ÇÉ«¿ªÊ¼ÍæÓÎÏ·)
MESSAGE_DEF(	CMD_TP_ENDPLAY,					CMD_TP_BASE	+ 6		,	ÓÃ»§½áÊøÍæÓÎÏ·½øÈëÑ¡/½¨/É¾½ÇÉ«»­Ãæ)
MESSAGE_DEF(	CMD_TP_NEWCHA,					CMD_TP_BASE	+ 7		,	ÐÂ½¨½ÇÉ«)
MESSAGE_DEF(	CMD_TP_DELCHA,					CMD_TP_BASE	+ 8		,	É¾³ý½ÇÉ«)
MESSAGE_DEF(	CMD_TP_PLAYEREXIT,				CMD_TP_BASE + 9		,	ÍË³ö½ÇÉ«ÊÍ·Å×ÊÔ´)
MESSAGE_DEF(	CMD_TP_REQPLYLST,				CMD_TP_BASE	+ 10	,	ÇëÇó±¾GateServerÉÏµÄÍæ¼ÒµØÖ·ÁÐ±í)
MESSAGE_DEF(	CMD_TP_DISC,					CMD_TP_BASE + 11, ÎÞ)
MESSAGE_DEF(	CMD_TP_ESTOPUSER_CHECK,			CMD_TP_BASE + 12, ÎÞ)
MESSAGE_DEF(	CMD_TP_CREATE_PASSWORD2,		CMD_TP_BASE + 13,	´´½¨¶þ´ÎÃÜÂë)
MESSAGE_DEF(	CMD_TP_UPDATE_PASSWORD2,		CMD_TP_BASE + 14,	¸üÐÂ¶þ´ÎÃÜÂë)

// Add by lark.li 20081119 begin
MESSAGE_DEF(	CMD_TP_SYNC_PLYLST,				CMD_TP_BASE	+ 15	,	·¢ËÍ±¾GateServerÉÏµÄÍæ¼ÒµØÖ·ÁÐ±í)

// End

//---------------------------------------------------
MESSAGE_DEF(	CMD_PT_KICKUSER,				CMD_PT_BASE	+ 11, ÎÞ)
MESSAGE_DEF(	CMD_PT_ESTOPUSER,				CMD_PT_BASE + 12,	½ûÑÔ½ÇÉ«)
MESSAGE_DEF(	CMD_PT_DEL_ESTOPUSER,			CMD_PT_BASE + 13,	½â³ý½ûÑÔ)

/*=====================================================================================================
*					Grou(p)Server <-> (A)ccountServer
*/
MESSAGE_DEF(	CMD_PA_LOGIN,					CMD_PA_BASE + 2, ÎÞ)
MESSAGE_DEF(	CMD_PA_USER_LOGIN,				CMD_PA_BASE + 4, ÎÞ)
MESSAGE_DEF(	CMD_PA_USER_LOGOUT,				CMD_PA_BASE + 5, ÎÞ)
MESSAGE_DEF(	CMD_PA_USER_DISABLE,			CMD_PA_BASE + 6, ÎÞ)
MESSAGE_DEF(	CMD_PA_USER_LOGIN1,				CMD_PA_BASE + 13, ÎÞ)
MESSAGE_DEF(	CMD_PA_USER_LOGIN2,				CMD_PA_BASE + 14, ÎÞ)
MESSAGE_DEF(	CMD_PA_USER_BILLBGN,			CMD_PA_BASE + 20, ÎÞ)
MESSAGE_DEF(	CMD_PA_USER_BILLEND,			CMD_PA_BASE + 21, ÎÞ)
MESSAGE_DEF(	CMD_PA_GROUP_BILLEND_AND_LOGOUT,CMD_PA_BASE + 22, ÎÞ)
MESSAGE_DEF(	CMD_PA_LOGOUT,					CMD_PA_BASE + 23, ÎÞ)
MESSAGE_DEF(	CMD_PA_GMBANACCOUNT,			CMD_PA_BASE + 24, GM·âÍ£ÕËºÅ)//Add by sunny.sun 20090828
//---------------------------------------------------
MESSAGE_DEF(	CMD_AP_LOGIN,					CMD_AP_BASE + 2, ÎÞ)
MESSAGE_DEF(	CMD_AP_USER_LOGIN,				CMD_AP_BASE + 3, ÎÞ)
MESSAGE_DEF(	CMD_AP_RELOGIN,					CMD_AP_BASE + 4, ÎÞ)
MESSAGE_DEF(	CMD_AP_KICKUSER,				CMD_AP_BASE	+ 11, ÎÞ)
MESSAGE_DEF(	CMD_AP_USER_LOGIN1,				CMD_AP_BASE + 12, ÎÞ)
MESSAGE_DEF(	CMD_AP_USER_LOGIN2,				CMD_AP_BASE + 13, ÎÞ)
MESSAGE_DEF(	CMD_AP_EXPSCALE,				CMD_AP_BASE + 14, ·À³ÁÃÔ)
/*=====================================================================================================
*					Ga(m)eServer <-> Ga(m)eServer
*
*/
MESSAGE_DEF(	CMD_MM_GATE_RELEASE,			CMD_MM_BASE + 1, ÎÞ)
MESSAGE_DEF(	CMD_MM_GATE_CONNECT,			CMD_MM_BASE + 2, ÎÞ)
MESSAGE_DEF(	CMD_MM_QUERY_CHA,				CMD_MM_BASE + 3	,	²éÑ¯½ÇÉ«»ù±¾ÐÅÏ¢)
MESSAGE_DEF(	CMD_MM_QUERY_CHAITEM,			CMD_MM_BASE + 4	,	²éÑ¯½ÇÉ«µÀ¾ßÐÅÏ¢)
MESSAGE_DEF(	CMD_MM_CALL_CHA,				CMD_MM_BASE + 5	,	½«½ÇÉ«ÕÙ»½µ½×Ô¼ºÉí±ß)
MESSAGE_DEF(	CMD_MM_GOTO_CHA,				CMD_MM_BASE + 6	,	½«×Ô¼º´«ËÍµ½½ÇÉ«Éí±ß)
MESSAGE_DEF(	CMD_MM_KICK_CHA,				CMD_MM_BASE + 7	,	½«½ÇÉ«ÌßÏÂÏß)
MESSAGE_DEF(	CMD_MM_GUILD_REJECT,			CMD_MM_BASE + 8	,	¾Ü¾ø¼ÓÈë¹«»áÉêÇëÍ¨ÖªÃüÁî)
MESSAGE_DEF(	CMD_MM_GUILD_APPROVE,			CMD_MM_BASE + 9	,	¾Ü¾ø¼ÓÈë¹«»áÉêÇëÍ¨ÖªÃüÁî)
MESSAGE_DEF(	CMD_MM_GUILD_KICK,				CMD_MM_BASE	+ 10,	´Ó¹«»áÀïÃæÌÞ³ýÄ³ÈË)
MESSAGE_DEF(	CMD_MM_GUILD_DISBAND,			CMD_MM_BASE	+ 11,	½âÉ¢¹«»á)
MESSAGE_DEF(	CMD_MM_QUERY_CHAPING,			CMD_MM_BASE + 12,	²éÑ¯½ÇÉ«µ½GameServerÂß¼­²ãµÄpingÖµ)
MESSAGE_DEF(	CMD_MM_NOTICE,					CMD_MM_BASE + 13,	ÏµÍ³Í¨¸æ)
MESSAGE_DEF(	CMD_MM_GUILD_MOTTO,				CMD_MM_BASE + 14,	¸üÐÂ¹«»á×ùÓÓÃû)
MESSAGE_DEF(	CMD_MM_DO_STRING,				CMD_MM_BASE + 15,	Ö´ÐÐ½Å±¾)
MESSAGE_DEF(	CMD_MM_CHA_NOTICE,				CMD_MM_BASE + 16,	¶ÔÖ¸¶¨Íæ¼ÒµÄÏµÍ³Í¨¸æ)
MESSAGE_DEF(	CMD_MM_LOGIN,					CMD_MM_BASE + 17,	Íæ¼ÒµÇÂ½ÓÎÏ·)
MESSAGE_DEF(	CMD_MM_GUILD_CHALL_PRIZEMONEY,	CMD_MM_BASE + 18,	ÍË»¹¹¤»áÕ½µÄ·ÑÓÃ)
MESSAGE_DEF(	CMD_MM_ADDCREDIT,				CMD_MM_BASE + 19,	Ôö¼ÓÉùÍû)
MESSAGE_DEF(	CMD_MM_STORE_BUY,				CMD_MM_BASE + 20,	¹ºÂòÉÌ³ÇÉÌÆ·)
MESSAGE_DEF(	CMD_MM_ADDMONEY,				CMD_MM_BASE + 21, ÎÞ)
MESSAGE_DEF(	CMD_MM_AUCTION,					CMD_MM_BASE + 22, ÎÞ)
MESSAGE_DEF(	CMD_MM_NOTICETOCHA,				CMD_MM_BASE + 23,	Ë®¾§½»Ò×Í¨Öª²»ÔÚÍ¬Ò»·þÎñÆ÷µÄ½»Ò×Õß)
MESSAGE_DEF(	CMD_MM_EXCHEXTERNVALUE,			CMD_MM_BASE + 24,	½Å±¾¸Ä±äÈ«¾Ö±äÁ¿)
MESSAGE_DEF(	CMD_MM_NOTICETRADER,			CMD_MM_BASE + 25,	Í¨Öª½»Ò×Õß)

// FEATURE: GUILD_BANK
MESSAGE_DEF(CMD_MM_UPDATEGUILDBANK, CMD_MM_BASE + 26, Update Guild Bank)
MESSAGE_DEF(CMD_MM_UPDATEGUILDBANKGOLD, CMD_MM_BASE + 27, Update Guild Bank Gold)
MESSAGE_DEF(CMD_MM_UPDATEGUILDPERMISSONS, CMD_MM_BASE + 28, Update Guild Bank Gold)

/*=====================================================================================================
*					Grou(p)Server <-> (C)lient
*/
//×é¶ÓÃüÁî¶¨ÒåBEGIN
	MESSAGE_DEF(	CMD_CP_TEAM_INVITE,			CMD_CP_BASE	+ 1		,	ÑûÇë×é¶Ó)
	MESSAGE_DEF(	CMD_CP_TEAM_ACCEPT,			CMD_CP_BASE	+ 2		,	È·ÈÏ×é¶Ó)
	MESSAGE_DEF(	CMD_CP_TEAM_REFUSE,			CMD_CP_BASE	+ 3		,	¾Ü¾ø×é¶Ó)
	MESSAGE_DEF(	CMD_CP_TEAM_LEAVE,			CMD_CP_BASE	+ 4		,	Àë¿ª×é¶Ó)
	MESSAGE_DEF(	CMD_CP_TEAM_KICK,			CMD_CP_BASE + 5		,	Ìß³ö×é¶Ó)
//×é¶ÓÃüÁî¶¨ÒåEND
//ºÃÓÑÃüÁî¶¨ÒåBEGIN
	MESSAGE_DEF(	CMD_CP_FRND_INVITE,			CMD_CP_BASE	+ 11	,	ºÃÓÑÑûÇë)
	MESSAGE_DEF(	CMD_CP_FRND_ACCEPT,			CMD_CP_BASE	+ 12	,	ºÃÓÑ½ÓÊÕ)
	MESSAGE_DEF(	CMD_CP_FRND_REFUSE,			CMD_CP_BASE	+ 13	,	ºÃÓÑ¾Ü¾ø)
	MESSAGE_DEF(	CMD_CP_FRND_DELETE,			CMD_CP_BASE	+ 14	,	ºÃÓÑÉ¾³ý)
	MESSAGE_DEF(	CMD_CP_FRND_MOVE_GROUP,		CMD_CP_BASE	+ 15	,	¸Ä±äºÃÓÑËùÊô·Ö×é)
	MESSAGE_DEF(	CMD_CP_FRND_REFRESH_INFO,	CMD_CP_BASE	+ 16	,	ÔÚ×Ô¼ºµÄ¿Í»§¶ËË¢ÐÂ±ðÈË£¨ºÃÓÑ¡¢¶ÓÓÑ¡¢Í¬ÃÅ¡¢Â·ÈË¡¢Ä°ÉúÈË£©µÄ¸öÈËÐÅÏ¢)
	MESSAGE_DEF(	CMD_CP_CHANGE_PERSONINFO,	CMD_CP_BASE	+ 17	,	¸Ä±äÍæ¼Ò×Ô¼ºµÄÍ·ÏñµÈ¸öÈËÐÅÏ¢)

// Add by lark.li 20080804 begin
	MESSAGE_DEF(	CMD_CP_FRND_DEL_GROUP,		CMD_CP_BASE	+ 18, ÎÞ)
	MESSAGE_DEF(	CMD_CP_FRND_ADD_GROUP,		CMD_CP_BASE	+ 19, ÎÞ)
	MESSAGE_DEF(	CMD_CP_FRND_CHANGE_GROUP,	CMD_CP_BASE	+ 20, ÎÞ)
// End

// Add by lark.li 20080808 begin
	MESSAGE_DEF(	CMD_CP_QUERY_PERSONINFO,	CMD_CP_BASE	+ 21, ÎÞ)
// End

//ºÃÓÑÃüÁî¶¨ÒåEND

// Modify by lark.li 20080806 begin
//MESSAGE_DEF(	CMD_CP_PING				CMD_CP_BASE	+ 20
MESSAGE_DEF(	CMD_CP_PING,					CMD_CP_BASE	+ 25, ÎÞ)

// Íâ¹Ò
//MESSAGE_DEF(	CMD_CP_REPORT_WG		CMD_CP_BASE + 21,	±¨¸æ·¢ÏÖÊ¹ÓÃÍâ¹Ò
MESSAGE_DEF(	CMD_CP_REPORT_WG,				CMD_CP_BASE + 26,	±¨¸æ·¢ÏÖÊ¹ÓÃÍâ¹Ò)
// End

//°ÝÊ¦ÃüÁî¶¨ÒåBEGIN
MESSAGE_DEF(	CMD_CP_MASTER_REFRESH_INFO,		CMD_CP_BASE	+ 31	,	ÔÚ×Ô¼ºµÄ¿Í»§¶ËË¢ÐÂÊ¦¸¸µÄ¸öÈËÐÅÏ¢)
MESSAGE_DEF(	CMD_CP_PRENTICE_REFRESH_INFO,	CMD_CP_BASE	+ 32	,	ÔÚ×Ô¼ºµÄ¿Í»§¶ËË¢ÐÂÍ½µÜµÄ¸öÈËÐÅÏ¢)
//°ÝÊ¦ÃüÁî¶¨ÒåEND

//Add by sunny.sun 20090605 for Guild Invite 
MESSAGE_DEF(	CMD_CP_GUILD_INVITE,			CMD_CP_BASE + 33, ÎÞ)
MESSAGE_DEF(	CMD_CP_GUILD_ACCEPT,			CMD_CP_BASE + 34, ÎÞ)
MESSAGE_DEF(	CMD_CP_GUILD_REFUSE,			CMD_CP_BASE + 35, ÎÞ)


//¹«»áÃüÁî¶¨ÒåBegin

//¹«»áÃüÁî¶¨ÒåEnd
//Begin ÁÄÌìÖ¸Áî
	MESSAGE_DEF(	CMD_CP_GM1SAY,				CMD_CP_BASE	+ 400		,	GMµÄÈ«·þ¹ã²¥)
	MESSAGE_DEF(	CMD_CP_SAY2TRADE,			CMD_CP_BASE	+ 401		,	½»Ò×ÆµµÀ)
	MESSAGE_DEF(	CMD_CP_SAY2ALL,				CMD_CP_BASE + 402		,	Õû¸öÓÎÏ·ÊÀ½ç¹ã²¥)
	MESSAGE_DEF(	CMD_CP_SAY2YOU,				CMD_CP_BASE	+ 403		,	P2PÁÄÌì(Íæ¼Ò¶ÔÍæ¼Ò))
	MESSAGE_DEF(	CMD_CP_SAY2TEM,				CMD_CP_BASE	+ 404		,	×é¶ÓÁÄÌì)
	MESSAGE_DEF(	CMD_CP_SAY2GUD,				CMD_CP_BASE	+ 405		,	ÐÐ»á(Guild)ÁÄÌì)

	MESSAGE_DEF(	CMD_CP_SESS_CREATE,			CMD_CP_BASE	+ 406		,	»á»°½¨Á¢)
	MESSAGE_DEF(	CMD_CP_SESS_SAY,			CMD_CP_BASE	+ 407		,	»á»°Ëµ»°)
	MESSAGE_DEF(	CMD_CP_SESS_ADD,			CMD_CP_BASE	+ 408		,	»á»°Ìí¼ÓÈËÔ±)
	MESSAGE_DEF(	CMD_CP_SESS_LEAVE,			CMD_CP_BASE	+ 409		,	Àë¿ª»á»°)
	MESSAGE_DEF(	CMD_CP_REFUSETOME,			CMD_CP_BASE	+ 410		,	´ò¿ª½ÓÊÕË½ÁÄ¿ª¹Ø)
	MESSAGE_DEF(	CMD_CP_GM1SAY1,				CMD_CP_BASE	+ 411		,	¹ö¶¯¹ã²¥)
	MESSAGE_DEF(	CMD_CP_GMSAY,				CMD_CP_BASE	+ 412		,	ÌØÊâÃüÁîÖ´ÐÐ)
	MESSAGE_DEF(CMD_CP_SAY2GUILD, CMD_CP_BASE + 413, ÐÐ»á(Guild)ÁÄÌì)

	// FEATURE: GUILD_BANK
	MESSAGE_DEF(CMD_CP_GUILDBANK, CMD_CP_BASE + 414, Guild Bank)

	// GUILD_ATTR
	MESSAGE_DEF(CMD_CP_GUILDATTR, CMD_CP_BASE + 416, Guild Attr)
	MESSAGE_DEF(CMD_CP_GUILDNAMECOLOR, CMD_CP_BASE + 417, Guild Color)
//End ÁÄÌìÖ¸Áî
//BeginÂÒ¶·°×Òø³ÇÅÅÃûÏµÍ³
//EndÂÒ¶·°×Òø³ÇÅÅÃûÏµÍ³
//---------------------------------------------------
//×é¶ÓÃüÁî¶¨ÒåBEGIN
	MESSAGE_DEF(	CMD_PC_TEAM_INVITE,			CMD_PC_BASE	+ 1			,	Í¨Öª¿Í»§¶ËÓÐÈËÑûÇë×é¶Ó)
	MESSAGE_DEF(	CMD_PC_TEAM_REFRESH,		CMD_PC_BASE	+ 2			,	Í¨Öª¿Í»§¶Ë×é¶Ó±ä»¯µÄÐÅÏ¢£¬Ê¹ÓÃCMD_PM_TEAM¶¨ÒåµÄ×ÓÀàÐÍ)
	MESSAGE_DEF(	CMD_PC_TEAM_CANCEL,			CMD_PC_BASE	+ 3			,	·þÎñÆ÷ÒòÎª³¬Ê±»ò³ÉÔ±ÊýÒÑÂúµÈÔ­ÒòÍ¨Öª±»ÑûÇë·½È¡Ïû±¾´ÎÑûÇë)
	//CMD_PC_TEAM_CANCEL×ÓÀàÐÍ¶¨ÒåBegin
		MESSAGE_DEF(	MSG_TEAM_CANCLE_BUSY,	1	,	×Ô¼ºµÄÑûÇë¶ÓÁÐ·±Ã¦)
		MESSAGE_DEF(	MSG_TEAM_CANCLE_TIMEOUT,2	,	±¾´ÎÑûÇë³¬Ê±)
		MESSAGE_DEF(	MSG_TEAM_CANCLE_OFFLINE,3	,	ÑûÇë·½ÒÑÏÂÏß)
		MESSAGE_DEF(	MSG_TEAM_CANCLE_ISFULL,	4	,	ÑûÇë·½µÄ×é³ÉÔ±ÊýÒÑÂú)
		MESSAGE_DEF(	MSG_TEAM_CANCLE_CANCEL,	5	,	±»ÑûÇë·½¼ÓÈë±ðµÄ/´´½¨ÁË×é¶Ó»òÑûÇë·½¼ÓÈëÁË±ðÈË´´½¨µÄ×é¶Ó¶øÈ¡ÏûÁË±¾´ÎÑûÇë)
	//CMD_PC_TEAM_CANCEL×ÓÀàÐÍ¶¨ÒåEnd
//×é¶ÓÃüÁî¶¨ÒåEND
//ºÃÓÑÃüÁî¶¨ÒåBEGIN
	MESSAGE_DEF(	CMD_PC_FRND_INVITE,			CMD_PC_BASE	+ 11, ÎÞ)
	MESSAGE_DEF(	CMD_PC_FRND_REFRESH,		CMD_PC_BASE	+ 12, ÎÞ)
	//CMD_PC_FRND_REFRESH×ÓÀàÐÍ¶¨ÒåBegin
		MESSAGE_DEF(	MSG_FRND_REFRESH_START,	1  ,	×ÓÀàÐÍ: ¸ÕÉÏÏßÊ±ºò´«ÊäµÄºÃÓÑÁÐ±í)
		MESSAGE_DEF(	MSG_FRND_REFRESH_ADD,	2  ,	×ÓÀàÐÍ: ºÃÓÑÔö¼Ó)
		MESSAGE_DEF(	MSG_FRND_REFRESH_DEL,	3  ,	×ÓÀàÐÍ: ºÃÓÑÉ¾³ý)
		MESSAGE_DEF(	MSG_FRND_REFRESH_ONLINE,4,	×ÓÀàÐÍ: ºÃÓÑÉÏÏß)
		MESSAGE_DEF(	MSG_FRND_REFRESH_OFFLINE,5  ,	×ÓÀàÐÍ: ºÃÓÑÀëÏß)
	//CMD_PC_FRND_REFRESH×ÓÀàÐÍ¶¨ÒåEnd
	MESSAGE_DEF(	CMD_PC_FRND_CANCEL,			CMD_PC_BASE	+ 13, ÎÞ)
	//CMD_PC_FRND_CANCEL×ÓÀàÐÍ¶¨ÒåBegin
		MESSAGE_DEF(	MSG_FRND_CANCLE_BUSY,	1	,	×Ô¼ºµÄÑûÇë¶ÓÁÐ·±Ã¦)
		MESSAGE_DEF(	MSG_FRND_CANCLE_TIMEOUT,2	,	±¾´ÎÑûÇë³¬Ê±)
		MESSAGE_DEF(	MSG_FRND_CANCLE_OFFLINE,3	,	ÑûÇë·½ÒÑÏÂÏß)
		MESSAGE_DEF(	MSG_FRND_CANCLE_INVITER_ISFULL,	4	,	ÑûÇë·½µÄºÃÓÑ³ÉÔ±ÊýÒÑÂú)
		MESSAGE_DEF(	MSG_FRND_CANCLE_SELF_ISFULL,	5	,	×Ô¼ºµÄºÃÓÑÊýÒÑÂú)
		MESSAGE_DEF(	MSG_FRND_CANCLE_CANCEL,	6	,	ÑûÇë·½È¡ÏûÁË±¾´ÎÑûÇë)
	//CMD_PC_FRND_CANCEL×ÓÀàÐÍ¶¨ÒåEnd
	MESSAGE_DEF(	CMD_PC_FRND_MOVE_GROUP,		CMD_PC_BASE + 15	,	¸Ä±äºÃÓÑ·Ö×é³É¹¦)
	MESSAGE_DEF(	CMD_PC_FRND_REFRESH_INFO,	CMD_PC_BASE	+ 16	,	Ë¢ÐÂºÃÓÑ¸öÈËÐÅÏ¢)
	MESSAGE_DEF(	CMD_PC_CHANGE_PERSONINFO,	CMD_PC_BASE	+ 17	,	¸Ä±äÍæ¼Ò×Ô¼ºµÄÍ·ÏñµÈ¸öÈËÐÅÏ¢)

// Add by lark.li 20080804 begin
	MESSAGE_DEF(	CMD_PC_FRND_DEL_GROUP,		CMD_PC_BASE + 18	,	É¾³ýºÃÓÑ·Ö×é³É¹¦)
	MESSAGE_DEF(	CMD_PC_FRND_ADD_GROUP,		CMD_PC_BASE + 19	,	ÐÂÔöºÃÓÑ·Ö×é³É¹¦)
	MESSAGE_DEF(	CMD_PC_FRND_CHANGE_GROUP,	CMD_PC_BASE + 20	,	¸Ä±äºÃÓÑ·Ö×é³É¹¦)
// End

// Add by lark.li 20080808 begin
MESSAGE_DEF(	CMD_PC_QUERY_PERSONINFO,		CMD_PC_BASE	+ 21	,	¸Ä±äÍæ¼Ò×Ô¼ºµÄÍ·ÏñµÈ¸öÈËÐÅÏ¢)
// End
	//
//ºÃÓÑÃüÁî¶¨ÒåEND

// Modify by lark.li 20080806
//MESSAGE_DEF(	CMD_PC_PING				CMD_PC_BASE	+ 20
MESSAGE_DEF(	CMD_PC_PING,					CMD_PC_BASE	+ 25, ÎÞ)
// End

//¹«»áÃüÁî¶¨ÒåBegin
	MESSAGE_DEF(	CMD_PC_GUILD,				CMD_PC_BASE	+ 30, ÎÞ)
	MESSAGE_DEF(CMD_PC_GUILDNAMECOLOR, CMD_MP_BASE + 32, Guild Color)
	MESSAGE_DEF(CMD_PC_GUILD_PERM, CMD_PC_BASE + 34, GuildPerm)

	//CMD_PC_GUILD×ÓÀàÐÍ¶¨ÒåBegin
		MESSAGE_DEF(	MSG_GUILD_START,	1, ÎÞ)
		MESSAGE_DEF(	MSG_GUILD_ADD,		2, ÎÞ)
		MESSAGE_DEF(	MSG_GUILD_DEL,		3, ÎÞ)
		MESSAGE_DEF(	MSG_GUILD_ONLINE,	4, ÎÞ)
		MESSAGE_DEF(	MSG_GUILD_OFFLINE,	5, ÎÞ)
		MESSAGE_DEF(	MSG_GUILD_STOP,		6, ÎÞ)
	//CMD_PC_GUILD×ÓÀàÐÍ¶¨ÒåEnd
//¹«»áÃüÁî¶¨ÒåEnd

	MESSAGE_DEF(	CMD_PC_GUILD_INVITE,		CMD_PC_BASE	+ 31, ÎÞ)
	//MESSAGE_DEF(	CMD_PC_GUILD_REFRESH		CMD_PC_BASE	+ 32
//Add by sunny.sun 20090608
MESSAGE_DEF(	CMD_PC_GUILD_CANCEL,		CMD_PC_BASE	+ 33, ÎÞ)
	MESSAGE_DEF(	MSG_GUILD_CANCLE_BUSY,			1	,	×Ô¼ºµÄÑûÇë¶ÓÁÐ·±Ã¦)
	MESSAGE_DEF(	MSG_GUILD_CANCLE_TIMEOUT,		2	,	±¾´ÎÑûÇë³¬Ê±)
	MESSAGE_DEF(	MSG_GUILD_CANCLE_OFFLINE,		3	,	¶Ô·½ÒÑÏÂÏß)
	MESSAGE_DEF(	MSG_GUILD_CANCLE_SELF_ISFULL,	4	,	¶Ô·½µÄ¹«»áÈËÔ±ÊýÒÑÂú)
	MESSAGE_DEF(	MSG_GUILD_CANCLE_CANCEL,		5	,	¶Ô·½È¡ÏûÁË±¾´Î¹«»áÑûÇë)
	MESSAGE_DEF(	MSG_GUILD_CANCLE_SELFCANCEL,	6	,	×Ô¼ºÈ¡ÏûÁËÑûÇë)
//End
//°ÝÊ¦ÃüÁî¶¨ÒåBEGIN
MESSAGE_DEF(	CMD_PC_MASTER_REFRESH,	CMD_PC_BASE	+ 41, ÎÞ)
	//CMD_PC_MASTER_REFRESH×ÓÀàÐÍ¶¨ÒåBegin
	MESSAGE_DEF(	MSG_MASTER_REFRESH_START,		1  ,	×ÓÀàÐÍ: ¸ÕÉÏÏßÊ±ºò´«ÊäµÄÊ¦¸¸ÁÐ±í)
	MESSAGE_DEF(	MSG_MASTER_REFRESH_ADD,			2  ,	×ÓÀàÐÍ: Ê¦¸¸Ôö¼Ó)
	MESSAGE_DEF(	MSG_MASTER_REFRESH_DEL,			3  ,	×ÓÀàÐÍ: Ê¦¸¸É¾³ý)
	MESSAGE_DEF(	MSG_MASTER_REFRESH_ONLINE,		4	 ,	×ÓÀàÐÍ: Ê¦¸¸ÉÏÏß)
	MESSAGE_DEF(	MSG_MASTER_REFRESH_OFFLINE,		5  ,	×ÓÀàÐÍ: Ê¦¸¸ÀëÏß)
	MESSAGE_DEF(	MSG_PRENTICE_REFRESH_START,		6  ,	×ÓÀàÐÍ: ¸ÕÉÏÏßÊ±ºò´«ÊäµÄÍ½µÜÁÐ±í)
	MESSAGE_DEF(	MSG_PRENTICE_REFRESH_ADD,		7  ,	×ÓÀàÐÍ: Í½µÜÔö¼Ó)
	MESSAGE_DEF(	MSG_PRENTICE_REFRESH_DEL,		8  ,	×ÓÀàÐÍ: Í½µÜÉ¾³ý)
	MESSAGE_DEF(	MSG_PRENTICE_REFRESH_ONLINE,	9  ,	×ÓÀàÐÍ: Í½µÜÉÏÏß)
	MESSAGE_DEF(	MSG_PRENTICE_REFRESH_OFFLINE,	10  ,	×ÓÀàÐÍ: Í½µÜÀëÏß)
	//CMD_PC_MASTER_REFRESH×ÓÀàÐÍ¶¨ÒåEnd
MESSAGE_DEF(	CMD_PC_MASTER_CANCEL,	CMD_PC_BASE	+ 42, ÎÞ)
	//CMD_PC_MASTER_CANCEL×ÓÀàÐÍ¶¨ÒåBegin
	MESSAGE_DEF(	MSG_MASTER_CANCLE_BUSY,				1	,	×Ô¼ºµÄ°ÝÊ¦¶ÓÁÐ·±Ã¦)
	MESSAGE_DEF(	MSG_MASTER_CANCLE_TIMEOUT,			2	,	±¾´Î°ÝÊ¦³¬Ê±)
	MESSAGE_DEF(	MSG_MASTER_CANCLE_OFFLINE,			3	,	¶Ô·½ÒÑÏÂÏß)
	MESSAGE_DEF(	MSG_MASTER_CANCLE_INVITER_ISFULL,	4	,	¶Ô·½µÄÍ½µÜÊýÒÑÂú)
	MESSAGE_DEF(	MSG_MASTER_CANCLE_SELF_ISFULL,		5	,	×Ô¼ºµÄÊ¦¸¸ÊýÒÑÂú)
	MESSAGE_DEF(	MSG_MASTER_CANCLE_CANCEL,			6	,	¶Ô·½È¡ÏûÁË±¾´Î°ÝÊ¦)
	//CMD_PC_MASTER_CANCEL×ÓÀàÐÍ¶¨ÒåEnd
MESSAGE_DEF(	CMD_PC_MASTER_REFRESH_INFO,			CMD_PC_BASE	+ 43	,	Ë¢ÐÂÊ¦¸¸¸öÈËÐÅÏ¢)
MESSAGE_DEF(	CMD_PC_PRENTICE_REFRESH_INFO,		CMD_PC_BASE	+ 44	,	Ë¢ÐÂÍ½µÜ¸öÈËÐÅÏ¢)
//
//°ÝÊ¦ÃüÁî¶¨ÒåEND

//Begin ÁÄÌìÖ¸Áî
	MESSAGE_DEF(	CMD_PC_GM1SAY,		CMD_PC_BASE	+ 400		,	GMµÄÈ«·þ¹ã²¥)
	MESSAGE_DEF(	CMD_PC_SAY2TRADE,	CMD_PC_BASE	+ 401		,	½»Ò×ÆµµÀ)
	MESSAGE_DEF(	CMD_PC_SAY2ALL,		CMD_PC_BASE + 402		,	Õû¸öÓÎÏ·ÊÀ½ç¹ã²¥)
	MESSAGE_DEF(	CMD_PC_SAY2YOU,		CMD_PC_BASE	+ 403		,	P2PÁÄÌì(Íæ¼Ò¶ÔÍæ¼Ò))
	MESSAGE_DEF(	CMD_PC_SAY2TEM,		CMD_PC_BASE	+ 404		,	×é¶ÓÁÄÌì)
	MESSAGE_DEF(	CMD_PC_SAY2GUD,		CMD_PC_BASE	+ 405		,	ÐÐ»á(Guild)ÁÄÌì)

	MESSAGE_DEF(	CMD_PC_SESS_CREATE,	CMD_PC_BASE	+ 406		,	»á»°½¨Á¢)
	MESSAGE_DEF(	CMD_PC_SESS_SAY,	CMD_PC_BASE	+ 407		,	»á»°Ëµ»°)
	MESSAGE_DEF(	CMD_PC_SESS_ADD,	CMD_PC_BASE	+ 408		,	»á»°Ìí¼ÓÈËÔ±)
	MESSAGE_DEF(	CMD_PC_SESS_LEAVE,	CMD_PC_BASE	+ 409		,	Àë¿ª»á»°)
	MESSAGE_DEF(	CMD_PC_GM1SAY1,	CMD_PC_BASE	+ 411			,	¹ö¶¯¹ã²¥)

	// FEATURE: GUILD_BANK
	MESSAGE_DEF(CMD_PC_ERRMSG, CMD_PC_BASE + 414, Error Message)
	MESSAGE_DEF(CMD_PC_UPDATEGUILDATTR, CMD_PC_BASE + 415,	Guild Attr)
	MESSAGE_DEF(CMD_PC_GUILDNOTICE, CMD_PC_BASE + 418, Guild Notice)


//End ÁÄÌìÖ¸Áî
//BeginÂÒ¶·°×Òø³ÇÅÅÃûÏµÍ³
	MESSAGE_DEF(	CMD_PC_GARNER2_ORDER,	CMD_PC_BASE +101, ÎÞ)
//EndÂÒ¶·°×Òø³ÇÅÅÃûÏµÍ³

// Add by lark.li 20081119 begin
MESSAGE_DEF(	CMD_OS_LOGIN,		CMD_OS_BASE + 1			,	µÇÂ¼)
MESSAGE_DEF(	CMD_OS_PING,		CMD_OS_BASE + 2			,	¼ì²âÃüÁî)
MESSAGE_DEF(	CMD_SO_LOGIN,		CMD_SO_BASE + 1			,	µÇÂ¼ÃüÁî)
MESSAGE_DEF(	CMD_SO_PING,		CMD_SO_BASE + 2			,	¼ì²âÃüÁî)
MESSAGE_DEF(	CMD_SO_WARING,		CMD_SO_BASE + 3			,	¾¯¸æÃüÁî)
MESSAGE_DEF(	CMD_SO_EXCEPTION,	CMD_SO_BASE + 4		,	Òì³£ÃüÁî)

MESSAGE_DEF(	CMD_SO_ON_LINE,		CMD_SO_BASE + 5			,	ÉÏÏßÃüÁî)
MESSAGE_DEF(	CMD_SO_OFF_LINE,	CMD_SO_BASE + 6			,	ÏÂÏßÃüÁî)
MESSAGE_DEF(	CMD_SO_ENTER_MAP,	CMD_SO_BASE + 7			,	½øÈëµØÍ¼ÃüÁî)
MESSAGE_DEF(	CMD_SO_LEAVE_MAP,	CMD_SO_BASE + 8			,	Àë¿ªµØÍ¼ÃüÁî)

// End

// Add by lark.li 20090601 begin
MESSAGE_DEF(	CMD_AB_LOGIN,		CMD_AB_BASE + 1			,	µÇÂ¼)
MESSAGE_DEF(	CMD_AB_LOGOUT,		CMD_AB_BASE + 2			,	µÇ³ö)

MESSAGE_DEF(	CMD_AB_USER_LOGINT,	CMD_AB_BASE + 3			,	Íæ¼ÒµÇÂ¼)
MESSAGE_DEF(	CMD_AB_USER_LOGOUT,	CMD_AB_BASE + 4			,	Íæ¼ÒµÇ³ö)

MESSAGE_DEF(	CMD_BA_USER_TIME,	CMD_BA_BASE + 1			,	Íæ¼ÒÊ±¼äÍ¨Öª)
MESSAGE_DEF(	CMD_BA_KILL_USER,	CMD_BA_BASE + 2			,	ÌßµôÍæ¼Ò£¨Ä¿Ç°Á½ÖÖÇé¿ö1.ÉÏÏßÊ±Íæ¼ÒÒÑ¾­¹ýÁË5Ð¡Ê±½¡¿µÊ±¼ä2.Íæ¼ÒÈÏÖ¤Ê§°Ü£©)

// End

inline static void InitMessage()
{
	MESSAGE_ADD(	CMD_CM_ROLEBASE,				CMD_CM_BASE + 300,	(300-400)ÈÎÎñÏµÍ³ÃüÁî»ùÊý)
	MESSAGE_ADD(	CMD_CM_GULDBASE,				CMD_CM_BASE	+ 400,	(400-430)¹«»áÃüÁî»ùÖ·)
	MESSAGE_ADD(	CMD_CM_CHARBASE,				CMD_CM_BASE	+ 430,	(430-450)½ÇÉ«ÃüÁî»ùÊý)
	MESSAGE_ADD(	CMD_CM_SAY,						CMD_CM_BASE + 1,	ÊÓÒ°ÄÚËµ»°)
	MESSAGE_ADD(	CMD_CM_BEGINACTION,				CMD_CM_BASE + 6,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_ENDACTION,				CMD_CM_BASE + 7,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_SYNATTR,					CMD_CM_BASE + 8,	Í¬²½½ÇÉ«ÊôÐÔ£¨Í¬²½ÊôÐÔµÄ×îÖÕÖµ£©)
	MESSAGE_ADD(	CMD_CM_SYNSKILLBAG,				CMD_CM_BASE + 9,	Í¬²½¼¼ÄÜÀ¸)
	MESSAGE_ADD(	CMD_CM_DIE_RETURN,				CMD_CM_BASE + 10,	ËÀÍö¸´»î)
	MESSAGE_ADD(	CMD_CM_SKILLUPGRADE,			CMD_CM_BASE + 11,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_PING,					CMD_CM_BASE + 15,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_REFRESH_DATA,			CMD_CM_BASE + 16,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_CHECK_PING,				CMD_CM_BASE + 17,	¼ÆËãPing)
	MESSAGE_ADD(	CMD_CM_MAP_MASK,				CMD_CM_BASE + 18,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_UPDATEHAIR,				CMD_CM_BASE + 20,	¿Í»§¶ËÇëÇó¸ü»»·¢ÐÍ)
	MESSAGE_ADD(	CMD_CM_TEAM_FIGHT_ASK,			CMD_CM_BASE + 21,	¶ÓÎéÌôÕ½ÇëÇó)
	MESSAGE_ADD(	CMD_CM_TEAM_FIGHT_ASR,			CMD_CM_BASE + 22,	¶ÓÎéÌôÕ½Ó¦´ð(ANSWER))
	MESSAGE_ADD(	CMD_CM_ITEM_REPAIR_ASK,			CMD_CM_BASE + 23,	µÀ¾ßÐÞÀíÇëÇó)
	MESSAGE_ADD(	CMD_CM_ITEM_REPAIR_ASR,			CMD_CM_BASE + 24,	µÀ¾ßÐÞÀíÓ¦´ð)
	MESSAGE_ADD(	CMD_CM_ITEM_FORGE_ASK,			CMD_CM_BASE + 25,	µÀ¾ß¾«Á¶ÇëÇó)
	MESSAGE_ADD(	CMD_CM_ITEM_FORGE_ASR,			CMD_CM_BASE + 26,	µÀ¾ß¾«Á¶Ó¦´ð)
	MESSAGE_ADD(	CMD_CM_TIGER_START,				CMD_CM_BASE + 27,	¿ªÆôÀÏ»¢»ú)
	MESSAGE_ADD(	CMD_CM_TIGER_STOP,				CMD_CM_BASE + 28,	Í£Ö¹ÀÏ»¢»ú)
	MESSAGE_ADD(	CMD_CM_ITEM_FORGE_CANACTION,	CMD_CM_BASE + 29,	ÓÐÌØÊâÈÎÎñÒÑÍê³É£¬¿ÉÒÔ×ö±ðµÄÊÂ¡£)
	MESSAGE_ADD(	CMD_CM_KITBAG_LOCK,				CMD_CM_BASE + 31,	±³°üËø¶¨ÇëÇó)
	MESSAGE_ADD(	CMD_CM_KITBAG_UNLOCK,			CMD_CM_BASE + 32,	±³°ü½âËøÇëÇó)
	MESSAGE_ADD(	CMD_CM_KITBAG_CHECK,			CMD_CM_BASE + 33,	¼ì²é±³°üËø¶¨×´Ì¬)
	MESSAGE_ADD(	CMD_CM_KITBAG_AUTOLOCK,			CMD_CM_BASE + 34,	ÏÂÏß×Ô¶¯Ëø¶¨)
	MESSAGE_ADD(	CMD_CM_KITBAGTEMP_SYNC,			CMD_CM_BASE + 35,	Í¬²½ÁÙÊ±±³°ü)
	MESSAGE_ADD(	CMD_CM_STORE_OPEN_ASK,			CMD_CM_BASE + 41,	´ò¿ªÉÌ³Ç)
	MESSAGE_ADD(	CMD_CM_STORE_LIST_ASK,			CMD_CM_BASE + 42,	ÉÌ³ÇÁÐ±í)
	MESSAGE_ADD(	CMD_CM_STORE_BUY_ASK,			CMD_CM_BASE + 43,	¹ºÂòµÀ¾ß)
	MESSAGE_ADD(	CMD_CM_STORE_CHANGE_ASK,		CMD_CM_BASE + 44,	¶Ò»»´ú±Ò)
	MESSAGE_ADD(	CMD_CM_STORE_QUERY,				CMD_CM_BASE + 45,	½»Ò×¼ÇÂ¼²éÑ¯)
	MESSAGE_ADD(	CMD_CM_STORE_VIP,				CMD_CM_BASE + 46,	VIP)
	MESSAGE_ADD(	CMD_CM_STORE_AFFICHE,			CMD_CM_BASE + 47,	¹«¸æÉÌÆ·)
	MESSAGE_ADD(	CMD_CM_STORE_CLOSE,				CMD_CM_BASE + 48,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_BLACKMARKET_EXCHANGE_REQ,CMD_CM_BASE + 51,	ºÚÊÐ¶Ò»»)
	MESSAGE_ADD(	CMD_CM_CHEAT_CHECK,				CMD_CM_BASE + 52,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_VOLUNTER_LIST,			CMD_CM_BASE + 61,	ÏÔÊ¾Ö¾Ô¸ÕßÁÐ±í)
	MESSAGE_ADD(	CMD_CM_VOLUNTER_ADD,			CMD_CM_BASE + 62,	Ìí¼Óµ½Ö¾Ô¸ÕßÁÐ±í)
	MESSAGE_ADD(	CMD_CM_VOLUNTER_DEL,			CMD_CM_BASE + 63,	´ÓÁÐ±íÖÐÉ¾³ý)
	MESSAGE_ADD(	CMD_CM_VOLUNTER_SEL,			CMD_CM_BASE + 64,	ÉêÇë×é¶Ó)
	MESSAGE_ADD(	CMD_CM_VOLUNTER_OPEN,			CMD_CM_BASE + 65,	´ò¿ªÖ¾Ô¸ÕßÃæ°å)
	MESSAGE_ADD(	CMD_CM_VOLUNTER_ASR,			CMD_CM_BASE + 66,	Ö¾Ô¸Õß×é¶ÓÓ¦´ð)
	MESSAGE_ADD(	CMD_CM_MASTER_INVITE,			CMD_CM_BASE + 71,	°ÝÊ¦)
	MESSAGE_ADD(	CMD_CM_MASTER_ASR,				CMD_CM_BASE + 72,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_MASTER_DEL,				CMD_CM_BASE + 73,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_PRENTICE_DEL,			CMD_CM_BASE + 74,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_PRENTICE_INVITE,			CMD_CM_BASE + 75,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_PRENTICE_ASR,			CMD_CM_BASE + 76,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_LIFESKILL_ASR,			CMD_CM_BASE + 80,	Éú»î¼¼ÄÜÓÃÔË£¬ºÏ³É£¬·Ö½âµÈµÄÇëÇó)
	MESSAGE_ADD(	CMD_CM_LIFESKILL_ASK,			CMD_CM_BASE + 81,	Éú»î¼¼ÄÜÓÃÔË£¬ºÏ³É£¬·Ö½âµÈµÄÈ·ÈÏ)
	MESSAGE_ADD(	CMD_CM_BIDUP,					CMD_CM_BASE + 86,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_SAY2CAMP,				CMD_CM_BASE + 91,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_GM_SEND,					CMD_CM_BASE + 92,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_GM_RECV,					CMD_CM_BASE + 93,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_PK_CTRL,					CMD_CM_BASE + 94,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_ITEM_LOTTERY_ASK,		CMD_CM_BASE + 95,	µÀ¾ß¾«Á¶ÇëÇó)
	MESSAGE_ADD(	CMD_CM_ITEM_LOTTERY_ASR,		CMD_CM_BASE + 96,	²ÊÆ±Éè¶¨Ó¦´ð)
	MESSAGE_ADD(	CMD_CM_CAPTAIN_CONFIRM_ASR,		CMD_CM_BASE + 97,	¾º¼¼³¡¶Ó³¤Ó¦´ð)
	MESSAGE_ADD(	CMD_CM_ITEM_AMPHITHEATER_ASK,	CMD_CM_BASE + 98,	Í¶Æ±·¢ËÍ)
	MESSAGE_ADD(	CMD_CM_ITEM_LOCK_ASK,			CMD_CM_BASE	+ 99,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_ITEM_UNLOCK_ASK,			CMD_CM_BASE + 100,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_SYNCHAR,					CMD_CM_BASE + 101, sync)
	MESSAGE_ADD(    CMD_CM_GETORIGNAMECOLOR,        CMD_CM_BASE + 102, GetOldColor)
	MESSAGE_ADD(	CMD_CM_CHANGETITLE,				CMD_CM_BASE + 103, ChangeTitle)
	MESSAGE_ADD(	CMD_CM_SYNGUILDCOLOR,			CMD_CM_BASE + 104, SynGuildColor)
	MESSAGE_ADD(	CMD_CM_GUILD_PUTNAME,		CMD_CM_GULDBASE	+ 1,	¿Í»§¶ËÊäÈë¹«»áÃû)
	MESSAGE_ADD(	CMD_CM_GUILD_TRYFOR,		CMD_CM_GULDBASE	+ 2,	ÉêÇë¼ÓÈë¹«»á)
	MESSAGE_ADD(	CMD_CM_GUILD_TRYFORCFM,		CMD_CM_GULDBASE	+ 3,	È·ÈÏÊÇ·ñ¸²¸ÇÒÔÇ°ÉêÇë)
	MESSAGE_ADD(	CMD_CM_GUILD_LISTTRYPLAYER,	CMD_CM_GULDBASE	+ 4,	ÁÐ³öËùÓÐ´ýÅú¸´Íæ¼Ò)
	MESSAGE_ADD(	CMD_CM_GUILD_APPROVE,		CMD_CM_GULDBASE	+ 5,	ÃÅÖíÅú×¼ÉêÇëÈË¼ÓÈë)
	MESSAGE_ADD(	CMD_CM_GUILD_REJECT,		CMD_CM_GULDBASE	+ 6,	ÃÅÖí¾Ü¾øÉêÇëÈË¼ÓÈë)
	MESSAGE_ADD(	CMD_CM_GUILD_KICK,			CMD_CM_GULDBASE	+ 7,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_GUILD_LEAVE,			CMD_CM_GULDBASE	+ 8,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_GUILD_DISBAND,		CMD_CM_GULDBASE	+ 9,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_GUILD_MOTTO,			CMD_CM_GULDBASE	+ 10,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_GUILD_CHALLENGE,		CMD_CM_GULDBASE + 11,	ÌôÕ½¹«»á)
	MESSAGE_ADD(	CMD_CM_GUILD_LEIZHU,		CMD_CM_GULDBASE + 12,	ÇÀÀÞÖ÷)
	MESSAGE_DEF(CMD_CM_GUILD_PERM, CMD_CM_GULDBASE + 13, Guild Perm)
	MESSAGE_ADD(	CMD_CM_LOGIN,				CMD_CM_CHARBASE + 1,	ÓÃ»§ÕÊºÅµÇÈë(ÓÃ»§Ãû/ÃÜÂë¶ÔÈÏÖ¤))
	MESSAGE_ADD(	CMD_CM_LOGOUT,				CMD_CM_CHARBASE + 2,	ÓÃ»§ÕÊºÅµÇ³ö)
	MESSAGE_ADD(	CMD_CM_BGNPLAY,				CMD_CM_CHARBASE + 3,	ÓÃ»§Ñ¡Ôñ½ÇÉ«¿ªÊ¼ÍæÓÎÏ·)
	MESSAGE_ADD(	CMD_CM_ENDPLAY,				CMD_CM_CHARBASE + 4,	ÓÃ»§½áÊøÍæÓÎÏ·½øÈëÑ¡/½¨/É¾½ÇÉ«»­Ãæ)
	MESSAGE_ADD(	CMD_CM_NEWCHA,				CMD_CM_CHARBASE	+ 5,	ÓÃ»§ÐÂ½¨½ÇÉ«)
	MESSAGE_ADD(	CMD_CM_DELCHA,				CMD_CM_CHARBASE	+ 6,	ÓÃ»§É¾³ý½ÇÉ«)
	MESSAGE_ADD(	CMD_CM_CANCELEXIT,			CMD_CM_CHARBASE + 7,	ÓÃ»§ÒªÇóÈ¡ÏûÍË³ö)
	MESSAGE_ADD(	CMD_CM_REQUESTNPC,			CMD_CM_ROLEBASE + 1,	¿Í»§¶ËÇëÇóºÍNPC»¥¶¯)
	MESSAGE_ADD(	CMD_CM_TALKPAGE,			CMD_CM_ROLEBASE + 2,	¿Í»§¶ËÇëÇóÓëNPC¶Ô»°Ò³ÃüÁî)
	MESSAGE_ADD(	CMD_CM_FUNCITEM,			CMD_CM_ROLEBASE + 3,	¿Í»§¶ËÇëÇóÖ¸¶¨¹¦ÄÜÑ¡Ïî²Ù×÷)
	MESSAGE_ADD(	CMD_CM_REQUESTTRADE,		CMD_CM_ROLEBASE + 8,	¿Í»§¶ËÇëÇó½øÐÐ½»Ò×)
	MESSAGE_ADD(	CMD_CM_TRADEITEM,			CMD_CM_ROLEBASE + 9,	¿Í»§¶ËÇëÇó½»Ò×ÎïÆ·)
	MESSAGE_ADD(	CMD_CM_REQUESTAGENCY,		CMD_CM_ROLEBASE + 10,	¿Í»§¶ËÇëÇónpc´úÀí½»Ò×)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_REQUEST,	CMD_CM_ROLEBASE + 12,	·¢Æð½»Ò×ÑûÇë)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_ACCEPT,	CMD_CM_ROLEBASE + 13,	½ÓÊÜ½»Ò×ÑûÇë)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_REJECT,	CMD_CM_ROLEBASE + 14,	¾Ü¾ø½»Ò×ÑûÇë)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_CANCEL,	CMD_CM_ROLEBASE + 15,	È¡Ïû½»Ò×)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_ITEM,		CMD_CM_ROLEBASE + 16,	½ÇÉ«½»Ò×ÎïÆ·)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_VALIDATEDATA,CMD_CM_ROLEBASE + 17,	È·ÈÏ½»Ò×Êý¾Ý)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_VALIDATE,	CMD_CM_ROLEBASE + 18,	È·ÈÏ½»Ò×)
	MESSAGE_ADD(	CMD_CM_CHARTRADE_MONEY,		CMD_CM_ROLEBASE + 19,	ÍÏ¶¯½ðÇ®)
	MESSAGE_ADD(	CMD_CM_MISSION,				CMD_CM_ROLEBASE + 22,	¿Í»§¶ËÇëÇóÈÎÎñ´¦Àí)
	MESSAGE_ADD(	CMD_CM_MISSIONLIST,			CMD_CM_ROLEBASE + 23,	ÈÎÎñÁÐ±í)
	MESSAGE_ADD(	CMD_CM_MISSIONTALK,			CMD_CM_ROLEBASE + 24,	ÈÎÎñ¶Ô»°)
	MESSAGE_ADD(	CMD_CM_MISLOG,				CMD_CM_ROLEBASE + 25,	ÈÎÎñÈÕÖ¾)
	MESSAGE_ADD(	CMD_CM_MISLOGINFO,			CMD_CM_ROLEBASE + 26,	µ¥¸öÈÎÎñÏêÏ¸ÐÅÏ¢)
	MESSAGE_ADD(	CMD_CM_MISLOG_CLEAR,		CMD_CM_ROLEBASE + 27,	Ç×ÇóÉ¾³ýÒ»¸öÈÎÎñ)
	MESSAGE_ADD(	CMD_CM_STALL_ALLDATA,		CMD_CM_ROLEBASE + 30,	ÇëÇó°ÚÌ¯Êý¾ÝÐÅÏ¢)
	MESSAGE_ADD(	CMD_CM_STALL_OPEN,			CMD_CM_ROLEBASE + 31,	ÇëÇó´ò¿ª°ÚÌ¯»õÎïÐÅÏ¢)
	MESSAGE_ADD(	CMD_CM_STALL_BUY,			CMD_CM_ROLEBASE + 32,	ÇëÇó¹ºÂò°ÚÌ¯»õÎï)
	MESSAGE_ADD(	CMD_CM_STALL_CLOSE,			CMD_CM_ROLEBASE + 33,	ÇëÇóÊÕÌ¯)
	MESSAGE_ADD(	CMD_CM_CREATE_BOAT,			CMD_CM_ROLEBASE + 38,	ÇëÇó´´½¨´¬)
	MESSAGE_ADD(	CMD_CM_UPDATEBOAT_PART,		CMD_CM_ROLEBASE + 39,	ÇëÇó¸ü»»´¬Ö»²¿¼þ)
	MESSAGE_ADD(	CMD_CM_BOAT_CANCEL,			CMD_CM_ROLEBASE + 40,	ÇëÇóÈ¡Ïû½¨Ôì´¬Ö»)
	MESSAGE_ADD(	CMD_CM_BOAT_LUANCH,			CMD_CM_ROLEBASE + 41,	Ñ¡Ôñ´¬Ö»³öº£)
	MESSAGE_ADD(	CMD_CM_BOAT_BAGSEL,			CMD_CM_ROLEBASE + 42,	Ñ¡ÔñÒ»ËÒ´¬Ö»»õ²Õ´ò°üÏÖÓÐ»õÎï)
	MESSAGE_ADD(	CMD_CM_BOAT_SELECT,			CMD_CM_ROLEBASE + 43,	Ñ¡Ôñ´¬Ö»)
	MESSAGE_ADD(	CMD_CM_BOAT_GETINFO,		CMD_CM_ROLEBASE + 44,	»ñÈ¡´¬Ö»ÐÅÏ¢)
	MESSAGE_ADD(	CMD_CM_ENTITY_EVENT,		CMD_CM_ROLEBASE + 45,	´¥·¢ÊµÌåÊÂ¼þ)
	MESSAGE_ADD(	CMD_CM_CREATE_PASSWORD2,	CMD_CM_ROLEBASE + 46,	´´½¨¶þ´ÎÃÜÂë)
	MESSAGE_ADD(	CMD_CM_UPDATE_PASSWORD2,	CMD_CM_ROLEBASE + 47,	¸üÐÂ¶þ´ÎÃÜÂë)
	MESSAGE_ADD(	CMD_CM_READBOOK_START,		CMD_CM_ROLEBASE + 48,	¿ªÊ¼¶ÁÊé)
	MESSAGE_ADD(	CMD_CM_READBOOK_CLOSE,		CMD_CM_ROLEBASE + 49,	½áÊø¶ÁÊé)
	MESSAGE_ADD(	CMD_CM_SELECT_TIME_ASK,		CMD_CM_ROLEBASE + 50,	Ñ¡ÔñµÄÀëÏßÊ±¼ä)
	MESSAGE_ADD(	CMD_CM_BEHINDGENIUS_ASK,	CMD_CM_ROLEBASE + 51,	ÇëÇóÖÆÔì±³ºóÁé)
	MESSAGE_ADD(	CMD_CM_GET_OUTTIMEEXP,		CMD_CM_ROLEBASE + 52,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_SELECT_COIN_ASK,		CMD_CM_ROLEBASE + 53,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_REQUEST_ANYTIMETRADE,CMD_CM_ROLEBASE + 54,	ËæÊ±½»Ò×)
	MESSAGE_ADD(	CMD_CM_REQUEST_JEWELRYUP_ASK,CMD_CM_ROLEBASE + 55,	±¦Ê¯Éý¼¶)
	MESSAGE_ADD(	CMD_CM_OPEN_CRYSTALTRADE_ASK,CMD_CM_ROLEBASE + 56,	´ò¿ªË®¾§½»Ò×Æ½Ì¨)
	MESSAGE_ADD(	CMD_CM_REQUEST_BUY_ASK,		CMD_CM_ROLEBASE + 57,	ÂòÈë¹Òµ¥)
	MESSAGE_ADD(	CMD_CM_REQUEST_SALE_ASK,	CMD_CM_ROLEBASE + 58,	Âô³ö¹Òµ¥)
	MESSAGE_ADD(	CMD_CM_REQUEST_CANCEL_ASK,	CMD_CM_ROLEBASE + 59,	È¡Ïû¹Òµ¥)
	MESSAGE_ADD(	CMD_CM_REQUEST_GETFLATMONEY,CMD_CM_ROLEBASE + 60,	È¡³öÆ½Ì¨ÕËºÅÉÏµÄÇ®)
	MESSAGE_ADD(	CMD_CM_REQUEST_REFRESH_ASK,	CMD_CM_ROLEBASE + 61,	ÇëÇóË¢ÐÂ)
	MESSAGE_ADD(	CMD_CM_CHA_CONFIRM_CRYSTALTRADE,CMD_CM_ROLEBASE + 62,	Íæ¼ÒÈ·ÈÏ½»Ò×ÐÅÏ¢)
	MESSAGE_ADD(	CMD_CM_GARNER2_REORDER,			CMD_CM_ROLEBASE + 71,	¿Í»§¶ËÇëÇóÖØÅÅÃû)
	MESSAGE_ADD(	CMD_CM_ANTIINDULGENCE,			CMD_CM_ROLEBASE + 72,	·À³ÁÃÔÆÆ½â)
	MESSAGE_ADD(	XTRAP_CMD_STEP_TWO,				CMD_CM_ROLEBASE + 73,	ÎÞ)
	MESSAGE_ADD(	XTRAP_MAP_FILE_CHANGE,			CMD_CM_ROLEBASE + 74,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_OPEN_CYCELMISSION,		CMD_CM_ROLEBASE + 75,	¹«»áÑ­»·ÈÎÎñ)//Add by sunny.sun 20090831
	MESSAGE_ADD(	CMD_MC_ROLEBASE,				CMD_MC_BASE	+ 300,	(300-400)ÈÎÎñÏµÍ³ÃüÁî»ùÖ·)
	MESSAGE_ADD(	CMD_MC_GULDBASE,				CMD_MC_BASE	+ 400,	(400-430)¹«»áÃüÁî»ùÖ·)
	MESSAGE_ADD(	CMD_MC_CHARBASE,				CMD_MC_BASE	+ 430,	(430-450)½ÇÉ«ÃüÁî»ùÖ·)
	MESSAGE_ADD(	CMD_MC_SAY,						CMD_MC_BASE + 1,	ÊÓÒ°ÄÚËµ»°)
	MESSAGE_ADD(	CMD_MC_MAPCRASH,				CMD_MC_BASE	+ 3,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_CHABEGINSEE,				CMD_MC_BASE + 4,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_CHAENDSEE,				CMD_MC_BASE + 5,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_ITEMBEGINSEE,			CMD_MC_BASE + 6,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_ITEMENDSEE,				CMD_MC_BASE + 7,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_NOTIACTION,				CMD_MC_BASE + 8,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_SYNATTR,					CMD_MC_BASE + 9,	Í¬²½½ÇÉ«ÊôÐÔ£¨Í¬²½ÊôÐÔµÄ×îÖÕÖµ£©)
	MESSAGE_ADD(	CMD_MC_SYNSKILLBAG,				CMD_MC_BASE + 10,	Í¬²½¼¼ÄÜÀ¸)
	MESSAGE_ADD(	CMD_MC_SYNASKILLSTATE,			CMD_MC_BASE + 11,	Í¬²½½ÇÉ«×´Ì¬)
	MESSAGE_ADD(	CMD_MC_PING,					CMD_MC_BASE + 15,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_ENTERMAP,				CMD_MC_BASE	+ 16,	GameServerÍ¨ÖªClient½øÈëµØÍ¼£¬¸½´øÍæ¼ÒµÄËùÓÐÐÅÏ¢¼°Æä¸½Êô½á¹¹)
	MESSAGE_ADD(	CMD_MC_SYSINFO,					CMD_MC_BASE	+ 17,	ÏµÍ³ÏûÏ¢ÌáÊ¾ÃüÁî£¬ÈçÍæ¼Ò²»ÔÚÏß£¬Äã»¹Ã»ÓÐÍê³ÉÊ²Ã´ÈÎÎñµÈµÈ¡£)
	MESSAGE_ADD(	CMD_MC_ALARM,					CMD_MC_BASE	+ 18,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_TEAM,					CMD_MC_BASE + 19,	GameServerÍ¨Öª¿Í»§¶ËÄ³¸ö¶ÓÔ±µÄÊôÐÔË¢ÐÂ(±ÈÈçÍâ¹Û, HP,SP))
	MESSAGE_ADD(	CMD_MC_FAILEDACTION,			CMD_MC_BASE + 20,	ÇëÇóµÄÐÐ¶¯Ö´ÐÐÊ§°Ü)
	MESSAGE_ADD(	CMD_MC_MESSAGE,					CMD_MC_BASE + 21,	ÌáÊ¾ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_ASTATEBEGINSEE,			CMD_MC_BASE + 22,	µØÃæ×´Ì¬)
	MESSAGE_ADD(	CMD_MC_ASTATEENDSEE,			CMD_MC_BASE + 23,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_TLEADER_ID,				CMD_MC_BASE + 24,	GameServerÍ¨Öª¿Í»§¶ËÄ³¸ö¶ÓÔ±µÄ¶Ó³¤ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_CHA_EMOTION,				CMD_MC_BASE + 25,	GameServer´¥·¢Ö¸¶¨½ÇÉ«×ö±íÇé   )
	MESSAGE_ADD(	CMD_MC_QUERY_CHA,				CMD_MC_BASE + 26,	²éÑ¯½ÇÉ«»ù±¾ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_QUERY_CHAITEM,			CMD_MC_BASE + 27,	²éÑ¯½ÇÉ«µÀ¾ß)
	MESSAGE_ADD(	CMD_MC_CALL_CHA,				CMD_MC_BASE + 28,	½«½ÇÉ«ÕÙ»½µ½×Ô¼ºÉí±ß)
	MESSAGE_ADD(	CMD_MC_GOTO_CHA,				CMD_MC_BASE + 29,	½«×Ô¼º´«ËÍµ½½ÇÉ«Éí±ß)
	MESSAGE_ADD(	CMD_MC_KICK_CHA,				CMD_MC_BASE + 30,	½«½ÇÉ«ÌßÏÂÏß)
	MESSAGE_ADD(	CMD_MC_SYNDEFAULTSKILL,			CMD_MC_BASE + 31,	Í¬²½Ä¬ÈÏ¼¼ÄÜ)
	MESSAGE_ADD(	CMD_MC_ADD_ITEM_CHA,			CMD_MC_BASE + 32,	Ìí¼ÓµÀ¾ß½ÇÉ«)
	MESSAGE_ADD(	CMD_MC_DEL_ITEM_CHA,			CMD_MC_BASE + 33,	É¾³ýµÀ¾ß½ÇÉ«)
	MESSAGE_ADD(	CMD_MC_QUERY_CHAPING,			CMD_MC_BASE + 34,	²éÑ¯½ÇÉ«µ½GameServerÂß¼­²ãµÄpingÖµ)
	MESSAGE_ADD(	CMD_MC_QUERY_RELIVE,			CMD_MC_BASE + 35,	×ÉÑ¯ÊÇ·ñ½ÓÊÜÔ­µØ¸´»î)
	MESSAGE_ADD(	CMD_MC_PREMOVE_TIME,			CMD_MC_BASE + 36,	Ô¤ÒÆ¶¯Ê±¼ä)
	MESSAGE_ADD(	CMD_MC_CHECK_PING,				CMD_MC_BASE + 37,	¼ÆËãPing)
	MESSAGE_ADD(	CMD_MC_MAP_MASK,				CMD_MC_BASE + 38,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_OPENHAIR,				CMD_MC_BASE + 39,	´ò¿ªÀí·¢½çÃæ)
	MESSAGE_ADD(	CMD_MC_UPDATEHAIR_RES,			CMD_MC_BASE + 40,	¸ü»»·¢ÐÍµÄ½á¹û)
	MESSAGE_ADD(	CMD_MC_EVENT_INFO,				CMD_MC_BASE + 41,	Í¬²½ÊÂ¼þÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_SIDE_INFO,				CMD_MC_BASE + 42,	Í¬²½·Ö±ßÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_TEAM_FIGHT_ASK,			CMD_MC_BASE + 43,	¶ÓÎéÌôÕ½ÇëÇó)
	MESSAGE_ADD(	CMD_MC_ITEM_REPAIR_ASK,			CMD_MC_BASE + 44,	µÀ¾ßÐÞÀíÇëÇó)
	MESSAGE_ADD(	CMD_MC_ITEM_REPAIR_ASR,			CMD_MC_BASE + 45,	µÀ¾ßÐÞÀíÓ¦´ð)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_REPAIR,		CMD_MC_BASE + 46,	¿ªÊ¼µÀ¾ßÐÞÀí)
	MESSAGE_ADD(	CMD_MC_APPEND_LOOK,				CMD_MC_BASE + 47,	¸½¼ÓÍâ¹Û)
	MESSAGE_ADD(	CMD_MC_ITEM_FORGE_ASK,			CMD_MC_BASE + 48,	µÀ¾ß¾«Á¶ÇëÇó)
	MESSAGE_ADD(	CMD_MC_ITEM_FORGE_ASR,			CMD_MC_BASE + 49,	µÀ¾ß¾«Á¶Ó¦´ð)
	MESSAGE_ADD(	CMD_MC_ITEM_USE_SUC,			CMD_MC_BASE + 50,	µÀ¾ßÊ¹ÓÃ³É¹¦)
	MESSAGE_ADD(	CMD_MC_KITBAG_CAPACITY,			CMD_MC_BASE + 51,	±³°üÈÝÁ¿)
	MESSAGE_ADD(	CMD_MC_ESPE_ITEM,				CMD_MC_BASE + 52,	ÌØÊâµÀ¾ß)
	MESSAGE_ADD(	CMD_MC_KITBAG_CHECK_ASR,		CMD_MC_BASE + 53,	±³°üËø¶¨×´Ì¬µÄÓ¦´ð)
	MESSAGE_ADD(	CMD_MC_KITBAGTEMP_SYNC,			CMD_MC_BASE + 54,	Í¬²½ÁÙÊ±±³°ü)
	MESSAGE_ADD(	CMD_MC_STORE_OPEN_ASR,			CMD_MC_BASE + 61,	´ò¿ªÉÌ³Ç)
	MESSAGE_ADD(	CMD_MC_STORE_LIST_ASR,			CMD_MC_BASE + 62,	ÉÌ³ÇÁÐ±í)
	MESSAGE_ADD(	CMD_MC_STORE_BUY_ASR,			CMD_MC_BASE + 63,	¹ºÂòµÀ¾ß)
	MESSAGE_ADD(	CMD_MC_STORE_CHANGE_ASR,		CMD_MC_BASE + 64,	¶Ò»»´ú±Ò)
	MESSAGE_ADD(	CMD_MC_STORE_QUERY,				CMD_MC_BASE + 65,	½»Ò×¼ÇÂ¼²éÑ¯)
	MESSAGE_ADD(	CMD_MC_STORE_VIP,				CMD_MC_BASE + 66,	VIP)
	MESSAGE_ADD(	CMD_MC_STORE_AFFICHE,			CMD_MC_BASE + 67,	¹«¸æÉÌÆ·)
	MESSAGE_ADD(	CMD_MC_POPUP_NOTICE,			CMD_MC_BASE + 68,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_BLACKMARKET_EXCHANGEDATA,	CMD_MC_BASE + 71,	ºÚÊÐ¶Ò»»Êý¾Ý)
	MESSAGE_ADD(	CMD_MC_BLACKMARKET_EXCHANGE_ASR,	CMD_MC_BASE + 72,	ºÚÊÐ¶Ò»»Ó¦´ð)
	MESSAGE_ADD(	CMD_MC_BLACKMARKET_EXCHANGEUPDATE,	CMD_MC_BASE + 73,	ºÚÊÐ¶Ò»»¸üÐÂ)
	MESSAGE_ADD(	CMD_MC_BLACKMARKET_TRADEUPDATE,	CMD_MC_BASE + 74,	ºÚÊÐ½»Ò×¸üÐÂ)
	MESSAGE_ADD(	CMD_MC_EXCHANGEDATA,			CMD_MC_BASE + 75,	ÆÕÍ¨¶Ò»»Êý¾Ý)
	MESSAGE_ADD(	CMD_MC_VOLUNTER_LIST,			CMD_MC_BASE + 81,	ÏÔÊ¾Ö¾Ô¸ÕßÁÐ±í)
	MESSAGE_ADD(	CMD_MC_VOLUNTER_STATE,			CMD_MC_BASE + 82,	Ö¾Ô¸Õß×´Ì¬)
	MESSAGE_ADD(	CMD_MC_VOLUNTER_SEL,			CMD_MC_BASE + 83,	ÉêÇë×é¶Ó)
	MESSAGE_ADD(	CMD_MC_VOLUNTER_OPEN,			CMD_MC_BASE + 84,	´ò¿ªÖ¾Ô¸ÕßÃæ°å)
	MESSAGE_ADD(	CMD_MC_VOLUNTER_ASK,			CMD_MC_BASE + 85,	Ö¾Ô¸Õß×é¶ÓÉêÇë)
	MESSAGE_ADD(	CMD_MC_LISTAUCTION,				CMD_MC_BASE + 86,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_MASTER_ASK,				CMD_MC_BASE + 91,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_PRENTICE_ASK,			CMD_MC_BASE + 92,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_CHAPLAYEFFECT,			CMD_MC_BASE + 93,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_SAY2CAMP,				CMD_MC_BASE + 96,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_GM_MAIL,					CMD_MC_BASE + 97,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_CHEAT_CHECK,				CMD_MC_BASE + 98,	ÎÞ)
	MESSAGE_ADD(	CMD_CM_ITEM_LOCK_ASR,			CMD_MC_BASE	+	99,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_ITEM_UNLOCK_ASR,			CMD_MC_BASE	+	95,	ÎÞ)
	MESSAGE_ADD(CMD_MC_PORTALTIMES, CMD_MC_BASE + 109, portal)

	// FEATURE: GUILD_BANL
	MESSAGE_ADD(CMD_MC_UPDATEGUILDBANKGOLD, CMD_MC_BASE + 120, Update Guild Bank Gold)

	MESSAGE_ADD(	CMD_MC_GUILD_GETNAME,		CMD_MC_GULDBASE	+ 1,	ÇëÇó¿Í»§¶ËÊäÈë¹«»áÃû)
	MESSAGE_ADD(	CMD_MC_LISTGUILD,			CMD_MC_GULDBASE	+ 2,	ÃüÁî¿Í»§¶Ë´ò¿ªÉêÇë¼ÓÈë´°¿Ú²¢¼ÓÈëÐÐ»áÁÐ±í)
	MESSAGE_ADD(	CMD_MC_GUILD_TRYFORCFM,		CMD_MC_GULDBASE	+ 3,	È·ÈÏÊÇ·ñ¸²¸ÇÒÔÇ°ÉêÇë)
	MESSAGE_ADD(	CMD_MC_GUILD_LISTTRYPLAYER,	CMD_MC_GULDBASE	+ 4,	ÁÐ³öËùÓÐ´ýÅú¸´Íæ¼Ò)
	MESSAGE_ADD(	CMD_MC_GUILD_LISTCHALL,		CMD_MC_GULDBASE + 5,	²éÑ¯¹«»áÌôÕ½ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_GUILD_MOTTO,			CMD_MC_GULDBASE	+ 10,	ÐÞ¸Ä×ùÓÒÃú)
	MESSAGE_ADD(	CMD_MC_GUILD_LEAVE,			CMD_MC_GULDBASE + 11,	Àë¿ª¹«»á)
	MESSAGE_ADD(	CMD_MC_GUILD_KICK,			CMD_MC_GULDBASE + 12,	±»Ìß³ö¹«»á)
	MESSAGE_ADD(	CMD_MC_GUILD_INFO,			CMD_MC_GULDBASE + 13,	Í¬²½¹«»áÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_LOGIN,				CMD_MC_CHARBASE + 1,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_LOGOUT,				CMD_MC_CHARBASE + 2,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_BGNPLAY,				CMD_MC_CHARBASE + 3,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_ENDPLAY,				CMD_MC_CHARBASE + 4,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_NEWCHA,				CMD_MC_CHARBASE	+ 5,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_DELCHA,				CMD_MC_CHARBASE	+ 6,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_STARTEXIT,			CMD_MC_CHARBASE + 7	,	¿ªÊ¼ÍË³öµ¹¼ÆÊ±)
	MESSAGE_ADD(	CMD_MC_CANCELEXIT,			CMD_MC_CHARBASE + 8	,	È¡ÏûÍË³öµ¹¼ÆÊ±)
	MESSAGE_ADD(	CMD_MC_CHAPSTR,				CMD_MC_CHARBASE	+ 10,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_CREATE_PASSWORD2,	CMD_MC_CHARBASE + 11,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_UPDATE_PASSWORD2,	CMD_MC_CHARBASE + 12,	ÎÞ)
	MESSAGE_ADD(	CMD_MC_SYNCCHAR, CMD_MC_CHARBASE + 13, ÎÞ)
	MESSAGE_ADD(    CMD_MC_GETORIGCOLOR, CMD_MC_CHARBASE + 14, GetOrigColor)
	MESSAGE_ADD(    CMD_MC_CHANGETITLE,	CMD_MC_CHARBASE + 15, SetTitle)
	MESSAGE_ADD(	CMD_MC_SYNGUILDCOLOR, CMD_MC_CHARBASE + 16, SynGuildColor)
	MESSAGE_ADD(	CMD_MC_TALKPAGE,			CMD_MC_ROLEBASE + 1,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ¶Ô»°ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_FUNCPAGE,			CMD_MC_ROLEBASE + 2,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ¹¦ÄÜÑ¡ÏîÒ³ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_CLOSETALK,			CMD_MC_ROLEBASE + 3,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ¹Ø±Õ¶Ô»°ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_HELPINFO,			CMD_MC_ROLEBASE + 4,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ°ïÖúÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_TRADEPAGE,			CMD_MC_ROLEBASE + 5,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ½»Ò×ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_TRADERESULT,			CMD_MC_ROLEBASE + 6,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ½»Ò×ÇëÇó´ð¸´ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_TRADE_DATA,			CMD_MC_ROLEBASE + 7,	NPC½»Ò×Êý¾ÝÐÅÏ¢¸üÐÂ)
	MESSAGE_ADD(	CMD_MC_TRADE_ALLDATA,		CMD_MC_ROLEBASE + 8,	NPC½»Ò×Êý¾ÝÐÅÏ¢¸üÐÂ)
	MESSAGE_ADD(	CMD_MC_TRADE_UPDATE,		CMD_MC_ROLEBASE + 9,	ºÚÊÐÉÌÈË½»Ò×ÐÅÏ¢¸üÐÂ)
	MESSAGE_ADD(	CMD_MC_EXCHANGE_UPDATE,		CMD_MC_ROLEBASE + 10,	ºÚÊÐÉÌÈË¶Ò»»ÐÅÏ¢¸üÐÂ)
	MESSAGE_ADD(	CMD_MC_CHARTRADE,			CMD_MC_ROLEBASE + 11,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ½»Ò×ÐÅÏ¢ )
	MESSAGE_ADD(	CMD_MC_CHARTRADE_REQUEST,	CMD_MC_ROLEBASE + 12,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍÆäËû½ÇÉ«ÑûÇë½»Ò×ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_CHARTRADE_CANCEL,	CMD_MC_ROLEBASE + 13,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ )
	MESSAGE_ADD(	CMD_MC_CHARTRADE_ITEM,		CMD_MC_ROLEBASE + 14,	·þÎñÆ÷Ïò¿Í»§¶Ë½ÇÉ«·¢ËÍ½»Ò×µÄÎïÆ·ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_CHARTRADE_PAGE,		CMD_MC_ROLEBASE + 15,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍ¿ªÊ¼½»Ò×½çÃæÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_CHARTRADE_VALIDATEDATA,	CMD_MC_ROLEBASE + 16,	È·ÈÏ½»Ò×Êý¾Ý)
	MESSAGE_ADD(	CMD_MC_CHARTRADE_VALIDATE,	CMD_MC_ROLEBASE + 17,	È·ÈÏ½»Ò×)
	MESSAGE_ADD(	CMD_MC_CHARTRADE_RESULT,	CMD_MC_ROLEBASE + 18,	È·ÈÏ½»Ò×³É¹¦)
	MESSAGE_ADD(	CMD_MC_CHARTRADE_MONEY,		CMD_MC_ROLEBASE + 19,	ÍÏ¶¯½ðÇ®²Ù×÷)
	MESSAGE_ADD(	CMD_MC_MISSION,				CMD_MC_ROLEBASE + 22,	·þÎñÆ÷Ïò¿Í»§¶Ë·¢ËÍÈÎÎñÁÐ±íÒ³ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_MISSIONLIST,			CMD_MC_ROLEBASE + 23,	ÈÎÎñÁÐ±í)
	MESSAGE_ADD(	CMD_MC_MISSIONTALK,			CMD_MC_ROLEBASE + 24,	ÈÎÎñ¶Ô»°)
	MESSAGE_ADD(	CMD_MC_NPCSTATECHG,			CMD_MC_ROLEBASE + 25,	ÇëÇóÇÐ»»npc×´Ì¬)
	MESSAGE_ADD(	CMD_MC_TRIGGER_ACTION,		CMD_MC_ROLEBASE + 26,	´¥·¢Æ÷¶¯×÷)
	MESSAGE_ADD(	CMD_MC_MISPAGE,				CMD_MC_ROLEBASE + 27,	ÈÎÎñÒ³ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_MISLOG,				CMD_MC_ROLEBASE + 28,	ÈÎÎñÈÕÖ¾)
	MESSAGE_ADD(	CMD_MC_MISLOGINFO,			CMD_MC_ROLEBASE + 29,	µ¥¸öÈÎÎñÏêÏ¸ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_MISLOG_CHANGE,		CMD_MC_ROLEBASE + 30,	ÈÎÎñ×´Ì¬±ä»¯)
	MESSAGE_ADD(	CMD_MC_MISLOG_CLEAR,		CMD_MC_ROLEBASE + 31,	Çå³ýÒ»¸öÈÎÎñ)
	MESSAGE_ADD(	CMD_MC_MISLOG_ADD,			CMD_MC_ROLEBASE + 32,	Ìí¼ÓÒ»¸öÈÎÎñ)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_FUSION,	CMD_MC_ROLEBASE + 33	,	¿ªÊ¼ÈÛºÏµÀ¾ß)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_UPGRADE,	CMD_MC_ROLEBASE + 34	,	¿ªÊ¼µÀ¾ßÉý¼¶)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_FORGE,	CMD_MC_ROLEBASE + 35	,	¿ªÊ¼µÀ¾ß¾«Á¶)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_UNITE,	CMD_MC_ROLEBASE + 36	,	¿ªÊ¼µÀ¾ßºÏ³É)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_MILLING,	CMD_MC_ROLEBASE + 37	,	¿ªÊ¼µÀ¾ß´òÄ¥)
	MESSAGE_ADD(	CMD_MC_CREATEBOAT,			CMD_MC_ROLEBASE + 38,	´´½¨´¬Ö»)
	MESSAGE_ADD(	CMD_MC_UPDATEBOAT,			CMD_MC_ROLEBASE + 39,	¸ÄÔì´¬Ö»)
	MESSAGE_ADD(	CMD_MC_UPDATEBOAT_PART,		CMD_MC_ROLEBASE + 40,	¸ÄÔì´¬Ö»µÄ²¿¼þ)
	MESSAGE_ADD(	CMD_MC_BERTH_LIST,			CMD_MC_ROLEBASE + 41,	´¬Ö»Í£²´ÁÐ±í)
	MESSAGE_ADD(	CMD_MC_BOAT_LIST,			CMD_MC_ROLEBASE + 42,	Í¬²½½ÇÉ«ÓµÓÐµÄËùÓÐ´¬Ö»ÊôÐÔ)
	MESSAGE_ADD(	CMD_MC_BOAT_ADD,			CMD_MC_ROLEBASE + 43,	Ìí¼ÓÒ»ËÒ´¬)
	MESSAGE_ADD(	CMD_MC_BOAT_CLEAR,			CMD_MC_ROLEBASE + 44,	Çå³ýÒ»ËÒ´¬)
	MESSAGE_ADD(	CMD_MC_BOATINFO,			CMD_MC_ROLEBASE + 45,	Í¬²½´¬Ö»ÊôÐÔÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_BOAT_BAGLIST,		CMD_MC_ROLEBASE + 46,	ÏÔÊ¾¿ÉÒÔ´ò°üµÄ´¬Ö»ÁÐ±í)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_EIDOLON_METEMPSYCHOSIS,	CMD_MC_ROLEBASE + 47,	¿ªÊ¼¾«Áé×ªÉú)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_EIDOLON_FUSION,	CMD_MC_ROLEBASE + 48,	¿ªÊ¼¾«ÁéÈÚºÏ)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_PURIFY,	CMD_MC_ROLEBASE + 49	,	¿ªÊ¼×°±¸Ìá´¿)
	MESSAGE_ADD(	CMD_MC_ENTITY_BEGINESEE,	CMD_MC_ROLEBASE + 50,	Í¬²½ÊÓÒ°ÊµÌåÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_ENTITY_ENDSEE,		CMD_MC_ROLEBASE + 51,	ÊµÌåÐÅÏ¢³¬³öÊÓÒ°)
	MESSAGE_ADD(	CMD_MC_ENTITY_CHGSTATE,		CMD_MC_ROLEBASE + 52,	ÊµÌå×´Ì¬¸Ä±ä)
	MESSAGE_ADD(	CMD_MC_STALL_ALLDATA,		CMD_MC_ROLEBASE + 54,	²é¿´°ÚÌ¯ËùÓÐÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_STALL_UPDATE,		CMD_MC_ROLEBASE + 55,	°ÚÌ¯»õÎï¸üÐÂÐÅÏ¢)
	MESSAGE_ADD(	CMD_MC_STALL_DELGOODS,		CMD_MC_ROLEBASE + 56,	É¾³ýÖ¸¶¨Î»ÖÃµÄ»õÎï)
	MESSAGE_ADD(	CMD_MC_STALL_CLOSE,			CMD_MC_ROLEBASE + 57,	Ì¯Ö÷ÊÕÌ¯)
	MESSAGE_ADD(	CMD_MC_STALL_START,			CMD_MC_ROLEBASE + 58,	³É¹¦°ÚÌ¯)
	MESSAGE_ADD(	CMD_MC_STALL_NAME,			CMD_MC_ROLEBASE + 59,	°ÚÌ¯Ãû)
	MESSAGE_ADD(	CMD_MC_BICKER_NOTICE,		CMD_MC_ROLEBASE + 60,	ÉÁ¶¯×ÖÌåÐÅÏ¢Í¨Öª)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_ENERGY,	CMD_MC_ROLEBASE + 71	,	¿ªÊ¼±´¿Ç³äµç)
	MESSAGE_ADD(	CMD_MC_BEGIN_GET_STONE,		CMD_MC_ROLEBASE + 72	,	¿ªÊ¼ÌáÈ¡±¦Ê¯)
	MESSAGE_ADD(	CMD_MC_BEGIN_TIGER,			CMD_MC_ROLEBASE + 73	,	¾«ÁéÀÏ»¢»ú½çÃæ)
	MESSAGE_ADD(	CMD_MC_TIGER_ITEM_ID,		CMD_MC_ROLEBASE + 74	,	ÀÏ»¢»úµÀ¾ßID)
	MESSAGE_ADD(	CMD_MC_TIGER_STOP,			CMD_MC_ROLEBASE + 75	,	ÀÏ»¢»ú½áÊø)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_FIX,		CMD_MC_ROLEBASE + 76	,	ÐÞÀíÆÆ¹ø)
	MESSAGE_ADD(	CMD_MC_BEGIN_GM_SEND,		CMD_MC_ROLEBASE + 77	,	·¢ËÍGMÓÊ¼þ)
	MESSAGE_ADD(	CMD_MC_BEGIN_GM_RECV,		CMD_MC_ROLEBASE + 78	,	½ÓÊÕGMÓÊ¼þ)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_LOTTERY,	CMD_MC_ROLEBASE + 79	,	¿ªÊ¼ÉèÖÃ²ÊÆ±)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_AMPHITHEATER,	CMD_MC_ROLEBASE + 80,	´ò¿ª¾º¼¼³¡ÅÅÐÐ°ñ)
	MESSAGE_ADD(	CMD_MC_LEAVE_TIME_ASR,		CMD_MC_ROLEBASE + 81,	½ÇÉ«ÀëÏßÊ±¼ä)
	MESSAGE_ADD(	CMD_MC_SELECT_TIME_ASR,		CMD_MC_ROLEBASE + 82,	»ñÈ¡ÀëÏß¾­ÑéÏûÏ¢»Ø¸´)
	MESSAGE_ADD(	CMD_MC_BEHINDGENIUS_ASR,	CMD_MC_ROLEBASE + 83	,	±³ºóÁéÖÆÔìÓ¦´ð)
	MESSAGE_ADD(	CMD_MC_BEGIN_ITEM_JEWELRYUP,CMD_MC_ROLEBASE + 84	,	µ¯¿ª±¦Ê¯Éý¼¶½çÃæ)
	MESSAGE_ADD(	CMD_MC_OPEN_CRYSTALTRADE_ASR,CMD_MC_ROLEBASE + 85	,	´ò¿ªË®¾§Æ½Ì¨Ó¦´ð)
	MESSAGE_ADD(	CMD_MC_REQUEST_BUY_ASR,		CMD_MC_ROLEBASE + 86 ,	ÂòÈë¹Òµ¥Ó¦´ð)
	MESSAGE_ADD(	CMD_MC_REQUEST_SALE_ASR,	CMD_MC_ROLEBASE + 87 ,	Âô³ö¹Òµ¥Ó¦´ð)
	MESSAGE_ADD(	CMD_MC_REQUEST_CANCEL_ASR,	CMD_MC_ROLEBASE + 88 ,	È¡Ïû¹Òµ¥Ó¦´ð)
	MESSAGE_ADD(	CMD_MC_REQUEST_GETFLATMONEY,CMD_MC_ROLEBASE + 89 ,	È¡³öÆ½Ì¨ÉÏµÄÇ®Ó¦´ð)
	MESSAGE_ADD(	CMD_MC_REQUEST_REFRESH_ASR,	CMD_MC_ROLEBASE + 90 ,	ÇëÇóË¢ÐÂÓ¦´ð)
	MESSAGE_ADD(	CMD_MC_GET_CRYSTALSTATE,	CMD_MC_ROLEBASE + 91 ,	»ñÈ¡½»Ò××´Ì¬)
	MESSAGE_ADD(	CMD_MC_LIFESKILL_BGING,			CMD_MC_BASE + 100	,	Éú»î¼¼ÄÜ¿ªÊ¼)
	MESSAGE_ADD(	CMD_MC_LIFESKILL_ASR,			CMD_MC_BASE + 101	,	Éú»î¼¼ÄÜÓÃÔË£¬ºÏ³É£¬·Ö½âµÈµÄÇëÇó)
	MESSAGE_ADD(	CMD_MC_LIFESKILL_ASK,			CMD_MC_BASE + 102	,	Éú»î¼¼ÄÜÓÃÔË£¬ºÏ³É£¬·Ö½âµÈµÄÈ·ÈÏ)
	MESSAGE_ADD(	CMD_MC_ITEM_LOTTERY_ASK,		CMD_MC_BASE + 103,	²ÊÆ±ÇëÇó)
	MESSAGE_ADD(	CMD_MC_ITEM_LOTTERY_ASR,		CMD_MC_BASE + 104,	²ÊÆ±Ó¦´ð)
	MESSAGE_ADD(	CMD_MC_CAPTAIN_ASK,				CMD_MC_BASE + 105,	¾º¼¼³¡¶Ó³¤È·ÈÏÇëÇó)
	MESSAGE_ADD(	CMD_MC_ITEM_AMPHITHEATER_ASR,	CMD_MC_BASE + 106, ÎÞ)
	MESSAGE_ADD(	CMD_MC_REQUEST_JEWELRYUP_ASR,	CMD_MC_BASE + 107, ÎÞ)
	MESSAGE_ADD(	XTRAP_CMD_STEP_ONE,				CMD_MC_BASE + 108, ÎÞ)
	MESSAGE_ADD(	CMD_TM_ENTERMAP,				CMD_TM_BASE	+ 3	,	½øµØÍ¼£¬¸½´ø½ÇÉ«ID¡¢½øÈëµØÍ¼µÄÃû×Ö¡¢Î»ÖÃ(x,y))
	MESSAGE_ADD(	CMD_TM_GOOUTMAP,				CMD_TM_BASE + 4		,	³öµØÍ¼)
	MESSAGE_ADD(	CMD_TM_LOGIN_ACK,				CMD_TM_BASE + 5, ÎÞ)
	MESSAGE_ADD(	CMD_TM_CHANGE_PERSONINFO,		CMD_TM_BASE	+6	,	ÐÞ¸Ä¸öÈËÐÅÏ¢)
	MESSAGE_ADD(	CMD_TM_MAPENTRY,				CMD_TM_BASE + 7, ÎÞ)
	MESSAGE_ADD(	CMD_TM_MAPENTRY_NOMAP,			CMD_TM_BASE + 8, ÎÞ)
	MESSAGE_ADD(	CMD_TM_MAP_ADMIN,				CMD_TM_BASE + 9	,	¹ÜÀíÔ±ÓÃµØÍ¼¿ØÖÆÃüÁî)
	MESSAGE_ADD(	CMD_TM_STATE,					CMD_TM_BASE + 10,	·þÎñÆ÷µÄ×´Ì¬£¨ÔËÐÐÄÚ´æÇé¿ö£¬°æ±¾£©)
	MESSAGE_ADD(	CMD_TM_PORTALTIMES, CMD_TM_BASE + 11, Portal time opening closing)
	MESSAGE_ADD(	CMD_MT_LOGIN,					CMD_MT_BASE + 1		,	GameServerµÇÂ½GateServer)
	MESSAGE_ADD(	CMD_MT_SWITCHMAP,				CMD_MT_BASE + 4		,	¿ªÊ¼ÇÐ»»µØÍ¼ÃüÁî)
	MESSAGE_ADD(	CMD_MT_KICKUSER,				CMD_MT_BASE + 5		,	GameServerÌßµôÄ³¸öÓÃ»§)
	MESSAGE_ADD(	CMD_MT_MAPENTRY,				CMD_MT_BASE + 6		,	µØÍ¼Èë¿Ú)
	MESSAGE_ADD(	CMD_MT_PALYEREXIT,				CMD_MT_BASE + 8		,	Í¨ÖªGateÊÍ·Å¸ÃÓÃ»§×ÊÔ´)
	MESSAGE_ADD(	CMD_MT_MAP_ADMIN,				CMD_MT_BASE + 9	,	¹ÜÀíÔ±ÓÃµØÍ¼¿ØÖÆÃüÁî)
	MESSAGE_ADD(	CMD_MT_STATE,					CMD_MT_BASE + 10,	·þÎñÆ÷µÄ×´Ì¬£¨ÔËÐÐÄÚ´æÇé¿ö£¬°æ±¾£©)
	MESSAGE_ADD(	CMD_PM_TEAM,					CMD_PM_BASE + 1    ,	GroupServer·¢À´µÄ×é¶ÓÐÅÏ¢¸üÐÂ)
	MESSAGE_ADD(	CMD_PM_GUILD_DISBAND,			CMD_PM_BASE	+ 2		,	½âÉ¢¹«»á)
	MESSAGE_ADD(	CMD_PM_GUILDINFO,				CMD_PM_BASE + 4	,	GroupServer·¢À´µÄ¹«»áÍ¬²½ÐÅÏ¢)
	MESSAGE_ADD(	CMD_PM_GUILD_CHALLMONEY,		CMD_PM_BASE + 5    ,	½«¹«»áÕ½Í¶±êµÄÇ®ÍË¸ø»á³¤)
	MESSAGE_ADD(	CMD_PM_GUILD_CHALL_PRIZEMONEY,	CMD_PM_BASE + 6    ,	½«¹«»áÕ½Õ½½±ÀøµÄÇ®¸ø»á³¤)
	MESSAGE_ADD(	CMD_PM_GARNER2_UPDATE,			CMD_PM_BASE +7, ÎÞ)
	MESSAGE_ADD(	CMD_PM_TEAM_CREATE,				CMD_PM_BASE + 8, ÎÞ)
	MESSAGE_ADD(	CMD_PM_SAY2ALL,					CMD_PM_BASE + 9, ÎÞ)
	MESSAGE_ADD(	CMD_PM_SAY2TRADE,				CMD_PM_BASE + 10, ÎÞ)
	MESSAGE_ADD(	CMD_PM_EXPSCALE,				CMD_PM_BASE + 11       ,	 ·À³ÁÃÔÏµÍ³  Ó°ÏìÍæ¼Ò¾­Ñé¼°µô±¦ÂÊ)
	MESSAGE_ADD(	CMD_PM_GUILD_INVITE,			CMD_PM_BASE + 12		,	¹«»áÑûÇë)

	// FEATURE: GUILD_BANK
	MESSAGE_ADD(CMD_PM_GUILDBANK, CMD_PM_BASE + 13, Guild Bank)


	MESSAGE_ADD(	CMD_MP_ENTERMAP,				CMD_MP_BASE	+ 1, ÎÞ)
	MESSAGE_ADD(	CMD_MP_GUILD_APPROVE,			CMD_MP_BASE	+ 2, ÎÞ)
	MESSAGE_ADD(	CMD_MP_GUILD_CREATE,			CMD_MP_BASE	+ 3, ÎÞ)
	MESSAGE_ADD(	CMD_MP_GUILD_KICK,				CMD_MP_BASE	+ 4, ÎÞ)
	MESSAGE_ADD(	CMD_MP_GUILD_LEAVE,				CMD_MP_BASE	+ 5, ÎÞ)
	MESSAGE_ADD(	CMD_MP_GUILD_DISBAND,			CMD_MP_BASE	+ 6, ÎÞ)
	MESSAGE_ADD(	CMD_MP_GUILD_MOTTO,				CMD_MP_BASE	+ 10, ÎÞ)
	MESSAGE_ADD(	CMD_MP_GUILD_CHALLMONEY,		CMD_MP_BASE + 13			,	½«Í¶±ê¹«»áµÄÇ®ÍË»¹»á³¤½ÇÉ«)
	MESSAGE_ADD(	CMD_MP_GUILD_CHALL_PRIZEMONEY,	CMD_MP_BASE + 14    ,	½«¹«»áÕ½½±ÀøµÄµÄÇ®¸ø»á³¤½ÇÉ«)
	MESSAGE_ADD(	CMD_MP_GARNER2_UPDATE,			CMD_MP_BASE + 7, ÎÞ)
	MESSAGE_ADD(	CMD_MP_GARNER2_CGETORDER,		CMD_MP_BASE + 15, ÎÞ)
	MESSAGE_ADD(	CMD_MP_TEAM_CREATE,				CMD_MP_BASE + 16, ÎÞ)
	MESSAGE_ADD(	CMD_MP_MASTER_CREATE,			CMD_MP_BASE + 17, ÎÞ)
	MESSAGE_ADD(	CMD_MP_MASTER_DEL,				CMD_MP_BASE + 18, ÎÞ)
	MESSAGE_ADD(	CMD_MP_MASTER_FINISH,			CMD_MP_BASE + 19, ÎÞ)
	MESSAGE_ADD(	CMD_MP_SAY2ALL,					CMD_MP_BASE + 20, ÎÞ)
	MESSAGE_ADD(	CMD_MP_SAY2TRADE,				CMD_MP_BASE + 21, ÎÞ)
	MESSAGE_ADD(	CMD_MP_GM1SAY1,					CMD_MP_BASE + 22, ÎÞ)
	MESSAGE_ADD(	CMD_MP_GM1SAY,					CMD_MP_BASE + 23, ÎÞ)
	MESSAGE_ADD(	CMD_MP_GMBANACCOUNT,			CMD_MP_BASE + 24, GM·âÍ£Ë¢ÆÁÕËºÅ)//Add by sunny.sun 20090828
	
	// FEATURE: GUILD_BANK
	MESSAGE_ADD(CMD_MP_GUILDBANK, CMD_MP_BASE + 25, Guild Bank)
	MESSAGE_ADD(CMD_MP_GUILDADDEXP, CMD_MP_BASE + 26, Guild Attr)
	MESSAGE_ADD(CMD_MP_GUILD_PERM, CMD_MP_BASE + 27, Guild Perm)
	MESSAGE_ADD(CMD_MP_GUILDNAMECOLOR, CMD_MP_BASE + 28, Guild Color)
	MESSAGE_ADD(CMD_MP_GUILDNOTICE, CMD_MP_BASE + 29, Guild Notice)

	MESSAGE_ADD(	CMD_TP_LOGIN,					CMD_TP_BASE + 1		,	GateServerµ½GroupServer½ø³Ì¼äµÇÂ½)
	MESSAGE_ADD(	CMD_TP_LOGOUT,					CMD_TP_BASE + 2		,	GateServerµ½GroupServer½ø³Ì¼äµÇ³ö)
	MESSAGE_ADD(	CMD_TP_USER_LOGIN,				CMD_TP_BASE	+ 3		,	ÓÃ»§µÇÈë)
	MESSAGE_ADD(	CMD_TP_USER_LOGOUT,				CMD_TP_BASE + 4		,	ÓÃ»§µÇ³ö)
	MESSAGE_ADD(	CMD_TP_BGNPLAY,					CMD_TP_BASE	+ 5		,	ÓÃ»§Ñ¡Ôñ½ÇÉ«¿ªÊ¼ÍæÓÎÏ·)
	MESSAGE_ADD(	CMD_TP_ENDPLAY,					CMD_TP_BASE	+ 6		,	ÓÃ»§½áÊøÍæÓÎÏ·½øÈëÑ¡/½¨/É¾½ÇÉ«»­Ãæ)
	MESSAGE_ADD(	CMD_TP_NEWCHA,					CMD_TP_BASE	+ 7		,	ÐÂ½¨½ÇÉ«)
	MESSAGE_ADD(	CMD_TP_DELCHA,					CMD_TP_BASE	+ 8		,	É¾³ý½ÇÉ«)
	MESSAGE_ADD(	CMD_TP_PLAYEREXIT,				CMD_TP_BASE + 9		,	ÍË³ö½ÇÉ«ÊÍ·Å×ÊÔ´)
	MESSAGE_ADD(	CMD_TP_REQPLYLST,				CMD_TP_BASE	+ 10	,	ÇëÇó±¾GateServerÉÏµÄÍæ¼ÒµØÖ·ÁÐ±í)
	MESSAGE_ADD(	CMD_TP_DISC,					CMD_TP_BASE + 11, ÎÞ)
	MESSAGE_ADD(	CMD_TP_ESTOPUSER_CHECK,			CMD_TP_BASE + 12, ÎÞ)
	MESSAGE_ADD(	CMD_TP_CREATE_PASSWORD2,		CMD_TP_BASE + 13,	´´½¨¶þ´ÎÃÜÂë)
	MESSAGE_ADD(	CMD_TP_UPDATE_PASSWORD2,		CMD_TP_BASE + 14,	¸üÐÂ¶þ´ÎÃÜÂë)
	MESSAGE_ADD(	CMD_TP_SYNC_PLYLST,				CMD_TP_BASE	+ 15	,	·¢ËÍ±¾GateServerÉÏµÄÍæ¼ÒµØÖ·ÁÐ±í)
	MESSAGE_ADD(	CMD_PT_KICKUSER,				CMD_PT_BASE	+ 11, ÎÞ)
	MESSAGE_ADD(	CMD_PT_ESTOPUSER,				CMD_PT_BASE + 12,	½ûÑÔ½ÇÉ«)
	MESSAGE_ADD(	CMD_PT_DEL_ESTOPUSER,			CMD_PT_BASE + 13,	½â³ý½ûÑÔ)

	MESSAGE_ADD(	CMD_PA_LOGIN,					CMD_PA_BASE + 2, ÎÞ)
	MESSAGE_ADD(	CMD_PA_USER_LOGIN,				CMD_PA_BASE + 4, ÎÞ)
	MESSAGE_ADD(	CMD_PA_USER_LOGOUT,				CMD_PA_BASE + 5, ÎÞ)
	MESSAGE_ADD(	CMD_PA_USER_DISABLE,			CMD_PA_BASE + 6, ÎÞ)
	MESSAGE_ADD(	CMD_PA_USER_LOGIN1,				CMD_PA_BASE + 13, ÎÞ)
	MESSAGE_ADD(	CMD_PA_USER_LOGIN2,				CMD_PA_BASE + 14, ÎÞ)
	MESSAGE_ADD(	CMD_PA_USER_BILLBGN,			CMD_PA_BASE + 20, ÎÞ)
	MESSAGE_ADD(	CMD_PA_USER_BILLEND,			CMD_PA_BASE + 21, ÎÞ)
	MESSAGE_ADD(	CMD_PA_GROUP_BILLEND_AND_LOGOUT,CMD_PA_BASE + 22, ÎÞ)
	MESSAGE_ADD(	CMD_PA_LOGOUT,					CMD_PA_BASE + 23, ÎÞ)
	MESSAGE_ADD(	CMD_PA_GMBANACCOUNT,			CMD_PA_BASE + 24, GM·âÍ£ÕËºÅ)//Add by sunny.sun 20090828

	MESSAGE_ADD(	CMD_AP_LOGIN,					CMD_AP_BASE + 2, ÎÞ)
	MESSAGE_ADD(	CMD_AP_USER_LOGIN,				CMD_AP_BASE + 3, ÎÞ)
	MESSAGE_ADD(	CMD_AP_RELOGIN,					CMD_AP_BASE + 4, ÎÞ)
	MESSAGE_ADD(	CMD_AP_KICKUSER,				CMD_AP_BASE	+ 11, ÎÞ)
	MESSAGE_ADD(	CMD_AP_USER_LOGIN1,				CMD_AP_BASE + 12, ÎÞ)
	MESSAGE_ADD(	CMD_AP_USER_LOGIN2,				CMD_AP_BASE + 13, ÎÞ)
	MESSAGE_ADD(	CMD_AP_EXPSCALE,				CMD_AP_BASE + 14, ·À³ÁÃÔ)
	MESSAGE_ADD(	CMD_MM_GATE_RELEASE,			CMD_MM_BASE + 1, ÎÞ)
	MESSAGE_ADD(	CMD_MM_GATE_CONNECT,			CMD_MM_BASE + 2, ÎÞ)
	MESSAGE_ADD(	CMD_MM_QUERY_CHA,				CMD_MM_BASE + 3	,	²éÑ¯½ÇÉ«»ù±¾ÐÅÏ¢)
	MESSAGE_ADD(	CMD_MM_QUERY_CHAITEM,			CMD_MM_BASE + 4	,	²éÑ¯½ÇÉ«µÀ¾ßÐÅÏ¢)
	MESSAGE_ADD(	CMD_MM_CALL_CHA,				CMD_MM_BASE + 5	,	½«½ÇÉ«ÕÙ»½µ½×Ô¼ºÉí±ß)
	MESSAGE_ADD(	CMD_MM_GOTO_CHA,				CMD_MM_BASE + 6	,	½«×Ô¼º´«ËÍµ½½ÇÉ«Éí±ß)
	MESSAGE_ADD(	CMD_MM_KICK_CHA,				CMD_MM_BASE + 7	,	½«½ÇÉ«ÌßÏÂÏß)
	MESSAGE_ADD(	CMD_MM_GUILD_REJECT,			CMD_MM_BASE + 8	,	¾Ü¾ø¼ÓÈë¹«»áÉêÇëÍ¨ÖªÃüÁî)
	MESSAGE_ADD(	CMD_MM_GUILD_APPROVE,			CMD_MM_BASE + 9	,	¾Ü¾ø¼ÓÈë¹«»áÉêÇëÍ¨ÖªÃüÁî)
	MESSAGE_ADD(	CMD_MM_GUILD_KICK,				CMD_MM_BASE	+ 10,	´Ó¹«»áÀïÃæÌÞ³ýÄ³ÈË)
	MESSAGE_ADD(	CMD_MM_GUILD_DISBAND,			CMD_MM_BASE	+ 11,	½âÉ¢¹«»á)
	MESSAGE_ADD(	CMD_MM_QUERY_CHAPING,			CMD_MM_BASE + 12,	²éÑ¯½ÇÉ«µ½GameServerÂß¼­²ãµÄpingÖµ)
	MESSAGE_ADD(	CMD_MM_NOTICE,					CMD_MM_BASE + 13,	ÏµÍ³Í¨¸æ)
	MESSAGE_ADD(	CMD_MM_GUILD_MOTTO,				CMD_MM_BASE + 14,	¸üÐÂ¹«»á×ùÓÓÃû)
	MESSAGE_ADD(	CMD_MM_DO_STRING,				CMD_MM_BASE + 15,	Ö´ÐÐ½Å±¾)
	MESSAGE_ADD(	CMD_MM_CHA_NOTICE,				CMD_MM_BASE + 16,	¶ÔÖ¸¶¨Íæ¼ÒµÄÏµÍ³Í¨¸æ)
	MESSAGE_ADD(	CMD_MM_LOGIN,					CMD_MM_BASE + 17,	Íæ¼ÒµÇÂ½ÓÎÏ·)
	MESSAGE_ADD(	CMD_MM_GUILD_CHALL_PRIZEMONEY,	CMD_MM_BASE + 18,	ÍË»¹¹¤»áÕ½µÄ·ÑÓÃ)
	MESSAGE_ADD(	CMD_MM_ADDCREDIT,				CMD_MM_BASE + 19,	Ôö¼ÓÉùÍû)
	MESSAGE_ADD(	CMD_MM_STORE_BUY,				CMD_MM_BASE + 20,	¹ºÂòÉÌ³ÇÉÌÆ·)
	MESSAGE_ADD(	CMD_MM_ADDMONEY,				CMD_MM_BASE + 21, ÎÞ)
	MESSAGE_ADD(	CMD_MM_AUCTION,					CMD_MM_BASE + 22, ÎÞ)
	MESSAGE_ADD(	CMD_MM_NOTICETOCHA,				CMD_MM_BASE + 23,	Ë®¾§½»Ò×Í¨Öª²»ÔÚÍ¬Ò»·þÎñÆ÷µÄ½»Ò×Õß)
	MESSAGE_ADD(	CMD_MM_EXCHEXTERNVALUE,			CMD_MM_BASE + 24,	½Å±¾¸Ä±äÈ«¾Ö±äÁ¿)
	MESSAGE_ADD(	CMD_MM_NOTICETRADER,			CMD_MM_BASE + 25,	Í¨Öª½»Ò×Õß)

	// FEATURE: GUILD_BANK
	MESSAGE_ADD(CMD_MM_UPDATEGUILDBANK, CMD_MM_BASE + 26, Update Guild Bank)
	MESSAGE_ADD(CMD_MM_UPDATEGUILDBANKGOLD, CMD_MM_BASE + 27, Update Guild Bank Gold)
	MESSAGE_ADD(CMD_MM_UPDATEGUILDPERMISSONS, CMD_MM_BASE + 28, Update Guild Bank Gold)

	MESSAGE_ADD(	CMD_CP_TEAM_INVITE,			CMD_CP_BASE	+ 1		,	ÑûÇë×é¶Ó)
	MESSAGE_ADD(	CMD_CP_TEAM_ACCEPT,			CMD_CP_BASE	+ 2		,	È·ÈÏ×é¶Ó)
	MESSAGE_ADD(	CMD_CP_TEAM_REFUSE,			CMD_CP_BASE	+ 3		,	¾Ü¾ø×é¶Ó)
	MESSAGE_ADD(	CMD_CP_TEAM_LEAVE,			CMD_CP_BASE	+ 4		,	Àë¿ª×é¶Ó)
	MESSAGE_ADD(	CMD_CP_TEAM_KICK,			CMD_CP_BASE + 5		,	Ìß³ö×é¶Ó)
	MESSAGE_ADD(	CMD_CP_FRND_INVITE,			CMD_CP_BASE	+ 11	,	ºÃÓÑÑûÇë)
	MESSAGE_ADD(	CMD_CP_FRND_ACCEPT,			CMD_CP_BASE	+ 12	,	ºÃÓÑ½ÓÊÕ)
	MESSAGE_ADD(	CMD_CP_FRND_REFUSE,			CMD_CP_BASE	+ 13	,	ºÃÓÑ¾Ü¾ø)
	MESSAGE_ADD(	CMD_CP_FRND_DELETE,			CMD_CP_BASE	+ 14	,	ºÃÓÑÉ¾³ý)
	MESSAGE_ADD(	CMD_CP_FRND_MOVE_GROUP,		CMD_CP_BASE	+ 15	,	¸Ä±äºÃÓÑËùÊô·Ö×é)
	MESSAGE_ADD(	CMD_CP_FRND_REFRESH_INFO,	CMD_CP_BASE	+ 16	,	ÔÚ×Ô¼ºµÄ¿Í»§¶ËË¢ÐÂ±ðÈË£¨ºÃÓÑ¡¢¶ÓÓÑ¡¢Í¬ÃÅ¡¢Â·ÈË¡¢Ä°ÉúÈË£©µÄ¸öÈËÐÅÏ¢)
	MESSAGE_ADD(	CMD_CP_CHANGE_PERSONINFO,	CMD_CP_BASE	+ 17	,	¸Ä±äÍæ¼Ò×Ô¼ºµÄÍ·ÏñµÈ¸öÈËÐÅÏ¢)
	MESSAGE_ADD(	CMD_CP_FRND_DEL_GROUP,		CMD_CP_BASE	+ 18, ÎÞ)
	MESSAGE_ADD(	CMD_CP_FRND_ADD_GROUP,		CMD_CP_BASE	+ 19, ÎÞ)
	MESSAGE_ADD(	CMD_CP_FRND_CHANGE_GROUP,	CMD_CP_BASE	+ 20, ÎÞ)
	MESSAGE_ADD(	CMD_CP_QUERY_PERSONINFO,	CMD_CP_BASE	+ 21, ÎÞ)
	MESSAGE_ADD(	CMD_CP_PING,					CMD_CP_BASE	+ 25, ÎÞ)
	MESSAGE_ADD(	CMD_CP_REPORT_WG,				CMD_CP_BASE + 26,	±¨¸æ·¢ÏÖÊ¹ÓÃÍâ¹Ò)
	MESSAGE_ADD(	CMD_CP_MASTER_REFRESH_INFO,		CMD_CP_BASE	+ 31	,	ÔÚ×Ô¼ºµÄ¿Í»§¶ËË¢ÐÂÊ¦¸¸µÄ¸öÈËÐÅÏ¢)
	MESSAGE_ADD(	CMD_CP_PRENTICE_REFRESH_INFO,	CMD_CP_BASE	+ 32	,	ÔÚ×Ô¼ºµÄ¿Í»§¶ËË¢ÐÂÍ½µÜµÄ¸öÈËÐÅÏ¢)
	MESSAGE_ADD(	CMD_CP_GUILD_INVITE,			CMD_CP_BASE + 33, ÎÞ)
	MESSAGE_ADD(	CMD_CP_GUILD_ACCEPT,			CMD_CP_BASE + 34, ÎÞ)
	MESSAGE_ADD(	CMD_CP_GUILD_REFUSE,			CMD_CP_BASE + 35, ÎÞ)
	MESSAGE_ADD(	CMD_CP_GM1SAY,				CMD_CP_BASE	+ 400		,	GMµÄÈ«·þ¹ã²¥)
	MESSAGE_ADD(	CMD_CP_SAY2TRADE,			CMD_CP_BASE	+ 401		,	½»Ò×ÆµµÀ)
	MESSAGE_ADD(	CMD_CP_SAY2ALL,				CMD_CP_BASE + 402		,	Õû¸öÓÎÏ·ÊÀ½ç¹ã²¥)
	MESSAGE_ADD(	CMD_CP_SAY2YOU,				CMD_CP_BASE	+ 403		,	P2PÁÄÌì(Íæ¼Ò¶ÔÍæ¼Ò))
	MESSAGE_ADD(	CMD_CP_SAY2TEM,				CMD_CP_BASE	+ 404		,	×é¶ÓÁÄÌì)
	MESSAGE_ADD(	CMD_CP_SAY2GUD,				CMD_CP_BASE	+ 405		,	ÐÐ»á(Guild)ÁÄÌì)
	MESSAGE_ADD(	CMD_CP_SESS_CREATE,			CMD_CP_BASE	+ 406		,	»á»°½¨Á¢)
	MESSAGE_ADD(	CMD_CP_SESS_SAY,			CMD_CP_BASE	+ 407		,	»á»°Ëµ»°)
	MESSAGE_ADD(	CMD_CP_SESS_ADD,			CMD_CP_BASE	+ 408		,	»á»°Ìí¼ÓÈËÔ±)
	MESSAGE_ADD(	CMD_CP_SESS_LEAVE,			CMD_CP_BASE	+ 409		,	Àë¿ª»á»°)
	MESSAGE_ADD(	CMD_CP_REFUSETOME,			CMD_CP_BASE	+ 410		,	´ò¿ª½ÓÊÕË½ÁÄ¿ª¹Ø)
	MESSAGE_ADD(	CMD_CP_GM1SAY1,				CMD_CP_BASE	+ 411		,	¹ö¶¯¹ã²¥)
	MESSAGE_ADD(	CMD_CP_GMSAY,				CMD_CP_BASE	+ 412		,	ÌØÊâÃüÁîÖ´ÐÐ)
	MESSAGE_ADD(CMD_CP_SAY2GUILD, CMD_CP_BASE + 413, ÐÐ»á(Guild)ÁÄÌì)


	// FEATURE: GUILD_BANK
	MESSAGE_ADD(CMD_CP_GUILDBANK, CMD_CP_BASE + 414, Guild Bank)
	MESSAGE_ADD(CMD_CP_GUILDATTR, CMD_CP_BASE + 416, Guild Attr)
	MESSAGE_ADD(CMD_CP_GUILDNAMECOLOR, CMD_CP_BASE + 417, Guild Color)


	MESSAGE_ADD(	CMD_PC_TEAM_INVITE,			CMD_PC_BASE	+ 1			,	Í¨Öª¿Í»§¶ËÓÐÈËÑûÇë×é¶Ó)
	MESSAGE_ADD(	CMD_PC_TEAM_REFRESH,		CMD_PC_BASE	+ 2			,	Í¨Öª¿Í»§¶Ë×é¶Ó±ä»¯µÄÐÅÏ¢£¬Ê¹ÓÃCMD_PM_TEAM¶¨ÒåµÄ×ÓÀàÐÍ)
	MESSAGE_ADD(	CMD_PC_TEAM_CANCEL,			CMD_PC_BASE	+ 3			,	·þÎñÆ÷ÒòÎª³¬Ê±»ò³ÉÔ±ÊýÒÑÂúµÈÔ­ÒòÍ¨Öª±»ÑûÇë·½È¡Ïû±¾´ÎÑûÇë)
	MESSAGE_ADD(	CMD_PC_FRND_INVITE,			CMD_PC_BASE	+ 11, ÎÞ)
	MESSAGE_ADD(	CMD_PC_FRND_REFRESH,		CMD_PC_BASE	+ 12, ÎÞ)
	MESSAGE_ADD(	CMD_PC_FRND_CANCEL,			CMD_PC_BASE	+ 13, ÎÞ)
	MESSAGE_ADD(	CMD_PC_FRND_MOVE_GROUP,		CMD_PC_BASE + 15	,	¸Ä±äºÃÓÑ·Ö×é³É¹¦)
	MESSAGE_ADD(	CMD_PC_FRND_REFRESH_INFO,	CMD_PC_BASE	+ 16	,	Ë¢ÐÂºÃÓÑ¸öÈËÐÅÏ¢)
	MESSAGE_ADD(	CMD_PC_CHANGE_PERSONINFO,	CMD_PC_BASE	+ 17	,	¸Ä±äÍæ¼Ò×Ô¼ºµÄÍ·ÏñµÈ¸öÈËÐÅÏ¢)
	MESSAGE_ADD(	CMD_PC_FRND_DEL_GROUP,		CMD_PC_BASE + 18	,	É¾³ýºÃÓÑ·Ö×é³É¹¦)
	MESSAGE_ADD(	CMD_PC_FRND_ADD_GROUP,		CMD_PC_BASE + 19	,	ÐÂÔöºÃÓÑ·Ö×é³É¹¦)
	MESSAGE_ADD(	CMD_PC_FRND_CHANGE_GROUP,	CMD_PC_BASE + 20	,	¸Ä±äºÃÓÑ·Ö×é³É¹¦)
	MESSAGE_ADD(	CMD_PC_QUERY_PERSONINFO,	CMD_PC_BASE	+ 21	,	¸Ä±äÍæ¼Ò×Ô¼ºµÄÍ·ÏñµÈ¸öÈËÐÅÏ¢)
	MESSAGE_ADD(	CMD_PC_PING,				CMD_PC_BASE	+ 25, ÎÞ)
	MESSAGE_ADD(	CMD_PC_GUILD,				CMD_PC_BASE	+ 30, ÎÞ)
	MESSAGE_ADD(	CMD_PC_GUILD_INVITE,		CMD_PC_BASE	+ 31, ÎÞ)
	MESSAGE_ADD(	CMD_PC_GUILD_CANCEL,		CMD_PC_BASE	+ 33, ÎÞ)
		MESSAGE_DEF(CMD_PC_GUILD_PERM, CMD_PC_BASE + 34, GuildPerm)

	MESSAGE_ADD(	CMD_PC_MASTER_REFRESH,	CMD_PC_BASE	+ 41, ÎÞ)
	MESSAGE_ADD(	CMD_PC_MASTER_CANCEL,	CMD_PC_BASE	+ 42, ÎÞ)
	MESSAGE_ADD(	CMD_PC_MASTER_REFRESH_INFO,	CMD_PC_BASE	+ 43	,	Ë¢ÐÂÊ¦¸¸¸öÈËÐÅÏ¢)
	MESSAGE_ADD(	CMD_PC_PRENTICE_REFRESH_INFO,CMD_PC_BASE	+ 44	,	Ë¢ÐÂÍ½µÜ¸öÈËÐÅÏ¢)
	MESSAGE_ADD(	CMD_PC_GM1SAY,				CMD_PC_BASE	+ 400		,	GMµÄÈ«·þ¹ã²¥)
	MESSAGE_ADD(	CMD_PC_SAY2TRADE,			CMD_PC_BASE	+ 401		,	½»Ò×ÆµµÀ)
	MESSAGE_ADD(	CMD_PC_SAY2ALL,				CMD_PC_BASE + 402		,	Õû¸öÓÎÏ·ÊÀ½ç¹ã²¥)
	MESSAGE_ADD(	CMD_PC_SAY2YOU,				CMD_PC_BASE	+ 403		,	P2PÁÄÌì(Íæ¼Ò¶ÔÍæ¼Ò))
	MESSAGE_ADD(	CMD_PC_SAY2TEM,				CMD_PC_BASE	+ 404		,	×é¶ÓÁÄÌì)
	MESSAGE_ADD(	CMD_PC_SAY2GUD,				CMD_PC_BASE	+ 405		,	ÐÐ»á(Guild)ÁÄÌì)
	MESSAGE_ADD(	CMD_PC_SESS_CREATE,			CMD_PC_BASE	+ 406		,	»á»°½¨Á¢)
	MESSAGE_ADD(	CMD_PC_SESS_SAY,			CMD_PC_BASE	+ 407		,	»á»°Ëµ»°)
	MESSAGE_ADD(	CMD_PC_SESS_ADD,			CMD_PC_BASE	+ 408		,	»á»°Ìí¼ÓÈËÔ±)
	MESSAGE_ADD(	CMD_PC_SESS_LEAVE,			CMD_PC_BASE	+ 409		,	Àë¿ª»á»°)
	MESSAGE_ADD(	CMD_PC_GM1SAY1,				CMD_PC_BASE	+ 411		,	¹ö¶¯¹ã²¥)

	// FEATURE: GUILD_BANK
	MESSAGE_ADD(CMD_PC_ERRMSG, CMD_PC_BASE + 414, Error Message)
	MESSAGE_ADD(CMD_PC_UPDATEGUILDATTR, CMD_PC_BASE + 416, Guild Perm)
	MESSAGE_ADD(CMD_PC_GUILDNAMECOLOR, CMD_PC_BASE + 417, Guild Color)
	MESSAGE_ADD(CMD_PC_GUILDNOTICE, CMD_PC_BASE + 418, Guild Notice)


	MESSAGE_ADD(	CMD_PC_GARNER2_ORDER,		CMD_PC_BASE +101, ÎÞ)
	MESSAGE_ADD(	CMD_OS_LOGIN,				CMD_OS_BASE + 1			,	µÇÂ¼)
	MESSAGE_ADD(	CMD_OS_PING,				CMD_OS_BASE + 2			,	¼ì²âÃüÁî)
	MESSAGE_ADD(	CMD_SO_LOGIN,				CMD_SO_BASE + 1			,	µÇÂ¼ÃüÁî)
	MESSAGE_ADD(	CMD_SO_PING,				CMD_SO_BASE + 2			,	¼ì²âÃüÁî)
	MESSAGE_ADD(	CMD_SO_WARING,				CMD_SO_BASE + 3			,	¾¯¸æÃüÁî)
	MESSAGE_ADD(	CMD_SO_EXCEPTION,			CMD_SO_BASE + 4		,	Òì³£ÃüÁî)
	MESSAGE_ADD(	CMD_SO_ON_LINE,				CMD_SO_BASE + 5			,	ÉÏÏßÃüÁî)
	MESSAGE_ADD(	CMD_SO_OFF_LINE,			CMD_SO_BASE + 6			,	ÏÂÏßÃüÁî)
	MESSAGE_ADD(	CMD_SO_ENTER_MAP,			CMD_SO_BASE + 7			,	½øÈëµØÍ¼ÃüÁî)
	MESSAGE_ADD(	CMD_SO_LEAVE_MAP,			CMD_SO_BASE + 8			,	Àë¿ªµØÍ¼ÃüÁî)
	MESSAGE_ADD(	CMD_AB_LOGIN,				CMD_AB_BASE + 1			,	µÇÂ¼)
	MESSAGE_ADD(	CMD_AB_LOGOUT,				CMD_AB_BASE + 2			,	µÇ³ö)
	MESSAGE_ADD(	CMD_AB_USER_LOGINT,			CMD_AB_BASE + 3			,	Íæ¼ÒµÇÂ¼)
	MESSAGE_ADD(	CMD_AB_USER_LOGOUT,			CMD_AB_BASE + 4			,	Íæ¼ÒµÇ³ö)
	MESSAGE_ADD(	CMD_BA_USER_TIME,			CMD_BA_BASE + 1			,	Íæ¼ÒÊ±¼äÍ¨Öª)
	MESSAGE_ADD(	CMD_BA_KILL_USER,			CMD_BA_BASE + 2			,	ÌßµôÍæ¼Ò£¨Ä¿Ç°Á½ÖÖÇé¿ö1.ÉÏÏßÊ±Íæ¼ÒÒÑ¾­¹ýÁË5Ð¡Ê±½¡¿µÊ±¼ä2.Íæ¼ÒÈÏÖ¤Ê§°Ü£©)
}

inline static void ClearMessage()
{
	MsgMap.clear();
}

#endif
