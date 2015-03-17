#pragma once
//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : beziercurve.h					                                     //
//  Description: Arc/Bezier curve drawing routines                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//


void Label(HDC hDC, int x, int y, const TCHAR * mess);
void Label(HDC hDC, POINT p[], int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);

void Dot(HDC hDC, int x, int y);

void P(POINT & rslt, POINT & p1, POINT p2, double t);

void Line(HDC hDC, POINT & p1, POINT & p2);
void Line(HDC hDC, int x0, int y0, int x1, int y1);
void Line(HDC hDC, int x0, int y0, int x1, int y1, HPEN hPen);

BOOL AngleArcTo(HDC hDC, int X, int Y, DWORD dwRadius, FLOAT eStartAngle, FLOAT eSweepAngle);

void Bezier(HDC hDC, double x1, double y1, double x2, double y2, 
			         double x3, double y3, double x4, double y4);

double P(double t, double p1, double p2, double p3, double p4);
double Error(double m, double & et);

BOOL EllipseToBezier(HDC hDC, int left, int top, int right, int bottom);

BOOL AngleArcToBezier(HDC hDC, int x0, int y0, int rx, int ry, 
					  double startangle, double sweepangle, double * err=NULL);
