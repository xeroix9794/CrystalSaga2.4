//----------------------------------------------------------------------
// ����:UI�ű���
// ����:lh 2004-10-26
// ��;:������,����֧�����������ܹ������ⲿ�ű�
//   ע:Ŀǰ�����ڳ����ж�̬ɾ��
// ����޸�����:
//----------------------------------------------------------------------
#ifndef __UI_SCRIPT_H
#define __UI_SCRIPT_H
#include "uiguidata.h"

namespace GUI
{

template <class T>
class UIScript
{
public:
	T*		GetObj( unsigned int nIndex )	{ if(nIndex>=list.size()) return NULL;	return list[nIndex];	}
	int		AddObj( T* p )					{ list.push_back(p); return (int)(list.size()-1);				}
 	void	ReleaseAll();

private:
	typedef vector<T*> vt;
	vt		list;

};

//��ȫ�ͷ��ڴ� by Waiting 2009-06-18
template<class T> void UIScript<T>::ReleaseAll() 
{
	//��Ҫ�����������Ϊ��Ҫ���������²����ʼ����
	for(vt::reverse_iterator rit = list.rbegin(); rit!=list.rend(); rit++)
	{
		T* pObj = *rit;

		//�����˳�ʱCrash by Waiting 2009-06-29
		CNoteGraph* n = dynamic_cast<CNoteGraph*>(pObj);
		if( n )
		{
			delete n;
			continue;
		}

		CTextGraph* t = dynamic_cast<CTextGraph*>(pObj);
		if( t )
		{
			delete t;
			continue;
		}

		CHintGraph* h = dynamic_cast<CHintGraph*>(pObj);
		if( h )
		{
			delete h;
			continue;
		}

		CGraph* g = dynamic_cast<CGraph*>(pObj);
		if( g )
		{
			delete g;
			continue;
		}

		delete pObj;
	}
	list.clear();
}
}//end of namespace GUI

#endif//__UI_SCRIPT_H
