//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : mandelbrot.cpp					                                     //
//  Description: Mandelbrot Set demo, Chapter 7                                      //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <stdio.h>
#include <math.h>

#include "..\..\include\win.h"
#include "..\..\include\Status.h"
#include "..\..\include\Canvas.h"
#include "..\..\include\ScrollCanvas.h"
#include "..\..\include\FrameWnd.h"
#include "..\..\include\GDIObject.h"
#include "..\..\include\color.h"

#include "resource.h"

class KMyCanvas : public KScrollCanvas
{
	typedef enum { 
					MAX_LIMIT  = 16384,
					MAX_HEIGHT = 16384,
	};
	
	virtual void    OnZoom(int x, int y, int mul, int div);
	virtual BOOL	OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void    OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual void    OnDraw(HDC hDC, const RECT * rcPaint);
	virtual	void	OnTimer(WPARAM wParam, LPARAM lParam);

	virtual void OnCreate(void)
	{
		m_hTimer = ::SetTimer(m_hWnd, 111, 100, NULL);
	}

	virtual void OnDestroy(void)
	{
		KillTimer(m_hWnd, m_hTimer);
	}


	int MandelCount(HDC hDC, int xi, int yi, int limit);

	double m_x, m_y, m_unit;
	int    m_hTimer;
	int    m_limit, m_lastlimit;
	int    m_lastwidth, m_lastheight;
	int    m_lastx0, m_lasty0;

	short * Buffer[MAX_HEIGHT];

	COLORREF Out[MAX_LIMIT];
	COLORREF In [MAX_LIMIT];

	void SetLimit(int limit, BOOL bRedraw);
	void ClearBuffer(int width, int height, int x, int y);

public:

	KMyCanvas()
	{
		m_x			 = (double) - 2.1;
		m_y			 = (double) - 1.375;
		m_unit		 = (double) 1.0 / 256;
		m_lastwidth  = 0;
		m_lastheight = 0;
				
		memset(Buffer, 0, sizeof(Buffer));
		SetSize((int) (3 * 256), (int) (2.75 * 256), 24, 24);
		SetLimit(32, FALSE);
	}

	
	~KMyCanvas()
	{
		ClearBuffer(0, 0, 0, 0);
	}

};


void KMyCanvas::ClearBuffer(int width, int height, int x0, int y0)
{
	if ( (width!=m_lastwidth) || (height!=m_lastheight) )	
	{
		for (int i=0; i<sizeof(Buffer)/sizeof(Buffer[0]); i++)
			if ( Buffer[i] )
			{
				delete [] Buffer[i];

				Buffer[i] = NULL;
			}

		for (i=0; i<height; i++)
		{
			Buffer[i] = new short[width];

			memset(Buffer[i], 0, width * sizeof(short));
		}

		m_lastwidth  = width;
		m_lastheight = height;
		m_lastx0     = x0;
		m_lasty0     = y0;
	}

	if ( y0 > m_lasty0 )	// move up
	{
		int n = y0 - m_lasty0;

		for (int y=0; y<height - n; y++)
			memcpy(Buffer[y], Buffer[y+n], width * sizeof(short));

		for (; y<height; y++)
			memset(Buffer[y], 0, width * sizeof(short));
	} 
	else if ( y0 < m_lasty0 )
	{
		int n = m_lasty0 - y0;

		for (int y=height-1; y>n; y--)
			memcpy(Buffer[y], Buffer[y-n], width * sizeof(short));

		for (; y>=0; y--)
			memset(Buffer[y], 0, width * sizeof(short));
	}

	for (int y=0; y<height; y++)
		if ( x0 > m_lastx0 )	// left
		{
			int n = x0 - m_lastx0;

			for (int x=0; x<width - n; x++)
				Buffer[y][x] = Buffer[y][x+n];

			for (; x<width; x++)
				Buffer[y][x] = 0;
		}
		else if ( x0 < m_lastx0 )
		{
			int n = m_lastx0 - x0;

			for (int x=width-1; x>n; x--)
				Buffer[y][x] = Buffer[y][x-n];

			for (; x>=0; x--)
				Buffer[y][x] = 0;
		}

	m_lastx0 = x0;
	m_lasty0 = y0;
}

// positive reach fixed point after n iteration
// negative too big           after n iternation
// 0		don't know after        limit number of iterations

int KMyCanvas::MandelCount(HDC hDC, int xi, int yi, int limit)
{
	const double thresh  = 4.0;
	const double epsilon = 0.000001;

	double x0, y0, x, y;

	x0 = m_x + m_unit * xi;
	y0 = m_y + m_unit * yi;
	
	double oldx[5], oldy[5];

	for (int i=0; i<5; i++)
	{
		oldx[i] = - 100;
		oldy[i] = - 100;
	}

//	if ( hDC )
//		MoveToEx(hDC, xi, yi, NULL);

	x = x0;
	y = y0;

	for (int count=1; count<limit; count++)
	{
	//	if ( hDC )
	//		LineTo(hDC, (int)((x-m_x)/m_unit), (int)((y-m_y)/m_unit));

		double xx = x * x;
		double yy = y * y;

		double x1 = xx - yy   + x0;
		double y1 = 2 * x * y + y0;

		if ( (fabs(x1-oldx[0]) < epsilon) && (fabs(y1-oldy[0]) < epsilon) )
			return count;

		if ( (fabs(x1-oldx[1]) < epsilon) && (fabs(y1-oldy[1]) < epsilon) )
			return count;

		if ( (fabs(x1-oldx[2]) < epsilon) && (fabs(y1-oldy[2]) < epsilon) )
			return count;

		if ( (fabs(x1-oldx[3]) < epsilon) && (fabs(y1-oldy[3]) < epsilon) )
			return count;

		if ( (fabs(x1-oldx[4]) < epsilon) && (fabs(y1-oldy[4]) < epsilon) )
			return count;

		if ( (fabs(x1-x) < epsilon) && (fabs(y1-y) < epsilon) )
			return count;

		if ( (xx + yy) > thresh )
			return - count;

		oldx[0] = oldx[1]; oldx[1] = oldx[2]; oldx[2] = oldx[3]; oldx[3] = oldx[4]; oldx[4] = x;
		oldy[0] = oldy[1]; oldy[1] = oldy[2]; oldy[2] = oldy[3]; oldy[3] = oldy[4]; oldy[4] = y;

		x = x1;
		y = y1;
	}

	return 0;
}

// #define SYSRGN 4

// extern "C" BOOL WINAPI GetRandomRgn(HDC hDC, HRGN hRgn, int which);




COLORREF inline Between(COLORREF from, COLORREF to, int val, int limit)
{
	return RGB( ( GetRValue(from) * (limit-val) + GetRValue(to) * val ) / limit,
				( GetGValue(from) * (limit-val) + GetGValue(to) * val ) / limit,
				( GetBValue(from) * (limit-val) + GetBValue(to) * val ) / limit
			  );
}


void KMyCanvas::SetLimit(int limit, BOOL bRedraw)
{
	m_limit = limit;

	KColor cout;
	KColor cin;

	cout.red   = 0;
	cout.green = 255;
	cout.blue  = 64;
	cout.ToHLS();

	cin.red = 244;
	cin.green = 241;
	cin.blue = 104;
	cin.ToHLS();

	for (int i=0; i<m_limit; i++)
	{
		cout.hue = cout.hue + 17; 
		if ( cout.hue>=360 ) cout.hue -= 360;
		cout.ToRGB();

		Out[i] = RGB(cout.red, cout.green, cout.blue);

		cin.hue = cin.hue + 33; 
		if ( cin.hue>=360 ) cin.hue -= 360;
		cin.ToRGB();

		In[i] = RGB(cin.red, cin.green, cin.blue);
	}

	if ( bRedraw )
	{
		InvalidateRect(m_hWnd, NULL, TRUE); 
		::UpdateWindow(m_hWnd); 
	}
}


void KMyCanvas::OnDraw(HDC hDC, const RECT * rcPaint)
{
	if ( m_lastlimit>= m_limit )
		return;

	TCHAR title [MAX_PATH];

	sprintf(title, "Mandelbrot Set (%f %f) zoom %d:%d unit %f", m_x, m_y, m_zoommul, m_zoomdiv, m_unit);
	SetWindowText(GetParent(m_hWnd), title);

	int tick = GetTickCount();

	if ( rcPaint )
		m_lastlimit = 0;

	RECT rect;

	GetClientRect(m_hWnd, & rect);

	int x0 = GetScrollPos(m_hWnd, SB_HORZ);
	int y0 = GetScrollPos(m_hWnd, SB_VERT);

//	ClearBuffer(rect.right, rect.bottom, x0, y0);
	
	HRGN hRgn = CreateRectRgn(0, 0, 1, 1);

	{
		GetRandomRgn(hDC, hRgn, SYSRGN);
		POINT p0;
		GetDCOrgEx(hDC, & p0);
	
		// change region to be relative to DC, NT only
//		if ( HIWORD(hDC) )
			OffsetRgn(hRgn, - p0.x, - p0.y);
	}

	m_lastlimit += 16;

	for (int y=0; y<rect.bottom; y++) 
	{
		COLORREF lastc = 0xFFFFFFFF;
		int		 count = 0;

		for (int x=0; x<rect.right;  x++)
//			if ( Buffer[y][x]==0 )
			if ( PtInRegion(hRgn, x, y) )
			{
				int count = MandelCount(NULL, x+x0, y+y0, m_limit);
				COLORREF c = 0;

				if ( count==0 )		// don't know
					c = RGB(64, 78, 170);		// Red
				else if ( count>0 )	// yes
					c = In[count];	// green
				else if ( count< -3 )
					c = Out[-count]; 

//				Buffer[y][x] = count;

				if ( c )
					SetPixel(hDC, x+x0, y+y0, c);
			}
	}

	tick = GetTickCount() - tick;

	wsprintf(title, "tick %d", tick);
	m_pStatus->SetText(1, title);
}


void KMyCanvas::OnZoom(int x, int y, int mul, int div)
{
	m_unit = m_unit  / mul * div;

	KScrollCanvas::OnZoom(x, y, mul, div);
}


void KMyCanvas::OnTimer(WPARAM wParam, LPARAM lParam)
{
/*	HDC hDC = GetDC(m_hWnd);
				
	SetWindowOrgEx(hDC, 0, 0, NULL);
	SetViewportOrgEx(hDC, - GetScrollPos(m_hWnd, SB_HORZ), - GetScrollPos(m_hWnd, SB_VERT), NULL);
				
	OnDraw(hDC, NULL);

	ReleaseDC(m_hWnd, hDC); */
}


void KMyCanvas::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	TCHAR temp[MAX_PATH];
				
	int x = GetScrollPos(m_hWnd, SB_HORZ) + LOWORD(lParam);
	int y = GetScrollPos(m_hWnd, SB_VERT) + HIWORD(lParam);
				
	sprintf(temp, "(%f, %f) %d", m_x + x * m_unit,
		                         m_y + y * m_unit,
								 MandelCount(NULL, x, y, m_limit));

   	m_pStatus->SetText(0, temp);
}			


BOOL KMyCanvas::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch ( LOWORD(wParam) )
	{
		case ID_VIEW_RESET:
			m_x = -2; m_y = -1; m_unit = (double) 0.0025;
			InvalidateRect(m_hWnd, NULL, TRUE);
			return TRUE;

		case  IDM_OPT_64 : SetLimit(   64, TRUE); return TRUE;
		case  IDM_OPT_128: SetLimit(  128, TRUE); return TRUE;
		case  IDM_OPT_256: SetLimit(  256, TRUE); return TRUE;
		case  IDM_OPT_512: SetLimit(  512, TRUE); return TRUE;
		case IDM_OPT_1024: SetLimit( 1024, TRUE); return TRUE;
		case IDM_OPT_4096: SetLimit( 2048, TRUE); return TRUE;
 	   case IDM_OPT_16384: SetLimit(16384, TRUE); return TRUE;
	}

	return FALSE;
}


class KLogoFrame : public KFrame
{
	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KFrame::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_PIXEL));
	}

public:
	KLogoFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
		KToolbar * pToolbar, KCanvas * pCanvas, KStatusWindow * pStatus) :
			KFrame(hInstance, pButtons, nCount, pToolbar, pCanvas, pStatus)
	{
	}

};

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmd, int nShow)
{
	KMyCanvas     canvas;
	KStatusWindow status;
	KLogoFrame    frame(hInst, NULL, 0, NULL, & canvas, & status);

/*	double x, y;
	double x0, y0;

	x0 = - 0.75;
	y0 = - 0.0;
	
	x = x0;
	y = y0;

	for (int count=1; count<=10000; count++)
	{
		char temp[32];

		sprintf(temp, "%6d %f %f\n", count, x, y);
		OutputDebugString(temp);

		double xx = x * x;
		double yy = y * y;

		double x1 = xx - yy   + x0;
		double y1 = 2 * x * y + y0;

		x = x1;
		y = y1;
	}
*/
	frame.CreateEx(0, _T("Mandelbrot"), _T("Mandelbrot Set"),
		WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();
    frame.MessageLoop();

	return 0;
}
