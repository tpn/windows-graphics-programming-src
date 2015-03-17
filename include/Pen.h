#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : pen.h						                                         //
//  Description: GDI pen object wrapper                                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

// Wrapper for GDI object selection
class KSelect
{
	HGDIOBJ m_hOld;
	HDC		m_hDC;

public:
	
	void Select(HDC hDC, HGDIOBJ hObject)
	{
		if ( hDC )
		{
			m_hDC  = hDC;
			m_hOld = SelectObject(hDC, hObject);
		}
		else
		{
			m_hDC  = NULL;
			m_hOld = NULL;
		}
	}

	void UnSelect(void)
	{
		if ( m_hDC )
		{
			SelectObject(m_hDC, m_hOld);
			m_hDC  = NULL;
			m_hOld = NULL;
		}
	}

};


class KPen : public KSelect
{
public:
	HPEN    m_hPen;

	KPen(int style, int width, COLORREF color, HDC hDC=NULL)
	{
		m_hPen = CreatePen(style, width, color);
		Select(hDC);
	}

	KPen(int style, int width, COLORREF color, int count, DWORD * gap, HDC hDC=NULL)
	{
		LOGBRUSH logbrush = { BS_SOLID, color, 0 };

		m_hPen = ExtCreatePen(style, width, & logbrush, count, gap);
		Select(hDC);
	}

	void Select(HDC hDC)
	{
		KSelect::Select(hDC, m_hPen);
	}

	~KPen()
	{
		UnSelect();
		DeleteObject(m_hPen);
	}
};
