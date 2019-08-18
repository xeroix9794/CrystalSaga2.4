#include "stdafx.h"
#include "steadyframe.h"
#include "CPerformance.h"

DWORD CSteadyFrame::_dwFPS = 0;

bool CSteadyFrame::Init()
{
	DWORD dwThread = 0;
	_dwRefreshFPS = DefaultFPS;

	_dwTotalTime=0; 
	_dwRunCount=0;	

	hThread = CreateThread( NULL, 0, _SleepThreadProc, this, 0, &dwThread);

	if( hThread != NULL )
	{
		//LG( "threadid", "%d:%s\n", dwThread, "_SleepThreadProc" );
		return true;
	}
	return false;
}

// Add by lark.li 20080923 begin
void CSteadyFrame::Exit()
{
	DWORD exitCode;
	if(GetExitCodeThread(hThread, &exitCode))
	{
		if(exitCode == STILL_ACTIVE)
		{
			if(SuspendThread(hThread) >= 0)
			{
				if(TerminateThread(hThread, 0))
				{
					//LG( "threadid", "_SleepThreadProc end\n");
					::CloseHandle(hThread);
				}
			}
			else
			{
				//DWORD error = ::GetLastError();
				//LG( "threadid", "_SleepThreadProc error %d\n", error);
			}
		}
	}
}
// End

#include "GameApp.h"
EShowWhat _GetShowWhat() 
{
	if( NULL==g_pGameApp )
		return eShowNo;

	HWND hwnd = g_pGameApp->GetHWND();
	if( NULL==hwnd )
		return eShowNo;	// �Ӵ��ر���, ����

	HWND hFore = GetForegroundWindow();
	if( hFore==hwnd )
		return eShowAll;

	if( IsIconic(hwnd) || !::IsWindowVisible(hwnd) )
		return eShowNo;

	if( hFore && IsZoomed(hFore) )
		return eShowNo;

/* ��δ�������𴰿�����Ӧ by Waiting 2009-06-24
	int nClipTypeWhileBackground = NULLREGION;
	HDC	hdc = GetDC(hwnd);
	if( !hdc )
		return eShowNo;	// ȡDCʧ��, ����

	RECT rcClipBoxWhileBackground = {0};
	nClipTypeWhileBackground = ::GetClipBox( hdc, &rcClipBoxWhileBackground );
	ReleaseDC(hwnd, hdc);
	if( nClipTypeWhileBackground==NULLREGION || nClipTypeWhileBackground==ERROR )
		return eShowNo;	// �Ӵ���ȫ����ס��, ����
*/
	return eShowSome;
}

void CSteadyFrame::_Sleep()
{
	int nCount=0;
	DWORD dwTime = GetTickCount();
	DWORD dwCurTime = dwTime;
	float fRate = 0.0;
	static DWORD s_dwLastTime = GetTickCount();
	while( true )
	{
		/*DWORD*/ dwTime = GetTickCount();
		DWORD dwElapsed = dwTime - s_dwLastTime;
		s_dwLastTime = dwTime;
		static DWORD s_dwSlower = 0;
		DWORD dwSleep = _dwTimeSpace > s_dwSlower ? _dwTimeSpace - s_dwSlower : 0;

		//���ͻ��˴��ڱ���ʱ�����ͻ���ˢ���� by Waiting 2009-06-21
		_eShowWhat = _GetShowWhat();
		if( _eShowWhat!=eShowAll )
		{
			//if( dwSleep )
			{
				if( _eShowWhat==eShowSome )
					dwSleep += 20;
				else
					dwSleep += 60;
			}
		}

        //��������Ϊ���������������º�ʱ����������˯�� by Waiting 2009-06-22
		if( dwElapsed < dwSleep * 2 )
		{
			if( dwElapsed > dwSleep )
			{
				s_dwSlower = dwElapsed-dwSleep;
				dwSleep -= s_dwSlower;
			}
			else
				s_dwSlower = 0;

			if( dwSleep > 100 )
				dwSleep = 100;

			if( dwSleep )
				Sleep( dwSleep );
		}
		else
		{
			s_dwSlower = dwSleep;
		}

		InterlockedIncrement( &_lRun );

		nCount++;
		if( nCount >= DefaultFPS )
		{
			dwCurTime = GetTickCount();
			fRate = (float)_dwTotalTime / (float)(dwCurTime - dwTime);
			if( fRate < 0.5f )
			{
				_dwRefreshFPS = _dwFPS + 3;
			}
			else if( fRate < 0.6f )
			{
				_dwRefreshFPS = _dwFPS + 2;
			}
			else if( fRate < 0.7f )
			{
				_dwRefreshFPS = _dwFPS + 1;
			}
			else if( fRate > 0.98f )
			{
				_dwRefreshFPS = _dwFPS - 3;
			}
			else if( fRate > 0.95f )
			{
				_dwRefreshFPS = _dwFPS - 2;
			}
			else if( fRate > 0.9f )
			{
				_dwRefreshFPS = _dwFPS - 1;
			}
			else
			{
				_dwRefreshFPS = _dwFPS;
			}

			if( _dwRefreshFPS>DefaultFPS + 3 )
				_dwRefreshFPS = DefaultFPS + 3;
			else if( _dwRefreshFPS<DefaultFPS - 3 )
				_dwRefreshFPS = DefaultFPS - 3;

			nCount=0;
			dwTime = dwCurTime;
			_dwTotalTime=0; 
			_dwRunCount=0;
		}
	}
}

bool CSteadyFrame::Run()
{
	if( _lRun>0 )
	{
		_lRun = 0;
		_dwCurTime = GetTickCount();

		_dwRunCount++;
		return true;
	}
	return false;
}

void CSteadyFrame::End()
{
	_dwTotalTime += GetTickCount() - _dwCurTime;
}
