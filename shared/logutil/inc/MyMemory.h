#ifndef __MY_MEMORY_H
#define __MY_MEMORY_H

#include <windows.h>
#include <tchar.h>
#include "MyAlloc.h"

#ifdef __cplusplus
#ifdef __MEM_DEBUG__

// Overloaded global new, there may still be some places where no calls are made here, if the objects are your concern, the idea subroutines the header file and tracks whether the call is made

inline void * __cdecl operator new(size_t size, const char *file, int line)  {
	void *ptr = (void *)malloc(size); 
	AddTrack(ptr, size, file, line); 
	return(ptr); 
};

inline void __cdecl operator delete(void* p, LPCSTR /* lpszFileName */, int /* nLine */) {
	RemoveTrack(p); 
	free(p);
}; 

inline void __cdecl operator delete[](void* p, LPCSTR /* lpszFileName */, int /* nLine */) {
	RemoveTrack(p); 
	free(p);
}; 

inline void __cdecl operator delete(void* p) {
	::operator delete(p,__FILE__,__LINE__);
}

inline void __cdecl operator delete[](void* p) {
	::operator delete(p,__FILE__,__LINE__);
}

#define DEBUG_NEW new(__FILE__, __LINE__) 

#else // !__MEM_DEBUG__ -> Release Ver.
#define DEBUG_NEW new
#endif // Release

#define new DEBUG_NEW 
#endif /* __cplusplus */


// Initialize memory debugging

extern void InitMemoryCheck();

// End the memory debug and start detecting unackaged memory / Individual processes, DLLs need to be called separately
extern void EndMemoryCheck();

// Format the output number, using a thousand semicolon, used to print the amount of memory
extern void FormatDollorString(char* szBuf, size_t dwBufSize, size_t dwDollor);

#endif //__MY_MEMORY_H