//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : text.cpp						                                     //
//  Description: Text demo program, Chapter 15                                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define NOCRYPT

#pragma pack(push, 4)
#include <windows.h>
#pragma pack(pop)

#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "..\..\include\wingraph.h"
#include "..\..\include\MVC.h"
#include "..\..\include\fonttext.h"
#include "..\..\include\gdiobject.h"
#include "..\..\include\emf.h"
#include "..\..\include\listview.h"
#include "..\..\include\OutlineMetric.h"

#include "Resource.h"
#include "Panose.h"
#include "ChooseFont.h"
#include "TextDemo.h"

class KDemoView : public KView
{
	int		m_nCommand;
	
	virtual int OnDraw(HDC hDC, const RECT * rcPaint)
	{
		switch ( m_nCommand )
		{
			case IDM_VIEW_TERM:
				Demo_Term(hDC, rcPaint);
				break;

			case IDM_VIEW_STOCK:
				Demo_StockFonts(hDC, rcPaint);
				break;

			case IDM_VIEW_COORDINATE:
				Demo_CoordinateSystem(hDC, rcPaint);
				break;

			case IDM_VIEW_ALIGN:
				Demo_TextAlignment(hDC, rcPaint);
				break;

			case IDM_VIEW_RTLREADING:
				Demo_RTL(hDC, rcPaint);
				break;

			case IDM_VIEW_JUSTIFICATION:
				Demo_Justification(hDC, rcPaint);
				break;

			case IDM_VIEW_WIDTH:
				Demo_CharacterWidth(hDC, rcPaint);
				break;

			case IDM_VIEW_GLYPHINDEX:
				Demo_GlyphIndex(hDC, rcPaint);
				break;

			case IDM_VIEW_PLACEMENT:
				Demo_CharacterPlacement(hDC, rcPaint);
				break;

			case IDM_VIEW_GLYPHOUTLINE:
				Demo_GlyphOutline(hDC);
				break;

			case IDM_VIEW_TABBEDTEXTOUT:
				Demo_TabbedTextOut(hDC);
				break;

			case IDM_VIEW_DRAWTEXT:
				Demo_Paragraph(hDC, false);
				break;

			case IDM_VIEW_PARAGRAPH:
				Demo_Paragraph(hDC, true);
				break;

			case IDM_VIEW_TEXTCOLOR:
				Demo_TextColor(hDC, (HINSTANCE) GetWindowLong(WindowFromDC(hDC), GWL_HINSTANCE));
				break;

			case IDM_VIEW_TEXTSTYLE:
				Demo_TextStyle(hDC);
				break;

			case IDM_VIEW_TEXTGEOMETRY:
				Demo_TextGeometry(hDC);
				break;

			case IDM_VIEW_TEXTASBITMAP:
				Demo_TextasBitmap(hDC);
				break;

			case IDM_VIEW_TEXTASBITMAP2:
				Demo_TextasBitmap2(hDC);
				break;

			case IDM_VIEW_TEXTASCURVE:
				Demo_TextasCurve(hDC);
				break;

			case IDM_VIEW_TEXTCURVE2:
				Demo_TextasCurve2(hDC);
				break;
		}

		return View_NoChange;
	}
	
public:

	virtual int OnCommand(int cmd, HWND hWnd)
	{
		if ( cmd==m_nCommand )
			return View_NoChange;

		switch ( cmd )
		{
			case IDM_VIEW_TERM:
			case IDM_VIEW_STOCK:
			case IDM_VIEW_COORDINATE:
			case IDM_VIEW_ALIGN:
			case IDM_VIEW_RTLREADING:
			case IDM_VIEW_JUSTIFICATION:
			case IDM_VIEW_WIDTH:
			case IDM_VIEW_GLYPHINDEX:
			case IDM_VIEW_PLACEMENT:
			case IDM_VIEW_GLYPHOUTLINE:
				m_nCommand = cmd;
				return View_Redraw;

			case IDM_VIEW_TABBEDTEXTOUT:
				m_nCommand = cmd;
				SetWindowText(hWnd, "TabbledTextOut/DrawText");
				return View_Redraw;

			case IDM_VIEW_DRAWTEXT:
				m_nCommand = cmd;
				SetWindowText(hWnd, "Text Formatting - DrawText");
				return View_Redraw;

			case IDM_VIEW_PARAGRAPH:
				m_nCommand = cmd;
				SetWindowText(hWnd, "Text Formatting - High Precision");
				return View_Redraw;

			case IDM_VIEW_TEXTCOLOR:
				m_nCommand = cmd;
				SetWindowText(hWnd, "Text Effects - Color");
				return View_Redraw;

			case IDM_VIEW_TEXTSTYLE:
				m_nCommand = cmd;
				SetWindowText(hWnd, "Text Effects - Style");
				return View_Redraw;

			case IDM_VIEW_TEXTGEOMETRY:
				m_nCommand = cmd;
				SetWindowText(hWnd, "Text Effects - Geometry");
				return View_Redraw;

			case IDM_VIEW_TEXTASBITMAP:
				m_nCommand = cmd;
				SetWindowText(hWnd, "Text Effects - Text as Bitmap");
				return View_Redraw;

			case IDM_VIEW_TEXTASBITMAP2:
				m_nCommand = cmd;
				SetWindowText(hWnd, "Text Effects - Text as Bitmap - Transparent Embossing");
				return View_Redraw;

			case IDM_VIEW_TEXTASCURVE:
				m_nCommand = cmd;
				SetWindowText(hWnd, "Text Effects - Text as Curve");
				return View_Redraw;

			case IDM_VIEW_TEXTCURVE2:
				m_nCommand = cmd;
				SetWindowText(hWnd, "Text Effects - Text Curve Transformation");
				return View_Redraw;

			case IDM_FILE_SAVE:
				{
					HDC hDC = QuerySaveEMFFile("Text EMF Sample\0", NULL, NULL);

					if ( hDC )
					{
						OnDraw(hDC, NULL);
						HENHMETAFILE hEmf = CloseEnhMetaFile(hDC);
						DeleteEnhMetaFile(hEmf);
					}
				}
				break;

		}

		return View_NoChange;
	}
	
	KDemoView(void)
	{
		m_nCommand  = IDM_VIEW_STOCK;
	}
};


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
	
		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_TEXT));
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

	
	BOOL CreateCanvas(KView * pView, const TCHAR * Title)
	{
		if ( pView==NULL )
			return FALSE;

		KMDICanvas   * pCanvas = new KMDICanvas();

		if ( pCanvas )
		{
			if ( pCanvas->Initialize(m_hInst, m_pStatus, pView, IDR_DEMO, IDI_TEXT) )
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
			case IDM_FILE_DEMO:
				CreateCanvas(new KDemoView(), _T("Demo - "));
				return TRUE;

			case IDM_FILE_PANOSE:
				{
					KPanoseView * pList = new KPanoseView;

					if ( pList )
						if ( pList->Initialize(m_hInst, m_pStatus, m_hWnd) )
							CreateMDIChild(pList, _T("PanoseViewClass"), _T("PANOSE Font Matching"));
						else
							delete pList;
				}
				return TRUE;

			case IDM_FILE_CHOOSEFONT:
				FontSelection(m_hInst);
				return TRUE;

			case IDM_FILE_LC:
				Test_LC();
				return TRUE;

			case IDM_FILE_POINT:
				Test_Point();
		}

		return FALSE;
	}

	
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
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
	
	frame.CreateEx(0, _T("ClassName"), _T("Text"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}
