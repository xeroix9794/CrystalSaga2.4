#ifndef __CSTEADY_FRAME_H
#define __CSTEADY_FRAME_H

#define DefaultFPS  60
enum EShowWhat
{
	eShowNo,
	eShowSome,
	eShowAll,
};

// �����ȶ�֡��,���Ҳ���֡
// ʵ�ַ���:������Ⱦ����
class CSteadyFrame
{
public:
	CSteadyFrame() : _eShowWhat(eShowNo) { 
		SetFPS( DefaultFPS );
	}

	bool	Init();

	static DWORD	GetFPS()	{ return _dwFPS;		}
	void	SetFPS( DWORD v )	{ 
		_dwFPS = v;	
		_dwTimeSpace = (int)(1000.0f/(float)_dwFPS);
	}

	bool	Run();
	
	// ȡ������Windows��������ε���ʾ Add by Waiting 2009-06-21
	EShowWhat GetShowWhat() { return _eShowWhat; }

	// Add by lark.li 20080923 begin
	void	Exit();
	// End

	DWORD	GetTick()		{ return _dwCurTime;		}
	void	End();

	void	RefreshFPS()	{ if(_dwFPS!=_dwRefreshFPS) SetFPS(_dwRefreshFPS);	}

private:
	static DWORD WINAPI _SleepThreadProc( LPVOID lpParameter ){
		((CSteadyFrame*)lpParameter)->_Sleep();
		return 0;
	}

	void	_Sleep();

private:
	static DWORD	_dwFPS;			// �趨��FPS,һ��Ҫ��Ⱦ����֡

	long	_lRun;

	DWORD	_dwCurTime;	
	DWORD	_dwTimeSpace;

	DWORD	_dwTotalTime;
	DWORD	_dwRunCount;

	DWORD	_dwRefreshFPS;

	// Add by lark.li 20080923 begin
	HANDLE hThread;
	// End
	EShowWhat _eShowWhat;
};
#endif//__CSTEADY_FRAME_H