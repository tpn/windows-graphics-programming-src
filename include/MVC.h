#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : mvc.h						                                         //
//  Description: KView class (drawing), KMDICanvas class (MDI child window)          //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KView
{
protected:
	int m_nPixelWidth;
	int m_nPixelHeight;

public:

	typedef enum { View_NoChange = 0, View_Redraw = 1, View_Resize = 2 };

	KView(void)
	{
		m_nPixelWidth  = 320;
		m_nPixelHeight = 240;
	}

	~KView(void)
	{
	}

	int GetPixelWidth(void) const
	{
		return m_nPixelWidth;
	}

	int GetPixelHeight(void) const
	{
		return m_nPixelHeight;
	}

	virtual int OnCommand(int cmd, HWND hWnd) = 0;
	virtual int OnDraw(HDC hDC, const RECT * rcPaint) = 0;
	
	virtual int OnKey(int vkey)
	{
		return View_NoChange;
	}
};


///////////////////////////////////////////////////
//			                         //
// KMDICanvas: A Generic MDI Child Window Class  //
//                                               //
/////////////////////////////////////////////////// 

class KMDICanvas : public KScrollCanvas
{
	HMENU		m_hViewMenu;
	int			m_nMenuID;
	int			m_nIconID;
	KView *		m_pView;
	HINSTANCE	m_hInstance;
	HBRUSH		m_hBackground;
	
	void Response(int rslt)
	{
		if ( rslt & KView::View_Resize )
		{
			if ( m_pView )
				SetSize(m_pView->GetPixelWidth(), m_pView->GetPixelHeight(), 20, 20, TRUE);
			rslt |= KView::View_Redraw;
		}

		if ( rslt & KView::View_Redraw )
			InvalidateRect(m_hWnd, NULL, TRUE);
	}

	virtual void  OnDraw(HDC hDC, const RECT * rcPaint)
	{
		if ( m_pView )
			Response(m_pView->OnDraw(hDC, rcPaint));
	}

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch( uMsg )
		{
			case WM_CREATE:
				m_hWnd		= hWnd;
				m_hViewMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(m_nMenuID));
				
				if ( m_pView )
					SetSize(m_pView->GetPixelWidth(), m_pView->GetPixelHeight(), 20, 20, TRUE);
				else
					SetSize(320, 240, 20, 20, TRUE);
				return 0;

			case WM_PAINT:
				return KScrollCanvas::WndProc(hWnd, uMsg, wParam, lParam);

			case WM_SIZE:
			case WM_HSCROLL:
			case WM_VSCROLL:
				return KScrollCanvas::WndProc(hWnd, uMsg, wParam, lParam);

			case WM_COMMAND:
				if ( m_pView )
					Response( m_pView->OnCommand(LOWORD(wParam), hWnd) );
				return 0;
				break;

			case WM_KEYDOWN:
				if ( m_pView )
					Response( m_pView->OnKey(wParam) );
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
		wc.hIcon          = LoadIcon(m_hInstance, MAKEINTRESOURCE(m_nIconID));
		wc.hCursor        = LoadCursor(NULL, IDC_ARROW); 

		if ( m_hBackground )
			wc.hbrBackground  = m_hBackground;
		else
			wc.hbrBackground  = (HBRUSH) (COLOR_WINDOW + 1);
		
		wc.lpszMenuName   = NULL;
		wc.lpszClassName  = NULL;
		wc.hIconSm        = NULL;
	}
	
public:

	const TCHAR * GetClassName(void) const
	{
		return _T("MDICanvas");
	}

	bool Initialize(HINSTANCE hInstance, KStatusWindow * pStatus, KView * pView, int menuid, int iconid)
	{
		m_nMenuID   = menuid;
		m_nIconID   = iconid;
		m_hInstance = hInstance;
		m_pStatus   = pStatus;
		m_pView     = pView;

		RegisterClass(GetClassName(), hInstance);
		
		return true;
	}

	KMDICanvas(HBRUSH hBackground = NULL)
	{
		m_hBackground = hBackground;
	}

	~KMDICanvas()
	{
		if ( m_pView )
			delete m_pView;
	}
};
