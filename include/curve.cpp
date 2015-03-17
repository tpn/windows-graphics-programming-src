//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : curve.cpp						                                     //
//  Description: Path, curve transformation, styled curve                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define  STRICT
#define  WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <math.h>
#include <assert.h>
#include <tchar.h>

#include "curve.h"
#include "pen.h"

int KPathData::GetPathData(HDC hDC)
{
	if ( m_pPoint ) delete m_pPoint;
	if ( m_pFlag  ) delete m_pFlag;

	m_nCount = ::GetPath(hDC, NULL, NULL, 0);

	if ( m_nCount>0 )
	{
		m_pPoint = new POINT[m_nCount];
		m_pFlag  = new  BYTE[m_nCount];

		if ( m_pPoint!=NULL && m_pFlag!=NULL )
			m_nCount = ::GetPath(hDC, m_pPoint, m_pFlag, m_nCount);
	}

	return m_nCount;
}


void KPathData::MapPoints(K2DMap & map)
{
	for (int i=0; i<m_nCount; i++)
		map.Map(m_pPoint[i].x, m_pPoint[i].y);
}


BOOL KTransCurve::MoveTo(HDC hDC, int x, int y)
{
	m_orgx = x;
	m_orgy = y;

	x0 = (float) x;
	y0 = (float) y;

	Map(x0, y0, m_dstx, m_dsty);

	return DrvMoveTo(hDC, (int) (m_dstx+0.5), (int) (m_dsty+0.5));
}


BOOL KTransCurve::BezierTo(HDC hDC, float x1, float y1, float x2, float y2, float x3, float y3)
{
	float dx3, dy3;

	Map(x3, y3, dx3, dy3);

	if ( ( fabs(dx3-m_dstx) + fabs(dy3-m_dsty) ) < m_seglen ) // no need to break ?
	{
		float dx1, dy1, dx2, dy2;
		
		Map(x1, y1, dx1, dy1);
		Map(x2, y3, dx2, dy2);

		POINT P[3] = {  (int) (dx1+0.5), (int) (dy1+0.5),
						(int) (dx2+0.5), (int) (dy2+0.5),
						(int) (dx3+0.5), (int) (dy3+0.5) };

		x0 = x3;
		y0 = y3;
		m_dstx = dx3;
		m_dsty = dy3;

		return DrvBezierTo(hDC, P);
	}
	else
	{
		BezierTo(hDC, 
			(x0+x1)          /2, (y0+y1)          /2, 
			(x0+x1*2+x2)     /4, (y0+y1*2+y2)     /4, 
			(x0+x1*3+x2*3+x3)/8, (y0+y1*3+y2*3+y3)/8);

		return BezierTo(hDC, 
			(x1+x2*2+x3)/4, (y1+y2*2+y3)/4, 
			(x2+x3)     /2, (y2+y3)     /2, 
			x3, y3);
	}
}
	

BOOL KPathData::Draw(HDC hDC, KTransCurve & trans, bool bPath)
{
	if ( m_nCount==0 )
		return FALSE;

	if ( bPath )
		BeginPath(hDC);

	for (int i=0; i<m_nCount; i++)
	{
		switch ( m_pFlag[i] & ~ PT_CLOSEFIGURE )
		{
			case PT_MOVETO:
				trans.MoveTo(hDC, m_pPoint[i].x, m_pPoint[i].y);
				break;

			case PT_LINETO:
				trans.LineTo(hDC, m_pPoint[i].x, m_pPoint[i].y);
				break;

			case PT_BEZIERTO:
				trans.BezierTo(hDC, 
					m_pPoint[i  ].x, m_pPoint[i  ].y,
					m_pPoint[i+1].x, m_pPoint[i+1].y,
					m_pPoint[i+2].x, m_pPoint[i+2].y);
				i+=2;
				break;

			default:
				assert(false);
		}

		if ( m_pFlag[i] & PT_CLOSEFIGURE )
			trans.CloseFigure(hDC);
	}

	if ( bPath )
		EndPath(hDC);

	return TRUE;
}


BOOL KStyleCurve::LineTo(double x, double y)
{
	double x2 = x;
	double y2 = y;

	double curlen = sqrt((x2-m_x1)*(x2-m_x1) + 
		                 (y2-m_y1)*(y2-m_y1));

	double length = m_pDash->GetLength(m_step);

	while ( curlen >= length )
	{
		double x1 = m_x1; 
		double y1 = m_y1;

		m_x1 += (x2-m_x1) * length / curlen;
		m_y1 += (y2-m_y1) * length / curlen;

		if ( ! m_pDash->DrawDash(x1, y1, m_x1, m_y1, m_step) )
			return FALSE;

		curlen -= length;

		m_step ++;
		length = m_pDash->GetLength(m_step);
	}

	return TRUE;
}


BOOL KStyleCurve::PolyDraw(const POINT *ppt, const BYTE *pbTypes, int nCount)
{
	int lastmovex = 0;
	int lastmovey = 0;

	for (int i=0; i<nCount; i++)
	{
		switch ( pbTypes[i] & ~ PT_CLOSEFIGURE )
		{
			case PT_MOVETO:
				m_x1 = lastmovex = ppt[i].x;
				m_y1 = lastmovey = ppt[i].y;
				break;

			case PT_LINETO:
				if ( ! LineTo(ppt[i].x, ppt[i].y) )
					return FALSE;
				break;
		}

		if ( pbTypes[i] & PT_CLOSEFIGURE )
			if ( ! LineTo(lastmovex, lastmovey) )
				return FALSE;
	}

	return TRUE;
}


void KPathData::MarkPoints(HDC hDC, bool bShowLine)
{
	if ( bShowLine )
	{
		KPen dot(PS_DOT, 0, RGB(0xFF, 0, 0), hDC);
		
		Polyline(hDC, m_pPoint, m_nCount);
	}

	KPen solid(PS_SOLID, 0, RGB(0, 0, 0xFF));
	solid.Select(hDC);

	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0xFF));

	for (int i=0; i<m_nCount; i++)
	{
#ifdef _DEBUG
		TCHAR temp[32];

		wsprintf(temp, _T("%3d %d (%d,%d)\n"), i, m_pFlag[i], m_pPoint[i].x, m_pPoint[i].y);
		OutputDebugString(temp);
#endif
		
		int x = m_pPoint[i].x;
		int y = m_pPoint[i].y;

		if ( m_pFlag[i] & PT_CLOSEFIGURE )
			SelectObject(hDC, hBrush);
		else
			SelectObject(hDC, GetStockObject(WHITE_BRUSH));

		switch ( m_pFlag[i] & ~ PT_CLOSEFIGURE )
		{
			case PT_MOVETO:
			{
					POINT P[] = { x, y-6, x+6, y+4, x-6, y+4, x, y-6 };

					Polygon(hDC, P, 4);
				}
				break;

			case PT_LINETO:
				Rectangle(hDC, x-5, y-5, x+5, y+5);
				break;

			case PT_BEZIERTO:
				Ellipse(hDC, x-5, y-5, x+5, y+5);
				break;
		}

	}

	SelectObject(hDC, GetStockObject(WHITE_BRUSH));
	DeleteObject(hBrush);

	solid.UnSelect();
}


BOOL KDashes::DrawDash(double x1, double y1, double x2, double y2, int step)
{
	HBRUSH hBrush = CreateSolidBrush(PALETTEINDEX(step % 20));
	HGDIOBJ hOld  = SelectObject(m_hDC, hBrush);

	SelectObject(m_hDC, GetStockObject(NULL_PEN));
	double dy = (x2-x1)/2;
	double dx = (y2-y1)/2;

	switch ( m_test & 3 )
	{
		case 0: // diamound
		{
			POINT P[5] = { (int)x1, (int)y1, (int)((x1+x2)/2-dx), (int)((y1+y2)/2+dy),
						   (int)x2, (int)y2, (int)((x1+x2)/2+dx), (int)((y1+y2)/2-dy),
						   (int)x1, (int)y1 };
			Polygon(m_hDC, P, 5);
			break;
		}

		case 1: // triangle
		{
			dx /= 0.866; // sqrt(3)/2
			dy /= 0.866;

			POINT P[4] = { (int)(x1-dx), (int)(y1+dy), (int)x2, (int)y2,
				           (int)(x1+dx), (int)(y1-dy), (int)(x1-dx), (int)(y1+dy)
		};
		Polygon(m_hDC, P, 4);
		break;
		}

		case 2: // circle
		{
			double r = sqrt(dx * dx + dy * dy);

			Ellipse(m_hDC, (int)((x1+x2)/2-r), (int)((y1+y2)/2-r),
				           (int)((x1+x2)/2+r), (int)((y1+y2)/2+r));
			break;
		}

		case 3: 
		{
			POINT P[5] = { (int)(x1-dx), (int)(y1+dy), (int)(x2-dx), (int)(y2+dy),
				           (int)(x2+dx), (int)(y2-dy), (int)(x1+dx), (int)(y1-dy),
						   (int)(x1-dx), (int)(y1+dy)
			};
			Polygon(m_hDC, P, 5);
			break;
		}
	}
	
	SelectObject(m_hDC, hOld);
	DeleteObject(hBrush);
	return TRUE;
}

