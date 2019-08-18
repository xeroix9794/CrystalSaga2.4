//----------------------------------------------------------------------
// ����:�������ؼ�
// ����:lh 2004-11-02
// ��;:��������һ����������ֻ��һ��ָ���ⲿʵ��Ŀ�ݷ�ʽ
// ����޸�����:
//----------------------------------------------------------------------

#pragma once
#include "UICommandCompent.h"

namespace GUI
{
typedef void (*GuiComEvent)(CGuiData *pSender,CCommandObj* pItem,bool& isAccept);

class CCommandObj;
class CFastCommand : public CCommandCompent
{
public:
	CFastCommand(CForm& frmOwn);
	CFastCommand(const CFastCommand& rhs);
	CFastCommand& operator=(const CFastCommand& rhs);
	~CFastCommand();
	GUI_CLONE(CFastCommand)

	virtual void		Render();
	virtual bool		MouseRun( int x, int y, DWORD key );
	virtual eAccept		SetCommand( CCommandObj* p, int x, int y );
    virtual void		DragRender();
    virtual CCompent*   GetHintCompent( int x, int y );
    virtual void        RenderHint( int x, int y );

	static void			DelCommand( CCommandObj* p );

	static CFastCommand*	FintFastCommand( CCommandObj* p );		// ���Ҷ�Ӧ�Ŀ�ݿؼ�

    CCommandObj*		GetCommand()					{ return _pCommand;		}
    void                DelCommand()                    { if( _pCommand ) DelCommand(_pCommand);    }
    void                AddCommand( CCommandObj* p );

public:
    GuiComEvent         evtChange;      // ����������仯

protected:
    void				_SetSelf();
    static	void		_DragEnd(CGuiData *pSender, int x, int y, DWORD key)	{
        ((CFastCommand*)(pSender))->_DragEnd( x, y, key );
    }
    void				_DragEnd( int x, int y, DWORD key );

protected:
	CCommandObj*		_pCommand;

	typedef vector<CFastCommand*> fasts;
	static fasts		_fast;

};

// ��������һ������ʵ��
class COneCommand : public CCommandCompent
{
public:
	enum eShowStyle
	{
		enumSmall,		// ������ʾͼ�귽ʽ
		enumSale,		// ��ʾ�۳�
	};
	COneCommand(CForm& frmOwn);
	COneCommand(const COneCommand& rhs);
	COneCommand& operator=(const COneCommand& rhs);
	~COneCommand();
	GUI_CLONE(COneCommand)

	virtual void		Render();
	virtual bool		MouseRun( int x, int y, DWORD key );
	virtual void		DragRender();
	virtual eAccept		SetCommand( CCommandObj* p, int x, int y );
    virtual CCompent*   GetHintCompent( int x, int y );
    virtual void        RenderHint( int x, int y );

	CCommandObj*		GetCommand()					{ return _pCommand;		}
    void                DelCommand();

    void                AddCommand( CCommandObj* p );

    virtual UseComandEvent  GetUseCommantEvent()        { return evtUseCommand; }

	void				SetActivePic( CGuiPic* p )		{ _pActive=p;			}
	void				SetIsShowActive( bool v )		{ _IsShowActive = v;	}
	void				SetShowStyle( eShowStyle v )	{ _eShowStyle=v;		}

public:
	GuiComEvent			evtBeforeAccept;
    GuiComEvent         evtThrowItem;

    UseComandEvent      evtUseCommand;

protected:
	void				_SetSelf();
	void				_Copy( const COneCommand& rhs );
	static	void		_DragEnd(CGuiData *pSender, int x, int y, DWORD key)	{
		((COneCommand*)(pSender))->_DragEnd( x, y, key );
	}
	void				_DragEnd( int x, int y, DWORD key );

protected:
	CCommandObj*		_pCommand;
	eShowStyle			_eShowStyle;
	bool				_IsShowActive;

    CGuiPic*			_pActive;			// ����ͼ��

};

// ��������


}
