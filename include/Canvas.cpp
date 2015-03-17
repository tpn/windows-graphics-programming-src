//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : canvas.cpp						                                     //
//  Description: Generic simple child window class                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <assert.h>

#include "win.h"
#include "Canvas.h"


KCanvas::KCanvas()
{
}


KCanvas::~KCanvas()
{
}


void KCanvas::OnDraw(HDC hDC, const RECT * rcPaint)
{
//	Rectangle(hDC, 0, 0, 100, 100);
}


// called by CFrame::OnCommand
BOOL KCanvas::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return FALSE;	// not processed
}


LRESULT KCanvas::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd = hWnd;

			return 0;

		case WM_PAINT:
			{
				PAINTSTRUCT ps; 

				HDC hDC = BeginPaint(m_hWnd, &ps);

				OnDraw(hDC, & ps.rcPaint);

				EndPaint(m_hWnd, &ps);
			}
			return 0;

		case WM_COMMAND:
			if ( OnCommand(wParam, lParam) )
				return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
