#pragma once

#ifndef _SCRIPT_H_
#define _SCRIPT_H_

extern "C" {
#include "lua.h"
#include "lualib.h"
#include <lauxlib.h>
#include <lualib.h>
}

#include <dbccommon.h>

extern BOOL	InitLuaScript();
extern BOOL CloseLuaScript();
extern BOOL	RegisterScript();
extern BOOL LoadScript();
extern void ReloadEntity(const char szFileName[]);

#define E_LUAPARAM		LG("luamis_error", "lua function [%s] takes wrong num parameter or wrong type patameter!", __FUNCTION__); if (g_pNoticeChar) g_pNoticeChar->SystemNotice("Lua function [%s] number of parameters or type wrong!", __FUNCTION__);
#define E_LUANULL		LG("luamis_error", "lua function [%s] patameter is null!", __FUNCTION__); if (g_pNoticeChar) g_pNoticeChar->SystemNotice("The lua function [%s] passed an argument pointer to a null error!", __FUNCTION__);
#define E_LUACOMPARE	LG("luamis_error", "lua function [%s] unknow compare character!", __FUNCTION__); if (g_pNoticeChar) g_pNoticeChar->SystemNotice("Lua function [%s] parameter error for the unknown comparison characters!", __FUNCTION__);

#define LUA_TRUE		1
#define LUA_FALSE		0
#define LUA_ERROR		-1

#endif // _SCRIPT_H_
