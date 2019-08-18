// Script.h Created by knight-gongjian 2004.12.1.
//---------------------------------------------------------
#pragma once

#ifndef _SCRIPT_H_
#define _SCRIPT_H_

extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "dbccommon.h"
#include "Character.h"
//---------------------------------------------------------

extern lua_State*	g_pLuaState;
extern CCharacter*  g_pNoticeChar;

extern BOOL	InitLuaScript();
extern BOOL CloseLuaScript();
extern BOOL	RegisterScript();
extern BOOL LoadScript();
extern void ReloadMission();
extern void ReloadLuaSdk();
extern void ReloadLuaInit();
extern void ReloadEntity( const char szFileName[] );


//#define E_LUAPARAM		LG( "luamis_error", "lua����[%s]���������������ʹ���!\n", __FUNCTION__ ); if( g_pNoticeChar ) g_pNoticeChar->SystemNotice( "lua����[%s]���������������ʹ���!", __FUNCTION__ );
//#define E_LUANULL		LG( "luamis_error", "lua����[%s]���ݲ���ָ��Ϊ�մ���!\n", __FUNCTION__ ); if( g_pNoticeChar ) g_pNoticeChar->SystemNotice( "lua����[%s]���ݲ���ָ��Ϊ�մ���!", __FUNCTION__ );
//#define E_LUACOMPARE	LG( "luamis_error", "lua����[%s]��������Ϊδ֪�ıȽ��ַ�!\n", __FUNCTION__ ); if( g_pNoticeChar ) g_pNoticeChar->SystemNotice( "lua����[%s]��������Ϊδ֪�ıȽ��ַ�!", __FUNCTION__ );
#define E_LUAPARAM		LG( "luamis_error", "lua function[%s]param number or type error!\n", __FUNCTION__ ); if( g_pNoticeChar ) g_pNoticeChar->SystemNotice( "lua function[%s]param number or type error!", __FUNCTION__ );
#define E_LUANULL		LG( "luamis_error", "lua function[%s]pass param pointer is null and error!\n", __FUNCTION__ ); if( g_pNoticeChar ) g_pNoticeChar->SystemNotice( "lua function[%s]pass param pointer is null and error!", __FUNCTION__ );
#define E_LUACOMPARE	LG( "luamis_error", "lua function[%s]param error is unknown of compara character!\n", __FUNCTION__ ); if( g_pNoticeChar ) g_pNoticeChar->SystemNotice( "lua function[%s]param error is unknow of compara character!", __FUNCTION__ );


#define LUA_TRUE		1	// ��ȷ
#define LUA_FALSE		0	// 
#define LUA_ERROR		-1	// ����

#endif // _SCRIPT_H_

//---------------------------------------------------------