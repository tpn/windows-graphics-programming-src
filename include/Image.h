#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : image.h						                                     //
//  Description: Generic image processing algorithms and classes                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include <math.h>
#include "Color.h"

// Abstract class for mapping single pixel and scan line
class KPixelMapper
{
	BYTE * m_pColor;	// point to color table BGR...
	int    m_nSize;		// size of an entry in the table 3 or 4
	int	   m_nCount;	// number of entries in the table

	// return true if data changed
	virtual bool MapRGB(BYTE & red, BYTE & green, BYTE & blue) = 0;

	// return true if data changed
	virtual bool MapIndex(BYTE & index)
	{
		MapRGB(m_pColor[index*m_nSize+2], 
			   m_pColor[index*m_nSize+1], 
			   m_pColor[index*m_nSize]);

		return false;
	}

public:

	KPixelMapper(void)
	{
		m_pColor = NULL;
		m_nSize  = 0;
		m_nCount = 0;
	}

	virtual ~ KPixelMapper()
	{
	}

	void SetColorTable(BYTE * pColor, int nEntrySize, int nCount)
	{
		m_pColor = pColor;
		m_nSize  = nEntrySize;
		m_nCount = nCount;
	}

	virtual bool StartLine(int line)
	{
		return true;
	}

	virtual void Map1bpp(BYTE * pBuffer, int width);
	virtual void Map2bpp(BYTE * pBuffer, int width);
	virtual void Map4bpp(BYTE * pBuffer, int width);
	virtual void Map8bpp(BYTE * pBuffer, int width);
	virtual void  Map555(BYTE * pBuffer, int width);
	virtual void  Map565(BYTE * pBuffer, int width);
	virtual void Map24bpp(BYTE * pBuffer, int width);
	virtual void Map32bpp(BYTE * pBuffer, int width);

};

typedef BYTE (* Operator)(BYTE red, BYTE green, BYTE blue);

class KDIB;
class KImage;

// Generic channel splitting class derived from CPixelMapper
// Controled by an operator function passed to CChannel::Split
class KChannel : public KPixelMapper
{
	Operator  m_Operator;
	int		  m_nBPS;
	
	BYTE * m_pBits;
	BYTE * m_pPixel;

	// return true if data changed
	virtual bool MapRGB(BYTE & red, BYTE & green, BYTE & blue)
	{
		* m_pPixel ++ = m_Operator(red, green, blue);

		return false;
	}

	virtual bool StartLine(int line)
	{
		m_pPixel = m_pBits + line * m_nBPS; // first pixel of a scanline

		return true;
	}

public:

	BITMAPINFO * Split(KImage & dib, Operator oper);
};


// RGB channel splitting operators

inline BYTE TakeRed(BYTE red, BYTE green, BYTE blue)
{
	return red;
}

inline BYTE TakeGreen(BYTE red, BYTE green, BYTE blue)
{
	return green;
}

inline BYTE TakeBlue(BYTE red, BYTE green, BYTE blue)
{
	return blue;
}

// KCMY channel splitting operators
inline BYTE TakeK(BYTE red, BYTE green, BYTE blue)
{
	// min ( 255-red, 255-green, 255-blue)
	if ( red < green )
		if ( green < blue )
			return 255 - blue;
		else
			return 255 - green;
	else
		return 255 - red;
}

inline BYTE TakeC(BYTE red, BYTE green, BYTE blue)
{
	return 255 - red; // - TakeK(red, green, blue);
}

inline BYTE TakeM(BYTE red, BYTE green, BYTE blue)
{
	return 255 - green; //  - TakeK(red, green, blue);
}

inline BYTE TakeY(BYTE red, BYTE green, BYTE blue)
{
	return 255 - blue; //  - TakeK(red, green, blue);
}

// HLS channel splitting operators
inline BYTE TakeH(BYTE red, BYTE green, BYTE blue)
{
	KColor color(red, green, blue);
	color.ToHLS();

	return (BYTE) (color.hue * 255 / 360);
}

inline BYTE TakeL(BYTE red, BYTE green, BYTE blue)
{
	KColor color(red, green, blue);
	color.ToHLS();

	return (BYTE) (color.lightness * 255);
}

inline BYTE TakeS(BYTE red, BYTE green, BYTE blue)
{
	KColor color(red, green, blue);
	color.ToHLS();

	return (BYTE) (color.saturation * 255);
}


// 0.299 * red + 0.587 * green + 0.114 * blue 
inline void MaptoGray(BYTE & red, BYTE & green, BYTE & blue)
{
	red   = ( red * 77 + green * 150 + blue * 29 + 128 ) / 256;
	green = red;
	blue  = red;
}

inline void LightenColor(BYTE & red, BYTE & green, BYTE & blue)
{
	KColor color( red, green, blue );
	color.ToHLS();

	color.lightness = sqrt(color.lightness);
	color.ToRGB();

	red   = color.red;
	green = color.green;
	blue  = color.blue;
}


// RGB Histogram class derived from CPixelMapper
class KHistogram : public KPixelMapper
{
	int		  m_FreqRed[256];
	int		  m_FreqGreen[256];
	int		  m_FreqBlue[256];
	int		  m_FreqGray[256];

	// return true if data changed
	virtual bool MapRGB(BYTE & red, BYTE & green, BYTE & blue)
	{
		m_FreqRed[red]     ++;
		m_FreqGreen[green] ++;
		m_FreqBlue[blue]   ++;
		m_FreqGray[(red * 77 + green * 150 + blue * 29 + 128 ) / 256] ++;

		return false;
	}

public:

	void Sample(KImage & dib);
};


// Abstract class for image convolution for single pixel and scan line
class KFilter
{
	int	m_nHalf;

	virtual BYTE Kernel(BYTE * pPixel, int dx, int dy) = 0;

public:

	int GetHalf(void) const
	{
		return m_nHalf;
	}

	KFilter(void)
	{
		m_nHalf = 1;
	}

	virtual ~ KFilter()
	{
	}

	virtual void DescribeFilter(HDC hDC, int x, int y)
	{
	}

	virtual void  Filter8bpp(BYTE * pDest, BYTE * pSource, int nWidth, int dy);
	virtual void Filter24bpp(BYTE * pDest, BYTE * pSource, int nWidth, int dy);
	virtual void Filter32bpp(BYTE * pDest, BYTE * pSource, int nWidth, int dy);
};


void Describe33Filter(HDC hDC, int x, int y, const int * matrix, TCHAR * name, int weight, int add);

template <int k00, int k01, int k02, int k10, int k11, int k12, int k20, int k21, int k22, 
		  int weight, int add, bool checkbound, TCHAR * name>
class K33Filter : public KFilter
{
	virtual BYTE Kernel(BYTE * pPixel, int dx, int dy)
	{
		int rslt = ( pPixel[-dy-dx] * k00 + pPixel[-dy] * k01 + pPixel[-dy+dx] * k02 +
			         pPixel[   -dx] * k10 + pPixel[0]   * k11 + pPixel[   +dx] * k12 +
			         pPixel[ dy-dx] * k20 + pPixel[dy]  * k21 + pPixel[ dy+dx] * k22 ) / weight + add;

		if ( checkbound )
			if ( rslt < 0 )
				return 0;
			else if ( rslt > 255 )
				return 255;

		return rslt;
	}

	void DescribeFilter(HDC hDC, int x, int y)
	{
		const int matrix[] = { k00, k01, k02, k10, k11, k12, k20, k21, k22 };
	
		Describe33Filter(hDC, x, y, matrix, name, weight, add);
	}
};


// Mimimum to grow the darker region
class KErosion : public KFilter
{
	inline void smaller(BYTE &x, BYTE y)
	{
		if ( y < x ) x = y;
	}

	BYTE Kernel(BYTE * pPixel, int dx, int dy)
	{
		BYTE m = pPixel[-dy-dx];

		smaller(m, pPixel[-dy]);
		smaller(m, pPixel[-dy+dx]);
		smaller(m, pPixel[   -dx]);
		smaller(m, pPixel[   +dx]);
		smaller(m, pPixel[ dy-dx]);
		smaller(m, pPixel[dy]);
		smaller(m, pPixel[ dy+dx]);
		
		return min(pPixel[0], m); // /2;
	}

	void DescribeFilter(HDC hDC, int x, int y)
	{
		Describe33Filter(hDC, x, y, NULL, _T("Erosion"), 0, 0);
	}
};

// Maximum to grow the light region
class KDilation : public KFilter
{
	inline void bigger(BYTE &x, BYTE y)
	{
		if ( y > x ) x = y;
	}

	BYTE Kernel(BYTE * pPixel, int dx, int dy)
	{
		BYTE m = pPixel[-dy-dx];

		bigger(m, pPixel[-dy]);
		bigger(m, pPixel[-dy+dx]);
		bigger(m, pPixel[   -dx]);
		bigger(m, pPixel[   +dx]);
		bigger(m, pPixel[ dy-dx]);
		bigger(m, pPixel[dy]);
		bigger(m, pPixel[ dy+dx]);
		
		return max( pPixel[0], m ); // /2;
	}

	void DescribeFilter(HDC hDC, int x, int y)
	{
		Describe33Filter(hDC, x, y, NULL, _T("Dilation"), 0, 0);
	}
};


// Maximum to grow the light region, then substract
class KOutline : public KFilter
{
	inline void bigger(BYTE &x, BYTE y)
	{
		if ( y > x ) x = y;
	}

	BYTE Kernel(BYTE * pPixel, int dx, int dy)
	{
		BYTE m = pPixel[0];
		
		bigger(m, pPixel[-dy-dx]);
		bigger(m, pPixel[-dy]);
		bigger(m, pPixel[-dy+dx]);
		bigger(m, pPixel[   -dx]);
		bigger(m, pPixel[   +dx]);
		bigger(m, pPixel[ dy-dx]);
		bigger(m, pPixel[dy]);
		bigger(m, pPixel[ dy+dx]);
		
		return m - pPixel[0];
	}

	void DescribeFilter(HDC hDC, int x, int y)
	{
		Describe33Filter(hDC, x, y, NULL, _T("Outline"), 0, 0);
	}
};


typedef enum
{
	FILTER_IDENTITY,

	FILTER_SMOOTH,
	FILTER_GUASSIANSMOOTH,
	FILTER_SHARPENING,
	FILTER_LAPLASION,
	FILTER_EMBOSS135,
	FILTER_EMBOSS90,
	FILTER_DILATION,
	FILTER_EROSION,
	FILTER_OUTLINE,
	
	FILTER_LAST = FILTER_OUTLINE
};

KFilter * StockFilters[];


class KImage : public KDIB
{
public:
	bool ToGreyScale(void);
	bool Lighten(void);
	bool GammaCorrect(double redgamma, double greengamma, double bluegamma);

	BITMAPINFO * SplitChannel(Operator oper);

	bool	PixelTransform(KPixelMapper & map);
	bool	Convolution(KFilter * pFilter);
};

