//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : winpaint.cpp			                                             //
//  Description: Visualize WM_PAINT message                                          //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>

#include "..\..\include\win.h"
#include "..\..\include\Canvas.h"
#include "..\..\include\Status.h"
#include "..\..\include\FrameWnd.h"
#include "..\..\include\LogWindow.h"

#include "Resource.h"


class KMyCanvas : public KCanvas
{
	virtual void    OnDraw(HDC hDC, const RECT * rcPaint);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int				m_nRepaint;	
	
	int				m_Red, m_Green, m_Blue;
	HRGN			m_hRegion;
	KLogWindow		m_Log;
	DWORD			m_Redraw;

public:

	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	KMyCanvas()
	{
		m_nRepaint = 0;
		m_hRegion  = CreateRectRgn(0, 0, 1, 1);
		
		m_Red	   = 0x4F;
		m_Green    = 0x8F;
		m_Blue     = 0xCF;
		m_Redraw   = 0;
	}
};


BOOL KMyCanvas::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch ( LOWORD(wParam) )
	{
		case IDM_VIEW_HREDRAW:
		case IDM_VIEW_VREDRAW:
			{
				HMENU hMenu = GetMenu(GetParent(m_hWnd));

				MENUITEMINFO mii;
				
				memset(& mii, 0, sizeof(mii));
				mii.cbSize = sizeof(mii);
				mii.fMask  = MIIM_STATE;
				
				if ( GetMenuState(hMenu, LOWORD(wParam), MF_BYCOMMAND) & MF_CHECKED )
					mii.fState = MF_UNCHECKED;
				else
					mii.fState = MF_CHECKED;
				SetMenuItemInfo(hMenu, LOWORD(wParam), FALSE, & mii);
				
				if ( LOWORD(wParam)==IDM_VIEW_HREDRAW )
					m_Redraw ^= WVR_HREDRAW;
				else
					m_Redraw ^= WVR_VREDRAW;
			}
			return TRUE;

		case IDM_FILE_EXIT:
			DestroyWindow(GetParent(m_hWnd));
			return TRUE;
	}

	return FALSE;	// not processed
}


LRESULT KMyCanvas::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr;

	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd = hWnd;
			m_Log.Create(m_hInst, _T("WinPaint"), LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH)));
			m_Log.Log(_T("WM_CREATE\r\n"));
			return 0;

		case WM_NCCALCSIZE:
			m_Log.Log(_T("WM_NCCALCSIZE\r\n"));
			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
			m_Log.Log("WM_NCCALCSIZE returns %x\r\n", lr);

			if ( wParam )
			{
				lr &= ~ (WVR_HREDRAW | WVR_VREDRAW);
				lr |= m_Redraw;
			}
			break;

		case WM_NCPAINT:
			m_Log.Log("WM_NCPAINT HRGN %0x\r\n", (HRGN) wParam);
			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
			m_Log.Log("WN_NCPAINT returns\r\n");
			break;

		case WM_ERASEBKGND:
			m_Log.Log("WM_ERASEBKGND HDC %0x\r\n", (HDC) wParam);
			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
			m_Log.Log("WM_ERASEBKGND returns\r\n");
			break;

		case WM_SIZE:
			m_Log.Log("WM_SIZE type %d, width %d, height %d\r\n", wParam, LOWORD(lParam), HIWORD(lParam));
			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
			m_Log.Log("WM_SIZE returns\r\n");
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps; 

				m_Log.Log("WM_PAINT\r\n");
				m_Log.Log("BeginPaint\r\n");
				HDC hDC = BeginPaint(m_hWnd, &ps);
				m_Log.Log("BeginPaint returns HDC %8x\r\n", hDC);

				OnDraw(hDC, &ps.rcPaint);

				m_Log.Log("EndPaint\r\n");
				EndPaint(m_hWnd, &ps);
				m_Log.Log("EndPaint returns GetObjectType(%08x)=%d\r\n", hDC, GetObjectType(hDC));
				m_Log.Log("WM_PAINT returns\r\n");
			}
			return 0;

		default:
			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return lr;
}


void KMyCanvas::OnDraw(HDC hDC, const RECT * rcPaint)
{
	RECT rect;

	GetClientRect(m_hWnd, & rect);
	
	GetRandomRgn(hDC, m_hRegion, SYSRGN);
	
	POINT Origin;
	GetDCOrgEx(hDC, & Origin);

	if ( ((unsigned) hDC) & 0xFFFF0000 )
		OffsetRgn(m_hRegion, - Origin.x, - Origin.y);

	m_nRepaint ++;

	TCHAR mess[64];

	wsprintf(mess, _T("HDC 0x%X, Org(%d, %d)"), hDC, Origin.x, Origin.y); 
	if ( m_pStatus )
		m_pStatus->SetText(pane_1, mess);

	switch ( m_nRepaint % 3 )
	{
		case 0: m_Red  = (m_Red   + 0x31) & 0xFF; break;
		case 1: m_Green= (m_Green + 0x31) & 0xFF; break;
		case 2: m_Blue = (m_Blue  + 0x31) & 0xFF; break;
	}

	SetTextAlign(hDC, TA_TOP | TA_CENTER);

	int size = GetRegionData(m_hRegion, 0, NULL);
	int rectcount = 0;

	if ( size )
	{
		RGNDATA * pRegion = (RGNDATA *) new char[size];
		GetRegionData(m_hRegion, size, pRegion);

		const RECT * pRect = (const RECT *) & pRegion->Buffer;
		rectcount = pRegion->rdh.nCount;

		TEXTMETRIC tm;
		GetTextMetrics(hDC, & tm);
		int lineheight = tm.tmHeight + tm.tmExternalLeading; 

		for (unsigned i=0; i<pRegion->rdh.nCount; i++)
		{
			int x = (pRect[i].left + pRect[i].right)/2;
			int y = (pRect[i].top + pRect[i].bottom)/2;

			wsprintf(mess, "WM_PAINT %d, rect %d", m_nRepaint, i+1);
			::TextOut(hDC, x, y - lineheight, mess, _tcslen(mess));

			wsprintf(mess, "(%d, %d, %d, %d)", pRect[i].left, pRect[i].top, pRect[i].right, pRect[i].bottom);
			::TextOut(hDC, x, y, mess, _tcslen(mess));

		}

		delete [] (char *) pRegion;
	}

	wsprintf(mess, _T("WM_PAINT message %d, %d rects in sysrgn"), m_nRepaint, rectcount);
	if ( m_pStatus )
		m_pStatus->SetText(pane_2, mess);


	HBRUSH hBrush = CreateSolidBrush(RGB(m_Red, m_Green, m_Blue));

	FrameRgn(hDC, m_hRegion, hBrush, 4, 4);
	FrameRgn(hDC, m_hRegion, (HBRUSH) GetStockObject(WHITE_BRUSH), 1, 1);

	DeleteObject(hBrush);
}


class KMyFrame : public KFrame
{
	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KFrame::GetWndClassEx(wc);
		wc.hIcon  = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH));
	}

public:
	KMyFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
			KToolbar * pToolbar, KCanvas * pCanvas, KStatusWindow * pStatus) :
		KFrame(hInstance, pButtons, nCount, pToolbar, pCanvas, pStatus)
	{
	}

};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KMyCanvas     canvas;
	KStatusWindow status;

	KMyFrame frame(hInst, NULL, 0, NULL, & canvas, & status);

	frame.CreateEx(0, _T("ClassName"), _T("WinPaint"),
		WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}
