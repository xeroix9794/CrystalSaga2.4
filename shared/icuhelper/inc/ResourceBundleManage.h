/**
 * Copyright (C), 2008-2010.
 * @file  ResourceBundleManage.h
 * @brief ��Դ�����ļ�
 * @history
 * 
 * @author Lark.Li
 * @version 1.0
 * @date 2008-11-18
 * @warning ����ʹ��ICU1.38�汾
 * 
 * @todo ��Ϊʱ��ԭ����������Եļ̳���
*/
#pragma once

#include <unicode/resbund.h>            // ��Դ������Ϣ
#include <unicode/ucnv.h>               // �ַ�����ת��
#include <unicode/uclean.h>             // �ַ�����ת��
#include <unicode/msgfmt.h>             // ��ʽ���ַ���
#include <unicode/brkiter.h>            // ����
#include <unicode/uchar.h>              // �ַ�����

#include <map>                          // map

using namespace std;

/**
 * @class CFormatParameter 
 * @author Lark.Li
 * @brief Ϊ��ʽ����Ϣ������ʹ�õĲ�����װ�����ⲻͬ���԰汾�Ĳ���λ�ò�ͬ����
*/
class CFormatParameter
{
private:
	Formattable* m_MsgArgs;
	int paraNum;
private:
	CFormatParameter(){}
public:
	CFormatParameter(int paraNum);
	~CFormatParameter();

	/**
	* @brief ȡ�ø�ʽ������
	* @return Formattable ��ʽ������
	*/
	Formattable* GetMsgArgs(){ return m_MsgArgs;} 

	/**
	* @brief ȡ�ò�������
	* @return int ��������
	*/
	int GetParaNum() { return paraNum; }

	/**
	* @brief ���ø���������
	* @param[in]  int  ����λ��
	* @param[in]  double  ����ֵ
	*/
	void setDouble(int index, double d);

	/**
	* @brief ����32λ��������
	* @param[in]  int  ����λ��
	* @param[in]  int32_t  ����ֵ
	*/
	void  setLong(int index, int32_t l);

	/**
	* @brief ����64λ��������
	* @param[in]  int  ����λ��
	* @param[in]  int64_t  ����ֵ
	*/
	void  setInt64(int index, int64_t ll) ;

	/**
	* @brief �������ڲ���
	* @param[in]  int  ����λ��
	* @param[in]  UDate  ����ֵ
	*/
	void  setDate(int index, UDate d) ;

	/**
	* @brief �����ַ�������
	* @param[in]  int  ����λ��
	* @param[in]  UnicodeString  ����ֵ
	*/
	void  setString(int index, const UnicodeString &stringToCopy);
};

/**
 * @class CResourceBundleManage 
 * @author Lark.Li
 * @brief �������е���Դ�ַ���
*/
class CResourceBundleManage
{
private:
	CResourceBundleManage(){}
	CResourceBundleManage(const char* configFileName);

public:
	virtual ~CResourceBundleManage(void);

	typedef map <string, string> StringMap;

private:
	char* m_ResDir;				// ��Դλ��
	char* m_ResLocale;			// ��Դ����

	Locale* m_pLocale;			// �����趨

	StringMap mapRes;
	ResourceBundle* m_pResourceBundle;
	UConverter *m_pConverter;
	int m_MaxSize;

	//int m_LogFlag;

	//FILE* m_LogFile;

    static CResourceBundleManage* _instance;

private:
	UErrorCode ToCodePageString(UConverter *conv, UChar* source, char* target, int destCapacity, int& len);
	bool Init();

public:

 	/**
	* @brief ʵ������Դ������
	* @param[in]  const char*  �����ļ�����
    * @exception resource is used before init
    * @exception resource is init twince
    * @return Locale ��Դ������
	*/
   static CResourceBundleManage* Instance(const char* configFileName = NULL);

	/**
	* @brief ȡ���趨��������Ϣ
	* @return Locale ������Ϣ
	*/
	const Locale& GetLocale() { return *m_pLocale; }

	/**
	* @brief ȡ�������ַ���ת����
	* @return UConverter �ַ���ת����
	*/
	const UConverter* GetConverter() { return m_pConverter; }

	/**
	* @brief ȡ����Դ����
	* @return int ��Դ����
	*/
	int GetSize(void);
	
	/**
	* @brief �ͷ���Դ
	*/
	void Release(void);

	/**
	* @brief ����keyȡ���ַ���value
	* @param[in]  const char*  �ַ�����key
	* @return const char* �ַ���value
	*/
	const char* LoadResString(const char* key);

	/**
	* @brief ����keyȡ���ַ���value(UnicodeString )
	* @param[in]  const char*  �ַ�����key
	* @return UnicodeString �ַ���value(UnicodeString )
	*/
	UnicodeString LoadUResString(const char* key);

	/**
	* @brief ���ݲ�����ʽ���ַ���
	* @param[in]  const char*  �ַ�����key
	* @param[in]  CFormatParameter&  ����Ĳ���
	* @param[out]  char[]  ���ݲ�����ʽ������ַ���
	* @return int �ַ����ĳ���
	*/
	int Format(const char* key, CFormatParameter& parameter, char buffer[]);

	/**
	* @brief ���ݲ�����ʽ���ַ���
	* @param[in]  const char*  �ַ�����format
	* @param[in]  CFormatParameter&  ����Ĳ���
	* @param[out]  char[]  ���ݲ�����ʽ������ַ���
	* @return int �ַ����ĳ���
	*/
	int FormatString(const char* formatStr, CFormatParameter& parameter, char buffer[]);
};

/**
 * @class CBreakLine 
 * @author Lark.Li
 * @brief ���ַ������ж��д���
*/
class CBreakLine
{
public:
	/**
	* @brief ����
	* @param[in]  const UnicodeString&  Ҫ������ַ���
	* @param[in]  const Locale&  �趨������
	* @param[in]  int32_t ÿ�е��ַ���
	* @param[out]  int32_t[] ÿ�е���ԭ���е���ʼλ��
	* @param[out]  int32_t[] ÿ�е���ԭ���е���ֹλ��
	* @param[in]  int32_t �������
	* @return int ʵ�ʵ�����
	*/
	static int32_t WrapParagraph(const UnicodeString& s, const Locale& locale, int32_t maxCharsPreLine, int32_t lineStarts[], int32_t lineEnds[], int32_t maxLines);
};
