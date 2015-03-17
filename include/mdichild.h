#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : mdichild.h						                                     //
//  Description: Simple MDI child window                                             //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KMDIChild : public KWindow
{
protected:
	HMENU		m_hViewMenu;
	int			m_nMenuID;
	HINSTANCE	m_hInstance;
	HBRUSH		m_hBackground;

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch( uMsg )
		{
			case WM_CREATE:
				m_hWnd		= hWnd;
				m_hViewMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(m_nMenuID));
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
		wc.hIcon          = NULL;
		wc.hCursor        = LoadCursor(NULL, IDC_ARROW); 
		wc.hbrBackground  = m_hBackground;
		wc.lpszMenuName   = NULL;
		wc.lpszClassName  = NULL;
		wc.hIconSm        = NULL;
	}
	
public:

	const TCHAR * GetClassName(void) const
	{
		return _T("MDIChild");
	}

	bool Initialize(HINSTANCE hInstance, int menuid)
	{
		m_nMenuID   = menuid;
		m_hInstance = hInstance;

		RegisterClass(GetClassName(), hInstance);
		
		return true;
	}

	KMDIChild(HBRUSH hBackground = NULL)
	{
		if ( hBackground ) 
			m_hBackground = hBackground;
		else
			m_hBackground = (HBRUSH) (COLOR_WINDOW + 1);
	}

};
