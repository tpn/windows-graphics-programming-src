//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : fonttext.cpp						                                 //
//  Description: Generic font and text routines and classes                          //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#pragma pack(push, 4)
#include <windows.h>
#pragma pack(pop)

#include <tchar.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "fonttext.h"

// convert point size to logical coordinate space size
int PointSizetoLogical(HDC hDC, int points, int divisor)
{
	POINT P[2] = // two POINTs in device space whose distance is the needed height
	{
		{ 0, 0 },
		{ 0, ::GetDeviceCaps(hDC, LOGPIXELSY) * points } 
	};

	DPtoLP(hDC, P, 2); // map device coordinate to logical size
	
	return abs(P[1].y - P[0].y) / 72 / divisor;
}


// Create a font as large as EM square size for accurate metrics
HFONT CreateReferenceFont(HFONT hFont, int & emsquare)
{
	LOGFONT           lf;
	OUTLINETEXTMETRIC otm[3]; // big enough for the strings

	HDC hDC      = GetDC(NULL);
	HGDIOBJ hOld = SelectObject(hDC, hFont);
	int size = GetOutlineTextMetrics(hDC, sizeof(otm), otm);
	SelectObject(hDC, hOld);
	ReleaseDC(NULL, hDC);

	if ( size )								// TrueType
	{
		GetObject(hFont, sizeof(lf), & lf);

		emsquare    = otm[0].otmEMSquare; // get EM square size
		lf.lfHeight = - emsquare;		  // font size for 1:1 mapping
		lf.lfWidth  = 0;				  // original proportion

		return CreateFontIndirect(&lf);
	}
	else
		return NULL;
}


// justify text string within a left..right margin
BOOL TextOutJust(HDC hDC, int left, int right, int y, LPCTSTR lpStr, int nCount, bool bAllowNegative, TCHAR cBreakChar)
{
	SIZE size;
	
	SetTextJustification(hDC, 0, 0);

	GetTextExtentPoint32(hDC, lpStr, nCount, & size);

	int nBreak = 0;
	for (int i=0; i<nCount; i++)
		if ( lpStr[i]==cBreakChar )
			nBreak ++;

	int breakextra = right - left - size.cx;
	
	if ( (breakextra<0) && ! bAllowNegative )
		breakextra =0;

	SetTextJustification(hDC, breakextra, nBreak);

	return TextOut(hDC, left, y, lpStr, nCount);
}


// ABC extent of a text string
// ( A0, B0, C0 ) + ( A1, B1, C1 ) = ( A0, B0+C0+A1+B1, C1 }
BOOL GetTextABCExtent(HDC hDC, LPCTSTR lpString, int cbString, long * pHeight, ABC * pABC)
{
	SIZE size;

	if ( ! GetTextExtentPoint32(hDC, lpString, cbString, & size) )
		return FALSE;

	* pHeight  = size.cy;
	pABC->abcB = size.cx;

	ABC abc; 
	GetCharABCWidths(hDC, lpString[0],          lpString[0],          & abc); // first
	pABC->abcB -= abc.abcA;
	pABC->abcA  = abc.abcA;

	GetCharABCWidths(hDC, lpString[cbString-1], lpString[cbString-1], & abc); // last
	pABC->abcB -= abc.abcC;
	pABC->abcC  = abc.abcC;

	return TRUE;
}


BOOL GetOpaqueBox(HDC hDC, LPCTSTR lpString, int cbString, RECT * pRect, int x, int y)
{
	long height;
	ABC  abc;

	if ( ! GetTextABCExtent(hDC, lpString, cbString, & height, & abc) )
		return FALSE;

	switch ( GetTextAlign(hDC) & (TA_LEFT | TA_RIGHT | TA_CENTER) )
	{
		case TA_LEFT    : break;
		case TA_RIGHT   : x -=   abc.abcB; break;
		case TA_CENTER  : x -= abc.abcB/2; break;
		default:		  assert(false);
	}

	switch ( GetTextAlign(hDC) & (TA_TOP | TA_BASELINE | TA_BOTTOM) )
	{
		case TA_TOP     : break;
		case TA_BOTTOM  : y = - height; break;
		case TA_BASELINE: 
			{
				TEXTMETRIC tm;
				GetTextMetrics(hDC, & tm);
				y = - tm.tmAscent;
			}
			break;
		default:		  assert(false);
	}

	pRect->left   = x + min(abc.abcA, 0);
	pRect->right  = x + abc.abcA + abc.abcB + max(abc.abcC, 0);
	pRect->top    = y;
	pRect->bottom = y + height;

	return TRUE;
}


// Pixel-level precise text alignment
BOOL PreciseTextOut(HDC hDC, int x, int y, LPCTSTR lpString, int cbString)
{
	long height;
	ABC  abc;

	if ( GetTextABCExtent(hDC, lpString, cbString, & height, & abc) )
		switch ( GetTextAlign(hDC) & (TA_LEFT | TA_RIGHT | TA_CENTER) )
		{
			case TA_LEFT  : x -= abc.abcA;    break;
			case TA_RIGHT : x += abc.abcC;    break;
			case TA_CENTER: x -= (abc.abcA-abc.abcC)/2;  break;
		}

	return TextOut(hDC, x, y, lpString, cbString);
}


KGlyph::~KGlyph(void)
{
	if ( m_pPixels )
	{
		delete [] m_pPixels;
		m_pPixels = NULL;
	}
}


DWORD KGlyph::GetGlyph(HDC hDC, UINT uChar, UINT uFormat, const MAT2 * pMat2)
{
	MAT2 mat2;

	if ( pMat2==NULL )
	{
		memset(&mat2, 0, sizeof(mat2));
		mat2.eM11.value = 1;
		mat2.eM22.value = 1;
		pMat2 = & mat2;
	}
	
	// query size
	m_nDataSize = GetGlyphOutline(hDC, uChar, uFormat, & m_metrics, 0, NULL, pMat2);

	if ( (m_nDataSize==0) || (m_nDataSize==GDI_ERROR) )
		return m_nDataSize;

	if ( m_pPixels && (m_nDataSize > m_nAllocSize) ) // deallocate if too small
	{
		delete m_pPixels;
		m_pPixels = NULL;
	}
		
	if ( m_pPixels==NULL )
	{
		m_pPixels = new BYTE[m_nDataSize];	// new buffer allocation
		assert(m_pPixels);
		m_nAllocSize   = m_nDataSize;
	}

	m_uFormat = uFormat;
	// real data
	m_nDataSize = GetGlyphOutline(hDC, uChar, uFormat, & m_metrics, m_nAllocSize, m_pPixels, pMat2);

	return m_nDataSize;
}


typedef struct
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD			 bmiColors[256];
}	BITMAPINFO8BPP;



BOOL KGlyph::DrawGlyphROP(HDC hDC, int x, int y, DWORD rop, COLORREF crBack, COLORREF crFore)
{
	int levels;

	switch (m_uFormat & 0x0F )
	{
		case GGO_BITMAP:       levels =  2; break;
		case GGO_GRAY2_BITMAP: levels =  5; break;
		case GGO_GRAY4_BITMAP: levels = 17; break;
		case GGO_GRAY8_BITMAP: levels = 65; break;

		default:
			return FALSE;
	}

	BITMAPINFO8BPP bmi;
	memset(& bmi, 0, sizeof(bmi));
	
	bmi.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth    =     m_metrics.gmBlackBoxX;
	bmi.bmiHeader.biHeight   = 0 - m_metrics.gmBlackBoxY; // top-down DIB
	bmi.bmiHeader.biPlanes   = 1;
	
	if ( levels==2 )
	{
		bmi.bmiHeader.biBitCount = 1;

		bmi.bmiColors[0].rgbRed   = GetRValue(crBack);
		bmi.bmiColors[0].rgbGreen = GetGValue(crBack);
		bmi.bmiColors[0].rgbBlue  = GetBValue(crBack);

		bmi.bmiColors[1].rgbRed   = GetRValue(crFore);
		bmi.bmiColors[1].rgbGreen = GetGValue(crFore);
		bmi.bmiColors[1].rgbBlue  = GetBValue(crFore);
	}
	else
	{
		bmi.bmiHeader.biBitCount = 8;
		
//		KColor back(crBack); back.ToHLS();
//		KColor fore(crFore); fore.ToHLS();
	
		for (int i=0; i<levels; i++)
		{
//			KColor m;

//			m.hue        = (back.hue        * (levels-i-1) + fore.hue        * i ) / (levels-1);
//			m.lightness  = (back.lightness  * (levels-i-1) + fore.lightness  * i ) / (levels-1);
//			m.saturation = (back.saturation * (levels-i-1) + fore.saturation * i ) / (levels-1);
//			m.ToRGB();

//			bmi.bmiColors[i].rgbRed   = m.red;
//			bmi.bmiColors[i].rgbGreen = m.green;
//			bmi.bmiColors[i].rgbBlue  = m.blue;

			bmi.bmiColors[i].rgbRed   = (GetRValue(crBack) * (levels-i-1) + GetRValue(crFore) * i ) / (levels-1);
			bmi.bmiColors[i].rgbGreen = (GetGValue(crBack) * (levels-i-1) + GetGValue(crFore) * i ) / (levels-1);
			bmi.bmiColors[i].rgbBlue  = (GetBValue(crBack) * (levels-i-1) + GetBValue(crFore) * i ) / (levels-1);
		}
	}

	return StretchDIBits(hDC, 
				x, y, m_metrics.gmBlackBoxX, m_metrics.gmBlackBoxY,
				0, 0, m_metrics.gmBlackBoxX, m_metrics.gmBlackBoxY,
				m_pPixels,
				(const BITMAPINFO *) & bmi,
				DIB_RGB_COLORS, rop);
}


// assuming TA_LEFT | TA_BASELINE alignment
BOOL KGlyph::DrawGlyph(HDC hDC, int x, int y, int & dx, int & dy)
{
	dx = m_metrics.gmCellIncX; // advancement
	dy = m_metrics.gmCellIncY;

	if ( GetBkMode(hDC)==OPAQUE ) // may overlap with next glyph
	{
		// black box is not big enough, need to calculate actual background box
		RECT opaque;

		TEXTMETRIC tm;
		GetTextMetrics(hDC, & tm);

		opaque.left   = min(x,                        x + m_metrics.gmptGlyphOrigin.x);
		opaque.right  = max(x + m_metrics.gmCellIncX, x + m_metrics.gmptGlyphOrigin.x + (int) m_metrics.gmBlackBoxX);
		opaque.top    = y - tm.tmAscent;
		opaque.bottom = y + tm.tmDescent;

		HBRUSH hBackBrush = CreateSolidBrush(GetBkColor(hDC));
		FillRect(hDC, & opaque, hBackBrush);
		DeleteObject(hBackBrush);
	}

	// if foreground color, use pen color, otherwise donot change desitnation

	HBRUSH hBrush = CreateSolidBrush(GetTextColor(hDC));
	HBRUSH hOld   = (HBRUSH) SelectObject(hDC, hBrush);

	BOOL rslt = DrawGlyphROP(hDC, 
					x + m_metrics.gmptGlyphOrigin.x, 
					y - m_metrics.gmptGlyphOrigin.y, 
					0xE20746,				// D^(S&(P^D)) -> if (S) P else D
					RGB(0,    0,    0),		// 0 -> 0
					RGB(0xFF, 0xFF, 0xFF));	// 1 -> 1

	SelectObject(hDC, hOld);
	DeleteObject(hBrush);

	return rslt;
}



FIXED MakeFixed(double value)
{
	long val = (long) (value * 65536);

	return * (FIXED *) & val;
}


BOOL OutlineTextOut(HDC hDC, int x, int y, const TCHAR * str, int count)
{
	if ( count<0 )
		count = _tcslen(str);

	KGlyph             glyph;
	KGlyphOutline<512> outline;

//	MAT2 mat2;
//	mat2.eM11 = MakeFixed(cos(0.1));
//	mat2.eM12 = MakeFixed(sin(0.1));
//	mat2.eM21 = MakeFixed(-sin(0.1));
//	mat2.eM22 = MakeFixed(cos(0.1));

	while ( count>0 )
	{
		if ( glyph.GetGlyph(hDC, * str, GGO_NATIVE/*, & mat2*/)>0 )
			if ( outline.DecodeOutline(glyph) )
				outline.Draw(hDC, x, y);

		x += glyph.m_metrics.gmCellIncX;
		y += glyph.m_metrics.gmCellIncY;

		str ++; 
		count --;
	}

	return TRUE;
}


BOOL BitmapTextOut(HDC hDC, int x, int y, const TCHAR * str, int count, int format)
{
	if ( count<0 )
		count = _tcslen(str);

	KGlyph glyph;

	while ( count>0 )
	{
		int dx=0, dy=0;

		if ( glyph.GetGlyph(hDC, * str, format)>0 )
			glyph.DrawGlyph(hDC, x, y, dx, dy);

		x += dx;
		y += dy;

		str ++; 
		count --;
	}

	return TRUE;
}


BOOL BitmapTextOutROP(HDC hDC, int x, int y, const TCHAR * str, int count, DWORD rop)
{
	if ( count<0 )
		count = _tcslen(str);

	KGlyph glyph;

	COLORREF crBack = GetBkColor(hDC);
	COLORREF crFore = GetTextColor(hDC);

	while ( count>0 )
	{
		if ( glyph.GetGlyph(hDC, * str, GGO_BITMAP)>0 )
			glyph.DrawGlyphROP(hDC, 
					x + glyph.m_metrics.gmptGlyphOrigin.x, 
					y - glyph.m_metrics.gmptGlyphOrigin.y, 
					rop, crBack, crFore);

		x += glyph.m_metrics.gmCellIncX;
		y += glyph.m_metrics.gmCellIncY;

		str ++; 
		count --;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////

// pixelsize: in logical space
BOOL KTextFormator::SetupPixel(HDC hDC, HFONT hFont, float pixelsize)
{
	OUTLINETEXTMETRIC otm[3]; // big enough for the strings

	if ( GetOutlineTextMetrics(hDC, sizeof(otm), otm)==0 )
		return FALSE;

	LOGFONT   lf;

	GetObject(hFont, sizeof(lf), & lf);

	int emsquare= otm[0].otmEMSquare; // get EM square size
	lf.lfHeight = - emsquare;		  // font size for 1:1 mapping
	lf.lfWidth  = 0;				  // original proportion

	HFONT hRefFont = CreateFontIndirect(&lf);

	HDC     hRefDC = CreateCompatibleDC(hDC);
	HGDIOBJ hOld   = SelectObject(hRefDC, hRefFont);
		
	int nCharWidth[MaxCharNo];

	GetCharWidth(hRefDC, 0, MaxCharNo-1, nCharWidth);
		
	if ( GetOutlineTextMetrics(hRefDC, sizeof(otm), otm)==0 )
		return FALSE;

	SelectObject(hRefDC, hOld);
		
	DeleteObject(hRefDC);
	DeleteObject(hRefFont);

	m_fHeight    = otm[0].otmTextMetrics.tmHeight * pixelsize / emsquare;
	m_fLinespace = ( otm[0].otmTextMetrics.tmHeight + otm[0].otmTextMetrics.tmExternalLeading) * pixelsize / emsquare;
	
	for (int i=0; i<MaxCharNo; i++)
		m_fCharWidth[i] = ((float) nCharWidth[i]) * pixelsize / emsquare;
		
	return TRUE;
}


BOOL KTextFormator::Setup(HDC hDC, HFONT hFont, float pointsize)
{
	return SetupPixel(hDC, hFont, pointsize * GetDeviceCaps(hDC, LOGPIXELSY) / 72);
}


BOOL KTextFormator::GetTextExtent(HDC hdc, LPCTSTR lpString, int cbString, float & width, float & height)
{
	if ( cbString<=0 )
		cbString = _tcslen(lpString);

	width = 0;
		
	for (int i=0; i<cbString; i++)
		width = width + m_fCharWidth[lpString[i]];

	height = m_fHeight;

	return TRUE;
}


class KLineBreaker
{
	LPCTSTR m_pText;
	int     m_nLength;
	int		m_nPos;

	BOOL SkipWhite(void)
	{
		// skip white space
		while ( (m_nPos<m_nLength) && (m_pText[m_nPos]<=' ') )
			m_nPos ++;

		return m_nPos < m_nLength;
	}

	// m_pText[m_nPos] is the starting of a word, find its end
	void GetWord(void)
	{
		while ( (m_nPos<m_nLength) && (m_pText[m_nPos]>' ') )
			m_nPos ++;
	}

	BOOL Breakable(int pos)
	{
		if ( m_pText[pos]<=' ') // space character is breakable
			return true;

		if ( pos && (m_pText[pos-1]<=' ') ) // having space before it
			return true;

		if ( pos && (m_pText[pos-1]=='-') ) // having hypen before it
			return true;

		return false;
	}

public:

	float textwidth, textheight;

	KLineBreaker(LPCTSTR pText, int nCount)
	{
		m_pText   = pText;
		m_nPos    = 0;

		if ( nCount<=0 )
			m_nLength = _tcslen(m_pText);
		else
			m_nLength = nCount;
	}

	BOOL GetLine(KTextFormator & formator, HDC hDC, int width, int & begin, int & end)
	{
		const float epsilon = (float) 0.001;

		if ( ! SkipWhite() )
			return FALSE;
	
		begin = m_nPos; // first no white space to diplay
		
		// add words until the line is too long
		while ( SkipWhite() )
		{
			// first end of word
			GetWord();

			formator.GetTextExtent(hDC, m_pText + begin, m_nPos - begin, textwidth, textheight);

			// break out if it's too long
			if ( textwidth >= (width-epsilon) )
				break;
		}

		if ( textwidth > width )
			for (int p=m_nPos-1; p>begin; p--) // find a place to break a word into two
				if ( Breakable(p) )
				{   // can we fit now ?
					formator.GetTextExtent(hDC, m_pText + begin, p - begin, textwidth, textheight);
					if ( textwidth<=(width+epsilon) )
					{
						m_nPos = p;
						break;
					}
				}

		end = m_nPos;

		return TRUE;
	}
};


BOOL KTextFormator::TextOut(HDC hDC, int x, int y, LPCTSTR pString, int nCount)
{
	int Dx[MAX_PATH];

	int lastx = 0;
	float sum = 0.0;

	int sumdx = 0;
	for (int i=0; i<nCount; i++)
	{
		sum = sum + m_fCharWidth[pString[i]];

		int newx = (int) (sum + 0.5);
		Dx[i] = newx - lastx;
		lastx = newx;

		sumdx += Dx[i];
	}

//	TCHAR temp[64];
//	sprintf(temp, "sum: %8.5f sumdx : %d\n", sum, sumdx);
//	OutputDebugString(temp);

	return ExtTextOut(hDC, x, y, 0, NULL, pString, nCount, Dx);
}


DWORD KTextFormator::DrawText(HDC hDC, LPCTSTR pString, int nCount, const RECT * pRect, UINT uFormat)
{
	if ( pString==NULL )
		return 0;

	KLineBreaker breaker(pString, nCount);

	int   x     = pRect->left;
	float y     = (float) pRect->top;
	int width = pRect->right - pRect->left;
	int begin, end;

	while ( breaker.GetLine(* this, hDC, width, begin, end) )
	{
		while ( (end>begin) && (pString[end-1]<=' ') )
			end --;

//		TCHAR mess[64];
//		sprintf(mess, "width %8.5f\n", breaker.textwidth);
//		OutputDebugString(mess);

		TextOut(hDC, x, (int)(y+0.5), pString + begin, end - begin);

		y += breaker.textheight;
	}
	
	return 0;
}


BOOL ColorText(HDC hDC, int x, int y, LPCTSTR pString, int nCount, HBRUSH hFore)
{
	HGDIOBJ hOld      = SelectObject(hDC, hFore);

	RECT rect;
	GetOpaqueBox(hDC, pString, nCount, & rect, x, y);
	PatBlt(hDC, rect.left, rect.top, 
		rect.right-rect.left, rect.bottom - rect.top, PATINVERT);

	int      oldBk    = SetBkMode(hDC, TRANSPARENT);
	COLORREF oldColor = SetTextColor(hDC, RGB(0, 0, 0));
	
	TextOut(hDC, x, y, pString, nCount);
	SetBkMode(hDC, oldBk);
	SetTextColor(hDC, oldColor);
	
	BOOL rslt = PatBlt(hDC, rect.left, rect.top, 
					rect.right-rect.left, rect.bottom - rect.top, PATINVERT);

	SelectObject(hDC, hOld);

	return rslt;
}


BOOL BitmapText(HDC hDC, int x, int y, LPCTSTR pString, int nCount, HBITMAP hBmp)
{
	RECT rect;
	GetOpaqueBox(hDC, pString, nCount, & rect, x, y);

	HDC hMemDC = CreateCompatibleDC(hDC);
	HGDIOBJ hOld = SelectObject(hMemDC, hBmp);

	BitBlt(hDC, rect.left, rect.top, 
				rect.right-rect.left, 
				rect.bottom - rect.top, hMemDC, 0, 0, SRCINVERT);

	int      oldBk    = SetBkMode(hDC, TRANSPARENT);
	COLORREF oldColor = SetTextColor(hDC, RGB(0, 0, 0));
	
	TextOut(hDC, x, y, pString, nCount);
	SetBkMode(hDC, oldBk);
	SetTextColor(hDC, oldColor);
	
	BOOL rslt = BitBlt(hDC, rect.left, rect.top, 
					rect.right-rect.left, rect.bottom - rect.top, 
					hMemDC, 0, 0, SRCINVERT);

	SelectObject(hMemDC, hOld);
	DeleteObject(hMemDC);

	return rslt;
}


BOOL BitmapText2(HDC hDC, int x, int y, LPCTSTR pString, int nCount, HBITMAP hBmp)
{
	RECT rect;
	GetOpaqueBox(hDC, pString, nCount, & rect, x, y);

	HDC hMemDC = CreateCompatibleDC(hDC);
	HGDIOBJ hOld = SelectObject(hMemDC, hBmp);

	BeginPath(hDC);
	SetBkMode(hDC, TRANSPARENT);
	TextOut(hDC, x, y, pString, nCount);
	EndPath(hDC);
	SelectClipPath(hDC, RGN_COPY);

	BOOL rslt = BitBlt(hDC, rect.left, rect.top, 
				rect.right-rect.left, 
				rect.bottom - rect.top, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOld);
	DeleteObject(hMemDC);

	return rslt;
}

BOOL OffsetTextOut(HDC hDC, int x, int y, LPCTSTR pStr, int nCount, 
				   int dx1, int dy1, COLORREF cr1,
				   int dx2, int dy2, COLORREF cr2)
{
	COLORREF cr = GetTextColor(hDC);
	int		 bk = GetBkMode(hDC);

	if ( bk==OPAQUE )
	{
		RECT rect;

		GetOpaqueBox(hDC, pStr, nCount, & rect, x, y);

		rect.left  += min(min(dx1, dx2), 0);
		rect.right += max(max(dx1, dx2), 0);
		rect.top   += min(min(dy1, dy2), 0);
		rect.bottom+= max(max(dy1, dy2), 0);

		ExtTextOut(hDC, x, y, ETO_OPAQUE, & rect, NULL, 0, NULL);
	}

	SetBkMode(hDC, TRANSPARENT);

	if ( (dx1!=0) || (dy1!=0) )
	{
		SetTextColor(hDC, cr1);
		TextOut(hDC, x + dx1, y + dy1, pStr, nCount);
	}

	if ( (dx1!=0) || (dy1!=0) )
	{
		SetTextColor(hDC, cr2);
		TextOut(hDC, x + dx2, y + dy2, pStr, nCount);
	}

	SetTextColor(hDC, cr);

	BOOL rslt = TextOut(hDC, x, y, pStr, nCount);
	SetBkMode(hDC, bk);

	return rslt;
}


double dis(double x0, double y0, double x1, double y1)
{
	x1 -= x0;
	y1 -= y0;

	return sqrt( x1 * x1 + y1 * y1 );
}

const double pi = 3.141592654;


BOOL DrawChar(HDC hDC, double x0, double y0, double x1, double y1, TCHAR ch)
{
	x1 -= x0;
	y1 -= y0;

	int escapement = 0;

	if ( (x1<0.01) && (x1>-0.01) )
		if ( y1>0 )
			escapement = 2700;
		else
			escapement =  900;
	else
	{
		double angle = atan(-y1/x1);
		
		escapement = (int) ( angle * 180 / pi * 10 + 0.5);

//		TCHAR temp[MAX_PATH];
//		sprintf(temp, "%8.5f %8.5f %8.5f %8.5f -> %d\n", x0, y0, x1, y1, escapement);
//		OutputDebugString(temp);
	}

	LOGFONT lf;
	GetObject(GetCurrentObject(hDC, OBJ_FONT), sizeof(lf), &lf);

	if ( lf.lfEscapement != escapement )
	{
		lf.lfEscapement = escapement;

		HFONT hFont = CreateFontIndirect(&lf);

		if ( hFont==NULL )
			return FALSE;

		DeleteObject(SelectObject(hDC, hFont));
	}
	
	TextOut(hDC, (int)x0, (int)y0, &ch, 1);

	return TRUE;
}


void PathTextOut(HDC hDC, LPCTSTR pString, POINT point[], int no)
{
//	MoveToEx(hDC, point[0].x, point[0].y, NULL);

//	for (int i=1; i<no; i++)
//		LineTo(hDC, point[i].x, point[i].y);

	double x0 = point[0].x;
	double y0 = point[0].y;

	for (int i=1; i<no; i++)
	{
		double x1 = point[i].x;
		double y1 = point[i].y;

		double curlen = dis(x0, y0, x1, y1);

		while ( true )
		{
			int length;
			GetCharWidth(hDC, * pString, * pString, & length);

			if ( curlen < length )
				break;

			double x00 = x0; 
			double y00 = y0;

			x0 += (x1-x0) * length / curlen;
			y0 += (y1-y0) * length / curlen;

			DrawChar(hDC, x00, y00, x0, y0, * pString);

			curlen -= length;
			pString ++;

			if ( * pString==0 )
			{
				i = no;
				break;
			}
		}
	}
}



BOOL PathTextOut(HDC hDC, LPCTSTR pString)
{
	if ( ! FlattenPath(hDC) ) // conver to polyline
		return FALSE;

	POINT * pp;
	BYTE  * pf;

	int no = GetPath(hDC, NULL, NULL, 0); // query point no

	if ( no<2 )	// at least two points
		return FALSE;

	pp = new POINT[no];
	pf = new  BYTE[no];

	no = GetPath(hDC, pp, pf, no); // real data

	PathTextOut(hDC, pString, pp, no); // aligning

	delete pp;
	delete pf;

	return TRUE;
}


void KTextBitmap::Blur(void)
{
	Average<4>(m_pBits, m_width*4, m_width, m_height);
}



BOOL KTextBitmap::RenderText(HDC hDC, int x, int y, const TCHAR * pString, int nCount)
{
	HGDIOBJ hOldFont = SelectObject(m_hMemDC, GetCurrentObject(hDC, OBJ_FONT));
	SetTextColor(m_hMemDC, GetTextColor(hDC));
	SetBkMode   (m_hMemDC, GetBkMode(hDC));
	SetBkColor  (m_hMemDC, GetBkColor(hDC));
	
	SetTextAlign(m_hMemDC, TA_LEFT | TA_TOP);
	BOOL rslt = TextOut(m_hMemDC, m_dx+x, m_dy+y, pString, nCount);
	
	SelectObject(m_hMemDC, hOldFont);

	return rslt;
}


BOOL KTextBitmap::Convert(HDC hDC, LPCTSTR pString, int nCount, int extra)
{
	RECT rect;

	ReleaseBitmap();

	SaveDC(hDC);

	SetTextAlign(hDC, TA_LEFT | TA_TOP);
	GetOpaqueBox(hDC, pString, nCount, & rect, 0, 0);

	m_width  = rect.right - rect.left + extra * 2;
	m_height = rect.bottom - rect.top + extra * 2;

	BITMAPINFOHEADER bmih;
	memset(& bmih, 0, sizeof(bmih));
	bmih.biSize     = sizeof(BITMAPINFOHEADER);
	bmih.biWidth    = m_width;
	bmih.biHeight   = m_height;
	bmih.biPlanes   = 1;
	bmih.biBitCount = 32;

	m_hBitmap = CreateDIBSection(hDC, (const BITMAPINFO *) & bmih, DIB_RGB_COLORS, (void **) & m_pBits, NULL, 0);
	m_hMemDC  = CreateCompatibleDC(hDC);
	m_hOldBmp = SelectObject(m_hMemDC, m_hBitmap);

	m_dx = extra - min(rect.left, 0);
	m_dy = extra;

	SetBkColor  (m_hMemDC, GetBkColor(hDC));

	rect.left   = 0;
	rect.top    = 0;
	rect.right  = m_width;
	rect.bottom = m_height;
	ExtTextOut(m_hMemDC, 0, 0, ETO_OPAQUE, & rect, NULL, 0, NULL);
	
	RenderText(hDC, 0, 0, pString, nCount);
	RestoreDC(hDC, -1);

	return TRUE;
}


// Embossing or Engraving by change edges only, good for non-solod background
void TransparentEmboss(HDC hDC, const TCHAR * pString, int nCount, COLORREF crTL, COLORREF crBR, int offset, int x, int y)
{
	KTextBitmap bmp;

	// generate a mask bitmap with top-left and bottom-right edges
	SetBkMode(hDC, OPAQUE);
	SetBkColor(hDC, RGB(0xFF, 0xFF, 0xFF));					  // white background
	SetTextColor(hDC, RGB(0, 0, 0));
	bmp.Convert(hDC, pString, nCount, offset*2);			  // black TL edge

	SetBkMode(hDC, TRANSPARENT);
	bmp.RenderText(hDC, offset*2, offset*2, pString, nCount); // black BR edge

	SetTextColor(hDC, RGB(0xFF, 0xFF, 0xFF));				  // white main text
	bmp.RenderText(hDC, offset, offset, pString, nCount);


	// mask destination with top-left and bottom-right edges
	bmp.Draw(hDC, x, y, SRCAND);

	// create a color bitmap with top-left and bottom-right edges
	SetBkColor(hDC, RGB(0, 0, 0));							  // black background
	SetTextColor(hDC, crTL);
	bmp.Convert(hDC, pString, nCount, offset);				  // TL edge

	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, crBR);
	bmp.RenderText(hDC, offset*2, offset*2, pString, nCount); // BR edge

	SetTextColor(hDC, RGB(0, 0, 0));				
	bmp.RenderText(hDC, offset, offset, pString, nCount);	  // black main text

	// draw color top-left and bottom-right edges
	bmp.Draw(hDC, x, y, SRCPAINT);
}
