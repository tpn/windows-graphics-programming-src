//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : logwindow.cpp						                                 //
//  Description: Generic Text-based Logging Window                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>

#include "win.h"
#include "logwindow.h"


void KLogWindow::Create(HINSTANCE hInst, const TCHAR * pTitle, HICON hIcon)
{
	m_hInst = hInst;
	m_hIcon = hIcon;

	CreateEx(0, _T("LOGWINDOW"), pTitle, WS_VISIBLE | WS_OVERLAPPEDWINDOW, 
		0, 0, 240, 600, NULL, NULL, hInst);
}


LRESULT KLogWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
			{
				m_hWnd	   = hWnd;

				RECT rect;
				GetClientRect(m_hWnd, & rect);

				m_hEditWnd = CreateWindow(_T("EDIT"), NULL,  
					WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 
					0, 0, rect.right, rect.bottom, m_hWnd, NULL, m_hInst, NULL);

				SendMessage(m_hEditWnd, WM_SETFONT, (LPARAM) GetStockObject(SYSTEM_FIXED_FONT), 0);
				SendMessage(m_hEditWnd, EM_LIMITTEXT, 2 * 1024 * 1024, 0);
			}

			return 0;

		case WM_SIZE:
			MoveWindow(m_hEditWnd, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);	
			return 0;

		case WM_NCDESTROY:
			delete this; // deallocate itself
			return 0;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}


void KLogWindow::Log(const char * format, ...)
{
    char buffer[1024];

    va_list ap;

	va_start(ap, format);
	vsprintf(buffer, format, ap);
	SendMessage(m_hEditWnd, EM_SETSEL, 0xFFFFFF, 0xFFFFFF);
	SendMessage(m_hEditWnd, EM_REPLACESEL, 0, (LPARAM) buffer);

	va_end(ap);
}


void KLogWindow::DumpRegion(const char * mess, HRGN hRgn, bool detail, int p1)
{
	if ( mess )
		Log(mess, p1);

	if ( hRgn==NULL )
		Log(" NULL");
	else
	{
		RECT rect;

		memset(& rect, 0, sizeof(rect));
	
		switch ( GetRgnBox(hRgn, & rect) )
		{
			case NULLREGION: 
				Log(" NULLREGION "); break;

			case SIMPLEREGION:
				Log(" SIMPLEREGION "); break;

			case COMPLEXREGION:
				Log(" COMPLEXREGION "); break;

			default:
				Log(" Error "); break;
		}

		Log(" RgnBox=[%d, %d, %d, %d) ", rect.left, rect.top, rect.right, rect.bottom);

		int size = GetRegionData(hRgn, 0, NULL);
		int rectcount = 0;

		if ( size )
		{
			RGNDATA * pRegion = (RGNDATA *) new char[size];
			GetRegionData(hRgn, size, pRegion);

			const RECT * pRect = (const RECT *) & pRegion->Buffer;
			rectcount = pRegion->rdh.nCount;

			Log("%d rectangles", rectcount);

			if ( detail )
			{
				Log("\r\n");
				for (unsigned i=0; i<pRegion->rdh.nCount; i++)
					Log("rect %d [%d, %d, %d, %d)\r\n", i, pRect[i].left, pRect[i].top, pRect[i].right, pRect[i].bottom);
			}

			delete [] (char *) pRegion;
		}
		else
			Log("0 rectangle");
	}

	Log("\r\n");
}
