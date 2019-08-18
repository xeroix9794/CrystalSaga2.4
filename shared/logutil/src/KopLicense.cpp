#include "KopLicense.h"
#include "Iphlpapi.h"

#include <atlcomtime.h>
#include <atlstr.h>
#include "MyMemory.h" // Debug memory

// #pragma comment(lib,"libeay32")			//	openssl
// #pragma comment(lib, "wininet.lib" )		//	http
// #pragma comment(lib, "Netapi32.lib" )	//	getmac

int RSA_Verify(int len, unsigned char *from, unsigned char *to, RSA *rsa) {
	int i, ret, total, nblocks, block_size;
	unsigned char *buf = NULL;

	if ((from == NULL) || (to == NULL) || rsa == NULL) {
		return -1;
	}

	block_size = RSA_size(rsa);
	nblocks = len / block_size;

	if ((buf = (unsigned char *)_ALLOC(block_size)) == NULL) {
		printf("virtual memory exhausted");
		return (-1);
	}

	for (i = total = 0; i < nblocks; i++, total += ret) {
		ret = RSA_public_decrypt(block_size, &from[i * block_size], buf, rsa, RSA_PKCS1_PADDING);
		if (ret == -1) {
			printf("unable to do %d bytes RSA decryption", len);
			_FREE(buf);
			return (-1);
		}
		memcpy(&to[total], buf, ret);
	}
	_FREE(buf);
	return (total);
}

/*
@brief Check if IP is in the specified list
@param iplist IP address list, separated by semicolons, separated by commas in the following form : 192.168.16.22, 2099 / 12 / 31 12 : 59 : 59; 192.168.16.21, 2099 / 12 / 31 12: 59 : 59;
*/
int CheckIP(const char* iplist, const char *ip) {
	
	// Some commented code was removed from here.

	// Monitor whether IP is valid and return 0 if it is legal

	CAtlString str = iplist;

	CAtlString resToken;
	int curPos = 0;
	int index = 0;

	resToken = str.Tokenize(";", curPos);

	COleDateTime nowDate = COleDateTime::GetCurrentTime();

	while (resToken != "") {

		// Find the date field
		int Enter = resToken.Find('\n');
		if (Enter > 0) {
			resToken = resToken.TrimLeft("\r");
			resToken = resToken.TrimLeft("\n");
		}

		index = resToken.Find(',');
		if (index > 0) {
			CAtlString ipStr = resToken.Left(index);
			CAtlString dateStr = resToken.Mid(index + 1);
			if (ipStr.Compare(ip) == 0) {
				CAtlString year = resToken.Mid(index + 1, 4);
				int iyear = atoi(year);

				CAtlString month = resToken.Mid(index + 6, 2);
				int imonth = atoi(month);

				CAtlString day = resToken.Mid(index + 9, 2);
				int iday = atoi(day);

				CAtlString hour = resToken.Mid(index + 12, 2);
				int ihour = atoi(hour);

				CAtlString minute = resToken.Mid(index + 15, 2);
				int imin = atoi(minute);

				CAtlString sec = resToken.Mid(index + 18, 2);
				int isec = atoi(sec);

				COleDateTime date(iyear, imonth, iday, ihour, imin, isec);

				// Legal date
				// if (date.ParseDateTime(dateStr)) {
					if (nowDate <= date) {
						return 0;
					}
					else {
						return -2;  // Expired
					}
				// }
			}
		}
		else {

			// No date setting assumes never expired
			if (resToken.Compare(ip) == 0) {
				return 0;
			}
		}

		resToken = str.Tokenize(";", curPos);
	}

	return -1;
}

int LicenseVerify(const char *filename, const char *ip) {
	int retval = VERFIY_UNKNOW;
	RSA *rsa = NULL;
	BIO *in = NULL;
	char *name = NULL;
	char *header = NULL;
	unsigned char *data = NULL;
	unsigned char buf[2048];
	long len, erro = 0;
	unsigned int i;
	bool blnFind = false;
	static unsigned char modulus[] =
		"\x00\xac\xeb\xab\x42\x8c\x8f\x4e\x27\x99\xe0\x75\xdd\x6f\x55"
		"\x32\x0b\x04\x11\xad\x15\xee\xd3\x41\x37\x7f\x80\x5e\xa9\xab"
		"\x75\x73\xfd\x34\x26\xd1\x93\x4d\x37\x8e\x5a\xf7\xa6\x60\x9d"
		"\x96\xd1\x67\x9e\x3d\xb8\x15\xb1\xd9\x89\x15\x65\xde\x29\xd5"
		"\x35\x7f\x43\x72\xcd\xe1\x7d\x8c\x9f\xd7\xb2\xbe\x74\x84\x46"
		"\x31\xe8\x86\x35\x82\x45\xeb\x9e\x51\x4c\xab\x17\x9f\xce\x04"
		"\x28\x9e\x3e\x29\x14\x7d\xcd\xc7\x1b\x8e\xaa\x1b\x8e\x84\xeb"
		"\x1f\x54\x16\x20\x62\x2d\x22\x3f\xc3\x2f\xe8\x39\x21\x22\x32"
		"\x2b\x15\x13\xf4\x1b\x09\x9c\x51\xbd";

	if ((in = BIO_new(BIO_s_file())) == NULL) {
		goto end;
	}

	if (BIO_read_filename(in, filename) <= 0) {
		retval = VERFIY_NoFile;
		goto end;
	}

	// Set the public key
	rsa = RSA_new();
	rsa->n = BN_bin2bn(modulus, sizeof(modulus) - 1, rsa->n);
	BN_hex2bn(&(rsa->e), "10001");

	for (;;) {
		i = PEM_read_bio(in, &name, &header, &data, &len);
		if (i == 0) {

			// ERR_clear_error();
			break;
		}

		// License data
		if (strcmp(name, PEM_STRING_LICENSE) == 0) {
			blnFind = true;
			break;
		}
	}

	if (blnFind && len <= 2048) {
		int ret = RSA_Verify(len, data, buf, rsa);
		if (ret > 0) {
			buf[ret] = '\0';

			// Compare IP
			ret = CheckIP((const char *)buf, ip);
			if (ret == 0) {
				retval = VERFIY_OK;
			}
			else if (ret == -2) {
				retval = VERFIY_InvalidDate;
			}
			else if (ret == 0) {
				retval = VERFIY_InvalidLic;
			}
		}
		else {
			retval = VERFIY_InvalidLic;
		}
	}
	else {
		retval = VERFIY_InvalidLic;
	}
end:
	if (name != NULL) {
		OPENSSL_free(name);
	}

	if (header != NULL) { 
		OPENSSL_free(header);
	}
	
	if (data != NULL) {
		OPENSSL_free(data);
	}

	if (in != NULL) {
		BIO_free(in);
	}

	if (rsa != NULL) {
		RSA_free(rsa);
	}

	rsa = NULL; 
	in = NULL;
	name = NULL; 
	header = NULL; 
	data = NULL;
	return retval;
}

// Get the MAC address
std::string GetMac(const char *ip) {
	std::string vMacAddresses("");
	try {
		IP_ADAPTER_INFO AdapterInfo[32];			// Allocate information for up to 32 NICs   
		DWORD dwBufLen = sizeof(AdapterInfo);		// Save memory size of buffer   
		DWORD dwStatus = GetAdaptersInfo(			// Call GetAdapterInfo  
		AdapterInfo,								// [out] buffer to receive data   
		&dwBufLen);									// [in] size of receive data buffer   
		
		// No network card? Other error?    
		if (dwStatus != ERROR_SUCCESS) {
			return vMacAddresses;
		}

		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
		char szBuffer[512];
		while (pAdapterInfo) {
			if (pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET) {
				sprintf_s(szBuffer, sizeof(szBuffer), "%02X-%02X-%02X-%02X-%02X-%02X"
					, pAdapterInfo->Address[0]
					, pAdapterInfo->Address[1]
					, pAdapterInfo->Address[2]
					, pAdapterInfo->Address[3]
					, pAdapterInfo->Address[4]
					, pAdapterInfo->Address[5]);
				vMacAddresses = szBuffer;
				
				if (strcmp(ip, szBuffer) == 0) {
					break;
				}
			}
			pAdapterInfo = pAdapterInfo->Next;
		}
	}
	catch (...) {
	}
	return vMacAddresses;
}

/*
@brief reports server status
@param startType Run Type 1 = On 0 = Off
*/
int ReportState(int startType, const char *ip) {
	try {
		HINTERNET hInet;
		HINTERNET hUrl;
		DWORD dwRead = 10;
		char szContent[1024];
		char strUrl[1024];

		memset(strUrl, 0, sizeof(strUrl));
		std::string mac = GetMac(ip);

		_snprintf_s(strUrl, _countof(strUrl), _TRUNCATE, "http://rdstatic.moliyo.com/KopGate.aspx?type=%d&mac=%s&ip=%s", startType, mac.c_str(), ip);

		if (!(hInet = InternetOpen((const char*)"KopGate", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0))) {
			return -1;
		}

		if (!(hUrl = InternetOpenUrl(hInet, (const char *)strUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0))) {
			return -1;
		}

		BOOL bRead;
		bRead = InternetReadFile(hUrl, szContent, 1023, &dwRead);
		InternetCloseHandle(hUrl);
		InternetCloseHandle(hInet);

		return 0;
	}
	catch (...) {
		return -1;
	}
}
