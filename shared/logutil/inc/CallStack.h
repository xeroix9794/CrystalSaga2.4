#ifndef __CALL_STACK_H
#define __CALL_STACK_H
#include <string>
using namespace std;
extern int AddCallingFunctionName(string &info, int nSkipFunc=1); // The first is definitely the text, the default skip
void WriteSystemInfo(FILE* fp);
#endif //__CALL_STACK_H