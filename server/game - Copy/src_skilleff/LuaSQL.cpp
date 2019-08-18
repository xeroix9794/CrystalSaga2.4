#include "stdafx.h"
#include "Script.h"
#include <future>
#include "LuaSQL.h"
#include "Lua.h"
#include "Sql.h"

void ExecuteQuery(char* query, char* host, char* user, char* pass)
{
	// Connect to sql server
	SQL Sql(host, user, pass);

	// Allocate statement handle
	if (Sql.retcode == SQL_SUCCESS || Sql.retcode == SQL_SUCCESS_WITH_INFO)
	{
		Sql.retcode = SQLAllocHandle(SQL_HANDLE_STMT, Sql.hdbc, &Sql.hstmtA);

		// Execute query
		Sql.retcode = SQLExecDirectA(Sql.hstmtA, (SQLCHAR*)query, SQL_NTS);

		// Check for errors
		if (Sql.retcode == SQL_SUCCESS_WITH_INFO || Sql.retcode == SQL_ERROR)
		{
			//Sql.DisplayErrors(SQL_HANDLE_STMT, Sql.hstmt);
		}

		// Free statement handle
		SQLFreeHandle(SQL_HANDLE_STMT, Sql.hstmtA);
	}
}

std::vector<std::future<void>> pending_queries;
int ExecuteQueryAsync(lua_State *L)
{
	if (lua_gettop(L) != 4)
	{
		return 0;
	}
	if (lua_isstring(L, 1) == 0)
	{
		return 0;
	}
	if (lua_isstring(L, 2) == 0)
	{
		return 0;
	}
	if (lua_isstring(L, 3) == 0)
	{
		return 0;
	}
	if (lua_isstring(L, 4) == 0)
	{
		return 0;
	}

	const char* hostPtr = lua_tostring(L, 1);
	const char* userPtr = lua_tostring(L, 2);
	const char* passPtr = lua_tostring(L, 3);
	const char* queryPtr = lua_tostring(L, 4);

	char* host = new char[strlen(hostPtr) + 1];
	strncpy_s(host, strlen(hostPtr) + 1, hostPtr, strlen(hostPtr) + 1);
	char* user = new char[strlen(userPtr) + 1];
	strncpy_s(user, strlen(userPtr) + 1, userPtr, strlen(userPtr) + 1);
	char* pass = new char[strlen(passPtr) + 1];
	strncpy_s(pass, strlen(passPtr) + 1, passPtr, strlen(passPtr) + 1);
	char* query = new char[strlen(queryPtr) + 1];
	strncpy_s(query, strlen(queryPtr) + 1, queryPtr, strlen(queryPtr) + 1);

	auto f = std::async(std::launch::async, ExecuteQuery, query, host, user, pass);
	pending_queries.push_back(std::move(f));

	return 1;
}

unsigned int handleC = 0;
std::map<unsigned int, SQL*> handle;
int LuaSql(lua_State *L)
{
	// Check for arguments
	if (lua_gettop(L) == 0)
	{
		lua_pushnumber(L, LUA_ARG_ERROR);
		return 1;
	}
	// Check and read command
	if (lua_isstring(L, 1) == 0)
	{
		lua_pushnumber(L, LUA_ARG_ERROR);
		return 1;
	}
	const char* command = lua_tostring(L, 1);

	// Create a connection to the database
	if (strcmp(command, "connect") == 0)
	{
		// Check arguments
		if (lua_gettop(L) != 4)
		{
			lua_pushnumber(L, 0);
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 2;
		}
		if (lua_isstring(L, 2) == 0)
		{
			lua_pushnumber(L, 0);
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 2;
		}
		if (lua_isstring(L, 3) == 0)
		{
			lua_pushnumber(L, 0);
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 2;
		}
		if (lua_isstring(L, 4) == 0)
		{
			lua_pushnumber(L, 0);
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 2;
		}

		// Get connection info
		const char* host = lua_tostring(L, 2);
		const char* user = lua_tostring(L, 3);
		const char* pass = lua_tostring(L, 4);

		// Connect to SQL Server
		SQL* Sql = new SQL(host, user, pass);

		// Check if connection was successful
		switch (Sql->retcode)
		{
		case SQL_SUCCESS:
		case SQL_SUCCESS_WITH_INFO:
			// Save connection
			handle.emplace(handleC, Sql);
			// Return handle id
			lua_pushnumber(L, 1);
			lua_pushnumber(L, handleC++);
			break;
		default:
			lua_pushnumber(L, 0);
			lua_pushnumber(L, Sql->retcode);
			break;
		}
		return 2;
	}
	// Execute query
	else if (strcmp(command, "query") == 0)
	{
		// Check arguments
		if (lua_gettop(L) != 3)
		{
			lua_pushnumber(L, 0);
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 2;
		}
		if (lua_isnumber(L, 2) == 0)
		{
			lua_pushnumber(L, 0);
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 2;
		}
		if (lua_isstring(L, 3) == 0)
		{
			lua_pushnumber(L, 0);
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 2;
		}

		// Check if connection handle is valid
		int connHandleId = (int)lua_tonumber(L, 2);
		if (handle.count(connHandleId) == 0)
		{
			lua_pushnumber(L, 0);
			lua_pushnumber(L, SQL_INVALID_HANDLE);
			return 2;
		}

		// Get connection
		SQL* Sql = handle.at(connHandleId);

		// Get query
		const char* query = lua_tostring(L, 3);

		// Allocate statement handle
		SQLHSTMT hstmt;
		Sql->retcode = SQLAllocHandle(SQL_HANDLE_STMT, Sql->hdbc, &hstmt);

		// Save statement handle
		Sql->hstmt.emplace(Sql->hstmtC, hstmt);

		// Execute query
		Sql->retcode = SQLExecDirectA(hstmt, (SQLCHAR*)query, SQL_NTS);

		// Return 1 on success, otherwise return 0
		switch (Sql->retcode)
		{
		case SQL_SUCCESS:
		case SQL_SUCCESS_WITH_INFO:
			lua_pushnumber(L, 1);
			lua_pushnumber(L, Sql->hstmtC++);
			break;
		default:
			//Sql->DisplayErrors(SQL_HANDLE_STMT, hstmt);
			lua_pushnumber(L, 0);
			lua_pushnumber(L, Sql->retcode);
		}
		return 2;
	}
	// Fetch data
	else if (strcmp(command, "fetch") == 0)
	{
		// Check arguments
		if (lua_gettop(L) != 3)
		{
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 1;
		}
		if (lua_isnumber(L, 2) == 0)
		{
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 1;
		}
		if (lua_isnumber(L, 3) == 0)
		{
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 1;
		}

		// Check if connection handle is valid
		int connHandleId = (int)lua_tonumber(L, 2);
		if (handle.count(connHandleId) == 0)
		{
			lua_pushnumber(L, SQL_INVALID_HANDLE);
			return 1;
		}

		// Get connection
		SQL* Sql = handle.at(connHandleId);

		// Check if statement handle is valid
		int stmtHandleId = (int)lua_tonumber(L, 3);
		if (Sql->hstmt.count(stmtHandleId) == 0)
		{
			lua_pushnumber(L, SQL_INVALID_HANDLE);
			return 1;
		}

		// Get statement handle
		SQLHSTMT hstmt = Sql->hstmt.at(stmtHandleId);

		// Fetch next row
		Sql->retcode = SQLFetch(hstmt);
		if (Sql->retcode == SQL_NO_DATA)
		{
			lua_pushnumber(L, Sql->retcode);
			return 1;
		}

		// Get number of columns
		SQLSMALLINT numCols;
		Sql->retcode = SQLNumResultCols(hstmt, &numCols);

		// Create lua table for fetched data
		lua_newtable(L);

		for (int i = 1; i < numCols + 1; i++)
		{
			// Get column info
			SQLCHAR colName[64];
			SQLULEN colSize;
			Sql->retcode = SQLDescribeColA(hstmt, i, colName, sizeof(colName), NULL, NULL, &colSize, NULL, NULL);

			// Push column name on stack
			lua_pushstring(L, (CHAR*)colName);

			// Fetch column data
			char* value = new char[colSize];
			SQLLEN null;
			Sql->retcode = SQLGetData(hstmt, i, SQL_C_CHAR, value, colSize, &null);
			switch (Sql->retcode)
			{
			case SQL_SUCCESS:
			case SQL_SUCCESS_WITH_INFO:
				break;
			default:
				lua_pushnumber(L, Sql->retcode);
				return 1;
			}

			// Check if data is null and push it on stack
			if (null == SQL_NULL_DATA)
			{
				lua_pushstring(L, "NULL");
			}
			else
			{
				lua_pushstring(L, value);
			}
			// Add fetched column data to the table
			lua_settable(L, -3);
		}
	}
	// Free statement handle
	else if (strcmp(command, "freehandle"))
	{
		// Check arguments
		if (lua_gettop(L) != 3)
		{
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 1;
		}
		if (lua_isnumber(L, 2) == 0)
		{
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 1;
		}
		if (lua_isnumber(L, 3) == 0)
		{
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 1;
		}

		// Check if connection handle is valid
		int connHandleId = (int)lua_tonumber(L, 2);
		if (handle.count(connHandleId) == 0)
		{
			lua_pushnumber(L, SQL_INVALID_HANDLE);
			return 1;
		}

		// Get connection
		SQL* Sql = handle.at(connHandleId);

		// Check if statement handle is valid
		int stmtHandleId = (int)lua_tonumber(L, 3);
		if (Sql->hstmt.count(stmtHandleId) == 0)
		{
			lua_pushnumber(L, SQL_INVALID_HANDLE);
			return 1;
		}

		// Free handle resources
		Sql->retcode = SQLFreeHandle(SQL_HANDLE_STMT, Sql->hstmt.at(stmtHandleId));
		if (Sql->retcode == SQL_SUCCESS)
		{
			Sql->hstmt.erase(stmtHandleId);
		}

		// Return result
		lua_pushnumber(L, Sql->retcode);
	}
	// Close database connection
	else if (strcmp(command, "close"))
	{
		// Check and read arguments
		if (lua_gettop(L) != 2)
		{
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 1;
		}
		if (lua_isnumber(L, 2) == 0)
		{
			lua_pushnumber(L, LUA_ARG_ERROR);
			return 1;
		}
		int handleId = (int)lua_tonumber(L, 2);
		if (handle.count(handleId) == 0)
		{
			lua_pushnumber(L, SQL_INVALID_HANDLE);
			return 1;
		}

		SQL Sql = *handle.at(handleId);
		Sql.retcode = SQLDisconnect(Sql.hdbc);
		handle.erase(handleId);

		lua_pushnumber(L, Sql.retcode);
	}
	return 1;
}