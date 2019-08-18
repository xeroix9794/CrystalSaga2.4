#include "stdafx.h"

#include "Sql.h"

SQL::SQL(std::string server, std::string user, std::string password)
{
	std::string InConnStr = "DRIVER={SQL Server};SERVER=" + server + ";UID=" + user + ";PWD=" + password + ";";

	// Allocate environment handle
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);

		// Allocate connection handle
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);

				retcode = SQLDriverConnectA(
					hdbc,
					NULL,
					(SQLCHAR*)InConnStr.c_str(),
					InConnStr.length(),
					OutConnStr,
					255,
					&OutConnStrLen,
					SQL_DRIVER_NOPROMPT);

				if (retcode == SQL_ERROR)
				{
					DisplayErrors(SQL_HANDLE_DBC, hdbc);
				}
			}
		}
	}
}

SQL::~SQL()
{
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
}

void DisplayError(SQLCHAR* SqlState, SQLINTEGER NativeError, SQLCHAR* Msg, SQLSMALLINT MsgLen)
{
	printf("SQL Error State: %s, Native Error Code: %i, Error: %s\n", SqlState, NativeError, Msg);
}

void SQL::DisplayErrors(SQLSMALLINT handleType, SQLHANDLE handle)
{
	SQLCHAR		SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
	SQLINTEGER	NativeError;
	SQLSMALLINT	i, MsgLen;
	SQLRETURN	rc2;

	// Get the status records
	i = 1;
	while ((rc2 = SQLGetDiagRecA(handleType, handle, i, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen)) != SQL_NO_DATA)
	{
		DisplayError(SqlState, NativeError, Msg, MsgLen);
		i++;
	}
}