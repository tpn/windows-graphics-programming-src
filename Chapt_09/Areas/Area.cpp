//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : area.cpp						                                     //
//  Description: Area fill demo program, Chapter 9                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define WINVER 0x0500
#define WIN32_LEAN_AND_MEAN
#define OEMRESOURCE

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "..\..\include\win.h"
#include "..\..\include\Canvas.h"
#include "..\..\include\Status.h"
#include "..\..\include\FrameWnd.h"
#include "..\..\include\Affine.h"
#include "..\..\include\Pen.h"
#include "..\..\include\Color.h"
#include "..\..\include\GDIObject.h"
#include "..\..\include\areafill.h"
#include "..\..\include\button.h"
#include "..\..\include\region.h"

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

	void TestDither(HDC hDC);
	void TestHatch(HDC hDC);
	void TestSysColorBrush(HDC hDC);

	void TestRectangle(HDC hDC);
	void TestDrawFrameControl(HDC hDC);

	void TestEllipse(HDC hDC);
	void TestPieChart(HDC hDC);

	void TestPolyFillMode(HDC hDC);
	void TestFillPath(HDC hDC);

	void TestRegion(HDC hDC);
	void TestRegionOper(HDC hDC);
	void TestRegionPaint(HDC hDC);
	void TestGradientFill(HDC hDC);

	KRectButton    rbutton;
	KEllipseButton ebutton;

public:

	KMyCanvas()
	{

		m_test  = -1;
		m_lastx = -1;
		m_lasty = -1;
	}
};

void KMyCanvas::SetTest(int test)
{
	m_test = test;

	HMENU hMain = GetMenu(GetParent(m_hWnd));

	TCHAR Temp[MAX_PATH];

	_tcscpy(Temp, _T("Areas - "));

	GetMenuString(hMain, m_test, Temp + _tcslen(Temp), 
		sizeof(Temp)/sizeof(TCHAR) - _tcslen(Temp), MF_BYCOMMAND);

	SetWindowText(GetParent(m_hWnd), Temp);
}


BOOL KMyCanvas::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch ( LOWORD(wParam) )
	{
		case IDM_TEST_DITHER:
		case IDM_TEST_HATCHBRUSH:
		case IDM_TEST_SYSCOLORBRUSH:
		case IDM_TEST_RECTANGLE:
		case IDM_TEST_DRAWFRAMECONTROL:	
		case IDM_TEST_ELLIPSE:
		case IDM_TEST_PIECHART:
		case IDM_TEST_POLYFILLMODE:
		case IDM_TEST_FILLPATH:
		case IDM_TEST_REGION:
		case IDM_TEST_REGIONOP:
		case IDM_TEST_REGIONPAINT:
		case IDM_TEST_GRADIENTFILL:
			SetTest( LOWORD(wParam) );
			InvalidateRect(m_hWnd, NULL, TRUE);
			return TRUE;

		case IDM_FILE_EXIT:
			DestroyWindow(GetParent(m_hWnd));
			return TRUE;
	}

	return FALSE;	// not processed
}


inline void Line(HDC hDC, int x1, int y1, int x2, int y2)
{
	MoveToEx(hDC, x1, y1, NULL);
	LineTo(hDC, x2, y2);
}

void KMyCanvas::DrawCrossHair(HDC hDC, bool on)
{
	if ( (m_lastx<0) || (m_lasty<0) )
		return;

	RECT rect;

	GetClientRect(m_hWnd, & rect);

	SetROP2(hDC, R2_XORPEN);
	
	{
		KPen pen(PS_DOT, 0, RGB(0, 0, 0xFF), hDC);

		Line(hDC, rect.left, m_lasty, rect.right, m_lasty);
		Line(hDC, m_lastx, rect.top, m_lastx, rect.bottom);
	}
}


void ZoomRect(HDC hDC, int x0, int y0, int x1, int y1, int dx, int dy, int zoom)
{
	// use black pen for border
	HGDIOBJ hOld = SelectObject(hDC, GetStockObject(BLACK_PEN));

	for (int y=y0; y<y1; y++)
	for (int x=x0; x<x1; x++)
	{
		COLORREF c = GetPixel(hDC, x, y);

		HBRUSH  hBrush = CreateSolidBrush(c);
		HGDIOBJ hOld   = SelectObject(hDC, hBrush);

		Rectangle(hDC, dx+(x-x0)*(zoom+1), dy+(y-y0)*(zoom+1), 
			dx+(x-x0)*(zoom+1)+zoom+2, 
			dy+(y-y0)*(zoom+1)+zoom+2);
		SelectObject(hDC, hOld);
		DeleteObject(hBrush);
	}

	SelectObject(hDC, hOld);
}


void KMyCanvas::TestDither(HDC hDC)
{
	SetViewportOrgEx(hDC, 10, 10, NULL);

	// no outline for dither rectangle
	SelectObject(hDC, GetStockObject(NULL_PEN)); 

	for (int y=0; y<16; y++)
	for (int x=0; x<16; x++)
	{
		HBRUSH  hBrush = CreateSolidBrush(RGB(y*16+x, y*16+x, 0xFF));
		HGDIOBJ hOld   = SelectObject(hDC, hBrush);

		Rectangle(hDC, 235+x*10, y*10, 235+x*10+9, y*10+9);

		if ( x==y ) // zoom in color on diagonal line 
			ZoomRect(hDC, 235+x*10, y*10, 235+x*10+8, y*10+8, 
				80*(x%8), 180+80*(x/8), 6);
		
		SelectObject(hDC, hOld);
		DeleteObject(hBrush);
	}
	SelectObject(hDC, GetStockObject(BLACK_PEN));

	SetViewportOrgEx(hDC, 0, 0, NULL);
}

TCHAR * HS_Names [] = 
{	_T("HS_HORIZONTAL"),
	_T("HS_VERTICAL"),
	_T("HS_FDIAGONAL"),
	_T("HS_BDIAGONAL"),
	_T("HS_CROSS"),
	_T("HS_DIAGCROSS")
};


void Frame(HDC hDC, int x0, int y0, int x1, int y1)
{
	unsigned short ChessBoard[] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };

	HBITMAP hBitmap = CreateBitmap(8, 8, 1, 1, ChessBoard);
	HBRUSH hBrush = CreatePatternBrush(hBitmap);
	DeleteObject(hBitmap);

	HGDIOBJ hOld  = SelectObject(hDC, hBrush);

	// PS_ALTERNATE rectangle
	PatBlt(hDC, x0, y0, x1-x0, 1, PATCOPY);
	PatBlt(hDC, x0, y1, x1-x0, 1, PATCOPY);
	PatBlt(hDC, x0, y0, 1, y1-y0, PATCOPY);
	PatBlt(hDC, x1, y0, 1, y1-y0, PATCOPY);
	
	int old = SetROP2(hDC, R2_MASKPEN);
	Rectangle(hDC, x0+5, y0+5, x1-5, y1-5);
	SetROP2(hDC, old);

//	int old = SetROP2(hDC, R2_MASKPEN);
//	SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
//	SetTextColor(hDC, RGB(0, 0, 0));
//	Rectangle(hDC, x0+5, y0+5, x1-5, y1-5);

//	SetBkColor(hDC, RGB(0x0, 0x0, 0x0));
//	SetTextColor(hDC, RGB(0xFF, 0, 0));
//	SetROP2(hDC, R2_MERGEPEN);
//	Rectangle(hDC, x0+5, y0+5, x1-5, y1-5);

	SetROP2(hDC, old);

	SelectObject(hDC, hOld);
	DeleteObject(hBrush);
}

void SemiFillRect(HDC hDC, int left, int top, int right, int bottom, COLORREF color)
{
	int nSave = SaveDC(hDC);

	const unsigned short ChessBoard[] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };

	HBITMAP hBitmap = CreateBitmap(8, 8, 1, 1, ChessBoard);
	HBRUSH hBrush = CreatePatternBrush(hBitmap);
	DeleteObject(hBitmap);
	HGDIOBJ hOldBrush = SelectObject(hDC, hBrush);
	HGDIOBJ hOldPen   = SelectObject(hDC, GetStockObject(NULL_PEN));

	SetROP2(hDC, R2_MASKPEN);
	SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
	SetTextColor(hDC, RGB(0, 0, 0));
	Rectangle(hDC, left, top, right, bottom);

	SetROP2(hDC, R2_MERGEPEN);
	SetBkColor(hDC, RGB(0x0, 0x0, 0x0));
	SetTextColor(hDC, color);
	Rectangle(hDC, left, top, right, bottom);

	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);
	DeleteObject(hBrush);

	RestoreDC(hDC, nSave);
}

void KMyCanvas::TestHatch(HDC hDC)
{
	int nSave = SaveDC(hDC);

	SetViewportOrgEx(hDC, 24, 24, NULL);

	SelectObject(hDC, GetStockObject(NULL_PEN));
	
	typedef enum { GAP = 120 };

	for (int hs= HS_HORIZONTAL; hs<=HS_DIAGCROSS; hs++)
	{
		HBRUSH hBrush = CreateHatchBrush(hs, RGB(0, 0, 0xFF));
		HGDIOBJ hOld  = SelectObject(hDC, hBrush);

		SetBkColor(hDC, RGB(0xFF, 0xFF, 0));
		Rectangle(hDC, hs*GAP, 0, hs*GAP+89, 89);

		SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
		
		SetTextAlign(hDC, TA_CENTER);
		TextOut(hDC, hs*GAP+88/2, 98, HS_Names[hs-HS_HORIZONTAL], _tcslen(HS_Names[hs-HS_HORIZONTAL]));

		SelectObject(hDC, hOld);
		DeleteObject(hBrush);

		ZoomRect(hDC, hs*GAP, 0, hs*GAP+8, 8, hs*GAP, 120, 8);
	}

/*	SelectObject(hDC, GetStockObject(NULL_PEN));

	SetBkColor(hDC, RGB(0xFF, 0xFF, 0));

	for (int i=0; i<8; i++)
	{
		SetViewportOrgEx(hDC, 24+i*32+i, 300, NULL);

		HBRUSH  hBrush = CreateHatchBrush(HS_FDIAGONAL, RGB(0, 0, 0xFF));
		HGDIOBJ hOld   = SelectObject(hDC, hBrush);
	
		Rectangle(hDC, 0, 0, 32+2, 32+2);

		SelectObject(hDC, hOld);
		DeleteObject(hBrush);
	}

	SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
*/

	// Pattern Brushes Demo
	HINSTANCE hCards = LoadLibrary("cards.dll");

	for (int i=0; i<4; i++)
	{
		HBRUSH hBrush;
		int    width, height;

		switch ( i )
		{
			case 0:
				{
					HBITMAP hBitmap = LoadBitmap(hCards, MAKEINTRESOURCE(52));
					BITMAP  bmp;

					GetObject(hBitmap, sizeof(bmp), & bmp);
					width = bmp.bmWidth; height = bmp.bmHeight;

					hBrush = CreatePatternBrush(hBitmap);
					DeleteObject(hBitmap);
				}
				break;

			case 1:
				{
					HRSRC hResource = FindResource(hCards, MAKEINTRESOURCE(52-14), RT_BITMAP);
					HGLOBAL hGlobal = LoadResource(hCards, hResource);
	
					hBrush  = CreateDIBPatternBrushPt(LockResource(hGlobal), DIB_RGB_COLORS);
					width   = ((BITMAPCOREHEADER *) hGlobal)->bcWidth; // old DIB format
					height  = ((BITMAPCOREHEADER *) hGlobal)->bcHeight;// old DIB format
				}
				break;

			case 2:
			case 3:
				{
					HRSRC hResource = FindResource(hCards, MAKEINTRESOURCE(52-28), RT_BITMAP);
					HGLOBAL hGlobal = LoadResource(hCards, hResource);

					hBrush  = CreateDIBPatternBrush(hGlobal, DIB_RGB_COLORS);
					width   = ((BITMAPCOREHEADER *) hGlobal)->bcWidth;  // old DIB format
					height  = ((BITMAPCOREHEADER *) hGlobal)->bcHeight; // old DIB format
				}
		}
		
		HGDIOBJ hOld  = SelectObject(hDC, hBrush);

		POINT P = { i*140+20 + width*i/4, 250 + height*i/4 };
		LPtoDP(hDC, &P, 1);
		SetBrushOrgEx(hDC, P.x, P.y, NULL); // make sure cards aligned with rectangle
	
		Rectangle(hDC, i*140+20, 250, i*140+20+width*3/2+1, 250+height*3/2+1);
	
		SelectObject(hDC, hOld);
		DeleteObject(hBrush);

		if ( i==3 )
		{
			Frame(hDC, i*140+20-10, 250-10, i*140+20+width*3/2+10, 250+height*3/2+10);

//			SemiFillRect(hDC, i*140+20-10, 250-10, i*140+20+width*3/2+10, 250+height*3/2+10, RGB(0, 0, 0xFF));

			ZoomRect(hDC, i*140+20-10, 250-10, i*140+20+20, 250+20,
						  580, 240, 6);
		}			
	}

	RestoreDC(hDC, nSave);
}


LRESULT KMyCanvas::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr;

	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd = hWnd;
			SetTest(IDM_TEST_DITHER);

			rbutton.DefineButton(10, 10, 50, 50);
			ebutton.DefineButton(70, 10, 50, 50);
			return 0;

		case WM_PAINT:
			{
				PAINTSTRUCT ps; 

				HDC hDC = BeginPaint(m_hWnd, &ps);

				OnDraw(hDC, &ps.rcPaint);

				if ( m_test== IDM_TEST_REGIONOP )
				{
					rbutton.DrawButton(hDC);
					ebutton.DrawButton(hDC);
				}

				DrawCrossHair(hDC, true);
				
				EndPaint(m_hWnd, &ps);
			}
			return 0;

		case WM_MOUSEMOVE:
			{
				HDC hDC = GetDC(hWnd);

				DrawCrossHair(hDC, false);	// erase

				m_lastx = LOWORD(lParam); m_lasty = HIWORD(lParam);

				if ( m_test==IDM_TEST_REGIONOP )
				{
					rbutton.UpdateButton(hDC, lParam);
					ebutton.UpdateButton(hDC, lParam);
				}

				DrawCrossHair(hDC, true);	// restore
				ReleaseDC(hWnd, hDC);
				
				TCHAR temp[32];
				wsprintf(temp, _T("(%d, %d)"), m_lastx, m_lasty);
				m_pStatus->SetText(0, temp);
		
			}
			return 0;
		
		case WM_LBUTTONDOWN:
			{
				if ( m_test==IDM_TEST_REGIONOP )
				{
					if ( rbutton.IsOnButton(lParam) )
						MessageBox(hWnd, "Rectangle Button Clicked", "PtInRegion", MB_OK);

					if ( ebutton.IsOnButton(lParam) )
						MessageBox(hWnd, "Ellipse Button Clicked", "PtInRegion", MB_OK);
				}
			}
			return 0;

		default:
			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return lr;
}


const TCHAR * SysColorName[] = 
{
	"COLOR_SCROLLBAR",
	"COLOR_BACKGROUND",
	"COLOR_ACTIVECAPTION",
	"COLOR_INACTIVECAPTION",
	"COLOR_MENU",
	"COLOR_WINDOW",
	"COLOR_WINDOWFRAME",
	"COLOR_MENUTEXT",
	"COLOR_WINDOWTEXT",
	"COLOR_CAPTIONTEXT",
	"COLOR_ACTIVEBORDER",
	"COLOR_INACTIVEBORDER",
	"COLOR_APPWORKSPACE",
	"COLOR_HIGHLIGHT",
	"COLOR_HIGHLIGHTTEXT",
	"COLOR_BTNFACE",
	"COLOR_BTNSHADOW",
	"COLOR_GRAYTEXT",
	"COLOR_BTNTEXT",
	"COLOR_INACTIVECAPTIONTEXT",
	"COLOR_BTNHIGHLIGHT",
	"COLOR_3DDKSHADOW",
	"COLOR_3DLIGHT",
	"COLOR_INFOTEXT",
	"COLOR_INFOBK",
	"??",
	"COLOR_HOTLIGHT",
	"COLOR_GRADIENTACTIVECAPTION",
	"COLOR_GRADIENTINACTIVECAPTION"
};


void KMyCanvas::TestSysColorBrush(HDC hDC)
{
	for (int idx=0; GetSysColorBrush(idx); idx++)
	{
		TCHAR temp[64];

		int y = idx*18 + 20;

		SelectObject(hDC, GetSysColorBrush(idx));

		TextOut(hDC, 20, y, SysColorName[idx], _tcslen(SysColorName[idx]));

		wsprintf(temp, "%2d %08x", idx, GetSysColorBrush(idx));
		TextOut(hDC, 280, y, temp, _tcslen(temp));

		COLORREF cr = GetSysColor(idx);
		wsprintf(temp, "RGB(0x%02x, 0x%02x, 0x%02x)", GetRValue(cr), GetGValue(cr), GetBValue(cr));
		TextOut(hDC, 540, y, temp, _tcslen(temp));

		Rectangle(hDC, 420, y, 520, y+15);

		BOOL rslt = DeleteObject(GetSysColorBrush(idx));
	}
}


void Mark(HDC hDC, int x, int y, int dx, int dy, const char * yl, const TCHAR * xl)
{
	HGDIOBJ hPen = CreatePen(PS_SOLID, 1, RGB(0xFF, 0, 0));
	HGDIOBJ hOld = SelectObject(hDC, hPen);

	TextOut(hDC, x+dx,y,      yl, _tcslen(yl));
	TextOut(hDC, x+2, y+dy-8, xl, _tcslen(xl));

	MoveToEx(hDC, x+dx, y, NULL); LineTo(hDC, x, y); LineTo(hDC, x, y+dy);
	
	SelectObject(hDC, hOld);
	DeleteObject(hPen);
}


void KMyCanvas::TestRectangle(HDC hDC)
{
	HBRUSH hBlue = CreateSolidBrush(RGB(0, 0, 0xFF));

	SelectObject(hDC, hBlue);
	
	for (int i=0; i<6; i++)
	{
		SetViewportOrgEx(hDC, (i%3)*180, (i/3)*180, NULL);

		const TCHAR * Title []= 
		{ 
			_T("black pen"), 
			_T("null pen"), 
			_T("3-pixel pen"), 
			_T("black pen, advanced"), 
			_T("3-pixel inside frame pen"), 
			_T("red pen, R2_NOTCOPYPEN")
		};

		HGDIOBJ hPen;
		
		switch (i)
		{
			case 0: hPen = GetStockObject(BLACK_PEN);
					break;
			case 1: hPen = GetStockObject(NULL_PEN);
					break;
			case 2: hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
					break;
			case 3: hPen = GetStockObject(BLACK_PEN); SetGraphicsMode(hDC, GM_ADVANCED);
				    break;
			case 4: hPen = CreatePen(PS_INSIDEFRAME, 3, RGB(0, 0, 0));
					break;
			case 5: hPen = CreatePen(PS_SOLID, 1, RGB(0xFF, 0x0, 0x0));
				    SetROP2(hDC, R2_NOTCOPYPEN);
		}

		HGDIOBJ hOld = SelectObject(hDC, hPen);
		Rectangle(hDC, 60, 60, 50, 50);
		SelectObject(hDC, hOld);
		SetGraphicsMode(hDC, GM_COMPATIBLE);
		DeleteObject(hPen);
		SetROP2(hDC, R2_COPYPEN);

		ZoomRect(hDC, 48, 48, 62, 62, 80, 50, 7);
		TextOut(hDC, 50, 180, Title[i], _tcslen(Title[i]));

		Mark(hDC, 80+ 2*8, 50+ 2*8, -25, -25, "T", "L");
		Mark(hDC, 80+12*8, 50+12*8,  25,  25, "B", "R");
	}

	////////////////////
	for (i=0; i<4; i++)
	{
		SetViewportOrgEx(hDC, (i%4)*180, 360+(i/4)*180, NULL);

		const TCHAR * Title []= 
		{ 
			_T("FillRect"), 
			_T("FrameRect"), 
			_T("InvertRect"), 
			_T("DrawFocusRect")
		};
	
		RECT rect = { 50, 50, 60, 60 };

	//	InflateRect(&rect, 2, 2);
	//	HBRUSH hBlue = CreateSolidBrush(RGB(0, 0, 0xFF));
	//	FillRect(hDC, & rect, hBlue);
	//	DeleteObject(hBlue);
	//	InflateRect(&rect, -2, -2);

		switch (i)
		{
			case 0:
				FillRect(hDC, & rect, hBlue);
				break;

			case 1:
				FrameRect(hDC, & rect, hBlue);
				break;

			case 2:
				InvertRect(hDC, & rect);
				break;

			case 3:
				DrawFocusRect(hDC, & rect);
				break;
		}

		ZoomRect(hDC, 48, 48, 62, 62, 80, 50, 7);
		TextOut(hDC, 50, 180, Title[i], _tcslen(Title[i]));

		Mark(hDC, 80+ 2*8, 50+ 2*8, -25, -25, "T", "L");
		Mark(hDC, 80+12*8, 50+12*8,  25,  25, "B", "R");

	}

	SelectObject(hDC, GetStockObject(WHITE_BRUSH));
	DeleteObject(hBlue);

	SetViewportOrgEx(hDC, 0, 0, NULL);
}


void DrawControls(HDC hDC, int typ, const int state[], int no)
{
	for (int s=0; s<no; s++)
	{
		int x =    s * 56 + 20;
		int y = typ  * 56 - 40;

		RECT rect = { x, y, x+40, y+40 };

		InflateRect(&rect, 3, 3);
		FillRect(hDC, & rect, GetSysColorBrush(COLOR_BTNFACE));
		InflateRect(&rect, -3, -3);

		DrawFrameControl(hDC, & rect, typ, state[s]);
	}
}

void KMyCanvas::TestDrawFrameControl(HDC hDC)
{
	////////////////////////
	const int CaptionState[] = { DFCS_CAPTIONCLOSE,
								 DFCS_CAPTIONMIN,
								 DFCS_CAPTIONMAX,
								 DFCS_CAPTIONRESTORE,
								 DFCS_CAPTIONHELP,
								 DFCS_CAPTIONHELP | DFCS_FLAT,
								 DFCS_CAPTIONHELP | DFCS_INACTIVE
	};

	DrawControls(hDC, DFC_CAPTION, CaptionState, sizeof(CaptionState)/sizeof(int));

	////////////////////////
	const int ButtonState[] = { DFCS_BUTTONCHECK, 
								DFCS_BUTTONRADIOIMAGE,
							    DFCS_BUTTONRADIOMASK, 
								DFCS_BUTTONRADIO,
								DFCS_BUTTON3STATE,
								DFCS_BUTTONPUSH,
								DFCS_BUTTONPUSH | DFCS_FLAT,
								DFCS_BUTTONPUSH | DFCS_INACTIVE

	}; 	

	DrawControls(hDC, DFC_BUTTON, ButtonState, sizeof(ButtonState)/sizeof(int));

	////////////////////////
	const int MenuState[] = { DFCS_MENUARROW,
							  DFCS_MENUCHECK,
							  DFCS_MENUBULLET,
							  DFCS_MENUARROWRIGHT,
							  DFCS_MENUARROWRIGHT | DFCS_FLAT,
							  DFCS_MENUARROWRIGHT | DFCS_INACTIVE
	};
	DrawControls(hDC, DFC_MENU, MenuState, sizeof(MenuState)/sizeof(int));

	////////////////////////
	const int ScrollState[] = { DFCS_SCROLLUP,
								DFCS_SCROLLDOWN,
								DFCS_SCROLLLEFT,
								DFCS_SCROLLRIGHT,
								DFCS_SCROLLCOMBOBOX,
								DFCS_SCROLLSIZEGRIP,
								DFCS_SCROLLSIZEGRIPRIGHT,
								DFCS_SCROLLSIZEGRIPRIGHT | DFCS_FLAT,
								DFCS_SCROLLSIZEGRIPRIGHT | DFCS_INACTIVE
	};
	
	DrawControls(hDC, DFC_SCROLL, ScrollState, sizeof(ScrollState)/sizeof(int));

	/////////////////////////
	const int PopupMenuState[] = { 0,
							  1,
							  2,
							  3,
							  3 | DFCS_FLAT,
							  3 | DFCS_INACTIVE
	};

	DrawControls(hDC, DFC_POPUPMENU, PopupMenuState, sizeof(PopupMenuState)/sizeof(int));

	//////////////////////////
	SetViewportOrgEx(hDC, 0, 280, NULL);

	for (int e=0; e<4; e++)
	{
		int Edge[] = { EDGE_RAISED, EDGE_SUNKEN, EDGE_ETCHED, EDGE_BUMP };
		int Flag[] = { BF_MIDDLE | BF_BOTTOM, 
					   BF_MIDDLE | BF_BOTTOMLEFT, 
					   BF_MIDDLE | BF_BOTTOMLEFT | BF_TOP,
					   BF_MIDDLE | BF_RECT, 
					   BF_MIDDLE | BF_RECT | BF_FLAT, 
					   BF_MIDDLE | BF_RECT | BF_MONO, 
					   BF_MIDDLE | BF_RECT | BF_SOFT, 
					   BF_MIDDLE | BF_RECT | BF_DIAGONAL, 
					   BF_MIDDLE | BF_RECT | BF_ADJUST };

		for (int f=0; f<sizeof(Flag)/sizeof(Flag[0]); f++)
		{
			RECT rect = { f*56+20, e*56 + 20, f*56+60, e*56+60 };

			InflateRect(&rect, 3, 3);   // bigger background
			FillRect(hDC, & rect, GetSysColorBrush(COLOR_BTNFACE));
			InflateRect(&rect, -3, -3); // restore size

			DrawEdge(hDC, & rect, Edge[e], Flag[f]);
		}
	}

	SetViewportOrgEx(hDC, 0, 0, NULL);
}


void Label(HDC hDC, int x, int y, const TCHAR * mess)
{
	TextOut(hDC, x, y, mess, _tcslen(mess));
}


void KMyCanvas::TestEllipse(HDC hDC)
{
	int nSave = SaveDC(hDC);

	HGDIOBJ hBrush = CreateSolidBrush(RGB(0xFF, 0xFF, 0));
	SelectObject(hDC, hBrush);

	// Zoomed in display shows the dimension of ellipse
	
	for (int i=0; i<=6; i+=3)
	{
		SetViewportOrgEx(hDC, i*200/3, 0, NULL);

		HGDIOBJ hPen = CreatePen(PS_INSIDEFRAME, i/3, RGB(0,0,0xFF));
		SelectObject(hDC, hPen);

		if (i==3)
			SetGraphicsMode(hDC, GM_ADVANCED);

		Ellipse(hDC, 30, 30, 40+i, 40+i);
		SetGraphicsMode(hDC, GM_COMPATIBLE);

		SelectObject(hDC, GetStockObject(BLACK_PEN));
		DeleteObject(hPen);

		ZoomRect(hDC, 28, 28, 42+i, 42+i, 70, 40, 7);
	
		Mark(hDC, 70+ 2*8,     40+ 2*8,     -30, -30, "Top", "Left");
		Mark(hDC, 70+12*8+i*8, 40+12*8+i*8,  30,  30, "Bottom", "Right");

		switch (i)
		{
			case 0: Label(hDC, 70, 230, "Solid pen"); break;
			case 3: Label(hDC, 70, 230, "Solid pen, advanced mode"); break;
			case 6: Label(hDC, 70, 230, "Inside frame pen"); break;
		}
	}


	const int A = 100;
	const int B = 100;
	
	POINT P[5] = { -A, -B, A, -B, A, B, -A, B, -A, -B };
	POINT S    = {  A*3/2,    0 };
	POINT E    = {  -A*4/3, - B*3/4 };

	for (i=0; i<3; i++)
	{
		if ( i==0 )
		{
			SetViewportOrgEx(hDC, 160, 420, 0);
			SetArcDirection(hDC, AD_COUNTERCLOCKWISE);
			Label(hDC, -65, P[0].y-60, "Chord, Anticlockwise");
		}
		else
		{
			SetViewportOrgEx(hDC, 160 + i* A * 3, 420, 0);
			SetArcDirection(hDC, AD_CLOCKWISE);

			if ( i==1)
				Label(hDC, -55, P[0].y-60, "Pie, Clockwise");
			else if (i==2)
			{
				Label(hDC, -30, P[0].y-60, "Ellipse");
			}
		}
	
		// defining lines
		{
			HGDIOBJ hOldBrush = SelectObject(hDC, GetStockObject(HOLLOW_BRUSH));

			HPEN hPen = CreatePen(PS_DOT, 0, RGB(0xFF, 0, 0)); // dotted line
			HPEN hOld = (HPEN) SelectObject(hDC, hPen);

			// bounding rectangle, start/end lines
			Rectangle(hDC, P[0].x, P[0].y, P[2].x, P[2].y);
			Ellipse(hDC, P[0].x, P[0].y, P[2].x, P[2].y);

			if ( i!=2 )
			{
				Line(hDC, 0, 0, S.x, S.y); 
				Line(hDC, 0, 0, E.x, E.y);
			}

			SelectObject(hDC, hOld);
			DeleteObject(hPen);

			SelectObject(hDC, hOldBrush);
		}

		LOGBRUSH logbrush = { BS_SOLID, RGB(0, 0, 0xFF), 0 };

		HPEN hPen;
		
		switch ( i )
		{
			case 0:	
				hPen = ExtCreatePen(PS_COSMETIC | PS_ALTERNATE, 1, & logbrush, 0, NULL); 
				break;
			
			case 1:	
				hPen = ExtCreatePen(PS_GEOMETRIC | PS_INSIDEFRAME | PS_JOIN_MITER, 10, & logbrush, 0, NULL); 
				break;

			case 2:	
				logbrush.lbStyle = BS_HATCHED;
				logbrush.lbHatch = HS_DIAGCROSS;
				hPen = ExtCreatePen(PS_GEOMETRIC | PS_INSIDEFRAME, 10, & logbrush, 0, NULL); 
				break;
		}

		HPEN hOld = (HPEN) SelectObject(hDC, hPen);

		switch ( i )
		{
			case 0:
				Chord(hDC, P[0].x, P[0].y, P[2].x, P[2].y, S.x, S.y, E.x, E.y);
				break;

			case 1:
				Pie(hDC, P[0].x, P[0].y, P[2].x, P[2].y, S.x, S.y, E.x, E.y);
				break;

			case 2:
				{
					HINSTANCE hCards = LoadLibrary("cards.dll");

					SetBkColor(hDC, RGB(0xFF, 0xFF, 0));
					SetBkMode(hDC, OPAQUE);
					HBITMAP hBitmap = LoadBitmap(hCards, MAKEINTRESOURCE(52));

					hBrush = CreatePatternBrush(hBitmap);
					DeleteObject(hBitmap);
	
					HGDIOBJ hOld = SelectObject(hDC, hBrush);

					Ellipse(hDC, P[0].x, P[0].y, P[2].x, P[2].y);
					SelectObject(hDC, hOld);
					DeleteObject(hBrush);

					SetBkMode(hDC, TRANSPARENT);
					break;
				}
		}

		SelectObject(hDC, hOld);
		DeleteObject(hPen);
	
		Label(hDC, P[0].x, P[0].y-20,  "(Left, Top)");
		Label(hDC, P[2].x-100, P[2].y+5, "(Bottom, Right)");

		if ( i!=2 )
		{
			Label(hDC, S.x-40, S.y-20, "(xStart, yStart)");
			Label(hDC, E.x-20, E.y-15,  "(xEnd, yEnd)");
		}
	}

	
	SelectObject(hDC, GetStockObject(WHITE_BRUSH));
	DeleteObject(hBrush);

	SetViewportOrgEx(hDC, 0, 0, NULL);

	RestoreDC(hDC, nSave);
}


void DrawPieChart(HDC hDC, int x0, int y0, int x1, int y1, 
				  double data[], COLORREF color[], int count)
{
	double sum = 0;
	
	for (int i=0; i<count; i++)
		sum += data[i];

	double angle = 0;
	
	for (i=0; i<count; i++)
	{
		double a = data[i] * 2 * 3.14159265358 / sum;

		HBRUSH hBrush = CreateSolidBrush(color[i]);
		HGDIOBJ hOld  = SelectObject(hDC, hBrush);

		Pie(hDC, x0, y0, x1, y1, 
			  (int) ((x1-x0) * cos(angle)),  
			- (int) ((y1-y0) * sin(angle)),
			  (int) ((x1-x0) * cos(angle+a)), 
			- (int) ((y1-y0) * sin(angle+a)));

		angle += a;

		SelectObject(hDC, hOld);
		DeleteObject(hBrush);
	}
}


void KMyCanvas::TestPieChart(HDC hDC)
{
	// Pie Chart
	double data[] = { 10, 20, 144, 50, 15 };
	COLORREF color[] = { RGB(0xFF, 0, 0), RGB(0xFF, 0xFF, 0 ),
		                 RGB(0xFF, 0, 0xFF), RGB(0, 0xFF, 0xFF),
						 RGB(0, 0xFF, 0) };

	SetViewportOrgEx(hDC, 200, 150, NULL);

	DrawPieChart(hDC, -100, -50, 100, 50, data, color, 5);

	// Rounded Rectangle
	SetViewportOrgEx(hDC, 450, 150, NULL);

	for (int i=0; i<10; i++)
		RoundRect(hDC, -100+i*10, -100+i*10, 100-i*10, 100-i*10, i*10, i*20);

	//// RoundRect Definition ////////////////////////

	SetViewportOrgEx(hDC, 300, 400, NULL);

	const int A = 150;
	const int B = 100;
	
	POINT P[5] = { -A, -B, A, -B, A, B, -A, B, -A, -B };

	int ew = A * 2/3;
	int eh = B * 2/3;

	// defining lines
	{
		LOGBRUSH logbrush = { BS_SOLID, RGB(0xFF, 0, 0), 0 };

		KGDIObject brush(hDC, GetStockObject(HOLLOW_BRUSH));
		KGDIObject attach(hDC, ExtCreatePen(PS_COSMETIC | PS_ALTERNATE, 1, & logbrush, 0, NULL));

		// bounding rectangle, start/end lines
		Rectangle(hDC, P[0].x, P[0].y, P[2].x, P[2].y);
	
		// paint the area first, no pen
		{
			KGDIObject brush(hDC, CreateSolidBrush(RGB(0xFF, 0xFF, 0)));
			KGDIObject pen  (hDC, GetStockObject(NULL_PEN));

			RoundRect(hDC, P[0].x, P[0].y, P[2].x, P[2].y, ew, eh);
		}

		// Four ellipse on corners
		Ellipse(hDC, P[0].x, P[0].y, P[0].x+ew, P[0].y+eh);
		Ellipse(hDC, P[0].x, P[2].y, P[0].x+ew, P[2].y-eh);
		Ellipse(hDC, P[2].x, P[0].y, P[2].x-ew, P[0].y+eh);
		Ellipse(hDC, P[2].x, P[2].y, P[2].x-ew, P[2].y-eh);

		MoveToEx(hDC, P[0].x + ew-1, P[0].y-20, NULL);
		LineTo(hDC,   P[0].x + ew-1, P[0].y+eh/2);
		Label(hDC, P[0].x + ew+2, P[0].y-20, "nLeft + nWidth");

		MoveToEx(hDC, P[0].x + ew/2, P[0].y + eh-1, NULL);
		LineTo(hDC, P[0].x - 20,   P[0].y + eh-1);
		Label(hDC, P[0].x - 120, P[0].y + eh, "nTop + nHeight");
	}

	// draw the outline, no brush
	{
		LOGBRUSH logbrush = { BS_SOLID, RGB(0, 0, 0xFF), 0 };

		KGDIObject brush(hDC, GetStockObject(NULL_BRUSH));
		KGDIObject attach(hDC, ExtCreatePen(PS_GEOMETRIC | PS_DOT, 5, & logbrush, 0, NULL));

		RoundRect(hDC, P[0].x, P[0].y, P[2].x, P[2].y, ew, eh);
	}

	Label(hDC, P[0].x, P[0].y-20,  "(nLeft, nTop)");
	Label(hDC, P[2].x-100, P[2].y+5, "(nBottom, nRight)");

	SetViewportOrgEx(hDC, 0, 0, NULL);
}


void KMyCanvas::TestPolyFillMode(HDC hDC)
{
	int nSave = SaveDC(hDC);

	LOGBRUSH logbrush = { BS_SOLID, RGB(0, 0, 0xFF), 0 };

	KGDIObject brush(hDC, CreateSolidBrush(RGB(0xFF, 0xFF, 0)));
	
	const POINT P2[] = { { 0, 0 }, { 0, 15 }, { 20, 15 } };

	for (int i=0; i<2; i++)
	{
		SetViewportOrgEx(hDC, 50+350*i, 50, NULL);
		
		{
			KGDIObject pen(hDC, ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_JOIN_MITER, 5, & logbrush, 0, NULL));
	
			if (i==1)
				SetGraphicsMode(hDC, GM_ADVANCED);

			Polygon(hDC, P2, 3);
			SetGraphicsMode(hDC, GM_COMPATIBLE);
		}

		{
			logbrush.lbColor = RGB(0xFF, 0xFF, 0);
			KGDIObject pen (hDC, ExtCreatePen(PS_COSMETIC | PS_SOLID, 1, & logbrush, 0, NULL));
			logbrush.lbColor = RGB(0, 0, 0xFF);
			
			KGDIObject brush(hDC, GetStockObject(NULL_BRUSH));

			Polygon(hDC, P2, 3);
		}

		ZoomRect(hDC, -5, -5, 20+8, 15+5, 45, -10, 7);
	}

	// Poly Fill Mode
	for (int t=0; t<2; t++)
	{
		logbrush.lbColor = RGB(0, 0, 0xFF);
		KGDIObject pen (hDC, ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_JOIN_MITER, 3, & logbrush, 0, NULL));

		if ( t==0 )
			SetPolyFillMode(hDC, ALTERNATE);
		else
			SetPolyFillMode(hDC, WINDING);

		for (int m=0; m<4; m++)
		{
			SetViewportOrgEx(hDC, 120+m*220, 350+t*220, NULL);
			
			const int s0[] = {  4, 4, 100, 0, 100, 1, 100, 2, 100, 3 };
			const int s1[] = {  8, 8, 100, 0, 100, 3, 100, 6, 100, 1, 
				                      100, 4, 100, 7, 100, 2, 100, 5 };
			const int s2[] = { 10, 5, 100, 0, 100, 1, 100, 2, 100, 3, 100, 4,  
				                       50, 0,  50, 1,  50, 2,  50, 3,  50, 4 };
			const int s3[] = { 10, 5, 100, 0, 100, 1, 100, 2, 100, 3, 100, 4,
				                       50, 4,  50, 3,  50, 2,  50, 1,  50, 0 };

			const int * spec[] = { s0, s1, s2, s3 };

			POINT P[10];

			int n = spec[m][0];			// number of points
			int d = spec[m][1];			// number of vertex for each polygon
			const int * s = spec[m]+2;	// radius, vertex index

			for (i=0; i<n; i++)
			{
				P[i].x = (int) ( s[i*2] * cos(s[i*2+1] * 2 * 3.1415927 / d) );
				P[i].y = (int) ( s[i*2] * sin(s[i*2+1] * 2 * 3.1415927 / d) );
			}

			if ( m<2 )
				Polygon(hDC, P, n);
			else
			{
				int V[2] = { 5, 5 };
				PolyPolygon(hDC, P, V, 2);
			}
		}
	}
/*
	CObject font(hDC, CreateFont(200, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS, 
		CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH, "MingLiu"));

	WCHAR Feng[] = { 'F', 0x5CF0 };

	SetBkMode(hDC, TRANSPARENT);

	BeginPath(hDC);
	TextOutW(hDC, 50, 150, Feng, 2);
	EndPath(hDC);

//	SetPolyFillMode(hDC, WINDING);

	StrokeAndFillPath(hDC);

	SetViewportOrgEx(hDC, 0, 0, NULL);
*/
	RestoreDC(hDC, nSave);
}


void KMyCanvas::TestFillPath(HDC hDC)
{
	const int nPoint = 26;
	POINT Point[nPoint];
	BYTE  Type[nPoint];

	// construction a path with two ellipse
	BeginPath(hDC);
	Ellipse(hDC, -100, -40, 100, 40);
	Ellipse(hDC, -40, -100, 40, 100);
	EndPath(hDC);

	// retrieve path data and rotate 45 degrees
	GetPath(hDC, Point, Type, nPoint);

	for (int i=0; i<nPoint; i++)
	{
		double x = Point[i].x * 0.707;
		double y = Point[i].y * 0.707;
		
		Point[i].x = (int) (x - y);
		Point[i].y = (int) (x + y);
	}

	KGDIObject brush(hDC, CreateSolidBrush(RGB(0xFF, 0xFF, 0)));
	KGDIObject pen  (hDC, CreatePen(PS_SOLID, 19, RGB(0, 0, 0xFF)));

	for (int t=0; t<8; t++)
	{
		SetViewportOrgEx(hDC, 120+(t%4)*180, 120+(t/4)*180, NULL);

		// constructe the rotated ellipses
		BeginPath(hDC);
		PolyDraw(hDC, Point, Type, nPoint);
		EndPath(hDC);

		if ( t>=4 )
			SetPolyFillMode(hDC, ALTERNATE);
		else
			SetPolyFillMode(hDC, WINDING);

		switch ( t % 4 )
		{
			case 0: FillPath(hDC);			break;
			case 1: StrokeAndFillPath(hDC); break;
			case 2: WidenPath(hDC); FillPath(hDC); break;
			case 3: WidenPath(hDC); 
					{
						KGDIObject thin(hDC, CreatePen(PS_SOLID, 3, RGB(0, 0, 0xFF)));
						StrokeAndFillPath(hDC); 
					}
		}
	}

	SetViewportOrgEx(hDC, 0, 0, NULL);
}



void KMyCanvas::TestRegion(HDC hDC)
{
	KGDIObject brush(hDC, CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF)));
	KGDIObject pen  (hDC, CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF)));

	HBRUSH red  = CreateSolidBrush(RGB(0xFF, 0xFF, 0));
	HBRUSH blue = CreateSolidBrush(RGB(0, 0, 0xFF));
	
	for (int i=0; i<6; i++)
	{
		SetViewportOrgEx(hDC, 150+120*i, 50, NULL);

		if ( i & 1 )
		{
			SetGraphicsMode(hDC, GM_ADVANCED);
			Label(hDC, -40, 300, _T("GM_ADVANCED"));
		}
		else
			Label(hDC, -45, 300, _T("GM_COMPATIBLE"));

		// Original Drawing Function
		switch ( i )
		{
			case 0:
			case 1: Rectangle(hDC, 0, 0, 20, 20); break;
			case 2: 
			case 3:	RoundRect(hDC, 0, 0, 20, 20, 10, 10); break;
			case 4:
			case 5:	Ellipse(hDC, 0, 0, 20, 20); break;
		}

		// Region
		HRGN hRgn = NULL;
		
		switch ( i )
		{
			case 0:
			case 1: hRgn = CreateRectRgn(20, 10, 0, 30);     break;
			case 2:
			case 3: hRgn = CreateRoundRectRgn(20, 10, 0, 30, 10, 10); break;
			case 4:
			case 5: hRgn = CreateEllipticRgn(20, 10, 0, 30); break;
		}
		
		PaintRgn(hDC, hRgn);
		FrameRgn(hDC, hRgn, red, 1, 1);

		DeleteObject(hRgn);

		// Path
		BeginPath(hDC);
		switch ( i )
		{
			case 0:
			case 1: Rectangle(hDC, 0, 20, 20, 40); break;
			case 2: 
			case 3:	RoundRect(hDC, 0, 20, 20, 40, 10, 10); break;
			case 4:
			case 5:	Ellipse(hDC, 0, 20, 20, 40); break;
		}
		EndPath(hDC);
		StrokeAndFillPath(hDC);

		// PathToRegion
		BeginPath(hDC);
		switch ( i )
		{
			case 0:
			case 1: Rectangle(hDC, 0, 30, 20, 50); break;
			case 2: 
			case 3:	RoundRect(hDC, 0, 30, 20, 50, 10, 10); break;
			case 4:
			case 5:	Ellipse(hDC, 0, 30, 20, 50); break;
		}
		EndPath(hDC);
		hRgn = PathToRegion(hDC);
		SetViewportOrgEx(hDC, 0, 0, NULL);
		PaintRgn(hDC, hRgn);
		FrameRgn(hDC, hRgn, red, 1, 1);
		SetViewportOrgEx(hDC, 150+120*i, 50, NULL);
		DeleteObject(hRgn);

		SetGraphicsMode(hDC, GM_COMPATIBLE);

		ZoomRect(hDC, -2, -2, 22, 52, -40, 55, 3);
	}

	SetViewportOrgEx(hDC, 0, 0, NULL);
	
	Label(hDC, 10, 50 +70, "Original API");
	Label(hDC, 10, 100+70, "Region");
	Label(hDC, 10, 150+70, "Path");
	Label(hDC, 10, 200+70, "PathToRgn");

	// Compare Polygon and CreatePolygonRgn
	{
		SetViewportOrgEx(hDC, 50, 400, NULL);

		POINT P[4] = { 0, 0, 0, 20, 30, 20, 30, 0 };
		Polygon(hDC, P, 4);

		POINT Q[4] = { 0, 25, 0, 45, 30, 45, 30, 25 };

		HRGN hRgn = CreatePolygonRgn(Q, 4, ALTERNATE);
		PaintRgn(hDC, hRgn);
		FrameRgn(hDC, hRgn, red, 1, 1);
		DeleteObject(hRgn);

		ZoomRect(hDC, -2, -2, 32, 22, 50, 0, 3);
		ZoomRect(hDC, -2, 25-2, 32, 25+22, 200, 0, 3);
	}

	// Compare PathToRgn
	{
		SetViewportOrgEx(hDC, 420, 400, NULL);

		//////////////// Path
		BeginPath(hDC);
		Ellipse(hDC, 0, 0, 30, 20);
		EndPath(hDC);

		StrokeAndFillPath(hDC);

		/////////////// PathToRegion
		BeginPath(hDC);
		Ellipse(hDC, 0, 25, 30, 45);
//		MoveToEx(hDC, 0, 25, NULL); LineTo(hDC, 30, 25); LineTo(hDC, 30, 45);
		EndPath(hDC);

		HRGN hRgn = PathToRegion(hDC);
		SetViewportOrgEx(hDC, 0, 0, NULL);
		PaintRgn(hDC, hRgn);
		FrameRgn(hDC, hRgn, blue, 1, 1);
		SetViewportOrgEx(hDC, 420, 400, NULL);
		
		DeleteObject(hRgn);

		////////// Path, FlattenPath, Poly
		BeginPath(hDC);
		Ellipse(hDC, 0, 50, 30, 70);
		EndPath(hDC);
		
		FlattenPath(hDC);
		POINT point[100];
		BYTE  typ[100];
		int n = GetPath(hDC, point, typ, 100);

		hRgn = CreatePolygonRgn(point, n, ALTERNATE);
		
		PaintRgn(hDC, hRgn);
		FrameRgn(hDC, hRgn, blue, 1, 1);
		DeleteObject(hRgn);

		ZoomRect(hDC, -2, -2, 32, 22, 50, 0, 3);
		ZoomRect(hDC, -2, 25-2, 32, 25+22, 200, 0, 3);
		ZoomRect(hDC, -2, 50-2, 32, 50+22, 350, 0, 3);
	}

	DeleteObject(red);
	DeleteObject(blue);

	SetViewportOrgEx(hDC, 0, 0, NULL);
}


const TCHAR * TestPtInRegion(int x, int y, HRGN hRegion)
{
	static TCHAR mess[128];
	RECT rect;

	GetRgnBox(hRegion, & rect);

	if ( PtInRegion(hRegion, x, y) )
		wsprintf(mess, "(%d, %d) in [%d,%d,%d,%d]", x, y, rect.left, rect.top, rect.right, rect.bottom);
	else
		wsprintf(mess, "(%d, %d) not in [%d,%d,%d,%d]", x, y, rect.left, rect.top, rect.right, rect.bottom);

	return mess;
}

BOOL RectContainedInRegion(HRGN hrgn, CONST RECT * lprc)
{
	HRGN hCombine = CreateRectRgnIndirect(lprc);
	CombineRgn(hCombine, hrgn, hCombine, RGN_OR);
	
	BOOL rslt = EqualRgn(hCombine, hrgn);
	
	DeleteObject(hCombine);

	return rslt;
}


void KMyCanvas::TestRegionOper(HDC hDC)
{
	HRGN hRgn1 = CreateRectRgn(0, 0, 0, 0);
	HRGN hRgn2 = CreateRectRgn(0, 0, 1, 1);
	HRGN hRgn3 = CreateRectRgn(1, 1, 1, 1);
	HRGN hRgn4 = CreateRectRgn(10, 10, 1, 1);
	HRGN hRgn5 = CreateEllipticRgn(10, 10, 1, 1);
	HRGN hRgn6 = CreateRectRgn(- (1<<27), - (1<<27), (1<<27)-1, (1<<27)-1);

	Label(hDC, 180, 10, TestPtInRegion(0, 0, hRgn1));
	Label(hDC, 180, 30, TestPtInRegion(0, 0, hRgn2));
	Label(hDC, 180, 50, TestPtInRegion(1, 0, hRgn2));
	Label(hDC, 180, 70, TestPtInRegion(1, 1, hRgn3));
	
	Label(hDC, 320, 10, TestPtInRegion(1, 1, hRgn4));
	Label(hDC, 320, 30, TestPtInRegion(5, 5, hRgn5));
	Label(hDC, 320, 50, TestPtInRegion(5, 5, hRgn6));

	if ( EqualRgn(hRgn1, hRgn3) )
		Label(hDC, 340, 70, "Empty Set Unique");
	else
		Label(hDC, 340, 70, "Empty Set Not Unique");

	RECT rect = { 2, 2, 4, 5 };
	RectContainedInRegion(hRgn4, & rect);
	RectContainedInRegion(hRgn2, & rect);

	DeleteObject(hRgn1);
	DeleteObject(hRgn2);
	DeleteObject(hRgn3);
	DeleteObject(hRgn4);
	DeleteObject(hRgn5);
	DeleteObject(hRgn6);
	
	////////////////////////////////
	{
		HRGN hRgn1 = CreateEllipticRgn(0, 0, 110, 100);
		HRGN hRgn2 = CreateEllipticRgn(0, 50, 110, 150);
		HBRUSH red = CreateSolidBrush(RGB(0, 0, 0xFF));

		for (int i=0; i<6; i++)
		{
			const TCHAR * Title[] = { "Two Regions", "RGN_AND", "RGN_OR", "RGN_XOR", "RGN_DIFF", "RGN_COPY" };

			SetViewportOrgEx(hDC, 20+i*115, 90, NULL);

			FrameRgn(hDC, hRgn1, red, 1, 1);
			FrameRgn(hDC, hRgn2, red, 1, 1);

			HRGN hRgn = CreateRectRgn(0, 0, 0, 0);

			switch ( i )
			{
				case 1: CombineRgn(hRgn, hRgn1, hRgn2, RGN_AND);  break;
				case 2: CombineRgn(hRgn, hRgn1, hRgn2, RGN_OR);   break;
				case 3: CombineRgn(hRgn, hRgn1, hRgn2, RGN_XOR);  break;
				case 4: CombineRgn(hRgn, hRgn1, hRgn2, RGN_DIFF); break;
				case 5: CombineRgn(hRgn, hRgn1, hRgn2, RGN_COPY);  break;
			}

			KGDIObject brush(hDC, CreateSolidBrush(RGB(0xFF, 0xFF, 0)));

			PaintRgn(hDC, hRgn);
			DeleteObject(hRgn);

			Label(hDC, 0, 160, Title[i]);
		}
	}

	// Test GetRegionData
	{
		HBRUSH brush[2];
		brush[0] = CreateSolidBrush(RGB(0, 0, 0xFF));
		brush[1] = CreateSolidBrush(RGB(0xFF, 0xFF, 0));

		for (int i=0; i<4; i++)
		{
			SetViewportOrgEx(hDC, 20+i*170, 280, NULL);

			HRGN hRgn;
			const TCHAR * desp1[] = { "CreateRectRgn",
				                     "CreateRoundRectRgn",
									 "CreateEllipticRgn",
									 "CreatePolygonRgn"
			};										

			const TCHAR * desp2[] = { "(0, 0, 21, 21)",
				                     "(0, 0, 21, 21, 10, 10)",
									 "(0, 0, 21, 21)",
									 "(0, 0, 21, 0, 10, 21)"
			};										

			switch (i)
			{
				case 0:	hRgn = CreateRectRgn(0, 0, 21, 21); break;
				case 1: hRgn = CreateRoundRectRgn(0, 0, 21, 21, 10, 10); break;
				case 2: hRgn = CreateEllipticRgn(0, 0, 21, 21); break;
				case 3: {
					POINT P[] = { 0, 0, 21, 0, 10, 21 };
					hRgn = CreatePolygonRgn(P, 3, WINDING);
						}
			}

			KRegion region;
			region.GetRegionData(hRgn);
//			DeleteObject(hRgn);

			TCHAR temp[64];

			RECT rect = region.m_pRegion->rdh.rcBound;

			Label(hDC, 0, 148, desp1[i]);
			Label(hDC, 0, 168, desp2[i]);

			wsprintf(temp, "rcBound: [%d, %d, %d, %d]", rect.left, rect.top, rect.right, rect.bottom);
			Label(hDC, 0, 190, temp);

			wsprintf(temp, "nCount : %d", region.m_nRectCount);
			Label(hDC, 0, 210, temp);

			wsprintf(temp, "Size   : %d bytes", region.m_nRegionSize);
			Label(hDC, 0, 230, temp);

			Rectangle(hDC, rect.left * 7, rect.top*7, rect.right*7, rect.bottom*7);

			for (int j=0; j<region.m_nRectCount; j++)
			{
				rect = region.m_pRect[j];

				rect.left   *= 7;
				rect.right  *= 7;
				rect.top    *= 7;
				rect.bottom *= 7;

				FillRect(hDC, & rect, brush[j&1]);
			}

			/////////// Play with ExtCreateRegion
  			if ( region.m_nRectCount>=2 )
			{
  				RECT r = region.m_pRect[0];
  				region.m_pRect[0] = region.m_pRect[region.m_nRectCount-1];
  				region.m_pRect[region.m_nRectCount-1] = r;
			}

//			{
//				long t = region.m_pRect[0].left;
//				region.m_pRect[0].left = region.m_pRect[0].right;
//				region.m_pRect[0].right = t;
//			}

			HRGN hRgn1 = region.CreateRegion(NULL);

			if ( hRgn1 )
				Label(hDC, 0, 250, "OK");
			else
				Label(hDC, 0, 250, "Fail");

			if ( hRgn1 )
			{
				GetRgnBox(hRgn1, & rect);
				wsprintf(temp, "rcBound: [%d, %d, %d, %d]", rect.left, rect.top, rect.right, rect.bottom);
				Label(hDC, 0, 270, temp);

				if ( EqualRgn(hRgn, hRgn1) )
					Label(hDC, 100, 250, "Equal");
				else
					Label(hDC, 100, 250, "Not Equal");

				SetViewportOrgEx(hDC, 20+i*170, 280+300, NULL);

				Rectangle(hDC, rect.left * 7, rect.top*7, rect.right*7, rect.bottom*7);

				region.GetRegionData(hRgn1);

				for (int j=0; j<region.m_nRectCount; j++)
				{
					rect = region.m_pRect[j];

					rect.left   *= 7;
					rect.right  *= 7;
					rect.top    *= 7;
					rect.bottom *= 7;

					FillRect(hDC, & rect, brush[j&1]);
				}

				DeleteObject(hRgn1);
			}

			DeleteObject(hRgn);
		}

		DeleteObject(brush[0]);
		DeleteObject(brush[1]);
	}
/*
	{
		RGNDATA rgn[5];

		RECT rr = { 0, 0, 100, 100 };

		rgn[0].rdh.dwSize   = sizeof(rgn[0].rdh);
		rgn[0].rdh.iType    = RDH_RECTANGLES;
		rgn[0].rdh.nCount   = 2;
		rgn[0].rdh.nRgnSize = 2 * sizeof(RECT);
		rgn[0].rdh.rcBound  = rr;
		memcpy(rgn[0].Buffer, & rr, sizeof(rr));
		memcpy(rgn[0].Buffer+sizeof(rr), & rr, sizeof(rr));

		HRGN hRgn = ExtCreateRegion(NULL, sizeof(rgn), rgn);
	}		
*/
	SetViewportOrgEx(hDC, 0, 0, NULL);
}


void KMyCanvas::TestRegionPaint(HDC hDC)
{
	HBRUSH yellow = CreateSolidBrush(RGB(0, 0, 0xFF));
	HPEN   blue   = CreatePen(PS_SOLID, 3, RGB(0xFF, 0xFF, 0));
	
	SelectObject(hDC, blue);
	SelectObject(hDC, yellow);

//	SetBkMode(hDC, TRANSPARENT);
	SetROP2(hDC, R2_MASKNOTPEN);

	RoundRect(hDC, 50, 50, 130, 150, 40, 40);	Label(hDC, 50, 160, "RoundRect");

	HRGN hRgn = CreateRoundRectRgn(50, 50, 130, 150, 40, 40);

	OffsetRgn(hRgn, 90, 0); PaintRgn(hDC, hRgn);					Label(hDC, 140,  30, "PaintRgn");
	OffsetRgn(hRgn, 90, 0);  FillRgn(hDC, hRgn,  yellow);			Label(hDC, 230, 160, "FillRgn");
	OffsetRgn(hRgn, 90, 0); FrameRgn(hDC, hRgn,  yellow, 1, 1);		Label(hDC, 320,  30, "FrameRgn(1,1)");
	OffsetRgn(hRgn, 90, 0); FrameRgn(hDC, hRgn,  yellow, 1, 10);	Label(hDC, 410, 160, "FrameRgn(1,10)");
	OffsetRgn(hRgn, 90, 0); FrameRgn(hDC, hRgn,  yellow, 10, 1);	Label(hDC, 500,  30, "FrameRgn(10,1)");
	OffsetRgn(hRgn, 90, 0); FrameRgn(hDC, hRgn,  yellow, 10, 10);	Label(hDC, 590, 160, "FrameRgn(10,10)");
	OffsetRgn(hRgn, 90, 0); InvertRgn(hDC, hRgn);					Label(hDC, 680,  30, "InvertRgn");

	DeleteObject(hRgn);
	
	int nSave = SaveDC(hDC);
	SetMapMode(hDC, MM_ANISOTROPIC);
	SetViewportOrgEx(hDC, 200, 200, NULL);
	SetViewportExtEx(hDC, 15, 15, NULL);

	hRgn = CreateRoundRectRgn(0, 0, 20, 20, 10, 10);

	FrameRgn(hDC, hRgn, yellow, 1, 1);
	
	DeleteObject(hRgn);

	RestoreDC(hDC, nSave);

	SelectObject(hDC, GetStockObject(WHITE_BRUSH));
	SelectObject(hDC, GetStockObject(BLACK_PEN));
	DeleteObject(yellow);
	DeleteObject(blue);
//	SetBkMode(hDC, OPAQUE);
	SetROP2(hDC, R2_COPYPEN);
}

class KGradient
{
	COLORREF m_c0;
	COLORREF m_c1;

	TRIVERTEX m_vertex[12];
	ULONG     m_index [12];
	
public:
	
	KGradient(COLORREF c0, COLORREF c1)
	{
		m_c0 = c0;
		m_c1 = c1;
	}

	void Define(int i, int x, int y, int cr);
	BOOL Fill(HDC hDC, int nVertex, int nMesh, DWORD nMode, const POINT * pPoint, const int *pColor);
};

void KGradient::Define(int i, int x, int y, int c)
{
	m_index[i]        = i;
	m_vertex[i].x     = x;
	m_vertex[i].y     = y;
	m_vertex[i].Red   = ( ( GetRValue(m_c0) * c + GetRValue(m_c1) * (2-c) ) / 2 ) << 8;
	m_vertex[i].Green = ( ( GetGValue(m_c0) * c + GetGValue(m_c1) * (2-c) ) / 2 ) << 8;
	m_vertex[i].Blue  = ( ( GetBValue(m_c0) * c + GetBValue(m_c1) * (2-c) ) / 2 ) << 8;
	m_vertex[i].Alpha = 0;
}

BOOL KGradient::Fill(HDC hDC, int nVertex, int nMesh, DWORD nMode, const POINT * pPoint, const int *pColor)
{
	for (int i=0; i<nVertex; i++)
		Define(i, pPoint[i].x, pPoint[i].y, pColor[i]);

	return ::GradientFill(hDC, m_vertex, nVertex, m_index, nMesh, nMode);
}


void KMyCanvas::TestGradientFill(HDC hDC)
{
	const COLORREF c0 = RGB(0x20, 0x20, 0x20);
	const COLORREF c1 = RGB(0xF0, 0xF0, 0x20);

	for (int i=0; i<4; i++)
		GradientRectangle(hDC, 100+110*i, 50, 200+110*i, 150, c0, c1, i*45);

	for (i=0; i<4; i++)
		SymGradientRectangle(hDC, 100+110*i, 160, 200+110*i, 260, c0, c1, i*45);

	for (i=0; i<4; i++)
		CornerGradientRectangle(hDC, 100+110*i, 270, 200+110*i, 370, c0, c1, i);

	CenterGradientRectangle(hDC, 560, 150, 660, 250, c0, c1);
	CenterGradientRectangle(hDC, 560, 260, 660, 360, c1, c0);

	//  Buttons
	SetViewportOrgEx(hDC, 20, 400, NULL);

	RoundRectButton(hDC, 0,   0,  80, 80,  0, 10, RGB(0x20, 0x20, 0x20), RGB(0xF0, 0xF0, 0x20));
	RoundRectButton(hDC, 100, 0, 180, 80, 40, 10, RGB(0xF0, 0x20, 0x20), RGB(0x20, 0xF0, 0x20));
	RoundRectButton(hDC, 200, 0, 280, 80, 80, 10, RGB(0xFF, 0xFF, 0x20), RGB(0x20, 0x20, 0xF0));
	
	   GradientRectangle(hDC, 0, 100, 200, 200, RGB(0xFF, 0x0, 0), RGB(0, 0, 0xFF), 0);
	HLSGradientRectangle(hDC, 0, 220, 200, 320, RGB(0xFF, 0x0, 0), RGB(0, 0, 0xFF), 200);

	RadialGradientFill(hDC, 400, 150, 400   , 150   , 100, RGB(0xFF, 0xFF, 0xFF), RGB(0, 0, 0xFF), 8);
	RadialGradientFill(hDC, 620, 150, 620-30, 150-60, 100, RGB(0xFF, 0xFF, 0xFF), RGB(0, 0, 0xFF), 16);
	RadialGradientFill(hDC, 840, 150, 840-30, 150+30, 100, RGB(0xFF, 0xFF, 0xFF), RGB(0, 0, 0xFF), 256);

	SetViewportOrgEx(hDC, 0, 0, NULL);

	{
		RECT r = { 700, 50, 850, 50+150 };

		KGDIObject blue(hDC, CreatePen(PS_SOLID, 3, RGB(0, 0, 0xFF)));
		KGDIObject yellow(hDC, CreateSolidBrush(RGB(0xFF, 0xFF, 0)));
	
		Rectangle(hDC, 700, 50, 850, 50+150);
		
		BrickPatternFill(hDC, 700, 50, 850, 50+150, 15, 15);
		BrickPatternFill(hDC, 860, 50, 860+150, 50+150, 10, 10);
	}
}

void KMyCanvas::OnDraw(HDC hDC, const RECT * rcPaint)
{
	switch ( m_test )
	{
		case IDM_TEST_DITHER:			TestDither(hDC);			break;
		case IDM_TEST_HATCHBRUSH:		TestHatch(hDC);				break;
		case IDM_TEST_SYSCOLORBRUSH:	TestSysColorBrush(hDC);		break;
		case IDM_TEST_RECTANGLE:		TestRectangle(hDC);			break;
		case IDM_TEST_DRAWFRAMECONTROL: TestDrawFrameControl(hDC);	break;
		case IDM_TEST_ELLIPSE:			TestEllipse(hDC);			break;
		case IDM_TEST_PIECHART:			TestPieChart(hDC);			break;
		case IDM_TEST_POLYFILLMODE:		TestPolyFillMode(hDC);		break;
		case IDM_TEST_FILLPATH:			TestFillPath(hDC);			break;
		case IDM_TEST_REGION:			TestRegion(hDC);			break;
		case IDM_TEST_REGIONOP:			TestRegionOper(hDC);		break;
		case IDM_TEST_REGIONPAINT:		TestRegionPaint(hDC);		break;
		case IDM_TEST_GRADIENTFILL:		TestGradientFill(hDC);		break;
	}
}


class KMyFrame : public KFrame
{
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if ( uMsg==WM_NCPAINT )
		{
			KFrame::WndProc(hWnd, uMsg, wParam, lParam);

 			HDC hDC = GetDCEx(hWnd, (HRGN) wParam, 0x10000 | DCX_WINDOW|DCX_INTERSECTRGN);
			
			RECT rect;
			GetWindowRect(hWnd, & rect);

			OffsetRect(&rect, - rect.left, - rect.top);

			for (int i=0; i<4; i++)
			{
				FrameRect(hDC, & rect, GetSysColorBrush(COLOR_SCROLLBAR+i));
			
				InflateRect(&rect, -1, -1);
			}
			
			ReleaseDC(hWnd, hDC);

			return 0;
		}
		else
			return KFrame::WndProc(hWnd, uMsg, wParam, lParam);
	}

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KFrame::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_AREA));
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

	frame.CreateEx(0, _T("AreaDemo"), _T("Areas"),
		WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}
