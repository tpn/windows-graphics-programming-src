#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : win.h						                                         //
//  Description: Generic window class                                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KWindow
{
protected:

	virtual void OnDraw(HDC hDC)
	{
	}

	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam)
	{
	}
    
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void GetWndClassEx(WNDCLASSEX & wc);

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
   
	HRESULT CommonMDIChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
		HMENU hMenu, int nWindowMenu);

	HPALETTE m_hPalette;
	int		 m_nUpdateCount;

	virtual LRESULT OnQueryNewPalette(void);
	virtual LRESULT OnPaletteChanged(HWND hWnd, WPARAM wParam);

	bool    m_bMainWindow;

public:

	HWND  m_hWnd;

	KWindow(void)
	{
		m_hWnd		   = NULL;
		m_hPalette	   = NULL;
		m_nUpdateCount = 0;
		m_bMainWindow  = false;
	}

	virtual ~KWindow(void)
	{
		if ( m_hPalette )
		{
			DeleteObject(m_hPalette);
			m_hPalette = NULL;
		}
	}
    
	virtual bool CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, LPCTSTR lpszName, DWORD dwStyle, 
		int x, int y, int nWidth, int nHeight, HWND hParent, HMENU hMenu, HINSTANCE hInst);

	bool RegisterClass(LPCTSTR lpszClass, HINSTANCE hInst);

	virtual WPARAM MessageLoop(void);

	BOOL ShowWindow(int nCmdShow) const
	{
		return ::ShowWindow(m_hWnd, nCmdShow);
	}

	BOOL UpdateWindow(void) const
	{
		return ::UpdateWindow(m_hWnd);
	}
};


int MyMessageBox(HWND hWnd, const TCHAR * text, const TCHAR * caption, DWORD style, int iconid);
