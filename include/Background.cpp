//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : background.cpp						                                 //
//  Description: Use DDB as window background                                        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <assert.h>

#include "DDB.h"
#include "Background.h"

const TCHAR Prop_KBackground[] = _T("KBackground Instance");

LRESULT KBackground::EraseBackground(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT KBackground::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ( uMsg == WM_ERASEBKGND )
		return EraseBackground(hWnd, uMsg, wParam, lParam);
	else
		return CallWindowProc(m_OldProc, hWnd, uMsg, wParam, lParam);
}

LRESULT KBackground::BackGroundWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	KBackground * pThis = (KBackground *) GetProp(hWnd, Prop_KBackground);

	if ( pThis )
		return pThis->WndProc(hWnd, uMsg, wParam, lParam);
	else
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL KBackground::Attach(HWND hWnd)
{
	SetProp(hWnd, Prop_KBackground, this);
	m_OldProc = (WNDPROC) SetWindowLong(hWnd, GWL_WNDPROC, (LONG) BackGroundWindowProc);

	return m_OldProc!=NULL;
}


BOOL KBackground::Detatch(HWND hWnd)
{
	RemoveProp(hWnd, Prop_KBackground);

	if ( m_OldProc )
		return SetWindowLong(hWnd, GWL_WNDPROC, (LONG) m_OldProc) == (LONG) BackGroundWindowProc;
	else
		return FALSE;
}


LRESULT KDDBBackground::EraseBackground(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ( m_DDB.GetBitmap() )
	{
		RECT rect;
		HDC hDC = (HDC) wParam;

		GetClientRect(hWnd, & rect);
		HRGN hRgn = CreateRectRgnIndirect(&rect);

		SaveDC(hDC);
		SelectClipRgn(hDC, hRgn);
		DeleteObject(hRgn);

		m_DDB.Draw(hDC, rect.left, rect.top, rect.right - rect.left, 
			rect.bottom - rect.top, SRCCOPY, m_nStyle);
		RestoreDC(hDC, -1);

		return 1;	// processed
	}
	else
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

