#pragma once

#include <map>
#include <sqlext.h>

class SQL
{
public:
	SQL(std::string, std::string, std::string);
	~SQL();

	void DisplayErrors(SQLSMALLINT handleType, SQLHANDLE);

public:
	SQLHENV		henv;
	SQLHDBC		hdbc;
	SQLHSTMT	hstmtA;
	SQLRETURN	retcode;

	SQLCHAR		OutConnStr[255];
	SQLSMALLINT	OutConnStrLen;

	unsigned int hstmtC = 0;
	std::map<unsigned int, SQLHSTMT> hstmt;
};