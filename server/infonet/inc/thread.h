
/* * * * * * * * * * * * * * * * * * * *

    �̷߳�װ
    Jampe
    2006/3/27

 * * * * * * * * * * * * * * * * * * * */


#if !defined __THREAD_H__
#define __THREAD_H__

#include "datatype.h"



#define THREAD_INIT        0
#define THREAD_RESTING     1
#define THREAD_WORKING     2
#define THREAD_STOPPED     3


class Thread
{
public:
    Thread();
    virtual ~Thread();

    virtual bool Begin(int flag = 0);  //  ��ʼ�߳�
    virtual bool Resume();  //  �ָ��߳�
    virtual bool Suspend(); //  �����߳�
    virtual bool Terminate();   //  ǿ����ֹ�߳�
    virtual bool SetPriority(int priority);     //  �������ȼ�
    virtual int GetPriority();  //  ��ȡ���ȼ�
    virtual int Wait(int time = INFINITE);  //  �ȴ�ʱ��
    virtual uint Run() = 0;

public:
    HANDLE          m_thread;
    uint            m_threadid;
    void*           m_param;
    int             m_ui_idx;

};


#endif
