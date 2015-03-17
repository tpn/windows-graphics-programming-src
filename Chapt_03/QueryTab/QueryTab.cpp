//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : querytab.cpp					                                     //
//  Description: Query GDI handle table demo, Chapter 3                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
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

typedef unsigned (CALLBACK * Proc0) (void);


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    Proc0 p = (Proc0) GetProcAddress(GetModuleHandle("GDI32.DLL"), "GdiQueryTable");

    if (p)
    {
        TCHAR temp[32];

        wsprintf(temp, "%8lX", p());
        MyMessageBox(NULL, temp, "GdiQueryTable() returns", MB_OK);
    }

    return 0;
}
