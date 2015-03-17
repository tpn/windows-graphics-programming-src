//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : truetype.h						                                     //
//  Description: Truetype font decoder                                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

typedef short	FWord;	// 16-bit signed integer in FUnits
typedef long	Fixed;	// 16.16-bit signed fixed-point number
typedef __int64 longDT; // seconds since 12:00 midnight Jan 1, 1904

typedef struct
{
    char    tag[4];
    ULONG   checkSum;
    ULONG   offset;
    ULONG   length;
}   TableEntry;


typedef struct
{
    Fixed			sfntversion;
    USHORT			numTables;
    USHORT			searchRange;
    USHORT			entrySelector;
    USHORT			rangeShift;
    TableEntry		entries[1];
}   TableDirectory;


typedef struct
{
	ULONG  TTCTag;			// TTC tag 'ttcf' 
	ULONG  Version;			// TTC version (initially 0x0001000) 
	ULONG  DirectoryCount;	// Number of Table Directories
	DWORD  Directory[1];	// offset to TableDirectory
}	TTC_Header;

typedef struct
{
	WORD	numberOfContours;	// If the number of contours is greater than or equal to zero, 
								// this is a single glyph; if negative, this is a composite glyph. 
	FWord	xMin;				// Minimum x for coordinate data. 
	FWord	yMin;				// Minimum y for coordinate data. 
	FWord	xMax;				// Maximum x for coordinate data. 
	FWord	yMax;				// Maximum y for coordinate data. 
}	GlyphHeader;   

inline unsigned long reverse(DWORD data)
{
    const unsigned char * p = (const unsigned char *) & data;
    
    return (p[0]<<24) | (p[1]<<16) | (p[2]<<8) | p[3];
}


inline unsigned short reverse(WORD data)
{
    const unsigned char * p = (const unsigned char *) & data;
    
    return ((data<<8) | (data>>8)) & 0xFFFF;
}

class KCurve;

//////////////////////////////////////
// HTMX : Horizontal Metrics
//////////////////////////////////////

typedef struct
{
	FWord	advanceWidth;
	FWord	lsb;
}	longHorMetric;


typedef struct
{
	longHorMetric hMetrics[1];			// numberOfHMetrics;	
	FWord         leftSideBearing[1];   // advanceWidth same as last
}	Table_HoriMetrics;


class KHorizontalMetrics
{
	const longHorMetric * pLong;
	const FWord         * pLSB;
	FWord				  lstWidth;

public:
	int				 m_Ascender;
	int				 m_Descender;
	int				 m_LineGap;
	int				 m_numberOfHMetrics;

	void SetHTMX(const void * htmx)
	{
		pLong    = (const longHorMetric *) htmx;
		pLSB     = (const FWord *) & pLong[m_numberOfHMetrics];
		lstWidth = pLong[m_numberOfHMetrics-1].advanceWidth;
	}

	void GetMetrics(int index, int & advwidth, int & lsb) const
	{
		if ( index < m_numberOfHMetrics )
		{
			advwidth = (short) reverse((WORD) pLong[index].advanceWidth);
			lsb      = (short) reverse((WORD) pLong[index].lsb);
		}
		else
		{
			advwidth = (short) reverse((WORD) lstWidth);
			lsb      = (short) reverse((WORD) pLSB[index - m_numberOfHMetrics]);
		}
	}
};


class KTrueType : public KHorizontalMetrics
{
	TableDirectory * m_pFontData;
	DWORD			 m_nFontSize;
	int			     m_nNumGlyphs;
	int				 m_nBias;
	int				 m_unitsPerEm;
	
	const WORD  *	 m_pShortGlyphIndex;
	const DWORD *	 m_pLongGlyphIndex;
	const BYTE  *    m_pGlyphTable;

	void			 Release(void);

	int DecodeCompositeGlyph(const void * pData, KCurve & curve) const;
	int DecodeGlyph(int index, KCurve & curve, XFORM * xm) const;

public:

	KTrueType(void)
	{
		m_pFontData  = NULL;
		m_nFontSize  = 0;
		m_nNumGlyphs = 0;
		m_nBias      = 0;

		m_pShortGlyphIndex = NULL;
		m_pLongGlyphIndex  = NULL;
		m_pGlyphTable      = NULL;
	}

	~KTrueType()
	{
		Release();
	}

	BOOL Load(LOGFONT & logfont);

	int GetTableNo(void) const
	{
        return reverse(m_pFontData->numTables);
	}

	const TableEntry * GetTableEntry(int index) const
	{
		return & m_pFontData->entries[index];
	}

	const BYTE * GetTable(int index) const
	{
		return (BYTE *) m_pFontData + m_nBias + reverse(m_pFontData->entries[index].offset);
	}

	const BYTE * GetTable(const char * tag) const;

	const GlyphHeader * GetGlyph(int index) const;

	int	  GetOutline(int index, KCurve & curve) const;

	double GetScale(void) const
	{
		if ( m_unitsPerEm==0 )
			return 1.0;
		else
			return 1.0 / m_unitsPerEm;
	}

	void DrawTTGlyph(HDC hDC, int x, int y, int index, double scale, int mark);
};


BOOL LogTableDirectory(KTrueType & tt, HINSTANCE hInstance, const char * FaceName);