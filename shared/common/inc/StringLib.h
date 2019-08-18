#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include <string>
using namespace std;

string StringLimit(const string& str,size_t len);
bool GetNameFormString(const string &str,string &name);
string CutFaceText(string &text,size_t cutLimitlen);
//void ReplaceText(string &text,const string strRpl);
//void FilterText(string &text,vector<char*> *p_strFilterTxt);
void ChangeParseSymbol(string &text,int nMaxCount);

//��InBuf�е��ַ�����nWidth��Żس�����pOutBuf��
//��ǿ�������ڲ���ȫ��Ч�ʣ��ⲿֱ�ӵ��ü��ɣ��贫��pOutBuf���� -Waiting Modify 2009-05-25
int StringNewLine(char* pOutBuf, int nOutBufSize, unsigned int nWidth, const char* pInBuf, unsigned int nInLen);

//��һ�����ֵ��ַ���szBuf����nCount����ָ���cSplit
const char* StringSplitNum( long nNumber, int nCount=3, char cSplit=',' );


