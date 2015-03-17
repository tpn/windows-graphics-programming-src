//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : font.cpp						                                     //
//  Description: Font demo program, Chapter 14                                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define NOCRYPT

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "..\..\include\win.h"
#include "..\..\include\Canvas.h"
#include "..\..\include\Toolbar.h"
#include "..\..\include\Status.h"
#include "..\..\include\FrameWnd.h"
#include "..\..\include\ScrollCanvas.h"
#include "..\..\include\ListView.h"
#include "..\..\include\MVC.h"
#include "..\..\include\logfont.h"

#include "CharSet.h"

#include "Resource.h"
#include "Raster.h"
#include "TrueType.h"
#include "FontFamily.h"

///////////////// Raster Font View /////////////////

class KRasterFontView : public KView
{
	TCHAR	m_nFontName[MAX_PATH];

	virtual int OnDraw(HDC hDC, const RECT * rcPaint)
	{
		int height = DecodeRasterFont(hDC, m_nFontName);

		if ( height > m_nPixelHeight )
		{
			m_nPixelHeight = height;
			return View_Resize;
		}

		return View_NoChange;
	}

public:

	virtual int OnCommand(int cmd, HWND hWnd)
	{
		return View_NoChange;
	}
	
	KRasterFontView(const TCHAR * fontname)
	{
		if ( fontname )
			_tcscpy(m_nFontName, fontname);
		else
			m_nFontName[0] = 0;
	}
};


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


class KTrueTypeFontView : public KView
{
	LOGFONT	  m_LogFont;
	KTrueType m_tt;
	int       m_option;
	
	int		  m_Start;
	int		  m_PageSize;

	virtual int OnDraw(HDC hDC, const RECT * rcPaint)
	{
		int i;
		double scale;

		switch ( m_option )
		{
			case IDM_VIEW_SMALL:
				scale = 32 * m_tt.GetScale();
				for (i=0; i<256; i++)
					m_tt.DrawTTGlyph(hDC, 20+(i%16)*42, 40+(i/16)*42, m_Start+i, scale, 0);
				break;

			case IDM_VIEW_MEDIUM:
				scale = 96 * m_tt.GetScale();
				for (i=0; i<70; i++)
					m_tt.DrawTTGlyph(hDC, 50+(i%10)*96, 80+(i/10)*96, m_Start+i, scale, 0);
				break;

			case IDM_VIEW_LARGE:
			case IDM_VIEW_LARGEPOINT:
			case IDM_VIEW_METRICS:
				scale = 384 * m_tt.GetScale();
				m_tt.DrawTTGlyph(hDC, 200, 500, m_Start, scale, m_option);
				break;


			case IDM_VIEW_COMPARE:
				// compare simple rasterization with TrueType font engine
				scale = 32 * m_tt.GetScale(); // 32 point
				
				m_tt.DrawTTGlyph(hDC, 20,    40, 36, scale, false); // ABC
				m_tt.DrawTTGlyph(hDC, 20+26, 40, 37, scale, false); // ABC
				m_tt.DrawTTGlyph(hDC, 20+49, 40, 38, scale, false); // ABC
				
				ZoomRect(hDC, 18, 17, 92, 17+25, 100,  10, 4);
				
				{
				    HFONT hFont = CreateFont(- 36 /** GetDeviceCaps(hDC, LOGPIXELSY) / 72*/,
							 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, 
							 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, 
							 PROOF_QUALITY, VARIABLE_PITCH, "Times New Roman");
					HGDIOBJ hOld = SelectObject(hDC, hFont);

					TextOut(hDC, 20, 145,"ABC", 3);
					SelectObject(hDC, hOld);
					DeleteObject(hFont);
				}

				ZoomRect(hDC, 18, 145+7, 92, 145+7+26, 100, 150, 4);
				
				scale = 32 * 5 * m_tt.GetScale();

				m_tt.DrawTTGlyph(hDC, 500,      180, 36, scale, false); // ABCD
				m_tt.DrawTTGlyph(hDC, 500+24*5, 180, 37, scale, false); // ABCD
				m_tt.DrawTTGlyph(hDC, 500+45*5, 180, 38, scale, false); // ABCD
		}

		return View_NoChange;
	}

public:

	virtual int OnCommand(int cmd, HWND hWnd)
	{
		if ( cmd==m_option )
			return View_NoChange;

		switch ( cmd )
		{
			case IDM_VIEW_COMPARE:
			case IDM_VIEW_SMALL : m_PageSize = 256; m_option = cmd; break;
			
			case IDM_VIEW_MEDIUM: m_PageSize = 70;  m_option = cmd; break;
			
			case IDM_VIEW_LARGEPOINT:
			case IDM_VIEW_METRICS:
			case IDM_VIEW_LARGE : m_PageSize = 1;   m_option = cmd; break;
			
			case IDM_VIEW_PAGEUP  : m_Start = max(0, m_Start- m_PageSize); break;
			case IDM_VIEW_PAGEDOWN: m_Start += m_PageSize; break;

			default:
				return View_NoChange;
		}
		
		return View_Redraw;
	}
	
	int OnKey(int vkey)
	{
		switch ( vkey )
		{
			case VK_PRIOR   : m_Start = max(0, m_Start- m_PageSize); break;
			case VK_NEXT    : m_Start += m_PageSize; break;
			
			case VK_LEFT	:
			case VK_UP      : m_Start = max(0, m_Start-1); break;

			case VK_DOWN    :
			case VK_RIGHT   : m_Start ++; break;

			default		    : return View_NoChange;
		}

		return View_Redraw;
	}

	KTrueTypeFontView(const LOGFONT * pLogFont, HINSTANCE hInst)
	{
		m_option   = IDM_VIEW_SMALL;
		m_Start    = 0;
		m_PageSize = 256;

		m_LogFont = * pLogFont;

		m_tt.Load(m_LogFont);

		LogTableDirectory(m_tt, hInst, m_LogFont.lfFaceName);
	}
};


//////// GLYPH View ///////////

class KGlyphView : public KView
{
	int		m_nCommand;

	void TestCharGlyph(HDC hDC, const RECT * rcPaint);
	void TestGlyphDesign(HDC hDC, const RECT * rcPaint);

	virtual int OnDraw(HDC hDC, const RECT * rcPaint)
	{
		switch ( m_nCommand )
		{
			case IDM_VIEW_CHARGLYPH:
				TestCharGlyph(hDC, rcPaint);
				break;

			case IDM_VIEW_GLYPHDESIGN:
				TestGlyphDesign(hDC, rcPaint);
				break;
		}
		
		return View_NoChange;
	}

public:

	virtual int OnCommand(int cmd, HWND hWnd)
	{
		if ( ((cmd==IDM_VIEW_CHARGLYPH) || (cmd==IDM_VIEW_GLYPHDESIGN)) && (cmd!=m_nCommand) )
		{
			m_nCommand = cmd;
			return View_Redraw;
		}

		return View_NoChange;
	}
	
	KGlyphView(void)
	{
		m_nCommand  = IDM_VIEW_CHARGLYPH;
	}
};


const char * TypeFaces[] =
{
	"Georgia",
	"Times New Roman",
	"Clarendon",
	"Bodoni Black",

	"Arial",
	"Microsoft Sans Serif",
	"Verdana",
	"Eras Demi ITC",

	"Curier New",
	"Dark Courier",
	"Lucica Console",
	"Letter Gothic",

	"Vivaldi",
	"Tempus Sans ITC",
	"Viner Hand ITC",
	"Comic Sans MS",

	"Old English",
	"BD Renaissance",
	"TreeHouse",
	"Unicorn"
};


const char * TypeFamily[]=
{
	"Roman",
	"Swiss",
	"Modern",
	"Script",
	"Decorative"
};


void TextOutWH(HDC hDC, int x, int y, WCHAR text[], int len)
{
	TEXTMETRIC tm;

	GetTextMetrics(hDC, & tm);

	for (int i=0; i<len; i++)
	{
		TextOutW(hDC, x, y, & text[i], 1);
	
		if (i==0) y+=40; else y+= 45;
	}
}


void KGlyphView::TestCharGlyph(HDC hDC, const RECT * rcPaint)
{
	int xx = 10;

	SetTextAlign(hDC, TA_BASELINE);

	for (int i=0; i<sizeof(TypeFaces)/sizeof(TypeFaces[i]); i++)
	{
		KLogFont logfont(52, TypeFaces[i]);
		HFONT hFont = logfont.Create();

		SelectObject(hDC, hFont);

		int width = 0 ;
		GetCharWidth(hDC, 'A', 'A', & width);

		TextOut(hDC, xx, 50, "A", 1);
		xx += width + 5;

		TextOut(hDC, 20+(i%4)*220, 100+(i/4)*50, TypeFamily[i/4], strlen(TypeFamily[i/4]));
		SelectObject(hDC, GetStockObject(SYSTEM_FONT));
		DeleteObject(hFont);
	}

	{
		KLogFont logfont(52, "MingLiu");
		logfont.SetCharSet(GB2312_CHARSET);

		HFONT hFont = logfont.Create();
		SelectObject(hDC, hFont);

		// glyph sharing
		WCHAR Share[] = { 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, ' ', ' ',
			0x5CAD, 0x5CF0, 0x5CFB, 0x5CED, 0x5CFD, 0x5D1A, 0
		};

		TextOutW(hDC, 10, 400, Share, wcslen(Share));

		// ligature
		WCHAR Ligature[] = { 'A', '+', 'E', '=', 0xC6, ' ', ' ',
			'C', '+', 'E', '=', 0x8C, ' ', ' ',
			'f', '+', 'i', '=', 0xFB01, ' ', ' ',
			'f', '+', 'l', '=', 0xFB02, 0 };

		TextOutW(hDC, 10, 460, Ligature, wcslen(Ligature));

		// context
		WCHAR Context[] = { '(', 0x6A2A, ')', ' ', 	0x3010, 0x6A2A, 0x3011, 0 };

		WCHAR Context2[] = { 0xFE35, 0x7AD6, 0xFE36, 0 };
		WCHAR Context3[] = { 0xFE3B, 0x7AD6, 0xFE3C, 0 };
		
		TextOutW(hDC,   10, 540, Context, wcslen(Context));
		TextOutWH(hDC, 250, 500, Context2, wcslen(Context2));
		TextOutWH(hDC, 350, 500, Context3, wcslen(Context3));

		SelectObject(hDC, GetStockObject(SYSTEM_FONT));
		DeleteObject(hFont);
	}

	{
		KLogFont logfont(52, "Tahoma");
		logfont.SetCharSet(ARABIC_CHARSET);

		HFONT hFont = logfont.Create();
		SelectObject(hDC, hFont);

		// context
		WCHAR Context[] = { 0xFB56, ' ', 0xFB57, ' ', 0xFB58, ' ', 0xFB59, ' ', ' ', 
							0xFB7A, ' ', 0xFB7B, ' ', 0xFB7C, ' ', 0xFB7D, ' ', ' ',
							0xFB8E, ' ', 0xFB8F, ' ', 0xFB90, ' ', 0xFB91, ' ', ' ', 0 };
		
		TextOutW(hDC, 10, 620, Context, wcslen(Context));

		SelectObject(hDC, GetStockObject(SYSTEM_FONT));
		DeleteObject(hFont);

	}
}


void KGlyphView::TestGlyphDesign(HDC hDC, const RECT * rcPaint)
{
	KLogFont logfont(200, "Times New Roman");

	HFONT hFont = logfont.Create();
	SelectObject(hDC, hFont);

	TextOut(hDC, 10, 10, "Glyph Design", 12);

	SelectObject(hDC, GetStockObject(SYSTEM_FONT));
	DeleteObject(hFont);

/*	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0xFF, 0, 0));
	SelectObject(hDC, hPen);
	MoveToEx(hDC, 0, 200, NULL);
	LineTo(hDC, 100, 200);

	SelectObject(hDC, GetStockObject(BLACK_PEN));
	MoveToEx(hDC, 10, 0, NULL);
	LineTo(hDC, 12, 200);
	LineTo(hDC, 14, 0);
	LineTo(hDC, 10, 0);
*/
}


////////////////////////// MDI Frame Window

const int Translate[] =
{
	IDM_FILE_CLOSE,
	IDM_FILE_EXIT,
	IDM_WINDOW_TILE,
	IDM_WINDOW_CASCADE,
	IDM_WINDOW_ARRANGE,
	IDM_WINDOW_CLOSEALL
};


class KMyMDIFrame : public KMDIFrame
{
	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KMDIFrame::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_FONT));
	}

	void CreateMDIChild(KCanvas * canvas, const TCHAR * klass, const TCHAR * title)
	{
		MDICREATESTRUCT mdic;

		mdic.szClass = klass;
		mdic.szTitle = title;
		mdic.hOwner  = m_hInst;
		mdic.x       = CW_USEDEFAULT;
		mdic.y       = CW_USEDEFAULT;
		mdic.cx      = CW_USEDEFAULT;
		mdic.cy      = CW_USEDEFAULT;
		mdic.style   = WS_VISIBLE | WS_BORDER;
		mdic.lParam  = (LPARAM) canvas;

		HWND hWnd = (HWND) SendMessage(m_hMDIClient, WM_MDICREATE, 0, (LPARAM) & mdic);
	}

	void CreateListViewCanvas(bool bFamily)
	{
		KListViewCanvas * pList = new KListViewCanvas(bFamily);

		if ( pList )
			if ( pList->Initialize(m_hInst, m_pStatus, m_hWnd) )
				if ( bFamily )
					CreateMDIChild(pList, _T("ListViewCanvas"), _T("Font Families"));
				else
					CreateMDIChild(pList, _T("ListViewCanvas"), _T("Fonts"));
			else
				delete pList;
	}

	BOOL CreateCanvas(KView * pView, const TCHAR * Title)
	{
		if ( pView==NULL )
			return FALSE;

		KMDICanvas   * pCanvas = new KMDICanvas();

		if ( pCanvas )
		{
			if ( pCanvas->Initialize(m_hInst, m_pStatus, pView, IDR_DIBVIEW, IDI_FONT) )
			{
				CreateMDIChild(pCanvas, pCanvas->GetClassName(), Title);
				return TRUE;
			}

			delete pCanvas;
		}

		delete pView;
		return FALSE;
	}

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam)
	{
		switch ( LOWORD(wParam) )
		{
			case IDM_FILE_FONTFAMILY:
				CreateListViewCanvas(true);
				return TRUE;

			case IDM_FILE_FONT:
				CreateListViewCanvas(false);
				return TRUE;

			case IDM_FILE_CODEPAGE:
				CreateCanvas(new KCharSetView(), _T("Code Page"));
				return TRUE;

			case IDM_FILE_GLYPH:
				CreateCanvas(new KGlyphView(), _T("Glyph"));
				return TRUE;
		}

		return FALSE;
	}

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if ( uMsg == WM_USER+1 )
			CreateCanvas(new KRasterFontView((const TCHAR *) lParam), "Raster Font");

		if ( uMsg == WM_USER+2 )
			CreateCanvas(new KTrueTypeFontView((const LOGFONT *) lParam, m_hInst), "TrueType Font");

		return KMDIFrame::WndProc(hWnd, uMsg, wParam, lParam);
	}

public:
	KMyMDIFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
		KToolbar * pToolbar, KStatusWindow * pStatus) :
		KMDIFrame(hInstance, pButtons, nCount, pToolbar, pStatus, Translate)
	{
	}
};


const TBBUTTON tbButtons[] =
{
	{ STD_FILENEW,	 IDM_FILE_NEW,   TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_FILENEW,  0 },
	{ STD_FILEOPEN,  IDM_FILE_OPEN,  TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_FILEOPEN, 0 }
};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KToolbar      toolbar;
	KStatusWindow status;

	KMyMDIFrame frame(hInst, tbButtons, 2, & toolbar, & status);

	frame.CreateEx(0, _T("ClassName"), _T("Font"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}
