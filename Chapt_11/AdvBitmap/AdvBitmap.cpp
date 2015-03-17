//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : advbitmap.cpp					                                     //
//  Description: Advanced bitmap demo program, Chapter 11                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define NOCRYPT

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "..\..\include\wingraph.h"
#include "Resource.h"


class KDIBView : public KScrollCanvas
{
	typedef enum { GAP = 16 };

	virtual void    OnDraw(HDC hDC, const RECT * rcPaint);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void			GetWndClassEx(WNDCLASSEX & wc);
	
	HMENU			m_hViewMenu;
	int				m_nViewOpt;
	HWND			m_hFrame;

public:

	HINSTANCE		m_hInst;
	KDIB			m_DIB;

	KDIBView(void)
	{
		m_hViewMenu = NULL;
		m_nViewOpt  = IDM_VIEW_STRETCHDIBITS;
	}

	bool Initialize(const TCHAR * pFileName, HINSTANCE hInstance, KStatusWindow * pStatus, HWND hFrame)
	{
		m_hFrame = hFrame;

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

	bool Initialize(BITMAPINFO * pDIB, HINSTANCE hInstance, KStatusWindow * pStatus, HWND hFrame)
	{
		m_hFrame = hFrame;

		if ( m_DIB.AttachDIB(pDIB, NULL, DIB_BMI_NEEDFREE) )
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
	wc.hIcon          = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_IMAGE));
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
				case IDM_VIEW_OVALMASK:
				case IDM_VIEW_STRETCHDIBCLIP:
				case IDM_VIEW_STRETCHBLTOVAL:
				case IDM_VIEW_STRETCHDIBITS:
				case IDM_VIEW_STRETCHDIBITS4:
				case IDM_VIEW_SETDIBITSTODEVICE:
				case IDM_VIEW_FITWINDOW:

				case IDM_VIEW_MASKRED:
				case IDM_VIEW_MASKGREEN:
				case IDM_VIEW_MASKBLUE:
				case IDM_VIEW_FADEIN:					
				case IDM_VIEW_ALPHAFADE:
				case IDM_VIEW_CUBE:
				case IDM_VIEW_CUBEPIXEL:
				case IDM_VIEW_SIMUPLGBLT:
					if ( LOWORD(wParam)!= m_nViewOpt )
					{
						m_nViewOpt = LOWORD(wParam);

						switch ( LOWORD(wParam) )
						{
							case IDM_VIEW_STRETCHDIBCLIP:
							case IDM_VIEW_OVALMASK:
							case IDM_VIEW_STRETCHBLTOVAL:
							case IDM_VIEW_STRETCHDIBITS:
							case IDM_VIEW_SETDIBITSTODEVICE:
							case IDM_VIEW_FADEIN:
							case IDM_VIEW_ALPHAFADE:
								SetSize(m_DIB.GetWidth() + GAP*2, m_DIB.GetHeight() + GAP*2, 5, 5, true);
								break;

							case IDM_VIEW_CUBE:
							case IDM_VIEW_CUBEPIXEL:
							case IDM_VIEW_SIMUPLGBLT:
							case IDM_VIEW_STRETCHDIBITS4:
								SetSize(m_DIB.GetWidth()*2 + GAP*3, m_DIB.GetHeight()*2 + GAP*3, 5, 5, true);
								break;
						}

						InvalidateRect(hWnd, NULL, TRUE);
					}
					return 0;

				case IDM_VIEW_DIBHEXDUMP:
					SendMessage(GetParent(GetParent(hWnd)), WM_USER+1, (WPARAM) & m_DIB, 0);	
			}
			return 0;

		default:
			return CommonMDIChildProc(hWnd, uMsg, wParam, lParam, m_hViewMenu, 3);
	}
}


void TransparentBltDrawIcon(HDC hDC, int x, int y, HICON hIcon)
{
	ICONINFO iconinfo;
	GetIconInfo(hIcon, & iconinfo);

	BITMAP bmp;
	GetObject(iconinfo.hbmMask, sizeof(bmp), & bmp);

	HDC hMemDC = CreateCompatibleDC(NULL);
	HGDIOBJ hOld = SelectObject(hMemDC, iconinfo.hbmColor);

	COLORREF crTrans = GetPixel(hMemDC, 0, 0);

	G_TransparentBlt(hDC,    x, y, bmp.bmWidth, bmp.bmHeight, 
		           hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight,
		           crTrans);

	SelectObject(hMemDC, hOld);
	DeleteObject(iconinfo.hbmMask);
	DeleteObject(iconinfo.hbmColor);
	DeleteObject(hMemDC);
}


void TestPlgBlt(HDC hDC, int x, int y, int w, int h, const BITMAPINFO * pBMI, const void * pBits, HINSTANCE hInstance, bool bSimulate)
{
	HBITMAP hBmp   = CreateDIBSection(hDC, pBMI, DIB_RGB_COLORS, NULL, NULL, NULL);
	HDC     hMemDC = CreateCompatibleDC(hDC);

	SelectObject(hMemDC, hBmp);
	StretchDIBits(hMemDC, 0, 0, w, h, 0, 0, w, h, pBits, pBMI, DIB_RGB_COLORS, SRCCOPY);
	
	HBITMAP hBack = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_WOOD01));
	BITMAP bmp;
	GetObject(hBack, sizeof(bmp), & bmp);
	MaskCube(hDC, 200, x, y, bmp.bmWidth, bmp.bmHeight, hBack, hMemDC, false, bSimulate);
	DeleteObject(hBack);

	MaskCube(hDC, 200, x, y, w, h, hBmp, hMemDC, true, bSimulate);

	DeleteObject(hMemDC);
	DeleteObject(hBmp);
}


void TestDIBTransform(HDC hDC, int x, int y, int w, int h, KDIB * pSrc)
{
	KAffine affine;

	affine.Rotate(-15);

	HBITMAP hBitmap = pSrc->TransformBitmap(& affine.m_xm, RGB(0xFF, 0xFF, 0), KDIB::method_direct);

	HDC hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, hBitmap);
	
	BITMAP bmp;

	GetObject(hBitmap, sizeof(BITMAP), & bmp);
	BitBlt(hDC, x, y, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);
	DeleteObject(hMemDC);
	DeleteObject(hBitmap);
}


BOOL OvalStretchDIBits(HDC hDC, int XDest, int YDest, int nDestWidth, int nDestHeight, 
			  int XSrc, int YSrc, int nSrcWidth, int nSrcHeight,
			  const void *pBits, const BITMAPINFO *pBMI, UINT iUsage)
{
	StretchDIBits(hDC, XDest, YDest, nDestWidth, nDestHeight, XSrc, YSrc, 
		nSrcWidth, nSrcHeight, pBits, pBMI, iUsage, SRCINVERT);

	SaveDC(hDC);
	SelectObject(hDC, GetStockObject(BLACK_BRUSH));
	SelectObject(hDC, GetStockObject(BLACK_PEN));
	Ellipse(hDC, XDest, YDest, XDest + nDestWidth, YDest + nDestHeight);
	RestoreDC(hDC, -1);

	return StretchDIBits(hDC, XDest, YDest, nDestWidth, nDestHeight, XSrc, YSrc, 
		nSrcWidth, nSrcHeight, pBits, pBMI, iUsage, SRCINVERT);
}


BOOL ClipOvalStretchDIBits(HDC hDC, int XDest, int YDest, int nDestWidth, int nDestHeight, 
			  int XSrc, int YSrc, int nSrcWidth, int nSrcHeight,
			  const void *pBits, const BITMAPINFO *pBMI, UINT iUsage)
{
	RECT rect = { XDest, YDest, XDest + nDestWidth, YDest + nDestHeight };
	LPtoDP(hDC, (POINT *) & rect, 2);

	HRGN hRgn = CreateEllipticRgnIndirect(& rect);

	SaveDC(hDC);

	SelectClipRgn(hDC, hRgn);
	DeleteObject(hRgn);

	BOOL rslt = StretchDIBits(hDC, XDest, YDest, nDestWidth, nDestHeight, XSrc, YSrc, 
		nSrcWidth, nSrcHeight, pBits, pBMI, iUsage, SRCCOPY);
	
	RestoreDC(hDC, -1);

	return rslt;
}


BOOL AlphaFade(HDC hDCDst, int XDst, int YDst, int nDstWidth, int nDstHeight,
			   HDC hDCSrc, int XSrc, int YSrc, int nSrcWidth, int nSrcHeight)
{
	for (int i=5; i>=1; i--)
	{
		// 1/5, 1/4, 1/3, 1/2, 1/1
		BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255 / i , 0 };

		AlphaBlend(hDCDst, XDst, YDst, nDstWidth, nDstHeight,
			       hDCSrc, YSrc, YSrc, nSrcWidth, nSrcHeight,
				   blend);
	}

	return TRUE;
}


BOOL OvalStretchBlt(HDC hDC,    int XDest, int YDest, int nDestWidth, int nDestHeight, 
				    HDC hDCSrc, int XSrc,  int YSrc,  int nSrcWidth,  int nSrcHeight)
{
	// Make the region outside the ellipse be BLACK in source surface
	SaveDC(hDCSrc);
	BeginPath(hDCSrc);
	Rectangle(hDCSrc, XSrc, YSrc, XSrc + nSrcWidth+1, YSrc + nSrcHeight+1);
	Ellipse(hDCSrc, XSrc, YSrc, XSrc + nSrcWidth, YSrc + nSrcHeight);
	EndPath(hDCSrc);
	SelectObject(hDCSrc, GetStockObject(BLACK_BRUSH));
	SelectObject(hDCSrc, GetStockObject(BLACK_PEN));
	FillPath(hDCSrc);
	RestoreDC(hDCSrc, -1);
	
	// Draw a BLACK ellipse in destination surface
	SaveDC(hDC);
	SelectObject(hDC, GetStockObject(BLACK_BRUSH));
	SelectObject(hDC, GetStockObject(BLACK_PEN));
	Ellipse(hDC, XDest, YDest, XDest + nDestWidth, YDest + nDestHeight);
	RestoreDC(hDC, -1);

	// Merge source surface to destination
	return StretchBlt(hDC, XDest, YDest, nDestWidth, nDestHeight, hDCSrc, XSrc, YSrc, 
		nSrcWidth, nSrcHeight, SRCPAINT);
}


extern DWORD dibtick;


void KDIBView::OnDraw(HDC hDC, const RECT * rcPaint)
{
	DWORD tm = GetTickCount();
	
	dibtick = 0;
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

		case IDM_VIEW_MASKRED:
			{
				HDC     hMemDC = CreateCompatibleDC(NULL);
				HBITMAP hBmp = ChannelSplit(m_DIB.GetBMI(), m_DIB.GetBits(), RGB(0xFF, 0, 0), hMemDC);

				BitBlt(hDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
				DeleteObject(hBmp);
				DeleteObject(hMemDC);

			//	CGDIObject red(hDC, CreateSolidBrush(RGB(0xFF, 0, 0)));
			//	m_DIB.DrawDIB(hDC, GAP,   GAP,    w, h, 0, 0,  w,  h, MERGECOPY);
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

		case IDM_VIEW_FADEIN:
			FadeIn(hDC, GAP, GAP, w, h, m_DIB.GetBMI(), m_DIB.GetBits());
			break;

		case IDM_VIEW_OVALMASK:
			OvalStretchDIBits(hDC, GAP, GAP, w, h, 0, 0, w, h, m_DIB.GetBits(), m_DIB.GetBMI(), DIB_RGB_COLORS);
			break;

		case IDM_VIEW_STRETCHDIBCLIP:
			ClipOvalStretchDIBits(hDC, GAP, GAP, w, h, 0, 0, w, h, m_DIB.GetBits(), m_DIB.GetBMI(), DIB_RGB_COLORS);
			break;

		case IDM_VIEW_ALPHAFADE:
			{
				HBITMAP hBmp = m_DIB.ConvertToDDB(hDC);
				HDC hMemDC   = CreateCompatibleDC(hDC);
				SelectObject(hMemDC, hBmp);

				AlphaFade(hDC, GAP, GAP, w, h, hMemDC, 0, 0, w, h);
				
				DeleteObject(hMemDC);
				DeleteObject(hBmp);
			}
			break;

		case IDM_VIEW_STRETCHBLTOVAL:
			{
				HBITMAP hBmp = m_DIB.ConvertToDDB(hDC);
				HDC hMemDC   = CreateCompatibleDC(hDC);
				SelectObject(hMemDC, hBmp);

				OvalStretchBlt(hDC, GAP, GAP, w, h, hMemDC, 0, 0, w, h);
				
				DeleteObject(hMemDC);
				DeleteObject(hBmp);
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

		case IDM_VIEW_CUBE:
			TestPlgBlt(hDC, GAP, GAP, w, h, m_DIB.GetBMI(), m_DIB.GetBits(), m_hInst, false);
			break;

		case IDM_VIEW_SIMUPLGBLT:
			TestPlgBlt(hDC, GAP, GAP, w, h, m_DIB.GetBMI(), m_DIB.GetBits(), m_hInst, true);
			break;

		case IDM_VIEW_CUBEPIXEL:
			TestDIBTransform(hDC, GAP, GAP, w, h, & m_DIB);
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

//////////////////////////////////////////////

void TestRop3(HINSTANCE hInstance, HDC hDC);

void TestIcons(HDC hDC)
{
	static HINSTANCE hMod = NULL;
	
	if ( hMod==NULL )
		hMod = LoadLibrary("Shell32.dll");

	int n = 0;

	HDC hMemDC = CreateCompatibleDC(hDC);

	for (int resid=3; resid<=100; resid++)
	{
		HICON hIcon = (HICON) LoadImage(hMod, MAKEINTRESOURCE(resid), IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR);

		if ( hIcon )
		{
			int x = (n%2)*420 + 20;
			int y = (n/2)*60  + 20;

			DrawIcon(hDC, x, y, hIcon); n++;
			
			MaskBltDrawIcon(hDC, x+56*5, y, hIcon);
			TransparentBltDrawIcon(hDC, x+56*6, y, hIcon);
			
			ICONINFO iconinfo;
			GetIconInfo(hIcon, & iconinfo);
			DestroyIcon(hIcon);

			BITMAP bmp;
			GetObject(iconinfo.hbmMask, sizeof(bmp), & bmp);

			HGDIOBJ hOld = SelectObject(hMemDC, iconinfo.hbmMask);
			BitBlt(hDC, x+56, y, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);
			SelectObject(hMemDC, iconinfo.hbmColor);
			BitBlt(hDC, x+56*2, y, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

			SelectObject(hMemDC, iconinfo.hbmMask);
			BitBlt(hDC, x+56*3, y, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCAND);
			SelectObject(hMemDC, iconinfo.hbmColor);
			BitBlt(hDC, x+56*3, y, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCINVERT);
			
			MaskBitmapNT(hDC, x+56*4, y, bmp.bmWidth, bmp.bmHeight, iconinfo.hbmMask, hMemDC);

		
			SelectObject(hMemDC, hOld);
			DeleteObject(iconinfo.hbmMask);
			DeleteObject(iconinfo.hbmColor);
		}
	}

	DeleteObject(hMemDC);
}


void ColorBitmap(HDC hDC, int x, int y, int w, int h, HDC hMemDC, HBRUSH hBrush)
{
	// P^(S&(P^D)), if (S) D else P
	HGDIOBJ hOldBrush = SelectObject(hDC, hBrush);
	BitBlt(hDC, x, y, w, h, hMemDC, 0, 0, 0xB8074A);
	SelectObject(hDC, hOldBrush);
}


void TestColoring(HDC hDC, HINSTANCE hInstance)
{
	HBITMAP hPttrn;
	HBITMAP hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_CONFUSE));
	BITMAP  bmp;
	GetObject(hBitmap, sizeof(bmp), & bmp);

	SetTextColor(hDC, RGB(0, 0, 0));
	SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));

	HDC hMemDC   = CreateCompatibleDC(NULL);
	HGDIOBJ hOld = SelectObject(hMemDC, hBitmap);

	for (int i=0; i<5; i++)
	{
		HBRUSH hBrush;
		switch (i)
		{
			case 0: hBrush = CreateSolidBrush(RGB(0xFF, 0, 0)); break;
			case 1: hBrush = CreateSolidBrush(RGB(0, 0xFF, 0)); break;
			case 2: hPttrn = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_PATTERN01));
					hBrush = CreatePatternBrush(hPttrn);
					DeleteObject(hPttrn);
					break;
			case 3: hBrush = CreateHatchBrush(HS_DIAGCROSS, RGB(0, 0, 0xFF)); break;
			case 4: hPttrn = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_WOOD01));
					hBrush = CreatePatternBrush(hPttrn);
					DeleteObject(hPttrn);
		}

		ColorBitmap(hDC, i*30+10-2, i*5+10-2, bmp.bmWidth, bmp.bmHeight, hMemDC, (HBRUSH)GetStockObject(WHITE_BRUSH));
		ColorBitmap(hDC, i*30+10+2, i*5+10+2, bmp.bmWidth, bmp.bmHeight, hMemDC, (HBRUSH)GetStockObject(DKGRAY_BRUSH));
		ColorBitmap(hDC, i*30+10, i*5+10, bmp.bmWidth, bmp.bmHeight, hMemDC, hBrush);
		DeleteObject(hBrush);
	}

	BitBlt(hDC, 240, 25, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOld);
	DeleteObject(hBitmap);
	DeleteObject(hMemDC);
}



void TestLoadImage(HDC hDC, HINSTANCE hInstance)
{
	HBITMAP hBitmap[3];
	
	const nID [] = { IDB_MOSQUIT1, IDB_MOSQUIT2, IDB_MOSQUIT3 };

	for (int i=0; i<3; i++)
		hBitmap[i] = (HBITMAP) LoadImage(hInstance, MAKEINTRESOURCE(nID[i]), IMAGE_BITMAP, 0, 0,
							LR_LOADTRANSPARENT | LR_CREATEDIBSECTION );

	BITMAP bmp;
	GetObject(hBitmap[0], sizeof(bmp), & bmp);

	HDC hMemDC = CreateCompatibleDC(hDC);

	SelectObject(hDC, GetSysColorBrush(COLOR_WINDOW));

	int lastx = -1;
	int lasty = -1;

	HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
	for (i=0; i<600; i++)
	{
		SelectObject(hMemDC, hBitmap[i%3]); 
		
		int newx = i;
		int newy = abs(200-i%400);

		if ( lastx!=-1 )
		{
			SetRectRgn(hRgn, newx, newy, newx+bmp.bmWidth, newy + bmp.bmHeight);
			ExtSelectClipRgn(hDC, hRgn, RGN_DIFF);
			PatBlt(hDC, lastx, lasty, bmp.bmWidth, bmp.bmHeight, PATCOPY);
			SelectClipRgn(hDC, NULL);
		}
		BitBlt(hDC, newx, newy, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

		lastx = newx; lasty = newy;
	}
	DeleteObject(hRgn);
	DeleteObject(hMemDC);
	DeleteObject(hBitmap[0]);
	DeleteObject(hBitmap[1]);
	DeleteObject(hBitmap[2]);
}


void TestAlphaBlending(HDC hDC, HINSTANCE hInstance)
{
	const int size = 100;

	// Solid block constant alpha blending
	for (int i=0; i<3; i++)
	{
		RECT  rect    = { i*(size+10) + 20, 20+size/3, i*(size+10) + 20 + size, 20+size/3 + size };

		const COLORREF Color[] = { RGB(0xFF, 0, 0), RGB(0, 0xFF, 0), RGB(0, 0, 0xFF)  };
		
		HBRUSH hBrush = CreateSolidBrush(Color[i]);
		FillRect(hDC, & rect, hBrush);				// three original solid rectangle
		DeleteObject(hBrush);

		BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255/2, 0 }; // constant alpha 0.5

		AlphaBlend(hDC, 360+((3-i)%3)*size/3, 20+i*size/3, size, size, // blend destination
				   hDC, i*(size+10)+20, 20+size/3, size, size, blend);	// original solid rectangle
	}

//	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 1));
//	RECT  rect = { 100, 100, 500, 200 };
//	FillRect(hDC, & rect, hBrush);
//	DeleteObject(hDC);
}


void Label(HDC hDC, int x, int y, const TCHAR * mess)
{
	TextOut(hDC, x, y, mess, _tcslen(mess));
}


void TestPalette(HDC hDC, HINSTANCE hInstance)
{
	TCHAR temp[64];

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

	PALETTEENTRY entries[256];

	int no = GetSystemPaletteEntries(hDC, 0, 256, entries);
	for (int i=0; i<no; i++)
	{
		wsprintf(temp, "%02x %02x %02x %02x", entries[i].peRed, entries[i].peGreen, entries[i].peBlue, entries[i].peFlags);
		Label(hDC, 10 + ( i % 8) * 100, 100 + (i/8) * 20, temp);
	}
}


class KTestView : public KScrollCanvas
{
	KAirBrush brush;

	void AirBrush(HWND hWnd, int x, int y)
	{
		HDC hDC = GetDC(hWnd);

		brush.Apply(hDC, x, y);

		ReleaseDC(hWnd, hDC);
	}

	virtual void OnDraw(HDC hDC, const RECT * rcPaint)
	{
		switch ( m_nViewOpt )
		{
			case IDM_VIEW_ROPCHART:
				TestRop3(m_hInst, hDC);
				break;

			case IDM_VIEW_ICON:
				TestIcons(hDC);
				break;

			case IDM_VIEW_COLORBITMAP:
				TestColoring(hDC, m_hInst);
				break;

			case IDM_VIEW_LOADIMAGE:
				TestLoadImage(hDC, m_hInst);
				break;

			case IDM_VIEW_ALPHABLEND:
				TestAlphaBlending(hDC, m_hInst);
				break;

			case IDM_VIEW_PALETTE:
				TestPalette(hDC, m_hInst);
		}
	}

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch( uMsg )
		{
			case WM_CREATE:
				m_hWnd		= hWnd;
				m_hViewMenu = LoadMenu(m_hInst, MAKEINTRESOURCE(IDR_DIBVIEW));
				SetSize(1200, 1200, 5, 5, true);

				return 0;

			case WM_LBUTTONDOWN:
				AirBrush(hWnd, LOWORD(lParam), HIWORD(lParam));
				return 0;

			case WM_MOUSEMOVE:
				if ( wParam & MK_LBUTTON )
					AirBrush(hWnd, LOWORD(lParam), HIWORD(lParam));		
				else
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
					case IDM_VIEW_ROPCHART:
					case IDM_VIEW_ICON:
					case IDM_VIEW_COLORBITMAP:
					case IDM_VIEW_LOADIMAGE:
					case IDM_VIEW_ALPHABLEND:
					case IDM_VIEW_PALETTE:
						if ( LOWORD(wParam)!= m_nViewOpt )
						{
							m_nViewOpt = LOWORD(wParam);
							SetSize(700, 800, 5, 5, true);
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
		wc.hIcon          = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_IMAGE));
		wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground  = (HBRUSH) (COLOR_WINDOW + 1);
		wc.lpszMenuName   = NULL;
		wc.lpszClassName  = NULL;
		wc.hIconSm        = NULL;
	}


	HMENU			m_hViewMenu;
	int				m_nViewOpt;

public:

	KTestView(void)
	{
		m_hViewMenu = NULL;
		m_nViewOpt  = IDM_VIEW_ROPCHART;
	}

	bool Initialize(HINSTANCE hInstance, KStatusWindow * pStatus)
	{
		m_hInst   = hInstance;
		m_pStatus = pStatus;

		RegisterClass(_T("TestView"), hInstance);

		brush.Create(32, 32, RGB(0xC0, 0x80, 0));
		return true;
	}

};


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
		}

		return FALSE;
	}

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lr = KMDIFrame::WndProc(hWnd, uMsg, wParam, lParam);

		if ( uMsg == WM_USER )
		{
			CreateDIBView((BITMAPINFO *) wParam, (const TCHAR *) lParam);
		}

//		if ( uMsg == WM_CREATE )
//		{
//			SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED); 
//			SetLayeredWindowAttributes(m_hWnd, 0, 0xC0, LWA_ALPHA);
//		}

		return lr;
	}

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KMDIFrame::GetWndClassEx(wc);

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


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KToolbar      toolbar;
	KStatusWindow status;

	KMyMDIFRame frame(hInst, tbButtons, 2, & toolbar, & status);

	TCHAR title[MAX_PATH];

	_tcscpy(title, "AdvBitmap");

	frame.CreateEx(0, _T("ClassName"), title,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);
	
	frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}

