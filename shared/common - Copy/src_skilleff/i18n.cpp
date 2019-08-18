#include ".\i18n.h"

const char* ConvertResString(const char* str)
{
	string key(str);

	if( '#' == *key.begin() && '#' == *key.rbegin())
	{
		string temp = key.substr(1, key.size() - 2);
		return CResourceBundleManage::Instance()->LoadResString(temp.c_str());
	}

	return str;
}
