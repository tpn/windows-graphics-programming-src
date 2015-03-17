//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : gdispeed.cpp				                                         //
//  Description: GDI speed test demo, Chapter 1                                      //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>

#include "..\..\include\timer.h"
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


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmd, int nShow)
{
    KTimer timer;
    TCHAR  mess[128];

    timer.Start(); 
	Sleep(1000); 
    unsigned cpuspeed10 = (unsigned)(timer.Stop()/100000);
    
    timer.Start(); 
    CreateSolidBrush(RGB(0xAA, 0xAA, 0xAA));
    unsigned time = (unsigned) timer.Stop();

    wsprintf(mess, _T("CPU speed       %d.%d mhz\n")
        _T("KTimer overhead %d clock cycles\n")
        _T("CreateSolidBrush %d clock cycles %d ns"),
        cpuspeed10 / 10,  cpuspeed10 % 10,
        (unsigned) timer.m_overhead,
        time, time * 10000 / cpuspeed10);
        
    MyMessageBox(NULL, mess, _T("How fast is GDI?"), MB_OK);

    return 0;
}
