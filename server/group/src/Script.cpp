#include "Script.h"
#include "Parser.h"
#include "LuaFunc.h"
#include <string>
#include <list>

using namespace std;

const char* GetResPath(const char *pszRes) {
	static char g_szTableName[255];
	string str = "resource";
	if (str.size() > 0)
		str += "/";

	str += pszRes;
	strncpy_s(g_szTableName, sizeof(g_szTableName), str.c_str(), _TRUNCATE);
	return g_szTableName;
}

lua_State*	g_pLuaState = NULL;

BOOL InitLuaScript() {
	g_pLuaState = lua_open();
	lua_baselibopen(g_pLuaState);
	lua_iolibopen(g_pLuaState);
	lua_strlibopen(g_pLuaState);
	lua_tablibopen(g_pLuaState);
	lua_mathlibopen(g_pLuaState);

	if (!RegisterScript())
		return FALSE;

	if (!LoadScript())
		return FALSE;

	return TRUE;
}

BOOL CloseLuaScript() {
	if (g_pLuaState) lua_close(g_pLuaState);
	g_pLuaState = NULL;
	return TRUE;
}

extern list<string> g_luaFNList;
BOOL RegisterScript() {
	lua_State *L = g_pLuaState;

	// Register the Lua function
	if (!RegisterLuaFunc())
		return FALSE;

	return TRUE;
}

void ReloadEntity(const char szFileName[]) {
	lua_dofile(g_pLuaState, szFileName);
}

BOOL LoadScript() {
	lua_dofile(g_pLuaState, GetResPath("script/apicall/apicall_get_online_count.lua"));
	return TRUE;
}
