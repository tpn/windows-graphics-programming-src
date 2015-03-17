//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : areafill.cpp						                                 //
//  Description: Gradient fill, pattern fill                                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <math.h>

#include "color.h"
#include "areafill.h"

BOOL GradientRectangle(HDC hDC, int x0, int y0, int x1, int y1, COLORREF c0, COLORREF c1, int angle)
{
	TRIVERTEX vert[4] = { 
		{ x0, y0,  R16(c0), G16(c0), B16(c0), 0 },				//  0:c0         3:(c0+c1)/2
		{ x1, y1,  R16(c1), G16(c1), B16(c1), 0 },				//
		{ x0, y1,  R16(c0, c1), G16(c0, c1), B16(c0, c1), 0 },	//
		{ x1, y0,  R16(c0, c1), G16(c0, c1), B16(c0, c1), 0 }	//  2:(c0+c1)/2  1: c1
	};

	ULONG Index[] = { 0, 1, 2, 0, 1, 3};

	switch ( angle % 180 )
	{
		case   0: 
			return GradientFill(hDC, vert, 2, Index, 1, GRADIENT_FILL_RECT_H);

		case  45: 
			return GradientFill(hDC, vert, 4, Index, 2, GRADIENT_FILL_TRIANGLE);

		case  90: 
			return GradientFill(hDC, vert, 2, Index, 1, GRADIENT_FILL_RECT_V);

		case 135: 
			vert[0].x = x1;
			vert[3].x = x0;
			vert[1].x = x0;
			vert[2].x = x1;
			return GradientFill(hDC, vert, 4, Index, 2, GRADIENT_FILL_TRIANGLE);
	}

	return FALSE;
}

BOOL SymGradientRectangle(HDC hDC, int x0, int y0, int x1, int y1, COLORREF c0, COLORREF c1, int angle)
{
	TRIVERTEX vert[] = { 
		{ x0,        y0,  R16(c0), G16(c0), B16(c0), 0 },
		{ x1, (y0+y1)/2,  R16(c1), G16(c1), B16(c1), 0 },
		{ x0, (y0+y1)/2,  R16(c1), G16(c1), B16(c1), 0 },
		{ x1,        y1,  R16(c0), G16(c0), B16(c0), 0 },
		
		{ x1,		 y0,  R16(c1), G16(c1), B16(c1), 0 },
		{ x0,		 y1,  R16(c1), G16(c1), B16(c1), 0 },

		{ (x0+x1)/2, y1,  R16(c1), G16(c1), B16(c1), 0 },
		{ (x0+x1)/2, y0,  R16(c1), G16(c1), B16(c1), 0 }
	};

	switch ( angle % 180 )
	{
		case 0:
		{
			ULONG Index[] = { 0, 6, 7, 3 };
			return GradientFill(hDC, vert, 8, Index, 2, GRADIENT_FILL_RECT_H);
		}

		case 45:
		{
			ULONG Index[] = { 0, 4, 5, 4, 5, 3 };
			return GradientFill(hDC, vert, 6, Index, 2, GRADIENT_FILL_TRIANGLE);
		}

		case 90:
		{
			ULONG Index[] = { 0, 1, 2, 3 };
			return GradientFill(hDC, vert, 4, Index, 2, GRADIENT_FILL_RECT_V);
		}

		case 135:
		{
			TRIVERTEX vert[4] = { 
				{ x0, y0,  R16(c1), G16(c1), B16(c1), 0 },
				{ x1, y0,  R16(c0), G16(c0), B16(c0), 0 },
				{ x0, y1,  R16(c0), G16(c0), B16(c0), 0 },
				{ x1, y1,  R16(c1), G16(c1), B16(c1), 0 }
			};

			ULONG Index[] = { 0, 1, 3, 0, 3, 2 };

			return GradientFill(hDC, vert, 4, Index, 2, GRADIENT_FILL_TRIANGLE);
		}
	}

	return FALSE;
}

BOOL CornerGradientRectangle(HDC hDC, int x0, int y0, int x1, int y1, COLORREF c0, COLORREF c1, int corner)
{
	TRIVERTEX vert[] = { 
		{ x0,        y0,  R16(c1), G16(c1), B16(c1), 0 },
		{ x1,        y0,  R16(c1), G16(c1), B16(c1), 0 },
		{ x1,		 y1,  R16(c1), G16(c1), B16(c1), 0 },
		{ x0,		 y1,  R16(c1), G16(c1), B16(c1), 0 }
	};

	vert[corner].Red   = R16(c0);
	vert[corner].Green = G16(c0);
	vert[corner].Blue  = B16(c0);

	ULONG Index[] = { corner, (corner+1)%4, (corner+2)%4, 
		              corner, (corner+3)%4, (corner+2)%4 };
	
	return GradientFill(hDC, vert, 4, Index, 2, GRADIENT_FILL_TRIANGLE);
}

BOOL CenterGradientRectangle(HDC hDC, int x0, int y0, int x1, int y1, COLORREF c0, COLORREF c1)
{
	TRIVERTEX vert[] = { 
		{ x0,        y0,		R16(c1), G16(c1), B16(c1), 0 },
		{ x1,        y0,		R16(c1), G16(c1), B16(c1), 0 },
		{ x1,		 y1,		R16(c1), G16(c1), B16(c1), 0 },
		{ x0,		 y1,		R16(c1), G16(c1), B16(c1), 0 },
		{ (x0+x1)/2, (y0+y1)/2, R16(c0), G16(c0), B16(c0), 0 }
	};

	ULONG Index[] = { 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4 };
	
	return GradientFill(hDC, vert, 5, Index, 4, GRADIENT_FILL_TRIANGLE);
}


void HLSGradientRectangle(HDC hDC, int x0, int y0, int x1, int y1, COLORREF cref0, COLORREF cref1, int nPart)
{
	KColor c0(cref0); c0.ToHLS();
	KColor c1(cref1); c1.ToHLS();

	for (int i=0; i<nPart; i++)
	{
		KColor c;

		c.hue        = ( c0.hue       * (nPart-1-i) + c1.hue       * i ) / (nPart-1);
		c.lightness  = ( c0.lightness * (nPart-1-i) + c1.lightness * i ) / (nPart-1);
		c.saturation = ( c0.saturation* (nPart-1-i) + c1.saturation* i ) / (nPart-1);
		c.ToRGB();

		HBRUSH hBrush = CreateSolidBrush(c.GetColorRef());

		RECT rect = { x0+i*(x1-x0)/nPart,     y0, 
					  x0+(i+1)*(x1-x0)/nPart, y1 }; 
		FillRect(hDC, & rect, hBrush);
		DeleteObject(hBrush);
	}
}


BOOL RadialGradientFill(HDC hDC, int x0, int y0, int x1, int y1, int r, COLORREF c0, COLORREF c1, int nPart)
{
	const double PI2 = 3.1415927 * 2;

	TRIVERTEX * pVertex = new TRIVERTEX[nPart+1];
	ULONG     * pMesh   = new ULONG[(nPart+1)*3];

	pVertex[0].x     = x1;
	pVertex[0].y     = y1;
	pVertex[0].Red   = R16(c0);
	pVertex[0].Green = G16(c0);
	pVertex[0].Blue  = G16(c0);
	pVertex[0].Alpha = 0;

	for (int i=0; i<nPart; i++)
	{
		pVertex[i+1].x     = x0 + (int) (r * cos(PI2 * i / nPart));
		pVertex[i+1].y     = y0 + (int) (r * sin(PI2 * i / nPart));
		pVertex[i+1].Red   = R16(c1);
		pVertex[i+1].Green = G16(c1);
		pVertex[i+1].Blue  = B16(c1);
		pVertex[i+1].Alpha = 0;

		pMesh[i*3+0] = 0;
		pMesh[i*3+1] = i+1;
		pMesh[i*3+2] = (i+1) % nPart+1;
	}

	BOOL rslt = GradientFill(hDC, pVertex, nPart+1, pMesh, nPart, GRADIENT_FILL_TRIANGLE);

	delete [] pVertex;
	delete [] pMesh;

	return rslt;
}


void BrickPatternFill(HDC hDC, int x0, int y0, int x1, int y1, int width, int height)
{
	width  = abs(width);
	height = abs(height);

	if ( x0>x1 ) { int t = x0; x0 = x1; x1 = t; }
	if ( y0>y1 ) { int t = y0; y0 = y1; y1 = t; }

//	Rectangle(hDC, x0, y0, x1, y1);

	for (int y=y0; y<y1; y += height )
	for (int x=x0; x<x1; x += width  )
	{
		MoveToEx(hDC, x,       y, NULL); LineTo(hDC, x+width,   y+height);
		MoveToEx(hDC, x+width, y, NULL); LineTo(hDC, x+width/2, y+height/2);
	}
}

void RoundRectButton(HDC hDC, int x0, int y0, int x1, int y1, int w, int d, COLORREF c1, COLORREF c0)
{
	for (int i=0; i<2; i++)
	{
		POINT P[3] = { x0+d*i, y0+d*i, x1-d*i, y1-d*i, x0+d*i+w, y0+d*i+w };

		LPtoDP(hDC, P, 3);

		HRGN hRgn = CreateRoundRectRgn(P[0].x, P[0].y, P[1].x, P[1].y, P[2].x-P[0].x, P[2].y-P[0].y);
		SelectClipRgn(hDC, hRgn);
		DeleteObject(hRgn);
	
		if ( i==0 )
			GradientRectangle(hDC, x0, y0, x1, y1, c1, c0, 45);
		else
			GradientRectangle(hDC, x0+d, y0+d, x1-d, y1-d, c0, c1, 45);
	}
	
	SelectClipRgn(hDC, NULL);
}
