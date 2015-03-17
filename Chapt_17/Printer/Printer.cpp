//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : printer.cpp					                                     //
//  Description: Printer demo program, Chapter 17                                    //
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
#include <stdio.h>

#include "..\..\include\win.h"
#include "..\..\include\dialog.h"
#include "..\..\include\Toolbar.h"
#include "..\..\include\Status.h"
#include "..\..\include\FrameWnd.h"
#include "..\..\include\GDIObject.h"
#include "..\..\include\pen.h"
#include "..\..\include\filedialog.h"
#include "..\..\include\outputsetup.h"
#include "..\..\include\fonttext.h"
#include "..\..\include\pagecanvas.h"
#include "..\..\include\areafill.h"

#include "resource.h"

//////////////////////////////////////////////////////////////

int nCall_AbortProc;

BOOL CALLBACK SimpleAbortProc(HDC hDC, int iError)
{
	nCall_AbortProc ++;

	return TRUE;
}

void SimplePrint(int nPages)
{
	TCHAR temp[MAX_PATH];

	DWORD size = MAX_PATH;
	GetDefaultPrinter(temp, & size); // default printer name

	HDC hDC = CreateDC(NULL, temp, NULL, NULL); // DC with default setting
	
	if ( hDC )
	{
		nCall_AbortProc = 0;
		SetAbortProc(hDC, SimpleAbortProc);

		DOCINFO docinfo;
		docinfo.cbSize       = sizeof(docinfo);
		docinfo.lpszDocName  = _T("SimplePrint");
		docinfo.lpszOutput   = NULL; 
		docinfo.lpszDatatype = _T("EMF");
		docinfo.fwType		 = 0; 

		if ( StartDoc(hDC, & docinfo) > 0 )
		{
			for (int p=0; p<nPages; p++) // one page at a time
				if ( StartPage(hDC) <= 0 )
					break;
				else
				{
					int width  = GetDeviceCaps(hDC, HORZRES);
					int height = GetDeviceCaps(hDC, VERTRES);
					int dpix   = GetDeviceCaps(hDC, LOGPIXELSX);
					int dpiy   = GetDeviceCaps(hDC, LOGPIXELSY);
			
					wsprintf(temp, _T("Page %d of %d"), p+1, nPages);
					SetTextAlign(hDC, TA_TOP | TA_RIGHT );
					TextOut(hDC, width, 0, temp, _tcslen(temp));

					Rectangle(hDC, 0, 0, dpix, dpiy);
					Rectangle(hDC, width, height, width-dpix, height-dpiy);
				
					if ( EndPage(hDC)<0 )
						break;
				}
			
			EndDoc(hDC);
		}

		DeleteDC(hDC);
	}

	wsprintf(temp, "AbortProc called %d times", nCall_AbortProc);
	MessageBox(NULL, temp, "SimlePrint", MB_OK);	
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

void Demo_LineCurve(HDC hDC, const RECT * rcPaint, int width, int height)
{
	// ROP2
	KLogFont logfont(- 10 * ONEINCH / 72, "Tahoma");
	KGDIObject font(hDC, logfont.CreateFont());

	for (int c=0; c<20; c++)
	{
		RECT rect = { c*200+400, 300, c*200+580, 4000 };

		HBRUSH hBrush = CreateSolidBrush(PALETTEINDEX(c));

		FillRect(hDC, & rect, hBrush);

		DeleteObject(hBrush);
	}
	
	{
		KGDIObject redbrush(hDC, CreateSolidBrush(RGB(0xFF, 0, 0)));
		SelectObject(hDC, GetStockObject(NULL_PEN));

		SetTextAlign(hDC, TA_TOP | TA_LEFT);

		for (int r=R2_BLACK; r<=R2_WHITE; r++)
		{
			SetROP2(hDC, r);
			TextOut(hDC, 4400, r*220+200, R2_Names[r-R2_BLACK], _tcslen(R2_Names[r-R2_BLACK]));

			Rectangle(hDC, 300, r*220+200, 4300, r*220+400);
		}
	
		SetROP2(hDC, R2_COPYPEN);
	}

	{
		KPen Red (PS_DOT,            0, RGB(0xFF, 0, 0));
		KPen Blue(PS_SOLID, ONEINCH/36, RGB(0, 0, 0xFF));

		for (int z=0; z<=2000; z+=400)
		{
			int x = 400, y = 6400;

			POINT p[4] = { x, y, x+ 400, y-z, x+800, y-z, x+1200, y }; x+= 1300;
			POINT q[4] = { x, y, x+ 400, y-z, x+800, y+z, x+1200, y }; x+= 1400;
		
			POINT r[4] = { x, y, x+1500, y-z, x- 200, y-z, x+1300, y }; x+= 1800;
			POINT s[4] = { x, y, x+1500, y-z, x- 200, y+z, x+1300, y }; x+= 1600;
		
			POINT t[4] = { x+600, y, x,  y-z, x+1300, y-z, x+600, y }; 

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
		}
	}

}

TCHAR * HS_Names [] = 
{	_T("HS_HORIZONTAL"),
	_T("HS_VERTICAL"),
	_T("HS_FDIAGONAL"),
	_T("HS_BDIAGONAL"),
	_T("HS_CROSS"),
	_T("HS_DIAGCROSS")
};

void Demo_AreaFill(HDC hDC, const RECT * rcPaint, int width, int height)
{
	const COLORREF c0 = RGB(0x20, 0x20, 0x20);
	const COLORREF c1 = RGB(0xF0, 0xF0, 0x20);

	for (int i=0; i<4; i++)
		GradientRectangle(hDC, 1000+1100*i, 500, 2000+1100*i, 1500, c0, c1, i*450);

	for (i=0; i<4; i++)
		SymGradientRectangle(hDC, 1000+1100*i, 1600, 2000+1100*i, 2600, c0, c1, i*450);

	for (i=0; i<4; i++)
		CornerGradientRectangle(hDC, 1000+1100*i, 2700, 2000+1100*i, 3700, c0, c1, i);

	CenterGradientRectangle(hDC, 5600, 1500, 6600, 2500, c0, c1);
	CenterGradientRectangle(hDC, 5600, 2600, 6600, 3600, c1, c0);

	//  Buttons
	RoundRectButton(hDC, 0,    4000,  800, 4800,   0, 100, RGB(0x20, 0x20, 0x20), RGB(0xF0, 0xF0, 0x20));
	RoundRectButton(hDC, 1000, 4000, 1800, 4800, 400, 100, RGB(0xF0, 0x20, 0x20), RGB(0x20, 0xF0, 0x20));
	RoundRectButton(hDC, 2000, 4000, 2800, 4800, 800, 100, RGB(0xFF, 0xFF, 0x20), RGB(0x20, 0x20, 0xF0));
	
	   GradientRectangle(hDC, 0, 5000, 2000, 6000, RGB(0xFF, 0x0, 0), RGB(0, 0, 0xFF), 0);
	HLSGradientRectangle(hDC, 0, 6200, 2000, 7200, RGB(0xFF, 0x0, 0), RGB(0, 0, 0xFF), 200);

	RadialGradientFill(hDC, 3200, 6300, 3200    , 6300   ,  1000, RGB(0xFF, 0xFF, 0xFF), RGB(0, 0, 0xFF), 8);
	RadialGradientFill(hDC, 5300, 6300, 5300-300, 6300-600, 1000, RGB(0xFF, 0xFF, 0xFF), RGB(0, 0, 0xFF), 16);
	RadialGradientFill(hDC, 7400, 6300, 7400-300, 6300+300, 1000, RGB(0xFF, 0xFF, 0xFF), RGB(0, 0, 0xFF), 256);

	{
		RECT r = { 7000, 500, 8500, 500+1500 };

		KGDIObject blue(hDC, CreatePen(PS_SOLID, 1 * ONEINCH/72, RGB(0, 0, 0xFF)));
		KGDIObject yellow(hDC, CreateSolidBrush(RGB(0xFF, 0xFF, 0)));
	
		Rectangle(hDC, 7000, 500, 8500, 500+1500);
		
		BrickPatternFill(hDC, 7000,  500, 8500,	 500+1500, 150, 150);
		BrickPatternFill(hDC, 7000, 2100, 8500, 2100+1500, 100, 100);
	}

	KLogFont logfont(- 8 * ONEINCH / 72, "Tahoma");
	KGDIObject font(hDC, logfont.CreateFont());

	{
		SelectObject(hDC, GetStockObject(NULL_PEN));
	
		typedef enum { GAP = 1200 };

		for (int hs= HS_HORIZONTAL; hs<=HS_DIAGCROSS; hs++)
		{
			HBRUSH hBrush = CreateHatchBrush(hs, RGB(0, 0, 0xFF));
			HGDIOBJ hOld  = SelectObject(hDC, hBrush);

			SetBkColor(hDC, RGB(0xFF, 0xFF, 0));
			Rectangle(hDC, hs*GAP, 8000, hs*GAP+890, 8890);

			SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
		
			SetTextAlign(hDC, TA_CENTER);
			TextOut(hDC, hs*GAP+880/2, 8980, HS_Names[hs-HS_HORIZONTAL], _tcslen(HS_Names[hs-HS_HORIZONTAL]));

			SelectObject(hDC, hOld);
			DeleteObject(hBrush);
		}
	}

	{
		HINSTANCE hCards = LoadLibrary("cards.dll");

		for (int i=0; i<3; i++)
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
				{
					HRSRC hResource = FindResource(hCards, MAKEINTRESOURCE(52-28), RT_BITMAP);
					HGLOBAL hGlobal = LoadResource(hCards, hResource);

					hBrush  = CreateDIBPatternBrush(hGlobal, DIB_RGB_COLORS);
					width   = ((BITMAPCOREHEADER *) hGlobal)->bcWidth;  // old DIB format
					height  = ((BITMAPCOREHEADER *) hGlobal)->bcHeight; // old DIB format
				}
		}
		
		HGDIOBJ hOld  = SelectObject(hDC, hBrush);

		POINT P = { i*1400+200 + width*10*i/4, 10000 + height*10*i/4 };
		LPtoDP(hDC, &P, 1);
		SetBrushOrgEx(hDC, P.x, P.y, NULL); // make sure cards aligned with rectangle
	
		Rectangle(hDC, i*1400+200, 10000, i*1400+200+width*30/2+1, 10000+height*30/2+1);
	
		SelectObject(hDC, hOld);
		DeleteObject(hBrush);

	}
	}

}


void Demo_Bitmap(HDC hDC, const RECT * rcPaint, int width, int height, HINSTANCE hInst)
{
	HDC hMemDC = CreateCompatibleDC(NULL);

	// load difference BITMAP as resource
	HPALETTE hPal = CreateHalftonePalette(hDC);
	HPALETTE hOld = SelectPalette(hDC, hPal, FALSE);
	RealizePalette(hDC);

	SetStretchBltMode(hDC, STRETCH_DELETESCANS);

	for (int i=0; i<6; i++)
	{
		int x = 100 + (i%3) * 3200;
		int y = 100 + (i/3) * 3200;

		HBITMAP hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(i+IDB_BITMAP1));

		KGDIObject bmp(hMemDC, hBmp);

		if ( hBmp==NULL ) break;

		BITMAP info;
		GetObject(hBmp, sizeof(BITMAP), & info);

		StretchBlt(hDC, x, y, 
			info.bmWidth * ONEINCH / 120, info.bmHeight * ONEINCH / 120, 
			hMemDC, 0, 0, info.bmWidth, info.bmHeight, SRCCOPY);
	}

	SelectPalette(hDC, hOld, FALSE);
	RealizePalette(hDC);
	DeleteObject(hPal);

	DeleteObject(hMemDC);
}


void Demo_Text(HDC hDC, const RECT * rcPaint, int width, int height)
{
	KLogFont logfont(- 10 * ONEINCH / 72, "Times New Roman");
	KGDIObject font(hDC, logfont.CreateFont());

	const TCHAR * mess = "A quick brown fox jumps over a lazy dog. "
						 "A quick brown fox jumps over a lazy dog. "
						 "A quick brown fox jumps over a lazy dog. ";

	SetBkMode(hDC, TRANSPARENT);
	SetTextAlign(hDC, TA_LEFT | TA_BASELINE);
	int y = ONEINCH/2;
	
	// use normal GDI TextOut
	SIZE size;
	GetTextExtentPoint32(hDC, mess, _tcslen(mess), & size);
	MoveToEx(hDC, 0, y, NULL); LineTo(hDC, size.cx, y);

	TextOut(hDC, 0, y, mess, _tcslen(mess));

	TCHAR temp[64];
	wsprintf(temp, "size = { %d, %d }", size.cx, size.cy);
	TextOut(hDC, 0, y + size.cy, temp, _tcslen(temp));

	y += size.cy * 3;

	KTextFormator format;

	format.SetupPixel(hDC, (HFONT) font.m_hObj, (float)(10.0 * ONEINCH/72));

	float wid, hei;
	format.GetTextExtent(hDC, mess, _tcslen(mess), wid, hei);
	
	MoveToEx(hDC, 0, y, NULL);
	LineTo(hDC, (int) wid, y);
	format.TextOut(hDC, 0, y, mess, _tcslen(mess));
	
	sprintf(temp, "size = { %5.3f, %5.3f }", wid, hei);
	TextOut(hDC, 0, y + size.cy, temp, _tcslen(temp));
}


class KTestPageCanvas : public KPageCanvas
{
	int m_demo;

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KPageCanvas::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_PRINT));
	}

public:
	virtual void UponDrawPage(HDC hDC, const RECT * rcPaint, int width, int height, int pageno)
	{
		switch ( m_demo )
		{
			case 0:
				KSurface::UponDrawPage(hDC, rcPaint, width, height, pageno);
				break;

			case IDM_VIEW_LINECURVE:
				Demo_LineCurve(hDC, rcPaint, width, height);
				break;

			case IDM_VIEW_AREAFILL:
				Demo_AreaFill(hDC, rcPaint, width, height);
				break;

			case IDM_VIEW_BITMAP:
				Demo_Bitmap(hDC, rcPaint, width, height, m_hInst);
				break;

			case IDM_VIEW_TEXT:
				Demo_Text(hDC, rcPaint, width, height);
				break;
		}
	}

	virtual int OnCommand(int cmd, HWND hWnd)
	{
		switch ( cmd )
		{
			case IDM_VIEW_LINECURVE:
			case IDM_VIEW_AREAFILL:
			case IDM_VIEW_BITMAP:
			case IDM_VIEW_TEXT:
				if ( cmd==m_demo )
					return View_NoChange;
				else
				{
					m_demo = cmd;
					return View_Redraw;
				}
				break;

			case IDM_VIEW_ZOOM500  : return SetZoom(500);
			case IDM_VIEW_ZOOM400  : return SetZoom(400);
			case IDM_VIEW_ZOOM200  : return SetZoom(200); 
			case IDM_VIEW_ZOOM150  : return SetZoom(150); 
			case IDM_VIEW_ZOOM100  : return SetZoom(100);
			case IDM_VIEW_ZOOM75   : return SetZoom( 75);
			case IDM_VIEW_ZOOM50   : return SetZoom( 50);
			case IDM_VIEW_ZOOM25   : return SetZoom( 25);
			case IDM_VIEW_ZOOM10   : return SetZoom( 10);

			case IDM_FILE_PRINT    : UponFilePrint();	  GetDimension(); return View_Resize;
			case IDM_FILE_PAGESETUP: UponFilePageSetup(); GetDimension(); return View_Resize;

			case IDM_FILE_PROPERTY:  
			{
				int nControlID[] = { IDC_LIST, IDC_DEFAULT, IDC_PRINTERS, 
					IDC_PRINTERPROPERTIES, IDC_ADVANCEDDOCUMENTPROPERTIES, IDC_DOCUMENTPROPERTIES };
			
				ShowProperty(m_OutputSetup, m_hInst, nControlID, IDD_PROPERTY);
			}
		}

		return View_NoChange;
	}
	
	KTestPageCanvas(HBRUSH hBackground) : KPageCanvas(hBackground)
	{
		m_demo = 0;
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


BOOL SendFile(HANDLE hOutput, const TCHAR * filename, bool bPrinter)
{
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, 
						NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( hFile==INVALID_HANDLE_VALUE )
		return FALSE;

	char buffer[1024];

	for (int size = GetFileSize(hFile, NULL); size; )
	{
		DWORD dwRead = 0, dwWritten = 0;

		if ( ! ReadFile(hFile, buffer, min(size, sizeof(buffer)), & dwRead, NULL) )
			break;

		if ( bPrinter )
			WritePrinter(hOutput, buffer, dwRead, & dwWritten);
		else
			WriteFile(hOutput, buffer, dwRead, & dwWritten, NULL);
		
		size -= dwRead;
	}

	return TRUE;
}

void Demo_WritePort(void)
{
	KFileDialog fd;

	if ( fd.GetOpenFileName(NULL, "prn", "Raw printer data") )
	{
		HANDLE hPort = CreateFile("lpt1:", GENERIC_WRITE, FILE_SHARE_READ,
		                   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if ( hPort!=INVALID_HANDLE_VALUE )
		{
			SendFile(hPort, fd.m_TitleName, false);
			CloseHandle(hPort);
		}
	}
}

void Demo_WritePrinter(void)
{
	PRINTDLG  pd;

	memset(&pd, 0, sizeof(PRINTDLG));
	pd.lStructSize = sizeof(PRINTDLG);

	if ( PrintDlg(&pd)==IDOK )
	{
		HANDLE hPrinter;

		DEVMODE * pDevMode = (DEVMODE *) GlobalLock(pd.hDevMode);

		PRINTER_DEFAULTS prn;
		prn.pDatatype     = "NT EMF 1.008";
		prn.pDevMode      = pDevMode;
		prn.DesiredAccess = PRINTER_ACCESS_USE;

		if ( OpenPrinter((char *) pDevMode->dmDeviceName, & hPrinter, & prn) )
		{
			KFileDialog fd;

			if ( fd.GetOpenFileName(NULL, "spl", "Windows 2000 EMF Spool file") )
			{
				DOC_INFO_1 docinfo;

				docinfo.pDocName    = "Testing WritePrinter";
				docinfo.pOutputFile = NULL;
				docinfo.pDatatype   = "NT EMF 1.008";

				StartDocPrinter(hPrinter, 1, (BYTE *) & docinfo);
				StartPagePrinter(hPrinter);
				
				SendFile(hPrinter, fd.m_TitleName, true);
				
				EndPagePrinter(hPrinter);
				EndDocPrinter(hPrinter);
			}

			ClosePrinter(hPrinter);
		}

		if ( pd.hDevMode )	GlobalFree(pd.hDevMode);
		if ( pd.hDevNames ) GlobalFree(pd.hDevNames);
	}
}


void Demo_OutputSetup(bool bShowDialog)
{
	KOutputSetup setup;

	DWORD flags = PD_RETURNDC;
	if ( ! bShowDialog )
		flags |= PD_RETURNDEFAULT;

	if ( IDOK == setup.PrintDialog(flags) )
	{
		HDC hDC = setup.GetPrinterDC();
		
		TCHAR temp[32];
		wsprintf(temp, "Printer DC=%x", hDC);
		MessageBox(NULL, temp, "KOutputSetup", MB_OK);
	}
}

class KMyMDIFrame : public KMDIFrame
{
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

	BOOL CreatePageCanvas(const TCHAR * Title)
	{
		KPageCanvas  * pCanvas;
		
		pCanvas = new KTestPageCanvas(GetSysColorBrush(COLOR_BTNSHADOW));

		if ( pCanvas )
		{
			if ( pCanvas->Initialize(m_hInst, m_pStatus, IDR_DEMO) )
			{
				CreateMDIChild(pCanvas, pCanvas->GetClassName(), Title);
				return TRUE;
			}

			delete pCanvas;
		}

		return FALSE;
	}

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam)
	{
		switch ( LOWORD(wParam) )
		{
			case IDM_FILE_DIRECTPRINT:
				Demo_WritePort();
				return TRUE;

			case IDM_FILE_OPENPRINTER:
				Demo_WritePrinter();
				return TRUE;

			case IDM_FILE_PRINTDIALOGDEFAULT:
				Demo_OutputSetup(false);
				return TRUE;

			case IDM_FILE_PRINTDIALOGPROMPT:
				Demo_OutputSetup(true);
				return TRUE;

			case IDM_FILE_DEFAULTPRINTERDC:
				{
					TCHAR name[64];

					DWORD size = 64;
					GetDefaultPrinter(name, & size);

					HDC hDC = CreateDC(NULL, name, NULL, NULL);
					
					if ( hDC )
						MessageBox(NULL, "Printer DC Created", name, MB_OK);
					DeleteDC(hDC);
				}
				return TRUE;

			case IDM_FILE_SIMPLEPRINT:
				SimplePrint(2);
				return TRUE;

			case IDM_FILE_NEW:
			{
				CreatePageCanvas(_T("Test Printer "));
				return TRUE;
			}
		}

		return FALSE;
	}

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return KMDIFrame::WndProc(hWnd, uMsg, wParam, lParam);
	}

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KMDIFrame::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_PRINT));
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
	
	frame.CreateEx(0, _T("ClassName"), _T("Printer"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}
