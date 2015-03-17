//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : palette.cpp					                                     //
//  Description: Palette demo program, Chapter 13                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define WINVER		 0x0500

#define NOCRYPT

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

// #define SWITCH_BITMAP defined in preprocessor MACRO for toolbar

#include "..\..\include\wingraph.h"
#include "..\..\include\PalLoadBitmap.h"
#include "..\..\include\Octree.h"
#include "..\..\include\toolbarb.h"

#include "Resource.h"
#include "DIBWindow.h"
#include "PalWindow.h"

class KDIBView : public KScrollCanvas
{
	typedef enum { GAP = 16 };

	virtual void    OnDraw(HDC hDC, const RECT * rcPaint);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void			GetWndClassEx(WNDCLASSEX & wc);
	
	HMENU			m_hViewMenu;
	int				m_nViewOpt;
	HWND			m_hFrame;
	int				m_nUpdateCount;
	BITMAPINFO *    m_pNewBMI;

public:

	HINSTANCE		m_hInstance;
	KImage			m_DIB;

	KDIBView(void)
	{
		m_hViewMenu    = NULL;
		m_nViewOpt     = IDM_VIEW_STRETCHDIBITS;
		m_nUpdateCount = 0;
		m_pNewBMI      = NULL;
	}

	~KDIBView(void)
	{
		if ( m_pNewBMI && ( m_pNewBMI!= m_DIB.GetBMI() ) )
			delete [] (BYTE *) m_pNewBMI;
	}

	bool Initialize(const TCHAR * pFileName, HINSTANCE hInstance, KStatusWindow * pStatus, HWND hFrame)
	{
		m_hFrame = hFrame;

		if ( m_DIB.LoadFile(pFileName) )
		{
			SetSize(m_DIB.GetWidth()  + GAP*2,
					m_DIB.GetHeight() + GAP*2, 5, 5);

			m_hInstance = hInstance;
			m_pStatus   = pStatus;

			RegisterClass(_T("DIBView"), hInstance);
			
			return true;
		}
		else
			return false;
	}

	bool Initialize(BITMAPINFO * pDIB, HINSTANCE hInstance, KStatusWindow * pStatus, HWND hFrame)
	{
		m_hFrame = hFrame;

		if ( m_DIB.AttachDIB(pDIB, NULL, DIB_BMI_NEEDFREE) )
		{
			SetSize(m_DIB.GetWidth()  + GAP*2,
					m_DIB.GetHeight() + GAP*2, 5, 5);

			m_hInstance = hInstance;
			m_pStatus   = pStatus;

			RegisterClass(_T("DIBView"), hInstance);
			
			return true;
		}
		else
			return false;
	}

	bool GetTitle(const TCHAR * pFileName, TCHAR * pTitle)
	{
		if ( pFileName )
		{
			wsprintf(pTitle, "%s, %d x %d pixel, %d bits", pFileName,
				m_DIB.GetWidth(), m_DIB.GetHeight(), m_DIB.GetDepth());

			return true;
		}
		else
			return false;
	}

	void CreateNewView(BITMAPINFO * pDIB, TCHAR * pTitle);
};


void KDIBView::GetWndClassEx(WNDCLASSEX & wc)
{
	memset(& wc, 0, sizeof(wc));

	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc    = WindowProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;       
	wc.hInstance      = NULL;
	wc.hIcon          = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_PALETTE));
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground  = (HBRUSH) (COLOR_GRAYTEXT + 1);
	wc.lpszMenuName   = NULL;
	wc.lpszClassName  = NULL;
	wc.hIconSm        = NULL;
}


// Let the main frame window handle it
void KDIBView::CreateNewView(BITMAPINFO * pDIB, TCHAR * pTitle)
{
	if ( pDIB )
		SendMessage(m_hFrame, WM_USER, (WPARAM) pDIB, (LPARAM) pTitle);
}


LRESULT KDIBView::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd		= hWnd;
			m_hViewMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_DIBVIEW));
			{
				HDC hDC = GetDC(hWnd);

				m_hPalette = CreateDIBPalette(m_DIB.GetBMI());

				if ( m_hPalette==NULL )
				{
					m_hPalette  = CreateHalftonePalette(hDC);
					m_pNewBMI   = IndexColorTable(m_DIB.GetBMI(), m_hPalette);
				}
				else
					m_pNewBMI   = IndexColorTable(m_DIB.GetBMI(), NULL);

				ReleaseDC(hWnd, hDC);
			}
			return 0;

		case WM_PAINT:
			m_nUpdateCount = 0;
			return KScrollCanvas::WndProc(hWnd, uMsg, wParam, lParam);

		case WM_SIZE:
		case WM_HSCROLL:
		case WM_VSCROLL:
		{
			LRESULT lr = KScrollCanvas::WndProc(hWnd, uMsg, wParam, lParam);

			int nMin, nMax, nPos;

			GetScrollRange(m_hWnd, SB_VERT, &nMin, &nMax);

			nPos = GetScrollPos(m_hWnd, SB_VERT);

			TCHAR mess[32];
			wsprintf(mess, "%d %d %d", nMin, nPos, nMax);
			m_pStatus->SetText(0, mess);

			return lr;
		}

		case WM_PALETTEISCHANGING:
			MessageBox(NULL, "Hello", "Hi", MB_OK);
			return 0;

		case WM_QUERYNEWPALETTE:
			return OnQueryNewPalette();
		
		case WM_PALETTECHANGED:
			return OnPaletteChanged(hWnd, wParam);

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDM_VIEW_STRETCHDIBITS:
				case IDM_VIEW_STRETCHHALFTONE:
				case IDM_VIEW_STRETCHPALCOLORS:
				case IDM_VIEW_SETDIBITSTODEVICE:
				case IDM_VIEW_FITWINDOW:

				case IDM_VIEW_MASKGREEN:
				case IDM_VIEW_MASKBLUE:
					if ( LOWORD(wParam)!= m_nViewOpt )
					{
						m_nViewOpt = LOWORD(wParam);

						switch ( LOWORD(wParam) )
						{
							case IDM_VIEW_STRETCHDIBITS:
							case IDM_VIEW_STRETCHHALFTONE:
							case IDM_VIEW_STRETCHPALCOLORS:
							case IDM_VIEW_SETDIBITSTODEVICE:
							case IDM_VIEW_FITWINDOW:

							case IDM_VIEW_MASKGREEN:
							case IDM_VIEW_MASKBLUE:

								SetSize(m_DIB.GetWidth() + GAP*2, m_DIB.GetHeight() + GAP*2, 5, 5, true);
								break;
						}

						InvalidateRect(hWnd, NULL, TRUE);
					}
					return 0;

				case IDM_COLORS_CDR_CLOSEST:
					CreateNewView(Convert8bpp(m_DIB.GetBMI()), "Color Depth Reduction: Closest Match");
					return 0;

				case IDM_COLORS_CDR_ERRORDIFFUSION:
					CreateNewView(Convert8bpp_ErrorDiffusion(m_DIB.GetBMI()), "Color Depth Reduction: Error Diffusuin");
					return 0;
			}
			return 0;

		default:
			return CommonMDIChildProc(hWnd, uMsg, wParam, lParam, m_hViewMenu, 3);
	}
}

extern DWORD dibtick;

void KDIBView::OnDraw(HDC hDC, const RECT * rcPaint)
{
	DWORD tm = GetTickCount();
	
	dibtick = 0;
	int w = m_DIB.GetWidth();
	int h = m_DIB.GetHeight();

	if ( m_hPalette )
	{
		SelectPalette(hDC, m_hPalette, FALSE);
		RealizePalette(hDC);
	}

	switch ( m_nViewOpt )
	{
		case IDM_VIEW_FITWINDOW:
			{   
				RECT rect;
				SetStretchBltMode(hDC, HALFTONE);

				GetClientRect(m_hWnd, & rect);
				m_DIB.DrawDIB(hDC, 0, 0, rect.right, rect.bottom, 0, 0, w, h, SRCCOPY);
			}
			break;

		case IDM_VIEW_MASKGREEN:
			{
				KGDIObject red(hDC, CreateSolidBrush(RGB(0, 0xFF, 0)));
				m_DIB.DrawDIB(hDC, GAP,   GAP,    w, h, 0, 0,  w,  h, MERGECOPY);
			}
			break;

		case IDM_VIEW_MASKBLUE:
			{
				KGDIObject red(hDC, CreateSolidBrush(RGB(0, 0, 0xFF )));
				m_DIB.DrawDIB(hDC, GAP,   GAP,    w, h, 0, 0,  w,  h, MERGECOPY);
			}
			break;

		case IDM_VIEW_STRETCHDIBITS:
			m_DIB.DrawDIB(hDC, GAP,   GAP,    w, h, 0, 0,  w,  h, SRCCOPY);
			break;

		case IDM_VIEW_STRETCHHALFTONE:
			SetStretchBltMode(hDC, HALFTONE);
			m_DIB.DrawDIB(hDC, GAP,   GAP,    w, h, 0, 0,  w,  h, SRCCOPY);
			break;

		case IDM_VIEW_STRETCHPALCOLORS:
			StretchDIBits(hDC, GAP, GAP, w, h, 0, 0, w, h, m_DIB.GetBits(), m_pNewBMI, DIB_PAL_COLORS, SRCCOPY);
			break;

		case IDM_VIEW_SETDIBITSTODEVICE:
			if ( ! m_DIB.IsCompressed() )
			{
				int  bps      = m_DIB.GetBPS();
				BYTE * buffer = new BYTE[bps];

				for (int i=0; i<m_DIB.GetHeight(); i++)
				{
					memcpy(buffer, m_DIB.GetBits() + bps*i, bps);

					for (int j=0; j<bps; j++)
						buffer[j] = ~ buffer[j];

					m_DIB.SetDIB(hDC, GAP, GAP, i, 1, buffer);
				}
				delete [] buffer;
			}
			break;

	}
	RestoreDC(hDC, -1);

	tm = GetTickCount() - tm;

	if ( dibtick )
		tm = dibtick;

	TCHAR mess[32];
	wsprintf(mess, "Time %d ms", tm);
	m_pStatus->SetText(1, mess);
}


void Label(HDC hDC, int x, int y, const TCHAR * mess)
{
	TextOut(hDC, x, y, mess, _tcslen(mess));
}


void WebColors(HDC hDC, int x, int y, int crtyp)
{
	for (int r=0; r<6; r++)
	for (int g=0; g<6; g++)
	for (int b=0; b<6; b++)
	{
		COLORREF cr;

		switch ( crtyp )
		{
			case 0: cr = RGB(r*51, g*51, b*51);        break;
			case 1: cr = PALETTERGB(r*51, g*51, b*51); break;
			case 2: cr = PALETTEINDEX(r*36+g*6+b);     break;
		}

		HBRUSH hBrush = CreateSolidBrush(cr);
				
		RECT rect = { r * 110 + g*16+   x, b*16+   y,  
			          r * 110 + g*16+15+x, b*16+15+y};  
		FillRect(hDC, & rect, hBrush);
		
		DeleteObject(hBrush);
	}
}


void AnalyzePalette(PALETTEENTRY entry[], int no, TCHAR mess[])
{
	int web = 0;
	int gray = 0;

	for (int i=0; i<no; i++)
	{
		if ( ( (entry[i].peBlue % 0x33) + (entry[i].peGreen % 0x33) + (entry[i].peRed % 0x33) )==0 )
			web ++;
		else if ( ( entry[i].peBlue==entry[i].peGreen) && ( entry[i].peGreen==entry[i].peBlue) )
			gray ++;

	}

	wsprintf(mess, "%d web colors, %d grayscale colors", web, gray);
}


void ShowPalette(HDC hDC, int x0, int y0, HPALETTE hPal)
{
	PALETTEENTRY entries[256];

	int no;
	
	if ( hPal )
		no = GetPaletteEntries(hPal, 0, 256, entries);
	else
		no = GetSystemPaletteEntries(hDC, 0, 256, entries);
	
	SelectObject(hDC, GetStockObject(SYSTEM_FIXED_FONT));

	TCHAR temp[64];

	for (int i=0; i<no; i++)
	{
		int x = x0 + ( i % 8) * 100;
		int y = y0 + 20 + (i/8) * 18;

		wsprintf(temp, "%02X%02X%02X %d", entries[i].peRed, entries[i].peGreen, entries[i].peBlue, entries[i].peFlags);
		Label(hDC, x, y, temp);

		HBRUSH hBrush = CreateSolidBrush(PALETTERGB(entries[i].peRed, entries[i].peGreen, entries[i].peBlue));

		RECT rect = { x + 70, y, x + 96, y+16 };

		FillRect(hDC, & rect, hBrush);
		DeleteObject(hBrush);
	}

	AnalyzePalette(entries, no, temp);
	TextOut(hDC, x0, y0, temp, _tcslen(temp));
}


HPALETTE CreateGrayscalePalette(void)
{
	LOGPALETTE * pLogPal = (LOGPALETTE *) new BYTE[sizeof(LOGPALETTE) + 255 * sizeof(PALETTEENTRY)];
	
	pLogPal->palVersion    = 0x0300;
	pLogPal->palNumEntries = 256;
	
	for (int i=0; i<256; i++)
	{
		PALETTEENTRY entry = { i, i, i, 0 };

		pLogPal->palPalEntry[i]	= entry;
	}
	
	HPALETTE hPal = CreatePalette(pLogPal);

	delete [] (BYTE *) pLogPal;

	return hPal;
}


HPALETTE CreateExplicitPalette(void)
{
	LOGPALETTE * pLogPal = (LOGPALETTE *) new BYTE[sizeof(LOGPALETTE) + 255 * sizeof(PALETTEENTRY)];
	
	pLogPal->palVersion    = 0x0300;
	pLogPal->palNumEntries = 256;
	
	for (int i=0; i<256; i++)
	{
		PALETTEENTRY entry = { i, 0, 0, PC_EXPLICIT };

		pLogPal->palPalEntry[i]	= entry;
	}
	
	HPALETTE hPal = CreatePalette(pLogPal);

	delete [] (BYTE *) pLogPal;

	return hPal;
}


void TestPalette(HDC hDC, HINSTANCE hInstance, int type)
{
	HPALETTE hPal;
		
	switch ( type )
	{
		case 0:	hPal = CreateHalftonePalette(hDC); break;
	
		case 2: SetSystemPaletteUse(hDC, SYSPAL_NOSTATIC);
		case 1:	hPal = CreateGrayscalePalette();   break;

		case 3: hPal = CreateExplicitPalette();
	}

	HPALETTE hOld = SelectPalette(hDC, hPal, FALSE);
	int n = RealizePalette(hDC);

	WebColors(hDC, 10,  10, 0);
	WebColors(hDC, 10, 130, 1);
//	WebColors(hDC, 10, 250, 2);

	TCHAR temp[64];

	wsprintf(temp, "%d colors realized", n);
	TextOut(hDC, 10, 250, temp, _tcslen(temp));

	ShowPalette(hDC, 10, 280, hPal);

	SelectPalette(hDC, hOld, TRUE);
	DeleteObject(hPal);
}


void TestPalette(HDC hDC, HINSTANCE hInstance)
{
	TCHAR temp[64];

	// palette support information
	if ( GetDeviceCaps(hDC, RASTERCAPS ) && RC_PALETTE )
		Label(hDC, 10, 10, "Palette Supported");
	else
		Label(hDC, 10, 10, "Palette not supported");

	wsprintf(temp, "SIZEPALETTE %d", GetDeviceCaps(hDC, SIZEPALETTE));
	Label(hDC, 10, 30, temp);

	wsprintf(temp, "NUMRESERVED %d", GetDeviceCaps(hDC, NUMRESERVED));
	Label(hDC, 10, 50, temp);

	wsprintf(temp, "COLORRES    %d", GetDeviceCaps(hDC, COLORRES));
	Label(hDC, 10, 70, temp);

	// web colors
	WebColors(hDC, 150,  10, 0);
	WebColors(hDC, 150, 130, 1);
//	WebColors(hDC, 150, 250, 2);

	// default palette
	HPALETTE hPalette = (HPALETTE) GetCurrentObject(hDC, OBJ_PAL);
	
	ShowPalette(hDC, 10, 260, hPalette);

	// colors in system palette
	HDC hMemDC = CreateCompatibleDC(hDC);
	HGDIOBJ hOld;
	{
		BYTE data[80][80];

		for (int i=0; i<80; i++)
		for (int j=0; j<80; j++)
		{
			data[i][j] = (i/5) * 16 + (j/5);
			if ( ((i%5)==0) || ((j%5)==0) )
				data[i][j] = 255;
		}

		HBITMAP hBitmap = CreateBitmap(80, 80, 1, 8, data);

		hOld = SelectObject(hMemDC, hBitmap);

		StretchBlt(hDC, 850, 10, 256, 256, hMemDC, 0, 0, 80, 80, SRCCOPY);

		SelectObject(hMemDC, hOld);
		DeleteObject(hBitmap);
	}

	HBITMAP hBitmap = (HBITMAP) LoadImage(hInstance, MAKEINTRESOURCE(IDB_TIGER2), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);

	hOld = SelectObject(hMemDC, hBitmap);
	BITMAP bmp;
	GetObject(hBitmap, sizeof(bmp), & bmp);

	BitBlt(hDC, 10, 350, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);
	
	SelectObject(hMemDC, hOld);
	
	DeleteObject(hBitmap);

	{ // load bitmap using system palette
		HPALETTE hPal = CreateSystemPalette();
		hBitmap = PaletteLoadBitmap(hInstance, MAKEINTRESOURCE(IDB_TIGER2), hPal);
		DeleteObject(hPal);

		hOld = SelectObject(hMemDC, hBitmap);
		BITMAP bmp;
		GetObject(hBitmap, sizeof(bmp), & bmp);

		BitBlt(hDC, 40+bmp.bmWidth, 350, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);
	
		SelectObject(hMemDC, hOld);
	
		DeleteObject(hBitmap);
	}

	DeleteObject(hMemDC);
/*
	{
		HDC hDC = GetDC(NULL);

		const WORD ChessBoard88[] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };

		HBITMAP hBmp = CreateBitmap(8, 8, 1, 1, ChessBoard88);
		HBRUSH  hBrush = CreatePatternBrush(hBmp);
		HGDIOBJ hOld = SelectObject(hDC, hBrush);
		
		SetBkColor(hDC, 0);							// 1 -> white
		SetTextColor(hDC, RGB(0xFF, 0xFF, 0xFF));	// 0 -> black 
		PatBlt(hDC, 0, 0, GetDeviceCaps(hDC, HORZRES), GetDeviceCaps(hDC, VERTRES), 0xA000C9);
	
		SetBkColor(hDC, RGB(0, 0, 0xFF)); // 1 -> blue
		SetTextColor(hDC, 0);			  // 0 -> white
		PatBlt(hDC, 0, 0, GetDeviceCaps(hDC, HORZRES), GetDeviceCaps(hDC, VERTRES), 0xFA0089);
	
		SelectObject(hDC, hOld);
		DeleteObject(hBrush);
		DeleteObject(hBmp);

		ReleaseDC(NULL, hDC);
	}
*/
}


class KTestView : public KScrollCanvas
{
	virtual void OnDraw(HDC hDC, const RECT * rcPaint)
	{
		switch ( m_nViewOpt )
		{
			case IDM_VIEW_PALETTE:
				TestPalette(hDC, m_hInstance);
				break;

			case IDM_VIEW_HALFTONEPALETTE:
				TestPalette(hDC, m_hInstance, 0);
				break;

			case IDM_VIEW_GRAYSCALEPALETTE:
				TestPalette(hDC, m_hInstance, 1);
				break;

			case IDM_VIEW_GRAYSCALEFULL:
				TestPalette(hDC, m_hInstance, 2);
				break;

			case IDM_VIEW_EXPLICITPALETTE:
				TestPalette(hDC, m_hInstance, 3);
				break;
		}
	}

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch( uMsg )
		{
			case WM_CREATE:
				m_hWnd		= hWnd;
				m_hViewMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_DIBVIEW));
				SetSize(1200, 1200, 5, 5, true);

				return 0;

			case WM_MOUSEMOVE:
				{
					HDC hDC = GetDC(hWnd);

					COLORREF cr = GetPixel(hDC, LOWORD(lParam), HIWORD(lParam));
					TCHAR mess[32];

					wsprintf(mess, "RGB(%02x, %02x, %02x)", GetRValue(cr), GetGValue(cr), GetBValue(cr));
					m_pStatus->SetText(1, mess);
					ReleaseDC(hWnd, hDC);
				}
				return 0;

			case WM_PAINT:
			case WM_SIZE:
			case WM_HSCROLL:
			case WM_VSCROLL:
				return KScrollCanvas::WndProc(hWnd, uMsg, wParam, lParam);

			case WM_COMMAND:
				switch ( LOWORD(wParam) )
				{
					case IDM_VIEW_PALETTE:
					case IDM_VIEW_HALFTONEPALETTE:
					case IDM_VIEW_GRAYSCALEPALETTE:
					case IDM_VIEW_GRAYSCALEFULL:
					case IDM_VIEW_EXPLICITPALETTE:
						if ( LOWORD(wParam)!= m_nViewOpt )
						{
							m_nViewOpt = LOWORD(wParam);
							SetSize(1000, 1000, 10, 10, true);
							InvalidateRect(hWnd, NULL, TRUE);
						}
						return 0;
				}
				return 0;

			default:
				return CommonMDIChildProc(hWnd, uMsg, wParam, lParam, m_hViewMenu, 3);
		}
	}

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		memset(& wc, 0, sizeof(wc));

		wc.cbSize         = sizeof(WNDCLASSEX);
		wc.style          = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc    = WindowProc;
		wc.cbClsExtra     = 0;
		wc.cbWndExtra     = 0;       
		wc.hInstance      = NULL;
		wc.hIcon          = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_PALETTE));
		wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground  = (HBRUSH) (COLOR_WINDOW + 1);
		wc.lpszMenuName   = NULL;
		wc.lpszClassName  = NULL;
		wc.hIconSm        = NULL;
	}


	HMENU			m_hViewMenu;
	int				m_nViewOpt;

public:

	HINSTANCE		m_hInstance;

	KTestView(void)
	{
		m_hViewMenu = NULL;
		m_nViewOpt  = IDM_VIEW_PALETTE;
	}

	bool Initialize(HINSTANCE hInstance, KStatusWindow * pStatus)
	{
		m_hInstance = hInstance;
		m_pStatus   = pStatus;

		RegisterClass(_T("TestView"), hInstance);

		return true;
	}

};


BOOL Switch8bpp(void)
{
	HDC hDC = GetDC(NULL);

	int hasPalette = (GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE);
	ReleaseDC(NULL, hDC);
	
	if ( hasPalette ) // palette supported
		return TRUE;

	int rslt = MyMessageBox(NULL, "Switch to 256 color mode?", "Palette Window", MB_YESNOCANCEL, IDI_PALETTE);
			
	if ( rslt==IDCANCEL )
		return FALSE;

	if ( rslt==IDYES ) // switch to 8-bpp allowed
	{
		DEVMODE dm;
		dm.dmSize        = sizeof(dm); // important, avoid GPF
		dm.dmDriverExtra = 0;
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, & dm); // query current
		dm.dmBitsPerPel = 8;									// set to 8-bpp
		ChangeDisplaySettings(&dm, 0);							// SWITCH
	}
	
	return TRUE;
}


const int Translate[] =
{
	IDM_FILE_CLOSE,
	IDM_FILE_EXIT,
	IDM_WINDOW_TILE,
	IDM_WINDOW_CASCADE,
	IDM_WINDOW_ARRANGE,
	IDM_WINDOW_CLOSEALL
};


class KMyMDIFRame : public KMDIFrame
{
	KToolbarB *     m_pToolbarB;

	int				m_nCount;
	HWND			m_Children[100];
	int				m_nChild;

	KPaletteWnd     m_palettewnd;

	KDIBWindow	    dibwindow1;
	KDIBWindow	    dibwindow2;
	KDIBWindow	    dibwindow3;

	KDIBWindow	    dibwindow4;
	KDIBWindow	    dibwindow5;
	KDIBWindow	    dibwindow6;

	void CreateMDIChild(KScrollCanvas * canvas, const TCHAR * klass, const TCHAR * filename, const TCHAR * title)
	{
		MDICREATESTRUCT mdic;

		TCHAR Temp[MAX_PATH];

		if ( ! canvas->GetTitle(filename, Temp) )
		{
			m_nCount ++;
			wsprintf(Temp, title, m_nCount);
		}

		mdic.szClass = klass;
		mdic.szTitle = Temp;
		mdic.hOwner  = m_hInst;
		mdic.x       = CW_USEDEFAULT;
		mdic.y       = CW_USEDEFAULT;
		mdic.cx      = CW_USEDEFAULT;
		mdic.cy      = CW_USEDEFAULT;
		mdic.style   = WS_VISIBLE | WS_BORDER;
		mdic.lParam  = (LPARAM) canvas;

		HWND hWnd = (HWND) SendMessage(m_hMDIClient, WM_MDICREATE, 0, (LPARAM) & mdic);

		if ( hWnd )
			m_Children[m_nChild++] = hWnd;
	}

	void CreateDIBView(const TCHAR * pFileName)
	{
		KDIBView * pDIBView = new KDIBView();

		if ( pDIBView )
			if ( pDIBView->Initialize(pFileName, m_hInst, m_pStatus, m_hWnd) )
				CreateMDIChild(pDIBView, _T("DIBView"), pFileName, _T("DIB %d"));
			else
				delete pDIBView;
	}

	void CreateDIBView(BITMAPINFO * pDIB, const TCHAR * pTitle)
	{
		KDIBView * pDIBView = new KDIBView();

		if ( pDIBView )
			if ( pDIBView->Initialize(pDIB, m_hInst, m_pStatus, m_hWnd) )
				CreateMDIChild(pDIBView, _T("DIBView"), pTitle, _T("DIB %d"));
			else
				delete pDIBView;
	}


	void CreateTestView(void)
	{
		KTestView * pTestView = new KTestView();

		if ( pTestView )
			if ( pTestView->Initialize(m_hInst, m_pStatus) )
				CreateMDIChild(pTestView, _T("TestView"), NULL, _T("Test %d"));
			else
				delete pTestView;
	}
	
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam)
	{
		switch ( LOWORD(wParam) )
		{
			case IDM_FILE_NEW:
				CreateDIBView(NULL);
				return TRUE;

			case IDM_FILE_TEST:
				CreateTestView();
				return TRUE;

			case IDM_FILE_OPEN:
			{
				KFileDialog fo;

				if ( fo.GetOpenFileName(m_hWnd, "bmp", "Bitmap Files") )
					CreateDIBView(fo.m_TitleName);
				
				return TRUE;
			}

			case IDM_FILE_PALETTEWND:
			{
				m_palettewnd.CreatePaletteWindow(m_hInst);
				return TRUE;
			}

			case IDM_FILE_PALMESSAGE:
			{
				HRSRC hRes = FindResource(m_hInst, MAKEINTRESOURCE(IDB_TIGER1), RT_BITMAP);
				HGLOBAL hGlobal = LoadResource(m_hInst, hRes);
				BITMAPINFO * pDIB = (BITMAPINFO *) LockResource(hGlobal);

				if ( pDIB )
				{
					dibwindow1.CreateDIBWindow(m_hInst, pDIB, 
						(const BYTE *) & pDIB->bmiColors[GetDIBColorCount( pDIB->bmiHeader)], pal_no);

					dibwindow2.CreateDIBWindow(m_hInst, pDIB, 
						(const BYTE *) & pDIB->bmiColors[GetDIBColorCount( pDIB->bmiHeader)], pal_halftone);

					dibwindow3.CreateDIBWindow(m_hInst, pDIB, 
						(const BYTE *) & pDIB->bmiColors[GetDIBColorCount( pDIB->bmiHeader)], pal_halftone | pal_react);
				}

				return TRUE;
			}
 
			case IDM_FILE_DIBPAL:
			{
				dibwindow4.CreateDIBWindow(m_hInst, MAKEINTRESOURCE(IDB_FLOWER), 
					pal_halftone | pal_react);

				dibwindow5.CreateDIBWindow(m_hInst, MAKEINTRESOURCE(IDB_FLOWER), 
					pal_halftone | pal_stretchHT | pal_react);
				
				dibwindow6.CreateDIBWindow(m_hInst, MAKEINTRESOURCE(IDB_FLOWER), 
					pal_bitmap | pal_react);
				
				return TRUE;
			}
		}

		return FALSE; // not processed
	}

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch ( uMsg )
		{
			case WM_USER:
				CreateDIBView((BITMAPINFO *) wParam, (const TCHAR *) lParam);
				return 0;

			case WM_PALETTECHANGED:
				{
					if ( hWnd != (HWND) wParam )
						for (int i=0; i<m_nChild; i++)
							SendMessage(m_Children[i], uMsg, wParam, lParam);

					m_pToolbarB->SetBitmap(m_hInst, IDR_277);
				}
				return 0;

			default:
			{
				LRESULT lr = KMDIFrame::WndProc(hWnd, uMsg, wParam, lParam);
				if ( uMsg == WM_CREATE )
					m_pToolbarB->SetBitmap(m_hInst, IDR_277);

				return lr;
			}
		}
	}

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KMDIFrame::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_PALETTE));
	}

public:
	KMyMDIFRame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
		KToolbarB * pToolbar, KStatusWindow * pStatus) :
		KMDIFrame(hInstance, pButtons, nCount, pToolbar, pStatus, Translate)
	{
		m_pToolbarB = pToolbar;
		m_nCount    = 0;
		m_nChild    = 0;
	}
};


const TBBUTTON tbButtons[] =
{
	{ STD_FILENEW,	 IDM_FILE_NEW,   TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_FILENEW,  0 },
	{ STD_FILEOPEN,  IDM_FILE_OPEN,  TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_FILEOPEN, 0 }
};



HPALETTE CreateDIBSectionPalette(HDC hDC, HBITMAP hDIBSec)
{
	HDC hMemDC   = CreateCompatibleDC(hDC);
	HGDIOBJ hOld = SelectObject(hMemDC, hDIBSec);

	RGBQUAD Color[256];

	int nEntries = GetDIBColorTable(hMemDC, 0, 256, Color);

	HPALETTE hPal = LUTCreatePalette((BYTE *) Color, sizeof(RGBQUAD), nEntries);

	SelectObject(hMemDC, hOld);
	DeleteObject(hMemDC);
	
	return hPal;
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KToolbarB     toolbar;
	KStatusWindow status;

	KDIBWindow	  dibwindow5;

	KMyMDIFRame frame(hInst, tbButtons, 2, & toolbar, & status);

	TCHAR title[MAX_PATH];

	_tcscpy(title, "Palette");

	frame.CreateEx(0, _T("ClassName"), title,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}

