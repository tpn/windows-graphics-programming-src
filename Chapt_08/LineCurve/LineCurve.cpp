//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : linecurve.cpp					                                     //
//  Description: Line/curve demo program, Chapter 8                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>

#include "..\..\include\win.h"
#include "..\..\include\Canvas.h"
#include "..\..\include\Status.h"
#include "..\..\include\FrameWnd.h"
#include "..\..\include\Affine.h"
#include "..\..\include\Pen.h"
#include "..\..\include\curve.h"
#include "..\..\include\BezierCurve.h"

#include "Resource.h"


class KMyCanvas : public KCanvas
{
	virtual void    OnDraw(HDC hDC, const RECT * rcPaint);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual	BOOL    OnCommand(WPARAM wParam, LPARAM lParam);

	int		m_test;
	int		m_lastx;
	int		m_lasty;

	void SetTest(int test);

	void DrawCrossHair(HDC hDC, bool on);

	void TestDrawMode(HDC hDC);
	void TestWidthCap(HDC hDC);
	void TestDCPen(HDC hDC);
	void TestPenStyle(HDC hDC);

	void TestCosmetic(HDC hDC);
	void TestGeometricWidth(HDC hDC);
	void TestGeometricStyle(HDC hDC);

	void TestLine1(HDC hDC);
	void TestLine2(HDC hDC);
	void TestBezier(HDC hDC);
	void BezierDemo(HDC hDC);

	void DragBezier(HDC hDC, int x, bool on);
	void TestBezierPen(HDC hDC);
	void TestArc(HDC hDC);

	void ArcToBezier(HDC hDC);
	void AngleArcToBezier(HDC hDC);

	void TestPath(HDC hDC);
	void TestPathStyleCurve(HDC hDC);

	void TestMapPath(HDC hDC);

	POINT m_bezier[4];

public:

	KMyCanvas()
	{
		m_test  = -1;
		m_lastx = -1;
		m_lasty = -1;

		m_bezier[0].x =  30; m_bezier[0].y = 330;
		m_bezier[1].x = 130; m_bezier[1].y =  30;
		m_bezier[2].x = 430; m_bezier[2].y = 130;
		m_bezier[3].x = 630; m_bezier[3].y = 430;
	}
};


void KMyCanvas::SetTest(int test)
{
	m_test = test;

	HMENU hMain = GetMenu(GetParent(m_hWnd));

	TCHAR Temp[MAX_PATH];

	_tcscpy(Temp, _T("LineCurve - "));

	GetMenuString(hMain, m_test, Temp + _tcslen(Temp), 
		sizeof(Temp)/sizeof(TCHAR) - _tcslen(Temp), MF_BYCOMMAND);

	SetWindowText(GetParent(m_hWnd), Temp);
}


BOOL KMyCanvas::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch ( LOWORD(wParam) )
	{
		case IDM_TEST_DRAWMODE:
		case IDM_TEST_WIDTHCAP:
		case IDM_TEST_DCPEN:
		case IDM_TEST_PENSTYLE:
		case IDM_TEST_COSMETIC:
		case IDM_TEST_GEOMETRIC_WIDTH:
		case IDM_TEST_GEOMETRIC_STYLE:
		case IDM_TEST_LINE1:
		case IDM_TEST_LINE2:
		case IDM_TEST_BEZIER:
		case IDM_TEST_BEZIERDEMO:
		case IDM_TEST_BEZIERPEN:
		case IDM_TEST_ARC:
		case IDM_TEST_ARCBEZIER:
		case IDM_TEST_ANGLEARCBEZIER:
		case IDM_TEST_PATH:
		case IDM_TEST_PATHSTYLECURVE:
		case IDM_TEST_MAPPATH:
			SetTest( LOWORD(wParam) );
			InvalidateRect(m_hWnd, NULL, TRUE);
			return TRUE;

		case IDM_FILE_EXIT:
			DestroyWindow(GetParent(m_hWnd));
			return TRUE;
	}

	return FALSE;	// not processed
}


void KMyCanvas::DrawCrossHair(HDC hDC, bool on)
{
	if ( (m_lastx<0) || (m_lasty<0) )
		return;

	if ( m_test == IDM_TEST_BEZIER )
		DragBezier(hDC, m_lastx, on);
	else
	{
		RECT rect;

		GetClientRect(m_hWnd, & rect);

		SetROP2(hDC, R2_XORPEN);
		KPen pen(PS_DOT, 0, RGB(0, 0, 0xFF), hDC);

		Line(hDC, rect.left, m_lasty, rect.right, m_lasty);
		Line(hDC, m_lastx, rect.top, m_lastx, rect.bottom);
	}
}


LRESULT KMyCanvas::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr;

	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd = hWnd;
			SetTest(IDM_TEST_DRAWMODE);
			return 0;

		case WM_PAINT:
			{
				PAINTSTRUCT ps; 

//				SetLastError(0);
//				HRGN hrgnClip = CreateRectRgn(0,0,0,0);
//				GetUpdateRgn(m_hWnd,hrgnClip,FALSE);
//				HDC hdc = GetDCEx(m_hWnd,NULL,DCX_VALIDATE|DCX_INTERSECTUPDATE);

				HDC hDC = BeginPaint(m_hWnd, &ps);

			//	DrawCrossHair(hDC);
				OnDraw(hDC, &ps.rcPaint);
				DrawCrossHair(hDC, true);

				EndPaint(m_hWnd, &ps);
			}
			return 0;

		case WM_MOUSEMOVE:
			{
				HDC hDC = GetDC(hWnd);

				DrawCrossHair(hDC, false);
				m_lastx = LOWORD(lParam); m_lasty = HIWORD(lParam);
				DrawCrossHair(hDC, true);
				ReleaseDC(hWnd, hDC);
				
				TCHAR temp[32];
				wsprintf(temp, _T("(%d, %d)"), m_lastx, m_lasty);
				m_pStatus->SetText(0, temp);
		
			}
			return 0;

		default:
			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return lr;
}


const TCHAR * R2_Names[] = 
{
	"R2_BLACK",
	"R2_NOTMERGEPEN",
	"R2_MASKNOTPEN",
	"R2_NOTCOPYPEN",
	"R2_MASKPENNOT",
	"R2_NOT",
	"R2_XORPEN",
	"R2_NOTMASKPEN",
	"R2_MASKPEN",
	"R2_NOTXORPEN",
	"R2_NOP",
	"R2_MERGENOTPEN",
	"R2_COPYPEN",
	"R2_MERGEPENNOT",
	"R2_MERGEPEN",
	"R2_WHITE"
};


void KMyCanvas::TestDrawMode(HDC hDC)
{
	for (int c=0; c<20; c++)
	{
		RECT rect = { c*20+40, 30, c*20+58, 400 };

		HBRUSH hBrush = CreateSolidBrush(PALETTEINDEX(c));

		FillRect(hDC, & rect, hBrush);

		DeleteObject(hBrush);
	}

	KPen pen(PS_SOLID, 0, RGB(0xFF, 0, 0), hDC);

	for (int r=R2_BLACK; r<=R2_WHITE; r++)
	{
		SetROP2(hDC, r);

		for (int i=0; i<18; i++)
		{
			Label(hDC, 440, r*22+20, R2_Names[r-R2_BLACK]);

			Line(hDC, 30, r*22+20+i, 430, r*22+20+i);
		}
	}
} 


void ZoomPixel(HDC hDC, int x0, int y0, int x1, int y1)
{
	COLORREF c = GetPixel(hDC, x0, y0);

	HBRUSH hBrush = CreateSolidBrush(c);
	SelectObject(hDC, hBrush);

	Rectangle(hDC, x1, y1, x1+11, y1+11);
	SelectObject(hDC, GetStockObject(WHITE_BRUSH));
	DeleteObject(hBrush);
}


void KMyCanvas::TestWidthCap(HDC hDC)
{
	char temp[6];

	KPen green(PS_SOLID, 0, RGB(0, 0xFF, 0));
	
	for (int i=0; i<12; i++)
	{
		KPen pen (PS_DOT, i, RGB(0, 0, 0xFF));

		int x0 = 40 + (i/2)*110;
		int x1 = x0 + 64;
		int y0 = 30 + (i & 1) * 200;
		
		// thick line
		Line(hDC, x0, y0, x1, y0, pen.m_hPen);

		// Center line
		Line(hDC, x0, y0, x1, y0, green.m_hPen);

		wsprintf(temp, "w=%d", i);
		Label(hDC, 40+x0-5*12, 90 + y0 -5*12-17, temp);

		// copy zoomed line
		for (int y=-5; y<6; y++)
		for (int x=-5; x<3;  x++)
			ZoomPixel(hDC, x0+x, y0+y, 40+x0+x*12, 90 + y0 + y*12);

		Line(hDC, x0, y0, x1, y0, pen.m_hPen);
	}
}


void KMyCanvas::TestDCPen(HDC hDC)
{
	Label(hDC, 10, 0, _T("Use DC_PEN to simulate gradient fill"));

	SelectObject(hDC, GetStockObject(DC_PEN));

	for (int i=0; i<128; i++)
	{
		SetDCPenColor(hDC, RGB(i, 255-i, 128+i));

		Line(hDC, 40, i+40, 150, i+40);
	}

	SelectObject(hDC, GetStockObject(BLACK_PEN));
	
	// Test DeleteObject on stock object
	BOOL rslt = DeleteObject(GetStockObject(DC_PEN));
}

const LPCTSTR mess_PenStyle [] =
{
	_T("PS_SOLID"),
	_T("PS_DASH"),
	_T("PS_DOT"),
	_T("PS_DASHDOT"),
	_T("PS_DASHDOTDOT"),
	_T("PS_NULL"),
	_T("PS_INSIDEFRAME"),
	_T("PS_USERSTYLE"),
	_T("PS_ALTERNATE")
};


void KMyCanvas::TestPenStyle(HDC hDC)
{
	// Pen style
	for (int i = PS_SOLID; i<=PS_INSIDEFRAME; i++)
	{
		Label(hDC, 10, i*20+10, mess_PenStyle[i]);

		SetBkColor(hDC, RGB(0, 0xFF, 0));
		SetBkMode(hDC, OPAQUE);

		{	
			KPen Pen(i, 1, RGB(0x00, 0x00, 0xFF), hDC);
		
			Line(hDC, 150, i*20 + 17, 180, i*20 + 17);
		}

		SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
	
		// copy to zoomed display
		for (int x=150; x<180; x++)
			ZoomPixel(hDC, x,                i*20 + 17,
				           200 + (x-150)*12, i*20 + 12);
	}
}


void KMyCanvas::TestCosmetic(HDC hDC)
{
	const LOGBRUSH logbrush = { BS_SOLID, RGB(0, 0, 0xFF), 0 };

//	SetMapMode(hDC, MM_ANISOTROPIC);
//	SetViewportExtEx(hDC, 2, 2, NULL);

	for (int i = PS_SOLID; i<=PS_ALTERNATE; i++)
	{
		HPEN hPen;
		const DWORD cycle[4] = { 4, 3, 2, 1 };
		
		if ( i==PS_USERSTYLE )
			hPen = ExtCreatePen(PS_COSMETIC | i, 1, & logbrush, 4, cycle);
		else
			hPen = ExtCreatePen(PS_COSMETIC | i, 1, & logbrush, 0, NULL);
		
		Label(hDC, 10, i*20+10, mess_PenStyle[i]);

		if ( hPen )
		{
			SelectObject(hDC, hPen);
			SetBkColor(hDC, RGB(0, 0xFF, 0));
			SetBkMode(hDC, OPAQUE);
		//	SetROP2(hDC, R2_XORPEN);

			// draw style line
			Line(hDC, 150, i*20 + 17, 180, i*20 + 17);

			SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
	
			SelectObject(hDC, GetStockObject(BLACK_PEN));
			DeleteObject(hPen);

			// copy to zoomed display
			for (int x=150; x<180; x++)
				ZoomPixel(hDC, x,                i*20 + 17,
					           200 + (x-150)*12, i*20 + 12);
		}
	}
}

void KMyCanvas::TestGeometricWidth(HDC hDC)
{
	const LOGBRUSH logbrush = { BS_SOLID, RGB(0, 0xFF, 0), 0 };

	int save = SaveDC(hDC);

	HPEN hGreen = CreatePen(PS_SOLID, 0, RGB(0, 0xFF, 0));

	SetBkColor(hDC, RGB(0, 0xFF, 0));
	SetBkMode(hDC, OPAQUE);

	// Geometric line width under transformation
	
		HPEN hPen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_FLAT, 61, & logbrush, 0, NULL);
		
		KAffine affine;

		POINT p[6] = { 0, 0, 200, 0, -00, -30, 200, -30, 200, 30, 0, 30 };

		affine.Rotate(30, 0, 0);
		affine.Translate(100, -200);

		SelectObject(hDC, hPen);
		SetMapMode(hDC, MM_ANISOTROPIC);
		SetViewportExtEx(hDC, 1, -1, NULL);

		SetGraphicsMode(hDC, GM_ADVANCED);

		SetWorldTransform(hDC, & affine.m_xm);

		LPtoDP(hDC, p, 6);

		Line(hDC, 0, 0, 200, 0);

		SelectObject(hDC, GetStockObject(BLACK_PEN));
		DeleteObject(hPen);

		ModifyWorldTransform(hDC, NULL, MWT_IDENTITY);
		SetGraphicsMode(hDC, GM_COMPATIBLE);

		SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
		SetTextAlign(hDC, TA_LEFT);

		SetViewportExtEx(hDC, 1, 1, NULL);

		Line(hDC, p[0].x, p[0].y, 20, 200);
		Label(hDC, 20, 200, "(x0,y0)");

		Line(hDC, p[1].x, p[1].y, 300, 80);
		Label(hDC, 300, 80, "(x1,y1)");

		Line(hDC, p[5].x, p[5].y, 20, 120);
		Label(hDC, 20, 120, "(x0-dx,y0+dy)");

		Line(hDC, p[4].x, p[4].y, 260, 40);
		Label(hDC, 260, 40, "(x1-dx,y1+dy)");

		Line(hDC, p[3].x, p[3].y, 300, 160);
		Label(hDC, 300, 160, "(x1+dx,y1-dy)");

		Line(hDC, p[2].x, p[2].y, 130, 250);
		Label(hDC, 130, 250, "(x0+dx,y0-dy)");

		Label(hDC, 20, 20, "dx = pen_width * sin(ø)/2");
		Label(hDC, 20, 50, "dy = pen_width * cos(ø)/2");
	
	DeleteObject(hGreen);

	RestoreDC(hDC, save);
}


class KGPen
{
public:
	DWORD    m_cycle[4];
	LOGBRUSH m_logbrush;
	HPEN     m_hPen;

	KGPen()
	{
		m_cycle[0] = 20;
		m_cycle[1] = 15;
		m_cycle[2] = 10;
		m_cycle[3] =  5;

		m_logbrush.lbStyle = BS_SOLID;
		m_logbrush.lbColor = RGB(0, 0, 0xFF);
		m_logbrush.lbHatch = 0;

		m_hPen = NULL;
	}

	~KGPen(void)
	{
		if ( m_hPen )
			DeleteObject(m_hPen);
	}

	void SetPen(int style, int width);

	void DrawLine(HDC hDC, const TCHAR * mess, int x0, int y0, int x1, int y1);
	void DrawPolyLine(HDC hDC, int count, POINT * cord);
	void DrawZ(HDC hDC, int endcap, int x, int y, COLORREF color, 
			   const TCHAR * desp, bool poly=false);
};

void KGPen::SetPen(int style, int width)
{
	if ( m_hPen )
		DeleteObject(m_hPen);

	if ( ( style & PS_STYLE_MASK ) == PS_USERSTYLE )
		m_hPen = ExtCreatePen(PS_GEOMETRIC | style, width, & m_logbrush, 4, m_cycle);
	else
		m_hPen = ExtCreatePen(PS_GEOMETRIC | style, width, & m_logbrush, 0, NULL);
}

void KGPen::DrawLine(HDC hDC, const TCHAR * mess, int x0, int y0, int x1, int y1)
{
	if ( mess )
		Label(hDC, x0, y0-25, mess);

	if ( m_hPen )
	{
		SelectObject(hDC, m_hPen);
		SetBkColor(hDC, RGB(0, 0xFF, 0));
		SetBkMode(hDC, OPAQUE);

		// draw style line
		Line(hDC, x0, y0, x1, y1);

		SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
	
		SelectObject(hDC, GetStockObject(BLACK_PEN));
	}
}

void KGPen::DrawPolyLine(HDC hDC, int count, POINT * cord)
{
	if ( m_hPen )
	{
		SelectObject(hDC, m_hPen);
		SetBkColor(hDC, RGB(0, 0xFF, 0));
		SetBkMode(hDC, OPAQUE);

		// draw style line
		Polyline(hDC, cord, count); 

		SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
	
		SelectObject(hDC, GetStockObject(BLACK_PEN));
	}
}

void KGPen::DrawZ(HDC hDC, int endcap, int x, int y, COLORREF color, const TCHAR * desp, bool poly)
{
	SetPen(PS_SOLID | endcap, 25);

	if ( poly )
	{
		POINT p[4] = { x, y, x+100, y, x, y+50, x+100, y+50 };
		
		DrawPolyLine(hDC, 4, p);
	}
	else
	{
		DrawLine(hDC, NULL, x,     y,    x+100, y);

		m_logbrush.lbColor = color;
		SetPen(PS_SOLID | endcap, 25);
		DrawLine(hDC, NULL, x+100, y,    x   ,  y+50);

		m_logbrush.lbColor = RGB(0, 0, 0xFF);
		SetPen(PS_SOLID | endcap, 25);
		DrawLine(hDC, NULL, x    , y+50, x+100, y+50);
	}

	SelectObject(hDC, GetStockObject(WHITE_PEN));
	Line(hDC, x, y, x+100, y); LineTo(hDC, x, y+50); LineTo(hDC, x+100, y+50);

	while ( desp )
	{
		const TCHAR * p = _tcschr(desp, '\n');
		int len;

		if ( p )
			len = ((int) p - (int) desp)/sizeof(TCHAR);
		else
			len = _tcslen(desp);

		TextOut(hDC, x, y+70, desp, len);
		y+= 20;

		if ( p )
			desp = p + 1;
		else
			desp = NULL;
	}
}


void KMyCanvas::TestGeometricStyle(HDC hDC)
{
	KGPen gpen;

	for (int i = PS_SOLID; i<=PS_ALTERNATE; i++)
	{
		Label(hDC, 10, i*20+30, mess_PenStyle[i]);

		int y = i*20 + 37;

		gpen.SetPen(i | PS_ENDCAP_FLAT, 3);
		gpen.DrawLine(hDC, (i==PS_SOLID) ? "w=3, flat" : NULL,          150, y, 250, y);
		
		gpen.SetPen(i | PS_ENDCAP_FLAT, 7);
		gpen.DrawLine(hDC, (i==PS_SOLID) ? "w=7, flat" : NULL,          270, y, 370, y);

		gpen.SetPen(i | PS_ENDCAP_SQUARE, 11);
		gpen.DrawLine(hDC, (i==PS_SOLID) ? "w=11, square" : NULL,       390, y, 490, y);

		gpen.m_logbrush.lbStyle = BS_HATCHED;
		gpen.m_logbrush.lbHatch = HS_DIAGCROSS;

		gpen.SetPen(i | PS_ENDCAP_ROUND, 15);
		gpen.DrawLine(hDC, (i==PS_SOLID) ? "w=15, round, hatch" : NULL, 510, y, 610, y);

		gpen.m_logbrush.lbStyle = BS_SOLID;
		gpen.m_logbrush.lbHatch = 0;
	}

	gpen.DrawZ(hDC, PS_ENDCAP_FLAT,    50, 260, RGB(0xFF, 0x3F, 0x0F), _T("PS_ENDCAP_FLAT"));
	gpen.DrawZ(hDC, PS_ENDCAP_SQUARE, 220, 260, RGB(0xFF, 0x3F, 0x0F), _T("PS_ENDCAP_SQUARE"));
	gpen.DrawZ(hDC, PS_ENDCAP_ROUND,  390, 260, RGB(0xFF, 0x3F, 0x0F), _T("PS_ENDCAP_ROUND"));
	
	SetROP2(hDC, R2_XORPEN);
	gpen.DrawZ(hDC, PS_ENDCAP_ROUND,  560, 260, RGB(0xFF, 0x3F, 0x0F), _T("PS_ENDCAP_ROUND\nR2_XORPEN"));

	SetROP2(hDC, R2_COPYPEN);

//	FLOAT f;

//	SetMiterLimit(hDC, 4.35, & f);

	gpen.DrawZ(hDC, PS_ENDCAP_FLAT   | PS_JOIN_BEVEL,  50, 400, RGB(0xFF, 0x3F, 0x0F), _T("PS_ENDCAP_FLAT\nPS_JOIN_BEVEL"), true);
	gpen.DrawZ(hDC, PS_ENDCAP_SQUARE | PS_JOIN_MITER, 220, 400, RGB(0xFF, 0x3F, 0x0F), _T("PS_ENDCAP_SQUARE\nPS_JOIN_MITER"), true);
	gpen.DrawZ(hDC, PS_ENDCAP_ROUND  | PS_JOIN_ROUND, 390, 400, RGB(0xFF, 0x3F, 0x0F), _T("PS_ENDCAP_ROUND\nPS_JOIN_ROUND"), true);
	
	SetROP2(hDC, R2_XORPEN);
	gpen.DrawZ(hDC, PS_ENDCAP_ROUND  | PS_JOIN_ROUND, 560, 400, RGB(0xFF, 0x3F, 0x0F), _T("PS_ENDCAP_ROUND\nPS_JOIN_ROUND\nR2_XORPEN"), true);
}


void KMyCanvas::TestLine1(HDC hDC)
{
	const int    N      = 19;
	const int    Radius = 200;
	const double theta  = 3.1415926 * 2 / N;
	
	SelectObject(hDC, GetStockObject(DC_PEN));

	// simple line drawing
	const COLORREF color[] = { 
		RGB(0, 0, 0), RGB(255,0,0), RGB(0,255,0), RGB(0,0, 255),
		RGB(255,255,0), RGB(0, 255, 255), RGB(255, 255, 0),
		RGB(127, 255, 0), RGB(0, 127, 255), RGB(255, 0, 127)
	};
	
	for (int p=0; p<N; p++)
	for (int q=0; q<p; q++)
	{
		SetDCPenColor(hDC, color[min(p-q, N-p+q)]);

		Line(hDC, (int)(220 + Radius * sin(p * theta)), 
			      (int)(220 + Radius * cos(p * theta)), 
		          (int)(220 + Radius * sin(q * theta)), 
			      (int)(220 + Radius * cos(q * theta)));
	}
}


void Triangle(HDC hDC, int x0, int y0, int x1, int y1, int x2, int y2, bool extra=false)
{
	POINT corner[5] = { x0, y0, x1, y1, x2, y2, x0, y0, x1, y1 };

	if ( extra ) // extra segment to make nicer closing join
		Polyline(hDC, corner, 5);
	else
		Polyline(hDC, corner, 4);
}


void CALLBACK LineDDAProc(int x, int y, LPARAM lpData)
{
	HDC hDC = (HDC) lpData;
	POINT cur;

	GetCurrentPositionEx(hDC, & cur);

	if ( (cur.x & 31)== 0 ) // every 32th call
		Triangle(hDC, x, y-16, x+20, y+18, x-20, y+18);

	cur.x ++;
	MoveToEx(hDC, cur.x, cur.y, NULL);
}


void KMyCanvas::TestLine2(HDC hDC)
{
	LOGBRUSH logbrush = { BS_SOLID, RGB(0, 0, 0xFF), 0 };

	HPEN hPen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | 
		            PS_ENDCAP_FLAT | PS_JOIN_MITER, 
					15, & logbrush, 0, NULL);
//	SetROP2(hDC, R2_XORPEN);
	
	HGDIOBJ hOld = SelectObject(hDC, hPen);
	
	// drawing triangle using mutlple LineTo calls
	SetViewportOrgEx(hDC, 100, 50, NULL);
	Line(hDC, 0, 0, 50, 86); 
	LineTo(hDC, -50, 86); LineTo(hDC, 0, 0);
	
	// using polyline
	SetViewportOrgEx(hDC, 230, 50, NULL);
	Triangle(hDC, 0, 0, 50, 86, -50, 86, false );

	// using polyline, extra segment
	SetViewportOrgEx(hDC, 360, 50, NULL);
	Triangle(hDC, 0, 0, 50, 86, -50, 86, true );

	// using LineDDA
	SetViewportOrgEx(hDC, 490, 50, NULL);
	SelectObject(hDC, hOld);
	DeleteObject(hPen);

	hPen = ExtCreatePen(PS_GEOMETRIC | PS_DOT | PS_ENDCAP_ROUND, 
					3, & logbrush, 0, NULL);
	SelectObject(hDC, hPen);

	LineDDA( 0,  0,   50, 86, LineDDAProc, (LPARAM) hDC); 
	LineDDA( 50, 86, -50, 86, LineDDAProc, (LPARAM) hDC); 
	LineDDA(-50, 86,   0,  0, LineDDAProc, (LPARAM) hDC);

	SetViewportOrgEx(hDC, 50, 150, NULL);
	SelectObject(hDC, hOld);
	DeleteObject(hPen);

	// PolyPolyline
	hPen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_ROUND, 
					5, & logbrush, 0, NULL);
	SelectObject(hDC, hPen);
	
	POINT P[10] = { 0,  0, 100,  0, 100, 100, 0, 100,  0,  0,
		          50, 50, 150, 50, 150, 150, 50,150, 50, 50 };
	DWORD c[2] = { 5, 5 };
	SetROP2(hDC, R2_XORPEN);
	PolyPolyline(hDC, P, c, 2);

	SetViewportOrgEx(hDC, 0, 0, NULL);
	SelectObject(hDC, hOld);
	DeleteObject(hPen);
}


void KMyCanvas::TestBezier(HDC hDC)
{
}


void KMyCanvas::DragBezier(HDC hDC, int x, bool on)
{
	SetROP2(hDC, R2_XORPEN);
	SetBkMode(hDC, TRANSPARENT);

	LOGBRUSH logbrush = { BS_SOLID, RGB(0xFF, 0xFF, 0), 0 };

	m_bezier[0].x =  30; m_bezier[0].y = 330;
	m_bezier[1].x = 130; m_bezier[1].y =  30;
	m_bezier[2].x = 430; m_bezier[2].y = 130;
	m_bezier[3].x = 630; m_bezier[3].y = 330;

	HPEN hPen, hOld;

	hPen = CreatePen(PS_DOT, 0, RGB(0xFF, 0, 0xFF));
	hOld = (HPEN) SelectObject(hDC, hPen);
	// Dotted line link control points
	Polyline(hDC, m_bezier, 4);
	SelectObject(hDC, hOld);
	DeleteObject(hPen);

	hPen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_ROUND, 3, & logbrush, 0, NULL);
	hOld = (HPEN) SelectObject(hDC, hPen);

	// Solid thick Bezier curve
//	PolyBezier(hDC, m_bezier, 4);
	Bezier(hDC, 30, 330, 130, 30, 430, 130, 630, 330);
	
	SelectObject(hDC, hOld);
	DeleteObject(hPen);

	double  t = (x%101) / 100.0;

	hPen = CreatePen(PS_DASH, 0, RGB(0, 0xFF, 0xFF));
	hOld = (HPEN) SelectObject(hDC, hPen);

	POINT P12; P(P12, m_bezier[0], m_bezier[1], t);
	POINT P23; P(P23, m_bezier[1], m_bezier[2], t);
	POINT P34; P(P34, m_bezier[2], m_bezier[3], t);

	POINT P1223; P(P1223, P12, P23, t);
	POINT P2334; P(P2334, P23, P34, t);
	POINT P0;    P(P0   , P1223, P2334, t);
	
	// P(t) lines
	Line(hDC, P12, P23);
	Line(hDC, P23, P34);
	Line(hDC, P1223, P2334);

	SelectObject(hDC, hOld);
	DeleteObject(hPen);

	SetROP2(hDC, R2_COPYPEN);

	// labels
	if ( on )
		SetTextColor(hDC, 0);
	else
		SetTextColor(hDC, RGB(0xFF, 0xFF, 0xFF));

	Label(hDC, m_bezier[0].x,    m_bezier[0].y+10, "P1");
	Label(hDC, m_bezier[1].x-20, m_bezier[1].y,    "P2");
	Label(hDC, m_bezier[2].x+15, m_bezier[2].y,    "P3");
	Label(hDC, m_bezier[3].x,    m_bezier[3].y+10, "P4");

	Label(hDC, P12.x-30, P12.y,    "P12");
	Label(hDC, P23.x+10, P23.y-20, "P23");
	Label(hDC, P34.x+30, P34.y+10, "P34");

	Label(hDC, P1223.x-20, P1223.y-20, "P1223");
	Label(hDC, P2334.x-20, P2334.y-20, "P2334");

	Label(hDC, P0.x-20, P0.y-20, "P");

	TCHAR num[10];

	sprintf(num, "t=%8.5f    ", t);
	Label(hDC, 250, 250, num);
}



void KMyCanvas::BezierDemo(HDC hDC)
{
	KPen Red (PS_DOT, 0, RGB(0xFF, 0, 0));
	KPen Blue(PS_SOLID, 3, RGB(0, 0, 0xFF));

	for (int z=0; z<=200; z+=40)
	{
		int x = 50, y = 240;

		POINT p[4] = { x, y, x+ 50, y-z, x+100, y-z, x+150, y }; x+= 160;
		POINT q[4] = { x, y, x+ 50, y-z, x+100, y+z, x+150, y }; x+= 180;
		
		POINT r[4] = { x, y, x+170, y-z, x- 20, y-z, x+150, y }; x+= 200;
		POINT s[4] = { x, y, x+170, y-z, x- 20, y+z, x+150, y }; x+= 180;
		
		POINT t[4] = { x+75, y, x,  y-z, x+150, y-z, x+75, y }; 

		Red.Select(hDC);
		Polyline(hDC, p, 4);
		Polyline(hDC, q, 4);
		Polyline(hDC, r, 4);
		Polyline(hDC, s, 4);
		Polyline(hDC, t, 4);
		Red.UnSelect();

		Blue.Select(hDC);
		PolyBezier(hDC, p, 4);
		PolyBezier(hDC, q, 4);
		PolyBezier(hDC, r, 4);
		PolyBezier(hDC, s, 4);
		PolyBezier(hDC, t, 4);
		Blue.UnSelect();

		if ( z==200 )
		{
			Label(hDC, p,   0, 10, -20, -10, +10, -10, -18,  10);
			Label(hDC, q,   0, 10, -20, -10, +15, -10, -18, -25);
			Label(hDC, r,   0, 10,  +5, -10, -20, -10, -18,  10);
			Label(hDC, s,   0, 10, -20, -10, +15, -10, -18, -25);
			Label(hDC, t, -20, 10,  +5, -10, -20, -10, + 5,  10);
		}				
	}
}


void KMyCanvas::TestBezierPen(HDC hDC)
{
	const POINT P[12] = 
	{   50, 200, 100,  50, 150, 350, 200, 200,
	   150,  50, 100, 350,  50, 200,
	   125, 275, 175, 200, 125, 125,  75, 200, 125, 275
	};
	
	const BYTE T[12] = 
	{ 	PT_MOVETO, PT_BEZIERTO, PT_BEZIERTO, PT_BEZIERTO,
	    PT_BEZIERTO , PT_BEZIERTO, PT_BEZIERTO | PT_CLOSEFIGURE,
	    PT_MOVETO, PT_LINETO, PT_LINETO, PT_LINETO, PT_LINETO | PT_CLOSEFIGURE
	};

	SetROP2(hDC, R2_XORPEN);

	LOGBRUSH logbrush = { BS_SOLID, RGB(0xFF, 0xFF, 0), 0 };

	HPEN hPen = ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_FLAT | PS_JOIN_MITER, 15, & logbrush, 0, NULL);
	SelectObject(hDC, hPen);

	PolyBezier(hDC, P, 7); // two Bezier curves
	Polyline(hDC, P+7, 5); // diamond shape
	
	SetViewportOrgEx(hDC, 200, 0, NULL);
	PolyDraw(hDC, P, T, 12);	// Bezier curves and diamon shape together
	
	SetViewportOrgEx(hDC, 0, 0, NULL);
	SelectObject(hDC, GetStockObject(BLACK_PEN));
	DeleteObject(hPen);
}


void KMyCanvas::TestArc(HDC hDC)
{
//	SetMapMode(hDC, MM_ANISOTROPIC);
//	SetViewportExtEx(hDC, 1, -1, NULL);

	// 200 by 250
	const int A =  80;
	const int B = 100;
	
	POINT P[5] = { -A, -B, A, -B, A, B, -A, B, -A, -B };
	POINT S    = {  A * 6/5, -B * 3/5 };
	POINT E    = {  A * 6/5,  B * 3/5 };

	POINT PAA[5] = { -A, -A, A, -A, A, A, -A, A, -A, -A };
	POINT SAA    = {  A * 6/5, -A * 3/5 };
	POINT EAA    = {  A * 6/5,  A * 3/5 };

	// Arc counterclockwise
	// ArcTo clockwise

	for (int i=0; i<3; i++)
	{
		if ( i==0 )
		{
			SetViewportOrgEx(hDC, 200, 300, 0);
			SetArcDirection(hDC, AD_COUNTERCLOCKWISE);
			Label(hDC, -65, P[0].y-40, "Arc, counterclockwise");
		}
		else
		{
			SetViewportOrgEx(hDC, 200 + i* A * 3, 300, 0);
			SetArcDirection(hDC, AD_CLOCKWISE);

			if ( i==1)
				Label(hDC, -55, P[0].y-40, "ArcTo, clockwise");
			else if (i==2)
			{
				Label(hDC, -30, P[0].y-40, "AngleArc");
				memcpy(P, PAA, sizeof(P));
				S = SAA;
				E = EAA;
			}
		}
	
		HPEN hPen = CreatePen(PS_DOT, 0, RGB(0xFF, 0, 0));
		HPEN hOld = (HPEN) SelectObject(hDC, hPen);

		// bounding rectangle, start/end lines
		Polyline(hDC, P, 5);	   Dot(hDC, P[0].x ,P[0].y); Dot(hDC, P[2].x ,P[2].y);
		Line(hDC, 0, 0, S.x, S.y); Dot(hDC, S.x, S.y);
		Line(hDC, 0, 0, E.x, E.y); Dot(hDC, E.x, E.y);

		SelectObject(hDC, hOld);
		DeleteObject(hPen);

		hPen = CreatePen(PS_INSIDEFRAME, 3, RGB(0, 0, 0xFF));
		SelectObject(hDC, hPen);

	//	SetROP2(hDC, R2_XORPEN);

		// solid arc
		switch ( i )
		{
			case 0:
				Arc(hDC, P[0].x, P[0].y, P[2].x, P[2].y, S.x, S.y, E.x, E.y);
				break;

			case 1:
				MoveToEx(hDC, P[0].x, (P[0].y+P[2].y)/2, NULL);
				ArcTo(hDC, P[0].x, P[0].y, P[2].x, P[2].y, S.x, S.y, E.x, E.y);
				break;

			case 2:
				MoveToEx(hDC, P[0].x, (P[0].y+P[2].y)/2, NULL);
				AngleArcTo(hDC, (P[0].x + P[2].y)/2, (P[0].x + P[2].y)/2,A, (float)26.56, (float)(360-26.56*2));
				break;
		}
		
		SelectObject(hDC, hOld);
		DeleteObject(hPen);
	
		// labels
		Label(hDC, P[0].x, P[0].y-20,  "(Left, Top)");
		Label(hDC, P[2].x-100, P[2].y+5, "(Bottom, Right)");

		if ( i==2 )
		{
			Label(hDC, S.x-20, S.y-20, "eStartAngle");
			Label(hDC, E.x-20, E.y+5,  "eStartAngle + eSweepAngle");
		}
		else
		{
			Label(hDC, S.x-20, S.y-20, "(xStart, yStart)");
			Label(hDC, E.x-20, E.y+5,  "(xEnd, yEnd)");
		}
	}

//	SetViewportExtEx(hDC, 1, 1, NULL);
	SetViewportOrgEx(hDC, 0, 0, NULL);
}



void KMyCanvas::ArcToBezier(HDC hDC)
{
	SetMapMode(hDC, MM_ANISOTROPIC);
	SetViewportExtEx(hDC, 1, -1, NULL);

	SetViewportOrgEx(hDC, 50, 450, NULL);

	Line(hDC, 0, 0, 0, 420);
	Line(hDC, 0, 0, 420, 0);

	// Draw Bezier curves for m=0.0 to 1.0
	{
		KPen pen(PS_COSMETIC | PS_ALTERNATE, 1, RGB(0xFF, 0, 0), 0, NULL, hDC);
	
		for (int m=0; m<=400; m+=80)
		{
			POINT P[4] = { 0, 400, m, 400, 400, m, 400, 0 };

		//	Polyline(hDC, P, 4);
			PolyBezier(hDC, P, 4);

			double error, t;
			error = Error(m/400.0, t);

			TCHAR mess[128];
			
			sprintf(mess, _T("m=%3.1f, error(%5.3f) = %+8.5f%%"), m/400.0, t, error*100);
			Label(hDC, 20, 170- m/3, mess);
		}
	}

	double M = 4*(sqrt(2)-1)/3;
	
	int m = (int) (M*400);
	
	POINT Q[4] = { 0, 400, m, 400, 400, m, 400, 0 };

	{
		KPen blue(PS_COSMETIC | PS_ALTERNATE, 1, RGB(0, 0, 0xFF), 0, NULL, hDC);

		Polyline(hDC, Q, 4);
	}

	{
		KPen pen(PS_SOLID, 3, RGB(0, 0, 0xFF), hDC);
		PolyBezier(hDC, Q, 4);

		Label(hDC, Q[0].x+5,  Q[0].y+20, "(0,1)");
		Label(hDC, Q[1].x+5,  Q[1].y+20, "(m,1)");
		Label(hDC, Q[2].x+10, Q[2].y+ 5, "(1,m)");
		Label(hDC, Q[3].x+10, Q[3].y+20, "(1,0)");
	}

	double error, t;
	error = Error(M, t);

	TCHAR mess[128];
			
	sprintf(mess, _T("m=%8.6f, error(%5.3f) = %+8.6f%%"), M, t, error*100);
	Label(hDC, 20, -20, mess);

	// Test FullEllipse
	for (int i=0; i<100; i+=10)
		EllipseToBezier(hDC, 530-i, 280-i, 530+i, 320+i);
	
	SetViewportExtEx(hDC, 1, 1, NULL);
	SetViewportOrgEx(hDC, 0, 0, NULL);
}


void KMyCanvas::AngleArcToBezier(HDC hDC)
{
	SetMapMode(hDC, MM_ANISOTROPIC);
	SetViewportExtEx(hDC, 1, -1, NULL);

	SetViewportOrgEx(hDC, 400, 350, NULL);

	for (int angle=15; angle<=240; angle+=15)
	{
		TCHAR mess[32];
		double err;

		::AngleArcToBezier(hDC, 0, 0, 50+angle*2/3, 50+angle*2/3, 0, angle*3.1415926/180, & err);

		sprintf(mess, "%d degree, error=%8.5f%%", angle, err*100);
		Label(hDC, -380, 350-angle, mess);
	}

	SetViewportExtEx(hDC, 1, 1, NULL);
	SetViewportOrgEx(hDC, 0, 0, NULL);
}


void KMyCanvas::TestPath(HDC hDC)
{
	SetViewportOrgEx(hDC, 150, 200, NULL);

	KPen wide(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_FLAT | PS_JOIN_MITER, 17, RGB(0, 0, 0xFF), 0, NULL);
//	KPen wide(PS_SOLID, 0, RGB(0, 0, 0xFF));
	wide.Select(hDC);

//	KPen wide(PS_COSMETIC | PS_SOLID, 1, RGB(0, 0, 0xFF), 0, NULL);
//	wide.Select(hDC);
	
	// Path construction
	{
		BeginPath(hDC);
		
		MoveToEx(hDC, 0, 0, NULL);
		AngleArc(hDC, 0, 0, 150, 0, 135);

		POINT P[] = { -75, 20, -20, -75, 40, -40 };
		PolyBezierTo(hDC, P, 3);
		
		CloseFigure(hDC);

		EndPath(hDC);
	}

//	SetViewportOrgEx(hDC, 160, 200, NULL);
	
//	int n = SaveDC(hDC);
//	StrokePath(hDC);
//	RestoreDC(hDC, n);
//	SetViewportOrgEx(hDC, 150, 200, NULL);

	KPathData org;	org.GetPathData(hDC);
	
	FlattenPath(hDC);
	KPathData flt;  flt.GetPathData(hDC);

	WidenPath(hDC);
	KPathData wid;  wid.GetPathData(hDC);

	wide.UnSelect();

	//////////////////////////////////////////////
	// Draw original path data
	PolyDraw(hDC, org.m_pPoint, org.m_pFlag, org.m_nCount);

	// Display origin control points 
	SetViewportOrgEx(hDC, 450, 200, NULL);
	org.MarkPoints(hDC);

	//////////////////////////////////////////////
	// Display flatten lines
	SetViewportOrgEx(hDC, 150, 400, NULL);
	PolyDraw(hDC, flt.m_pPoint, flt.m_pFlag, flt.m_nCount);
	

	// Display flatten control
	SetViewportOrgEx(hDC, 450, 400, NULL);
	flt.MarkPoints(hDC);

	///////////////////////////////////////////
	// Display flatten lines
	SetViewportOrgEx(hDC, 150, 600, NULL);
	PolyDraw(hDC, wid.m_pPoint, wid.m_pFlag, wid.m_nCount);

	// Display flatten control
	SetViewportOrgEx(hDC, 450, 600, NULL);
	wid.MarkPoints(hDC);

	SetViewportOrgEx(hDC, 750, 200, NULL);

	// Use WidenPath and Stroke path to show geometric line construction
	for (int i=0; i<3; i++)
	{
		const WideStyle[] = { PS_ENDCAP_SQUARE | PS_JOIN_MITER,
							  PS_ENDCAP_ROUND  | PS_JOIN_ROUND,
							  PS_ENDCAP_FLAT   | PS_JOIN_BEVEL
							};
		const ThinStyle[] = { PS_ALTERNATE, PS_DOT, PS_SOLID };
		const Color    [] = { RGB(0xFF, 0, 0), RGB(0, 0, 0xFF), RGB(0,0,0) };


		{
			KPen wide(PS_GEOMETRIC | PS_SOLID | WideStyle[i], 70, RGB(0, 0, 0xFF), 0, NULL, hDC);

			BeginPath(hDC);
				MoveToEx(hDC, 150, 0, NULL);
				LineTo(hDC, 0, 0);
				LineTo(hDC, 100, -100);
			EndPath(hDC);
			WidenPath(hDC);
		}
		
		if (i==2 )
		{
			KPathData pd; 
			pd.GetPathData(hDC);
			pd.MarkPoints(hDC, false);
		}

		{
			KPen thin(PS_COSMETIC | ThinStyle[i], 1, Color[i], 0, NULL, hDC);
			
			StrokePath(hDC);
		}
	}

	SetViewportOrgEx(hDC, 0, 0, NULL);

#ifdef TEST_INSIDEFRAME

	BeginPath(hDC);

	Ellipse(hDC, 0, 0, 100, 100);

	HPEN    hPen = CreatePen(PS_INSIDEFRAME, 21, RGB(0xFF, 0, 0));
	HGDIOBJ hOld = SelectObject(hDC, hPen);
	
	Ellipse(hDC, 0, 0, 100, 100);

	EndPath(hDC);

	SelectObject(hDC, hOld);

	StrokePath(hDC);

	DeleteObject(hPen);

#endif
}


void KMyCanvas::TestPathStyleCurve(HDC hDC)
{
	SetViewportOrgEx(hDC, 400, 300, NULL);

	for (int i=6; i<10; i++)
	{
		BeginPath(hDC);
		Ellipse(hDC, -50-i*25, -50-i*20, 50+i*25, 50+i*20);
		CloseFigure(hDC);
		EndPath(hDC);

		FlattenPath(hDC);
		KPathData flt;  
		flt.GetPathData(hDC);

		KDashes dash(hDC, i);
		KStyleCurve sc(& dash);

		sc.PolyDraw(flt.m_pPoint, flt.m_pFlag, flt.m_nCount);
	}

	SetViewportOrgEx(hDC, 0, 0, NULL);
}


void KMyCanvas::TestMapPath(HDC hDC)
{
	{
		KPen wide(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_FLAT | PS_JOIN_MITER, 15, 
				RGB(0, 0, 0xFF), 0, NULL, hDC);

		BeginPath(hDC);
			Rectangle(hDC, 50, 50, 200, 500);
		EndPath(hDC);

		WidenPath(hDC);
	}

	KPathData pd;
	pd.GetPathData(hDC);

	StrokePath(hDC);

	KBiLinearMap bl;
	bl.SetWindow(50, 50, 150, 450);
	
	POINT Dest[] = { 200+9, 500, 200+9, 50, 500+9, 400, 450+9, 300 };
	bl.SetDestination(Dest);

	pd.MapPoints(bl);

	BeginPath(hDC);
		PolyDraw(hDC, pd.m_pPoint, pd.m_pFlag, pd.m_nCount);
	EndPath(hDC);

	SelectObject(hDC, GetStockObject(BLACK_BRUSH));
	FillPath(hDC);
}


void KMyCanvas::OnDraw(HDC hDC, const RECT * rcPaint)
{
	switch ( m_test )
	{
		case IDM_TEST_DRAWMODE:
			TestDrawMode(hDC);
			break;

		case IDM_TEST_WIDTHCAP:
			TestWidthCap(hDC);
			break;

		case IDM_TEST_DCPEN:
			TestDCPen(hDC);
			break;

		case IDM_TEST_PENSTYLE:
			TestPenStyle(hDC);
			break;

		case IDM_TEST_COSMETIC:
			TestCosmetic(hDC);
			break;

		case IDM_TEST_GEOMETRIC_WIDTH:
			TestGeometricWidth(hDC);
			break;

		case IDM_TEST_GEOMETRIC_STYLE:
			TestGeometricStyle(hDC);
			break;

		case IDM_TEST_LINE1:
			TestLine1(hDC);
			break;

		case IDM_TEST_LINE2:
			TestLine2(hDC);
			break;

		case IDM_TEST_BEZIER:
			TestBezier(hDC);
			break;

		case IDM_TEST_BEZIERDEMO:
			BezierDemo(hDC);
			break;

		case IDM_TEST_BEZIERPEN:
			TestBezierPen(hDC);
			break;

		case IDM_TEST_ARC:
			TestArc(hDC);
			break;

		case IDM_TEST_ARCBEZIER:
			ArcToBezier(hDC);
			break;

		case IDM_TEST_ANGLEARCBEZIER:
			AngleArcToBezier(hDC);
			break;

		case IDM_TEST_PATH:
			TestPath(hDC);
			break;

		case IDM_TEST_PATHSTYLECURVE:
			TestPathStyleCurve(hDC);
			break;

		case IDM_TEST_MAPPATH:
			TestMapPath(hDC);
	}
}

class KLogoFrame : public KFrame
{
	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KFrame::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_LINE));
	}

public:
	KLogoFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
		KToolbar * pToolbar, KCanvas * pCanvas, KStatusWindow * pStatus) :
			KFrame(hInstance, pButtons, nCount, pToolbar, pCanvas, pStatus)
	{
	}

};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KMyCanvas     canvas;
	KStatusWindow status;

	KLogoFrame frame(hInst, NULL, 0, NULL, & canvas, & status);

	frame.CreateEx(0, _T("LineCurveDemo"), _T("LineCurve"),
		WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}
