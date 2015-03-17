//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : ellipse.cpp			                                             //
//  Description: Simple non-rectangular window demo, Chapter 5                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>

#include "resource.h"

int MyMessageBox(HWND hWnd, const TCHAR * text, const TCHAR * caption, DWORD style)
{
	MSGBOXPARAMS param;

	memset(& param, 0, sizeof(param));
	param.cbSize	  = sizeof(param);
	param.hwndOwner   = hWnd;
	param.hInstance   = GetModuleHandle(NULL);
	param.lpszText    = text;
	param.lpszCaption = caption;
	param.dwStyle     = style | MB_USERICON;
	param.lpszIcon    = MAKEINTRESOURCE(IDI_GRAPH);

	return MessageBoxIndirect(&param);
}


const TCHAR szProgram [] = _T("Window Region");
const TCHAR szRectWin [] = _T("Rectangular Window");
const TCHAR szEptcWin [] = _T("Elliptic Window");


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    HWND hWnd = CreateWindow(_T("EDIT"), NULL, WS_OVERLAPPEDWINDOW,
		10, 10, 200, 100, GetDesktopWindow(), NULL, hInst, NULL);
	ShowWindow(hWnd, SW_SHOW);

	SetWindowText(hWnd, szRectWin);

	MyMessageBox(NULL, szRectWin, szProgram, MB_OK);

	HRGN hRgn = CreateEllipticRgn(0, 0, 200, 100);
	SetWindowRgn(hWnd, hRgn, TRUE);
	DeleteObject(hRgn);

	SetWindowText(hWnd, szEptcWin);
	MyMessageBox(NULL, szEptcWin, szProgram, MB_OK);

	DestroyWindow(hWnd);

	return 0;
}

