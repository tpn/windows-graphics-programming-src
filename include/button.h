#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : button.h						                                     //
//  Description: Clickable button without window                                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KButton
{
protected:
	HRGN m_hRegion; 
	bool m_bOn;
	int  m_x, m_y, m_w, m_h;

public:

	KButton()
	{
		m_hRegion = NULL;
		m_bOn     = false;
	}

	virtual ~KButton()
	{
	}

	void DefineButton(int x, int y, int w, int h)
	{
		m_x = x; m_y = y; m_w = w; m_h = h;
	}

	virtual void DrawButton(HDC hDC)
	{
	}

	void UpdateButton(HDC hDC, LPARAM xy)
	{
		if ( m_bOn != IsOnButton(xy) )
		{
			m_bOn = ! m_bOn;
			DrawButton(hDC);
		}
	}
	
	bool IsOnButton(LPARAM xy) const
	{
		return PtInRegion(m_hRegion, LOWORD(xy), HIWORD(xy)) != 0;
	}
};


class KRectButton : public KButton
{
public:
	void DrawButton(HDC hDC)
	{
		RECT rect = { m_x, m_y, m_x+m_w, m_y+m_h };

		if ( m_hRegion == NULL )
			m_hRegion = CreateRectRgnIndirect(& rect);

		InflateRect(&rect, 2, 2);
		FillRect(hDC, & rect, GetSysColorBrush(COLOR_BTNFACE));
		InflateRect(&rect, -2, -2);

		DrawFrameControl(hDC, &rect, DFC_CAPTION, DFCS_CAPTIONHELP | (m_bOn ? 0 : DFCS_INACTIVE));
	}
};


class KEllipseButton : public KButton
{
public:
	void DrawButton(HDC hDC)
	{
		RECT rect = { m_x, m_y, m_x+m_w, m_y+m_h };

		if ( m_hRegion == NULL )
			m_hRegion = CreateEllipticRgnIndirect(& rect);

		if ( m_bOn )
		{
			FillRgn(hDC, m_hRegion, GetSysColorBrush(COLOR_CAPTIONTEXT));
			FrameRgn(hDC, m_hRegion, GetSysColorBrush(COLOR_ACTIVEBORDER), 2, 2);
		}
		else
		{
			FillRgn(hDC, m_hRegion, GetSysColorBrush(COLOR_INACTIVECAPTIONTEXT));
			FrameRgn(hDC, m_hRegion, GetSysColorBrush(COLOR_INACTIVEBORDER), 2, 2);
		}
	}
};


