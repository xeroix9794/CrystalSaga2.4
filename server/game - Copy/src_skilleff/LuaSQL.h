#pragma once

#include "Lua.h"

#define LUA_ARG_ERROR 10

int LuaSql(lua_State *L);
int ExecuteQueryAsync(lua_State *L);