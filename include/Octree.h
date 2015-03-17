#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : octree.h						                                     //
//  Description: Octree color quantilization, color reduction                        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KNode
{
public:
	bool        IsLeaf;
	unsigned	Pixels;
	
	unsigned	SigmaRed;
	unsigned	SigmaGreen;
	unsigned	SigmaBlue;
	KNode *		Child[8];

	KNode(bool leaf)
	{
		IsLeaf     = leaf;
		Pixels     = 0;
		SigmaRed   = 0;
		SigmaGreen = 0;
		SigmaBlue  = 0;
		memset(Child, 0, sizeof(Child));
	}

	RemoveAll(void);

	int PickLeaves(RGBQUAD * pEntry, int * pFreq, int size);
};


class KOctree
{
	typedef enum { MAXMODE = 65536 };

	KNode   * pRoot;
	int		TotalNode;
	int		TotalLeaf;

	void Reduce(KNode * pTree, unsigned threshold);
	

public:

	KOctree()
	{
		pRoot     = new KNode(false);
		TotalNode = 1;
		TotalLeaf = 0;
	}

	~KOctree()
	{
		if ( pRoot )
		{
			pRoot->RemoveAll();
			pRoot = NULL;
		}
	}

	void AddColor (BYTE r, BYTE g, BYTE b);
	void ReduceLeaves(int limit);
	int  GenPalette(RGBQUAD *entry, int * Freq, int size);

	void Merge(KNode * pNode, KNode & target);
};


class KPaletteGen : public KPixelMapper
{
	KOctree octree;

	// return true if data changed
	virtual bool MapRGB(BYTE & red, BYTE & green, BYTE & blue)
	{
		octree.AddColor(red, green, blue);

		return false;
	}

public:

	void AddBitmap(KImage & dib)
	{
		dib.PixelTransform(* this);
	}

	int GetPalette(RGBQUAD * pEntry, int * pFreq, int size)
	{
		return octree.GenPalette(pEntry, pFreq, size);
	}
};


class KColorMatch
{
public:
	
	RGBQUAD   * m_Colors;
	int		    m_nEntries;

	int square(int i)
	{
		return i * i;
	}

public:

	BYTE ColorMatch(int red, int green, int blue)
	{
		int  dis  = 0x7FFFFFFF;
		BYTE best = 0;

		if (   red<0 )   red=0; else if (   red>255 )   red=255;
		if ( green<0 ) green=0; else if ( green>255 ) green=255;
		if (  blue<0 )  blue=0; else if (  blue>255 )  blue=255;

		for (int i=0; i<m_nEntries; i++)
		{
			int d = square(red - m_Colors[i].rgbRed);
			if ( d>dis ) continue;

			d += square(green - m_Colors[i].rgbGreen);
			if ( d>dis ) continue;

			d += square(blue - m_Colors[i].rgbBlue);

			if ( d < dis )
			{
				dis = d;
				best = i;
			}
		}

		return best;
	}

	void Setup(int nEntry, RGBQUAD * pColor)
	{
		m_nEntries   = nEntry;
		m_Colors     = pColor;
	}

};


class KColorReduction : public KPixelMapper
{
protected:
	int			 m_nBPS;
	BYTE	   * m_pBits;
	BYTE	   * m_pPixel;
	KColorMatch  m_Matcher;

	// return true if data changed
	virtual bool MapRGB(BYTE & red, BYTE & green, BYTE & blue)
	{
		*m_pPixel ++ = m_Matcher.ColorMatch(red, green, blue);
		return false;
	}
	
	virtual bool StartLine(int line)
	{
		m_pPixel = m_pBits + line * m_nBPS; // first pixel of a scanline
		return true;
	}

public:

	BITMAPINFO * Convert8bpp(BITMAPINFO * pDIB);
};


class KErrorDiffusionColorReduction : public KColorReduction
{
	int			 * red_error;
	int			 * green_error;
	int			 * blue_error;

	bool		 m_bForward;

	virtual bool StartLine(int line)
	{
		m_pPixel = m_pBits + line * m_nBPS; // first pixel of a scanline

		m_bForward = (line & 1) == 0;

		return true;
	}

	virtual void Map24bpp(BYTE * pBuffer, int width);

public:

	BITMAPINFO * Convert8bpp(BITMAPINFO * pDIB);
};


HPALETTE     CreateDIBPalette(const BITMAPINFO * pDIB);
BITMAPINFO * IndexColorTable(BITMAPINFO * pDIB, HPALETTE hPal);
HPALETTE     LUTCreatePalette(const BYTE * pRGB, int nSize, int nColor);

int			 GenPalette(BITMAPINFO * pDIB, RGBQUAD * pEntry, int * pFreq, int size);

BITMAPINFO * Convert8bpp(BITMAPINFO * pDIB);
BITMAPINFO * Convert8bpp_ErrorDiffusion(BITMAPINFO * pDIB);


