//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : hello4.cpp						                                     //
//  Description: Hello World Demo 4, DirectDraw, Chapter 1                           //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <ddraw.h>

#include "..\..\include\win.h"

const TCHAR szMessage[] = _T("Hello, World !");
const TCHAR szFace[]    = _T("Times New Roman");
const TCHAR szHint[]    = _T("Press ESC to quit.");
const TCHAR szProgram[] = _T("HelloWorld4");


void CenterText(HDC hDC, int x, int y, LPCTSTR szFace, 
                LPCTSTR szMessage, int point)
{
    HFONT hFont = CreateFont(
        - point * GetDeviceCaps(hDC, LOGPIXELSY) / 72,
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


class KDDrawWindow : public KWindow
{
    LPDIRECTDRAW        lpdd;
    LPDIRECTDRAWSURFACE lpddsprimary;
    
    void OnKeyDown(WPARAM wParam, LPARAM lParam)
    {
        if ( wParam==VK_ESCAPE )
            PostMessage(m_hWnd, WM_CLOSE, 0, 0);
    }

    void Blend(int left, int right, int top, int bottom);

    void OnDraw(HDC hDC)
    {
        TextOut(hDC, 0, 0, szHint, lstrlen(szHint));
        CenterText(hDC, GetSystemMetrics(SM_CXSCREEN)/2,
	    GetSystemMetrics(SM_CYSCREEN)/2,
	    szFace, szMessage, 48);

        Blend(80, 560, 160, 250);
    }

public:
    KDDrawWindow(void)
    {
        lpdd         = NULL;
        lpddsprimary = NULL;
    }

    ~KDDrawWindow(void)
    {
        if ( lpddsprimary )
        {
            lpddsprimary->Release();
            lpddsprimary = NULL;
        }

        if ( lpdd )
        {
            lpdd->Release();
            lpdd = NULL;
        }
    }

    bool CreateSurface(void);
};


bool KDDrawWindow::CreateSurface(void)
{
    HRESULT hr;

    hr = DirectDrawCreate(NULL, &lpdd, NULL);
    if (hr!=DD_OK) 
        return false;

    hr = lpdd->SetCooperativeLevel(m_hWnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE);
    if (hr!=DD_OK) 
        return false;

    hr = lpdd->SetDisplayMode(640, 480, 32);
    if (hr!=DD_OK) 
        return false;

    DDSURFACEDESC ddsd;
    memset(& ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    return lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL)==DD_OK;
}


void inline Blend(unsigned char *dest, unsigned char *src)
{
    dest[0] = (dest[0] + src[0])/2;
    dest[1] = (dest[1] + src[1])/2;
    dest[2] = (dest[2] + src[2])/2; 
}


void KDDrawWindow::Blend(int left, int right, 
                         int top, int bottom)
{
    DDSURFACEDESC ddsd;

    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    HRESULT hr = lpddsprimary->Lock(NULL, &ddsd, 
        DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
    assert(hr==DD_OK);

    unsigned char *screen = (unsigned char *) ddsd.lpSurface;

    for (int y=top; y<bottom; y++)
    {
        unsigned char * pixel = screen + y * ddsd.lPitch 
           + left * 4;

        for (int x=left; x<right; x++, pixel+=4)
            if ( pixel[0]!=255 || pixel[1]!=255 || 
               pixel[2]!=255 ) // non white
            {
                ::Blend(pixel-4, pixel);           // left
                ::Blend(pixel+4, pixel);           // right

                ::Blend(pixel-ddsd.lPitch, pixel); // up
                ::Blend(pixel+ddsd.lPitch, pixel); // down
            }
    }

    lpddsprimary->Unlock(ddsd.lpSurface);
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, 
                   LPSTR lpCmd, int nShow)
{
    KDDrawWindow win;

    win.CreateEx(0, szProgram, szProgram,
	             WS_POPUP,
	             0, 0,
	             GetSystemMetrics( SM_CXSCREEN ),
	             GetSystemMetrics( SM_CYSCREEN ),
	             NULL, NULL, hInst);

    if ( ! win.CreateSurface() )
	{
		MessageBox(NULL, _T("DirectDraw Initialization failed!"), _T("Hello4"), MB_OK);
		return -1;
	}

    win.ShowWindow(nShow);
    win.UpdateWindow();

    return win.MessageLoop();
}