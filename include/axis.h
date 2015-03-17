#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : axis.h						                                         //
//  Description: Arrow, axis drawing                                                 //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KRotate
{
	int m_x0, m_y0, m_dx, m_dy;

public:

	int m_ds;
	
	KRotate(int x0, int y0, int x1, int y1)
	{
		m_x0 = x0;
		m_y0 = y0;
		m_dx = x1 - x0;
		m_dy = y1 - y0;
		m_ds = (int) sqrt( m_dx * m_dx + m_dy * m_dy);
	}

	int RotateX(int x, int y)
	{
		return x * m_dx / m_ds - y * m_dy / m_ds;
	}

	int RotateY(int x, int y)
	{
		return x * m_dy / m_ds + y * m_dx / m_ds;
	}
};


// Draw a line with an arrow	
void Arrow(HDC hDC, int x0, int y0, int x1, int y1, int width, int height);

// Convert size in points to device coordinate
void PointToDevice(HDC hDC, SIZE & size);

// Convert size in device coordinate to logical coordinate
void DeviceToLogical(HDC hDC, SIZE & size);

// Generic algorithm for axes display: width height in device coordinate
void ShowAxes(HDC hDC, int width, int height);
