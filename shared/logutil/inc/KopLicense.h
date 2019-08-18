#ifndef _KOP_LICENSE_H_
#define _KOP_LICENSE_H_

#include <windows.h>
#include <wininet.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/pem.h>

#include <string>

// #pragma comment(lib,"libeay32")//openssl
// #pragma comment(lib, "wininet.lib" )   //http
// #pragma comment(lib,   "Netapi32.lib" )//getmac

#define PEM_STRING_LICENSE "LICENSE DATA"

// Licensing results
enum LICENSEVERFIY {
	VERFIY_OK = 0,
	VERFIY_NoFile,
	VERFIY_InvalidLic,
	VERFIY_InvalidIP,
	VERFIY_InvalidMAC,
	VERFIY_InvalidDate,
	VERFIY_UNKNOW
};

extern int RSA_Verify(int len, unsigned char *from, unsigned char *to, RSA *rsa);

/*
@brief Check if IP is in the specified list
@param iplist IP address list, separated by a semicolon as follows: 192.168.16.22; 192.168.16.23; 192.168.16.223
*/
extern int CheckIP(const char* iplist, const char *ip);
extern int LicenseVerify(const char *filename, const char *ip);

// Get the MAC address
extern std::string GetMac(const char *ip);

/*s
@brief reports server status
@param startType Run Type 1 = On 0 = Off
*/
extern int ReportState(int startType, const char *ip);

#endif //_KOP_LICENSE_H_