#ifndef XSTRING_H
#define XSTRING_H

#include "TChar.h"

// �Ӹ����ַ���(_TCHAR *in)�ĸ���λ��(long *in_from)��ȡ�ַ��ܣ�
// ֱ�������ַ����б�(_TCHAR *end_list)�е���һ�ַ�ʱ����
long StringGet(_TCHAR *out, long out_max, _TCHAR *in, long *in_from, _TCHAR *end_list, long end_len);


// �Ӹ����ַ���(_TCHAR *in)�ĸ���λ��(long *in_from)�޳��ַ����б�(_TCHAR *end_list)�е���һ�ַ�
void StringSkipCompartment(_TCHAR *in, long *in_from, _TCHAR *skip_list, long skip_len);





















#endif
