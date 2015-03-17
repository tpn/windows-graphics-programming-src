#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : fonttext.h				                                             //
//  Description: Font &  text routines and classes                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

// convert point size to logical coordinate space size
int PointSizetoLogical(HDC hDC, int points, int divisor=1);

// Create a font as large as EM square size for accurate metrics
HFONT CreateReferenceFont(HFONT hFont, int & emsquare);

// justify text string within a left..right margin
BOOL TextOutJust(HDC hDC, int left, int right, int y, LPCTSTR lpStr, int nCount, bool bAllowNegative=false, TCHAR cBreakChar=' ');


// ABC extent of a text string
// ( A0, B0, C0 ) + ( A1, B1, C1 ) = ( A0, B0+C0+A1+B1, C1 }
BOOL GetTextABCExtent(HDC hDC, LPCTSTR lpString, int cbString, long * pHeight, ABC * pABC);

BOOL GetOpaqueBox(HDC hDC, LPCTSTR lpString, int cbString, RECT * pRect, int x, int y);

// Pixel-level precise text alignment
BOOL PreciseTextOut(HDC hDC, int x, int y, LPCTSTR lpString, int cbString);


class KLogFont
{
public:
	LOGFONT m_lf;

	KLogFont(int height, const TCHAR * typeface=NULL)
	{
		m_lf.lfHeight         = height;
		m_lf.lfWidth          = 0;
		m_lf.lfEscapement     = 0;
		m_lf.lfOrientation    = 0;
		m_lf.lfWeight         = FW_NORMAL;
		m_lf.lfItalic         = FALSE;
		m_lf.lfUnderline      = FALSE;
		m_lf.lfStrikeOut      = FALSE;
		m_lf.lfCharSet        = ANSI_CHARSET;
		m_lf.lfOutPrecision   = OUT_TT_PRECIS;
		m_lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
		m_lf.lfQuality        = DEFAULT_QUALITY;
		m_lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

		if ( typeface )
			_tcsncpy(m_lf.lfFaceName, typeface, LF_FACESIZE-1);
		else
			m_lf.lfFaceName[0] = 0;
	}

	HFONT CreateFont(void)
	{
		return ::CreateFontIndirect(& m_lf);
	}

	int GetObject(HFONT hFont)
	{
		return ::GetObject(hFont, sizeof(m_lf), & m_lf);
	}
};


template <int maxlength>
class KPlacement
{
public:
	GCP_RESULTS m_gcp;

	TCHAR		m_strOut[maxlength];
	int			m_dx    [maxlength];
	unsigned	m_order [maxlength];
	int			m_caret [maxlength];
	char		m_class [maxlength];
	WORD		m_glyphs[maxlength];

	DWORD GetPlacement(HDC hDC, const TCHAR * mess, DWORD flags=0, int maxextent=0)
	{
		memset(&m_gcp, 0, sizeof(m_gcp));
	
		m_gcp.lStructSize = sizeof(GCP_RESULTS);
		m_gcp.lpOutString = m_strOut;
		m_gcp.lpOrder     = m_order;
		m_gcp.lpDx        = m_dx;
		m_gcp.lpCaretPos  = m_caret;
		m_gcp.lpClass     = m_class;
		m_gcp.lpGlyphs    = m_glyphs;
		m_gcp.nGlyphs     = maxlength;

		return GetCharacterPlacement(hDC, mess, _tcslen(mess), maxextent, & m_gcp, flags);
	}

	BOOL GlyphTextOut(HDC hDC, int x, int y)
	{
		return ExtTextOut(hDC, x, y, ETO_GLYPH_INDEX, NULL, (LPCTSTR) m_glyphs, m_gcp.nGlyphs, m_dx);
	}
};


class KKerningPair
{
public:
	KERNINGPAIR * m_pKerningPairs;
	int			  m_nPairs;

	KKerningPair(HDC hDC)
	{
		m_nPairs = GetKerningPairs(hDC, 0, NULL);

		if ( m_nPairs==0 )
			m_pKerningPairs = NULL;
		else
		{
			m_pKerningPairs = new KERNINGPAIR[m_nPairs];
			assert(m_pKerningPairs);

			GetKerningPairs(hDC, m_nPairs, m_pKerningPairs);
		}
	}

	int GetKerning(TCHAR first, TCHAR second)
	{
		for (int i=0; i<m_nPairs; i++)
			if ( (m_pKerningPairs[i].wFirst ==first) && 
				 (m_pKerningPairs[i].wSecond==second) )
				 return m_pKerningPairs[i].iKernAmount;

		return 0;
	}

	~KKerningPair(void)
	{
		if ( m_pKerningPairs )
		{
			delete m_pKerningPairs;
			m_pKerningPairs = NULL;
		}
	}
};


class KGlyph
{
public:
	GLYPHMETRICS  m_metrics;
	BYTE *        m_pPixels;
	DWORD		  m_nAllocSize;
	DWORD		  m_nDataSize;
	int			  m_uFormat;

	KGlyph()
	{
		m_pPixels = NULL;
	}

	~KGlyph(void);

	DWORD GetGlyph(HDC hDC, UINT uChar, UINT uFormat, const MAT2 * pMat2=NULL);
	
	BOOL  DrawGlyphROP(HDC HDC, int x, int y, DWORD rop, COLORREF crBack, COLORREF crFore);
	BOOL  DrawGlyph(HDC HDC, int x, int y, int & dx, int & dy);
};


template <int MAX_POINTS>
class KGlyphOutline
{
public:
	POINT m_Point[MAX_POINTS];
	BYTE  m_Flag [MAX_POINTS];

	int   m_nPoints;

private:
	void AddPoint(int x, int y, BYTE flag)
	{
		if ( m_nPoints < MAX_POINTS )
		{
			m_Point[m_nPoints].x = x;
			m_Point[m_nPoints].y = y;
			m_Flag [m_nPoints]   = flag;
			m_nPoints ++;
		}
		else
			assert(false);
	}

	void AddQSpline(int x1, int y1, int x2, int y2)
	{
		assert(m_nPoints);

		// p0 p1 p2 -> p0 (p0+2p1)/3 (2p1+p2)/3, p2
		int x0 = m_Point[m_nPoints-1].x;
		int y0 = m_Point[m_nPoints-1].y;

		AddCSpline((x0+2*x1)/3, (y0+2*y1)/3, (2*x1+x2)/3, (2*y1+y2)/3, x2, y2);
	}

	void AddCSpline(int x1, int y1, int x2, int y2, int x3, int y3)
	{
		assert(m_nPoints);

		AddPoint(x1, y1, PT_BEZIERTO);
		AddPoint(x2, y2, PT_BEZIERTO);
		AddPoint(x3, y3, PT_BEZIERTO);
	}

	void MarkLast(BYTE flag)
	{
		assert(m_nPoints);
		m_Flag[m_nPoints-1] |= flag;
	}

	void Transform(int dx, int dy)
	{
		for (int i=0; i<m_nPoints; i++)
		{
			m_Point[i].x = (   m_Point[i].x + 32768 ) / 65536 + dx;
			m_Point[i].y = ( - m_Point[i].y + 32768 ) / 65536 + dy;
		}
	}

public:
	int DecodeTTPolygon(const TTPOLYGONHEADER * lpHeader, int size);

	BOOL Draw(HDC hDC, int x, int y)
	{
		Transform(x, y);
	
		return PolyDraw(hDC, m_Point, m_Flag, m_nPoints);
	}

	int DecodeOutline(KGlyph & glyph)
	{
		if ( (glyph.m_uFormat==GGO_NATIVE) || (glyph.m_uFormat==GGO_BEZIER) )
			return DecodeTTPolygon((const TTPOLYGONHEADER *) glyph.m_pPixels, glyph.m_nDataSize);
		else
			return 0;
	}

};


template <int MAX_POINTS>
int KGlyphOutline<MAX_POINTS>::DecodeTTPolygon(const TTPOLYGONHEADER * pHeader, int size)
{
	m_nPoints = 0;

	while ( size>sizeof(TTPOLYGONHEADER) )
    {
		int curvesize = pHeader->cb;

		size -= curvesize;

		if ( size<0 )
		{
			assert(false);
			break;
		}

		if (pHeader->dwType != TT_POLYGON_TYPE)
		{
			assert(false);
			break;
		}

		const POINT * p = (const POINT *) & pHeader->pfxStart;

		AddPoint(p[0].x, p[0].y, PT_MOVETO);

		const TTPOLYCURVE * pCurve = (const TTPOLYCURVE *) & pHeader[1];
		
		while ( (unsigned) pCurve < ( ((unsigned) pHeader) + curvesize) )
		{
			int i;

			p = (const POINT *) pCurve->apfx;

			switch ( pCurve->wType )
			{
				case TT_PRIM_LINE:		// polyline. 
					for (i=0; i<pCurve->cpfx; i++)
						AddPoint(p[i].x, p[i].y, PT_LINETO);
					break;

				case TT_PRIM_QSPLINE:	// quadratic Bézier spline.
					assert(pCurve->cpfx>=2);

					for (i=0; i<(pCurve->cpfx-1); i++)
						if ( i==(pCurve->cpfx-2) )
							AddQSpline(p[i].x, p[i].y, p[i+1].x, p[i+1].y);
						else
							AddQSpline(
								p[i].x, 
								p[i].y, 
								( p[i].x + p[i+1].x ) / 2, 
								( p[i].y + p[i+1].y ) / 2);
					break;

				case TT_PRIM_CSPLINE:	// cubic Bezier spline
					assert((pCurve->cpfx % 3) ==0);
					for (i=0; i<pCurve->cpfx; i+=3)
						AddCSpline(p[i].x, p[i].y, p[i+1].x, p[i+1].y, p[i+2].x, p[i+2].y);
					break;

				default:
					assert(false);
					return 0;
			}

			pCurve = (const TTPOLYCURVE *) & pCurve->apfx[pCurve->cpfx];
		}

		MarkLast(PT_CLOSEFIGURE);

		pHeader = (const TTPOLYGONHEADER *) ((unsigned) pHeader + curvesize);
    }

	return m_nPoints;
}

BOOL BitmapTextOut(HDC hDC, int x, int y, const TCHAR * str, int count, int format = GGO_BITMAP);
BOOL BitmapTextOutROP(HDC hDC, int x, int y, const TCHAR * str, int count, DWORD rop);

BOOL OutlineTextOut(HDC hDC, int x, int y, const TCHAR * str, int count);

///////////////////////////////////////////////////////////

class KTextFormator
{
	typedef enum { MaxCharNo = 256 };

	float m_fCharWidth[MaxCharNo];
	float m_fHeight;
	float m_fLinespace;

public:

	BOOL  SetupPixel(HDC hDC, HFONT hFont, float pixelsize);
	BOOL  Setup(HDC hDC, HFONT hFont, float pointsize);

	float GetLinespace(void) const
	{
		return m_fLinespace;
	}

	BOOL  GetTextExtent(HDC hdc, LPCTSTR pString, int cbString, float & width, float & height);
	BOOL  TextOut(HDC hDC, int x, int y, LPCTSTR pString, int nCount);
	DWORD DrawText(HDC hDC, LPCTSTR pString, int nCount, const RECT * pRect, UINT uFormat);
};


BOOL ColorText(HDC hDC, int x, int y, LPCTSTR pString, int nCount, HBRUSH hFore);
 BOOL BitmapText(HDC hDC, int x, int y, LPCTSTR pString, int nCount, HBITMAP hBmp);
BOOL BitmapText2(HDC hDC, int x, int y, LPCTSTR pString, int nCount, HBITMAP hBmp);
BOOL OffsetTextOut(HDC hDC, int x, int y, LPCTSTR pStr, int nCount, 
				   int dx1, int dy1, COLORREF cr1,
				   int dx2, int dy2, COLORREF cr2);

BOOL DrawChar(HDC hDC, double x0, double y0, double x1, double y1, TCHAR ch);
void PathTextOut(HDC hDC, LPCTSTR pString, POINT point[], int no);
BOOL PathTextOut(HDC hDC, LPCTSTR pString);


// 3x3 average filter for any 8-bpp, 24-bpp, or 32-bpp frame buffer
template <int bytepp>
void Average(BYTE * pBits, int pitch, int width, int height)
{
	BYTE * buffer = new BYTE[pitch*2];

	for (int y=1; y<height; y++)
	{
		pBits += pitch; // first channel of pixel 0

		if ( y<(height-1) )
		{
			BYTE * P = pBits + bytepp;				// pixel 1
			BYTE * B = buffer + pitch * (y & 1);	// pixel 1..n-2	

			for (int x=1; x<(width-1); x++)
			{
				for (int b=0; b<bytepp; b++)
				{
					B[0] = ( P[-pitch-bytepp] + P[-pitch] + P[-pitch+bytepp] +
							 P[      -bytepp] + P[     0] + P[       bytepp] +
							 P[ pitch-bytepp] + P[ pitch] + P[ pitch+bytepp])/9;

					P ++;
					B ++;
				}
			}
		}

		if ( y>=2 )
		{
			memcpy(pBits-pitch+bytepp, buffer + pitch * ( 1 - (y & 1)), (width-2) * bytepp);
		}
	}

	delete [] buffer;
}


// Conver text string to bitmap
class KTextBitmap
{
public:
	
	HBITMAP		m_hBitmap;
	HDC		    m_hMemDC;
	HGDIOBJ		m_hOldBmp;
	int			m_width;
	int			m_height;
	int			m_dx;
	int			m_dy;
	BYTE *      m_pBits;

	BOOL Convert(HDC hDC, LPCTSTR pString, int nCount, int extra);

	void ReleaseBitmap(void)
	{
		if ( m_hOldBmp )
		{
			SelectObject(m_hMemDC, m_hOldBmp);
			m_hOldBmp = NULL;
		}

		if ( m_hBitmap )
		{
			DeleteObject(m_hBitmap);
			m_hBitmap = NULL;
		}
	}

	KTextBitmap()
	{
		m_hBitmap = NULL;
		m_hMemDC  = NULL;
		m_hOldBmp = NULL;
	}

	~KTextBitmap()
	{
		if ( m_hMemDC )
		{
			DeleteObject(m_hMemDC);
			m_hMemDC = NULL;
		}
	}

	void Blur(void);

	BOOL Draw(HDC hDC, int x, int y, int rop=SRCCOPY)
	{
		return BitBlt(hDC, x - m_dx, y - m_dy, m_width, m_height,
			          m_hMemDC, 0, 0, rop);
	}

	BOOL RenderText(HDC hDC, int x, int y, const TCHAR * pString, int nCount);
};

// Embossing or Engraving by change edges only, good for non-solod background
void TransparentEmboss(HDC hDC, const TCHAR * pString, int nCount, COLORREF crTL, COLORREF crBR, int offset, int x, int y);
	

