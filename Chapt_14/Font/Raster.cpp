//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : raster.cpp						                                     //
//  Description: Raster/vector font decoding                                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define NOCRYPT

#include <windows.h>
#include <assert.h>
#include <tchar.h>

#include "..\..\include\mmfile.h"
#include "raster.h"

class KFontHeader20
{
public:
    WORD        Version;              // 0x200 for version 2.0, 0x300 for version 3.00
    DWORD       Size;                 // Size of whole file 
    CHAR        Copyright[60];
    WORD        Type;                 // Raster Font if Type & 1 == 0 
    WORD        Points;               // Nominal Point size 
    WORD        VertRes;              // Nominal Vertical resolution 
    WORD        HorizRes;             // Nominal Horizontal resolution
    WORD        Ascent;               
    WORD        IntLeading;           
    WORD        ExtLeading;           
    BYTE        Italic;               
    BYTE        Underline;            
    BYTE        StrikeOut;            
    WORD        Weight;               
    BYTE        CharSet;              
    WORD        PixWidth;             // width. 0 ==> Variable
    WORD        PixHeight;            
    BYTE        Family;               // Pitch and Family
    WORD        AvgWidth;             // Width of character 'X' 
    WORD        MaxWidth;             // Maximum width 
    BYTE        FirstChar;            // First character defined in font
    BYTE        LastChar;             // Last character defined in font 
    BYTE        DefaultChar;          // Sub. for out of range chars. 
    BYTE        BreakChar;            // Word Break Character 
    WORD        WidthBytes;           // No.Bytes/row of Bitmap
    DWORD       Device;               // Offset to Device Name string 
    DWORD       Face;                 // Offset to Face Name String
    DWORD       BitsPointer;          // Offset to Bit Map
    DWORD       BitsOffset;           // Offset to Bit Map 
};


class KFontHeader30 : public KFontHeader20
{
public:
	BYTE		fsDBfiller;			  /* Word alignment for the offset table  */

    DWORD       fsFlags;              /* Bit flags                            */
    WORD        fsAspace;             /* Global A space, if any               */
    WORD        fsBspace;             /* Global B space, if any               */
    WORD        fsCspace;             /* Global C space, if any               */
    DWORD       fsColorPointer;       /* offset to color table, if any        */
    DWORD       fsReserved[4];        /*                                      */
    BYTE        fsCharOffset;         /* Area for storing the char. offsets   */
};

typedef struct
{
	SHORT GIwidth;
    SHORT GIoffset;
}	GLYPHINFO_20;

typedef struct
{
     SHORT GIwidth;
     LONG  GIoffset;
} GLYPHINFO_30;


int VectorCharOut(HDC hDC, int x, int y, int ch, const KFontHeader20 * pH, int sx=1, int sy=1)
{
	typedef struct
	{
		short offset;
		short width;
	}	VectorGlyph;

	const VectorGlyph * pGlyph = (const VectorGlyph *) ( (BYTE *) & pH->BitsOffset + 4);

	if ( (ch<pH->FirstChar) || (ch>pH->LastChar) )
		ch = pH->DefaultChar;
	else
		ch -= pH->FirstChar;

	int width  = pGlyph[ch].width;
	int length = pGlyph[ch+1].offset - pGlyph[ch].offset;

	signed char * pStroke = (signed char *) pH + pH->BitsOffset + pGlyph[ch].offset;
	
	int dx = 0; 
	int dy = 0;

	while ( length>0 )
	{
		bool move = false;

		if ( pStroke[0]==-128 )
		{
			move = true; pStroke++; length --;
		}

		if ( (pStroke[0]==0) && (pStroke[1]==0) && (pStroke[2]==0) )
			break;

		dx += pStroke[0];
		dy += pStroke[1];

		if ( move )
			MoveToEx(hDC, x + dx*sx, y + dy*sy, NULL);
		else
			LineTo(hDC, x + dx*sx, y + dy*sy);

		pStroke += 2; length -= 2;
	}

	return width * sx;
}


int CharOut(HDC hDC, int x, int y, int ch, KFontHeader20 * pH, int sx=1, int sy=1)
{
	GLYPHINFO_20 * pGlyph = (GLYPHINFO_20 *) ( (BYTE *) & pH->BitsOffset + 5);
	
	if ( (ch<pH->FirstChar) || (ch>pH->LastChar) )
		ch = pH->DefaultChar;
	else
		ch -= pH->FirstChar;

	int width  = pGlyph[ch].GIwidth;
	int height = pH->PixHeight;

	struct { BITMAPINFOHEADER bmiHeader; RGBQUAD bmiColors[2]; } dib = 
	{ 
		{ sizeof(BITMAPINFOHEADER), width, -height, 1, 1, BI_RGB }, 
		{ { 0xFF, 0xFF, 0xFF, 0 }, { 0, 0, 0, 0 } }
	};
    
	int bpl = ( width + 31 ) / 32 * 4;
	BYTE data[64/8*64]; // enough for 64x64

	const BYTE * pPixel = (const BYTE *) pH + pGlyph[ch].GIoffset;
	
	for (int i=0; i<(width+7)/8; i++)
	for (int j=0; j<height; j++)
		data[bpl * j + i] = * pPixel ++;

	StretchDIBits(hDC, x, y, width * sx, height * sy, 0, 0, width, height, 
					   data, (BITMAPINFO *) & dib, DIB_RGB_COLORS, SRCCOPY);

	return width * sx;
}


int Disp20(HDC hDC, int x0, int y0, KFontHeader20 * pH)
{
	char desp[MAX_PATH];
	wsprintf(desp, "%d pts, %dx%d dpi, %dx%d pixel, avgw %d, maxw %d, charset %d",
		pH->Points, pH->VertRes, pH->HorizRes, pH->PixWidth, pH->PixHeight, pH->AvgWidth, pH->MaxWidth,
		pH->CharSet);
	TextOut(hDC, x0, y0+10, desp, _tcslen(desp));
	y0 += 40;

	int dy = pH->PixHeight + 2;
	int dx = pH->MaxWidth  + 2;
	int col;

	if ( pH->Points>24 )
		col = 16;
	else
		col = 32;

	for (int ch = 0; ch<256; ch++)
		if ( pH->Type & 1 )
			VectorCharOut(hDC, x0 + dx * (ch%col), y0 + dy * (ch/col), ch, pH);
		else
			CharOut(hDC, x0 + dx * (ch%col), y0 + dy * (ch/col), ch, pH);

	return 60 + dy * (256/col);
}


int Disp30(HDC hDC, int x0, int y0, KFontHeader30 * pH)
{
	GLYPHINFO_30 * pGlyph = (GLYPHINFO_30 *) & pH->fsCharOffset;

	int height = pH->PixHeight;

	int no = pH->LastChar - pH->FirstChar + 1;

	for (int i=0; i<no; i++)
	{
		int width = pGlyph[i].GIwidth;

		const BYTE * pPixel = (const BYTE *) pH + pGlyph[i].GIoffset;
			
		for (int y=0; y<height; y++)
		{
			for (int x=0; x<(width+7)/8; x++)
			{
				int xx = 0;
				for (int b=0x80; b!=0; b>>=1)
				{
					if ( pPixel[x] & b )
						SetPixel(hDC, i*20 + xx, y+10, RGB(0xFF, 0xFF, 0));
					else
						SetPixel(hDC, i*20 + xx, y+10, RGB(0, 0, 0xFF));
						
					xx ++;
				}
				pPixel ++;
			}
		}
	}

	return 0;
}

typedef struct 
{
     WORD	rnOffset;
     WORD	rnLength;
     WORD	rnFlags;
     WORD	rnID;
     WORD	rnHandle;
     WORD	rnUsage;
} NAMEINFO;

typedef struct 
{
     WORD        rtTypeID;
     WORD        rtResourceCount;
     DWORD       rtReserved;
     NAMEINFO    rtNameInfo[1];
} TYPEINFO;

typedef struct
{
	WORD     rscAlignShift;
	TYPEINFO rscTypes[1];
//	WORD     rscEndTypes;
//	BYTE     rscResourceNames[];
//	BYTE     rscEndNames;
}	RESTABLE;


int DecodeRasterFont(HDC hDC, const TCHAR * filename)
{
    KMemoryMappedFile bmpfont;

	int y = 40;

	KFontHeader20 * pFont = NULL;

    if ( bmpfont.Open(filename) )
    {
		TextOut(hDC, 10, 10, filename, _tcslen(filename));

        KFontHeader20 * pH;

		pH = (KFontHeader20 *) bmpfont.m_View;

		switch ( pH->Version )
		{
			case 0x200:	
				y += Disp20(hDC, 20, y, pH); 
				break;

			case 0x300: 
				y += Disp30(hDC, 20, y, (KFontHeader30 *) pH); 
				break;

			case IMAGE_DOS_SIGNATURE: // MZ
				{
					IMAGE_DOS_HEADER * pDOS = (IMAGE_DOS_HEADER *) pH;
					IMAGE_OS2_HEADER * pOS2 = (IMAGE_OS2_HEADER *) ( (BYTE *) pH + pDOS->e_lfanew);

					RESTABLE * pResTable = (RESTABLE *) ((BYTE *) pOS2 + pOS2->ne_rsrctab);

					int unit	= 1 << pResTable->rscAlignShift;

					TYPEINFO * pTypeInfo = pResTable->rscTypes;

					// search fir RT_FONT resource
					while ( pTypeInfo->rtTypeID!=0 )
					{
						if ( (pTypeInfo->rtTypeID & 0xFFF) == (WORD) RT_FONT )
							for (int i=0; i<pTypeInfo->rtResourceCount; i++)
							{
								WORD offset = pTypeInfo->rtNameInfo[i].rnOffset * unit;

								pFont = (KFontHeader20 *) ((BYTE *) pH + offset);

								switch ( pFont->Version )
								{
									case 0x100:
										y += Disp20(hDC, 20, y, pFont);
										break;

									case 0x200:	
										y += Disp20(hDC, 20, y, pFont); 
										break;

									case 0x300: 
										y += Disp30(hDC, 20, y, (KFontHeader30 *) pFont); 
										break;
								}
							}

						pTypeInfo = (TYPEINFO *) & pTypeInfo[pTypeInfo->rtResourceCount];
					}
				}
		}
	}

	y+= 280;

	if ( pFont )
	{
		int x = 50;

		for (int i=1; i<10; i++)
			if ( pFont->Type & 1 )
			{
				HPEN hPen = CreatePen(PS_SOLID, i, 0);
				HGDIOBJ hOld = SelectObject(hDC, hPen);

				x += VectorCharOut(hDC, x, y-pFont->Ascent*i, 'A', pFont, i, i) + 5;							
				SelectObject(hDC, hOld);
				DeleteObject(hPen);
			}
			else
				x += CharOut(hDC, x, y-pFont->Ascent*i, 'A', pFont, i, i) + 5;
	}		
	
	return y + 20;
}
