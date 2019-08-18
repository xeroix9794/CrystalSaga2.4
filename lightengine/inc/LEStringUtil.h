#ifndef LEStringUtil_H
#define LEStringUtil_H

#include "LEEffPrerequisites.h"
#include "LEStringVector.h"

class LIGHTENGINE_API LEStringUtil
{
public:
	typedef std::stringstream StrStreamType;

public:
	/** ��ȥ�κοո��ַ�, ��׼�ո�����ո��Tabsbe�ȵ�.
	@remarks
	����ָ���Ƿ���ȥǰ�˻����Ŀո�Ĭ�϶���ȥ.
	*/
	static void trim(String& str, bool left = true, bool right = true);

	/** ����LEStringVector�������ڷָ���֮��������Ӵ�.
	@param
	delims �ָ����б�
	@param
	maxSplits ִ�зָ��������������Ϊ0�����޴ηָ�������ָ���������0
	��ָ������Ӵ������ﵽ�����Ϊֹ.
	*/
	static LEStringVector split(const std::string& str, const std::string& delims = "\t\n ", unsigned int maxSplits = 0);

	/** ���ַ����е��ַ�ȫ��ת����Сд.
	*/
	static void toLowerCase(String& str);

	/** ���ַ����е��ַ�ȫ��ת���ɴ�дд.
	*/
	static void toUpperCase(String& str);

};
#endif