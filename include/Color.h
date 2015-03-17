#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : color.h						                                     //
//  Description: Color space conversion                                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

const COLORREF black 	   = RGB(   0,    0,    0);
const COLORREF darkred     = RGB(0x80,    0,    0);
const COLORREF darkgreen   = RGB(   0, 0x80,    0);
const COLORREF darkyellow  = RGB(0x80, 0x80,    0);
const COLORREF darkblue    = RGB(   0,    0, 0x80);
const COLORREF darkmagenta = RGB(0x80,    0, 0x80);
const COLORREF darkcyan    = RGB(   0, 0x80, 0x80);
const COLORREF darygray    = RGB(0x80, 0x80, 0x80);

const COLORREF moneygreen  = RGB(0xC0, 0xDC, 0xC0);
const COLORREF skyblue     = RGB(0xA6, 0xCA, 0xF0);
const COLORREF cream       = RGB(0xFF, 0xFB, 0xF0);
const COLORREF mediumgray  = RGB(0xA0, 0xA0, 0xA4);
const COLORREF lightgray   = RGB(0xC0, 0xC0, 0xC0);

const COLORREF red         = RGB(0xFF,    0,    0);
const COLORREF green       = RGB(   0, 0xFF,    0);
const COLORREF yellow      = RGB(   0, 0xFF,    0);
const COLORREF blue        = RGB(   0,    0, 0xFF);
const COLORREF magenta	   = RGB(0xFF,    0, 0xFF);
const COLORREF cyan	       = RGB(   0, 0xFF, 0xFF);
const COLORREF white	   = RGB(0xFF, 0xFF, 0xFF);


class KColor
{
	typedef enum
	{
		Red,
		Green,
		Blue
	};

public:

	unsigned char red;
	unsigned char green;
	unsigned char blue;

	double lightness, saturation, hue;

	KColor()
	{
	}

	KColor(double h, double l, double s)
	{
		hue		   = h;
		lightness  = l;
		saturation = s;
	}

	KColor (BYTE r, BYTE g, BYTE b)
	{
		red   = r;
		green = g;
		blue  = b;
	}

	KColor(COLORREF color)
	{
		red   = GetRValue(color);
		green = GetGValue(color);
		blue  = GetBValue(color);
	}
	
	void ToHLS(void);
	void ToRGB(void);

	COLORREF GetColorRef(void) const
	{
		return RGB(red, green, blue);
	}
};


