//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : windowdc.cpp			                                             //
//  Description: Window/Client DC Demo, Chapter 5	                                 //
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


void Test(HWND hWnd, const TCHAR * mess)
{
	SetWindowText(hWnd, mess);
	
	HDC hDC1 = GetWindowDC(hWnd);
	HDC hDC2 = GetDC(hWnd);

	TCHAR temp[MAX_PATH];

	wsprintf(temp, "%s GetWindowDC=%x, GetDC=%x", mess, hDC1, hDC2);

	MyMessageBox(NULL, temp, _T("Window Region"), MB_OK);

	ReleaseDC(hWnd, hDC1);
	ReleaseDC(hWnd, hDC2);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	HDC hDC1 = CreateIC(_T("DISPLAY"), NULL, NULL, NULL);
	HDC hDC2 = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

	BOOL r = Rectangle(hDC1, 10, 10, 20, 200);

	int rs = GetLastError();

	r = Rectangle(hDC2, 10, 10, 20, 200);

	rs = GetLastError();

	HWND hWnd = CreateWindow(_T("EDIT"), NULL, 
					WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 
					10, 10, 200, 100, GetDesktopWindow(), 
					NULL, hInstance, NULL);
	
	HWND hWnd1 = CreateWindow(_T("EDIT"), NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,
					5, 5, 20, 20, hWnd, NULL, hInstance, NULL);

	ShowWindow(hWnd, SW_SHOW);
	
	Test(hWnd, _T("Rectangular Window"));

	HRGN hRgn = CreateEllipticRgn(0, 0, 300, 300);

	SetWindowRgn(hWnd, hRgn, TRUE);

	Test(hWnd, _T("Elliptic Window"));
	
	DestroyWindow(hWnd);

	DeleteDC(hDC1);
	DeleteDC(hDC2);
	
	return 0;
}