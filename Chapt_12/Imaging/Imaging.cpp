//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : imaging.cpp					                                     //
//  Description: Image Processing Demostration Program, Chapter 12                   //
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

	KImage			m_DIB;

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
	wc.hbrBackground  = (HBRUSH) GetStockObject(WHITE_BRUSH); // (COLOR_GRAYTEXT + 1);
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
				case IDM_COLOR_GREYSCALE:
					m_DIB.ToGreyScale();
					InvalidateRect(hWnd, NULL, TRUE);
					return 0;

				case IDM_COLOR_LIGHTEN:
					m_DIB.Lighten();
					InvalidateRect(hWnd, NULL, TRUE);
					return 0;

				case IDM_COLOR_GAMMA22:
					m_DIB.GammaCorrect(2.2, 2.2, 2.2);
					InvalidateRect(hWnd, NULL, TRUE);
					return 0;

				case IDM_COLOR_SPLITRGB:
					CreateNewView(m_DIB.SplitChannel(TakeRed),   "Red Channel");
					CreateNewView(m_DIB.SplitChannel(TakeGreen), "Green Channel");
					CreateNewView(m_DIB.SplitChannel(TakeBlue),  "Blue Channel");
					return 0;

				case IDM_COLOR_SPLITHLS:
					CreateNewView(m_DIB.SplitChannel(TakeH),  "Hue Channel");
					CreateNewView(m_DIB.SplitChannel(TakeL),  "Lightness Channel");
					CreateNewView(m_DIB.SplitChannel(TakeS),  "Saturation Channel");
					return 0;

				case IDM_COLOR_SPLITKCMY:
					CreateNewView(m_DIB.SplitChannel(TakeK),  "Black Channel");
					CreateNewView(m_DIB.SplitChannel(TakeC),  "Cyan Channel");
					CreateNewView(m_DIB.SplitChannel(TakeM),  "Magenta Channel");
					CreateNewView(m_DIB.SplitChannel(TakeY),  "Yellow Channel");
					return 0;

				case IDM_COLOR_SMOOTH:
					{
						m_DIB.Convolution(StockFilters[FILTER_SMOOTH]);
						InvalidateRect(hWnd, NULL, TRUE);
					}
					return 0;

				case IDM_COLOR_GUASIANSMOOTH:
					{
						m_DIB.Convolution(StockFilters[FILTER_GUASSIANSMOOTH]);
						InvalidateRect(hWnd, NULL, TRUE);
					}
					return 0;

				case IDM_COLOR_SHARPENING:
					{
						m_DIB.Convolution(StockFilters[FILTER_SHARPENING]);
						InvalidateRect(hWnd, NULL, TRUE);
					}
					return 0;

				case IDM_COLOR_LAPLASIAN:
					{
						m_DIB.Convolution(StockFilters[FILTER_LAPLASION]);
						InvalidateRect(hWnd, NULL, TRUE);
					}
					return 0;

				case IDM_COLOR_EMBOSS135:
					{
						m_DIB.Convolution(StockFilters[FILTER_EMBOSS135]);
						InvalidateRect(hWnd, NULL, TRUE);
					}
					return 0;

				case IDM_COLOR_EMBOSS90:
					{
						m_DIB.Convolution(StockFilters[FILTER_EMBOSS90]);
						InvalidateRect(hWnd, NULL, TRUE);
					}
					return 0;

				case IDM_VIEW_STRETCHDIBITS:
				case IDM_VIEW_MASKRED:
				case IDM_VIEW_ROTATEGDI:
				case IDM_VIEW_ROTATE:
				case IDM_VIEW_ROTATE24:
				case IDM_VIEW_FILTERS1:
				case IDM_VIEW_FILTERS2:
				case IDM_VIEW_FILTERS3:
					if ( LOWORD(wParam)!= m_nViewOpt )
					{
						m_nViewOpt = LOWORD(wParam);

						switch ( LOWORD(wParam) )
						{
							case IDM_VIEW_STRETCHDIBITS:
							case IDM_VIEW_MASKRED:
								SetSize(m_DIB.GetWidth() + GAP*2, m_DIB.GetHeight() + GAP*2, 5, 5, true);
								break;

							case IDM_VIEW_ROTATEGDI:
							case IDM_VIEW_ROTATE:
							case IDM_VIEW_ROTATE24:
								SetSize(m_DIB.GetWidth()*2 + GAP*3, m_DIB.GetHeight()*2 + GAP*3, 5, 5, true);
								break;

							case IDM_VIEW_FILTERS1:
							case IDM_VIEW_FILTERS2:
							case IDM_VIEW_FILTERS3:
								SetSize(m_DIB.GetWidth()*3 + GAP*2, m_DIB.GetHeight()*2 + GAP*3, 5, 5, true);
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


void TestDIBTransform(HDC hDC, int x, int y, int w, int h, KDIB * pSrc, int method)
{
	KAffine affine;

	affine.Rotate(-15);

	HBITMAP hBitmap = pSrc->TransformBitmap(& affine.m_xm, RGB(0xFF, 0xFF, 0), method);

	assert(hBitmap);
	HDC hMemDC = CreateCompatibleDC(hDC);
	SelectObject(hMemDC, hBitmap);
	
	BITMAP bmp;

	GetObject(hBitmap, sizeof(BITMAP), & bmp);
	BitBlt(hDC, x, y, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);
	DeleteObject(hMemDC);
	DeleteObject(hBitmap);
}


void TestFilters(HDC hDC, int x, int y, int w, int h, KImage & dib, int nFilterID[], int scale)
{
	BYTE * pTemp = new BYTE[dib.m_nImageSize];
	memcpy(pTemp, dib.GetBits(), dib.m_nImageSize);	//save pixel array

	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, 0); // RGB(0xFF, 0xFF, 0xFF));

	for (int i=0; i<4; i++)
	{
		int xx = x + (w*scale + 120) * ( i % 2);
		int yy = y + (h*scale + 10) * ( i / 2);

		int filter = nFilterID[i];

		if ( StockFilters[filter] )
		{
			dib.Convolution(StockFilters[filter]);

			StockFilters[filter]->DescribeFilter(hDC, xx + w*scale + 5, yy + 5);
		}

		StretchDIBits(hDC, xx, yy, 
			w*scale, h*scale, 0, 0, w, h, dib.GetBits(), dib.GetBMI(), DIB_RGB_COLORS, SRCCOPY);
		
		memcpy(dib.GetBits(), pTemp, dib.m_nImageSize); // restore pixel array
	}

	delete pTemp;
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
		case IDM_VIEW_MASKRED:
			{
				HDC     hMemDC = CreateCompatibleDC(NULL);
				HBITMAP hBmp = ChannelSplit(m_DIB.GetBMI(), m_DIB.GetBits(), RGB(0xFF, 0, 0), hMemDC);

				BitBlt(hDC, GAP, GAP, w, h, hMemDC, 0, 0, SRCCOPY);
				DeleteObject(hBmp);
				DeleteObject(hMemDC);
			}
			break;

		case IDM_VIEW_STRETCHDIBITS:
			m_DIB.DrawDIB(hDC, GAP,   GAP,    w, h, 0, 0,  w,  h, SRCCOPY);
			break;

		case IDM_VIEW_ROTATEGDI:
			TestDIBTransform(hDC, GAP, GAP, w, h, & m_DIB, KDIB::method_gdi);
			break;

		case IDM_VIEW_ROTATE:
			TestDIBTransform(hDC, GAP, GAP, w, h, & m_DIB, KDIB::method_direct);
			break;

		case IDM_VIEW_ROTATE24:
			TestDIBTransform(hDC, GAP, GAP, w, h, & m_DIB, KDIB::method_24bpp);
			break;

		case IDM_VIEW_FILTERS1:
			{
				int id[] = { 0, 1, 2, 3 };

				TestFilters(hDC, GAP, GAP, w, h, m_DIB, id, 3);
			}
			break;

		case IDM_VIEW_FILTERS2:
			{
				int id[] = { 0, 4, 5, 6 };

				TestFilters(hDC, GAP, GAP, w, h, m_DIB, id, 1);
			}
			break;

		case IDM_VIEW_FILTERS3:
			{
				int id[] = { 0, 7, 8, 9 };

				TestFilters(hDC, GAP, GAP, w, h, m_DIB, id, 2);
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


	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam)
	{
		switch ( LOWORD(wParam) )
		{
//			case IDM_FILE_NEW:
//				CreateDIBView(NULL);
//				return TRUE;

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
//	{ STD_FILENEW,	 IDM_FILE_NEW,   TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_FILENEW,  0 },
	{ STD_FILEOPEN,  IDM_FILE_OPEN,  TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_FILEOPEN, 0 }
};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KToolbar      toolbar;
	KStatusWindow status;

	KMyMDIFRame frame(hInst, tbButtons, 1, & toolbar, & status);

	TCHAR title[MAX_PATH];

	_tcscpy(title, "Imaging");

	frame.CreateEx(0, _T("ClassName"), title,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);
	
	frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}