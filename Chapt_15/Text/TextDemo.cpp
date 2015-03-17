//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : textdemo.cpp					                                     //
//  Description: Text demo routines                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define NOCRYPT
#define WINVER 0x0500

#pragma pack(push, 4)
#include <windows.h>
#pragma pack(pop)

#include <assert.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>

#include "..\..\include\gdiobject.h"
#include "..\..\include\fonttext.h"
#include "..\..\include\color.h"
#include "..\..\include\axis.h"
#include "..\..\include\curve.h"
#include "..\..\include\fonttext.h"
#include "..\..\include\OutlineMetric.h"
#include "..\..\include\win.h"

#include "TextDemo.h"
#include "resource.h"

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

//////// GLYPH View ///////////

void Line(HDC hDC, int x0, int y0, int x1, int y1)
{
	MoveToEx(hDC, x0, y0, NULL);
	LineTo(hDC, x1, y1);
}


void Box(HDC hDC, int x0, int y0, int x1, int y1)
{
	MoveToEx(hDC, x0, y0, NULL);
	LineTo(hDC, x1, y0);
	LineTo(hDC, x1, y1);
	LineTo(hDC, x0, y1);
	LineTo(hDC, x0, y0);
}

void disp(HDC hDC, int x, int y, const TCHAR * mess, int val)
{
	TCHAR temp[MAX_PATH];

	wsprintf(temp, mess, val);
	TextOut(hDC, x, y, temp, _tcslen(temp));
}

void disp(HDC hDC, int x, int y, const TCHAR * mess)
{
	TextOut(hDC, x, y, mess, _tcslen(mess));
}


int Demo_Font(HDC hDC, int x, int y, HFONT hFont, const TCHAR * name)
{
	HGDIOBJ hOld  = SelectObject(hDC, hFont);

	EXTLOGFONT elf;
	int size = GetObject(hFont, sizeof(elf), & elf);

	LOGFONT lf;
	GetObject(hFont, sizeof(lf), & lf);
	
	TEXTMETRIC tm;
	GetTextMetrics(hDC, & tm);
	int height = tm.tmHeight;

	disp(hDC, x, y, name); y+= height;

//	TCHAR face[MAX_PATH];

//	GetTextFace(hDC, MAX_PATH, face);

	TCHAR mess[MAX_PATH];
	wsprintf(mess, "{%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, """"%s""""}", 
		lf.lfHeight, lf.lfWidth, lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
		lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet, lf.lfOutPrecision,
		lf.lfClipPrecision, lf.lfQuality, lf.lfPitchAndFamily, lf.lfFaceName);

	disp(hDC, x, y, mess); y+= height;
	
//	wsprintf(mess, "%d %d %d %d", size, sizeof(LOGFONT), sizeof(ENUMLOGFONTEX), sizeof(ENUMLOGFONTEXDV));
//	disp(hDC, x, y, mess); y+= height;
//	disp(hDC, x, y, face); y+= height;

	SelectObject(hDC, hOld);
 
	return y;
}

int Demo_Font(HDC hDC, int x, int y, int stockid, const TCHAR * name)
{
	return Demo_Font(hDC, x, y, (HFONT) GetStockObject(stockid), name);
}


void Demo_StockFonts(HDC hDC, const RECT * rcPaint)
{
	int y = 10;

	int base = GetDialogBaseUnits();
	TCHAR temp[64];
	wsprintf(temp, "DialogBaseUnits: baseunixX=%d, baseunitY=%d", LOWORD(base), HIWORD(base));
	TextOut(hDC, 10, y, temp, _tcslen(temp));
	y+= 30;

	wsprintf(temp, "GetDeviceCaps(LOGPIXELSX)=%d, GetDeviceCaps(LOGPIXELSX)=%d",
		GetDeviceCaps(hDC, LOGPIXELSX), GetDeviceCaps(hDC, LOGPIXELSY));
	TextOut(hDC, 10, y, temp, _tcslen(temp));

	y += 30;

	y = Demo_Font(hDC, 10, y, DEFAULT_GUI_FONT,	   "GetStockObject(DEFAULT_GUI_FONT)")    + 5;
	y = Demo_Font(hDC, 10, y, OEM_FIXED_FONT,	   "GetStockObject(OEM_FIXED_FONT)")      + 5;
	y = Demo_Font(hDC, 10, y, ANSI_FIXED_FONT,	   "GetStockObject(ANSI_FIXED_FONT)")     + 5;
	y = Demo_Font(hDC, 10, y, ANSI_VAR_FONT,	   "GetStockObject(ANSI_VAR_FONT)")       + 5;
	y = Demo_Font(hDC, 10, y, SYSTEM_FONT,		   "GetStockObject(SYSTEM_FONT)")         + 5;
	y = Demo_Font(hDC, 10, y, DEVICE_DEFAULT_FONT, "GetStockObject(DEVICE_DEFAULT_FONT)") + 5;
	y = Demo_Font(hDC, 10, y, SYSTEM_FIXED_FONT,   "GetStockObject(SYSTEM_FIXED_FONT)")   + 5;


/*	{
		KLogFont lf(- PointSizetoLogical(hDC, 36), "Tahoma");
		
		lf.m_lf.lfItalic = TRUE;
		lf.m_lf.lfCharSet = GB2312_CHARSET;
		HFONT hFont = lf.CreateFont();

		SelectObject(hDC, hFont);

		TCHAR temp[32];
		GetTextFace(hDC, 32, temp);
		TextOut(hDC, 400, 10, temp, _tcslen(temp));

		wsprintf(temp, "%d %d", GB2312_CHARSET, GetTextCharset(hDC));
		TextOut(hDC, 400, 60, temp, _tcslen(temp));

		SelectObject(hDC, GetStockObject(DEFAULT_GUI_FONT));
		DeleteObject(hFont);
	}
*/
}

// Terminology and Metrics


/*  int    otmsCharSlopeRise; 
    int    otmsCharSlopeRun; 
    int    otmItalicAngle; 
    UINT   otmEMSquare; 
    int    otmAscent; 
    int    otmDescent; 
    UINT   otmLineGap; 
    UINT   otmsCapEmHeight; 
    UINT   otmsXHeight; 
    RECT   otmrcFontBox; 
    UINT   otmusMinimumPPEM; 
*/

void Demo_Term(HDC hDC, const RECT * rcPaint)
{
	int point = 216;

	HFONT hFont = CreateFont( point * GetDeviceCaps(hDC, LOGPIXELSY) / 72,
						 0, 0, 0, FW_NORMAL, TRUE, FALSE, FALSE, 
						 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, 
						 ANTIALIASED_QUALITY, VARIABLE_PITCH, "Times New Roman");
	SetTextAlign(hDC, TA_BASELINE | TA_LEFT);
	SetBkMode(hDC, TRANSPARENT);

	SelectObject(hDC, hFont);

	{
		KOutlineTextMetric otm(hDC);

		TEXTMETRIC tm;

		GetTextMetrics(hDC, & tm);
		int x = 300;
		int y = 450;

		// glyph bounding box
		Rectangle(hDC, x + otm.m_pOtm->otmrcFontBox.left,
			           y - otm.m_pOtm->otmrcFontBox.top,
					   x + otm.m_pOtm->otmrcFontBox.right,
					   y - otm.m_pOtm->otmrcFontBox.bottom);

		// draw the character(s)
		TextOut(hDC, x, y, "@", 1);

		SIZE size;

		GetTextExtentPoint(hDC, "@" /*"Ç"*/, 1, & size);

		// vertical metrics lines
		HPEN hPen = CreatePen(PS_DOT, 0, RGB(0, 0, 0));
		HGDIOBJ hOld = SelectObject(hDC, hPen);

		Line(hDC, x-160, y-tm.tmAscent-tm.tmExternalLeading,  x+400, y-tm.tmAscent-tm.tmExternalLeading);
		Line(hDC, x-160, y-tm.tmAscent,						  x+400, y-tm.tmAscent);
		Line(hDC, x-160, y-tm.tmAscent+tm.tmInternalLeading,  x+400, y-tm.tmAscent+tm.tmInternalLeading);
		Line(hDC, x-160, y,									  x+400, y);
		Line(hDC, x-160, y+tm.tmDescent,					  x+400, y+tm.tmDescent);

		SelectObject(hDC, hOld);
		DeleteObject(hPen);

		SelectObject(hDC, GetStockObject(SYSTEM_FONT));

		DeleteObject(hFont);

		disp(hDC, 10,  30, "tmHeight          %d", tm.tmHeight);
		disp(hDC, 10,  55, "tmAscent          %d", tm.tmAscent);
		disp(hDC, 10,  80, "tmDescent         %d", tm.tmDescent);
		disp(hDC, 10, 105, "tmInternalLeading %d", tm.tmInternalLeading);
		disp(hDC, 10, 130, "tmExternalLeading %d", tm.tmExternalLeading);

		Line(hDC, x+500, y-otm.m_pOtm->otmMacAscent-otm.m_pOtm->otmMacLineGap,  x+600, 
			y-otm.m_pOtm->otmMacAscent-otm.m_pOtm->otmMacLineGap);
		Line(hDC, x+500, y-otm.m_pOtm->otmMacAscent,					x+620, y-otm.m_pOtm->otmMacAscent);
		Line(hDC, x+500, y,									    x+660, y);
		Line(hDC, x+500, y-otm.m_pOtm->otmMacDescent,					x+680, y-otm.m_pOtm->otmMacDescent);
	
		disp(hDC, 510,  30, "otmAscent   %d", otm.m_pOtm->otmMacAscent);
		disp(hDC, 510,  55, "otmDescent  %d", otm.m_pOtm->otmMacDescent);
		disp(hDC, 510,  80, "otmLineGap  %d", otm.m_pOtm->otmMacLineGap);

		// strikeout
		SelectObject(hDC, GetStockObject(LTGRAY_BRUSH));
		Rectangle(hDC, x,         y - otm.m_pOtm->otmsStrikeoutPosition, 
				 x+size.cx, y - otm.m_pOtm->otmsStrikeoutPosition + otm.m_pOtm->otmsStrikeoutSize); 

		// underscore
		Rectangle(hDC, x,     y - otm.m_pOtm->otmsUnderscorePosition, 
				 x+size.cx, y - otm.m_pOtm->otmsUnderscorePosition + otm.m_pOtm->otmsUnderscoreSize); 
		SelectObject(hDC, GetStockObject(WHITE_BRUSH));

		x += size.cx;

		// slope
		MoveToEx(hDC, x - otm.m_pOtm->otmsCharSlopeRun*3, y + otm.m_pOtm->otmsCharSlopeRise*3, NULL);
		LineTo(hDC,   x + otm.m_pOtm->otmsCharSlopeRun*11, y - otm.m_pOtm->otmsCharSlopeRise*11);

		Box(hDC, x + otm.m_pOtm->otmptSubscriptOffset.x,
			     y + otm.m_pOtm->otmptSubscriptOffset.y,
				 x + otm.m_pOtm->otmptSubscriptOffset.x + otm.m_pOtm->otmptSubscriptSize.x,
				 y + otm.m_pOtm->otmptSubscriptOffset.y - otm.m_pOtm->otmptSubscriptSize.y);

		Box(hDC, x + otm.m_pOtm->otmptSuperscriptOffset.x,
			     y - otm.m_pOtm->otmptSuperscriptOffset.y,
				 x + otm.m_pOtm->otmptSuperscriptOffset.x + otm.m_pOtm->otmptSuperscriptSize.x,
				 y - otm.m_pOtm->otmptSuperscriptOffset.y - otm.m_pOtm->otmptSuperscriptSize.y);
	}
}


int e_linespace;
int e_pageheight;
int e_externalleading;

int LinesPerPage(HDC hDC, int nPointSize, int nPageHeight)
{
	KLogFont lf(-PointSizetoLogical(hDC, nPointSize), "Times New Roman");

	HFONT hFont  = lf.CreateFont();
	HGDIOBJ hOld = SelectObject(hDC, hFont);

	TEXTMETRIC tm;
	GetTextMetrics(hDC, & tm);

	int linespace = tm.tmHeight + tm.tmExternalLeading;
	SelectObject(hDC, hOld);
	DeleteObject(hFont);

	POINT P[2] = { 0, 0, 0, nPageHeight }; // device coordinate
	DPtoLP(hDC, P, 2);					   // logical coordinate
	nPageHeight = abs(P[1].y-P[0].y);

	e_linespace       = linespace;
	e_pageheight      = nPageHeight;
	e_externalleading = tm.tmExternalLeading;

	return (nPageHeight + tm.tmExternalLeading) / linespace;
}

/*
const int UHR_WIN95 = 1800;
const int UHR_WINNT = 7200;

// setup ultra-high-resolution logical coordinate system
void Setup_UHR(HDC hDC, int dpi=UHR_WIN95)
{
	SetMapMode(hDC, MM_ANISOTROPIC);

	SetWindowExtEx(hDC, dpi, dpi, NULL);
	SetWindowOrgEx(hDC, 0, 0,     NULL);

	SetViewportExtEx(hDC, GetDeviceCaps(hDC, LOGPIXELSX), GetDeviceCaps(hDC, LOGPIXELSY), NULL);
	SetViewportOrgEx(hDC, 0, 0, NULL);
}
*/

void Demo_Metrics(HDC hDisp, HDC hDC, int x, int y, int mapmode, int point, int winext=1)
{
	SaveDC(hDC);

	if ( mapmode==MM_ANISOTROPIC )
	{
		SetMapMode(hDC, MM_ANISOTROPIC);

		SetWindowExtEx(hDC, winext, winext, NULL);
		SetViewportExtEx(hDC, 1, 1, NULL);
	}
	else
		SetMapMode(hDC, mapmode);
	
	int lineno = LinesPerPage(hDC, point, 10 * GetDeviceCaps(hDC, LOGPIXELSY));
	
	SIZE wndext, viewext;
	GetWindowExtEx(hDC, & wndext);
	GetViewportExtEx(hDC, & viewext);

	RestoreDC(hDC, -1);

	TCHAR temp[MAX_PATH];
	wsprintf(temp, _T("%d dpi, %d:%d, Page height %d, linespace %d, external leading %d, Lines per page %d"), 
		GetDeviceCaps(hDC, LOGPIXELSY),
		wndext.cx, viewext.cx,
		e_pageheight, e_linespace, 
		e_externalleading, lineno);

	TextOut(hDisp, x, y, temp, _tcslen(temp));
}


// Test the relationship between logical coordinate space resolution vs font metrics accuracy
void Test_LC(void)
{
	HDC hDC = GetDC(NULL);

	SetMapMode(hDC, MM_ANISOTROPIC);
	SetViewportExtEx(hDC, 1, 1, NULL);
	
	TCHAR mess[MAX_PATH];
	mess[0] = 0;

	for (int i=1; i<=64; i*=2)
	{
		SetWindowExtEx(hDC, i, i, NULL);

		KLogFont lf(-PointSizetoLogical(hDC, 24), "Times New Roman"); // 24-point
		HFONT hFont = lf.CreateFont();
		SelectObject(hDC, hFont);

		TEXTMETRIC tm; 
		GetTextMetrics(hDC, & tm);
		wsprintf(mess + _tcslen(mess), "%d:1 lfHeight=%d, tmHeight=%d\n", i, lf.m_lf.lfHeight, tm.tmHeight);

		SelectObject(hDC, GetStockObject(ANSI_VAR_FONT));
		DeleteObject(hFont);
	}
	ReleaseDC(NULL, hDC);

	MyMessageBox(NULL, mess, "LCS vs. TEXTMETRIC", MB_OK, IDI_TEXT);
}


// Test the relationship between font size vs font metrics accuracy
void Test_Point(void)
{
	HDC hDC = GetDC(NULL);

	TCHAR mess[MAX_PATH*2];
	mess[0] = 0;

	for (int i=1; i<=64; i*=2)
	{
		KLogFont lf(-PointSizetoLogical(hDC, 24*i), "Times New Roman"); 
		HFONT hFont = lf.CreateFont();
		SelectObject(hDC, hFont);

		TEXTMETRIC tm; 
		GetTextMetrics(hDC, & tm);
		wsprintf(mess + _tcslen(mess), "%d point lfHeight=%d, tmHeight=%d\n", 24*i, lf.m_lf.lfHeight, tm.tmHeight);

		SelectObject(hDC, GetStockObject(ANSI_VAR_FONT));
		DeleteObject(hFont);
	}
	ReleaseDC(NULL, hDC);

	MyMessageBox(NULL, mess, "Point Size vs. TEXTMETRIC", MB_OK, IDI_TEXT);
}


void Demo_CoordinateSystem(HDC hDC, const RECT * rcPaint)
{
	Demo_Metrics(hDC, hDC, 30,  30, MM_TEXT,      10);
	Demo_Metrics(hDC, hDC, 30,  60, MM_LOENGLISH, 10);
	Demo_Metrics(hDC, hDC, 30,  90, MM_HIENGLISH, 10);
	Demo_Metrics(hDC, hDC, 30, 120, MM_TWIPS,     10);

	PRINTDLG pd;
	HWND hwnd = NULL;

	// Initialize PRINTDLG
	ZeroMemory(&pd, sizeof(PRINTDLG));
	pd.lStructSize = sizeof(PRINTDLG);
	pd.hwndOwner   = hwnd;
	pd.hDevMode    = NULL;      // Don't forget to free or store hDevMode.
	pd.hDevNames   = NULL;      // Don't forget to free or store hDevNames.
	pd.Flags       = PD_RETURNDC | PD_RETURNDEFAULT; 
	pd.nCopies     = 1;
	pd.nFromPage   = 0xFFFF; 
	pd.nToPage     = 0xFFFF; 
	pd.nMinPage    = 1; 
	pd.nMaxPage    = 0xFFFF; 

	// default printer
	if ( PrintDlg(&pd)) 
	{
		Demo_Metrics(hDC, pd.hDC, 30,  150, MM_TEXT, 10);
	
		// 1800-dpi ultra-high-resolution logical coordinate system
		// Demo_Metrics(hDC, pd.hDC, 30, 200, MM_ANISOTROPIC);
		// Demo_Metrics(hDC, hDC,    30, 230, MM_ANISOTROPIC);

		// Delete DC when done.
		DeleteDC(pd.hDC);
	}

	// reference
	Demo_Metrics(hDC, hDC, 30, 200, MM_ANISOTROPIC, 15, 1);
	
	// test changing logic coordinate on metrics accuracy
	Demo_Metrics(hDC, hDC, 30, 230, MM_ANISOTROPIC, 15, 5);
	Demo_Metrics(hDC, hDC, 30, 260, MM_ANISOTROPIC, 15, 10);
	Demo_Metrics(hDC, hDC, 30, 290, MM_ANISOTROPIC, 15, 20);
	
	// test font size on metrics accuracy
	Demo_Metrics(hDC, hDC, 30, 320, MM_ANISOTROPIC,  75, 1);
	Demo_Metrics(hDC, hDC, 30, 350, MM_ANISOTROPIC, 150, 1);
	Demo_Metrics(hDC, hDC, 30, 380, MM_ANISOTROPIC, 300, 1);
}


void Demo_TextAlignment(HDC hDC, const RECT * rcPaint)
{
	KLogFont lf(-PointSizetoLogical(hDC, 48), "Georgia");

	{
		const TCHAR * mess[] = { "TA_TOP | TA_LEFT", "TA_BASELINE | TA_CENTER", "TA_BOTTOM | TA_RIGHT" };

		TextOut(hDC,  50, 10, mess[0], _tcslen(mess[0]));
		TextOut(hDC, 200, 10, mess[1], _tcslen(mess[1]));
		TextOut(hDC, 400, 10, mess[2], _tcslen(mess[2]));
	}

	KGDIObject font(hDC, lf.CreateFont());

	SetTextColor(hDC, RGB(0, 0, 0));          // black
	SetBkColor(hDC,   RGB(0xD0, 0xD0, 0xD0)); // gray
	SetBkMode(hDC,    OPAQUE);			      // opaque

	int x = 50; 
	int y = 110;
	
	const TCHAR * mess = "Align";

	for (int i=0; i<3; i++, x+=250)
	{
		const UINT Align[] = { TA_TOP | TA_LEFT, TA_BASELINE | TA_CENTER, TA_BOTTOM | TA_RIGHT };
			
		SetTextAlign(hDC, Align[i] | TA_UPDATECP);
		MoveToEx(hDC, x, y, NULL);			// set cp	
		TextOut(hDC, x, y, mess, _tcslen(mess));
		POINT cp;
		MoveToEx(hDC, 0, 0, & cp);			// get cp
		Line(hDC, cp.x-5, cp.y+5, cp.x+5, cp.y-5); // mark cp
		Line(hDC, cp.x-5, cp.y-5, cp.x+5, cp.y+5);

		Line(hDC, x, y-75, x, y+75); // vertical refer line

		SIZE size;

		GetTextExtentPoint32(hDC, mess, _tcslen(mess), & size);

		Box(hDC, x, y, x + size.cx, y + size.cy);
	}
	x -= 250 * 3;
	Line(hDC, x-20, y, x+520, y); // horizontal refer line
}


void Demo_RTL(HDC hDC, const RECT * rcPaint)
{
//	LCID lcid = GetThreadLocale();
//	SetThreadLocale(MAKELCID(MAKELANGID(LANG_ARABIC, SUBLANG_ARABIC_EGYPT), SORT_DEFAULT));
	
	KLogFont lf(-PointSizetoLogical(hDC, 36), "Lucida Sans Unicode");
	lf.m_lf.lfCharSet = ARABIC_CHARSET;
	lf.m_lf.lfQuality = ANTIALIASED_QUALITY;

	KGDIObject font(hDC, lf.CreateFont());
	TEXTMETRIC tm;
	GetTextMetrics(hDC, & tm);
	int linespace = tm.tmHeight + tm.tmExternalLeading;

	const TCHAR * mess = "1-360-212-0000 \xD0\xD1\xD2";
	
	for (int i=0; i<4; i++)
	{
		if ( i & 1 )
			SetTextAlign(hDC, TA_TOP | TA_LEFT | TA_RTLREADING);
		else
			SetTextAlign(hDC, TA_TOP | TA_LEFT);

		if ( i & 2 )
			SetLayout(hDC, LAYOUT_RTL);
		else
			SetLayout(hDC, 0);
		
		TextOut(hDC, 10, 10 + linespace * i, mess, _tcslen(mess));
	}

//	SetThreadLocale(lcid);
}


void Demo_Justification(HDC hDC, const RECT * rcPaint)
{
	SetTextColor(hDC, RGB(0, 0, 0));          // black
	SetBkColor(hDC,   RGB(0xE0, 0xE0, 0xE0)); // gray
	SetBkMode(hDC,    OPAQUE);			      // opaque

	{
		KLogFont   lf(-PointSizetoLogical(hDC, 48), "Georgia");
		KGDIObject font(hDC, lf.CreateFont());

		int x = 50; 
		int y = 50;
	
		const TCHAR * mess = "Extra";

		SetTextAlign(hDC, TA_LEFT | TA_TOP);

		for (int i=0; i<3; i++)
		{
			SetTextCharacterExtra(hDC, i*10-10);
			TextOut(hDC, x, y, mess, _tcslen(mess));

			SIZE size;
			GetTextExtentPoint32(hDC, mess, _tcslen(mess), & size);
			Box(hDC, x, y, x + size.cx, y + size.cy);

			y += size.cy + 10;
		}

		SetTextCharacterExtra(hDC, 0);
	}

	{
		KLogFont   lf(-PointSizetoLogical(hDC, 16), "Times New Roman");
		KGDIObject font(hDC, lf.CreateFont());

		int left  = 300;
		int right = 600;
		int y     =  50;

		TEXTMETRIC tm;
		GetTextMetrics(hDC, & tm);
		int ls = tm.tmHeight + tm.tmExternalLeading;

		const TCHAR * line = "The quick brown fox jumps over the lazy dog.";
		const TCHAR * pend = line;

		while ( pend )
		{
			pend = _tcschr(pend, ' ');
			int nCount;

			if ( pend )
				nCount = ((int) pend - (int) line) / sizeof(TCHAR);
			else
				nCount = _tcslen(line);

			TextOutJust(hDC, left, right, y, line, nCount, true);
			y += ls;

			if ( pend )
				pend ++;
		}
		
		SetTextJustification(hDC, 0, 0);

		TextOut(hDC, left, y+1, line, _tcslen(line));

		// Line(hDC, left,  180, left, 340);
		// Line(hDC, right, 180, right, 340);
	}
}


void Demo_CharacterWidth(HDC hDC, const RECT * rcPaint)
{
	KLogFont lf(-PointSizetoLogical(hDC, 144), "Georgia");
	lf.m_lf.lfItalic = TRUE;
	lf.m_lf.lfQuality = ANTIALIASED_QUALITY;

	SetTextColor(hDC, RGB(0, 0, 0));          // black
	SetBkColor(hDC,   RGB(0xD0, 0xD0, 0xD0)); // gray
	SetBkMode(hDC,    OPAQUE);			      // opaque

	int x = 100; 
	int y =  80;
	
	const TCHAR * mess = "font";
	ABC abc[128];
	SIZE size;

	{
		KGDIObject font(hDC, lf.CreateFont());
	
		SetTextAlign(hDC, TA_LEFT | TA_TOP);
		TextOut(hDC, x, y, mess, _tcslen(mess));
	
		GetCharABCWidths(hDC, 0, 0x7F, abc);

		GetTextExtentPoint32(hDC, mess, _tcslen(mess), & size);

		int xx = x;
		int yy = 0;

		for (int i=0; mess[i]; i++)
		{
			ABC & ch = abc[mess[i]];

			Line(hDC, xx, y-25, xx, y+size.cy+25);			// current cursor position
		
			if ( ch.abcA < 0 )
			{
				RECT rect = { xx, y-25, xx+ch.abcA, y-5 };
				FillRect(hDC, & rect, (HBRUSH) GetStockObject(GRAY_BRUSH));
			}
			xx += ch.abcA;
			Line(hDC, xx, y-25, xx, y-5);					// cp + A

			xx += ch.abcB;
			Line(hDC, xx, y+size.cy+5, xx, y+size.cy+25);   // cp + A + B

			if ( ch.abcC < 0 )
			{
				RECT rect = { xx, y+size.cy+5, xx+ch.abcC, y+size.cy+25 };
				FillRect(hDC, & rect, (HBRUSH) GetStockObject(GRAY_BRUSH));
			}
			xx += ch.abcC;
		}
		Line(hDC, xx, y-25, xx, y+size.cy+25);			// current cursor position
	
		Box(hDC, x, y-40, x + size.cx, y - 50);
	}

	SetBkColor(hDC,   RGB(0xFF, 0xFF, 0xFF)); // white
	
	int sum = 0;

	x += size.cx + 55;
	y -= 40;

	TCHAR temp[64];

	for (int i=0; mess[i]; i++)
	{
		ABC & ch = abc[mess[i]];

		sum += ch.abcA + ch.abcB + ch.abcC;

		wsprintf(temp, "'%c'(0x%02x): %d %d %d", mess[i], mess[i], ch.abcA, ch.abcB, ch.abcC);
		TextOut(hDC, x, y, temp, _tcslen(temp));
		y+= 25;
	}

	wsprintf(temp, "ABC widths sum: %d", sum);
	TextOut(hDC, x, y, temp, _tcslen(temp)); y+= 25;

	wsprintf(temp, "GetTextExtent %d %d", size.cx, size.cy);
	TextOut(hDC, x, y, temp, _tcslen(temp)); y+= 25;

	{
		ABC  abc;
		long height;

		{
			KLogFont lf(-PointSizetoLogical(hDC, 144), "Georgia");
			lf.m_lf.lfItalic = TRUE;

			KGDIObject font(hDC, lf.CreateFont());

			GetTextABCExtent(hDC, mess, _tcslen(mess), & height, & abc);
		}

		wsprintf(temp, "GetTextABCExtent %d %d %d", abc.abcA, abc.abcB, abc.abcC);
		TextOut(hDC, x, y, temp, _tcslen(temp)); y+= 25;
	}

	{
		KLogFont lf(-PointSizetoLogical(hDC, 48), "Georgia");
		lf.m_lf.lfItalic = TRUE;
		lf.m_lf.lfQuality = ANTIALIASED_QUALITY;

		KGDIObject font(hDC, lf.CreateFont());
		SetBkColor(hDC,   RGB(0xD0, 0xD0, 0xD0)); // gray
		SetBkMode(hDC,    OPAQUE);			      // opaque

		const TCHAR * mess = "faroff";
		int           len  = _tcslen(mess);
		
		SIZE size;
		GetTextExtentPoint32(hDC, mess, len, & size);

		int left  = 100;
		int y     = 350;
		
		int right = left + size.cx * 3/ 2;
		int dx    = size.cx * 5/3;
		int dy    = size.cy * 11 / 10;		
		
		SetTextAlign(hDC, TA_LEFT);
		TextOut(hDC, left, y, mess, len);
		PreciseTextOut(hDC, left+dx, y, mess, len);

		SetTextAlign(hDC, TA_CENTER);
		TextOut(hDC, (left+right)/2, y+dy, mess, len);
		PreciseTextOut(hDC, (left+right)/2+dx, y+dy, mess, len);
		
		SetTextAlign(hDC, TA_RIGHT);
		TextOut(hDC, right, y+dy*2, mess, len);
		PreciseTextOut(hDC, right+dx, y+dy*2, mess, len);
		
		Line(hDC, left, y-5, left, y+dy*3+5);
		Line(hDC, right, y-5, right, y+dy*3+5);

		Line(hDC, left+dx, y-5, left+dx, y+dy*3+5);
		Line(hDC, right+dx, y-5, right+dx, y+dy*3+5);
	}
}


void dispsmall(HDC hDC, int x, int y, int row, const TCHAR * mess)
{
	HGDIOBJ hOld = SelectObject(hDC, GetStockObject(ANSI_FIXED_FONT));
	int oldmd    = SetBkMode(hDC, TRANSPARENT);

	TEXTMETRIC tm;
	GetTextMetrics(hDC, & tm);

	TextOut(hDC, x, y + row * ( tm.tmHeight + tm.tmExternalLeading-1), mess, _tcslen(mess));

	SelectObject(hDC, hOld);
	SetBkMode(hDC, oldmd);
}


void Demo_CharacterPlacement(HDC hDC, const RECT * rcPaint)
{
	KLogFont lf(-PointSizetoLogical(hDC, 60), "Times New Roman");
	lf.m_lf.lfItalic  = TRUE;
	lf.m_lf.lfQuality = ANTIALIASED_QUALITY;

	SetTextColor(hDC, RGB(0, 0, 0));          // black
	SetBkColor(hDC,   RGB(0xD0, 0xD0, 0xD0)); // gray
	SetBkMode(hDC,    OPAQUE);			      // opaque

	int x = 32; 
	int y = 32;
	
	KGDIObject font(hDC, lf.CreateFont());

	const TCHAR mess [] = "AVOWAL";

	// originl text out
	TextOut(hDC, x, y, mess, _tcslen(mess));

	KPlacement<MAX_PATH> placement;

	TEXTMETRIC tm;
	GetTextMetrics(hDC, & tm);
	int linespace = tm.tmHeight + tm.tmExternalLeading;

	KKerningPair kerning(hDC);

	SIZE size;
	GetTextExtentPoint32(hDC, mess, _tcslen(mess), & size);

	for (int test=0; test<3; test++)
	{
		y += linespace;
	
		int opt;

		switch ( test )
		{
			case 0: opt = 0; break;
			case 1: opt = GCP_USEKERNING; break;
			case 2: opt = GCP_USEKERNING | GCP_JUSTIFY; break;
		}
		
		placement.GetPlacement(hDC, mess, opt | GCP_MAXEXTENT, size.cx*11/10);
		placement.GlyphTextOut(hDC, x, y);

		int xx = x;
		TCHAR temp[128];
		
		for (unsigned i=0; mess[i]; i++)
		{
			Line(hDC, xx, y, xx, y + linespace);

			wsprintf(temp, "gi('%c')=%3d, dx[%2d]=%3d", 
				placement.m_strOut[i], placement.m_glyphs[i],
				i, placement.m_dx[placement.m_order[i]]);

			if ( (test==1) && mess[i+1] )
				wsprintf(temp+_tcslen(temp), ", kern('%c','%c')=%3d", 
					mess[i], mess[i+1], kerning.GetKerning(mess[i], mess[i+1]));

		//	if ( (test==2) && (i==0) )
		//		wsprintf(temp+_tcslen(temp), "extent %d", size.cx*11/10);

			dispsmall(hDC, x + size.cx * 12/10, y, i, temp);

			xx += placement.m_dx[placement.m_order[i]];
		}

		wsprintf(temp, "extent %d, sum dx=%d", size.cx*11/10, xx-x);
		dispsmall(hDC, x + size.cx * 12/10, y, _tcslen(mess), temp);
		
		Line(hDC, xx, y, xx, y + linespace);
	}


	// RTL reading
	{
		KLogFont lf(-PointSizetoLogical(hDC, 48), "Andalus");
		lf.m_lf.lfCharSet = ARABIC_CHARSET;
		lf.m_lf.lfQuality = ANTIALIASED_QUALITY;

		KGDIObject font(hDC, lf.CreateFont());
		
		if ( GetFontLanguageInfo(hDC) & GCP_REORDER )
			disp(hDC, 500, 410, "GCP_REORDER supported");
		else
			disp(hDC, 500, 410, "GCP_REORDER not supported");

		KPlacement<MAX_PATH> placement;
				
		const TCHAR * mess = "abc \xC7\xC8\xC9\xCA";
	
		SIZE size;
		GetTextExtentPoint32(hDC, mess, _tcslen(mess), & size);
		
		TEXTMETRIC tm;
		GetTextMetrics(hDC, & tm);
		int linespace = tm.tmHeight + tm.tmExternalLeading + 5;
				
		for (test=0; test<2; test++)
		{
			int y = 420 + linespace * (test+1);
			int x = 500;
		
			SetTextAlign(hDC, TA_LEFT | TA_RTLREADING);
			placement.GetPlacement(hDC, mess, (test==0) ? 0 : GCP_REORDER);
			placement.GlyphTextOut(hDC, x, y);
			SetTextAlign(hDC, TA_LEFT);

			TCHAR temp[128];
		
			for (unsigned i=0; mess[i]; i++)
			{
				wsprintf(temp, "order[%d]=%d, gi(0x%02x)=%3d, dx[%d]=%3d", 
					i, placement.m_order[i],
					placement.m_strOut[i] & 0xFF, placement.m_glyphs[i] & 0xFFFF,
					i, placement.m_dx[placement.m_order[i]]);

				dispsmall(hDC, x + size.cx * 12/10, y, i, temp);
			}
		}
	}

	// extra kerning samples
	{
		KLogFont lf(-PointSizetoLogical(hDC, 48), "Tahoma");
		lf.m_lf.lfQuality = ANTIALIASED_QUALITY;

		KGDIObject font(hDC, lf.CreateFont());

		const TCHAR mess1[] = "F,) Ta r.";  
		const TCHAR mess2[] = "’‘ f] f}";

		// originl text out
		int x = 50;
		int y = 420;
		TEXTMETRIC tm;
		GetTextMetrics(hDC, & tm);

		TextOut(hDC, x, y, mess1, _tcslen(mess1));

		KPlacement<MAX_PATH> placement;

		placement.GetPlacement(hDC, mess1, GCP_USEKERNING);
		placement.GlyphTextOut(hDC, x, y+tm.tmHeight + tm.tmExternalLeading+10);

		x += 250;

		TextOut(hDC, x, y, mess2, _tcslen(mess2));

		placement.GetPlacement(hDC, mess2, GCP_USEKERNING);
		placement.GlyphTextOut(hDC, x, y+tm.tmHeight + tm.tmExternalLeading+10);

	}

}


void Demo_GlyphIndex(HDC hDC, const RECT *)
{
	KLogFont lf(-PointSizetoLogical(hDC, 12), "Courier New");
	lf.m_lf.lfQuality = ANTIALIASED_QUALITY;

	KGDIObject font(hDC, lf.CreateFont());

	// query for size
	DWORD size = GetFontUnicodeRanges(hDC, NULL);

	GLYPHSET * pGlyphSet = (GLYPHSET *) new BYTE[size];

	// get real data
	pGlyphSet->cbThis = size;
	size = GetFontUnicodeRanges(hDC, pGlyphSet);

	int x = 20;
	int y = 20;

	TEXTMETRIC tm;
	GetTextMetrics(hDC, & tm);

	for (unsigned i=0; i<min(16, pGlyphSet->cRanges); i++)
	{
		TCHAR text[MAX_PATH];
		wsprintf(text, "%3d 0x%04X %3d", i, pGlyphSet->ranges[i].wcLow, pGlyphSet->ranges[i].cGlyphs);
		TextOut(hDC, x, y, text, _tcslen(text));
		
		WCHAR sample[8];
		WORD  glyphindex[8];

		int cGlyph = min(pGlyphSet->ranges[i].cGlyphs, 8);
		
		for (int j=0; j<cGlyph; j++)
			sample[j] = pGlyphSet->ranges[i].wcLow + j;

		TextOutW(hDC, x+150, y, sample, cGlyph);

		GetGlyphIndicesW(hDC, sample, cGlyph, glyphindex, 0);
			
		text[0] = 0;
		for (j=0; j<cGlyph; j++)
		{
			wsprintf(text + _tcslen(text), "%4d ", glyphindex[j]);
		}
			
		TextOut(hDC, x+300, y, text, _tcslen(text));

		y += tm.tmHeight + tm.tmExternalLeading;
	}

	delete (BYTE *) pGlyphSet;

	// exttextout
	{
		SetTextColor(hDC, RGB(0, 0, 0));          // black
		SetBkColor(hDC,   RGB(0xD0, 0xD0, 0xD0)); // gray
		SetBkMode(hDC,    OPAQUE);			      // opaque

		x = 50;
		y = 350;

		KLogFont lf(-PointSizetoLogical(hDC, 36), "Tahoma");
		lf.m_lf.lfQuality = ANTIALIASED_QUALITY;
		lf.m_lf.lfCharSet = GB2312_CHARSET;

		KGDIObject font(hDC, lf.CreateFont());
		TEXTMETRIC tm;
		GetTextMetrics(hDC, & tm);

		const TCHAR * mess = "3.14159";

		RECT rect = { x-10, y+40, x+250, y+20 };

		ExtTextOut(hDC, x, y, ETO_OPAQUE, & rect, mess, _tcslen(mess), NULL);

		y += tm.tmHeight + tm.tmExternalLeading;

		RECT rect1 = { x-10, y+40, x+250, y+20 };

		ExtTextOut(hDC, x, y, ETO_CLIPPED, & rect1, mess, _tcslen(mess), NULL);

		y += tm.tmHeight + tm.tmExternalLeading;

		RECT rect2 = { x-10, y+40, x+250, y+20 };

		ExtTextOut(hDC, x, y, ETO_CLIPPED | ETO_OPAQUE, & rect2, mess, _tcslen(mess), NULL);

		y += tm.tmHeight + tm.tmExternalLeading;

		ExtTextOut(hDC, x, y, ETO_NUMERICSLATIN, NULL, mess, _tcslen(mess), NULL);

		int dx[] = { 30, 30, 30, 30, 30, 30, 30, 30 };
		y += tm.tmHeight + tm.tmExternalLeading;
		
		ExtTextOut(hDC, x, y, ETO_NUMERICSLOCAL, NULL, mess, _tcslen(mess), dx);
	}

}


//////////////////////////////////////////


void Demo_GlyphOutline(HDC hDC)
{
	KLogFont lf(-PointSizetoLogical(hDC, 96), "Times New Roman");
	lf.m_lf.lfItalic  = TRUE;
	lf.m_lf.lfQuality = ANTIALIASED_QUALITY;

	KGDIObject font(hDC, lf.CreateFont());

	int x = 20; int y = 160;
	int dx, dy;
	
	SetTextAlign(hDC, TA_BASELINE | TA_LEFT);

	SetBkColor(hDC,   RGB(0xFF, 0xFF, 0)); // yellow
	SetTextColor(hDC, RGB(0, 0, 0xFF));	   // blue
//	SetBkMode(hDC, TRANSPARENT);

//	MAT2 mat2;
//	mat2.eM11 = MakeFixed(cos(0.1));
//	mat2.eM12 = MakeFixed(sin(0.1));
//	mat2.eM21 = MakeFixed(-sin(0.1));
//	mat2.eM22 = MakeFixed(cos(0.1));

	KGlyph glyph;

	TextOut(hDC, x, y, "1248", 4); y+= 150;

	glyph.GetGlyph(hDC, '1', GGO_BITMAP); // , & mat2);
	glyph.DrawGlyph(hDC, x, y, dx, dy);	x+=dx;

	glyph.GetGlyph(hDC, '2', GGO_GRAY2_BITMAP); // , & mat2);
	glyph.DrawGlyph(hDC, x, y, dx, dy); x+=dx;

	glyph.GetGlyph(hDC, '4', GGO_GRAY4_BITMAP); // , & mat2);
	glyph.DrawGlyph(hDC, x, y, dx, dy); x+=dx;

	glyph.GetGlyph(hDC, '8', GGO_GRAY8_BITMAP); // , & mat2);
	glyph.DrawGlyph(hDC, x, y, dx, dy); x+=dx;

	OutlineTextOut(hDC, x+50, 130, "Outline", -1);

	SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
	SetTextColor(hDC, RGB(0, 0, 0));
	
	TextOut(hDC, x+50, 250, "Outline", 7);
}



void Demo_TabbedTextOut(HDC hDC)
{
	int tabstop[] = { -120, 125, 250 };

	const TCHAR * lines[] = 
	{ 
		"Group" "\t"  "Result" "\t" "Function"    "\t" "Parameters",
		"Font"  "\t"  "DWORD"  "\t" "GetFontData" "\t" "(HDC hdc, ...)",
		"Text"  "\t"  "BOOL"   "\t" "TextOut"     "\t" "(HDC hdc, ...)"
	};

	int x=50, y=50;
	
	for (int i=0; i<3; i++)
		y += HIWORD(TabbedTextOut(hDC, x, y, lines[i], _tcslen(lines[i]), 
				sizeof(tabstop)/sizeof(tabstop[0]), tabstop, x));

//	for (i=0; i<5; i++)
//		Line(hDC, x + abs(tabstop[i]), 0, x + abs(tabstop[i]), 100);

	{
		KLogFont lf(-PointSizetoLogical(hDC, 10), "MS Shell Dlg");
		lf.m_lf.lfQuality = ANTIALIASED_QUALITY;

		KGDIObject font(hDC, lf.CreateFont());

		{
			const TCHAR * mess = 
				"The DrawText function draws formatted text in the specified rectangle. "
				"It formats the text according to the specified method (expanding tabs, "
				"justifying characters, breaking lines, and so forth).";

			int width = 170;
			int x     = 220;
			RECT rect = { x, 120, x+width, 120+60 };
	
			SetTextAlign(hDC, TA_LEFT | TA_TOP | TA_NOUPDATECP);

			int opt[] = { DT_WORDBREAK | DT_LEFT,
					  DT_WORDBREAK | DT_CENTER,
					  DT_WORDBREAK | DT_RIGHT | DT_EDITCONTROL
					};

			for (i=0; i<3; i++)
			{
				Box(hDC, rect.left-1, rect.top-1, rect.right+1, rect.bottom+1);
				DrawText(hDC, mess, _tcslen(mess), & rect, opt[i]);

				OffsetRect(&rect, 0, rect.bottom - rect.top + 10);
			}

			x += width + 10;
			{
				KLogFont lf(-PointSizetoLogical(hDC, 20), "MS Shell Dlg");
				lf.m_lf.lfQuality = ANTIALIASED_QUALITY;

				KGDIObject font(hDC, lf.CreateFont());

				RECT rect2 = { x, 120, x+width*2, 120 + 120 };
				Box(hDC, rect2.left-1, rect2.top-1, rect2.right+1, rect2.bottom+1);
				DrawText(hDC, mess, _tcslen(mess), & rect2, DT_WORDBREAK | DT_LEFT);
			}
		}

		{
			const TCHAR * mess =
				"&Open \tc:\\Win\\system32\\gdi32.dll";

			RECT rect = { 20, 120, 20+180, 120 + 32 };

			int opt[] = { DT_SINGLELINE | DT_TOP,
						  DT_SINGLELINE | DT_VCENTER | DT_EXPANDTABS,
						  DT_SINGLELINE | DT_BOTTOM  | DT_EXPANDTABS | DT_PATH_ELLIPSIS,
						  DT_SINGLELINE | DT_NOPREFIX | DT_EXPANDTABS | DT_WORD_ELLIPSIS,
						  DT_SINGLELINE | DT_HIDEPREFIX | DT_EXPANDTABS | DT_END_ELLIPSIS,
						};

			for (i=0; i<5; i++)
			{
				Box(hDC, rect.left-1, rect.top-1, rect.right+1, rect.bottom+1);
				DrawText(hDC, mess, _tcslen(mess), & rect, opt[i]);

				OffsetRect(&rect, 0, rect.bottom - rect.top + 10);
			}
		}
	}
}



void Demo_Paragraph(HDC hDC, bool bPrecise)
{
	const TCHAR * mess = 
		"The DrawText function draws formatted text in the specified rectangle. "
    	"It formats the text according to the specified method (expanding tabs, "
		"justifying characters, breaking lines, and so forth).";

	int x = 20;
	int y = 20;

	SetBkMode(hDC, TRANSPARENT);

	for (int size=6; size<=21; size+=3) // 6, 9, 12, 15, 18, 21
	{
		int width  = size * 42;
		int height = size *  5;

		KLogFont lf(-PointSizetoLogical(hDC, size), "MS Shell Dlg");
		lf.m_lf.lfQuality = ANTIALIASED_QUALITY;

		KGDIObject font(hDC, lf.CreateFont());

		RECT rect = { x, y, x+width, y+height };

		if ( bPrecise )
		{
			KTextFormator format;
		
			format.Setup(hDC, (HFONT) font.m_hObj, (float) size);

			format.DrawText(hDC, mess, _tcslen(mess), & rect, DT_WORDBREAK | DT_LEFT);
		}
		else
			DrawText(hDC, mess, _tcslen(mess), & rect, DT_WORDBREAK | DT_LEFT);

		Box(hDC, rect.left-1, rect.top-1, rect.right+1, rect.bottom+1);

		y = rect.bottom + 10;
	}	
}



void Demo_TextColor(HDC hDC, HINSTANCE hInst)
{
	KLogFont lf(-PointSizetoLogical(hDC, 16), "Times New Roman");
	lf.m_lf.lfQuality = ANTIALIASED_QUALITY;
	KGDIObject font(hDC, lf.CreateFont());

	int x = 10, y= 10;
	TEXTMETRIC tm;
	GetTextMetrics(hDC, & tm);

	SetBkMode(hDC, OPAQUE);

	for (int ci=0; ci<8; ci+=2)
	{
		const COLORREF Color[] = 
			{ RGB(0xFF, 0, 0), RGB(0, 0xFF, 0), RGB(0, 0, 0xFF),
			  RGB(0xFF, 0xFF, 0), RGB(0, 0xFF, 0xFF), RGB(0xFF, 0, 0xFF),
			  RGB(0, 0, 0), RGB(0xFF, 0xFF, 0xFF)
			};
		
		TCHAR temp[64];

		SetTextColor(hDC, Color[ci]);
		SetBkColor(hDC,   Color[ci+1]);

		wsprintf(temp, "0x%06X 0x%06X", Color[ci], Color[ci+1]);

		TextOut(hDC, x, y, temp, _tcslen(temp));

		y+= tm.tmHeight + tm.tmExternalLeading;
	}

	{
		HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0xFF));

		const TCHAR * mess = "GrayString";
		x = 260; y = 10;
	
		KLogFont lf(-PointSizetoLogical(hDC, 64), "Times New Roman");
		lf.m_lf.lfItalic  = TRUE;
		lf.m_lf.lfWeight  = FW_BOLD;
		lf.m_lf.lfQuality = ANTIALIASED_QUALITY;
		KGDIObject font(hDC, lf.CreateFont());

		GrayString(hDC, hBrush, NULL, (LPARAM) mess, _tcslen(mess), x, y, 0, 0);

		DeleteObject(hBrush);

		y+= 80;

		hBrush = CreateHatchBrush(HS_DIAGCROSS, RGB(0xFF, 0, 0));
		SetBkColor(hDC, RGB(0, 0xFF, 0xFF));
		ColorText(hDC, x, y, "ColorText", 9, hBrush);
		DeleteObject(hBrush);
		y+= 80;

		{
			KLogFont lf(-PointSizetoLogical(hDC, 72), "Tahoma");
			lf.m_lf.lfQuality = ANTIALIASED_QUALITY;
			lf.m_lf.lfWeight  = FW_BOLD;
			KGDIObject font(hDC, lf.CreateFont());

			HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TIGER));
			    BitmapText(hDC, x, y, "TIGER", 5, hBmp);

			BitmapText2(hDC, x, y+100, "TIGER", 5, hBmp);
			
			DeleteObject(hBmp);
		}
	}
}


typedef struct
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD			 bmiColors[256];
}	BITMAPINFO4BPP;

bool Disp(const TCHAR * pFileName, HDC hDC)
{
	if ( pFileName==NULL )
		return false;

	HANDLE handle = CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if ( handle == INVALID_HANDLE_VALUE )
		return false;

	DWORD size = GetFileSize(handle, NULL);

	BYTE * pBits = new BYTE[size];
	
	DWORD dwRead;
	ReadFile(handle, pBits, size, & dwRead, NULL);
	CloseHandle(handle);

	BITMAPINFO4BPP bmi;
	memset(& bmi, 0, sizeof(bmi));
	
	int width  = 864;
	int height = 1763;

	bmi.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth    = width;
	bmi.bmiHeader.biHeight   = height; // top-down DIB
	bmi.bmiHeader.biPlanes   = 1;
	bmi.bmiHeader.biBitCount = 4;

	for (int i=0; i<16; i++)
	{
		bmi.bmiColors[i].rgbRed   = i * 255 / 15;
		bmi.bmiColors[i].rgbGreen = i * 255 / 15;
		bmi.bmiColors[i].rgbBlue  = i * 255 / 15;
	}

	int bps = (width * 4 + 31)/32 * 4;


	BYTE * p = pBits;

	for (i=0; i<height; i++)
	{
		BYTE temp[512];

		for (int j=0; j<bps/2; j++)
			temp[j] = p[j*2];

		for (j=bps/2; j<bps; j++)
			temp[j] = p[j*2+1];

		memcpy(p, temp, bps);

		p += bps;
	}

	StretchDIBits(hDC, 0, 0, width, height, 0, 0, width, height,
		pBits, (BITMAPINFO *) & bmi, DIB_RGB_COLORS, SRCCOPY);
	
	delete [] pBits;

	return true;
}

void Demo_TextStyle(HDC hDC)
{
	SetBkColor(hDC, RGB(0xFF, 0xFF, 0));
	SetTextColor(hDC, RGB(0, 0, 0xFF));

	for (int f=0; f<2; f++)
	{
		for (int i=0; i<11; i++)
		{
			KLogFont lf(-PointSizetoLogical(hDC, 48), "Times New Roman");

			if ( f==1 )
				_tcscpy(lf.m_lf.lfFaceName, "Colonna MT");

			SetBkColor(hDC, RGB(0xFF, 0xFF, 0));
			SetTextColor(hDC, RGB(0, 0, 0xFF));

			int x = 10 + f*520 + (i/4)*145; 
			int y = 10 +         (i%4)* 80;
	
			switch ( i+2 )
			{
				case 2: lf.m_lf.lfWeight  = FW_NORMAL; break;
			//	case 0: lf.m_lf.lfWeight  = FW_THIN;   break;
			//	case 1: lf.m_lf.lfWeight  = FW_BOLD;   break;
				case 3: lf.m_lf.lfWeight  = FW_BLACK;  break;
			
				case 4: lf.m_lf.lfQuality = ANTIALIASED_QUALITY; break;
			
				case 5:	lf.m_lf.lfItalic  = TRUE;				 break;

				case 6: lf.m_lf.lfItalic  = TRUE; 
						lf.m_lf.lfWeight  = FW_BOLD;              break;

				case 7: lf.m_lf.lfUnderline = TRUE;
						break;

				case 8: lf.m_lf.lfStrikeOut = TRUE;
						break;

				case 9: SetTextColor(hDC, RGB(0xFF, 0xFF, 0));
						SetBkColor(hDC, RGB(0, 0, 0xFF));
						break;
			}
		
			KGDIObject font(hDC, lf.CreateFont());

			const TCHAR * mess = "Style";

			SetBkMode(hDC, OPAQUE);

			switch ( i+2 )
			{
				case 10: 
					OffsetTextOut(hDC, x, y, "Shadow", 6, 4, 4, 
						GetSysColor(COLOR_3DSHADOW), 0, 0, 0);
					break;

				case 11: 
					SetTextColor(hDC, RGB(0xFF, 0xFF, 0));
					SetBkColor(hDC, RGB(0xD0, 0xD0, 0));
					OffsetTextOut(hDC, x, y, "Emboss", 6, 
						-1, -1, GetSysColor(COLOR_3DLIGHT),
						 1,  1, GetSysColor(COLOR_3DDKSHADOW));
					break;

				case 12: 
					SetTextColor(hDC, RGB(0xD0, 0xD0, 0));
					OffsetTextOut(hDC, x, y, "Engrave", 7,
						-1, -1, GetSysColor(COLOR_3DDKSHADOW),
						 1,  1, GetSysColor(COLOR_3DLIGHT));
					break;

				default:
					TextOut(hDC, x, y, mess, _tcslen(mess));
			}
		}
	}

//	LoadFile("c:\\mail_img.bin", hDC);
}

const double pi = 3.141592654;

HFONT ScaleFont(HDC hDC, int percent)
{
	LOGFONT lf;

	GetObject(GetCurrentObject(hDC, OBJ_FONT), sizeof(lf), & lf);

	TEXTMETRIC tm;
	GetTextMetrics(hDC, & tm);
	lf.lfWidth = (tm.tmAveCharWidth * percent + 50)/100;

	return CreateFontIndirect(&lf);
}

void Demo_TextGeometry(HDC hDC)
{
	KLogFont lf(-PointSizetoLogical(hDC, 16), "MS Shell Dlg");
	lf.m_lf.lfQuality    = ANTIALIASED_QUALITY;

	SetBkColor(hDC, RGB(0xFF, 0xFF, 0));
	SetTextAlign(hDC, TA_LEFT | TA_TOP);

	int x0 =  10;
	int y0 = 310;

	for (int i=0; i<10; i++)
	{
		int degree = i * 10;
		int r      = 160;

		lf.m_lf.lfEscapement  = degree * 10;
		lf.m_lf.lfOrientation = 0;

		KGDIObject font(hDC, lf.CreateFont());

		TCHAR mess[32];
		wsprintf(mess, "%02d° deg", degree);

		int x =  x0 + (int)( r * cos(degree*pi/180) );
		int y =  y0 - (int)( r * sin(degree*pi/180) );

		if ( i < 5 )
		{
			SetGraphicsMode(hDC, GM_ADVANCED);
			SetBkMode(hDC, OPAQUE); // TRANSPARENT);
		}
		
		TextOut(hDC, x, y, mess, _tcslen(mess));

		if ( i < 5 )
		{
			SetGraphicsMode(hDC, GM_COMPATIBLE);
			SetBkMode(hDC, OPAQUE);
		}

		// MoveToEx(hDC, x0, y0, NULL);
		// LineTo(hDC, x, y);
	}

	x0 = 400;
	y0 =  20;
	{
		KLogFont lf(-PointSizetoLogical(hDC, 24), "@SimSun");
		lf.m_lf.lfQuality = ANTIALIASED_QUALITY;
		lf.m_lf.lfCharSet = GB2312_CHARSET;
		lf.m_lf.lfEscapement  = 2700;
		lf.m_lf.lfOrientation = 2700;
		
		KGDIObject font(hDC, lf.CreateFont());

		SetBkColor(hDC, RGB(0xFF, 0xFF, 0));

		WCHAR line1[] = { 0x59D1, 0x82CF, 0x57CE, 0x5916, 0x5BD2, 0x5C71, 0x5BFA };
		WCHAR line2[] = { 0x591C, 0x534A, 0x949F, 0x58F0, 0x5230, 0x5BA2, 0x8239 };

		TextOutW(hDC, x0, y0, line1, 7);  x0 -= 45;
		TextOutW(hDC, x0, y0, line2, 7);  x0 -= 50;
		TextOut (hDC, x0, y0, "270 degree", 10);
	}

	
	x0 = 430;
	y0 = 20;

	{
		KLogFont lf(-PointSizetoLogical(hDC, 24), "Times New Roman");
		lf.m_lf.lfQuality = ANTIALIASED_QUALITY;

		int linespace;
		KGDIObject font(hDC, lf.CreateFont());
		
		TEXTMETRIC tm;

		GetTextMetrics(hDC, & tm);

		linespace = tm.tmHeight + tm.tmExternalLeading;

		for (int ratio=25; ratio<=125; ratio+=25)
		{
			KGDIObject font(hDC, ScaleFont(hDC, ratio));
			
			TCHAR temp[64];

			wsprintf(temp, "%d%c W", ratio, '%');
			TextOut(hDC, x0, y0, temp, _tcslen(temp));

			SIZE size; 
			GetTextExtentPoint32(hDC, temp, _tcslen(temp), & size);
			y0 += size.cy + 5;
		}
	}

	{
		BeginPath(hDC);
		MoveToEx(hDC, 20, 40, NULL);

		POINT p[] = { 200, 0, 300, 500, 560, 250 };
		PolyBezierTo(hDC, p, 3);
		EndPath(hDC);

		KLogFont lf(-PointSizetoLogical(hDC, 22), "Times New Roman");
		lf.m_lf.lfQuality = ANTIALIASED_QUALITY;
		KGDIObject font(hDC, lf.CreateFont());

		SetTextAlign(hDC, TA_LEFT | TA_BASELINE);
//		SetBkMode(hDC, TRANSPARENT);

		PathTextOut(hDC, "Text on a curve is possible with some simple math.");
	}

}


void Demo_TextasBitmap(HDC hDC)
{
	KLogFont lf(-PointSizetoLogical(hDC, 48), "Times New Roman");
	lf.m_lf.lfWeight  = FW_BOLD;
	lf.m_lf.lfItalic  = TRUE;
	lf.m_lf.lfQuality = ANTIALIASED_QUALITY;
	KGDIObject font(hDC, lf.CreateFont());

	TEXTMETRIC tm;
	GetTextMetrics(hDC, & tm);
	int linespace = tm.tmHeight + tm.tmExternalLeading;

	SetTextAlign(hDC, TA_TOP | TA_LEFT);
	SetBkMode(hDC, TRANSPARENT);
	SetBkColor(hDC, RGB(0xFF, 0xFF, 0));

	// reflection
	{
		SaveDC(hDC);

		SetMapMode(hDC, MM_ANISOTROPIC);
		SetWindowExtEx(hDC, 1, 1, NULL);
		SetViewportExtEx(hDC, -1, -1, NULL);
		SetViewportOrgEx(hDC, 300, 100, NULL);

		ShowAxes(hDC, 600, 180);

		int x= 0, y = 0;

		const TCHAR * mess = "Reflection";
	
		SetTextAlign(hDC, TA_LEFT | TA_BASELINE);
		SetTextColor(hDC, RGB(0, 0, 0xFF)); // blue ( dark )
		BitmapTextOut(hDC, x, y, mess, _tcslen(mess), GGO_GRAY4_BITMAP);

		SetTextColor(hDC, RGB(0xFF, 0xFF, 0)); // yellow (ligh)
			TextOut(hDC, x, y, mess, _tcslen(mess));

		RestoreDC(hDC, -1);
	}

	{
		int x = 20;
		int y = 70;

		KLogFont lf(-PointSizetoLogical(hDC, 48), "Times New Roman");
		lf.m_lf.lfWeight = FW_BOLD;
		lf.m_lf.lfQuality= ANTIALIASED_QUALITY;
		KGDIObject font(hDC, lf.CreateFont());

		const TCHAR * mess = "Raster";

		SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
		SetTextColor(hDC, RGB(0xFF, 0, 0));
		BitmapTextOutROP(hDC, x, y, mess, _tcslen(mess), SRCAND);

		SetBkColor(hDC, RGB(0, 0, 0));
		SetTextColor(hDC, RGB(0, 0xFF, 0));
		BitmapTextOutROP(hDC, x+5, y+5, mess, _tcslen(mess), SRCINVERT);

		{
			x = 10;
			y = 160;

			KTextBitmap bmp;

			SetBkMode(hDC, OPAQUE);
			SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
			SetTextColor(hDC, RGB(0, 0, 0xFF));

			const TCHAR * mess = "fuzzy";
			bmp.Convert(hDC, mess, _tcslen(mess), 7);
			
			for (int i=0; i<3; i++)
			{
				bmp.Draw(hDC, x, y);

				x += bmp.m_width + 10;

				bmp.Blur();
				bmp.Blur();
			}
		}

		{
			x = 10;
			y = 250;

			KTextBitmap bmp;

			SetBkMode(hDC, OPAQUE);
			SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));   // white
			SetTextColor(hDC, RGB(0x80, 0x80, 0x80)); // gray

			const TCHAR * mess = "Soft-Shadow";
			bmp.Convert(hDC, mess, _tcslen(mess), 7);
			
			for (int i=0; i<8; i++)
				bmp.Blur();

			bmp.Draw(hDC, x, y);
			
			SetBkMode(hDC, TRANSPARENT);
			SetTextColor(hDC, RGB(0, 0, 0xFF)); // blue
			TextOut(hDC, x-5, y-5, mess, _tcslen(mess));
		}
	}
}


const BITMAPINFO * LoadDIB(HMODULE hModule, LPCTSTR pBitmapName)
{
	HRSRC   hRes = FindResource(hModule, pBitmapName, RT_BITMAP);

	if ( hRes==NULL )
		return NULL;

	HGLOBAL hGlb = LoadResource(hModule, hRes);

	if ( hGlb==NULL )
		return NULL;

	return (const BITMAPINFO *) LockResource(hGlb);
}

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


void DispResBitmap(HDC hDC, int x, int y, int resid, int & width, int & height)
{
	const BITMAPINFO * pBMI = LoadDIB(
		(HINSTANCE) GetWindowLong(WindowFromDC(hDC), GWL_HINSTANCE), 
		MAKEINTRESOURCE(resid));
	
	width  = pBMI->bmiHeader.biWidth;
	height = abs(pBMI->bmiHeader.biHeight);

	const BYTE * pBits = (const BYTE *) & pBMI->bmiColors[GetDIBColorCount(pBMI->bmiHeader)];

	StretchDIBits(hDC, x, y, width, height, 
		0, 0, width, height, pBits, pBMI, DIB_RGB_COLORS, SRCCOPY);
}


void Demo_TextasBitmap2(HDC hDC)
{
	int x = 50;
	int y = 50;

	KLogFont lf(-PointSizetoLogical(hDC, 36), "Times New Roman");
	KGDIObject font(hDC, lf.CreateFont());

	for (int i=0; i<2; i++)
	{
		int width, height;

		DispResBitmap(hDC, x, y, IDB_FULLTIGER, width, height);

		const TCHAR * mess = "Siberian Tiger";
		TransparentEmboss(hDC, mess, _tcslen(mess), 
			RGB(0x80, 0x80, 0), RGB(0xFF, 0xFF, 0xFF),
			// RGB(0x80, 0x80, 0x40), RGB(0xE0, 0xE0, 0xE0), 
			2, x+260, y-5);

		{
			KLogFont lf(-PointSizetoLogical(hDC, 20), "Impact");
			KGDIObject font(hDC, lf.CreateFont());

			const TCHAR * mess1 = "http://mercury.spaceports.com/~schmode/";
			TransparentEmboss(hDC, mess1, _tcslen(mess1), 
				RGB(0xFF, 0xFF, 0xFF), RGB(0, 0, 0),
				// RGB(0xE0, 0xE0, 0xE0), RGB(0xC0, 0xC0, 0x40), 
				1, x+5, y+240);
		}

		y += height + 20;
	}
}


void Demo_TextasCurve(HDC hDC)
{
	KLogFont lf(-PointSizetoLogical(hDC, 72), "BookMan Old Style");
	lf.m_lf.lfWeight  = FW_BOLD;
	lf.m_lf.lfQuality = ANTIALIASED_QUALITY;
	KGDIObject font(hDC, lf.CreateFont());

	const TCHAR * mess = "gdi";
	SetBkMode(hDC, TRANSPARENT);

	KGDIObject yellowbrush(hDC, CreateSolidBrush(RGB(0xFF, 0xFF, 0))); 

	LOGBRUSH lb = { BS_SOLID, DIB_RGB_COLORS, 0 };

	for (int test=0; test<9; test++)
	{
		int x = 10 + (test % 3) * 180;
		int y = 10 + (test / 3) *  95;

		TCHAR num[16];
		wsprintf(num, "%d", test+1);

		dispsmall(hDC, x, y+20, 0, num);

		BeginPath(hDC);
		TextOut(hDC, x, y, mess, _tcslen(mess));
		EndPath(hDC);

		switch (test)
		{
			case 0:
				StrokePath(hDC); // black outline
				break;

			case 1:
				FillPath(hDC);	 // yellow fill
				break;

			case 2:
				StrokeAndFillPath(hDC); // black outline, yellow fill
				break;

			case 3:						// 3 pixel round-dot geometric pen
				{
					KGDIObject dotpen(hDC, ExtCreatePen(PS_GEOMETRIC | PS_DOT | PS_ENDCAP_ROUND | PS_JOIN_ROUND,
						3, & lb, 0, NULL));
					StrokePath(hDC);
				}
				break;

			case 4:						// 15 pixel simple pen + fill
				{
					KGDIObject bluepen(hDC, CreatePen(PS_SOLID, 15, RGB(0, 0, 0xFF)));
					SaveDC(hDC);
					StrokeAndFillPath(hDC);
					RestoreDC(hDC, -1);
					FillPath(hDC);
				}
				break;

			case 5:						// 15 pixel MITER pen + fill
				{
					KGDIObject miter(hDC, ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_JOIN_MITER,
						15, & lb, 0, NULL));
					SaveDC(hDC);
					StrokeAndFillPath(hDC);
					RestoreDC(hDC, -1);
					FillPath(hDC);
				}
				break;

			case 6:
				{
					KGDIObject bluepen(hDC, CreatePen(PS_SOLID, 7, RGB(0, 0, 0xFF)));
					SaveDC(hDC);
					StrokeAndFillPath(hDC);
					RestoreDC(hDC, -1);
				}
				SelectObject(hDC, GetStockObject(WHITE_PEN));
				StrokePath(hDC);
				break;

			case 7:
			case 8:
				{
					for (int width=17; width>1; width -=2)
					{
						// (85, 85, 0) -> (225, 225, 0)
						KGDIObject pen (hDC, CreatePen(PS_SOLID, width, 
								RGB(0xFF - width*10, 0xFF - width*10, 0)) );
	
						SaveDC(hDC);
						StrokePath(hDC);
						RestoreDC(hDC, -1);
					}

					if ( test==7 )
						FillPath(hDC);
					else
					{
						SetTextColor(hDC, RGB(0x40, 0x40, 0x00));	// dark yellow
						TextOut(hDC, x-1, y-1, mess, _tcslen(mess));

						SetTextColor(hDC, RGB(0xFF, 0xFF, 0));	
						TextOut(hDC, x+1, y+1, mess, _tcslen(mess));

						SetTextColor(hDC, RGB(0x80, 0x80, 0));		
						TextOut(hDC, x, y, mess, _tcslen(mess));
					}
				}
				break;
		}
	} 
}


class KMove : public KTransCurve
{
	int m_dx, m_dy;

public:
	KMove(int seg, int dx, int dy) : KTransCurve(seg)
	{
		m_dx = dx;
		m_dy = dy;
	}

	virtual Map(float x, float y, float & rx, float & ry)
	{
		rx = x + m_dx;
		ry = y + m_dy;
	}
};


class KWave : public KTransCurve
{
	int m_dx, m_dy;

public:
	KWave(int seg, int dx, int dy) : KTransCurve(seg)
	{
		m_dx = dx;
		m_dy = dy;
	}

	virtual Map(float x, float y, float & rx, float & ry)
	{
		rx = x + m_dx;
		ry = y + m_dy + (int) (sin(x/50.0) * 20); // 100 pixel cycle +-20
	}
};


class KRandom : public KTransCurve
{
	int m_dx, m_dy;

public:
	KRandom(int seg, int dx, int dy) : KTransCurve(seg)
	{
		m_dx = dx;
		m_dy = dy;
	}

	virtual Map(float x, float y, float & rx, float & ry)
	{
		rx = x + m_dx + (rand() % 7-3);
		ry = y + m_dy + (rand() % 7-3);
	}
};


class KExtrude : public KTransCurve
{
	int m_dx, m_dy;
	int m_x0, m_y0;
	
	int m_depth;
	int m_eye_x;
	int m_eye_y;
	int m_eye_z;

public:
	
	KExtrude(int seglen, int dx, int dy, int depth, int ex, int ey, int ez) : KTransCurve(seglen)
	{
		m_dx    = dx;
		m_dy    = dy;
		m_depth = depth;

		m_eye_x = ex;
		m_eye_y = ey;
		m_eye_z = ez;
	}

	virtual Map(float x, float y, float & rx, float & ry)
	{
		rx = x + m_dx;
		ry = y + m_dy;
	}

	void Map3D(long & x, long & y, int z)
	{
		x = ( m_eye_z * x - m_eye_x * z) / ( m_eye_z - z);
		y = ( m_eye_z * y - m_eye_y * z) / ( m_eye_z - z);
	}

	virtual BOOL DrvMoveTo(HDC hDC, int x, int y)
	{
		m_x0 = x;
		m_y0 = y;

		return TRUE;
	}

	virtual BOOL DrvLineTo(HDC hDC, int x1, int y1)
	{
		POINT p[5] = { m_x0, m_y0, 
					   x1,   y1,
					   x1,   y1,
					   m_x0, m_y0,
					   m_x0, m_y0 };

		Map3D(p[0].x, p[0].y, 0);
		Map3D(p[1].x, p[1].y, 0);
		Map3D(p[2].x, p[2].y, m_depth);
		Map3D(p[3].x, p[3].y, m_depth);
		Map3D(p[4].x, p[4].y, 0);

		m_x0 = x1;
		m_y0 = y1;

		return Polygon(hDC, p, 4);
	}

	virtual BOOL DrvBezierTo(HDC hDC, POINT p[])
	{
		DrvLineTo(hDC, p[0].x, p[0].y);
		DrvLineTo(hDC, p[1].x, p[1].y);
		
		return DrvLineTo(hDC, p[2].x, p[2].y);
	}
};


void Demo_TextasCurve2(HDC hDC)
{
	KLogFont lf(-PointSizetoLogical(hDC, 96), "Impact");
	KGDIObject font(hDC, lf.CreateFont());

	const TCHAR * mess = "CURVE";
	SetBkMode(hDC, TRANSPARENT);
	int x = 10, y= 0;

	KGDIObject yellowbrush(hDC, CreateSolidBrush(RGB(0xFF, 0xFF, 0))); 

	BeginPath(hDC);
	TextOut(hDC, x, y, mess, _tcslen(mess));
	EndPath(hDC);

	KPathData pd;

	pd.GetPathData(hDC);

	StrokeAndFillPath(hDC);

	{
		KWave wave(8, 360, 0);
		pd.Draw(hDC, wave, true);
		StrokeAndFillPath(hDC);
	}

	{
		KRandom random(12, 0, 140);

		pd.Draw(hDC, random, true);
		StrokeAndFillPath(hDC);
	}

	{
		KExtrude extrude(5, 360, 140, 2, 0, 0, -100);

		{
			KGDIObject darkyellowb(hDC, CreateSolidBrush(RGB(0x80, 0x80, 0))); 
			KGDIObject darkyellowp(hDC, CreatePen(PS_SOLID, 1, RGB(0x80, 0x80, 0))); 
			
			pd.Draw(hDC, extrude, false);
		}

		KMove move(12, 360, 140);
		pd.Draw(hDC, move, true);
		StrokeAndFillPath(hDC);
	}

	// text to region
	{
		BeginPath(hDC);
		TextOut(hDC, 30, 290, "Tiger", 5);
		EndPath(hDC);

		SelectClipPath(hDC, RGN_COPY);
		int width, height;

		DispResBitmap(hDC, 10, 280, IDB_FULLTIGER, width, height);
		SelectClipRgn(hDC, NULL);
		// TextOut(hDC, 30, 290, "Tiger", 5);
	}

	{
		KLogFont lf(-PointSizetoLogical(hDC, 512), "Wingdings");
		lf.m_lf.lfCharSet = SYMBOL_CHARSET;

		KGDIObject font(hDC, lf.CreateFont());

		const TCHAR * mess = "\x43";
		SetBkMode(hDC, OPAQUE);
		int x = 600, y = 10;
		
		SetBkMode(hDC, TRANSPARENT);
		BeginPath(hDC);
		TextOut(hDC, x, y, mess, _tcslen(mess));
		EndPath(hDC);

		HRGN hRgn = PathToRegion(hDC);

		FrameRgn(hDC, hRgn, (HBRUSH) GetStockObject(GRAY_BRUSH), 1, 1);
		DeleteObject(hRgn);
	}
}