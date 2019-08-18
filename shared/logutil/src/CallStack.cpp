#include <windows.h>
#include <imagehlp.h> // link with imagehlp.lib as well...
#include <stdio.h>
#include <stdlib.h>
#include <winerror.h>
#include <string>

using namespace std;

#pragma warning(disable:4311) // type cast warning

#ifndef _MSC_VER
#error Visual C++ Required (Visual C++ specific information is displayed)
#endif

typedef BOOL(__stdcall *tSymCleanup)(IN HANDLE hProcess);
typedef PVOID(__stdcall *tSymFunctionTableAccess)(HANDLE hProcess, DWORD AddrBase);
typedef DWORD(__stdcall *tSymGetModuleBase)(IN HANDLE hProcess, IN DWORD dwAddr);
typedef BOOL(__stdcall *tSymGetModuleInfo)(IN HANDLE hProcess, IN DWORD dwAddr, OUT PIMAGEHLP_MODULE ModuleInfo);
typedef DWORD(__stdcall *tSymGetOptions)(VOID);
typedef DWORD(__stdcall *tSymSetOptions)(IN DWORD SymOptions);
typedef DWORD(__stdcall WINAPI *tUnDecorateSymbolName)(PCSTR DecoratedName, PSTR UnDecoratedName, DWORD UndecoratedLength, DWORD Flags);
typedef BOOL(__stdcall *tSymGetSymFromAddr)(IN HANDLE hProcess, IN DWORD dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_SYMBOL Symbol);
typedef BOOL(__stdcall *tSymInitialize)(IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess);

typedef BOOL(__stdcall *tStackWalk)(DWORD MachineType, HANDLE hProcess,HANDLE hThread, LPSTACKFRAME StackFrame, PVOID ContextRecord,
									PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine, PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
									PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine, PTRANSLATE_ADDRESS_ROUTINE TranslateAddress);

#define sizeof_Name 128
#define sizeof_CONTEXT sizeof(CONTEXT)+96
#define sizeof_STACKFRAME sizeof(STACKFRAME)+16
#define sizeof_symbol sizeof(IMAGEHLP_SYMBOL)+sizeof_Name

int AddCallingFunctionName(string &info, int nSkipFunc) {
	info += "Call Stack:\n";
	char temp[80];

	// we load imagehlp.dll dynamically because the NT4-version does not
	// offer all the functions that are in the NT5 lib
	HINSTANCE hImagehlpDll = LoadLibrary("imagehlp.dll");
	if (hImagehlpDll == NULL) {
		_snprintf_s(temp, _countof(temp), _TRUNCATE, "LoadLibrary( \"imagehlp.dll\" ): LastError = %lu\n", GetLastError());
		info += temp;
		return 0;
	}

	tSymCleanup	pSymCleanup = (tSymCleanup)GetProcAddress(hImagehlpDll, "SymCleanup");
	tSymFunctionTableAccess pSymFunctionTableAccess = (tSymFunctionTableAccess)GetProcAddress(hImagehlpDll, "SymFunctionTableAccess");
	tSymGetModuleBase pSymGetModuleBase = (tSymGetModuleBase)GetProcAddress(hImagehlpDll, "SymGetModuleBase");
	tSymGetSymFromAddr pSymGetSymFromAddr = (tSymGetSymFromAddr)GetProcAddress(hImagehlpDll, "SymGetSymFromAddr");
	tSymInitialize pSymInitialize = (tSymInitialize)GetProcAddress(hImagehlpDll, "SymInitialize");
	//	tSymGetModuleInfo pSymGetModuleInfo = (tSymGetModuleInfo) GetProcAddress(hImagehlpDll, "SymGetModuleInfo");
	//	tSymGetOptions pSymGetOptions = (tSymGetOptions) GetProcAddress(hImagehlpDll, "SymGetOptions");
	//	tSymSetOptions pSymSetOptions = (tSymSetOptions) GetProcAddress(hImagehlpDll, "SymSetOptions");
	//	tUnDecorateSymbolName pUnDecorateSymbolName	= (tUnDecorateSymbolName) GetProcAddress(hImagehlpDll, "UnDecorateSymbolName");
	tStackWalk pStackWalk = (tStackWalk)GetProcAddress(hImagehlpDll, "StackWalk");

	if (pSymCleanup == NULL || pSymFunctionTableAccess == NULL || pSymGetModuleBase == NULL ||
		pSymGetSymFromAddr == NULL || pSymInitialize == NULL ||
		// pSymGetModuleInfo == NULL || pSymGetOptions == NULL || 
		// pSymSetOptions == NULL || pUnDecorateSymbolName == NULL ||
		pStackWalk == NULL) {
		
		info += "GetProcAddress(): some required function not found.";
		FreeLibrary(hImagehlpDll);
		return 0;
	}

	// Initialize the IMAGEHLP package to decode addresses to symbols
	// Get image filename of the main executable
	char* pPath = NULL;
	char filepath[MAX_PATH];
	DWORD filepathlen = GetModuleFileName(NULL, filepath, sizeof(filepath));
	if (filepathlen == 0) {
		info += "NtStackTrace: Failed to get pathname for program\n";
	}
	else {
	
		// Strip the filename, leaving the path to the executable
		char* lastdir = strrchr(filepath, '/');
		if (lastdir == NULL) {
			lastdir = strrchr(filepath, '\\');
		}

		if (lastdir != NULL) {
			lastdir[0] = '\0';
		}

		// Initialize the symbol table routines, supplying a pointer to the path
		pPath = filepath;
		if (strlen(filepath) == 0) {
			pPath = NULL;
		}
	}

	HANDLE hProc = GetCurrentProcess();
	HANDLE hThread = GetCurrentThread();
	if (!pSymInitialize(hProc, pPath, TRUE)) {
		info += "NtStackTrace: failed to initialize symbols\n";
		FreeLibrary(hImagehlpDll);
		return 0;
	}

	// Allocate and initialize frame and symbol structures
	STACKFRAME* frm = (STACKFRAME*)malloc(sizeof_STACKFRAME);
	IMAGEHLP_SYMBOL* sym = (IMAGEHLP_SYMBOL*)malloc(sizeof_symbol);
	if (NULL == frm || NULL == sym) {
		pSymCleanup(hProc);
		info += "malloc error\n";
		FreeLibrary(hImagehlpDll);
		return 0;
	}
	memset(frm, 0, sizeof(STACKFRAME));
	memset(sym, 0, sizeof_symbol);
	sym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
	sym->MaxNameLength = sizeof_Name - 1;

	// Initialize the starting point based on the architecture of the current machine
	DWORD machType = IMAGE_FILE_MACHINE_I386;

	// The CONTEXT structure is not used on x86 systems
	CONTEXT* cxt = NULL;

	// Initialize the STACKFRAME to describe the current routine
	frm->AddrPC.Mode = AddrModeFlat;
	frm->AddrStack.Mode = AddrModeFlat;
	frm->AddrFrame.Mode = AddrModeFlat;

	// If we were called from an exception handler, the exception
	// structure would contain an embedded CONTEXT structure. We
	// could initialize the following addresses from the CONTEXT
	// registers passed to us.

	// Use _asm to fetch the processor register values
	int i;
	_asm mov i, esp // Stack pointer (CONTEXT .Esp field)
	frm->AddrStack.Offset = i;

	_asm mov i, ebp // Frame pointer (CONTEXT .Ebp field)
	frm->AddrFrame.Offset = i;

	// We'd like to fetch the current instruction pointer, but the x86 IP
	// register is a bit special. Use roughly the current offset instead
	// of a dynamic fetch (use offset because address should be past the prologue).
	// _asm mov i, ip // ip is a special register, this is illegal
	// frm->AddrPC.Offset = i;
	frm->AddrPC.Offset = ((DWORD)&AddCallingFunctionName) + 0x08c;

	// The top stack frame is the call to this routine itself -
	// the next is the routine that called us, and the third is
	// the one that called our caller. That's the one we want to
	// return.
	DWORD symDisp, lastErr;
	BOOL stat;
	for (i = 0; i < 6; i++) {

		// Call the routine to trace to the next frame
		stat = pStackWalk(machType, hProc, hThread, frm, cxt, NULL, pSymFunctionTableAccess, pSymGetModuleBase, NULL);
		if (!stat) {
			lastErr = GetLastError();
			if (lastErr == ERROR_NOACCESS || lastErr == ERROR_INVALID_ADDRESS || lastErr == ERROR_SUCCESS) {
				
				// TO REMOVE
			}
			else {
				_snprintf_s(temp, _countof(temp), _TRUNCATE, TEXT("<stack walk terminated with error %d>\n"), lastErr);
				info += temp;
			}
			break;
		}

		// Decode the closest routine symbol name
		if (i > nSkipFunc - 1) {
			if (pSymGetSymFromAddr(hProc, frm->AddrPC.Offset, &symDisp, sym)) {
				_snprintf_s(temp, _countof(temp), _TRUNCATE, TEXT("%08x at %08x %s\n"), frm->AddrPC.Offset, sym->Address, sym->Name);
				info += temp;
			}
			else {
				_snprintf_s(temp, _countof(temp), _TRUNCATE, TEXT("%08x "), frm->AddrPC.Offset);
				info += temp;
				lastErr = GetLastError();
				if (lastErr == ERROR_INVALID_ADDRESS) { // Seems normal for last frame on Intel
					info += "<No Symbol Available>\n";
				}
				else {
					_snprintf_s(temp, _countof(temp), _TRUNCATE, TEXT("<error:%d>\n"), lastErr);
					info += temp;
				}
			}
		}
	}

	pSymCleanup(hProc);
	if (cxt) {
		free(cxt); // If on Intel, freeing the NULL CONTEXT is a no-op...
	}
	
	free(frm);
	free(sym);
	FreeLibrary(hImagehlpDll);
	
	return 0;
}
