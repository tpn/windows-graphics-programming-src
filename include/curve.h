#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : curve.h						                                     //
//  Description: Path data, curve transformation, styled curve                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KDash
{
public:
	virtual double GetLength(int step)
	{
		return 10;
	}

	virtual BOOL DrawDash(double x1, double y1, double x2, double y2, int step)
	{
		return FALSE;
	}
};



class K2DMap
{
public:
	virtual Map(long & px, long & py) = 0;
};


class KBiLinearMap : public K2DMap
{
	double x00, y00, x01, y01, x10, y10, x11, y11;
	double orgx, orgy, width, height;

public:
	void SetWindow(int x, int y, int w, int h)
	{
		orgx   = x;
		orgy   = y;
		width  = w;
		height = h;
	}

	void SetDestination(POINT P[])
	{
		x00 = P[0].x; y00 = P[0].y;
		x01 = P[1].x; y01 = P[1].y;
		x10 = P[2].x; y10 = P[2].y;
		x11 = P[3].x; y11 = P[3].y;
	}

	virtual Map(long & px, long & py)
	{
		double x = (px - orgx ) / width;
		double y = (py - orgy ) / height;

		px = (long) ( (1-x) * ( x00 * (1-y) + x01 * y ) +
			              x * ( x10 * (1-y) + x11 * y ) );

		py = (long) ( (1-x) * ( y00 * (1-y) + y01 * y ) +
			              x * ( y10 * (1-y) + y11 * y ) );
	}
};


class KTransCurve
{
	int   m_orgx; // first point in a figure
	int   m_orgy;

	float x0;	  // current last point	
	float y0;
	float m_dstx;
	float m_dsty;
	int   m_seglen; // segment length to break

	virtual Map(float x, float y, float & rx, float & ry)
	{
		rx = x;
		ry = y;
	}

	virtual BOOL DrvLineTo(HDC hDC, int x, int y)
	{
		return ::LineTo(hDC, x, y);
	}

	virtual BOOL DrvMoveTo(HDC hDC, int x, int y)
	{
		return ::MoveToEx(hDC, x, y, NULL);
	}

	virtual BOOL DrvBezierTo(HDC hDC, POINT p[])
	{
		return ::PolyBezierTo(hDC, p, 3);
	}	

	BOOL BezierTo(HDC hDC, float x1, float y1, float x2, float y2, float x3, float y3);
	
public:

	KTransCurve(int seglen)
	{
		m_seglen = seglen;
	}

	BOOL   MoveTo(HDC hDC, int x, int y);

	BOOL BezierTo(HDC hDC, int x1, int y1, int x2, int y2, int x3, int y3)
	{
		return BezierTo(hDC, (float) x1, (float) y1, (float) x2, (float) y2, (float) x3, (float) y3);
	}
	
	BOOL CloseFigure(HDC hDC)
	{
		LineTo(hDC, m_orgx, m_orgy );

		return ::CloseFigure(hDC);
	}

	BOOL LineTo(HDC hDC, int x3, int y3)
	{
		return BezierTo(hDC, (x0*2+x3)/3, (y0*2+y3)/3, (x0+x3*2)/3, (y0+y3*2)/3, (float) x3, (float) y3);
	}
};


class KPathData
{
public:
	POINT * m_pPoint;
	BYTE  * m_pFlag;
	int     m_nCount;

	KPathData()
	{
		m_pPoint = NULL;
		m_pFlag  = NULL;
		m_nCount = 0;
	}

	~KPathData()
	{
		if ( m_pPoint ) delete m_pPoint;
		if ( m_pFlag  ) delete m_pFlag;
	}

	int  GetPathData(HDC hDC);
	void MarkPoints(HDC hDC, bool bShowLine=true);
	void MapPoints(K2DMap & map);

	BOOL Draw(HDC hDC, KTransCurve & transformer, bool bPath);
};


class KStyleCurve
{
	int		 m_step;
	double	 m_x1, m_y1;
	KDash  * m_pDash;

public:

	KStyleCurve(KDash * pDash)
	{
		m_x1   = 0;
		m_y1   = 0;
		m_step = 0;
		m_pDash= pDash;
	}

	BOOL MoveTo(double x, double y);
	BOOL LineTo(double x, double y);
	BOOL PolyDraw(const POINT *ppt, const BYTE *pbTypes, int nCount);
};


class KDashes : public KDash
{
	HDC    m_hDC;
	int    m_test;

	virtual double GetLength(int step)
	{
		if ( step & 1)
			return 5+m_test;
		else
			return 5+m_test*2;
	}

	virtual BOOL DrawDash(double x1, double y1, double x2, double y2, int step);
	
public:
	
	KDashes(HDC hDC, int test)
	{
		m_hDC   = hDC;
		m_test  = test;
	}
};


