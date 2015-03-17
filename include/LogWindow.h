#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : logwindow.h				                                         //
//  Description: Text-based logging window                                           //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

// must be dynamically allocated to maintain the window, delete itself in WM_NCDESTROY message

class KLogWindow : KWindow
{
	HINSTANCE m_hInst;
	HWND	  m_hEditWnd;
	HICON	  m_hIcon;

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KWindow::GetWndClassEx(wc);

		wc.hIcon = m_hIcon;
	}

public:
	KLogWindow()
	{
		m_hInst    = NULL;
		m_hEditWnd = NULL;
		m_hIcon    = NULL;
	}

	virtual ~ KLogWindow()
	{
	}
	
	void ShowWindow(bool bShow)
	{
		if ( bShow )
			::ShowWindow(m_hWnd, SW_RESTORE | SW_SHOWNORMAL);
		else
			::ShowWindow(m_hWnd, SW_HIDE | SW_MINIMIZE);
	}

	void Create(HINSTANCE hInst, const TCHAR * pTitle, HICON hIcon=NULL);
	
	void Log(const char * format, ...);
	void DumpRegion(const char * mess, HRGN hRgn, bool detail, int p1=0);
};
