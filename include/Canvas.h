#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : canvas.h						                                     //
//  Description: Simple drawing child window                                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KStatusWindow;

// main drawing window, normally a child window of a frame window

class KCanvas : public KWindow
{
public:
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void    OnDraw(HDC hDC, const RECT * rcPaint);
	virtual BOOL	OnCommand(WPARAM wParam, LPARAM lParam);
	
	KStatusWindow * m_pStatus;
	HINSTANCE		m_hInst;

	KCanvas();

	void SetStatus(HINSTANCE hInst, KStatusWindow * pStatus)
	{
		m_hInst   = hInst;
		m_pStatus = pStatus;
	}

	virtual ~KCanvas();
};