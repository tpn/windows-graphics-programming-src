//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : win.cpp						                                     //
//  Description: Generic Window Class                                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include ".\win.h"


// Default message handler for main program window, dispatch to OnKeyDown, OnDraw, etc.
LRESULT KWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_KEYDOWN:
			OnKeyDown(wParam, lParam);
			return 0;

		case WM_PAINT:
        {
            PAINTSTRUCT ps; 
                
            BeginPaint(m_hWnd, &ps);
            OnDraw(ps.hdc);
            EndPaint(m_hWnd, &ps);
        }
        return 0;

		case WM_PALETTEISCHANGING: // should not happen
			MessageBox(NULL, _T("Hello"), _T("Hi"), MB_OK);
			return 0;

		case WM_PALETTECHANGED:
			return OnPaletteChanged(hWnd, wParam);

		case WM_QUERYNEWPALETTE:
			return OnQueryNewPalette();

		case WM_DESTROY:
			if ( m_bMainWindow )
				PostQuitMessage(0); // main window only
			return 0;
	}

   return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


// Generic window procedure passed to WIN32 API, dispatches to KWindow::WndProc
LRESULT CALLBACK KWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	KWindow * pWindow;
        
	if ( uMsg==WM_NCCREATE )
	{   
		assert( ! IsBadReadPtr((void *) lParam, sizeof(CREATESTRUCT)) );
		MDICREATESTRUCT * pMDIC = (MDICREATESTRUCT *) ((LPCREATESTRUCT) lParam)->lpCreateParams;

		pWindow = (KWindow *) (pMDIC->lParam);

		assert( ! IsBadReadPtr(pWindow, sizeof(KWindow)) );
		SetWindowLong(hWnd, GWL_USERDATA, (LONG) pWindow);
	}
	else
		pWindow=(KWindow *)GetWindowLong(hWnd, GWL_USERDATA);

	if ( pWindow )
		return pWindow->WndProc(hWnd, uMsg, wParam, lParam);
	else
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


// Register WNDCLASS for the window class. Registering only once
bool KWindow::RegisterClass(LPCTSTR lpszClass, HINSTANCE hInst)
{
	WNDCLASSEX wc;

	// Check if class is already registered
	wc.cbSize = sizeof(wc);

	if ( ! GetClassInfoEx(hInst, lpszClass, &wc) )
	{
		GetWndClassEx(wc);				// fill class attributes

		wc.hInstance     = hInst;
		wc.lpszClassName = lpszClass;

		if ( !RegisterClassEx(&wc) )	// register
			return false;
	}

	return true;
}


// Handles window creation
bool KWindow::CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, LPCTSTR lpszName, DWORD dwStyle, 
    int x, int y, int nWidth, int nHeight, HWND hParent, 
    HMENU hMenu, HINSTANCE hInst)
{
	if ( ! RegisterClass(lpszClass, hInst) )
		return false;

	// Use MDICREATESTRUCT to support MDI child window
	MDICREATESTRUCT mdic;
	memset(& mdic, 0, sizeof(mdic));
	mdic.lParam = (LPARAM) this;

	m_hWnd = CreateWindowEx(dwExStyle, lpszClass, 
		lpszName, dwStyle, x, y, nWidth, nHeight, 
		hParent, hMenu, hInst, & mdic);
    
	return m_hWnd!=NULL;
}


// Fill WNDCLASSEX, virtual function
void KWindow::GetWndClassEx(WNDCLASSEX & wc)
{
	memset(& wc, 0, sizeof(wc));

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0; // CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;       
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = NULL;
	wc.hIconSm       = NULL;
}


WPARAM KWindow::MessageLoop(void)
{
	MSG msg;

	m_bMainWindow = true;

	while ( GetMessage(&msg, NULL, 0, 0) )
	{
#ifdef _DEBUG
		TCHAR temp[MAX_PATH];

		wsprintf(temp, _T("Message(0x%x, 0x%x, 0x%x, 0x%x)\n"), msg.hwnd, msg.message, msg.wParam, msg.lParam);
		OutputDebugString(temp);
#endif
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


// Common message processing for MDI Child Window
HRESULT KWindow::CommonMDIChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
									HMENU hMenu, int nWindowMenu)
{
	switch ( uMsg )
	{
		case WM_NCDESTROY:						   // should be the last message
			SetWindowLong(hWnd, GWL_USERDATA, 0);  // make sure no more message through WindowProc
			delete this;						   // MDI child are created using new operator, time to delete	
			return 0;

		case WM_MDIACTIVATE:
			if ( lParam==(LPARAM) hWnd )		   // if current window activate, switch menu
				SendMessage(GetParent(hWnd), WM_MDISETMENU, (WPARAM) hMenu, (LPARAM) GetSubMenu(hMenu, nWindowMenu));

			// send message to parent window to response to child window change
			SendMessage(GetParent(GetParent(hWnd)), WM_USER, lParam!=(LPARAM) hWnd, 0);	
			return 0;
			
		default:
			// generic MDI child window message handling provided by OS
			return DefMDIChildProc(hWnd, uMsg, wParam, lParam);
	}
}


LRESULT KWindow::OnQueryNewPalette(void)
{
	if ( m_hPalette==NULL )
		return FALSE;

	HDC      hDC = GetDC(m_hWnd);
	HPALETTE hOld= SelectPalette(hDC, m_hPalette, FALSE);

	BOOL changed = RealizePalette(hDC) != 0;
	SelectPalette(hDC, hOld, FALSE);
	ReleaseDC(m_hWnd, hDC);

	if ( changed )
	{
		OutputDebugString(_T("InvalidateRect\n"));
		InvalidateRect(m_hWnd, NULL, TRUE); // repaint
	}

	return changed;
}


LRESULT KWindow::OnPaletteChanged(HWND hWnd, WPARAM wParam)
{ 
	if ( ( hWnd != (HWND) wParam ) && m_hPalette )
	{
		HDC hDC = GetDC(hWnd);
		HPALETTE hOld = SelectPalette(hDC, m_hPalette, FALSE);
				
		if ( RealizePalette(hDC) )
			if ( m_nUpdateCount >=2 )
			{
				InvalidateRect(hWnd, NULL, TRUE);
				m_nUpdateCount = 0;
			}
			else
			{
				UpdateColors(hDC);
				m_nUpdateCount ++;
			}

		SelectPalette(hDC, hOld, FALSE);
		ReleaseDC(hWnd, hDC);
	}

	return 0;
}


int MyMessageBox(HWND hWnd, const TCHAR * text, const TCHAR * caption, DWORD style, int iconid)
{
	MSGBOXPARAMS param;

	memset(& param, 0, sizeof(param));
	param.cbSize	  = sizeof(param);
	param.hwndOwner   = hWnd;
	param.hInstance   = GetModuleHandle(NULL);
	param.lpszText    = text;
	param.lpszCaption = caption;
	param.dwStyle     = style | MB_USERICON;
	param.lpszIcon    = MAKEINTRESOURCE(iconid);

	return MessageBoxIndirect(&param);
}
