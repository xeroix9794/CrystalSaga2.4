#ifndef _EXTENDATTR_H_
#define _EXTENDATTR_H_

//˵��: ԭ���Ĵ�����ù̶���ʽ��ȡ����չ����,����������ʱ,����Ҫ��Ӧ�޸�,����Ч�ʺܵ�,����д


// �ַ���ת������չ����
bool		Strin2ChaExtendAttr(CCharacter *pCCha, std::string &strAttr);

// ��չ����ת�����ַ���
char*	ChaExtendAttr2String(CCharacter *pCCha, char *szAttrBuf, int nLen);

#endif