//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : bitmaps.cpp					                                     //
//  Description: Basic bitmap demo program, Chapter 10                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define NOCRYPT
#define OEMRESOURCE

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "..\..\include\wingraph.h"
#include "..\..\include\CheckMark.h"
#include "..\..\include\BitmapMenu.h"
#include "..\..\include\EditView.h"
#include "..\..\include\Background.h"

#include "Resource.h"

BOOL SaveWindow(HWND hWnd, bool bClient, int nFrame, COLORREF crFrame);

class KDIBView : public KScrollCanvas
{
	typedef enum { GAP = 16 };

	virtual void    OnDraw(HDC hDC, const RECT * rcPaint);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void			GetWndClassEx(WNDCLASSEX & wc);
	
	HMENU			m_hViewMenu;
	int				m_nViewOpt;

public:

	HINSTANCE		m_hInst;
	KDIB			m_DIB;

	KDIBView(void)
	{
		m_hViewMenu = NULL;
		m_nViewOpt  = IDM_VIEW_STRETCHDIBITS;
	}

	bool Initialize(const TCHAR * pFileName, HINSTANCE hInstance, KStatusWindow * pStatus)
	{
		if ( m_DIB.LoadFile(pFileName) )
		{
			SetSize(m_DIB.GetWidth()  + GAP*2,
					m_DIB.GetHeight() + GAP*2, 5, 5);

			m_hInst   = hInstance;
			m_pStatus = pStatus;

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
	wc.hIcon          = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_IMAGE));
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground  = (HBRUSH) (COLOR_GRAYTEXT + 1);
	wc.lpszMenuName   = NULL;
	wc.lpszClassName  = NULL;
	wc.hIconSm        = NULL;
}


LRESULT KDIBView::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd		= hWnd;
			m_hViewMenu = LoadMenu(m_hInst, MAKEINTRESOURCE(IDR_DIBVIEW));
			return 0;

		case WM_PAINT:
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

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDM_VIEW_STRETCHDIBITS:
				case IDM_VIEW_STRETCHDIBITS4:
				case IDM_VIEW_SETDIBITSTODEVICE:
				case IDM_VIEW_FITWINDOW:
					if ( LOWORD(wParam)!= m_nViewOpt )
					{
						m_nViewOpt = LOWORD(wParam);

						switch ( LOWORD(wParam) )
						{
							case IDM_VIEW_STRETCHDIBITS:
							case IDM_VIEW_SETDIBITSTODEVICE:
								SetSize(m_DIB.GetWidth() + GAP*2, m_DIB.GetHeight() + GAP*2, 5, 5, true);
								break;

							case IDM_VIEW_STRETCHDIBITS4:
								SetSize(m_DIB.GetWidth()*2 + GAP*3, m_DIB.GetHeight()*2 + GAP*3, 5, 5, true);
						}

						InvalidateRect(hWnd, NULL, TRUE);
					}
					return 0;

				case IDM_FILE_SAVEDIB1BPP:	SaveWindowToDIB(hWnd,  1, BI_RGB); return 0;
				case IDM_FILE_SAVEDIB4BPP:	SaveWindowToDIB(hWnd,  4, BI_RGB); return 0;
				case IDM_FILE_SAVEDIB4RLE:	SaveWindowToDIB(hWnd,  4, BI_RLE4); return 0;
				case IDM_FILE_SAVEDIB8BPP:	SaveWindowToDIB(hWnd,  8, BI_RGB); return 0;
				case IDM_FILE_SAVEDIB8RLE:	SaveWindowToDIB(hWnd,  8, BI_RLE8); return 0;
				case IDM_FILE_SAVEDIB16BPP: SaveWindowToDIB(hWnd, 16, BI_RGB); return 0;
		  case IDM_FILE_SAVEDIB16BITFIELDS: SaveWindowToDIB(hWnd, 16, BI_BITFIELDS); return 0;
				case IDM_FILE_SAVEDIB24BPP: SaveWindowToDIB(hWnd, 24, BI_RGB); return 0;
				case IDM_FILE_SAVEDIB32BPP: SaveWindowToDIB(hWnd, 32, BI_RGB); return 0;
		  case IDM_FILE_SAVEDIB32BITFIELDS: SaveWindowToDIB(hWnd, 32, BI_BITFIELDS); return 0;

			case IDM_FILE_SAVEDIBSECTION:  SaveWindow(hWnd, true, 25, RGB(209, 177, 80)); return 0;

				case IDM_VIEW_DIBHEXDUMP:
					SendMessage(GetParent(GetParent(hWnd)), WM_USER+1, (WPARAM) & m_DIB, 0);	
			}
			return 0;

		default:
			return CommonMDIChildProc(hWnd, uMsg, wParam, lParam, m_hViewMenu, 3);
	}
}

void KDIBView::OnDraw(HDC hDC, const RECT * rcPaint)
{
//	HDC		hMemDC  = CreateCompatibleDC(hDC);
//	HBITMAP hBitmap = CreateBitmap(m_DIB.GetWidth(), m_DIB.GetHeight(), 1, 1, NULL);
//	HGDIOBJ hOld    = SelectObject(hMemDC, hBitmap);
//
//	SetBkColor(hMemDC, RGB(0xFF, 0xFF, 0xFF));
//	SetTextColor(hMemDC, RGB(0xFF, 0xFF, 0xFF));
//	SetStretchBltMode(hMemDC, STRETCH_DELETESCANS);
//	m_DIB.StretchDIBits(hMemDC, 0, 0, SRCCOPY);

//	BitBlt(hDC, GAP, GAP, m_DIB.GetWidth(), m_DIB.GetHeight(), 
//		        hMemDC, 0, 0, SRCCOPY);

//	SelectObject(hMemDC, hOld);
//	DeleteObject(hBitmap);
//	DeleteObject(hMemDC);

	SaveDC(hDC);
//	CAffine affine;
//	affine.Rotate(5);
//	affine.Translate(20, 20);
//	SetGraphicsMode(hDC, GM_ADVANCED);
//	SetWorldTransform(hDC, & affine.m_xm);

	int w = m_DIB.GetWidth();
	int h = m_DIB.GetHeight();

	switch ( m_nViewOpt )
	{
		case IDM_VIEW_FITWINDOW:
			{
				RECT rect;

				GetClientRect(m_hWnd, & rect);
				m_DIB.DrawDIB(hDC, 0, 0, rect.right, rect.bottom, 0, 0, w, h, SRCCOPY);
			}
			break;

		case IDM_VIEW_STRETCHDIBITS:
			m_DIB.DrawDIB(hDC, GAP,   GAP,    w, h, 0, 0,  w,  h, SRCCOPY);
			break;

		case IDM_VIEW_STRETCHDIBITS4:
			m_DIB.DrawDIB(hDC, GAP,     GAP,     w, h, 0, 0,  w,  h, SRCCOPY);
			m_DIB.DrawDIB(hDC, GAP*2+w, GAP,     w, h, w, 0, -w,  h, SRCCOPY);
			m_DIB.DrawDIB(hDC, GAP,     GAP*2+h, w, h, 0, h,  w, -h, SRCCOPY);
			m_DIB.DrawDIB(hDC, GAP*2+w, GAP*2+h, w, h, w, h, -w, -h, SRCCOPY);
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
}



//////////////////////

const int TextureID[] = 
{ 
	IDB_BRICK01, 
	IDB_BRICK02, 
	IDB_WOOD01, 
	IDB_WOOD02, 
	IDB_ROCK01, 
	IDB_MARBLE01, 
	IDB_PAPER01 
};

class KDDBView : public KScrollCanvas
{
	typedef enum { GAP = 16 };

	virtual void    OnDraw(HDC hDC, const RECT * rcPaint);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void			GetWndClassEx(WNDCLASSEX & wc);
	
	HMENU			m_hViewMenu;
	int				m_nViewOpt;

	void			TouchMenu(HMENU hMenu);
	KCheckMark		m_uibmp;
	KBitmapMenu     m_texture;

public:

	KDDBView(void)
	{
		m_hViewMenu = NULL;
		m_nViewOpt  = IDM_VIEW_CREATEBITMAP;
	}

	bool Initialize(HINSTANCE hInstance, KStatusWindow * pStatus)
	{
		// SetSize(m_DIB.GetWidth()  + GAP*2,	m_DIB.GetHeight() + GAP*2, 5, 5);

		m_hInst   = hInstance;
		m_pStatus = pStatus;

		RegisterClass(_T("DDBView"), hInstance);
			
		return true;
	}

	void Test_CreateBitmap(HDC hDC, const RECT * rcPaint);
	void Test_LargestDDB(HDC hDC, const RECT * rcPaint);
	void Test_LoadBitmap(HDC hDC, const RECT * rcPaint);
	void Test_Blt(HDC hDC, const RECT * rcPaint);
	void Test_Blt_Color(HDC hDC, const RECT * rcPaint);
	void Test_Blt_GenMask(HDC hDC, const RECT * rcPaint);
};


void KDDBView::TouchMenu(HMENU hMainMenu)
{
	HMENU hMenu = GetSubMenu(hMainMenu, 3);

	m_uibmp.LoadToolbar(m_hInst, IDB_LICON, true);

	for (int i=0; i<20; i++)
		m_uibmp.SetCheckMarks(hMenu, i, MF_BYPOSITION, i, -1);

	m_texture.AddToMenu(GetSubMenu(hMainMenu, 4), 7, m_hInst, TextureID, 10);
}


void KDDBView::GetWndClassEx(WNDCLASSEX & wc)
{
	memset(& wc, 0, sizeof(wc));

	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc    = WindowProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;       
	wc.hInstance      = NULL;
	wc.hIcon          = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_IMAGE));
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground  = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName   = NULL;
	wc.lpszClassName  = NULL;
	wc.hIconSm        = NULL;
}


LRESULT KDDBView::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd		= hWnd;
			m_hViewMenu = LoadMenu(m_hInst, MAKEINTRESOURCE(IDR_DDBVIEW));
			TouchMenu(m_hViewMenu);
			return 0;

		case WM_PAINT:
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

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDM_VIEW_CREATEBITMAP:
				case IDM_VIEW_LARGESTDDB:
				case IDM_VIEW_LOADBITMAP:
				case IDM_VIEW_LOADBITMAPHEX:
				case IDM_VIEW_CREATEDIBITMAP:
				case IDM_VIEW_CREATEDIBITMAPHEX:

				case IDM_VIEW_BLT_NORMAL:
				case IDM_VIEW_BLT_CENTER:
				case IDM_VIEW_BLT_STRETCH:
				case IDM_VIEW_BLT_TILE:
				case IDM_VIEW_BLT_STRETCHPROP:
				case IDM_VIEW_BITBLT_COLOR:
				case IDM_VIEW_GENMASK:
					if ( LOWORD(wParam)!= m_nViewOpt )
					{
						m_nViewOpt = LOWORD(wParam);

						InvalidateRect(hWnd, NULL, TRUE);
					}
					return 0;
				
				default:
					switch ( m_texture.OnCommand(LOWORD(wParam)) )
					{
						case 1: return 0;
						case 2: InvalidateRect(hWnd, NULL, TRUE); return 0;
					}
					break;
			}
	}

	return CommonMDIChildProc(hWnd, uMsg, wParam, lParam, m_hViewMenu, 3);
}



// special routine to display a 8x8 DDB as 32x32
void TestDDB88(HDC hDC, int x, int y, HBITMAP hBmp, const TCHAR * desp)
{
	HDC hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, hBmp);

	StretchBlt(hDC, x, y, 32, 32, hMemDC, 0, 0, 8, 8, SRCCOPY);
	
	TCHAR mess[64];

	TextOut(hDC, x+50, y, desp, _tcslen(desp));
		
	DecodeDDB(hBmp, mess);
	TextOut(hDC, x+50, y+20, mess, _tcslen(mess));

	DeleteObject(hMemDC);
	DeleteObject(hBmp);
}


void KDDBView::Test_CreateBitmap(HDC hDC, const RECT * rcPaint)
{
	// Test supported DDB format	
	const SIZE format[] = { 
			{ 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 4 }, { 1, 8 }, 
			{ 1, 15 }, { 1, 16}, { 1, 24 }, { 1, 32 }, { 1, 48 },
			{ 2, 2 }, { 3, 3 }, { 4, 4 }, { 5, 5 }, { 6, 6 } };

	TextOut(hDC, 100, 10, "Supported DDB Formats", 21);

	for (int i=0; i<sizeof(format)/sizeof(format[0]); i++)
	{
		TCHAR mess[128];
	
		HBITMAP hBmp = CreateBitmap(16, 16, format[i].cx, format[i].cy, NULL);

		wsprintf(mess, "CreateBitmap(16, 16, %d, %d, NULL) ", format[i].cx, format[i].cy);
		TextOut(hDC, 10, 30+i*17, mess, _tcslen(mess));

		DecodeDDB(hBmp, mess);
		TextOut(hDC, 250, 30+i*17, mess, _tcslen(mess));
		DeleteObject(hBmp);
	}

	
	SetViewportOrgEx(hDC, 150, 0, NULL);

	// Initialized 1 bpp
	const WORD Chess44_WORD [] = { 0xCC, 0xCC, 0x33, 0x33, 0xCC, 0xCC, 0x33, 0x33 };
	
	TestDDB88(hDC, 300, 20, CreateBitmap(8, 8, 1, 1, Chess44_WORD), _T("CreateBitmap(8, 8, 1, 1, Chess44_WORD)"));

	// Uninitialized 1 bpp
	TestDDB88(hDC, 300, 70, CreateBitmap(8, 8, 1, 1, NULL), _T("CreateBitmap(8, 8, 1, 1, NULL)"));

	// CreateBitmapIndirect, 1 bpp, DWORD aligned pixel array
	DWORD Chess44_DWORD [] = { 0xCC, 0xCC, 0x33, 0x33, 0xCC, 0xCC, 0x33, 0x33 };
	
	BITMAP bbp = { 0, 8, 8, sizeof(DWORD), 1, 1, Chess44_DWORD };

	TestDDB88(hDC, 300, 120, CreateBitmapIndirect(&bbp), _T("CreateBitmapIndirect({0,8,8,sizeof(DWORD),1,1,Chess44_DWORD})"));

	// CreateCompatibleBitmap: Mem DC
	HDC hMemDC = CreateCompatibleDC(hDC);
	TestDDB88(hDC, 300, 170, CreateCompatibleBitmap(hMemDC, 8, 8), _T("CreateCompatibleBitmap(hMemDC, 8, 8)"));
	DeleteObject(hMemDC);

	SetViewportOrgEx(hDC, 0, 0, NULL);
}
	

void KDDBView::Test_LargestDDB(HDC hDC, const RECT * rcPaint)
{
	HDC hMemDC = CreateCompatibleDC(hDC);
		
	// Calculate largest DDB size
	TCHAR mess[128];
	HDC DC[2] = { hMemDC, hDC };

	for (int y=0; y<2; y++)
	{
		const TCHAR * desp[] = { "Largest monochrome",
		"Largest screen compatible bitmap" };

		HBITMAP hBmp = LargestDDB(DC[y]);

		TextOut(hDC, 20, 10+y*20, desp[y], _tcslen(desp[y]));

		DecodeDDB(hBmp, mess);
		TextOut(hDC, 280, 10+y*20, mess, _tcslen(mess));

		DeleteObject(hBmp);
	}

	// test maximum number of DDB that can be created
	for (y=0; y<2; y++)
	{
		HBITMAP hBmp[1024];

		for (int x=0; x<1024; x++)
		{
			hBmp[x] = CreateCompatibleBitmap(DC[y], GetSystemMetrics(SM_CXSCREEN), 
				GetSystemMetrics(SM_CYSCREEN));
			if ( hBmp[x]==NULL )
				break;
		}

		wsprintf(mess, "Maximum number of DDB: %d, each of them is ", x);
		TextOut(hDC,  20, 60 + y*20, mess, _tcslen(mess));

		DecodeDDB(hBmp[0], mess);
		TextOut(hDC, 340, 60 + y*20, mess, _tcslen(mess));

		x--;
		while ( x>=0 )
		{
			DeleteObject(hBmp[x]);
			x --;
		}
	}

	DeleteObject(hMemDC);
}


void KDDBView::Test_LoadBitmap(HDC hDC, const RECT * rcPaint)
{
	TCHAR mess[128];

	HDC hMemDC = CreateCompatibleDC(hDC);

	// load difference BITMAP as resource
	HPALETTE hPal = CreateHalftonePalette(hDC);
	HPALETTE hOld = SelectPalette(hDC, hPal, FALSE);
	RealizePalette(hDC);

	for (int i=0; i<6; i++)
	{
		int x = 10 + (i%3) * 320;
		int y = 10 + (i/3) * 320;

		KDIB dib;

		dib.LoadBitmap(m_hInst, MAKEINTRESOURCE(i+IDB_BITMAP1));

		HBITMAP hBmp;

		if ( (m_nViewOpt==IDM_VIEW_LOADBITMAP) || (m_nViewOpt==IDM_VIEW_LOADBITMAPHEX) )
			hBmp = LoadBitmap(m_hInst, MAKEINTRESOURCE(i+IDB_BITMAP1));
		else
			hBmp = dib.ConvertToDDB(hDC);

		KGDIObject bmp(hMemDC, hBmp);

		if ( hBmp==NULL ) break;

		BITMAP info;
		GetObject(hBmp, sizeof(BITMAP), & info);

		BitBlt(hDC, x, y, info.bmWidth, info.bmHeight, hMemDC, 0, 0, SRCCOPY);

		dib.DecodeDIBFormat(mess);
		TextOut(hDC, x, y + info.bmHeight+20, mess, _tcslen(mess));
		
		DecodeDDB(bmp.m_hObj, mess);
		TextOut(hDC, x, y + info.bmHeight+40, mess, _tcslen(mess));

		int size = GetBitmapBits(hBmp, 0, NULL);
		wsprintf(mess, "DDB %d bytes", size);
		TextOut(hDC, x, y+ info.bmHeight+60, mess, _tcslen(mess));

		if ( (m_nViewOpt==IDM_VIEW_LOADBITMAPHEX) || (m_nViewOpt==IDM_VIEW_CREATEDIBITMAPHEX) )
			SendMessage(GetParent(GetParent(m_hWnd)), WM_USER+1, (WPARAM) hBmp, 1);
	}
	SelectPalette(hDC, hOld, FALSE);
	RealizePalette(hDC);
	DeleteObject(hPal);

	DeleteObject(hMemDC);
}


void KDDBView::Test_Blt(HDC hDC, const RECT * rcPaint)
{
	KDDB ddb;

	int nSeq;

	m_texture.GetChecked(nSeq);

	ddb.LoadBitmap(m_hInst, TextureID[nSeq]);

	RECT rect;

	GetClientRect(m_hWnd, & rect);
	int cwidth = rect.right;
	int cheight = rect.bottom;

	switch ( m_nViewOpt )
	{
		case IDM_VIEW_BLT_NORMAL:
			ddb.Draw(hDC, 0, 0, cwidth, cheight, SRCCOPY, KDDB::draw_normal);
			break;

		case IDM_VIEW_BLT_CENTER:
			ddb.Draw(hDC, 0, 0, cwidth, cheight, SRCCOPY, KDDB::draw_center);
			break;

		case IDM_VIEW_BLT_STRETCH:
			ddb.Draw(hDC, 0, 0, cwidth, cheight, SRCCOPY, KDDB::draw_stretch);
			break;

		case IDM_VIEW_BLT_TILE:
			ddb.Draw(hDC, 0, 0, cwidth, cheight, SRCCOPY, KDDB::draw_tile);
			break;

		case IDM_VIEW_BLT_STRETCHPROP:
			ddb.Draw(hDC, 0, 0, cwidth, cheight, SRCCOPY, KDDB::draw_stretchprop);
	}
}


void KDDBView::Test_Blt_Color(HDC hDC, const RECT * rcPaint)
{
	KDDB ddb;

	ddb.LoadBitmap(m_hInst, IDB_LION);
	RECT rect;

	GetClientRect(m_hWnd, & rect);
	int cwidth = rect.right;
	int cheight = rect.bottom;

	BITMAP bmp;
	GetObject(ddb.GetBitmap(), sizeof(bmp), & bmp);
	
	HDC hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, ddb.GetBitmap());

	COLORREF ColorTable[] = { 
		RGB(0xFF, 0, 0),    RGB(0, 0xFF, 0),    RGB(0, 0, 0xFF),
		RGB(0xFF, 0xFF, 0), RGB(0, 0xFF, 0xFF), RGB(0xFF, 0, 0xFF) 
	};

	for (int j=0; j<cheight; j+= bmp.bmHeight )
	for (int i=0; i<cwidth;  i+= bmp.bmWidth  )
	{
		SetTextColor(hDC, ColorTable[j/bmp.bmHeight]);
		SetBkColor(hDC,   ColorTable[i/bmp.bmWidth] | RGB(0xC0, 0xC0, 0xC0));

		BitBlt(hDC, i, j, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);
	}

	DeleteObject(hMemDC);
}


void KDDBView::Test_Blt_GenMask(HDC hDC, const RECT * rcPaint)
{
	KDDB ddb;

	KGDIObject brush(hDC, CreateSolidBrush(RGB(0xFF, 0xFF, 0)));

	
	ddb.LoadBitmap(m_hInst, IDB_CUBE);
	BITMAP bmp;
	GetObject(ddb.GetBitmap(), sizeof(bmp), & bmp); 

	PatBlt(hDC, 0, 0, bmp.bmWidth * 5 + 70, bmp.bmHeight * 2 + 30, PATCOPY);

	ddb.Draw(hDC, 10, 10, 0, 0, SRCCOPY, KDDB::draw_normal);

	COLORREF ct[] = {
		RGB(128, 128, 128), RGB(192, 192, 192), RGB(255, 255, 255),
		RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255),
		RGB(255, 255, 0), RGB(0, 255, 255), RGB(255, 0, 255)
	};

	HDC hMemDC = CreateCompatibleDC(hDC);

	for (int i=0; i<sizeof(ct)/sizeof(COLORREF); i++)
	{
		HBITMAP hOld = ddb.CreateMask(ct[i], hMemDC);

		SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));
		SetTextColor(hDC, RGB(0, 0, 0));

		BitBlt(hDC, ((i+1)%5)* (bmp.bmWidth+10)+10, ((i+1)/5)* (bmp.bmHeight+10)+10, bmp.bmWidth, bmp.bmHeight,
			hMemDC, 0, 0, SRCCOPY);

		DeleteObject(SelectObject(hMemDC, hOld));
	}

	DeleteObject(hMemDC);
}


void KDDBView::OnDraw(HDC hDC, const RECT * rcPaint)
{
	switch ( m_nViewOpt )
	{
		case IDM_VIEW_CREATEBITMAP:
			Test_CreateBitmap(hDC, rcPaint);
			break;

		case IDM_VIEW_LARGESTDDB:
			Test_LargestDDB(hDC, rcPaint);
			break;

		case IDM_VIEW_LOADBITMAP:
		case IDM_VIEW_CREATEDIBITMAP:
			Test_LoadBitmap(hDC, rcPaint);
			break;

		case IDM_VIEW_LOADBITMAPHEX:
			Test_LoadBitmap(hDC, rcPaint);
			m_nViewOpt = IDM_VIEW_LOADBITMAP;
			break;

		case IDM_VIEW_CREATEDIBITMAPHEX:
			Test_LoadBitmap(hDC, rcPaint);
			m_nViewOpt = IDM_VIEW_CREATEDIBITMAP;
			break;

		case IDM_VIEW_BLT_NORMAL:
		case IDM_VIEW_BLT_CENTER:
		case IDM_VIEW_BLT_STRETCH:
		case IDM_VIEW_BLT_TILE:
		case IDM_VIEW_BLT_STRETCHPROP:
			Test_Blt(hDC, rcPaint);
			break;

		case IDM_VIEW_BITBLT_COLOR:
			Test_Blt_Color(hDC, rcPaint);
			break;

		case IDM_VIEW_GENMASK:
			Test_Blt_GenMask(hDC, rcPaint);
			break;
	}
}

//////////////////////
//    DIB Section   //
//////////////////////

class KDIBSectionView : public KScrollCanvas
{
	typedef enum { GAP = 16 };

	virtual void    OnDraw(HDC hDC, const RECT * rcPaint);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void			GetWndClassEx(WNDCLASSEX & wc);
	
	HMENU			m_hViewMenu;
	int				m_nViewOpt;

public:

	KDIBSectionView(void)
	{
		m_hViewMenu = NULL;
		m_nViewOpt  = IDM_VIEW_CREATEBITMAP;
	}

	bool Initialize(HINSTANCE hInstance, KStatusWindow * pStatus)
	{
		m_hInst   = hInstance;
		m_pStatus = pStatus;

		RegisterClass(_T("DIBSectionView"), hInstance);
			
		return true;
	}

	void Test_CreateDIBSection(HDC hDC, const RECT * rcPaint);
};


void KDIBSectionView::Test_CreateDIBSection(HDC hDC, const RECT * rcPaint)
{
	TextOut(hDC, 10, 10, "CreateDIBSection", 10);

	TCHAR temp[128];
	KDIBSection dibsection[11];

	for (int i=0; i<11; i++)
	{
		const int static bitcount   [] = { 0,      1,      4,       4,      8,       8,      16,     16,           24,     32    , 32};
		const int static compression[] = { BI_PNG, BI_RGB, BI_RLE4, BI_RGB, BI_RLE8, BI_RGB, BI_RGB, BI_BITFIELDS, BI_RGB, BI_RGB, BI_BITFIELDS };

		KBitmapInfo bmi;

		bmi.SetFormat(125, 125, bitcount[i], compression[i]);

		dibsection[i].CreateDIBSection(hDC, bmi.GetBMI(), DIB_RGB_COLORS, NULL, NULL);
		
		dibsection[i].DecodeDIBSectionFormat(temp);
		TextOut(hDC, 20, 40 + i*20, temp, _tcslen(temp));

		if ( dibsection[i].GetBitmap() )
		{
			HBITMAP hBig = LargestDIBSection(bmi.GetBMI());

			DecodeDDB(hBig, temp);
			TextOut(hDC, 720, 40 + i*20, temp, _tcslen(temp));

			DeleteObject(hBig);
		}
	}
}


class KRipDialog : public KDialog
{
	int m_width;
	int m_height;

    virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
    	switch (uMsg)
	    {
		    case WM_INITDIALOG:
				{
					for (int i=1; i<10; i++)
					{
						TCHAR mess[64];

						wsprintf(mess, "%d%c % %d x %d pixels, %d Kb", i*100, '%', i*m_width, i*m_height,
							(m_width*i*3+3)/4*4*m_height*i/1024);
						SendDlgItemMessage(hWnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM) mess);
						SendDlgItemMessage(hWnd, IDC_LIST, LB_SETCURSEL, 0, 0);
					}
				}
				return TRUE;
	
		    case WM_COMMAND:
				if ( LOWORD(wParam)==IDOK )
				{
					EndDialog(hWnd, 1 + SendDlgItemMessage(hWnd, IDC_LIST, LB_GETCURSEL, 0, 0));
					return TRUE;
				}
				else if ( LOWORD(wParam)==IDCANCEL )
				{
					EndDialog(hWnd, 0);
					return TRUE;
				}
	    }

	    return FALSE;
    }

public:
	KRipDialog(int width, int height)
	{
		m_width  = width;
		m_height = height;
	}
};


BOOL RenderEMF(HINSTANCE hInstance)
{
	KFileDialog fd;

	if ( ! fd.GetOpenFileName(NULL, "emf", "Enhanced Metafiles") )
		return FALSE;

	HENHMETAFILE hemf = GetEnhMetaFile(fd.m_TitleName);

	if ( hemf==NULL )
		return FALSE;

	if ( ! fd.GetSaveFileName(NULL, "tga", "Targa 24 bit Image") )
	{
		DeleteEnhMetaFile(hemf);
		return FALSE;
	}

	ENHMETAHEADER emfheader;

	GetEnhMetaFileHeader(hemf, sizeof(ENHMETAHEADER), & emfheader);

	int width  = emfheader.rclBounds.right  - emfheader.rclBounds.left;
	int height = emfheader.rclBounds.bottom - emfheader.rclBounds.top;

	KRipDialog sizedialog(width, height);

	int scale = sizedialog.Dialogbox(hInstance, MAKEINTRESOURCE(IDD_RIP), NULL);
	BOOL rslt;

	if ( scale )
		rslt = RenderEMF(hemf, width * scale, height * scale, fd.m_TitleName);
	else
		rslt = FALSE;

	DeleteEnhMetaFile(hemf);

	return rslt;
}


void KDIBSectionView::GetWndClassEx(WNDCLASSEX & wc)
{
	memset(& wc, 0, sizeof(wc));

	wc.cbSize         = sizeof(WNDCLASSEX);
	wc.style          = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc    = WindowProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;       
	wc.hInstance      = NULL;
	wc.hIcon          = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_IMAGE));
	wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground  = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName   = NULL;
	wc.lpszClassName  = NULL;
	wc.hIconSm        = NULL;
}


LRESULT KDIBSectionView::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd		= hWnd;
			m_hViewMenu = LoadMenu(m_hInst, MAKEINTRESOURCE(IDR_DIBSECTIONVIEW));
			return 0;

		case WM_PAINT:
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

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDM_VIEW_CREATEDIBSECTION:
					if ( LOWORD(wParam)!= m_nViewOpt )
					{
						m_nViewOpt = LOWORD(wParam);

						InvalidateRect(hWnd, NULL, TRUE);
					}
					return 0;

				case IDM_VIEW_RIP:
					RenderEMF(m_hInst);
					return 0;
			}
	}

	return CommonMDIChildProc(hWnd, uMsg, wParam, lParam, m_hViewMenu, 3);
}


void KDIBSectionView::OnDraw(HDC hDC, const RECT * rcPaint)
{
	switch ( m_nViewOpt )
	{
		case IDM_VIEW_CREATEBITMAP:
			Test_CreateDIBSection(hDC, rcPaint);
			break;
	}
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
	int				m_nCount;
	KDDBBackground  m_background;

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

		SendMessage(m_hMDIClient, WM_MDICREATE, 0, (LPARAM) & mdic);
	}

	void CreateDIBView(const TCHAR * pFileName)
	{
		KDIBView * pDIBView = new KDIBView();

		if ( pDIBView )
			if ( pDIBView->Initialize(pFileName, m_hInst, m_pStatus) )
				CreateMDIChild(pDIBView, _T("DIBView"), pFileName, _T("DIB %d"));
			else
				delete pDIBView;
	}
	
	void CreateDDBView(void)
	{
		KDDBView * pDDBView = new KDDBView();

		if ( pDDBView )
			if ( pDDBView->Initialize(m_hInst, m_pStatus) )
				CreateMDIChild(pDDBView, _T("DDBView"), NULL, _T("DDB %d"));
			else
				delete pDDBView;
	}

	void CreateDIBSectionView(void)
	{
		KDIBSectionView * pDIBSectionView = new KDIBSectionView();

		if ( pDIBSectionView )
			if ( pDIBSectionView->Initialize(m_hInst, m_pStatus) )
				CreateMDIChild(pDIBSectionView, _T("DIBSectionView"), NULL, _T("DIBSection %d"));
			else
				delete pDIBSectionView;
	}

	void CreateHexView(WPARAM wParam, LPARAM lParam)
	{
		KEditView * pEditView = new KEditView();

		if ( pEditView )
			if ( pEditView->Initialize(m_hInst) )
			{
				CreateMDIChild(pEditView, _T("EditView"), NULL, _T("Hex Dump %d"));
				pEditView->Decode(wParam, lParam);
			}
			else
				delete pEditView;
	}

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam)
	{
		switch ( LOWORD(wParam) )
		{
			case IDM_FILE_NEW:
				CreateDIBView(NULL);
				return TRUE;

			case IDM_FILE_NEWDDB:
				CreateDDBView();
				return TRUE;

			case IDM_FILE_NEW_DIBSECTION:
				CreateDIBSectionView();
				return TRUE;

			case IDM_FILE_OPEN:
			{
				KFileDialog fo;

				if ( fo.GetOpenFileName(m_hWnd, "bmp", "Bitmap Files") )
					CreateDIBView(fo.m_TitleName);
				
				return TRUE;
			}
		}

		return FALSE; // not processed
	}

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if ( uMsg==WM_USER + 1)
		{
			CreateHexView(wParam, lParam);
			return 0;
		}
		
		LRESULT lr = KMDIFrame::WndProc(hWnd, uMsg, wParam, lParam);

		if ( uMsg == WM_CREATE )
		{
			m_background.SetBitmap(m_hInst, IDB_PUZZLE);
			m_background.Attach(m_hMDIClient);
		}

		return lr;
	}

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KFrame::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_IMAGE));
	}

public:
	KMyMDIFRame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
		KToolbar * pToolbar, KStatusWindow * pStatus) :
		KMDIFrame(hInstance, pButtons, nCount, pToolbar, pStatus, Translate)
	{
		m_nCount = 0;
	}
};


const TBBUTTON tbButtons[] =
{
	{ STD_FILENEW,	 IDM_FILE_NEW,   TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_FILENEW,  0 },
	{ STD_FILEOPEN,  IDM_FILE_OPEN,  TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_FILEOPEN, 0 }
};


class KMyDialog : public KDialog
{
	KDDBBackground whole;
	KDDBBackground groupbox;
	KDDBBackground frame;
	KDDBBackground button;

	HINSTANCE      m_hInstance;

    virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
    	switch (uMsg)
	    {
		    case WM_INITDIALOG:
				m_hWnd = hWnd;
				
				whole.SetBitmap(m_hInstance, IDB_PAPER01);
				whole.SetStyle(KDDB::draw_tile);
				whole.Attach(hWnd);
				
				groupbox.SetBitmap(m_hInstance, IDB_BRICK02);
				groupbox.SetStyle(KDDB::draw_center);
				groupbox.Attach(GetDlgItem(hWnd, IDC_GROUPBOX));

				frame.SetBitmap(m_hInstance, IDB_BRICK02);
				frame.SetStyle(KDDB::draw_stretchprop);
				frame.Attach(GetDlgItem(hWnd, IDC_FRAME)); 
				return TRUE;
	
		    case WM_COMMAND:
				if ( LOWORD(wParam)==IDOK )
				{
					EndDialog(hWnd, 1);
					return TRUE;
				}
	    }

	    return FALSE;
    }

public:
	KMyDialog(HINSTANCE hInst)
	{
		m_hInstance = hInst;
	}
};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KToolbar      toolbar;
	KStatusWindow status;
	KMyDialog	  testdialog(hInst);

	testdialog.Dialogbox(hInst, MAKEINTRESOURCE(IDD_BACKGROUND), NULL);

	KMyMDIFRame frame(hInst, tbButtons, 2, & toolbar, & status);

	TCHAR title[MAX_PATH];

	HDC hDC = GetDC(NULL);
	wsprintf(title, "DIBShop (%s)", PixelFormatName(PixelFormat(hDC)));
	ReleaseDC(NULL, hDC);

	frame.CreateEx(0, _T("ClassName"), title,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);
	

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}
