//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : pagecanvas.cpp						                                 //
//  Description: KSurface class implementation                                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <commdlg.h>

#include "outputsetup.h"
#include "pagecanvas.h"

void DrawFrame(HDC hDC, int x0, int y0, int x1, int y1, COLORREF crPen, COLORREF crBrush, bool frame)
{
//	TCHAR temp[MAX_PATH];

//	wsprintf(temp, "%d %d %d %d   w=%d, h=%d\n", x0, y0, x1, y1, x1-x0, y1-y0);
//	OutputDebugString(temp);

	HPEN   hPen   = CreatePen(PS_SOLID, 0, crPen);
	HBRUSH hBrush = CreateSolidBrush(crBrush);

	HGDIOBJ hOldPen = SelectObject(hDC, hPen);
	HGDIOBJ hOldBrush = SelectObject(hDC, hBrush);
	
	Rectangle(hDC, x0-1, y0-1, x1+1, y1+1);
	
	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);
	DeleteObject(hOldPen);
	DeleteObject(hOldBrush);

	if ( frame )
	{
		RECT rectr = { x1+1, y0+1, x1+3, y1+3 };
		FillRect(hDC, & rectr, (HBRUSH) GetStockObject(BLACK_BRUSH));

		RECT rectb = { x0+1, y1+1, x1+3, y1+3 };
		FillRect(hDC, & rectb, (HBRUSH) GetStockObject(BLACK_BRUSH));
	}
}

void KSurface::DrawPaper(HDC hDC, const RECT * rcPaint, int col, int row)
{
	// paper frame
	DrawFrame(hDC, px(0, col), py(0, row), px(m_Paper.cx, col), py(m_Paper.cy, row), 
		RGB(0, 0, 0), RGB(0xE0, 0xE0, 0xE0), true);

	// minimum margin : printable margin
	DrawFrame(hDC, px(m_MinMargin.left, col), py(m_MinMargin.top, row),
		           px(m_Paper.cx - m_MinMargin.right, col),
				   py(m_Paper.cy - m_MinMargin.bottom, row), 
				   RGB(0xF0, 0xF0, 0xF0), RGB(0xF0, 0xF0, 0xF0), false);

	// margin
	DrawFrame(hDC, px(m_Margin.left, col), py(m_Margin.top, row),
		           px(m_Paper.cx - m_Margin.right, col), py(m_Paper.cy - m_Margin.bottom, row), 
				   RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), false);
}


// convert from 1/1000 inch to BASE_DPI
	
void KSurface::SetPaper(void)
{
	POINT paper;
	RECT  margin;
	RECT  minmargin; 
	
	m_OutputSetup.GetPaperSize(paper);
	m_OutputSetup.GetMargin(margin);
	m_OutputSetup.GetMinMargin(minmargin);

	m_Paper.cx     = paper.x * BASE_DPI / 1000;
	m_Paper.cy     = paper.y * BASE_DPI / 1000;

	m_Margin.left  = margin.left   * BASE_DPI / 1000;
	m_Margin.right = margin.right  * BASE_DPI / 1000;
	m_Margin.top   = margin.top    * BASE_DPI / 1000;
	m_Margin.bottom= margin.bottom * BASE_DPI / 1000;

	m_MinMargin.left  = minmargin.left   * BASE_DPI / 1000;
	m_MinMargin.right = minmargin.right  * BASE_DPI / 1000;
	m_MinMargin.top   = minmargin.top    * BASE_DPI / 1000;
	m_MinMargin.bottom= minmargin.bottom * BASE_DPI / 1000;
}


// calculate overall surface size needed to fit nPage divided into nCol columns
void KSurface::CalculateSize(void)
{
	int nPage = GetPageCount();
	int nCol  = GetColumnCount();
	int nRow  = (nPage + nCol - 1) / nCol;

	m_nSurfaceWidth  = px(m_Paper.cx, 0) * nCol + MARGIN_X;
	m_nSurfaceHeight = py(m_Paper.cy, 0) * nRow + MARGIN_Y; 
}

int gcd(int m, int n)
{
	if ( m==0 )
		return n;
	else
		return gcd(n % m, m);
}

void SetupULCS(HDC hDC, int zoom)
{
	SetMapMode(hDC, MM_ANISOTROPIC);

	int mul = ONEINCH						 * 100;
	int div = GetDeviceCaps(hDC, LOGPIXELSX) * zoom;
	int fac = gcd(mul, div);

	mul    /= fac;
	div    /= fac;

	  SetWindowExtEx(hDC, mul, mul, NULL);
	SetViewportExtEx(hDC, div, div, NULL);
}

bool KSurface::UponSetZoom(int zoom)
{
	if ( zoom == m_nZoom )
		return false;

	m_nZoom = zoom;

	HDC hDC  = GetDC(NULL);
	m_nScale = zoom * GetDeviceCaps(hDC, LOGPIXELSX) / 100;
	DeleteDC(hDC);
	
	CalculateSize();

	return true;
}


void KSurface::RefreshPaper(void)
{
	int zoom = m_nZoom;

	m_nZoom = 0;
	SetPaper();
	UponSetZoom(zoom);
}


void KSurface::UponInitialize(HWND hWnd)
{
	m_OutputSetup.SetDefault(hWnd, 1, GetPageCount());

	m_nZoom = 100;
	RefreshPaper();
}


void KSurface::UponDrawPage(HDC hDC, const RECT * rcPaint, int width, int height, int pageno)
{
	for (int h=0; h<=(height-BASE_DPI); h += BASE_DPI)
	for (int w=0; w<=(width-BASE_DPI);  w += BASE_DPI)
		Rectangle(hDC, w, h, w+BASE_DPI, h+BASE_DPI);
}

// draw page on screen, with paper border, scale
void KSurface::UponDraw(HDC hDC, const RECT * rcPaint)
{
	int nPage = GetPageCount();
	int nCol  = GetColumnCount();
	int nRow  = (nPage + nCol - 1) / nCol;

	for (int p=0; p<nPage; p++)
	{
		SaveDC(hDC);

		int col = p % nCol;
		int row = p / nCol;
		DrawPaper(hDC, rcPaint, col, row);
		SetupULCS(hDC, m_nZoom);
		OffsetViewportOrgEx(hDC, px(m_Margin.left, col), py(m_Margin.top, row), NULL);
		UponDrawPage(hDC, rcPaint, GetDrawableWidth(), GetDrawableHeight(), p);

		RestoreDC(hDC, -1);
	}
}


// print document to printer
bool KSurface::UponPrint(HDC hDC, const TCHAR * pDocName)
{
	int scale = GetDeviceCaps(hDC, LOGPIXELSX);
		
	SetMapMode(hDC, MM_ANISOTROPIC);
		
	  SetWindowExtEx(hDC, BASE_DPI, BASE_DPI, NULL);
	SetViewportExtEx(hDC, scale,    scale,    NULL);

	OffsetViewportOrgEx(hDC, (m_Margin.left - m_MinMargin.left) * scale / BASE_DPI, 
		                     (m_Margin.top  - m_MinMargin.top ) * scale / BASE_DPI, NULL);

	DOCINFO docinfo;

	docinfo.cbSize       = sizeof(docinfo);
	docinfo.lpszDocName  = pDocName;
	docinfo.lpszOutput   = NULL; 
	docinfo.lpszDatatype = _T("EMF");
	docinfo.fwType		 = 0; 

	if ( StartDoc(hDC, & docinfo) <= 0)
		return false;

	int nFrom = 0;
	int nTo   = GetPageCount();

	for (int pageno=nFrom; pageno<nTo; pageno++)
	{
		if ( StartPage(hDC) < 0 )
		{
			AbortDoc(hDC);
			return FALSE;
		}

		UponDrawPage(hDC, NULL, GetDrawableWidth(), GetDrawableHeight(), pageno);
		EndPage(hDC);
	}

	EndDoc(hDC);

	return true;
}


bool KSurface::UponFilePrint(void)
{
	int rslt = m_OutputSetup.PrintDialog(PD_RETURNDC | PD_SELECTION);

	if ( rslt==IDOK )
		UponPrint(m_OutputSetup.GetPrinterDC(), GetDocumentName());

	m_OutputSetup.DeletePrinterDC();

	return false;
}


bool KSurface::UponFilePageSetup(void)
{
	if ( m_OutputSetup.PageSetup(PSD_MARGINS) )
	{
		RefreshPaper();
		return true;
	}
	
	return false;
}

////////////////////////////////////////////////////


void KPageCanvas::Response(int rslt)
{
	if ( rslt & View_Resize )
	{
		SetSize(GetPixelWidth(), GetPixelHeight(), 20, 20, TRUE);
		rslt |= View_Redraw;
	}

	if ( rslt & View_Redraw )
		InvalidateRect(m_hWnd, NULL, TRUE);
}


void KPageCanvas::OnCreate(void)
{
	UponInitialize(m_hWnd);		// initialize KSurface

	GetDimension();
	SetSize(m_nPixelWidth, m_nPixelHeight, 20, 20, TRUE);
}

LRESULT KPageCanvas::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd		= hWnd;
			m_hViewMenu = LoadMenu(m_hInst, MAKEINTRESOURCE(m_nMenuID));

			OnCreate();
			return 0;

		case WM_PAINT:
			return KScrollCanvas::WndProc(hWnd, uMsg, wParam, lParam);

		case WM_SIZE:
		case WM_HSCROLL:
		case WM_VSCROLL:
			return KScrollCanvas::WndProc(hWnd, uMsg, wParam, lParam);

		case WM_COMMAND:
			Response( OnCommand(LOWORD(wParam), hWnd) );
			return 0;
			break;

		default:
			return CommonMDIChildProc(hWnd, uMsg, wParam, lParam, m_hViewMenu, 2);
	}
}


void KPageCanvas::GetWndClassEx(WNDCLASSEX & wc)
{
	memset(& wc, 0, sizeof(wc));

	wc.cbSize      = sizeof(WNDCLASSEX);
	wc.style       = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra  = 0;
	wc.cbWndExtra  = 0;       
	wc.hInstance   = NULL;
	wc.hIcon       = NULL;
	wc.hCursor     = LoadCursor(NULL, IDC_ARROW); 

	if ( m_hBackground )
		wc.hbrBackground  = m_hBackground;
	else
		wc.hbrBackground  = (HBRUSH) (COLOR_WINDOW + 1);
		
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = NULL;
	wc.hIconSm       = NULL;
}
	

bool KPageCanvas::Initialize(HINSTANCE hInstance, KStatusWindow * pStatus, int menuid)
{
	m_nMenuID  = menuid;
	m_hInst    = hInstance;
	m_pStatus  = pStatus;

	RegisterClass(GetClassName(), hInstance);
		
	return true;
}


KPageCanvas::KPageCanvas(HBRUSH hBackground)
{
	m_hBackground  = hBackground;
	m_nPixelWidth  = 320;
	m_nPixelHeight = 240;
}
