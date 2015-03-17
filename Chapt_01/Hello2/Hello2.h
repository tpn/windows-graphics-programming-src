//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : hello2.h						                                     //
//  Description: Hellow World Demo 2, full screen display, Chapter 1                 //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <assert.h>

void CenterText(HDC hDC, int x, int y, LPCTSTR szFace, LPCTSTR szMessage, int point)
{
    HFONT hFont = CreateFont(- point * GetDeviceCaps(hDC, LOGPIXELSY) / 72,
							 0, 0, 0, FW_BOLD, TRUE, FALSE, FALSE, 
							 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, 
							 PROOF_QUALITY, VARIABLE_PITCH, szFace);
    assert(hFont);

    HGDIOBJ hOld = SelectObject(hDC, hFont);

    SetTextAlign(hDC, TA_CENTER | TA_BASELINE);

    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(0, 0, 0xFF));
    TextOut(hDC, x, y, szMessage, _tcslen(szMessage));

    SelectObject(hDC, hOld);
    DeleteObject(hFont);
}

const TCHAR szMessage[] = _T("Hello, World");
const TCHAR szFace[]    = _T("Times New Roman");

#pragma comment(linker, "-merge:.rdata=.text")
#pragma comment(linker, "-align:512")

extern "C" void WinMainCRTStartup()
{
    HDC hDC = GetDC(NULL);
    assert(hDC);

    CenterText(hDC, GetSystemMetrics(SM_CXSCREEN) / 2,
		    GetSystemMetrics(SM_CYSCREEN) / 2,
		    szFace, szMessage, 72);
    
    ReleaseDC(NULL, hDC);
    ExitProcess(0);
}