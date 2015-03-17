//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dibwindow.h			                                             //
//  Description: DIB displaying window with palette support                          //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

typedef enum
{
	pal_no		 = 0x00,
	pal_halftone = 0x01, 
	pal_bitmap   = 0x02,
	
	pal_react    = 0x04,
	pal_stretchHT= 0x08
};

HPALETTE CreateDIBSectionPalette(HDC hDC, HBITMAP hDIBSec);


class KDIBWindow : public KWindow
{
	const BITMAPINFO * m_pBMI;
	const BYTE		 * m_pBits;
	HBITMAP			   m_hDIBSection;
	int				   m_nOption;
	HINSTANCE		   m_hInst;

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KWindow::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_PALETTE));
	}
	
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch ( uMsg )
		{
			case WM_CREATE:
				m_hWnd     = hWnd;
				{
					HDC hDC = GetDC(m_hWnd);

					if ( (m_nOption & 3)==pal_bitmap )
						if ( m_pBMI )
							m_hPalette = CreateDIBPalette(m_pBMI);
						else
							m_hPalette = CreateDIBSectionPalette(hDC, m_hDIBSection);
					else if ( (m_nOption & 3)==pal_halftone )
						m_hPalette = CreateHalftonePalette(hDC);
					else
						m_hPalette = NULL;
					ReleaseDC(m_hWnd, hDC);
				}
				return 0;

			case WM_PAINT:
				{
					PAINTSTRUCT ps;

					HDC hDC = BeginPaint(hWnd, & ps);
					HPALETTE hOld = SelectPalette(hDC, m_hPalette, FALSE);
					RealizePalette(hDC);

					if ( m_nOption & pal_stretchHT )
					{
						SetStretchBltMode(hDC, STRETCH_HALFTONE);
						TextOut(hDC, 0, 0, "halftone", 8);
					}
					else
						SetStretchBltMode(hDC, STRETCH_DELETESCANS);

					if ( m_pBMI )
						StretchDIBits(hDC, 10, 10, m_pBMI->bmiHeader.biWidth, m_pBMI->bmiHeader.biHeight,
							0, 0, m_pBMI->bmiHeader.biWidth, m_pBMI->bmiHeader.biHeight,
							m_pBits, m_pBMI, DIB_RGB_COLORS, SRCCOPY);
	
					if ( m_hDIBSection )
					{
						HDC hMemDC = CreateCompatibleDC(hDC);
						HGDIOBJ hOld = SelectObject(hMemDC, m_hDIBSection);

						BITMAP bmp;
						GetObject(m_hDIBSection, sizeof(bmp), & bmp);
						// have to use StretchBlt to use HALFTONE stretching mode
						StretchBlt(hDC, 10, 10, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

						SelectObject(hMemDC, hOld);
						DeleteObject(hMemDC);
					}
					
					EndPaint(hWnd, & ps);
				}
				return 0;

			case WM_PALETTECHANGED:
				if ( m_nOption & pal_react )
					return OnPaletteChanged(hWnd, wParam);
				break;

			case WM_QUERYNEWPALETTE:
				return OnQueryNewPalette();

			case WM_NCDESTROY:
				DeleteObject(m_hPalette);
				m_hPalette = NULL;
				return 0;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

public:

	KDIBWindow()
	{
		m_pBMI		  = NULL;
		m_pBits		  = NULL;
		m_hDIBSection = NULL;
		m_hInst       = NULL;
	}

	~KDIBWindow()
	{
		if ( m_hDIBSection )
			DeleteObject(m_hDIBSection);
	}

	void CreateDIBWindow(HINSTANCE hInst, const BITMAPINFO * pBMI, const BYTE * pBits, int option)
	{
		if ( pBMI==NULL )
			return;

		m_nOption = option;
		m_pBMI    = pBMI;
		m_pBits   = pBits;

		TCHAR title[32];
		wsprintf(title, _T("DIB Window (%d)"), m_nOption);

		CreateEx(0, _T("DIBWindow"), title, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			CW_USEDEFAULT, CW_USEDEFAULT, m_pBMI->bmiHeader.biWidth + 28, 
				m_pBMI->bmiHeader.biHeight + 48, NULL, NULL, hInst);
		ShowWindow(SW_NORMAL);
		UpdateWindow();
	}

	void CreateDIBWindow(HINSTANCE hInst, const TCHAR * resname, int option)
	{
		m_hInst       = hInst;
		m_nOption     = option;
		m_hDIBSection = (HBITMAP) LoadImage(hInst, resname, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		
		if ( m_hDIBSection==NULL )
			return;

		TCHAR title[32];
		wsprintf(title, _T("DIBSection Window (%d)"), m_nOption);

		CreateEx(0, _T("DIBWindow"), title, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			CW_USEDEFAULT, CW_USEDEFAULT, 300, 
				400, NULL, NULL, hInst);
		ShowWindow(SW_NORMAL);
		UpdateWindow();
	}

};
