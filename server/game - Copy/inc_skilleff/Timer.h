//=============================================================================
// FileName: Expand.h
// Creater: 
// Date: 2004.11.22
// Comment: 
//=============================================================================

#ifndef TIMER_H
#define TIMER_H

/**
 * @class CTimer 
 * @author 
 * @brief �򵥶�ʱ��������ʹ�ûص����ƣ�ֻ�ܵ��߳�ʹ��
 *					ֻ���ֶ�����IsOK���ж�ʱ���Ƿ���		
 */
class CTimer
{
public:

	/**
	* @brif ��ʼ
	* @param[in]  dwInterval  ʱ����
	*/
    void Begin(DWORD dwInterval)
    {
        _dwLastTick = GetTickCount();
        _dwInterval = dwInterval;
    }

	/**
	* @brif �Ƿ񵽶�ʱʱ��
	* @param[in]  dwCurTime  ����ʱ��
	*
	* @return DWORD ��ʱʱ���ص�ʱ�ı���
	*/
	DWORD IsOK(DWORD dwCurTime)
    {
		if (dwCurTime < _dwLastTick)
		{
			_dwLastTick = dwCurTime;
			return 0;
		}
		DWORD	dwExecTime = (dwCurTime - _dwLastTick) / _dwInterval;
		if (dwExecTime)
			_dwLastTick = dwCurTime - ((dwCurTime - _dwLastTick) % _dwInterval);

		return dwExecTime;
    }

	/**
	* @brif ����
	*/
	void Reset(void) {_dwLastTick = GetTickCount();}

	/**
	* @brif ���ؼ��
	*/
	DWORD GetInterval(void) {return _dwInterval;}

	/**
	* @brif �趨���
	* @param[in]  dwInterval  ʱ����
	*/
	void SetInterval(DWORD dwInterval) {_dwInterval = dwInterval;}

protected:

    DWORD _dwLastTick;
    DWORD _dwInterval;
};

#endif // TIMER_H