#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : charset.h						                                     //
//  Description: KCharSetView class                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define UNICODE_CHARSET 0


class KCharSetView : public KView
{
	BYTE			m_nCharSet;
	HFONT			m_hAnsiFont;
		
	int				m_width;
	int				m_height;
	int				m_font;
	int				m_row;
	
	void DispRow(HDC hDC, int x0, int y0, int y, WCHAR ch, int length);

public:

	KCharSetView(void);

	virtual int OnCommand(int cmd, HWND hWnd);
	virtual int OnDraw(HDC hDC, const RECT * rcPaint);

};
