#if !defined(LOGUTIL_PRECOMPILED_HEADER_FILE)
#define LOGUTIL_PRECOMPILED_HEADER_FILE

// Platform unrelated macro definition
#define LINE_COMMENT /##/
#define LC LINE_COMMENT
#define $ LINE_COMMENT

// C standard library
#include <stdio.h>
#include <time.h>

// C ++ standard library
#include <list>
#include <map>
#include <list>
#include <string>
using namespace std;


// Platform related definitions
#if defined(WIN32)

// Win32 platform required to define and include
#pragma warning(disable : 4251)
#pragma warning(disable : 4786)

#include <windows.h>
#include <io.h>
#include <direct.h>

#elif defined(LINUX)

// Linux platform required to define and include
#include <unistd.h>
#include <string.h>

#include <sys/io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/dir.h>

#define VOID void
#define BYTE unsigned char
#define LPBYTE unsigned char*
#define BOOL bool
#define DWORD unsigned long
#define LPCSTR const char*
#define TRUE true
#define FALSE false
#define HANDLE void*

#define _LOG
#define _LOG_CONSOLE
#elif defined(FREEBSD)

// FreeBSD platform required to define and include
#endif
#endif
