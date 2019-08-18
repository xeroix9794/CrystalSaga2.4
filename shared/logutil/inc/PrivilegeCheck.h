#pragma once

#include <utility>
#include <vector>
#include <map>
#include <fstream>

using namespace std;

class PrivilegeCheck {
private:
	PrivilegeCheck(void);

public:
	~PrivilegeCheck(void);

	static PrivilegeCheck* Instance();	// The only instance

	bool Init(const char* configFileName);
	bool HasPrivilege(int level, const char* command, const char* accountName = "", const char* ip = "");

	bool IsAdmin(const char* accountName, const char* ip = "");

private:
	typedef vector<string> CommandV;			// Command array
	typedef map<int, CommandV> PrivilegeMap;	// Permission collection

	typedef CommandV::iterator CommandIt;		// Command
	typedef PrivilegeMap::iterator PrivilegeIt;	// Authority

	static PrivilegeCheck* _instance;
	PrivilegeMap m_mapPrivilege;
};
