//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : test.cpp							                                 //
//  Description: Sample EMF decompiled as C++ program                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "resource.h"

HINSTANCE hModule;

int GetDIBColorCount(const BITMAPINFOHEADER & bmih)
{
    if ( bmih.biBitCount <= 8 )
        if ( bmih.biClrUsed )
            return bmih.biClrUsed;
        else
            return 1 << bmih.biBitCount;
    else if ( bmih.biCompression==BI_BITFIELDS )
        return 3 + bmih.biClrUsed;
    else
        return bmih.biClrUsed;
}

class KDIB
{
    const BITMAPINFO * m_pDIB;
    const void       * m_pBits;

public:
    KDIB()
    {
        m_pDIB = NULL;
    }

    void Load(int resid)
    {
        if ( m_pDIB )
            return;

        HRSRC hRsc = FindResource(hModule, MAKEINTRESOURCE(resid), RT_BITMAP);
        
        if ( hRsc )
        {
            m_pDIB  = (const BITMAPINFO *) LockResource(LoadResource(hModule, hRsc));
            m_pBits =  & m_pDIB->bmiColors[GetDIBColorCount(m_pDIB->bmiHeader)];
        }
    }

    int StretchDIBits(HDC hDC, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh, int coloruse, DWORD rop)
    {
        return ::StretchDIBits(hDC, dx, dy, dw, dh, sx, sy, sw, sh, 
                    m_pBits, m_pDIB, coloruse, rop);
    }
   
};


void OnDraw(HDC hDC)
{
    HRGN hRegion = NULL;

	HGDIOBJ hObj[1] = { NULL };
	
	// Header
	
	static KDIB Dib_1; Dib_1.Load(IDB_BITMAP1);// 350x250x8
	Dib_1.StretchDIBits(hDC, 10,10,350,250, 0,0,350,250, DIB_RGB_COLORS, SRCCOPY);
	
	Dib_1.StretchDIBits(hDC, 10,270,350,250, 0,0,350,250, DIB_RGB_COLORS, SRCCOPY);
	
	static KDIB Dib_2; Dib_2.Load(IDB_BITMAP2);// 350x129x8
	Dib_2.StretchDIBits(hDC, 370,10,350,125, 0,4,350,125, DIB_RGB_COLORS, SRCCOPY);
	
	static KDIB Dib_3; Dib_3.Load(IDB_BITMAP3);// 350x129x8
	Dib_3.StretchDIBits(hDC, 370,136,350,125, 0,0,350,125, DIB_RGB_COLORS, SRCCOPY);
	
	Dib_1.StretchDIBits(hDC, 370,270,175,125, 0,125,175,125, DIB_RGB_COLORS, SRCCOPY);
	
	Dib_1.StretchDIBits(hDC, 370,396,175,125, 0,0,175,125, DIB_RGB_COLORS, SRCCOPY);
	
	Dib_1.StretchDIBits(hDC, 546,270,175,125, 175,125,175,125, DIB_RGB_COLORS, SRCCOPY);
	
	Dib_1.StretchDIBits(hDC, 546,396,175,125, 175,0,175,125, DIB_RGB_COLORS, SRCCOPY);
	// EMREOF(0, 16, 20)
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch ( uMsg )
    {
        case WM_PAINT:
            {
                PAINTSTRUCT ps;

                HDC hDC = BeginPaint(hWnd, & ps);
                OnDraw(hDC);
                EndPaint(hWnd, & ps);

                return 0;
            }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, int nCmdShow)
{
    WNDCLASSEX wc;

    memset(& wc, 0, sizeof(wc));
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
    wc.lpszClassName = "EMFDC";

    if ( ! RegisterClassEx(& wc) )
        return -1;

    hModule = hInst;

    HWND hWnd = CreateWindowEx(0, "EMFDC", "Decompiled EMF", WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                    NULL, NULL, hInst, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;

    while ( GetMessage(&msg, NULL, 0, 0) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
