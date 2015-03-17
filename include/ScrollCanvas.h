#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : scrollcanvas.h				                                         //
//  Description: Scrolling child window                                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//


class KScrollCanvas : public KCanvas
{
protected:
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int m_width,   m_height;
	int m_linedx,  m_linedy;
	int m_zoommul, m_zoomdiv;

	virtual void OnZoom(int x, int y, int mul, int div);
	
	virtual void OnTimer(WPARAM wParam, LPARAM lParam)
	{
	}

	virtual void OnMouseMove(WPARAM wParam, LPARAM lParam)
	{
	}

	virtual void OnCreate(void)
	{
	}

	virtual void OnDestroy(void)
	{
	}

public:

	KScrollCanvas(void)
	{
		m_width   = 0; m_height  = 0;
		m_linedx  = 0; m_linedy  = 0;
		m_zoommul = 1; m_zoomdiv = 1;
	}

	void SetSize(int width, int height, int linedx, int linedy, bool resize=false);
	void SetScrollBar(int side, int maxsize, int pagesize);
	void OnScroll(int nBar, int nScrollCode, int nPos);

	virtual bool GetTitle(const TCHAR * pFileName, TCHAR * pTitle)
	{
		return false;
	}
};
