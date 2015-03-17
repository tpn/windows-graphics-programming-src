//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : axis.cpp						                                     //
//  Description: Arrow and axis drawing                                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <math.h>

#include "axis.h"


// Draw a line with an arrow	
void Arrow(HDC hDC, int x0, int y0, int x1, int y1, int width, int height)
{
	MoveToEx(hDC, x0, y0, NULL);
	LineTo(hDC, x1, y1);

	KRotate rotate(x0, y0, x1, y1);

	POINT arrow[4] = 
	{ 
		x1, y1, 
		
		rotate.RotateX(rotate.m_ds-width,  height) + x0, 
		rotate.RotateY(rotate.m_ds-width,  height) + y0,
		
		rotate.RotateX(rotate.m_ds-width, -height) + x0, 
		rotate.RotateY(rotate.m_ds-width, -height) + y0,
		
		x1, y1
	};

	HGDIOBJ hOld = SelectObject(hDC, GetStockObject(BLACK_BRUSH));
	Polygon(hDC, arrow, 4);
	SelectObject(hDC, hOld);
}


// Convert size in points to device coordinate
void PointToDevice(HDC hDC, SIZE & size)
{
	int dpix = GetDeviceCaps(hDC, LOGPIXELSX);
	int dpiy = GetDeviceCaps(hDC, LOGPIXELSY);

	size.cx = size.cx * dpix / 72;
    size.cy = size.cy * dpiy / 72;
}


// Convert size in device coordinate to logical coordinate
void DeviceToLogical(HDC hDC, SIZE & size)
{
	POINT p[2] = { 0, 0, size.cx, size.cy };

	DPtoLP(hDC, p, 2);

	size.cx = abs(p[1].x - p[0].x);
	size.cy = abs(p[1].y - p[0].y);
}


// Generic algorithm for axes display: width height in device coordinate
void ShowAxes(HDC hDC, int width, int height)
{
	POINT corner[2];

	// display axes for the area [10, 10, width-10, height-10]
	corner[0].x = 10;
	corner[0].y = 10;
	corner[1].x = width - 10;
	corner[1].y = height - 10;

	// covert to logical coordinate
	DPtoLP(hDC, corner, 2);

	// we need to display a 10x2 point arrow, convert to device coordinate, then to logical
	SIZE s = { 10, 2 };
	PointToDevice(hDC, s);
	DeviceToLogical(hDC, s);

	// X-axis
	Arrow(hDC, min(corner[0].x, corner[1].x), 0, 
		       max(corner[0].x, corner[1].x), 0, s.cx, s.cy);

	// Y-axis
	Arrow(hDC, 0, min(corner[0].y, corner[1].y), 
		       0, max(corner[0].y, corner[1].y), s.cx, s.cy);
}

