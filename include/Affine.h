#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : affine.h						                                     //
//  Description: Affine transformation                                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

// eM11 eM12
// eM21 eM22
// eDx  eDy
// x' =	m11 * x + m21 * y + dx
// y' = m12 * x + m22 * y + dy

class KAffine
{
public:
	XFORM m_xm;

	KAffine()
	{
		Reset();
	}

	void Map(int x, int y, float & rx, float & ry)
	{
		rx = m_xm.eM11 * x + m_xm.eM21 * y + m_xm.eDx;
		ry = m_xm.eM12 * x + m_xm.eM22 * y + m_xm.eDy;
	}
	
	void Reset();
	BOOL SetTransform(const XFORM & xm);
	BOOL Combine(const XFORM & b);
	BOOL Invert(void);

	BOOL Translate(FLOAT dx, FLOAT dy);
	BOOL Scale(FLOAT sx, FLOAT dy);
	BOOL Rotate(FLOAT angle, FLOAT x0=0, FLOAT y0=0);

	BOOL MapTri(FLOAT px0, FLOAT py0, FLOAT qx0, FLOAT qy0, FLOAT rx0, FLOAT ry0);

	BOOL MapTri(FLOAT px0, FLOAT py0, FLOAT qx0, FLOAT qy0, FLOAT rx0, FLOAT ry0,
				FLOAT px1, FLOAT py1, FLOAT qx1, FLOAT qy1, FLOAT rx1, FLOAT ry1);

	BOOL MapTriInt(int px0, int py0, int qx0, int qy0, int rx0, int ry0,
				int px1, int py1, int qx1, int qy1, int rx1, int ry1)
	{
		return MapTri((float) px0, (float) py0, (float) qx0, (float) qy0, (float) rx0, (float) ry0,
					  (float) px1, (float) py1, (float) qx1, (float) qy1, (float) rx1, (float) ry1);
	}

	BOOL GetDPtoLP(HDC hDC);
};


void minmax(int x0, int x1, int x2, int x3, int & minx, int & maxx);

class KReverseAffine : public KAffine
{
	int x0, y0, x1, y1, x2, y2;

public:
	int minx, maxx, miny, maxy;

	KReverseAffine(const POINT * pPoint)
	{
		x0 = pPoint[0].x;	//   P0       P1
		y0 = pPoint[0].y;	//
		x1 = pPoint[1].x;	//
		y1 = pPoint[1].y;	//   P2       P3
		x2 = pPoint[2].x;
		y2 = pPoint[2].y;
	}

	bool Simple(void) const
	{
		return (y0==y1) && (x0==x2);
	}

	void Setup(int nXSrc, int nYSrc, int nWidth, int nHeight)
	{
		MapTriInt(x0,    y0,	  x1,           y1,    x2,    y2, 
					nXSrc, nYSrc, nXSrc+nWidth, nYSrc, nXSrc, nYSrc+nHeight);
		
		minmax(x0, x1, x2, x2 + x1 - x0, minx, maxx);
		minmax(y0, y1, y2, y2 + y1 - y0, miny, maxy);
	}
};
