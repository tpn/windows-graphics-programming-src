//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : emfmain.cpp					                                     //
//  Description: EMF demo program, Chapter 16				                         //
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
#include "..\..\include\ListView.h"
#include "..\..\include\MVC.h"
#include "..\..\include\GDIObject.h"
#include "..\..\include\pen.h"
#include "..\..\include\filedialog.h"
#include "..\..\include\treeview.h"
#include "..\..\include\emf.h"
#include "..\..\include\fonttext.h"
#include "..\..\include\axis.h"

#include "resource.h"
#include "EMFView.h"

//////////////////////////////////////////////////////////////

class KEMFResourceDialog : public KDialog
{
	HENHMETAFILE hEmf;

    virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
    	switch (uMsg)
	    {
		    case WM_INITDIALOG:
				{
					hEmf = LoadEMF((HMODULE) GetWindowLong(hWnd, GWL_HINSTANCE), MAKEINTRESOURCE(IDR_EMF1));

					SendDlgItemMessage(hWnd, IDC_EMF, STM_SETIMAGE, IMAGE_ENHMETAFILE,
						(LPARAM) hEmf);

					return TRUE;
				}

			case WM_NCDESTROY:
				if ( hEmf )
					DeleteEnhMetaFile(hEmf);
				return TRUE;

		    case WM_COMMAND:
				if ( LOWORD(wParam)==IDOK )
					EndDialog(hWnd, IDOK);
				return TRUE;
	    }

	    return FALSE;
    }
};


void Demo_EMFResource(HINSTANCE hInst)
{
	KEMFResourceDialog dlg;

	dlg.Dialogbox(hInst, MAKEINTRESOURCE(IDD_EMFRES));
}

//////////////////////////////////////////////////////////////

void SampleDraw(HDC hDC, int x, int y)
{
	Ellipse(hDC, x+25, y+25, x+75, y+75);

	SetTextAlign(hDC, TA_CENTER | TA_BASELINE);

	const TCHAR * mess = "Default Font";
	TextOut(hDC, x+50, y+50, "Default Font", _tcslen(mess));
}


void Demo_Scale(HDC hDC)
{
	HENHMETAFILE hSample1;
	HENHMETAFILE hSample2;
	
	{
		HDC hDCEMF = CreateEnhMetaFile(hDC, NULL, NULL, NULL);
		SampleDraw(hDCEMF, 0, 0);
		hSample1 = CloseEnhMetaFile(hDCEMF);


		RECT rect = { 0, 0, 100, 100 };
		Map10um(hDC, rect);
		hDCEMF = CreateEnhMetaFile(hDC, NULL, & rect, NULL);
		SampleDraw(hDCEMF, 0, 0);
		hSample2 = CloseEnhMetaFile(hDCEMF);
	}

	HBRUSH yellow = CreateSolidBrush(RGB(0xFF, 0xFF, 0));

	// test EMF without proper frame rectangle
	{
		RECT rect = { 10, 10, 10+100, 10+100 }; 
		
		FillRect(hDC, & rect, yellow);
		SampleDraw(hDC, 10, 10); // original
	}

	for (int test=0, x=120; test<3; test++)
	{
		RECT rect = { x, 10, x+(test/2+1)*100, 10+((test+1)/2+1)*100 };
	
		FillRect(hDC, & rect, yellow);
		PlayEnhMetaFile(hDC, hSample1, & rect);

		x = rect.right + 10;
	}

	// test EMF with proper frame rectangle
	{
		RECT rect = { 10, 220, 10+100, 220+100 };
		
		FillRect(hDC, & rect, yellow); // original
		SampleDraw(hDC, 10, 220);
	}

	for (test=0, x=120; test<3; test++)
	{
		RECT rect = { x, 220, x+(test/2+1)*100, 220+((test+1)/2+1)*100 };
	
		FillRect(hDC, & rect, yellow);
		PlayEnhMetaFile(hDC, hSample2, & rect);

		x = rect.right + 10;
	}

	DeleteObject(yellow);

	DeleteEnhMetaFile(hSample1);
	DeleteEnhMetaFile(hSample2);
}


////////////////////////////////////


void Demo_LineCurveArea(HDC hDC)
{
	{
		int x= 300, y = 50;

		MoveToEx(hDC, x,    y,   NULL); LineTo(hDC, x +50, y);
		MoveToEx(hDC, x+50, y+1, NULL); LineTo(hDC, x+100, y+1);

		Rectangle(hDC, x,    y+10, x +50, y+60);
		Rectangle(hDC, x+50, y+30, x+100, y+80);

		Ellipse(hDC, x+110, y+10, x+160, y+100);
		Ellipse(hDC, x+160, y+10, x+210, y+100);

		PatBlt(hDC, x,     y+100, 100, 100, BLACKNESS);
		PatBlt(hDC, x+100, y+110, 100, 100, PATINVERT);

		RECT rect = { x, y+ 250, x+100, y+350 };
		FillRect(hDC, & rect, GetSysColorBrush(COLOR_3DSHADOW));

		OffsetRect(& rect, 120, 0);
		FrameRect(hDC, & rect, GetSysColorBrush(COLOR_ACTIVECAPTION));

		OffsetRect(& rect, 120, 0);
		InvertRect(hDC, & rect);
	}

	KPen Red (PS_DOT, 0, RGB(0xFF, 0, 0));
	KPen Blue(PS_SOLID, 3, RGB(0, 0, 0xFF));

	for (int z=0; z<=160; z+=40)
	{
		int x = 10, y = 200;

		POINT p[4] = { x, y, x+ 40, y-z, x+80, y-z, x+120, y }; x+= 136;
		POINT q[4] = { x, y, x+ 40, y-z, x+80, y+z, x+120, y }; 
		
		Red.Select(hDC);
		Polyline(hDC, p, 4);
		Polyline(hDC, q, 4);
		Red.UnSelect();

		Blue.Select(hDC);
		PolyBezier(hDC, p, 4);
		PolyBezier(hDC, q, 4);
		Blue.UnSelect();
	} 
}


////////////////////////////////

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


void Demo_Bitmaps(HDC hDC)
{
	const BITMAPINFO * pBMI = LoadDIB(
		(HINSTANCE) GetWindowLong(WindowFromDC(hDC), GWL_HINSTANCE), 
		MAKEINTRESOURCE(IDB_PANDA));

	// load as DIB
	const BYTE * pBits = (const BYTE *) & pBMI->bmiColors[GetDIBColorCount(pBMI->bmiHeader)];
	
	int width  = pBMI->bmiHeader.biWidth;
	int height = abs(pBMI->bmiHeader.biHeight);

	int x = 10;
	int y = 10;

	// full DIB, duplication: is GDI smart enough to merge: NO
	{
		StretchDIBits(hDC, x, y, width, height, 0, 0, width, height, 
			pBits, pBMI, DIB_RGB_COLORS, SRCCOPY);
		y += height + 10;

		StretchDIBits(hDC, x, y, width, height, 0, 0, width, height, 
			pBits, pBMI, DIB_RGB_COLORS, SRCCOPY);
		y += height + 10;
	}

	x += width + 10;
	y  = 10;

	// 1/2 DIB: is GDI smart enough to split: YES
	{
		StretchDIBits(hDC, x, y, width, height/2, 0, height/2, width, height/2, 
			pBits, pBMI, DIB_RGB_COLORS, SRCCOPY);
		y += height/2 + 1;

		StretchDIBits(hDC, x, y, width, height/2, 0, 0, width, height/2, 
			pBits, pBMI, DIB_RGB_COLORS, SRCCOPY);
		y += height/2 + 9;
	}

	// 1/4 DIB: is GDI smart enough to split: NO

	{
		StretchDIBits(hDC, x, y, width/2, height/2, 
			0, height/2, width/2, height/2, pBits, pBMI, DIB_RGB_COLORS, SRCCOPY);

		y += height/2 + 1;

		StretchDIBits(hDC, x, y, width/2, height/2, 
			0, 0, width/2, height/2, pBits, pBMI, DIB_RGB_COLORS, SRCCOPY);

		y -= height/2 + 1;

		x += width/2 + 1;

		
		StretchDIBits(hDC, x, y, width/2, height/2, 
			width/2, height/2, width/2, height/2, pBits, pBMI, DIB_RGB_COLORS, SRCCOPY);

		y += height/2 + 1;

		StretchDIBits(hDC, x, y, width/2, height/2, 
			width/2, 0, width/2, height/2, pBits, pBMI, DIB_RGB_COLORS, SRCCOPY);

		y -= height/2 + 1;

		x += width/2 + 1;

	}
}


void Comment(HDC hDC, const char * mess)
{
	GdiComment(hDC, _tcslen(mess), (const BYTE *) mess);
}


int LineSpace(HDC hDC)
{
	TEXTMETRIC tm;

	GetTextMetrics(hDC, & tm);

	return tm.tmHeight + tm.tmExternalLeading;
}


void Demo_Texts(HDC hDC)
{
	KLogFont lf(-PointSizetoLogical(hDC, 15), "Times New Roman");
	KGDIObject font(hDC, lf.CreateFont());

	KGDIObject yellowbrush(hDC, CreateSolidBrush(RGB(0xFF, 0xFF, 0))); 

	const TCHAR * mess1 = "Quick\t Brown\t Fox\t Jumps\t Over\t a\t Lazy\t Dog.";

	int x = 10, y = 10;

	SetTextAlign(hDC, TA_LEFT | TA_TOP);

	Comment(hDC, "->TextOut");
	TextOut(hDC, x, y, mess1, _tcslen(mess1));
	Comment(hDC, "<-TextOut");

	y += LineSpace(hDC) + 10;

	Comment(hDC, "ExtTextOut");

	ExtTextOut(hDC, x, y, 0, NULL, mess1, _tcslen(mess1), NULL);
	y += LineSpace(hDC) + 10;

	int tab[1] = { 50 };

	Comment(hDC, "->TabbedTextOut");
	TabbedTextOut(hDC, x, y, mess1, _tcslen(mess1), 1, tab, x);
	Comment(hDC, "<-TabbedTextOut");

	y += LineSpace(hDC) + 10;

	{

		const TCHAR * mess = 
			"The DrawText function draws formatted text in the specified rectangle. "
			"It formats the text according to the specified method (expanding tabs, "
			"justifying characters, breaking lines, and so forth).";

		int width = 250;
		int x     =  20;
		RECT rect = { x, y, x+width, y+200 };
	
		SetTextAlign(hDC, TA_LEFT | TA_TOP | TA_NOUPDATECP);

		Comment(hDC, "->DrawText");
		DrawText(hDC, mess, _tcslen(mess), & rect, DT_WORDBREAK | DT_LEFT);
		Comment(hDC, "<-DrawText");
	}
}


//////////////////////////////////////////

void Demo_Mapping(HDC hDC)
{
	KLogFont lf(-PointSizetoLogical(hDC, 16), "Times New Roman");
	KGDIObject font(hDC, lf.CreateFont());

	KGDIObject yellowbrush(hDC, CreateSolidBrush(RGB(0xFF, 0xFF, 0))); 

	const TCHAR * mess = "A Quick Brown Fox Jumps Over a Lazy Dog.";
	const TCHAR * mes2 = "Jackdaws Love My Big Sphinx of Quartz.";
	int x = 0, y = 0;

	SetTextAlign(hDC, TA_LEFT | TA_CENTER);
	
	SetMapMode(hDC, MM_ANISOTROPIC);
	SetViewportExtEx(hDC, 1, -1, 0);
	SetViewportOrgEx(hDC, 200, 100, NULL);

	TextOut(hDC, x, y,	  mess, _tcslen(mess));
	TextOut(hDC, x, y+24, mes2, _tcslen(mes2));

	ShowAxes(hDC, 400, 200);
}


class KDemoView : public KView
{
	int		m_nCommand;
	
	virtual int OnDraw(HDC hDC, const RECT * rcPaint)
	{
		switch ( m_nCommand )
		{
			case IDM_VIEW_SCALE:
				Demo_Scale(hDC);
				break;

			case IDM_VIEW_LINECURVE:
				Demo_LineCurveArea(hDC);
				break;

			case IDM_VIEW_BITMAPS:
				Demo_Bitmaps(hDC);
				break;

			case IDM_VIEW_TEXT:
				Demo_Texts(hDC);
				break;

			case IDM_VIEW_MAPPING:
				Demo_Mapping(hDC);
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
			case IDM_VIEW_SCALE:
				m_nCommand = cmd;
				SetWindowText(hWnd, "EMF Scaling");
				return View_Redraw;

			case IDM_VIEW_LINECURVE:
				m_nCommand = cmd;
				SetWindowText(hWnd, "Lines, Curves, and Areas in EMF");
				return View_Redraw;

			case IDM_VIEW_BITMAPS:
				m_nCommand = cmd;
				SetWindowText(hWnd, "Bitmaps in EMF");
				return View_Redraw;

			case IDM_VIEW_TEXT:
				m_nCommand = cmd;
				SetWindowText(hWnd, "Texts in EMF");
				return View_Redraw;

			case IDM_VIEW_MAPPING:
				m_nCommand = cmd;
				SetWindowText(hWnd, "Mapping Mode in EMF");
				return View_Redraw;

			case IDM_FILE_SAVE:
				{
					RECT rect;

					GetClientRect(hWnd, & rect);

					HDC hDC = GetDC(hWnd);
					Map10um(hDC, rect);
					ReleaseDC(hWnd, hDC);

					hDC = QuerySaveEMFFile("EMF Sample\0", & rect, NULL);

					if ( hDC )
					{
						OnDraw(hDC, NULL);
						HENHMETAFILE hEmf = CloseEnhMetaFile(hDC);
						DeleteEnhMetaFile(hEmf);
					}
				}
				break;

			case IDM_EDIT_COPY:
				{
					HDC hDC = CreateEnhMetaFile(NULL, NULL, NULL, NULL);
					
					if ( hDC )
					{
						OnDraw(hDC, NULL);

						HENHMETAFILE hEmf = CloseEnhMetaFile(hDC);
						CopyToClipboard(hWnd, hEmf);
						DeleteEnhMetaFile(hEmf);
					}
					else
						assert(false);
				}
				break;
		}

		return View_NoChange;
	}
	
	KDemoView(void)
	{
		m_nCommand  = IDM_VIEW_LINECURVE;
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


HENHMETAFILE TestEMFGen(void)
{
	RECT rect;

	HDC hdcRef = GetDC(NULL);

	GetClientRect(GetDesktopWindow(), &rect);
	Map10um(hdcRef, rect);

	HDC hDC = CreateEnhMetaFile(hdcRef, "c:\\test.emf", & rect, "EMF.EXE\0TestEMF\0");

	ReleaseDC(NULL, hdcRef);

	if ( hDC )
	{
		GetClientRect(GetDesktopWindow(), &rect);

		Rectangle(hDC, rect.right/3, rect.bottom/3, rect.right*2/3, rect.bottom*2/3);
		
		HENHMETAFILE hEmf = CloseEnhMetaFile(hDC);

		if ( hEmf )
			MyMessageBox(NULL, _T("c:\\test.emf generated."), _T("EMF"), MB_OK, IDI_GRAPH);
		else
			MyMessageBox(NULL, _T("c:\\test.emf generation failed."), _T("EMF"), MB_OK, IDI_GRAPH);

		return hEmf;
	}
	
	return NULL;
}


class KMyMDIFrame : public KMDIFrame
{
	HWND			m_Children[100];
	int				m_nChild;

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

		if ( (hWnd) && (m_nChild<100) )
			m_Children[m_nChild++] = hWnd;
	}

	BOOL CreateCanvas(KView * pView, const TCHAR * Title)
	{
		if ( pView==NULL )
			return FALSE;

		KMDICanvas   * pCanvas = new KMDICanvas();

		if ( pCanvas )
		{
			if ( pCanvas->Initialize(m_hInst, m_pStatus, pView, IDR_DEMO, IDI_GRAPH) )
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

			case IDM_FILE_SIMPLEEMF:
				{
					HENHMETAFILE hEmf = TestEMFGen();
					DeleteEnhMetaFile(hEmf);
				}
				return TRUE;

			case IDM_FILE_OPEN:
				{
					TCHAR filename[MAX_PATH];

					filename[0] = 0;
					HENHMETAFILE hEmf = QueryOpenEMFFile(filename);

					if ( hEmf || filename[0] )
					{
						KEMFView * pView = new KEMFView;

						if ( pView )
							if ( pView->Initialize(m_hInst, m_pStatus, m_hWnd, hEmf, filename) )
								CreateMDIChild(pView, _T("EMFViewClass"), filename);
							else
								delete pView;
					}
				}
				return TRUE;
			
			case IDM_FILE_EMFRES:
				Demo_EMFResource(m_hInst);
				return TRUE;

			case IDM_EDIT_PASTE:
				{
					HENHMETAFILE hEmf = PasteFromClipboard(m_hWnd);

					if ( hEmf )
					{
						KEMFView * pView = new KEMFView;

						if ( pView )
							if ( pView->Initialize(m_hInst, m_pStatus, m_hWnd, hEmf, NULL) )
								CreateMDIChild(pView, _T("EMFViewClass"), "Clipbpard");
							else
								delete pView;
					}
				}
				return TRUE;
		}

		return FALSE;
	}

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch ( uMsg )
		{
			case WM_PALETTECHANGED:
				{
					if ( hWnd != (HWND) wParam )
						for (int i=0; i<m_nChild; i++)
							SendMessage(m_Children[i], uMsg, wParam, lParam);
				}
				return 0;
		}

		return KMDIFrame::WndProc(hWnd, uMsg, wParam, lParam);
	}

	
	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KMDIFrame::GetWndClassEx(wc);
		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH));
	}

public:
	KMyMDIFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
		KToolbar * pToolbar, KStatusWindow * pStatus) :
		KMDIFrame(hInstance, pButtons, nCount, pToolbar, pStatus, Translate)
	{
		m_nChild = 0;
	}
};


const TBBUTTON tbButtons[] =
{
	{ STD_FILENEW,	 IDM_FILE_NEW,   TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_FILENEW,  0 },
	{ STD_FILEOPEN,  IDM_FILE_OPEN,  TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_FILEOPEN, 0 }
};


HINSTANCE hModule;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KToolbar      toolbar;
	KStatusWindow status;

	hModule = hInst;

	KMyMDIFrame frame(hInst, tbButtons, 2, & toolbar, & status);
	
	frame.CreateEx(0, _T("ClassName"), _T("EMF"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}
