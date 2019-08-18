// Script.cpp Created by knight-gongjian 2004.12.1.
//---------------------------------------------------------

#include "Script.h"
#include "NpcScript.h"
#include "CharScript.h"
#include "EntityScript.h"

//---------------------------------------------------------
extern const char* GetResPath(const char *pszRes);

CCharacter* g_pNoticeChar = NULL;
lua_State*	g_pLuaState = NULL;

BOOL InitLuaScript()
{
	g_pLuaState = lua_open();
	lua_baselibopen( g_pLuaState );
	lua_iolibopen( g_pLuaState );
	lua_strlibopen( g_pLuaState );
	lua_tablibopen( g_pLuaState );
	lua_mathlibopen( g_pLuaState );

	if( !RegisterScript() )
		return FALSE;
	
	if( !LoadScript() )
		return FALSE;

	return TRUE;
}

BOOL CloseLuaScript()
{
	if( g_pLuaState ) lua_close( g_pLuaState );
	g_pLuaState = NULL;
	return TRUE;
}

BOOL RegisterScript()
{
	if( !RegisterCharScript() || !RegisterNpcScript() )
		return FALSE;

	if( !RegisterEntityScript() )
		return FALSE;

	return TRUE;
}

void ReloadLuaInit()
{
	lua_dofile( g_pLuaState, GetResPath("script/initial.lua") );
}

void ReloadLuaSdk()
{	
	lua_dofile( g_pLuaState, GetResPath("script/MisSdk/NpcSdk.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisSdk/MissionSdk.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisSdk/scriptsdk.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/ScriptDefine.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/NpcDefine.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/templatesdk.lua") );

	// 由updateall会触发ai_sdk更新
	lua_dofile( g_pLuaState, GetResPath("script/birth/birth_conf.lua"));
	lua_dofile( g_pLuaState, GetResPath("script/ai/ai.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/calculate/skilleffect.lua"));
}

void ReloadNpcScript()
{
	// 装载NPC任务数据信息
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/MissionScript01.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/MissionScript02.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/MissionScript03.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/MissionScript04.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/MissionScript05.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/MissionScript06.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/MissionScript07.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/MissionScript08.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/SendMission.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/EudemonScript.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/CharBornScript.lua") );

	// 装载NPC对话数据信息
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/NpcScript01.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/NpcScript02.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/NpcScript03.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/NpcScript04.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/NpcScript05.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/NpcScript06.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/NpcScript07.lua") );
	lua_dofile( g_pLuaState, GetResPath("script/MisScript/NpcScript08.lua") );
}

void ReloadEntity( const char szFileName[] )
{
	lua_dofile( g_pLuaState, szFileName );
}

BOOL LoadScript()
{
	ReloadLuaInit();
	ReloadLuaSdk();
	ReloadNpcScript();
	return TRUE;
}
