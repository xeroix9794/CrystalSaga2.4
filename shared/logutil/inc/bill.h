
#ifndef BILL_H_
#define BILL_H_

int PSS_GetCredit(char const* section, char const* name, std::string& strErr);

class CPAI {
public:
	CPAI();
	~CPAI() {
	}

	int Login(char const* name, char const* pwd);	// Modify the return value, 10000 successful, 10001 failed, 10004 connection failed
	
	std::string GetPassport();
	std::string LastError();
};

#endif