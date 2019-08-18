
#include "StdAfx.h"
#include "GameMovie.h"


CGameMovie::CGameMovie(void)
{
	_pGraph		= 0;
	_pControl	= 0;
	_pVidWin	= 0;
	_pPosition	= 0;
	_hWnd		= 0;
	_pEvent		= 0;
	_hResult	= CoInitialize(0);
}


CGameMovie::~CGameMovie(void)
{
	Cleanup();
	CoUninitialize();
}


// ����
bool CGameMovie::Play(HWND hWnd, TCHAR* pszFileName)
{
	Cleanup();

	_hResult = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
								IID_IGraphBuilder, (void**)&_pGraph);
	if(FAILED(_hResult))
	{
		Cleanup();
		return false;
	}

	_hResult = _pGraph->QueryInterface(IID_IMediaControl, (void**)&_pControl);
	if(FAILED(_hResult))
	{
		Cleanup();
		return false;
	}

	_hResult = _pGraph->QueryInterface(IID_IVideoWindow, (void**)&_pVidWin);
	if(FAILED(_hResult))
	{
		Cleanup();
		return false;
	}

	_hResult = _pGraph->QueryInterface(IID_IMediaEventEx, (void **)&_pEvent);
	if(FAILED(_hResult))
	{
		Cleanup();
		return false;
	}
	_pEvent->SetNotifyWindow((OAHWND)hWnd, WM_GRAPHNOTIFY, 0);


#ifdef _UNICODE
	_hResult = _pGraph->RenderFile(pszFileName, 0);
#else
	WCHAR szFileNameW[MAX_PATH + 1] = {0};
    MultiByteToWideChar(CP_ACP, 0, pszFileName, -1, szFileNameW, MAX_PATH);
	_hResult = _pGraph->RenderFile(szFileNameW, 0);
#endif
	if(FAILED(_hResult))
	{
		Cleanup();
		return false;
	}

	_hResult = _pVidWin->put_Owner((OAHWND)hWnd);
	if(FAILED(_hResult))
	{
		Cleanup();
		return false;
	}

	RECT rc;
	GetClientRect(hWnd, &rc);

	//_pVidWin->SetWindowPosition((rc.right - rc.left - 720)/2, (rc.bottom - rc.top - 404)/2, 720, 404);
	//	int height = (rc.right - rc.left) * 9.0 / 16.0;
	int height = (int)((rc.right - rc.left) * 9.0 / 16.0);

	_pVidWin->SetWindowPosition(rc.left, (rc.bottom - rc.top - height)/2, rc.right - rc.left, height);

	_pVidWin->put_WindowStyle(WS_CHILD|WS_CLIPSIBLINGS);

	//_hResult = _pVidWin->put_FullScreenMode(OATRUE);
	_pVidWin->put_Visible(OATRUE);

	_hResult = _pControl->Run();
	if(FAILED(_hResult))
	{
		Cleanup();
		return false;
	}

	_hResult = _pGraph->QueryInterface(IID_IMediaPosition,(void**)&_pPosition);
	if(FAILED(_hResult))
	{
		Cleanup();
		return false;
	}

	_hWnd = hWnd;
	return true;
}


// ֹͣ����
void CGameMovie::Stop(void)
{
	if(_pControl)
	{
		_hResult = _pControl->Stop();
	}
}


// ���
void CGameMovie::Cleanup()
{
	Stop();
	_hWnd = 0;

	SAFE_RELEASE(_pControl);
	SAFE_RELEASE(_pGraph);
	SAFE_RELEASE(_pVidWin);
	SAFE_RELEASE(_pPosition);
	SAFE_RELEASE(_pEvent);
}


// �Ƿ����ڲ���
bool CGameMovie::IsPlaying(void)
{
	try
	{
		if(_pPosition)
		{
			REFTIME nCurrent(0), nLength(0);
			_pPosition->get_CurrentPosition(&nCurrent);
			_pPosition->get_Duration(&nLength);

			return nCurrent < nLength;
		}
	}
	catch(...)
	{
	}

	return false;
}

void CGameMovie::HandleGraphEvent()
{
    // Disregard if we don't have an IMediaEventEx pointer.
    if (_pEvent == NULL)
    {
        return;
    }
    // Get all the events
    long evCode;
    LONG_PTR param1, param2;
//    HRESULT hr;
    while (SUCCEEDED(_pEvent->GetEvent(&evCode, &param1, &param2, 0)))
    {
        _pEvent->FreeEventParams(evCode, param1, param2);
        switch (evCode)
        {
        case EC_COMPLETE:  // Fall through.
        case EC_USERABORT: // Fall through.
        case EC_ERRORABORT:
 			
		
            Cleanup();
            PostQuitMessage(0);
            return;
        }
    } 
}

CGameMovie*	m_pMovie;