//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : truetype.cpp					                                     //
//  Description: Truetype font decoding                                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "..\..\include\win.h"
#include "..\..\include\LogWindow.h"
#include "TrueType.h"
#include "resource.h"

const char tag_cmap[] = "cmap"; // character to glyph mapping
const char tag_glyf[] = "glyf"; // glyph data
const char tag_head[] = "head"; // font header
const char tag_hhea[] = "hhea"; // horizontal header
const char tag_hmtx[] = "hmtx"; // horizontal metrics
const char tag_loca[] = "loca"; // index to location
const char tag_maxp[] = "maxp"; // maximum profile
const char tag_name[] = "name"; // naming
const char tag_post[] = "post"; // PostScript
const char tag_ttcf[] = "ttcf"; // TrueType font collection
const char tag_kern[] = "kern"; // Kerning table



void KTrueType::Release(void)
{
	if ( m_pFontData )
	{
		delete [] (BYTE *) m_pFontData;
		m_pFontData = NULL;
	}
}

const BYTE * KTrueType::GetTable(const char * tag) const
{
	int no = GetTableNo();

	for (int i=0; i<no; i++)
		if ( memcmp(m_pFontData->entries[i].tag, tag, 4)==0 )
			return GetTable(i);

	return NULL;
}


const GlyphHeader * KTrueType::GetGlyph(int index) const
{
	if ( (index<0) || (index>=m_nNumGlyphs) )
		return NULL;

	if ( m_pGlyphTable==NULL )
		return NULL;

	if ( m_pShortGlyphIndex )
		return (const GlyphHeader *) ( m_pGlyphTable + reverse(m_pShortGlyphIndex[index]) * 2);
	
	if ( m_pLongGlyphIndex )
		return (const GlyphHeader *) ( m_pGlyphTable + reverse(m_pLongGlyphIndex[index]) );

	return NULL;
}

///////////////////////////////////////////////////////
//				h e a d		T A B L E				 //
///////////////////////////////////////////////////////

typedef struct
{
	Fixed	Table;				// version number 0x00010000 for version 1.0. 
	Fixed	fontRevision;		// Set by font manufacturer. 
	ULONG	checkSumAdjustment;	// To compute: set it to 0, sum the entire font as ULONG, then store 0xB1B0AFBA - sum. 
	ULONG	magicNumber;		// Set to 0x5F0F3CF5. 
	USHORT	flags;				// Bit 0 - baseline for font at y=0; 
								// Bit 1 - left sidebearing at x=0;
								// Bit 2 - instructions may depend on point size; 
								// Bit 3 - force ppem to integer values for all internal scaler math; may use fractional ppem sizes if this bit is clear; 
								// Bit 4 - instructions may alter advance width (the advance widths might not scale linearly); 
								// Bits 5-10 - defined by Apple; 
								// Bit 11 - font compressed with Agfa MicroType Compression. 
								// Note: All other bits must be zero. 
	USHORT	unitsPerEm;			// Valid range is from 16 to 16384 
	longDT	created;			// International date (8-byte field). 
	longDT	modified;			// International date (8-byte field). 
	FWord	xMin;				// For all glyph bounding boxes. 
	FWord	yMin;				// For all glyph bounding boxes. 
	FWord	xMax;				// For all glyph bounding boxes. 
	FWord	yMax;				// For all glyph bounding boxes. 
	USHORT	macStyle;			// Bit 0 bold (if set to 1); Bit 1 italic (if set to 1) 
								// Bits 2-15 reserved (set to 0). 
	USHORT  lowestRecPPEM;		// Smallest readable size in pixels. 
	SHORT	fontDirectionHint;	//  0 Fully mixed directional glyphs; 
								//  1 Only strongly left to right; 
								//  2 Like 1 but also contains neutrals; 
								// -1 Only strongly right to left; 
								// -2 Like -1 but also contains neutrals. 
	SHORT	indexToLocFormat;	// 0 for short offsets, 1 for long. 
	SHORT	glyphDataFormat;	// 0 for current format. 
}	Table_head;


///////////////////////////////////////////////////////
//				m a x p		T A B L E				 //
///////////////////////////////////////////////////////

typedef struct
{
	Fixed  Version;					// 0x00010000 for version 1.0. 
	USHORT numGlyphs;				// The number of glyphs in the font. 
	USHORT maxPoints;				// Maximum points in a non-composite glyph. 
	USHORT maxContours;				// Maximum contours in a non-composite glyph. 
	USHORT maxCompositePoints;		// Maximum points in a composite glyph. 
	USHORT maxCompositeContours;	// Maximum contours in a composite glyph. 
	USHORT maxZones;				// 1 if instructions do not use the twilight zone (Z0), 
									// or 2 if instructions do use Z0; should be set to 2 in most cases. 
	USHORT maxTwilightPoints;		// Maximum points used in Z0. 
	USHORT maxStorage;				// Number of Storage Area locations.  
	USHORT maxFunctionDefs;			// Number of FDEFs. 
	USHORT maxInstructionDefs;		// Number of IDEFs. 
	USHORT maxStackElements;		// Maximum stack depth2. 
	USHORT maxSizeOfInstructions;	// Maximum byte count for glyph instructions. 
	USHORT maxComponentElements;	// Maximum number of components referenced at "top level" for any composite glyph. 
	USHORT maxComponentDepth;		// Maximum levels of recursion; 1 for simple components. 
}	Table_maxp;
   

/////////////////////////////////////
//  CMAP						   //
/////////////////////////////////////

typedef struct
{
	USHORT	Platform;		// platform ID
	USHORT	EncodingID;		// encoding ID
	ULONG	TableOffset;	// offset to encoding table
}	submap;

typedef struct
{
	USHORT	TableVersion;	 // table version 0
	USHORT	NumSubTable;	 // number of encoding tables
	submap  TableHead[1];    // heads for encoding tables
}	Table_cmap;

typedef struct
{
	USHORT	format;			// format: 0, 2, 4, 6
	USHORT	length;			// size
	USHORT	version;		// version
	BYTE	map[1];			// mapping data
}	Table_Encode;


//////////////////////////////////////
// HHEA Horizontal Header
//////////////////////////////////////

typedef struct
{
	Fixed  version;				// 0x00010000 for version 1.0. 
	FWord  Ascender;			// Typographic ascent. 
	FWord  Descender;			// Typographic descent. 
	FWord  LineGap;				// Typographic line gap. 
	FWord  advanceWidthMax;		// Maximum advance width value in 'hmtx' table. 
	FWord  minLeftSideBearing;	// Minimum left sidebearing value in 'hmtx' table. 
	FWord  minRightSideBearing; // Minimum right sidebearing value; calculated as Min(aw - lsb - (xMax - xMin)). 
	FWord  xMaxExtent;			// Max(lsb + (xMax - xMin)). 
	SHORT  caretSlopeRise;		// Used to calculate the slope of the cursor (rise/run); 1 for vertical. 
	SHORT  caretSlopeRun;		// 0 for vertical. 
	SHORT  reserved[5];		    // set to 0 
	SHORT  metricDataFormat;	// 0 for current format. 
	USHORT numberOfHMetrics;	// Number of hMetric entries in 'hmtx' table; 
							    // must be equal to the CharStrings INDEX count in the 'CFF ' table. 
}	Table_HoriHeader;


//////////////////////////////////////
// KERN Kerning Table
//////////////////////////////////////

typedef struct
{
	FWord	leftglyph;
	FWord	rightglyph;
	FWord	move;
}	KerningPair;

typedef struct
{
	FWord		Version;
	FWord		nSubTables;
	
	FWord		SubTableVersion;
	FWord		Bytesinsubtable;
	FWord		Coveragebits;
	FWord		Numberpairs;
	FWord		SearchRange;
	FWord		EntrySelector;
	FWord		RangeShift;
	KerningPair KerningPair[1];		// variable size
}	Table_Kerning;


////////////////////////////////////

typedef struct
{
	USHORT	version;			// 0x0001 
	SHORT	xAvgCharWidth;		// weighted average width of a..z
	USHORT	usWeightClass;      // FW_THIN..FW_BLACK
	USHORT	usWidthClass;		// FWIDTH_ULTRA_CONDENSED..FWIDTH_ULTRA_EXPANDED
	SHORT	fsType;				// embedding licensing rights
	SHORT	ySubscriptXSize;   
	SHORT	ySubscriptYSize;   
	SHORT	ySubscriptXOffset;   
	SHORT	ySubscriptYOffset;   
	SHORT	ySuperscriptXSize;   
	SHORT	ySuperscriptYSize;   
	SHORT	ySuperscriptXOffset;   
	SHORT	ySuperscriptYOffset;   
	SHORT	yStrikeoutSize;     // strikeout stroke width in design units.
	SHORT	yStrikeoutPosition;   
	SHORT	sFamilyClass;       // IBM font class
	PANOSE	panose;   
	ULONG	ulUnicodeRange1;	// Bits 0-31   Unicode Character Range
	ULONG	ulUnicodeRange2;	// Bits 32-63 
	ULONG	ulUnicodeRange3;	// Bits 64-95 
	ULONG	ulUnicodeRange4;	// Bits 96-127 
	CHAR	achVendID[4];		// vendor ID 
	USHORT	fsSelection;        // ITALIC UNDERSCORE NEGATIVE OUTLINE STRIKEOUT BOLD REGULAR
	USHORT	usFirstCharIndex;   // first UNICODE char
	USHORT	usLastCharIndex;    // last UNICODE char
	USHORT	sTypoAscender;      // typographic ascender 
	USHORT	sTypoDescender;     // typographic ascender 
	USHORT	sTypoLineGap;		// typographic line gap
	USHORT	usWinAscent;		// ascender metric for Windows
	USHORT	usWinDescent;		// descender metric for Windows
	ULONG	ulCodePageRange1;	// Bits 0-31 
	ULONG	ulCodePageRange2;	// Bits 32-63 
}	Table_OS2;

/////////////////////////////////////

TableDirectory * GetTrueTypeFont(HDC hDC)
{
	// read font size
	DWORD FontSize = GetFontData(hDC, 0, 0, NULL, 0);
	
	TableDirectory * pFont = (TableDirectory *) new BYTE[FontSize];
	if ( pFont==NULL )
		return NULL;

	GetFontData(hDC, 0, 0, pFont, FontSize);

	return pFont;
}


BOOL KTrueType::Load(LOGFONT & logfont)
{
	HFONT hFont  = CreateFontIndirect(& logfont);
	HDC   hDC	 = GetDC(NULL);
	HGDIOBJ hOld = SelectObject(hDC, hFont);

	Release();

	// read font collection size
	DWORD FCsize = GetFontData(hDC, * (DWORD *) tag_ttcf, 0, NULL, 0);
	
	// read font size
	m_nFontSize = GetFontData(hDC, NULL, 0, NULL, 0);
	
	if ( (m_nFontSize==0) || (m_nFontSize==0xFFFFFFFF) )
		return FALSE;
	
	if ( (FCsize==0) || (FCsize==0xFFFFFFFF) )	// not a font collecton
		m_nBias = 0;
	else										// is font collection, calculate bias
		m_nBias = m_nFontSize - FCsize;

	m_pFontData = (TableDirectory *) new BYTE[m_nFontSize];
	GetFontData(hDC, NULL, 0, m_pFontData, m_nFontSize);

	SelectObject(hDC, hOld);
	ReleaseDC(NULL, hDC);
	DeleteObject(hFont);

	const Table_maxp * pmaxp = (const Table_maxp *) GetTable(tag_maxp);
	
	if ( pmaxp )
		m_nNumGlyphs = reverse(pmaxp->numGlyphs);

	const Table_head * phead = (const Table_head *) GetTable(tag_head);

	if ( phead->indexToLocFormat )
		m_pLongGlyphIndex  = (const DWORD *)  GetTable(tag_loca);
	else
		m_pShortGlyphIndex = (const WORD *) GetTable(tag_loca);

	m_pGlyphTable = GetTable(tag_glyf);
	m_unitsPerEm  = reverse(phead->unitsPerEm);
	
	{
		const Table_HoriHeader * phhea = (const Table_HoriHeader *) GetTable(tag_hhea);

		m_Ascender		   = (short) reverse((WORD) phhea->Ascender);
		m_Descender		   = (short) reverse((WORD) phhea->Descender);
		m_LineGap		   = (short) reverse((WORD) phhea->LineGap);
		m_numberOfHMetrics = (short) reverse((WORD) phhea->numberOfHMetrics);

		SetHTMX(GetTable(tag_hmtx));
	}

	{
		const Table_Kerning * pKern = (const Table_Kerning *) GetTable(tag_kern);
		
		int i = 0;
	}
	return TRUE;
}


///////////////////////////////////////////////////////
//				g l y p		T A B L E				 //
///////////////////////////////////////////////////////

// Simple Glyph
//	USHORT endPtsOfContours[n];	// Array of last points of each contour; n is the number of contours. 
//	USHORT instructionLength;	// Total number of bytes for instructions. 
//	BYTE instructions[n];		// Array of instructions for each glyph; n is the number of instructions. 
//	BYTE flags[n];				// Array of flags for each coordinate in outline; n is the number of flags. 
//	BYTE or SHORT xCoordinates[ ] First coordinates relative to (0,0); others are relative to previous point. 
//	BYTE or SHORT yCoordinates[ ] First coordinates relative to (0,0); others are relative to previous point. 

typedef enum
{
	G_ONCURVE	= 0x01,		// on curve, off curve
	G_REPEAT	= 0x08,		// next byte is flag repeat count

	G_XMASK		= 0x12,
	G_XADDBYTE  = 0x12,		// X is positive byte
	G_XSUBBYTE	= 0x02,     // X is negative byte
	G_XSAME		= 0x10,		// X is same
	G_XADDINT	= 0x00,		// X is signed word

	G_YMASK		= 0x24,
	G_YADDBYTE  = 0x24,		// Y is positive byte
	G_YSUBBYTE	= 0x04,     // Y is negative byte
	G_YSAME		= 0x20,		// Y is same
	G_YADDINT	= 0x00,		// Y is signed word
};  

// Simple Glyph
//	USHORT endPtsOfContours[n];	// Array of last points of each contour; n is the number of contours. 
//	USHORT instructionLength;	// Total number of bytes for instructions. 
//	BYTE instructions[n];		// Array of instructions for each glyph; n is the number of instructions. 
//	BYTE flags[n];				// Array of flags for each coordinate in outline; n is the number of flags. 
//	BYTE or SHORT xCoordinates[ ] First coordinates relative to (0,0); others are relative to previous point. 
//	BYTE or SHORT yCoordinates[ ] First coordinates relative to (0,0); others are relative to previous point. 

void Mark(HDC hDC, int x, int y, COLORREF cr)
{
	SetPixel(hDC, x, y, cr);
	
	SetPixel(hDC, x-2, y-1, cr);
	SetPixel(hDC, x-2, y,   cr);
	SetPixel(hDC, x-2, y+1, cr);
	
	SetPixel(hDC, x+2, y-1, cr);
	SetPixel(hDC, x+2, y,   cr);
	SetPixel(hDC, x+2, y+1, cr);

	SetPixel(hDC, x-1, y-2, cr);
	SetPixel(hDC, x,   y-2, cr);
	SetPixel(hDC, x+1, y-2, cr);

	SetPixel(hDC, x-1, y+2, cr);
	SetPixel(hDC, x,   y+2, cr);
	SetPixel(hDC, x+1, y+2, cr);
}


// draw a 2nd degree Bezier curve segment
BOOL Bezier(HDC hDC, int & x0, int & y0, int x1, int y1, int x2, int y2)
{
	// p0 p1 p2 -> p0 (p0+2p1)/3 (2p1+p2)/3, p2
				
	POINT P[3] = { { (x0+2*x1)/3, (y0+2*y1)/3 }, { (2*x1+x2)/3, (2*y1+y2)/3 }, { x2, y2 } };
	x0 = x2; y0 = y2;

	return PolyBezierTo(hDC, P, 3);
}


class KScale
{
	int    _x;
	int    _y;
	double _scale;

public:
	KScale(int x, int y, double scale)
	{
		_x     = x;
		_y     = y;
		_scale = scale;
	}

	void Map(POINT & p)
	{
		p.x = (int) (_x + p.x * _scale);
		p.y = (int) (_y - p.y * _scale);
	}

	void Line(HDC hDC, int x0, int y0, int x1, int y1)
	{
		MoveToEx(hDC, (int) (_x + x0 * _scale), (int) (_y - y0 * _scale), NULL);
		LineTo  (hDC, (int) (_x + x1 * _scale), (int) (_y - y1 * _scale));
	}
};


class KCurve
{
	typedef enum { MAX_POINTS = 1024 };

	POINT m_Point[MAX_POINTS];
	BYTE  m_Flag [MAX_POINTS];

	RECT  m_bound;
	int   m_len;
	int   m_open;

public:

	int   m_glyphsize;
	int   m_instrsize;
	int   m_glyphindex;
	int   m_Ascender;
	int	  m_Descender;
	int   m_LineGap;
	int   m_advancewidth;
	int   m_lsb;
	
	typedef enum { FLAG_ON=1, FLAG_OPEN=2, FLAG_EXTRA=4 };

	void SetBound(signed short x0, signed short y0, signed short x1, signed short y1)
	{
		m_bound.left   = x0;
		m_bound.top    = y0;
		m_bound.right  = x1;
		m_bound.bottom = y1;
	}

	void Reset(void)
	{
		m_len    = 0;
		m_open   = 0;
	}

	void Append(int x, int y, BYTE flag)
	{
		m_Point[m_len].x = x;
		m_Point[m_len].y = y;

		if ( m_open==m_len )
			flag |= FLAG_OPEN;

		m_Flag[m_len]    = flag;
	
		assert(m_len<= MAX_POINTS);
		m_len ++;
	}

	void Add(int x, int y, BYTE flag)
	{
		if ( m_len && ( (flag & FLAG_ON)==0 ) && ( (m_Flag[m_len-1] & FLAG_ON)==0 ) )
		{
			Append((m_Point[m_len-1].x+x)/2, (m_Point[m_len-1].y+y)/2, FLAG_ON | FLAG_EXTRA); // add a middle point
		}

		Append(x, y, flag);
	}

	void Close(void)
	{
		Append(m_Point[m_open].x, m_Point[m_open].y, FLAG_ON | FLAG_EXTRA);
		m_open = m_len;
	}

	int GetLength(void) const
	{
		return m_len;
	}

	BOOL Draw(HDC hDC, int x, int y, double scale, int mark, int emsquare)
	{
		KScale sc(x, y, scale);

		// scale points
		for (int i=0; i<m_len; i++)
			sc.Map(m_Point[i]);

		// em square
		if ( mark )
		{
			TCHAR temp[64];
			wsprintf(temp, "Glyph: %d, Size %d Inst %d EM square %d", 
				m_glyphindex, m_glyphsize, m_instrsize, emsquare);
			TextOut(hDC, x, (int)(y-emsquare*scale)-40, temp, _tcslen(temp));

			if ( (mark==IDM_VIEW_LARGE) || (mark==IDM_VIEW_METRICS) )
			{
				// bounding box
				RECT r = { (int) (x + m_bound.left  * scale),
					   (int) (y - m_bound.bottom* scale),
					   (int) (x + m_bound.right * scale),
					   (int) (y - m_bound.top   * scale) };
				
				FrameRect(hDC, & r, GetSysColorBrush(COLOR_ACTIVECAPTION));
			}

			if ( mark!=IDM_VIEW_METRICS )
			{
				// em square
				HPEN hPen = CreatePen(PS_DOT, 1, 0);
				HGDIOBJ hOld = SelectObject(hDC, hPen);

				for (int i=0; i<=emsquare; i+=(emsquare/16))
				{
					sc.Line(hDC, i, 0, i, emsquare);
					sc.Line(hDC, 0, i, emsquare, i);
				}

				SelectObject(hDC, hOld);
				DeleteObject(hOld);
			}

			if ( mark==IDM_VIEW_METRICS )
			{
				// descender
				sc.Line(hDC, - emsquare/10, m_Descender, emsquare*11/10, m_Descender);

				// base
				sc.Line(hDC, - emsquare/10, 0, emsquare*11/10, 0);

				// ascender
				sc.Line(hDC, - emsquare/10, m_Ascender , emsquare*11/10, m_Ascender );

				sc.Line(hDC, m_bound.left - m_lsb,				    m_Descender,
							 m_bound.left - m_lsb,					m_Ascender);
				sc.Line(hDC, m_bound.left - m_lsb + m_advancewidth, m_Descender,  
							 m_bound.left - m_lsb + m_advancewidth, m_Ascender);


			}
		}

		i = 0;
		int x0, y0;

		// curve
		while ( i<m_len )
		{
			if ( m_Flag[i] & FLAG_OPEN )
			{
				x0 = m_Point[i].x; y0 = m_Point[i].y; MoveToEx(hDC, x0, y0, NULL);
			}
			else if ( m_Flag[i] & FLAG_ON )
			{
				x0 = m_Point[i].x; y0 = m_Point[i].y; LineTo(hDC, x0, y0);
			}
			else // off on, convert to cubic Bezier curve
			{
				// p0 p1 p2 -> p0 (p0+2p1)/3 (2p1+p2)/3, p2
				Bezier(hDC, x0, y0, m_Point[i].x, m_Point[i].y, m_Point[i+1].x, m_Point[i+1].y);
				i ++;
			}

			i ++;
		}

		// mark control points
		if ( mark && (mark!=IDM_VIEW_METRICS) )
		{
			int seq = 0;

			for (i=0; i<m_len; i++)
			{
				if ( (m_Flag[i] & FLAG_EXTRA)==0 )
				{
					if ( m_Flag[i] & FLAG_ON )
						Mark(hDC, m_Point[i].x, m_Point[i].y, RGB(0xFF, 0, 0));
					else 
						Mark(hDC, m_Point[i].x, m_Point[i].y, RGB(0, 0, 0xFF));

					if ( mark==3 )
					{
						TCHAR temp[4];
						wsprintf(temp, "%d", seq);

						TextOut(hDC, m_Point[i].x+5, m_Point[i].y-5, temp, _tcslen(temp));
					}
					seq ++;
				}
			}
		}
		return TRUE;
	}

};


int KTrueType::DecodeGlyph(int index, KCurve & curve, XFORM * xm) const
{
	const GlyphHeader * pHeader = GetGlyph(index);

	if ( pHeader==NULL )
	{
	//	assert(false);
		return 0;
	}

	int	nContour = (short) reverse(pHeader->numberOfContours);

	if ( nContour<0 )
	{
		return DecodeCompositeGlyph(pHeader+1, curve); // after the header
	}

	if ( nContour==0 )
		return 0;

	curve.SetBound(reverse((WORD)pHeader->xMin), reverse((WORD)pHeader->yMin), 
		           reverse((WORD)pHeader->xMax), reverse((WORD)pHeader->yMax));

	const USHORT * pEndPoint = (const USHORT *) (pHeader+1);

	int nPoints = reverse(pEndPoint[nContour-1]) + 1;  // endpoint of last contour + 1
	int nInst   = reverse(pEndPoint[nContour]);		   // instructon length	

	curve.m_glyphindex = index;
	curve.m_glyphsize  = (int) GetGlyph(index+1) - (int) GetGlyph(index);
	curve.m_Ascender   = m_Ascender;
	curve.m_Descender  = m_Descender;
	curve.m_LineGap    = m_LineGap;
	
	GetMetrics(index, curve.m_advancewidth, curve.m_lsb);
	
	if ( curve.m_glyphsize==0 )
		return 0;

	curve.m_instrsize  = nInst;
	
	const BYTE * pFlag = (const BYTE *) & pEndPoint[nContour] + 2 + nInst;	// first byte in flag
	const BYTE * pX    = pFlag;

	int xlen = 0;

	for (int i=0; i<nPoints; i++, pX++)
	{
		int unit = 0;

		switch ( pX[0] & G_XMASK )
		{
			case G_XADDBYTE:
			case G_XSUBBYTE:
				unit = 1;
				break;

			case G_XADDINT:
				unit = 2;
		}

		if ( pX[0] & G_REPEAT )
		{
			xlen += unit * (pX[1]+1); 

			i += pX[1];
			pX ++;
		}
		else
			xlen += unit;
	}

	const BYTE * pY = pX + xlen;

	int x = 0;
	int y = 0;

	i = 0;
	BYTE flag = 0;
	int  rep  = 0;

	for (int j=0; j<nContour; j++)
	{
		int limit = reverse(pEndPoint[j]);

		while ( i<=limit )
		{
			if ( rep==0 )
			{
				flag =  * pFlag++;
				rep  = 1;

				if ( flag & G_REPEAT )
					rep += * pFlag ++;
			}
		
			int dx = 0, dy = 0;

			switch ( flag & G_XMASK )
			{
				case G_XADDBYTE: dx =   pX[0]; pX ++; break;
				case G_XSUBBYTE: dx = - pX[0]; pX ++; break;
				case G_XADDINT:  dx = (signed short )( (pX[0] << 8) + pX[1]); pX+=2;
			}

			switch ( flag & G_YMASK )
			{
				case G_YADDBYTE: dy =   pY[0]; pY ++; break;
				case G_YSUBBYTE: dy = - pY[0]; pY ++; break;
				case G_YADDINT:  dy = (signed short )( (pY[0] << 8) + pY[1]); pY+=2;
			}

			x += dx;
			y += dy;
			assert(abs(x)<16384);
			assert(abs(y)<16384);

			if ( xm )
				curve.Add((int) ( x * xm->eM11 + y * xm->eM21 + xm->eDx ), 
					      (int) ( x * xm->eM12 + y * xm->eM22 + xm->eDy ), 
						  (flag & G_ONCURVE) ? KCurve::FLAG_ON : 0);
			else
				curve.Add(x, y, (flag & G_ONCURVE) ? KCurve::FLAG_ON : 0);
			
			rep --;
			i ++;
		}

		curve.Close();
	}

	return curve.GetLength();
}


int KTrueType::GetOutline(int index, KCurve & curve) const
{
	curve.Reset();
	
	return DecodeGlyph(index, curve, NULL);
}


void KTrueType::DrawTTGlyph(HDC hDC, int x, int y, int index, double scale, int mark)
{
	KCurve curve;
	
	GetOutline(index, curve);

	curve.Draw(hDC, x, y, scale, mark, m_unitsPerEm);
}



BOOL LogTableDirectory(KTrueType & tt, HINSTANCE hInstance, const char * FaceName)
{
	KLogWindow * pLog = new KLogWindow;

	if ( pLog==NULL )
		return FALSE;

	pLog->Create(hInstance, "Table Directory", LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FONT)));

	pLog->Log("%s \r\n", FaceName);
		
    int no = tt.GetTableNo();

	pLog->Log("%d tables\r\n\r\n", no);

	pLog->Log("Tag           CheckSum   Offset   Length\r\n");
	pLog->Log("----------------------------------------\r\n");

    for (int i=0; i<no; i++)
    {
		const TableEntry * pEntry = tt.GetTableEntry(i);

        unsigned long offset = reverse(pEntry->offset);
        unsigned long length = reverse(pEntry->length);
			
		pLog->Log("%c%c%c%c %08X %08X %8x %8x\r\n", 
				pEntry->tag[0], pEntry->tag[1], 
				pEntry->tag[2], pEntry->tag[3], 
				* ((DWORD *) pEntry->tag),
				pEntry->checkSum,
				offset, length);
    }

	return TRUE;
}



/*
HBITMAP GetBackgroundBitmap(HWND hWnd)
{
	ASSERT(::IsWindow( hWnd));

	HWND hParentWnd = NULL;
	HBITMAP hBitmap    = NULL;

	RECT rcWnd;
	RECT rcParentWnd;

	hParentWnd = ::GetParent( hWnd);

	if (!hParentWnd)
		return NULL;

	::GetWindowRect( hWnd, &rcWnd);

	rcParentWnd = rcWnd;
	::ScreenToClient( hParentWnd, (LPPOINT)&rcParentWnd);
	::ScreenToClient( hParentWnd, ((LPPOINT)&rcParentWnd)+1);
	::GetClientRect( hWnd, &rcWnd);

	HDC hParentDC = GetDC( hParentWnd);

	if(!hParentDC)
		return NULL;

	HDC hBkgDC = ::CreateCompatibleDC( hParentDC);

	if (!hBkgDC)
	{
		::ReleaseDC( hParentWnd, hParentDC);
		::DeleteDC( hParentDC);

		return NULL;
	}

	hBitmap = ::CreateCompatibleBitmap( hParentDC, rcWnd.right-rcWnd.left, 
												   rcWnd.bottom-rcWnd.top);

	HBITMAP hOldBitmap = (HBITMAP)::SelectObject( hBkgDC, hBitmap);

	::SendMessage( hParentWnd, WM_ERASEBKGND, (WPARAM)hParentDC, 0L);

	::BitBlt( hBkgDC, rcWnd.left, rcWnd.top, rcWnd.right-rcWnd.left, 
				rcWnd.bottom-rcWnd.top, hParentDC, rcParentWnd.left, rcParentWnd.top, 
				SRCCOPY);

	::SelectObject( hBkgDC, hOldBitmap);
	::DeleteObject( hOldBitmap);

	::DeleteDC( hBkgDC);

	::ReleaseDC( hParentWnd, hParentDC);
	::DeleteDC( hParentDC);

	return hBitmap;
}
*/

// Composite Glyph

typedef enum
{
	ARG_1_AND_2_ARE_WORDS		= 0x0001, // If this is set, the arguments are words; otherwise, they are bytes. 
	ARGS_ARE_XY_VALUES			= 0x0002, // If this is set, the arguments are xy values; otherwise, they are points. 
	ROUND_XY_TO_GRID			= 0x0004, // For the xy values if the preceding is true. 
	WE_HAVE_A_SCALE				= 0x0008, // This indicates that there is a simple scale for the component. Otherwise, scale = 1.0. 
	
	RESERVED					= 0x0010, // This bit is reserved. Set it to 0. 
	MORE_COMPONENTS				= 0x0020, // Indicates at least one more glyph after this one. 
	WE_HAVE_AN_X_AND_Y_SCALE	= 0x0040, // The x direction will use a different scale from the y direction. 
	WE_HAVE_A_TWO_BY_TWO		= 0x0080, // There is a 2 by 2 transformation that will be used to scale the component. 
	
	WE_HAVE_INSTRUCTIONS		= 0x0100, // Following the last component are instructions for the composite character. 
	USE_MY_METRICS				= 0x0200, // If set, this forces the aw and lsb (and rsb) for the composite to be equal to those from this original glyph. This works for hinted and unhinted characters. 
	OVERLAP_COMPOUND		    = 0x0400, // Used by Apple in GX fonts. 
	SCALED_COMPONENT_OFFSET     = 0x0800, // Composite designed to have the component offset scaled (designed for Apple rasterizer). 
	
	UNSCALED_COMPONENT_OFFSET   = 0x1000, // Composite designed not to have the component offset scaled (designed for the Microsoft TrueType rasterizer).
};


class KDataStream
{
	const unsigned char * pData;

public:
	unsigned char GetByte(void)
	{
		return * pData ++;
	}

	unsigned short GetWord(void)
	{
		unsigned short rslt = GetByte();
		
		return rslt * 256 + GetByte();
	}

	KDataStream(const void * pInput)
	{
		pData = (const unsigned char *) pInput;
	}
};

	       
int KTrueType::DecodeCompositeGlyph(const void * pGlyph, KCurve & curve) const
{
	KDataStream str(pGlyph);

	unsigned flags;

	int len = 0;

	do 
	{
		flags      = str.GetWord();
		
		unsigned glyphIndex = str.GetWord();

		// Argument1 and argument2 can be either x and y offsets to be added to the glyph or two point numbers. 
		// In the latter case, the first point number indicates the point that is to be matched to the new glyph. 
		// The second number indicates the new glyph's "matched" point. Once a glyph is added, its point numbers 
		// begin directly after the last glyphs (endpoint of first glyph + 1). 

		// When arguments 1 and 2 are an x and a y offset instead of points and the bit ROUND_XY_TO_GRID is set to 1, 
		// the values are rounded to those of the closest grid lines before they are added to the glyph. 
		// X and Y offsets are described in FUnits. 

		signed short argument1;
		signed short argument2;

		if ( flags & ARG_1_AND_2_ARE_WORDS )
		{
			argument1 = str.GetWord(); // (SHORT or FWord) argument1;
			argument2 = str.GetWord(); // (SHORT or FWord) argument2;
		} 
		else 
		{
			argument1 = (signed char) str.GetByte();
			argument2 = (signed char) str.GetByte();
		}
	
		signed short xscale, yscale, scale01, scale10;

		xscale  = 1;
		yscale  = 1;
		scale01 = 0;
		scale10 = 0;

		if ( flags & WE_HAVE_A_SCALE )
		{
			xscale = str.GetWord();
			yscale = xscale;			// Format 2.14 
		} 
		else if ( flags & WE_HAVE_AN_X_AND_Y_SCALE ) 
		{
			xscale = str.GetWord();
			yscale = str.GetWord();
		} 
		else if ( flags & WE_HAVE_A_TWO_BY_TWO ) 
		{
			xscale  = str.GetWord();
			scale01 = str.GetWord();
			scale10 = str.GetWord();
			yscale  = str.GetWord();
		}

		if ( flags & ARGS_ARE_XY_VALUES )
		{
			XFORM xm;

			xm.eDx  = (float) argument1;
			xm.eDy  = (float) argument2;
			xm.eM11 = xscale  / (float) 16384.0;
			xm.eM12 = scale01 / (float) 16384.0;
			xm.eM21 = scale10 / (float) 16384.0;
			xm.eM22 = yscale  / (float) 16384.0;
			
			len += DecodeGlyph(glyphIndex, curve, & xm);
		}
		else
			assert(false);
	} 
	while ( flags & MORE_COMPONENTS );

	if ( flags & WE_HAVE_INSTRUCTIONS )
	{
		unsigned numInstr = str.GetWord();

		for (unsigned i=0; i<numInstr; i++)
			str.GetByte();
	}

	// The purpose of USE_MY_METRICS is to force the lsb and rsb to take on a desired value. 
	// For example, an i-circumflex (Unicode 00ef) is often composed of the circumflex and a dotless-i. 
	// In order to force the composite to have the same metrics as the dotless-i, 
	// set USE_MY_METRICS for the dotless-i component of the composite. Without this bit, 
	// the rsb and lsb would be calculated from the HMTX entry for the composite (or would need to be 
	// explicitly set with TrueType instructions). 

	// Note that the behavior of the USE_MY_METRICS operation is undefined for rotated composite components.

	return len;
}
