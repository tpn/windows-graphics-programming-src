//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : framewnd.cpp						                                 //
//  Description: SDI and MDI frame window classes                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <assert.h>

#include "win.h"
#include "Status.h"
#include "Canvas.h"
#include "Toolbar.h"

#include "FrameWnd.h"


KFrame::KFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
			   KToolbar * pToolbar, KCanvas * pCanvas, KStatusWindow * pStatus)
{
	m_hInst		 = hInstance;
	m_pStatus    = NULL;
	m_pCanvas    = NULL;
	m_pToolbar	 = NULL;

	m_pButtons	 = pButtons;
	m_nButtons	 = nCount;

	m_pToolbar	 = pToolbar;
	m_pCanvas	 = pCanvas;
	m_pStatus	 = pStatus;

	m_hMDIClient = NULL;
	m_hAccel	 = NULL;
}


KFrame::~KFrame()
{
}


// default implementation: create a canvas window and a status window
LRESULT KFrame::OnCreate(void)
{
	RECT rect;

	// toolbar window is at the top of client area
	if ( m_pToolbar )
	{
		m_pToolbar->Create(m_hWnd, m_hInst, ID_STATUSWINDOW, m_pButtons, m_nButtons);
		GetWindowRect(m_pToolbar->m_hWnd,  & rect);

		m_nToolbarHeight = 37; // rect.bottom - rect.top;
	}
	else
		m_nToolbarHeight = 0;
	
	// status window is at the bottom of client area
	if ( m_pStatus )
	{
		m_pStatus->Create(m_hWnd, ID_STATUSWINDOW);
		GetWindowRect(m_pStatus->m_hWnd,  & rect);

		m_nStatusHeight = rect.bottom - rect.top;
	}
	else
		m_nStatusHeight = 0;
	
	// create a canvas window above the status window
	if ( m_pCanvas )
	{
		GetClientRect(m_hWnd, & rect);

		m_pCanvas->SetStatus(m_hInst, m_pStatus);

		m_pCanvas->CreateEx(0, _T("Canvas Class"), NULL, WS_VISIBLE | WS_CHILD,
					0, m_nToolbarHeight, rect.right, rect.bottom - m_nToolbarHeight - m_nStatusHeight, 
					m_hWnd, NULL, m_hInst);
	}

	return 0;
}


LRESULT KFrame::OnSize(int width, int height)
{
	// need to tell status window about resizing
	if ( m_pToolbar )
		m_pToolbar->Resize(m_hWnd, width, height);

	if ( m_pStatus )
		m_pStatus->Resize(m_hWnd, width, height);
	
	// resize canvas window
	if ( m_pCanvas )
		MoveWindow(m_pCanvas->m_hWnd, 
			   0, m_nToolbarHeight, width, height - m_nToolbarHeight - m_nStatusHeight, TRUE);

	if ( m_hMDIClient )
		MoveWindow(m_hMDIClient, 
			   0, m_nToolbarHeight, width, height - m_nToolbarHeight - m_nStatusHeight, TRUE);

	return 0;
}


BOOL KFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// forward message to canvas window
	if ( m_pCanvas && m_pCanvas->OnCommand(wParam, lParam) )
		return TRUE;

	return FALSE;	// not processed
}


LRESULT KFrame::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
		    m_hWnd = hWnd;

			return OnCreate();

		case WM_SIZE:
			return OnSize(LOWORD(lParam), HIWORD(lParam));

		case WM_COMMAND:
			if ( OnCommand(wParam, lParam) )
				return 0;
			break;

		case WM_DESTROY:
			PostQuitMessage(0);		// this is the only main window, destroying window means quit program
			return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


WPARAM KFrame::MessageLoop(void)
{
	MSG msg;

	while ( GetMessage(&msg, NULL, 0, 0) )
		if ( ! TranslateMDISysAccel(m_hMDIClient, &msg) &&
			 ! TranslateAccelerator(m_hWnd, m_hAccel, & msg) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	return msg.wParam;
}


///////////////////////// KMDIFrame

BOOL CALLBACK CloseEnumProc(HWND hWnd, LPARAM lParam)
{
	if ( GetWindow(hWnd, GW_OWNER) )
		return TRUE;

	SendMessage(GetParent(hWnd), WM_MDIRESTORE, (WPARAM) hWnd, 0);

	if ( ! SendMessage(hWnd, WM_QUERYENDSESSION, 0, 0) )
		return TRUE;

	SendMessage(GetParent(hWnd), WM_MDIDESTROY, (WPARAM) hWnd, 0);

	return TRUE;
}


BOOL KMDIFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return FALSE; // not processed
}


LRESULT KMDIFrame::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
		    m_hWnd		= hWnd;
			m_hMainMenu = GetMenu(hWnd);
				
			OnCreate();	// toolbar & status

			CLIENTCREATESTRUCT client;
			client.hWindowMenu  = GetSubMenu(m_hMainMenu, 1);
			client.idFirstChild = 1000;

			m_hMDIClient = CreateWindow(_T("MDICLIENT"), NULL,
				WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
				0, 0, 0, 0, hWnd, (HMENU) 1, m_hInst, & client);

			return 0;
		}
		break;

		case WM_COMMAND:
			if ( OnCommand(wParam, lParam) ) // allow overridding
				return 0;

			if ( LOWORD(wParam)==m_Translate[Enum_WINDOW_TILE] )
			{
				SendMessage(m_hMDIClient, WM_MDITILE, 0, 0);
				return 0;
			}
			else if ( LOWORD(wParam)==m_Translate[Enum_WINDOW_CASCADE] )
			{
				SendMessage(m_hMDIClient, WM_MDICASCADE, 0, 0);
				return 0;
			}
			else if ( LOWORD(wParam)==m_Translate[Enum_WINDOW_ARRANGE] )
			{
				SendMessage(m_hMDIClient, WM_MDIICONARRANGE, 0, 0);
				return 0;
			}
			else if ( LOWORD(wParam)==m_Translate[Enum_WINDOW_CLOSEALL] )
			{
				EnumChildWindows(m_hMDIClient, CloseEnumProc, 0);
				return 0;
			}
			else if ( LOWORD(wParam)==m_Translate[Enum_FILE_CLOSE] )
			{
				HWND h = (HWND) SendMessage(m_hMDIClient, WM_MDIGETACTIVE, 0, 0);

				if ( SendMessage(h, WM_QUERYENDSESSION, 0, 0) )
					SendMessage(m_hMDIClient, WM_MDIDESTROY, (WPARAM) h, 0);

				return 0;
			}
			else if ( LOWORD(wParam)==m_Translate[Enum_FILE_EXIT] )
			{
				SendMessage(hWnd, WM_CLOSE, 0, 0);
				return 0;
			}
			else
			{
				HWND h = (HWND) SendMessage(m_hMDIClient, WM_MDIGETACTIVE, 0, 0);

				if ( h )
					SendMessage(h, WM_COMMAND, wParam, lParam);
			}
			break;

		// forward message to active child window

		case WM_QUERYNEWPALETTE:
		{
			HWND h = (HWND) SendMessage(m_hMDIClient, WM_MDIGETACTIVE, 0, 0);

			if ( h )
				SendMessage(h, uMsg, wParam, lParam);
		}
		break;

		case WM_SIZE:
			return OnSize(LOWORD(lParam), HIWORD(lParam));

		case WM_QUERYENDSESSION:
		case WM_CLOSE:
			SendMessage(hWnd, WM_COMMAND, m_Translate[Enum_WINDOW_CLOSEALL], 0); // Close all child window
				
			if ( GetWindow(m_hMDIClient, GW_CHILD) )			   // stay if child winodow staying	
				return 0;
			break;

		case WM_DESTROY:
			PostQuitMessage(0);		// this is the only main window, destroying window means quit program
			return 0;

		case WM_USER:
			if ( wParam )
				SendMessage(m_hMDIClient, WM_MDISETMENU, (WPARAM) m_hMainMenu, (LPARAM) GetSubMenu(m_hMainMenu, 1));

			DrawMenuBar(hWnd);
			return 0;
	}	

	return DefFrameProc(hWnd, m_hMDIClient, uMsg, wParam, lParam);
}

