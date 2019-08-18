// WinUnit - Maria Blees (maria.blees@microsoft.com)

/*
@file ErrorHandler.cpp
The implementation file for the application-wide error handling functions
used by WinUnit.exe.  These have been put in a separate class to avoid
cluttering up main (and so the dependencies can be more easily removed
if desired).
*/

#include "ErrorHandler.h"

#include "ReturnValues.h"
#include "Stacktrace.h"

#include <windows.h>	// for SetUnhandledExceptionFilter
#include <eh.h>			// for set_terminate
#include <crtdbg.h>		// for _CrtSetReport*
#include <signal.h>		// for signal()

#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include <sstream>

using namespace std;

// This function sets up a process-wide unhandled exception handler.
void ErrorHandler::Initialize() {
	::SetUnhandledExceptionFilter(UnhandledExceptionFilter);
	::_set_invalid_parameter_handler((_invalid_parameter_handler)InvalidParameterHandler);
}

/*
This function is intended to disable error dialogs as well as possible
to make automating running the tool easier. It is called when the -n option
is specified.
@remarks
The only settings that are per-process are SetErrorMode and
SetUnhandledExceptionFilter (the latter being called in Initialize).
The rest are per-CRT.  This means that they will only apply to errors in
executables outside this one if the external executable is using the same
CRT.  This executable (WinUnit.exe) is compiled with the /MDd switch for
Debug and the /MD switch for Release.  Test code called by this executable
needs to have been compiled with the same switches and using the same CRT
DLL in order for the CRT-specific settings to be effective.  Fortunately
the errors handled by these settings are uncommon and do indicate serious
problems that should be fixed.  When they are fixed, trapping them will
no longer be a problem!
*/

void ErrorHandler::DisableErrorDialogs() {
	s_nonInteractive = true;

	/* DWORD oldProcessErrorMode = */ ::SetErrorMode(SEM_NOGPFAULTERRORBOX);

	// Send debug-mode error messages to STDERR (no dialog box)
	// int oldErrorReportMode = 
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);

	// _HFILE oldErrorFile =
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

	// Send assert messages to the debug to STDERR (no dialog box)
	// int oldAssertReportMode =
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);

	// _HFILE oldAssertFile =
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

	// Called instead of terminate()
	// terminate_function prevTerminateFunction =
	::set_terminate(TerminateFunction);

	// Called when abort() is called
	// SignalHandlerPointer previousHandler =
	signal(SIGABRT, AbortFunction);

	// Suppresses the usual abort warning when abort() is called
	// unsigned int oldAbortFlags =
	// ::_set_abort_behavior(0, _WRITE_ABORT_MSG);

	// Send messages from "assert" to stderr rather than bringing up a dialog.
	// int oldCrtErrorMode =
	::_set_error_mode(_OUT_TO_STDERR);
}


/*
This function deals with the structured exceptions that aren't caught
elsewhere.  It displays an appropriate error message to the logger
(if set) and then either calls ExitProcess (if "no UI" was specified)
or continues on to the REAL UnhandledExceptionFilter, which shows
the customary crash dialog.
@return EXCEPTION_CONTINUE_SEARCH if we want to continue on and show
the default crash dialog.
*/
LONG WINAPI ErrorHandler::UnhandledExceptionFilter(EXCEPTION_POINTERS* pExceptionPointers) { // Pointer to information about the exception
	RuntimeStack statck(pExceptionPointers);

	std::stringstream text;
	text << "UnhandledException" << std::endl << statck << std::endl;
	std::string mText = text.str();

	std::string	strfile;
	LG_GetDir(strfile);
	strfile += "\\exception.txt";
	// FILE * fp = fopen(strfile.c_str(),"a+");
	FILE * fp = NULL;

	if (fopen_s(&fp, strfile.c_str(), "a+") != 0) {
		return EXCEPTION_CONTINUE_SEARCH;
	}

	if (fp) {
		SYSTEMTIME st;
		char tim[100] = { 0 };
		GetLocalTime(&st);
		_snprintf_s(tim, sizeof(tim), _TRUNCATE, "%02d-%02d %02d:%02d:%02d", st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		fwrite(tim, strlen(tim), 1, fp);
		fwrite(mText.c_str(), strlen(mText.c_str()) - 1, 1, fp);
		fclose(fp);
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void WINAPI ErrorHandler::InvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved) {
	std::string	strfile;
	LG_GetDir(strfile);
	strfile += "\\exception_str.txt";
	// FILE * fp = fopen(strfile.c_str(),"a+");
	FILE * fp = NULL;
	try {
		if (fopen_s(&fp, strfile.c_str(), "a+") != 0) {
			return;
		}

		if (fp) {
			SYSTEMTIME st;
			char tim[100] = { 0 };
			GetLocalTime(&st);
			_snprintf_s(tim, sizeof(tim), _TRUNCATE, "%02d-%02d %02d:%02d:%02d", st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			fwrite(tim, strlen(tim), 1, fp);
			fprintf_s(fp, "%s\n", ErrorHandler::_current.c_str());
			fclose(fp);
		}
	}
	catch (...) {
		printf("InvalidParameterHandler \n");
	}
}

/*
If ErrorHandler::DisableErrorDialogs was called (meaning the -n option
was specified), this function replaces calls to terminate().
@remarks
According to the documentation, terminate() is called in the following 
cases:
A matching catch handler cannot be found for a thrown C++ exception.
An exception is thrown by a destructor function during stack unwind.
The stack is corrupted after throwing an exception.
*/
void ErrorHandler::TerminateFunction() {
	DisplayError(L"Premature shutdown.  terminate() was called.");
	::ExitProcess(WINUNIT_EXIT_UNHANDLED_EXCEPTION);
}

// If ErrorHandler::DisableErrorDialogs was called (meaning the -n option
// was specified), this function is called when abort() is called.
void ErrorHandler::AbortFunction(int /* signal */) {
	DisplayError(L"Premature shutdown.  abort() was called.");
	::ExitProcess(WINUNIT_EXIT_UNHANDLED_EXCEPTION);
}

// This function displays error messages in a standard way, called by the
// the three error handlers above.
// @remarks
// I'm taking the risk of using non-trivial functions and the CRT here but, 
// well, the app was going to shut down anyway.
void ErrorHandler::DisplayError(const wchar_t* errorMessage, const wchar_t* details /* L"" */) {
}

void ErrorHandler::SetCurrent(const char* current) {
	_current = current;
}

bool ErrorHandler::s_nonInteractive = false;
string ErrorHandler::_current = "";
