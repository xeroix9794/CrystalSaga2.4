#ifndef _INPUTBOX_H_
#define _INPUTBOX_H_
#include <sstream>
#endif

// ��һ�����ص�Window Edit�ؼ�����������������
// ����windows�ؼ���������ֵ����,ɾ��,�˸�,����,ճ��,���λ�ü���ȷ����Ĵ���
class CInputBox 
{

public:


	CInputBox::CInputBox()
	:_bAccountMode(FALSE), _hEdit(0), _IsDigit( false )
	{

	}

	void	SetIsDigit( bool IsDigit ) { _IsDigit = IsDigit; }
	HWND    GetEditWindow()						{ return _hEdit;  }
	void	SetEditWindow(HWND hEdit)			{ _hEdit = hEdit; }
	void	HandleWindowMsg(DWORD dwMsgType, DWORD dwParam1, DWORD dwParam2);
	
	
	void	SetAccountMode(BOOL bAccount);		// ����Ϊ�ʺ�����ģʽ, ���ε��ո�, ���ŵ�
	void	SetPasswordMode(BOOL bPassword);	// ����Ϊ����ģʽ, �õ�*���ַ���
	void	SetDigitalMode(BOOL bDigitalMode);	// ����Ϊ����ģʽ, ֻ������������
	void	SetMaxNum(int nMaxNum);				// ��������������������
	void	SetMultiMode(BOOL bMultiLine);		// ����Ϊ����ģʽ	
	void	SetSel(int nStartChar, int nEndChar);


	void	SetCursorTail();					// ��������õ�����β��
	char*	RefreshText();
	int		RefreshCursor();
	void	ClearText();
	char*	SetText(const char *pszText);
	void	ReplaceSel(const char *pszRplText, BOOL bCanUndo = TRUE);	//�滻��ǰѡ������ - added by Arcol

protected:

	char	_szText[1024];	// ���������
	int		_nCursorPos;	// ��������ִ��е�λ��
	BOOL	_bAccountMode;	// �Ƿ����ʺ�ģʽ
	HWND	_hEdit;
	bool	_IsDigit;
};


inline void CInputBox::SetMaxNum(int nMaxNum)
{
	::SendMessage(_hEdit, EM_LIMITTEXT, nMaxNum, 0);
}

inline char* CInputBox::RefreshText()
{
	static string oldText;
	oldText = _szText;
	
	GetWindowText(_hEdit, _szText, 1000);

	if( _IsDigit && strlen( _szText ) )
	{
		std::istringstream str( _szText );
		int tst;
		str >> tst;
		if( str.fail() || !str.eof() )
		{
			SetText( oldText.c_str() );
			::SendMessage(_hEdit, EM_SETSEL, _nCursorPos, oldText.size() );
		}
	}
	
	size_t length = oldText.size();
	if( length >= 3 && oldText.substr( 0, length - 1 ) == _szText )
	{
		const char* oldtext = oldText.c_str();
		char c1 = oldtext[length - 3];
		char c2 = oldtext[length - 2];
		char c3 = oldtext[length - 1];

		if( c1 == '#' && c2 >= '0' && c2 <= '9' && c3 >= '0' && c3 <= '9' )
		{
			_szText[length - 3] = 0;
			SetText( _szText );
			SetCursorTail();
		}
	}
	
	return _szText;
}

inline char* CInputBox::SetText(const char *pszText)
{
	SetWindowText(_hEdit, pszText);
	//strcpy(_szText, pszText);
	strncpy_s(_szText,sizeof(_szText), pszText,_TRUNCATE);
	return _szText;
}

inline void CInputBox::ClearText()
{
	//strcpy(_szText, "");
	strncpy_s(_szText,sizeof(_szText), "",_TRUNCATE);
	SetWindowText(_hEdit, "");
}

inline int CInputBox::RefreshCursor()
{
	::SendMessage(_hEdit, EM_GETSEL, (WPARAM)&_nCursorPos, 0);
	return _nCursorPos;
}

inline void CInputBox::SetCursorTail()
{
	int nLen = (int)(strlen(_szText));
	::SendMessage(_hEdit, EM_SETSEL, nLen, nLen);
}

inline void CInputBox::SetSel(int nStartChar, int nEndChar)
{
	::SendMessage(_hEdit, EM_SETSEL, nStartChar, nEndChar);
}

inline void CInputBox::HandleWindowMsg(DWORD dwMsgType, DWORD dwParam1, DWORD dwParam2)
{
	// ����Ϸ���ڵ�windows�ļ������봫�ݵ�edit�ؼ�
	// if(!(dwMsgType==WM_KEYDOWN || dwMsgType==WM_CHAR)) return;

	// if(dwMsgType==WM_IME_NOTIFY || dwMsgType==WM_IME_STARTCOMPOSITION) return;

	if(dwMsgType==WM_KEYDOWN || dwMsgType==WM_KEYUP)
	{
		if(dwParam1==VK_UP) return; // ���ε�UP��, ���ⴥ��Edit�ؼ��Ķ�Ӧ����
		// ::SendMessage(_hEdit, WM_KEYDOWN, dwParam1, dwParam2);
	}
	else if(dwMsgType==WM_CHAR)
	{
		if(_bAccountMode)
		{
			if(dwParam1==' ' || dwParam1==',') return;
		}
	}
	::SendMessage(_hEdit, dwMsgType, dwParam1, dwParam2);
}

inline void CInputBox::ReplaceSel(const char *pszRplText, BOOL bCanUndo)
{
	::SendMessage(_hEdit, EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)(LPCTSTR)pszRplText);
}

extern CInputBox g_InputBox;
