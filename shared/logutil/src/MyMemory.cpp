#include "util2.h"
#include "MyMemory.h"

#ifdef __MEM_DEBUG__ 
#include "CPerformance.h"
#define __POOL__ 		// The original list of forms of memory records, divided into several groups
#define POOL_SIZE 1024	// Each group has up to 1024 lists

// test results, the efficiency is better than the full list, so debugging memory, can have better efficiency
// Memory allocation records, mining list structure does not use STL to avoid repeated records in the STL
struct ALLOC_INFO {
	void* address;
	size_t size;
	char file[64];
	DWORD line;

#ifndef __POOL__
	ALLOC_INFO* pNext;
#endif

};

#ifdef __POOL__

// Each group is also a list
struct MEM_POOL {
	ALLOC_INFO  Pool[POOL_SIZE];
	int	nUseCount;		// The number of memory objects in this group
	size_t nSizeCount;	// The total number of memory used in this group
	int nLastUsed;		// The last one used
	int nFirstUnused;	// The first unused Index
	MEM_POOL* pNext;
	MEM_POOL* pPrev;
};
#endif //__POOL__

class CAllocList {
#ifdef _DEBUG
	BOOL CheckError();
#endif
public:
#ifdef __POOL__
	MEM_POOL* pHead;
	MEM_POOL* pEnd;
#else
	ALLOC_INFO* pHead;
#endif
	size_t g_dwMaxUseMemory;
	size_t g_dwCurUseMemory;
	DWORD m_nCurObjCnt;
	DWORD m_nMaxObjCnt;
	CRITICAL_SECTION Mem_Crit;
	static BOOL bInited;
	static BOOL bEnding;
	static BOOL bIsMySelf;

	// Add a project at the end
	inline void AddInfo(void* address, size_t size, const char* file, DWORD line);
	inline void DelInfo(void* address);
	void DumpUnfreed(int nCnt = 0);
	int nDumpWhile;
	int nDumpPatch;

	CAllocList();
	virtual ~CAllocList();
};

CAllocList* allocList;
BOOL CAllocList::bInited = FALSE;
BOOL CAllocList::bEnding = FALSE;
BOOL CAllocList::bIsMySelf = FALSE;

CAllocList::CAllocList() {
	pHead = NULL;

#ifdef __POOL__
	pEnd = NULL;
#endif

	nDumpWhile = 4000;
	nDumpPatch = 0;
	g_dwMaxUseMemory = 0;
	g_dwCurUseMemory = 0;
	m_nCurObjCnt = 0;
	m_nMaxObjCnt = 0;
	InitializeCriticalSection(&Mem_Crit);
}

CAllocList::~CAllocList() {
	DeleteCriticalSection(&Mem_Crit);

#ifdef __POOL__
	MEM_POOL* pCur = pHead;
	while (pCur) {
		MEM_POOL* pNext = pCur->pNext;
		free(pCur);
		pCur = pNext;
	}
#else
	ALLOC_INFO* pCur = pHead;
	while(pCur) {
		ALLOC_INFO* pNext = pCur->pNext;
		free(pCur);
		pCur = pNext;
	}
#endif

}

void CAllocList::AddInfo(void* address, size_t size, const char* file, DWORD line) {
	if (NULL == address) {
		return;
	}

#ifdef __POOL__
	EnterCriticalSection(&Mem_Crit);
	MEM_POOL* pPrev = NULL;
	MEM_POOL* pPool = pHead;
	int nIndex = 0;
	while (pPool) {
		if (pPool->nUseCount < POOL_SIZE) {
			nIndex = pPool->nFirstUnused;
			while (pPool->Pool[nIndex].address) {
				nIndex++;
				if (nIndex >= POOL_SIZE) {
					nIndex = 0;
				}
			}
			break;
		}
		pPrev = pPool;
		pPool = pPool->pNext;
	}

	if (pPool == NULL) {
		pPool = (MEM_POOL*)malloc(sizeof(MEM_POOL));
		memset(pPool, 0, sizeof(MEM_POOL));
		if (pPrev) {
			pPrev->pNext = pPool;
			pPool->pPrev = pPrev;
		}
		else {
			pHead = pPool;
		}
		pEnd = pPool;
	}

	if (pPool) {
		pPool->Pool[nIndex].address = address;
		pPool->Pool[nIndex].size = size;
		strncpy_s(pPool->Pool[nIndex].file, _countof(pPool->Pool[nIndex].file), file, _TRUNCATE);
		pPool->Pool[nIndex].line = line;
		pPool->nUseCount++;
		pPool->nSizeCount += size;
		pPool->nLastUsed = nIndex;
		pPool->nFirstUnused = nIndex == POOL_SIZE - 1 ? 0 : nIndex + 1;
		m_nCurObjCnt++;
		if (m_nMaxObjCnt < m_nCurObjCnt) {
			m_nMaxObjCnt = m_nCurObjCnt;
		}
	}
	
	// CheckError();
	g_dwCurUseMemory += size;
	if (g_dwMaxUseMemory < g_dwCurUseMemory) {
		g_dwMaxUseMemory = g_dwCurUseMemory;
	}

	LeaveCriticalSection(&Mem_Crit);
	// if (size > 2050) 

#else
	ALLOC_INFO* pNew = (ALLOC_INFO*)malloc(sizeof(ALLOC_INFO));
	if (pNew) {
		pNew->address = address;
		pNew->size = size;
		strncpy_s(pNew->file, _countof(pNew->file), file, _TRUNCATE); 
		pNew->line = line;

		EnterCriticalSection(&Mem_Crit);
		pNew->pNext = pHead;
		pHead = pNew;
		g_dwCurUseMemory += size;
		if (g_dwMaxUseMemory < g_dwCurUseMemory) {
			g_dwMaxUseMemory = g_dwCurUseMemory;
		}
		LeaveCriticalSection(&Mem_Crit);
	}
#endif
}

long g_nDontCare = 0;
void CAllocList::DelInfo(void* address) {
	if (NULL == address) {
		if (!g_nDontCare) {
			int kk = 0;
		}
		return;
	}
	EnterCriticalSection(&Mem_Crit);

#ifdef __POOL__
	MEM_POOL* pPrev = NULL;
	MEM_POOL* pPool = pEnd;
	BOOL bFound = FALSE;
	int nIndex = 0;
	while (pPool) {
		nIndex = pPool->nLastUsed;
		while (1) {
			if (pPool->Pool[nIndex].address == address) {
				bFound = TRUE;
				break;
			}
		
			nIndex--;
			
			if (nIndex < 0) {
				nIndex = POOL_SIZE - 1;
			}
			
			if (nIndex == pPool->nLastUsed) {
				break;
			}
		}

		if (bFound) {
			break;
		}
		pPool = pPool->pPrev;
	}

	if (bFound) {
		g_dwCurUseMemory -= pPool->Pool[nIndex].size;
		pPool->nUseCount--;
		m_nCurObjCnt--;
		// if( pPool->Pool[nIndex].size> 2050 )
		{
			IP("%s (%d) : Del %08x size %d\n", pPool->Pool[nIndex].file, pPool->Pool[nIndex].line, (DWORD)pPool->Pool[nIndex].address, pPool->Pool[nIndex].size);
		}

		if (pPool->nUseCount == 0) {
			if (pPool->pPrev) {
				pPool->pPrev->pNext = pPool->pNext;
			}
			else {
				pHead = pPool->pNext;
			}

			if (pPool->pNext) {
				pPool->pNext->pPrev = pPool->pPrev;
			}
			else {
				pEnd = pPool->pPrev;
			}
		
			free(pPool);
		}
		else {
			pPool->Pool[nIndex].address = 0;
			pPool->nSizeCount -= pPool->Pool[nIndex].size;
			if (pPool->nLastUsed == nIndex) {
				pPool->nLastUsed--;
				if (pPool->nLastUsed < 0) {
					pPool->nLastUsed = POOL_SIZE - 1;
				}
			}
			pPool->nFirstUnused = nIndex;
		}
	}
	else {
		if (!g_nDontCare) {
			IP("Unknown Del %08x\n", address);
		}
	}
	
	// CheckError();
#else
	ALLOC_INFO* pPrev = NULL;
	ALLOC_INFO* pCur = pHead;
	while(pCur) {
		if (pCur->address == address) {
			if (pPrev) {
				pPrev->pNext = pCur->pNext;
			}
			else {
				pHead = pCur->pNext;
			}
			g_dwCurUseMemory -= pCur->size;
			free(pCur);
			break;
		}
		pPrev = pCur;
		pCur = pCur->pNext;
	}
#endif
	LeaveCriticalSection(&Mem_Crit);
}

#ifdef _DEBUG
BOOL CAllocList::CheckError() {
	MEM_POOL* pPool = pHead;
	while(pPool) {
		int c = 0;
		for (int i = 0; i < POOL_SIZE; i++) {
			if (pPool->Pool[i].address) {
				c++;
			}
		}

		if (c != pPool->nUseCount) {
			return TRUE;
		}

		pPool = pPool->pNext;
	}
	return FALSE;
}
#endif

void CAllocList::DumpUnfreed(int nCnt) {
	int c = 0;
	size_t totalSize = 0;
	EnterCriticalSection(&Mem_Crit);

#ifdef __POOL__
	MEM_POOL* pPool = pHead;
	while (pPool) {
		totalSize += pPool->nSizeCount;
		c += pPool->nUseCount;
		pPool = pPool->pNext;
	}
	
	/*
	At the end of the application, the detection of unbuffered memory begins
	Here is the use of 8 records each time the record, but not all of the print is not released, you should look at the last line of output
	*/

	if (CAllocList::bEnding && c < nDumpWhile) {
		IP("unfree cnt=%u size=%u\n", c, totalSize);
		nDumpWhile = c;
		pPool = pHead;

		if (nDumpPatch + 8 >= c) {
			nDumpPatch = 0;
		}

		int n = 0;
		ALLOC_INFO* pPrev = NULL;
		BOOL bDumpOK = FALSE;
		while (pPool) {
			for (int i = 0; i < POOL_SIZE; i++) {
				ALLOC_INFO* pInfo = &pPool->Pool[i];
				if (pInfo->address) {
					if (n >= nDumpPatch && n < nDumpPatch + 8) {
						if (pPrev == NULL || strcmp(pInfo->file, pPrev->file) || pInfo->line != pPrev->line) {
							IP("%s (%d) : Address %08x size %d\n", pInfo->file, pInfo->line, (DWORD)pInfo->address, pInfo->size);
							pPrev = pInfo;
							n++;
							if (n == nDumpPatch + 8) {
								nDumpPatch = n;
								break;
							}
						}
					}
					else if (n < nDumpPatch) {
						n++;
					}
					else {
						bDumpOK = TRUE;
						break;
					}
				}
			}
			pPool = pPool->pNext;
		}
	}
#else
	ALLOC_INFO* pInfo = pHead;
	while (pInfo) {
		totalSize += pInfo->size; 
		pInfo = pInfo->pNext;
		c++;
	}

	if (CAllocList::bEnding && c < nDumpWhile) {
		IP("unfree cnt=%u size=%u\n", c, totalSize);
		nDumpWhile = c;
		pInfo = pHead;

		if (nDumpPatch + 8 >= c) {
			nDumpPatch = 0;
		}

		int n = 0;
		ALLOC_INFO* pPrev = NULL;
		while(pInfo) {
			if (n >= nDumpPatch && n < nDumpPatch + 8) {
				if (pPrev == NULL || strcmp(pInfo->file,pPrev->file) || pInfo->line != pPrev->line) {
					IP("%s (%d) : Address %08x size %d\n", pInfo->file, pInfo->line, (DWORD)pInfo->address, pInfo->size); 
					pPrev = pInfo;
					n ++;
					if (n == nDumpPatch + 8) {
						nDumpPatch = n;
						break;
					}
				}
			}
			else if(n < nDumpPatch) {
				n++;
			}
			else {
				break;
			}

			pInfo = pInfo->pNext;
		}
	}
#endif
	LeaveCriticalSection(&Mem_Crit);
};

size_t GetMaxUseMemory() { return allocList ? allocList->g_dwMaxUseMemory : 0; }
size_t GetCurUseMemory() { return allocList ? allocList->g_dwCurUseMemory : 0; }
DWORD  GetCurObject()	 { return allocList ? allocList->m_nCurObjCnt : 0; }
DWORD  GetMaxObject()	 { return allocList ? allocList->m_nMaxObjCnt : 0; }

void AddTrack(void* addr, size_t asize, const char *fname, unsigned long lnum) {
	if (CAllocList::bIsMySelf) {
		return;
	}

	if (CAllocList::bEnding) {
		return;
	}

	if (!CAllocList::bInited) {
		InitMemoryCheck();
	}

	allocList->AddInfo(addr, asize, fname, lnum);
};

void RemoveTrack(void* addr) {
	if (CAllocList::bIsMySelf) {
		return;
	}

	if (!CAllocList::bInited) {
		return;
	}

	allocList->DelInfo(addr);
	if (CAllocList::bEnding) {
		if (allocList->pHead == NULL) {
			CAllocList::bInited = FALSE;
			CAllocList::bIsMySelf = TRUE;
			::delete allocList;
			CAllocList::bIsMySelf = FALSE;
			allocList = NULL;
			IP("memory all clear !!\n");
		}
		else {
			allocList->DumpUnfreed(1);
		}
	}
};
#endif //__MEM_DEBUG__

void InitMemoryCheck() {
#ifdef __MEM_DEBUG__
	if (CAllocList::bInited) {
		return;
	}

	if (NULL == allocList) {

		// Identify this variable to avoid unnecessary work
		CAllocList::bIsMySelf = TRUE;
		allocList = ::new CAllocList;
		CAllocList::bIsMySelf = FALSE;
	}
	CAllocList::bInited = TRUE;
#endif
}

void EndMemoryCheck() {
#ifdef __MEM_DEBUG__
	CAllocList::bEnding = TRUE;
#endif
}

void FormatDollorString(char* szBuf, size_t dwBufSize, size_t dwDollor) {
	if (dwDollor > 1000000) {
		_stprintf_s(szBuf, dwBufSize, TEXT("%3u,%03u,%03u"), dwDollor / 1000000, (dwDollor / 1000) % 1000, dwDollor % 1000);
	}
	else if (dwDollor > 1000) {
		_stprintf_s(szBuf, dwBufSize, TEXT("%3u,%03u"), dwDollor / 1000, dwDollor % 1000);
	}
	else {
		_stprintf_s(szBuf, dwBufSize, TEXT("%3u"), dwDollor);
	}
}