#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : framewnd.h						                                     //
//  Description: SDI & MDI frame windows                                             //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include <commctrl.h>

class KStatusWindow;
class KCanvas;
class KToolbar;

// SDI frame window, can have a toolbar, canvas window and a status window
class KFrame : public KWindow
{
protected:
	typedef enum { ID_STATUSWINDOW = 101,
				   ID_TOOLBAR      = 102
				};

	KToolbar        * m_pToolbar;
	KCanvas         * m_pCanvas;
	
	const TBBUTTON  * m_pButtons;
	int				  m_nButtons;

	int			      m_nToolbarHeight;
	int			  	  m_nStatusHeight;

public:
	HWND			  m_hMDIClient;
	HINSTANCE	      m_hInst;
	HACCEL			  m_hAccel;	
	KStatusWindow   * m_pStatus;

	virtual LRESULT OnCreate(void);
	virtual LRESULT OnSize(int width, int height);
	virtual BOOL	OnCommand(WPARAM wParam, LPARAM lParam);

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	KFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
		   KToolbar * pToolbar, KCanvas * pCanvas, KStatusWindow * pStatus);
	
	virtual ~KFrame();
	
	virtual WPARAM MessageLoop(void);
};


typedef enum
{
	Enum_FILE_CLOSE,
	Enum_FILE_EXIT,

	Enum_WINDOW_TILE,
	Enum_WINDOW_CASCADE,
	Enum_WINDOW_ARRANGE,
	Enum_WINDOW_CLOSEALL
};


// MDI Frame Window
class KMDIFrame : public KFrame
{
	HMENU m_hMainMenu;
	int	  m_Translate[Enum_WINDOW_CLOSEALL+1];

protected:
	virtual BOOL	OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	KMDIFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
		KToolbar * pToolbar, KStatusWindow * pStatus, const int * pTranslate) :
		KFrame(hInstance, pButtons, nCount, pToolbar, NULL, pStatus)
	{
		m_hMainMenu = NULL;

		if ( pTranslate )
			memcpy(m_Translate, pTranslate, sizeof(m_Translate));
		else
			memset(m_Translate, 0xFF, sizeof(m_Translate));
	}
};
