//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : affine.cpp						                                     //
//  Description: Affine transformation                                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <math.h>

#include "Affine.h"


// Reset to identity transform
void KAffine::Reset()
{
	m_xm.eM11 = 1;
	m_xm.eM12 = 0;
	m_xm.eM21 = 0;
	m_xm.eM22 = 1;
	m_xm.eDx  = 0;
	m_xm.eDy  = 0;
}


// Copy transform if valid
BOOL KAffine::SetTransform(const XFORM & xm)
{
	if ( xm.eM11 * xm.eM22 == xm.eM12 * xm.eM21 )
		return FALSE;

	m_xm = xm;	
	return TRUE;
}


// transform = transform * b
BOOL KAffine::Combine(const XFORM & b)
{
	if ( b.eM11 * b.eM22 == b.eM12 * b.eM21 )
		return FALSE;

	XFORM a = m_xm;

	// 11 12   11 12
	// 21 22   21 22
	m_xm.eM11 = a.eM11 * b.eM11 + a.eM12 * b.eM21;
	m_xm.eM12 = a.eM11 * b.eM12 + a.eM12 * b.eM22;
	m_xm.eM21 = a.eM21 * b.eM11 + a.eM22 * b.eM21;
	m_xm.eM22 = a.eM21 * b.eM12 + a.eM22 * b.eM22;
	m_xm.eDx  = a.eDx  * b.eM11 + a.eDy  * b.eM21 + b.eDx;
	m_xm.eDy  = a.eDx  * b.eM12 + a.eDy  * b.eM22 + b.eDy;

	return TRUE;
}


// transform = 1 / transform
//  M = A * x + B 
//  Inv(M) = Inv(A) * x - Inv(A) * B
BOOL KAffine::Invert(void)
{
	FLOAT det = m_xm.eM11 * m_xm.eM22 - m_xm.eM21 * m_xm.eM12;

	if ( det==0 )
		return FALSE;

	XFORM old = m_xm;

	m_xm.eM11 =   old.eM22 / det;
	m_xm.eM12 = - old.eM12 / det;
	m_xm.eM21 = - old.eM21 / det;
	m_xm.eM22 =   old.eM11 / det;
	
	m_xm.eDx  = - ( m_xm.eM11 * old.eDx + m_xm.eM21 * old.eDy );
	m_xm.eDy  = - ( m_xm.eM12 * old.eDx + m_xm.eM22 * old.eDy );

	return TRUE;
}


BOOL KAffine::Translate(FLOAT dx, FLOAT dy)
{
	m_xm.eDx += dx;
	m_xm.eDy += dy;

	return TRUE;
}


BOOL KAffine::Scale(FLOAT sx, FLOAT sy)
{
	if ( (sx==0) || (sy==0) )
		return FALSE;

	m_xm.eM11 *= sx;
	m_xm.eM12 *= sx;
	m_xm.eM21 *= sy;
	m_xm.eM22 *= sy;
	m_xm.eDx  *= sx;
	m_xm.eDy  *= sy;

	return TRUE;
}


BOOL KAffine::Rotate(FLOAT angle, FLOAT x0, FLOAT y0)
{
	XFORM xm;

	Translate(-x0, -y0);	// make (x0,y0) the origin
	
    double rad = angle * (3.14159265359 / 180); 

	xm.eM11 =   (FLOAT) cos(rad);
	xm.eM12 =   (FLOAT) sin(rad);
	xm.eM21 = -         xm.eM12;
	xm.eM22 =           xm.eM11;
	xm.eDx  = 0;
	xm.eDy  = 0;

	Combine(xm);			// rotate
	Translate(x0, y0);		// move origin back

	return TRUE;
}


// Find a transform which maps (0,0) (1,0) (0,1) to p, q, and r respectively
BOOL KAffine::MapTri(FLOAT px0, FLOAT py0, FLOAT qx0, FLOAT qy0, FLOAT rx0, FLOAT ry0)
{
	// px0 = dx, qx0 = m11 + dx, rx0 = m21 + dx
	// py0 = dy, qy0 = m12 + dy, ry0 = m22 + dy
	m_xm.eM11 = qx0 - px0;
	m_xm.eM12 = qy0 - py0;
	m_xm.eM21 = rx0 - px0;
	m_xm.eM22 = ry0 - py0;
	m_xm.eDx  = px0;
	m_xm.eDy  = py0;

	return m_xm.eM11 * m_xm.eM22 != m_xm.eM12 * m_xm.eM21;
}


// Find a transform which maps p0, q0, and r0 to p1, p1 and r1 respectively
BOOL KAffine::MapTri(FLOAT px0, FLOAT py0, FLOAT qx0, FLOAT qy0, FLOAT rx0, FLOAT ry0,
			  	     FLOAT px1, FLOAT py1, FLOAT qx1, FLOAT qy1, FLOAT rx1, FLOAT ry1)
{
	if ( ! MapTri(px0, py0, qx0, qy0, rx0, ry0) )
		return FALSE;

	Invert();		// transform p0, q0, and r0 back to (0,0),(1,0),(0,1)

	KAffine map1;

	if (! map1.MapTri(px1, py1, qx1, qy1, rx1, ry1) )
		return FALSE;

	return Combine(map1.m_xm);	// then to p1,r1,q1
}


// get the combined world to device coordinate space mapping
BOOL KAffine::GetDPtoLP(HDC hDC)
{
	if ( ! GetWorldTransform(hDC, & m_xm) )
		return FALSE;

	POINT origin;
	GetWindowOrgEx(hDC, & origin);
	Translate( - (FLOAT) origin.x, - (FLOAT) origin.y);

	SIZE  sizew, sizev;
	GetWindowExtEx  (hDC, & sizew);
	GetViewportExtEx(hDC, & sizev);
	
	Scale( (FLOAT) sizew.cx/sizev.cx, (FLOAT) sizew.cy/sizev.cy);

	GetViewportOrgEx(hDC, & origin);
	Translate( (FLOAT) origin.x, (FLOAT) origin.y);

	return TRUE;
}


void minmax(int x0, int x1, int x2, int x3, int & minx, int & maxx)
{
	if ( x0<x1 )
	{	
		minx = x0; maxx = x1;	
	}
	else
	{	
		minx = x1; maxx = x0;
	}

	if ( x2<minx) 
		minx = x2; 
	else if ( x2>maxx) 
		maxx = x2;

	if ( x3<minx) 
		minx = x3; 
	else if ( x3>maxx) 
		maxx = x3;
}
