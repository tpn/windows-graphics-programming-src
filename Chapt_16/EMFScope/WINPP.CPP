//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : winpp.cpp							                                 //
//  Description: Another C++ window class                                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <assert.h>

#include "Winpp.h"


int mapx(int x)
{
	return LOWORD(GetDialogBaseUnits()) * x / 4;
}


int mapy(int y)
{
	return HIWORD(GetDialogBaseUnits()) * y / 8;
}


int unmapx(int x)
{
	return x * 4 / LOWORD(GetDialogBaseUnits());
}


int unmapy(int y)
{
	return y * 8 / HIWORD(GetDialogBaseUnits());
}


const char *LoadStringTemp(int id)
{
	static char buffer[256];

	if (!LoadString(0, id, buffer, sizeof(buffer)-2))
		buffer[0] = 0;

	return buffer;
}


HWND KWindow::Createwindow(LPCTSTR   lpClassName,
                           int       id_icon,
                           int       id_cursor,

                           LPCTSTR   lpWindowName,	
                           DWORD     dwStyle,	
                           int       x, 
                           int       y, 
                           int       w, 
                           int       h,
                           HWND      hWndParent,	
                           HINSTANCE hInstance,
                           HMENU     hMenu,
                           DWORD     dwExStyle,
                           HBRUSH    hBrush)
{
    char ClassName[MAX_PATH];
    HWND Sibling;

    wsprintf(ClassName, "%s_%08lx", lpClassName, hInstance);

    Sibling = FindWindow(ClassName, NULL);

    HCURSOR hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(id_cursor));
    if (hCursor==NULL)
        hCursor = LoadCursor(NULL, IDC_ARROW);

    HICON   hIcon   = LoadIcon(hInstance, MAKEINTRESOURCE(id_icon));
    
    if (Sibling != NULL)
    {
        SetClassLong(Sibling, GCL_WNDPROC, (DWORD) Thunk);
        SetClassLong(Sibling, GCL_HCURSOR, (DWORD) hCursor);
        SetClassLong(Sibling, GCL_HICON,   (DWORD) hIcon);
    }
    else
    {
        WNDCLASS wc;

        if (GetClassInfo(hInstance, ClassName, &wc))
            UnregisterClass(ClassName, hInstance);
        
        wc.style          = CS_HREDRAW | CS_VREDRAW;
	    wc.lpfnWndProc    = (WNDPROC) (void *) Thunk;
	    wc.cbClsExtra     = 0;
	    wc.cbWndExtra     = 0;       
	    wc.hInstance      = hInstance;
	    wc.hIcon          = hIcon;
	    wc.hCursor        = hCursor;
	    wc.hbrBackground  = hBrush; 
	    wc.lpszMenuName   = NULL;
	    wc.lpszClassName  = ClassName;

        if (!RegisterClass(&wc))
            return NULL;
    }

    m_hWnd = CreateWindowEx(dwExStyle, 
                            ClassName,
			                lpWindowName,
				            dwStyle, 
				            x, y, w, h,
				            hWndParent, 
                            hMenu, 
				            hInstance,
				            NULL);

    return m_hWnd;

}


int KWindow::MessageLoop(int nCmdShow)
{	
	MSG  msg;
	
	ShowWindow(m_hWnd, nCmdShow);
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

#define SCROLL_RATIO   5

void KWindow::Scroll (int message, WORD wPos, WORD wScrollType)
{
    int  xBar;                       // Where scrollbar is now.
    int  nMin;                       // Minumum scroll bar value.
    int  nMax;                       // Maximum scroll bar value.
    int  dx;                         // How much to move.
    int  nOneUnit;                   // # of pixels for LINEUP/LINEDOWN
    int  cxClient;                   // Width of client area.
    int  nHorzOrVert;                // Doing the horizontal or vertical?
    RECT rect;                       // Client area.

	GetClientRect (m_hWnd, &rect);

	if (message == WM_HSCROLL)
	{
		nHorzOrVert = SB_HORZ;
		cxClient    = rect.right - rect.left;
    }
	else
    {
	    nHorzOrVert = SB_VERT;
		cxClient    = rect.bottom - rect.top;
    }

	// One a SB_LINEUP/SB_LINEDOWN we will move the DIB by
    //  1/SCROLL_RATIO of the client area (i.e. if SCROLL_RATIO
    //  is 4, it will scroll the DIB a quarter of the client
    //  area's height or width.

	nOneUnit = cxClient / SCROLL_RATIO;
	if (!nOneUnit)
		nOneUnit = 1;

	xBar = GetScrollPos (m_hWnd, nHorzOrVert);
	GetScrollRange (m_hWnd, nHorzOrVert, &nMin, &nMax);

	switch (wScrollType)
    {
		case SB_LINEDOWN:             // One line right.
			dx = nOneUnit;
			break;

		case SB_LINEUP:               // One line left.
			dx = -nOneUnit;
			break;

		case SB_PAGEDOWN:             // One page right.
			dx = cxClient;
			break;

		case SB_PAGEUP:               // One page left.
			dx = -cxClient;
			break;

		case SB_THUMBPOSITION:        // Absolute position.
			dx = wPos - xBar;
			break;

		default:                      // No change.
			dx = 0;
			break;
    }

	if (dx)
	{
		xBar += dx;

		if (xBar < nMin)
        {
			dx  -= xBar - nMin;
			xBar = nMin;
        }

		if (xBar > nMax)
        {
			dx  -= xBar - nMax;
			xBar = nMax;
        }

		if (dx)
        {
			SetScrollPos (m_hWnd, nHorzOrVert, xBar, TRUE);

			if (nHorzOrVert == SB_HORZ)
				ScrollWindow (m_hWnd, -dx, 0, NULL, NULL);
			else
				ScrollWindow (m_hWnd, 0, -dx, NULL, NULL);

			UpdateWindow (m_hWnd);
        }
	}
}



