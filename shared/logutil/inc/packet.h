#ifndef PACKET_H_
#define PACKET_H_

#include "pch.h"

typedef struct tagLogPacketHdr {
	DWORD dwSize;		// Head length
	DWORD dwFullSize;	// The length of the entire package
	DWORD dwItemCount;	// The number of items in the data area
	DWORD dwReserved;	// Reserved
} LOGPACKETHDR, *PLOGPACKETHDR;

typedef struct tagLogItem {
	DWORD dwSize; // The total length of this subkey: dwSize = sizeof DWORD + sizeof data
	BYTE data[1];
} LOGITEM, *PLOGITEM;

class CLogString {
	friend class CLogPacket;

public:
	CLogString();
	CLogString(char* String);
	CLogString(PBYTE pbSrc, DWORD dwLen);
	CLogString(const CLogString& LogString);
	~CLogString();

	CLogString& operator =(const char* String);
	CLogString& operator =(const CLogString& LogString);
	
	operator LPSTR() { 
		return m_pBuf; 
	}

private:
	CHAR* m_pBuf;
	DWORD m_dwBufLen;
};

class CLogPacket {
public:
	CLogPacket(DWORD Size = 800);
	~CLogPacket(void);

	void operator <<(LOGPACKETHDR& hdr);
	void operator >>(LOGPACKETHDR& hdr);
	void operator <<(CLogString& lstr);
	void operator >>(CLogString& lstr);
	void operator <<(string& lstr);
	void operator >>(string& lstr);
	void operator <<(COPYDATASTRUCT& cds);
	void operator >>(COPYDATASTRUCT& cds);

private:
	DWORD m_Size;		// The entire package allocates the storage size
	PVOID m_pPacket;
	PLOGPACKETHDR m_pPacketHdr;
	PVOID m_pPacketData;
	DWORD m_DataSize;	// Packet size in the data area
	PBYTE m_pDataCurr;	// The current data area reads the write position pointer
};


#endif