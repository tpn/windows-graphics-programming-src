//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : scrollcanvas.cpp					                                 //
//  Description: Scrolling window                                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>

#include "win.h"
#include "Canvas.h"
#include "ScrollCanvas.h"


// nBar should be eiter SB_HORZ or SB_VERT
void KScrollCanvas::OnScroll(int nBar, int nScrollCode, int nPos)
{
	SCROLLINFO si;
	
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask  = SIF_PAGE;
 
	GetScrollInfo(m_hWnd, nBar, &si);
	
	int nPageSize = si.nPage;
	int nDis;

	switch (nScrollCode)
    {
		case SB_LINEDOWN:             // One line right/down.
			nDis = (nBar==SB_VERT) ? m_linedy : m_linedx;
			break;

		case SB_LINEUP:               // One line left/up.
			nDis = (nBar==SB_VERT) ? -m_linedy : - m_linedx;
			break;

		case SB_PAGEDOWN:             // One page right/down.
			nDis = nPageSize;
			break;

		case SB_PAGEUP:               // One page left/up.
			nDis = - nPageSize;
			break;

		case SB_THUMBPOSITION:        // Absolute position.
			nDis = nPos - GetScrollPos(m_hWnd, nBar);
			break;

		default:                      // No change.
			nDis = 0;
			break;
    }

	if (nDis)
	{
		int nMin, nMax;

		GetScrollRange(m_hWnd, nBar, &nMin, &nMax);

		nPos = GetScrollPos(m_hWnd, nBar) + nDis;
		nPos = max(nMin, nPos);
		nPos = min(nMax-nPageSize, nPos);

		nDis = nPos - GetScrollPos(m_hWnd, nBar);

		if ( nDis )
        {
			SetScrollPos(m_hWnd, nBar, nPos, TRUE);

			if (nBar == SB_HORZ)
				ScrollWindow(m_hWnd, - nDis, 0, NULL, NULL);
			else
				ScrollWindow(m_hWnd, 0, - nDis, NULL, NULL);

		//	::UpdateWindow(m_hWnd);
        }
	}
}


void KScrollCanvas::SetScrollBar(int side, int maxsize, int pagesize)
{
	if ( pagesize < maxsize )
	{
		SCROLLINFO si;

		si.cbSize = sizeof(SCROLLINFO);
		si.fMask  = SIF_RANGE | SIF_PAGE;
		si.nMin   = 0;
		si.nMax   = maxsize - 1;
		si.nPage  = pagesize;
		si.nPos   = 0;

		::EnableScrollBar(m_hWnd, side, ESB_ENABLE_BOTH);
		::SetScrollInfo(m_hWnd, side, &si, TRUE);
	}
	else
	{
		SetScrollPos(m_hWnd, side, 0, FALSE);
		::EnableScrollBar(m_hWnd, side, ESB_DISABLE_BOTH);
	}
}


int GCD(int x, int y)
{
	while ( x!=y )
		if ( x>y ) x-=y; else y-=x;

	return x;
}


void KScrollCanvas::OnZoom(int x, int y, int mul, int div)
{
	m_zoommul *= mul;
	m_zoomdiv *= div;

	int factor = GCD(m_zoommul, m_zoomdiv);

	m_zoommul /= factor;
	m_zoomdiv /= factor;

	// add scrollbar offset
	x = ( x + GetScrollPos(m_hWnd, SB_HORZ) ) * mul / div;
	y = ( y + GetScrollPos(m_hWnd, SB_VERT) ) * mul / div;

	// update canvas
	m_width  = m_width  * mul / div;
	m_height = m_height * mul / div;

	RECT rect;

	GetClientRect(m_hWnd, & rect);

	// reset scrollbars
	SetScrollBar(SB_HORZ, m_width,  rect.right);
	SetScrollBar(SB_VERT, m_height, rect.bottom);

	// x in center the center of window if needed
	x -= rect.right/2;
	if ( x<0 ) 
		x = 0;
	if ( x > m_width - rect.right ) 
		x = m_width - rect.right;

	SetScrollPos(m_hWnd, SB_HORZ, x, FALSE);
	
	y -= rect.bottom/2;
	if ( y<0) y = 0;
	if ( y > m_height - rect.bottom ) 
		y = m_height - rect.bottom;

	SetScrollPos(m_hWnd, SB_VERT, y, FALSE);

	// repaint			
	InvalidateRect(m_hWnd, NULL, TRUE);
	::UpdateWindow(m_hWnd);
}


void KScrollCanvas::SetSize(int width, int height, int linedx, int linedy, bool resize)
{
	m_width  = width;
	m_height = height;
	m_linedx = linedx;
	m_linedy = linedy;

	if ( resize )
	{
		RECT rect;

		GetClientRect(m_hWnd, & rect);

		SetScrollBar(SB_HORZ, m_width,  rect.right);
		SetScrollBar(SB_VERT, m_height, rect.bottom);
	}
}


LRESULT KScrollCanvas::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd = hWnd;
			OnCreate();
			return 0;

		case WM_SIZE:
			SetScrollBar(SB_HORZ, m_width,  LOWORD(lParam));
			SetScrollBar(SB_VERT, m_height, HIWORD(lParam));
			return 0;

		case WM_PAINT:
			{
				PAINTSTRUCT ps; 

				HDC hDC = BeginPaint(m_hWnd, &ps);

				SetWindowOrgEx(hDC, 0, 0, NULL);
				SetViewportOrgEx(hDC, - GetScrollPos(hWnd, SB_HORZ), 
					                  - GetScrollPos(hWnd, SB_VERT), NULL);

				OnDraw(hDC, & ps.rcPaint);

				EndPaint(m_hWnd, &ps);
			}
			return 0;

		case WM_RBUTTONDOWN:
			OnZoom( LOWORD(lParam), HIWORD(lParam), 1, 2);
			return 0;

		case WM_LBUTTONDOWN:
			OnZoom( LOWORD(lParam), HIWORD(lParam), 2, 1);
			return 0;

		case WM_HSCROLL:
			OnScroll(SB_HORZ, LOWORD(wParam), HIWORD(wParam));
			return 0;

		case WM_VSCROLL:
			OnScroll(SB_VERT, LOWORD(wParam), HIWORD(wParam));
			return 0;

		case WM_TIMER:
			OnTimer(wParam, lParam);
			return 0;

		case WM_MOUSEMOVE:
			OnMouseMove(wParam, lParam);
			return 0;
		
		case WM_DESTROY:
			OnDestroy();
			return 0;

		default:
			return KCanvas::WndProc(hWnd, uMsg, wParam, lParam);
	}
}
