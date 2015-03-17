//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : beziercurve.cpp					                                 //
//  Description: Generic arc/bezier curves drawing                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "BezierCurve.h"

// Simple text label
void Label(HDC hDC, int x, int y, const char * mess)
{
	TextOut(hDC, x, y, mess, _tcslen(mess));
}


// Label Bezier Curve control points
void Label(HDC hDC, POINT p[], int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	Label(hDC, p[0].x+x1, p[0].y+y1, "P1");
	Label(hDC, p[1].x+x2, p[1].y+y2, "P2");
	Label(hDC, p[2].x+x3, p[2].y+y3, "P3");
	Label(hDC, p[3].x+x4, p[3].y+y4, "P4");
}


// Point inteporation
void P(POINT & rslt, POINT & p1, POINT p2, double t)
{
	rslt.x = (int) ( (1-t) * p1.x + t * p2.x );
	rslt.y = (int) ( (1-t) * p1.y + t * p2.y );
}


// Line
void Line(HDC hDC, int x0, int y0, int x1, int y1)
{
	MoveToEx(hDC, x0, y0, NULL); LineTo(hDC, x1, y1);
}


// Line
void Line(HDC hDC, int x0, int y0, int x1, int y1, HPEN hPen)
{
	SelectObject(hDC, hPen);

	MoveToEx(hDC, x0, y0, NULL); LineTo(hDC, x1, y1);

	SelectObject(hDC, GetStockObject(BLACK_PEN));
}


// Line
void Line(HDC hDC, POINT & p1, POINT & p2)
{
	MoveToEx(hDC, p1.x, p1.y, NULL);
	LineTo(hDC, p2.x, p2.y);
}


// 3x3 dot
void Dot(HDC hDC, int x, int y)
{
	for (int i=-1; i<=1; i++)
	for (int j=-1; j<=1; j++)
		SetPixel(hDC, x+i, y+j, RGB(0, 0, 0xFF));
}


// simulate AngleArcTo using ArcTo
BOOL AngleArcTo(HDC hDC, int X, int Y, DWORD dwRadius, FLOAT eStartAngle, FLOAT eSweepAngle)
{
	const FLOAT pi = (FLOAT) 3.141592653586;

	if ( eSweepAngle >=360 )	// more than one circle is one circle
		eSweepAngle = 360;
	else if ( eSweepAngle <= -360 )
		eSweepAngle = - 360;

	FLOAT  eEndAngle   = (eStartAngle + eSweepAngle ) * pi / 180;
	       eStartAngle = eStartAngle * pi / 180;
	
	int dir;
	
	if ( eSweepAngle > 0 )
		dir = SetArcDirection(hDC, AD_COUNTERCLOCKWISE);
	else
		dir = SetArcDirection(hDC, AD_CLOCKWISE);
	
	BOOL rslt = ArcTo(hDC, X - dwRadius, Y - dwRadius, X + dwRadius, Y + dwRadius,
		              X + (int) (dwRadius * 10 * cos(eStartAngle)), 
					  Y - (int) (dwRadius * 10 * sin(eStartAngle)),
			          X + (int) (dwRadius * 10 * cos(eEndAngle)),   
					  Y - (int) (dwRadius * 10 * sin(eEndAngle)));

	SetArcDirection(hDC, dir);
	return rslt;
}


// Break Bezier curve into lines

void Bezier(HDC hDC, double x1, double y1, double x2, double y2, 
			         double x3, double y3, double x4, double y4)
{
	double A = y4 - y1;
	double B = x1 - x4;
	double C = y1 * (x4-x1) - x1 * ( y4-y1);
	// Ax + By + C = 0  is line (x1,y1) - (x4,y4)

	double AB  = A * A + B * B;

	// distance from (x2,y2) to the line is less than 1
	// distance from (x3,y3) to the line is less than 1
	if ( ( A * x2 + B * y2 + C ) * ( A * x2 + B * y2 + C ) < AB )
	if ( ( A * x3 + B * y3 + C ) * ( A * x3 + B * y3 + C ) < AB )
	{
		MoveToEx(hDC, (int)x1, (int)y1, NULL);
		LineTo(hDC, (int)x4, (int)y4);

		return;
	}
	
	double x12   = x1+x2;
	double y12   = y1+y2;
	double x23   = x2+x3;
	double y23   = y2+y3;
	double x34   = x3+x4;
	double y34   = y3+y4;

	double x1223 = x12+x23;
	double y1223 = y12+y23;
	double x2334 = x23+x34;
	double y2334 = y23+y34;

	double x     = x1223 + x2334;
	double y     = y1223 + y2334;

	Bezier(hDC, x1, y1, x12/2, y12/2, x1223/4, y1223/4, x/8, y/8);
	Bezier(hDC, x/8, y/8, x2334/4, y2334/4, x34/2, y34/2, x4, y4);
}


// Calculate coordinate according to Bezier curve formula
double P(double t, double p1, double p2, double p3, double p4)
{
	return     (1-t)*(1-t)*(1-t) * p1 +
		   3 * (1-t)*(1-t)*t     * p2 +
		   3 * (1-t)*t*t         * p3 +
		       t*t*t             * p4;
}


// calculate the largest error from a 90 degree Bezier curve relative to the unit circle
double Error(double m, double & et)
{
//	double R = 4 * ( sqrt(2.0) - 1 ) / 3;

	double t     = 0;
	double error = 0;

	while ( t<=0.5 )
	{
		double x = P(t, 0, m, 1, 1);
	    double y = P(t, 1, 1, m, 0);

		x = sqrt(x * x + y * y) - 1;

		if ( fabs(x)>fabs(error) )
		{
			et    = t;
			error = x;
		}

		t += 0.001;
	}

	return error;
}

/*
Approximating sqrt(2)-1

n		m		n/m
1		2		0.5
2		5		0.4
5		12		0.416666667
12		29		0.413793103
29		70		0.414285714
70		169		0.414201183
169		408		0.414215686
408		985		0.414213198
985		2378	0.414213625
2378	5741	0.414213552
5741	13860	0.414213564

n:m -> m : 2m+n
*/

// Drawing a full ellipse using 4 Bezier curves
BOOL EllipseToBezier(HDC hDC, int left, int top, int right, int bottom)
{
	const double M = 0.55228474983;
	
	POINT P[13];

	int dx = (int) ((right - left) * (1-M) / 2);
	int dy = (int) ((bottom - top) * (1-M) / 2);

	P[ 0].x = right;			//	 .   .   .   .   .   
	P[ 0].y = (top+bottom)/2;   //       4   3   2
	P[ 1].x = right;			//
	P[ 1].y = top + dy;         //   5               1
	P[ 2].x = right - dx;       //
	P[ 2].y = top;				//   6              0,12
	P[ 3].x = (left+right)/2;	//
	P[ 3].y = top;				//   7               11	
								//
	P[ 4].x = left + dx;		//       8   9   10	
	P[ 4].y = top;				
	P[ 5].x = left;
	P[ 5].y = top + dy;
	P[ 6].x = left;
	P[ 6].y = (top+bottom)/2;

	P[ 7].x = left;
	P[ 7].y = bottom - dy;
	P[ 8].x = left + dx;
	P[ 8].y = bottom;
	P[ 9].x = (left+right)/2;
	P[ 9].y = bottom;

	P[10].x = right - dx;
	P[10].y = bottom;
	P[11].x = right;
	P[11].y = bottom - dy;
	P[12].x = right;
	P[12].y = (top+bottom)/2;

	return PolyBezier(hDC, P, 13);
}


// Drawing an arc using a singple Bezier curve

BOOL AngleArcToBezier(HDC hDC, int x0, int y0, int rx, int ry, 
					  double startangle, double sweepangle, double * err)
{
	double XY[8];
	POINT P[4];

	// Compute bezier curve for arc centered along y axis
	// Anticlockwise: (0,-B), (x,-y), (x,y), (0,B) 
	double B = ry * sin(sweepangle/2);
	double C = rx * cos(sweepangle/2);
	double A = rx  - C;

	double X = A*4/3;
	double Y = B - X * (rx-A)/B;

	XY[0] =   C;
	XY[1] = - B;
	XY[2] =   C+X;
	XY[3] = - Y;
	XY[4] =   C+X;
	XY[5] =   Y;
	XY[6] =   C;
	XY[7] =   B;

	// rotate to the original angle
	double s = sin(startangle + sweepangle/2);
	double c = cos(startangle + sweepangle/2);

	double xy[8];
	
	for (int i=0; i<4; i++)
	{
		if ( err )
		{
			xy[i*2]   = XY[i*2] * c - XY[i*2+1] * s;
			xy[i*2+1] = XY[i*2] * s + XY[i*2+1] * c;
		}

		P[i].x = x0 + (int) (XY[i*2] * c - XY[i*2+1] * s);
		P[i].y = y0 + (int) (XY[i*2] * s + XY[i*2+1] * c);
	}

	if ( err )
	{		
		double t   = 0;

		* err = 0;

		while ( t<=1 )
		{
			double x = ::P(t, xy[0], xy[2], xy[4], xy[6]) / rx;
			double y = ::P(t, xy[1], xy[3], xy[5], xy[7]) / ry;
			double r = sqrt(x*x + y*y);

			if ( fabs(r-1) > fabs(*err) )
				*err = r-1;

			t += 0.01;
		}
	}

	return PolyBezier(hDC, P, 4);
}

