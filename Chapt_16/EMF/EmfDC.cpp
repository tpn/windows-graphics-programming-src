//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : emfdc.cpp						                                     //
//  Description: EMF decompiler								                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define  STRICT
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#define WINVER		 0x500

#include <windows.h>
#include <assert.h>
#include <commctrl.h>
#include <fstream.h>

#include "..\..\include\treeview.h"
#include "..\..\include\mmfile.h"

#include "EmfDC.h"
#include "Formator.h"
#include "OutPipe.h"

#include "resource.h"


const DicItem Dic_BkMode[] =
{
	pair(OPAQUE),
	pair(TRANSPARENT),
	dummyitem
};


const DicItem Dic_ArcDirection[] =
{
	pair(AD_COUNTERCLOCKWISE),
	pair(AD_CLOCKWISE),
	dummyitem
};
 

const DicItem Dic_PolyFillMode[] =
{
	pair(ALTERNATE),
	pair(WINDING),
	dummyitem
};


const DicItem Dic_MapMode[] =
{
	pair(MM_ANISOTROPIC), 
	pair(MM_HIENGLISH), 
	pair(MM_HIMETRIC), 
	pair(MM_ISOTROPIC), 
	pair(MM_LOENGLISH), 
	pair(MM_LOMETRIC), 
	pair(MM_TEXT), 
	pair(MM_TWIPS),
	dummyitem
};
 

const DicItem Dic_StretchBltMode[] =
{ 
	pair(STRETCH_ANDSCANS), 
	pair(STRETCH_DELETESCANS), 
	pair(STRETCH_ORSCANS),
	pair(STRETCH_HALFTONE), 
	pair(BLACKONWHITE),
	pair(COLORONCOLOR),
	pair(HALFTONE),
	pair(WHITEONBLACK),
	dummyitem
};


const DicItem Dic_ICMMode [] =
{
	pair(ICM_ON),
	pair(ICM_OFF),
	pair(ICM_QUERY),
	dummyitem
};


const DicItem Dic_RegionOper [] =
{
	pair(RGN_AND),
	pair(RGN_XOR),
	pair(RGN_COPY), 
	pair(RGN_DIFF), 
	pair(RGN_OR),
	dummyitem
};


const DicItem Dic_TransCombineMode [] =
{
	pair(MWT_IDENTITY),
	pair(MWT_LEFTMULTIPLY),
	pair(MWT_RIGHTMULTIPLY),
	dummyitem
};


const DicItem Dic_ROP2 [] =
{
	pair(R2_BLACK), 
	pair(R2_COPYPEN), 
	pair(R2_MASKNOTPEN), 
	pair(R2_MASKPEN), 
	pair(R2_MASKPENNOT),
	pair(R2_MERGENOTPEN), 
	pair(R2_MERGEPEN), 
	pair(R2_MERGEPENNOT), 
	pair(R2_NOP), 
	pair(R2_NOT), 
	pair(R2_NOTCOPYPEN), 
	pair(R2_NOTMASKPEN), 
	pair(R2_NOTMERGEPEN), 
	pair(R2_NOTXORPEN), 
	pair(R2_WHITE), 
	pair(R2_XORPEN),
	dummyitem
};


const DicItem Dic_FloodFillMode [] =
{
	pair(FLOODFILLBORDER),
	pair(FLOODFILLSURFACE),
	dummyitem
};


const DicItem Dic_PenStyle[] =
{
//	tripod(PS_COSMETIC,     PS_TYPE_MASK),
	tripod(PS_GEOMETRIC,    PS_TYPE_MASK),

	tripod(PS_SOLID,		PS_STYLE_MASK),
	tripod(PS_DASH,			PS_STYLE_MASK),
	tripod(PS_DOT,			PS_STYLE_MASK),
	tripod(PS_DASHDOT,		PS_STYLE_MASK),
	tripod(PS_DASHDOTDOT,	PS_STYLE_MASK),
	tripod(PS_NULL,			PS_STYLE_MASK),
	tripod(PS_INSIDEFRAME,	PS_STYLE_MASK),
	tripod(PS_USERSTYLE,	PS_STYLE_MASK),
	tripod(PS_ALTERNATE,	PS_STYLE_MASK),

//	tripod(PS_ENDCAP_ROUND, PS_ENDCAP_MASK),
	tripod(PS_ENDCAP_SQUARE,PS_ENDCAP_MASK),
	tripod(PS_ENDCAP_FLAT,  PS_ENDCAP_MASK),
	
//	tripod(PS_JOIN_ROUND,   PS_JOIN_MASK),
	tripod(PS_JOIN_BEVEL,   PS_JOIN_MASK),
	tripod(PS_JOIN_MITER,   PS_JOIN_MASK),
	
	dummyitem
};


const DicItem Dic_ROP3[] =
{
	pair(BLACKNESS), 
	pair(DSTINVERT), 
	pair(MERGECOPY), 
	pair(MERGEPAINT), 
	pair(NOTSRCCOPY), 
	pair(NOTSRCERASE), 
	pair(PATCOPY), 
	pair(PATINVERT), 
	pair(PATPAINT), 
	pair(SRCAND), 
	pair(SRCCOPY), 
	pair(SRCERASE), 
	pair(SRCINVERT), 
	pair(SRCPAINT), 
	pair(WHITENESS), 
	dummyitem
};


const DicItem Dic_DIBColor [] =
{
	pair(DIB_PAL_COLORS),
//	pair(DIB_PAL_INDICES),
	pair(DIB_RGB_COLORS),
	dummyitem
};

typedef struct
{
	unsigned      type;

	const TCHAR * funcname;
	const DicItem * dic;
}	EmrInfo;


const DicItem Dic_TextAlign [] = 
{
	tripod(TA_NOUPDATECP,     0x01),
	tripod(TA_UPDATECP,       0x01),

	tripod(TA_LEFT,			  0x06),
	tripod(TA_RIGHT,          0x06),
	tripod(TA_CENTER,         0x06),

	tripod(TA_TOP,            0x18),
	tripod(TA_BOTTOM,         0x18),
	tripod(TA_BASELINE,       0x18),

	tripod(TA_RTLREADING,     0x100),

	dummyitem
};


const DicItem Dic_ExtTextOption [] = 
{
	tripod(ETO_OPAQUE,			ETO_OPAQUE),
	tripod(ETO_CLIPPED,			ETO_CLIPPED),
	tripod(ETO_GLYPH_INDEX,		ETO_GLYPH_INDEX),
	tripod(ETO_RTLREADING,		ETO_RTLREADING),
	tripod(ETO_NUMERICSLOCAL,	ETO_NUMERICSLOCAL),
	tripod(ETO_NUMERICSLATIN,	ETO_NUMERICSLATIN),
	tripod(ETO_IGNORELANGUAGE,	ETO_IGNORELANGUAGE),
	tripod(ETO_PDY,				ETO_PDY),
	dummyitem
};


const EmrInfo Pattern [] = 
{
	{ EMR_SETWINDOWEXTEX          , "SetWindowExtEx"		"(hDC, %d, %d, NULL);" },
    { EMR_SETWINDOWORGEX          , "SetWindowOrgEx"		"(hDC, %d, %d, NULL);" },
    { EMR_SETVIEWPORTEXTEX        , "SetViewportExtEx"		"(hDC, %d, %d, NULL);" },
    { EMR_SETVIEWPORTORGEX        , "SetViewportOrgEx"		"(hDC, %d, %d, NULL);" },
    { EMR_SCALEVIEWPORTEXTEX      , "ScaleViewportExtEx"	"(hDC, %d, %d, %d, %d, NULL);" },
    { EMR_SCALEWINDOWEXTEX        , "ScaleWindowExtEx"		"(hDC, %d, %d, %d, %d, NULL);" },
	{ EMR_SETLAYOUT				  , "SetLayout"				"(hDC, %d)" },
	{ EMR_SETMETARGN              , "SetMetaRgn"			"(hDC);" },
        
	{ EMR_BEGINPATH               , "BeginPath"				"(hDC);" },
    { EMR_ENDPATH                 , "EndPath"				"(hDC);" },
	{ EMR_ABORTPATH               , "AbortPath"				"(hDC);" },
    { EMR_FLATTENPATH             , "FlattenPath"			"(hDC);" },
    { EMR_WIDENPATH               , "WidenPath"				"(hDC);" },
    { EMR_CLOSEFIGURE             , "CloseFigure"			"(hDC);" },
	{ EMR_FILLPATH                , "FillPath"				"(hDC);" },
    { EMR_STROKEANDFILLPATH       , "StrokeAndFillPath"		"(hDC);" },
    { EMR_STROKEPATH              , "StrokePath"			"(hDC);" },
    { EMR_SAVEDC                  , "SaveDC"				"(hDC);" },
    { EMR_RESTOREDC               , "RestoreDC"				"(hDC, %d);" },
    
	{ EMR_CREATEPEN               , "#o=CreatePen(%L, %d%_, #c);", Dic_PenStyle },
    { EMR_CREATEBRUSHINDIRECT     , "#o=#C;" },
	{ EMR_EXTCREATEFONTINDIRECTW  , "#o=CreateFont(%d,%d,%d,%d,%d,%b,%b,%b,%b,%b,%b,%b,%b,%S);" },
    { EMR_CREATEPALETTE           , "#o=CreatePalette(LOGPALETTE {%D,%D,...});" },
    
	{ EMR_SELECTOBJECT            , "SelectObject(hDC, #o);" },
	{ EMR_SELECTPALETTE           , "SelectPalette(hDC, (HPALETTE)#o, TRUE);" },
	{ EMR_SETCOLORSPACE           , "SetColorSpace(hDC, #o);" },
    { EMR_DELETEOBJECT            , "DeleteObject(#o);" },
    { EMR_DELETECOLORSPACE        , "DeleteColorSpace(#o);" },
    
	{ EMR_SETTEXTCOLOR            , "SetTextColor(hDC, #c);" },
    { EMR_SETBKCOLOR              , "SetBkColor(hDC, #c);" },
	{ EMR_SETBRUSHORGEX           , "SetBrushOrgEx(hDC, %d, %d, NULL);" },
	{ EMR_SELECTCLIPPATH          , "SelectClipPath(hDC, %L);", Dic_RegionOper },
    { EMR_SETTEXTALIGN            , "SetTextAlign(hDC, %L);", Dic_TextAlign },
    
	{ EMR_SETBKMODE               , "SetBkMode(hDC, %L);",			Dic_BkMode },
    { EMR_SETARCDIRECTION         , "SetArcDirection(hDC, %L);",		Dic_ArcDirection },
    { EMR_SETPOLYFILLMODE         , "SetPolyFillMode(hDC, %L);",		Dic_PolyFillMode },
    { EMR_SETMAPMODE              , "SetMapMode(hDC, %L);",			Dic_MapMode },
    { EMR_SETSTRETCHBLTMODE       , "SetStretchBltMode(hDC, %L);",	Dic_StretchBltMode },
    { EMR_SETMAPPERFLAGS          , "SetMapperFlags(hDC, %d);" },
    { EMR_SETICMMODE              , "SetICMMode(hDC, %d);",          Dic_ICMMode },
    { EMR_SETROP2                 , "SetROP2(hDC, %L);",             Dic_ROP2 },

    { EMR_SETMITERLIMIT           , "SetMiterLimit(hDC, %f);" },
    { EMR_SETWORLDTRANSFORM       , "SetWorldTransform(hDC, %f, %f, %f, %f, %f, %f);" }, 
    { EMR_MODIFYWORLDTRANSFORM    , "ModifyWorldTransform(hDC, %f, %f, %f, %f, %f, %f, %L);", Dic_TransCombineMode },
    
	{ EMR_REALIZEPALETTE          , "RealizePalette(hDC);" },
    { EMR_RESIZEPALETTE           , "ResizePalette(hDC, #o, %d);" },
    
	{ EMR_OFFSETCLIPRGN           , "OffsetClipRgn(hDC, %d, %d);" },
    { EMR_EXCLUDECLIPRECT         , "ExcludeClipRect(hDC, %d, %d, %d, %d);" },
    { EMR_INTERSECTCLIPRECT       , "IntersectClipRect(hDC, %d, %d, %d, %d);" },
    
    { EMR_SETPIXELV               , "SetPixelV(hDC, %d, %d, #c);" },
    { EMR_MOVETOEX                , "MoveToEx(hDC, %d, %d, NULL);" },
    { EMR_LINETO                  , "LineTo(hDC, %d, %d);" },
	{ EMR_RECTANGLE               , "Rectangle(hDC, %d, %d, %d, %d);" },
    { EMR_ELLIPSE                 , "Ellipse(hDC, %d, %d, %d, %d);" },
	{ EMR_ROUNDRECT               , "RoundRect(hDC, %d, %d, %d, %d, %d, %d);" },
    { EMR_PIE                     , "Pie(hDC, %d, %d, %d, %d, %d, %d, %d, %d);" },
    { EMR_ARC                     , "Arc(hDC, %d, %d, %d, %d, %d, %d, %d, %d);" },
    { EMR_CHORD                   , "Chord(hDC, %d, %d, %d, %d, %d, %d, %d, %d);" },
    { EMR_ARCTO                   , "ArcTo(hDC, %d, %d, %d, %d, %d, %d, %d, %d);" },
	{ EMR_ANGLEARC                , "AngleArc(hDC, %d, %d, %d, %f, %f);" },
    { EMR_EXTFLOODFILL            , "ExtFloodFill(hDC, %d, %d, #c, %L);", Dic_FloodFillMode },
    
    { EMR_POLYLINE16              , "\nstatic const POINT Points_%n[]=#p;\n"     "Polyline(hDC, Points_%n, %4);" },
    { EMR_POLYLINETO16            , "\nstatic const POINT Points_%n[]=#p;\n"   "PolylineTo(hDC, Points_%n, %4);" },
	{ EMR_POLYBEZIER16            , "\nstatic const POINT Points_%n[]=#p;\n"   "PolyBezier(hDC, Points_%n, %4);" },
    { EMR_POLYBEZIERTO16          , "\nstatic const POINT Points_%n[]=#p;\n" "PolyBezierTo(hDC, Points_%n, %4);" },
    { EMR_POLYGON16               , "\nstatic const POINT Points_%n[]=#p;\n"      "Polygon(hDC, Points_%n, %4);" },
    
	{ EMR_POLYLINE                , "\nstatic const POINT Points_%n[]=#P;\n"     "Polyline(hDC, Points_%n, %4);" },
    { EMR_POLYLINETO              , "\nstatic const POINT Points_%n[]=#P;\n"   "PolylineTo(hDC, Points_%n, %4);" },
	{ EMR_POLYBEZIER              , "\nstatic const POINT Points_%n[]=#P;\n"   "PolyBezier(hDC, Points_%n, %4);" },
    { EMR_POLYBEZIERTO            , "\nstatic const POINT Points_%n[]=#P;\n" "PolyBezierTo(hDC, Points_%n, %4);" },
    { EMR_POLYGON                 , "\nstatic const POINT Points_%n[]=#P;\n"      "Polygon(hDC, Points_%n, %4);" },
    
	//////////////////
//  { EMR_HEADER                  , "// Header" },
    
  	{ EMR_POLYDRAW16              , "PolyDraw16" },
    
	{ EMR_EOF                     , "// EMREOF(%d, %d, %d)" },
    { EMR_SETCOLORADJUSTMENT      , "SetColorAdjustment" },
    { EMR_SETPALETTEENTRIES       , "SetPaletteEntries" },
    { EMR_POLYDRAW                , "PolyDraw" },
    
    { EMR_GDICOMMENT              , "// GdiComment(%d, %c%c%c%c, 0x%x)" },
    { EMR_FILLRGN                 , "FillRgn" },
    { EMR_FRAMERGN                , "FrameRgn" },
    { EMR_INVERTRGN               , "InvertRgn" },
    { EMR_PAINTRGN                , "PaintRgn" },
    { EMR_MASKBLT                 , "MaskBlt" },
    { EMR_PLGBLT                  , "PlgBlt" },
    { EMR_SETDIBITSTODEVICE       , "SetDIBSToDevice" },
    { EMR_CREATEDIBPATTERNBRUSHPT , "CreateDIBPatternBrushPT" },
    { EMR_POLYTEXTOUTA            , "PolyTextOutA" },
    { EMR_POLYTEXTOUTW            , "PolyTextOutW" },
    { EMR_CREATECOLORSPACE        , "CreateColorSpace" },
    { EMR_GLSRECORD               , "GlsRecord" },
    { EMR_GLSBOUNDEDRECORD        , "GlsBOundedRecord" },
    { EMR_PIXELFORMAT             , "PixelFormat" },

	{ EMR_COLORCORRECTPALETTE	  , "ColorCorrectPalette" },
	{ EMR_SETICMPROFILEA		  , "SetICMProfileA" },
	{ EMR_SETICMPROFILEW		  , "SetICMProfileW" },
	{ EMR_ALPHABLEND			  , "AlphaBlend" },
	{ EMR_TRANSPARENTBLT		  , "TransParentBlt" },
	{ EMR_GRADIENTFILL			  , "GradientFill" },

//	{ EMR_DRAWESCAPE			  , "DrawEscape" },
//	{ EMR_EXTESCAPE               , "ExtEscape" },
//	{ EMR_STARTDOC                , "StartDoc" },
//	{ EMR_FORCEUFIMAPPING         , "ForceEuFiMapping" },
//	{ EMR_NAMEDESCAPE			  , "NamedEscape" },
//	{ EMR_TRANSPARENTDIB		  , "TransParentDIB" },
//	{ EMR_SETLINKEDUFIS           , "SetLinkEDUFIS" },
//	{ EMR_SETTEXTJUSTIFICATION    , "SetTextJustification" }
//  { EMR_EXTSELECTCLIPRGN        , "ExtSelectClipRgn" },
//	{ EMR_SMALLTEXTOUT            , "TextOut" },
//	{ EMR_CREATEMONOBRUSH         , "CreateMonoBrush" },
    
};

typedef struct
{
    EMR    emr;     
	POINTL ptlReference;     
	DWORD  nChars;     
	DWORD  fOptions;     
	DWORD  iGraphicsMode;
	FLOAT  exScale;
	FLOAT  eyScale;
	char   Text[1];
} EMRSMALLTEXTOUT;
		  

inline const void * Offset(const void * base, long ofst)
{
	return (const void *) ( (const char *) base + ofst);
}

const TCHAR Format_ENHMETAHEADER [] =
	"4iType: %d\0" 
    "4nSize: %d\0" 
    "grclBounds: { %d, %d, %d, %d}\0"
    "grclFrame: { %d, %d, %d, %d}\0" 
    "4dSignature: 0x%08x\0" 
    "4nVersion: 0x%x\0" 
    "4nBytes: %d\0" 
    "4nRecords: %d\0"
    "2nHandles: %d\0"
    "2sReserved: %d\0" 
    "4nDescription: %d\0" 
    "4offDescription: %d\0" 
    "4nPalEntries: %d\0" 
    "8szlDevice: { %d, %d }\0" 
    "8szlMillimeters: { %d, %d }\0"
    "4cbPixelFormat: %d\0"
    "4offPixelFormat: %d\0"
    "4bOpenGL: %d\0"
	"8szMicroMeters: { %d, %d }\0";



bool KEmfDC::Decode(const EMR * emr, KTreeView * pTree)
{
	if ( pTree && emr->iType==EMR_HEADER )
	{
		pTree->InsertTree(TVI_LAST, TVI_ROOT, "1: ENHMETAHEADER", Format_ENHMETAHEADER, emr);
		return true;
	}

	const TCHAR * rslt = DecodeRecord(emr);

	if ( pTree )
	{
		TCHAR temp[1024];

		m_nSeq ++;
		wsprintf(temp, "%d: ", m_nSeq);
		_tcscat(temp, rslt);

		wsprintf(temp + _tcslen(temp), ", %d bytes", emr->nSize);
		
		pTree->InsertItem(temp);
	}

	return true;
}


const TCHAR * KEmfDC::DecodeRecord(const EMR * emr)
{
	fmt.Newline();

	for (int i=0; i< sizeof(Pattern)/sizeof(Pattern[0]); i++)
		if ( Pattern[i].type == emr->iType)
		{
			fmt.Format(Pattern[i].funcname, (const long *) & emr[1], Pattern[i].dic);
			
			return fmt.m_buffer;
		}

	switch ( emr->iType )
	{		
		case EMR_HEADER:
			fmt.Write("// Header");
			break;

		case EMR_BITBLT:
			{
				EMRBITBLT  * bitblt = (EMRBITBLT *) emr;

				if ( bitblt->cbBitsSrc==0 ) // no bitmap
					fmt.Format("PatBlt(hDC,%d,%d,%d,%d,", & bitblt->xDest);
				else
				{
					fmt.Format("BitBlt(hDC,%d,%d,%d,%d,", & bitblt->xDest);

				//	assert(false);
					fmt.Write("hMemDC");
				
					fmt.Format(",%d,%d,", & bitblt->xSrc);
				}

				fmt.Write(Lookup(bitblt->dwRop, Dic_ROP3));
				fmt.Write(");");
			}
			break;

		case EMR_STRETCHBLT:
			{
				EMRSTRETCHBLT * cmd = (EMRSTRETCHBLT *) emr;

				if ( cmd->cbBitsSrc==0 ) // no bitmap
					fmt.Format("PatBlt(hDC,%d,%d,%d,%d,", & cmd->xDest);
				else
				{
					TCHAR format[64];

					fmt.AddDIB( cmd, 
					        cmd->offBmiSrc,  cmd->cbBmiSrc,
				            cmd->offBitsSrc, cmd->cbBitsSrc,
							format, false);
				
					fmt.Newline();
					fmt.Format("StretchDIBits(hDC, %d,%d", & cmd->xDest);
					fmt.Format(",%d,%d",                   & cmd->cxDest);
					fmt.Format(", %d,%d,",				   & cmd->xSrc);

					if ( ! fmt.IsOpen() )
					{
						fmt.Format("%d,%d, pBits, pBMI",	   & cmd->cxSrc);
						fmt.Write(format);
					}
					else
						fmt.Format("%d,%d, #b, #B",			   & cmd->cxSrc);
					
					fmt.Format(", %L,", & cmd->iUsageSrc, Dic_DIBColor);
				}
				
				fmt.Write(Lookup(cmd->dwRop, Dic_ROP3));
				fmt.Write(");");
			}
			break;

		case EMR_EXTSELECTCLIPRGN:
			{
				EMREXTSELECTCLIPRGN * extselectcliprgn = (EMREXTSELECTCLIPRGN *) emr;

				if ( extselectcliprgn->cbRgnData == 0 )
					fmt.Write("SelectClipRgn(hDC, NULL);");
				else
				{
					fmt.AddRegion(extselectcliprgn->cbRgnData, (const RGNDATA *) & extselectcliprgn->RgnData);
					
					fmt.Newline();
					fmt.Format("ExtSelectClipRgn(hDC, hRegion, %L);", & extselectcliprgn->iMode, Dic_RegionOper);

					fmt.Newline();
					fmt.Write("DeleteObject(hRegion);");
				}
			}
			break;

/*		case EMR_SMALLTEXTOUT:
			{
				EMRSMALLTEXTOUT * smalltextout = (EMRSMALLTEXTOUT *) emr;

				fmt.Format("ExtTextOutA(hDC, %d,%d,",
							& smalltextout->ptlReference.x);

				fmt.WriteDec(smalltextout->fOptions & 0xFC00);
				fmt.Write(",NULL,");
				fmt.WriteString(smalltextout->Text, smalltextout->nChars, false);
				fmt.Write(",");
				fmt.WriteDec(smalltextout->nChars);
				fmt.Write(",NULL);");
			}
			break;
*/			
		case EMR_EXTTEXTOUTA:
		case EMR_EXTTEXTOUTW:
			{
				EMREXTTEXTOUTW * cmd = (EMREXTTEXTOUTW *) emr;

				if ( (cmd->emrtext.offDx!=0) && (cmd->emrtext.nChars!=0) )
				{
					fmt.Format("const int Dx_%m[]=", NULL);
					fmt.WriteArray(Offset(emr, cmd->emrtext.offDx), cmd->emrtext.nChars, sizeof(long));
					fmt.Put(';');

					fmt.Newline();
				}

				if (cmd->emrtext.fOptions & ( ETO_CLIPPED | ETO_OPAQUE) )
				{
					fmt.Format("const RECT Rect_%n={%d,%d,%d,%d};", & cmd->emrtext.rcl);
					fmt.Newline();
				}

				if (emr->iType == EMR_EXTTEXTOUTA)
					fmt.Write("ExtTextOutA");
				else
					fmt.Write("ExtTextOutW");
				
				fmt.Format("(hDC, %d,%d,", & cmd->emrtext.ptlReference);
				
				if ( cmd->emrtext.fOptions == 0)
					fmt.Put('0');
				else
					fmt.Write(cmd->emrtext.fOptions, Dic_ExtTextOption);
				
				fmt.Put(',');
				
				if (cmd->emrtext.fOptions & (ETO_CLIPPED | ETO_OPAQUE) )
					fmt.Format("& Rect_%n,", NULL);
				else
					fmt.Write("NULL,");

				if (emr->iType == EMR_EXTTEXTOUTW)
					fmt.Put('L');

				if (emr->iType == EMR_EXTTEXTOUTA)
					fmt.WriteString(Offset(emr, cmd->emrtext.offString), cmd->emrtext.nChars, false);
				else
					fmt.WriteString(Offset(emr, cmd->emrtext.offString), cmd->emrtext.nChars, true);

				fmt.Write(",");
				fmt.WriteDec(cmd->emrtext.nChars);

				if ( (cmd->emrtext.offDx!=0) && (cmd->emrtext.nChars!=0) )
					fmt.Format(",Dx_%m);", NULL);
				else
					fmt.Write(",NULL);");
			}
			break;

		case EMR_POLYPOLYGON16:
		case EMR_POLYPOLYLINE16:
		case EMR_POLYPOLYGON:
		case EMR_POLYPOLYLINE:
			{
				EMRPOLYPOLYGON * polypolygon = ( EMRPOLYPOLYGON *) emr;

				fmt.Format("static const int Count_%n[]=", 0);
				fmt.WriteArray( polypolygon->aPolyCounts, polypolygon->nPolys, sizeof(polypolygon->aPolyCounts[0]));
				fmt.Put(';');

				fmt.Newline();
				fmt.Format("static const int Vertex_%m[]=", 0);

				switch ( emr->iType )
				{
					case EMR_POLYPOLYGON16:
					case EMR_POLYPOLYLINE16:
						fmt.WriteArray(& polypolygon->aPolyCounts[polypolygon->nPolys], polypolygon->cptl * 2, sizeof(short));
						break;

					case EMR_POLYPOLYGON:
					case EMR_POLYPOLYLINE:
						fmt.WriteArray(& polypolygon->aPolyCounts[polypolygon->nPolys], polypolygon->cptl * 2, sizeof(long));
						break;
				}
				
				fmt.Put(';');

				fmt.Newline();
	
				switch ( emr->iType )
				{
					case EMR_POLYPOLYGON16:
					case EMR_POLYPOLYGON:
						fmt.Write("PolyPolygon");
						break;

					case EMR_POLYPOLYLINE16:
					case EMR_POLYPOLYLINE:
						fmt.Write("PolyPolyline");
						break;
				}
				fmt.Format("(hDC, (const POINT *) Vertex_%m, Count_%n, %d);", & polypolygon->nPolys);
			}
			break;

		case EMR_STRETCHDIBITS:
			{
				EMRSTRETCHDIBITS * cmd = (EMRSTRETCHDIBITS *) emr;

				TCHAR format[64];

				fmt.Newline();

				bool newdib = fmt.AddDIB( cmd, 
									cmd->offBmiSrc,  cmd->cbBmiSrc,
									cmd->offBitsSrc, cmd->cbBitsSrc,
									format, false);
				
				if ( ! fmt.IsOpen() )
				{
					fmt.Newline();
					fmt.Format("StretchDIBits(hDC, %d,%d", & cmd->xDest);
					fmt.Format(",%d,%d",                   & cmd->cxDest);

					fmt.Format(", %d,%d,%d,%d, pBits, pBMI",    & cmd->xSrc);
					fmt.Write(format);
				}
				else
				{
					if ( newdib )
					{
						fmt.Write("// ");
						fmt.Write(format);
						fmt.Newline();
					}

					TCHAR temp[32];
					wsprintf(temp, "Dib_%d.", fmt.m_curPackedDIB+1);
					fmt.Write(temp);

					fmt.Format("StretchDIBits(hDC, %d,%d", & cmd->xDest);
					fmt.Format(",%d,%d",                   & cmd->cxDest);
					fmt.Format(", %d,%d,%d,%d",			   & cmd->xSrc);
				}

				fmt.Format(", %L", & cmd->iUsageSrc, Dic_DIBColor);
				fmt.Format(", %L);", & cmd->dwRop,    Dic_ROP3);
			}
			break;

		case EMR_CREATEMONOBRUSH:
			{
				EMRCREATEMONOBRUSH * cmd = (EMRCREATEMONOBRUSH *) emr;

				assert( (cmd->offBmi + cmd->cbBmi) == cmd->offBits);

				fmt.Format("const unsigned long DIB_%n[]=", NULL);
				fmt.WriteArray(Offset(cmd, cmd->offBmi), (cmd->cbBmi + cmd->cbBits)/sizeof(long), sizeof(long), false);
				fmt.Put(';');
				
				fmt.Newline();
				fmt.Format("#o=CreateDIBPatternBrushPt((const BITMAPINFO *) DIB_%n,%L);", & cmd->ihBrush, Dic_DIBColor);
			}
			break;

		case EMR_EXTCREATEPEN:
			{
				EMREXTCREATEPEN * cmd = (EMREXTCREATEPEN *) emr;

				fmt.Format("static const LOGBRUSH LogBrush_%m = #R;", & cmd->elp.elpBrushStyle);
				
				fmt.Newline();
				fmt.Format("#o=ExtCreatePen(", & cmd->ihPen);

				assert(cmd->cbBmi == 0);
				assert(cmd->cbBits == 0);

				fmt.Write(Lookup(cmd->elp.elpPenStyle, Dic_PenStyle));
				fmt.Put(',');
				fmt.WriteDec(cmd->elp.elpWidth);
				
				fmt.Format(",& LogBrush_%m,%d,NULL);", & cmd->elp.elpNumEntries);
			}
			break;

		default:			
			fmt.Format("// Unknown record [%d]", & emr->iType);
	}

	return fmt.m_buffer;
}


// Decompile from an EMF file
bool KEmfDC::DeCompile(const TCHAR * outfile, const char * FileName, 
				  KTreeView * pTree, HENHMETAFILE & hEmf)
{
	if ( ! Open(FileName) )
		return false;
	
	return DeCompileBuffer(outfile, m_View, pTree, hEmf);
}


// Decompile from an EMF object
bool KEmfDC::DeCompile(const TCHAR * outfile, HENHMETAFILE hEmf, KTreeView * pTree)
{
	int size = GetEnhMetaFileBits(hEmf, 0, NULL);

	if ( size<=0 )
		return false;

	BYTE * pBits = new BYTE[size];

	HCURSOR hOld = SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

	GetEnhMetaFileBits(hEmf, size, pBits);
	bool rslt = DeCompileBuffer(outfile, pBits, pTree, hEmf);

	DestroyCursor(SetCursor(hOld));

	delete [] pBits;

	return rslt;
}


bool IsEMFHeader(const void * buffer)
{
	try
	{
		const ENHMETAHEADER * header = (const ENHMETAHEADER *) buffer;

		return ( (header->iType      == EMR_HEADER) && 
			     (header->nVersion   == 0x10000)    &&
			     (header->dSignature == ENHMETA_SIGNATURE) );
	}
	catch (...)
	{
		return false;
	}

	return false;
}


extern HINSTANCE hModule;

// main entry for decompiling from a memory buffer
bool KEmfDC::DeCompileBuffer(const TCHAR * outfilename, const void * buffer, KTreeView * pTree, HENHMETAFILE & hEmf)
{
	const EMR * emr = (const EMR *) buffer;
	
	// if not normal EMF file
	while ( ! IsEMFHeader(emr) )
	{
		if ( IsEMFHeader(emr+1) )
		{
			emr ++;

			if ( hEmf==NULL )
				hEmf = SetEnhMetaFileBits(emr[-1].nSize, (const BYTE *) emr);
			
			break;
		}
		else
			emr = (const EMR *) ( ( const char * ) emr + emr->nSize );
	}

	const ENHMETAHEADER * pHeader = (const ENHMETAHEADER *) emr;
	
	if ( pTree==NULL )
	{
		fmt.Open(outfilename);

		HRSRC hRsc = FindResource(hModule, MAKEINTRESOURCE(IDR_PRE), RT_RCDATA);
		
		if ( hRsc )
		{
			HGLOBAL hResData  = LoadResource(hModule, hRsc);
			const char * pPgm = (const char *) LockResource(hResData);

			fmt.Write(pPgm);
		}

		fmt.Indent(1);
		fmt.Newline(); fmt.Write("HGDIOBJ hObj["); fmt.WriteDec((long) pHeader->nHandles); fmt.Write("] = { NULL };");
		fmt.Newline();
	}
	m_nSeq = 1;

	bool bOptimize = false;

	// enumuerate all EMF records
	while ( (emr->iType>=EMR_MIN) && (emr->iType<=EMR_MAX) )
	{
		bool rslt = true;

		if ( bOptimize )
		{
			const EMR * next = (const EMR *) ( ( const char * ) emr + emr->nSize );

			if ( next->iType == emr->iType )
				switch ( emr->iType )
				{
					case EMR_SETWINDOWORGEX:
					case EMR_SETWINDOWEXTEX:
					case EMR_SETVIEWPORTORGEX:
					case EMR_SETVIEWPORTEXTEX:
					case EMR_SETTEXTCOLOR:
					case EMR_SETBKCOLOR:
					case EMR_SETBRUSHORGEX:
					case EMR_SELECTCLIPPATH:
					case EMR_SETTEXTALIGN:
	    
					case EMR_SETBKMODE:
					case EMR_SETARCDIRECTION:
					case EMR_SETPOLYFILLMODE:
					case EMR_SETMAPMODE:
					case EMR_SETSTRETCHBLTMODE:
					case EMR_SETMAPPERFLAGS:
					case EMR_SETICMMODE:
					case EMR_SETROP2:

					case EMR_SETMITERLIMIT:
					case EMR_SETWORLDTRANSFORM:
					case EMR_MOVETOEX:
						fmt.Write("/* */");
						break;
				
					default:
						rslt = Decode(emr, pTree);
				}
			else 
				rslt = Decode(emr, pTree);
		}
		else
			rslt = Decode(emr, pTree);
		
		if (! rslt ) 
			break;

		if ( emr->iType== EMR_EOF )
			break;

		emr = (const EMR *) ( ( const char * ) emr + emr->nSize );
	}
	
	if ( pTree==NULL )
	{
		fmt.Indent(-1);

		HRSRC hRsc = FindResource(hModule, MAKEINTRESOURCE(IDR_POST), RT_RCDATA);
		
		if ( hRsc )
		{
			HGLOBAL hResData  = LoadResource(hModule, hRsc);
			const char * pPgm = (const char *) LockResource(hResData);

			fmt.Write(pPgm);
		}

		fmt.Close();
	}
	
	return true;
}