//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : toolbar.cpp						                                 //
//  Description: Toolbar class                                                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commctrl.h>

#include "Toolbar.h"

void KToolbar::Create(HWND hParent, HINSTANCE hInst, UINT nControlID, const TBBUTTON * pButtons, int nCount)
{
	m_ControlID = nControlID;
	m_hWnd      = CreateToolbarEx(hParent, WS_CHILD | WS_BORDER | WS_VISIBLE,
							nControlID, 	40,
							(HINSTANCE) HINST_COMMCTRL,	IDB_VIEW_LARGE_COLOR,
							pButtons, nCount, 
							24, 24, 30, 30, sizeof(TBBUTTON) );

//	SendMessage(m_hWnd, TB_SETBITMAPSIZE, 0, MAKELONG(20, 20));

	m_hToolTip  = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP,
						CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
						hParent, NULL,
						hInst, NULL);

	m_ResInstance = (HINSTANCE) HINST_COMMCTRL;
	m_ResId       = IDB_VIEW_LARGE_COLOR;

	TOOLINFO ti;

	ti.cbSize = sizeof(TOOLINFO);
	ti.hwnd   = m_hWnd;
	ti.hinst  = hInst;
	ti.uFlags = TTF_SUBCLASS;

	for (int i=0; i<nCount; i++)
		if (pButtons[i].fsStyle == TBSTYLE_BUTTON)
		{
			TCHAR Str[MAX_PATH];

			SendMessage(m_hWnd, TB_GETITEMRECT, i, (LPARAM) & ti.rect);

			LoadString(hInst, pButtons[i].dwData, Str, sizeof(Str));
			
			ti.uId      = pButtons[i].idCommand;
			ti.lpszText = Str;

			SendMessage(m_hToolTip, TTM_ADDTOOL, 0, (LPARAM) & ti);
		}

	// set the tooltip control as part of the toolbar
	SendMessage(m_hWnd, TB_SETTOOLTIPS, (WPARAM) m_hToolTip, 0);
}


void KToolbar::Resize(HWND hParent, int width, int height)
{
	RECT Self;

	GetClientRect(m_hWnd, & Self);

	MoveWindow(m_hWnd, 0, 0, width, Self.bottom - Self.top, TRUE);	
}
