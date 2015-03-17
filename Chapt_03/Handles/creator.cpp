//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : creator.cpp				                                         //
//  Description: Testing GDI object creation, Chapter 3                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define OEMRESOURCE
#define NOCRYPT
#define STRICT

#include <windows.h>
#include <tchar.h>

#include "Creator.h"
#include "gditable.h"


const LPCTSTR Commands [nTotalCommands] =
{
	_T("GetStockObject(BLACK_BRUSH)"),
	_T("GetStockObject(DKGRAY_BRUSH)"), 
	_T("GetStockObject(GRAY_BRUSH)"), 
	_T("GetStockObject(HOLLOW_BRUSH)"), 
	_T("GetStockObject(LTGRAY_BRUSH)"), 
	_T("GetStockObject(NULL_BRUSH)"), 
	_T("GetStockObject(WHITE_BRUSH)"), 
	_T("GetStockObject(BLACK_PEN)"),  
	_T("GetStockObject(NULL_PEN)"),  
	_T("GetStockObject(WHITE_PEN)"),  
	_T("GetStockObject(ANSI_FIXED_FONT)"),  
	_T("GetStockObject(ANSI_VAR_FONT)"),  
	_T("GetStockObject(DEVICE_DEFAULT_FONT)"),  
	_T("GetStockObject(DEFAULT_GUI_FONT)"),  
	_T("GetStockObject(OEM_FIXED_FONT)"),  
	_T("GetStockObject(SYSTEM_FONT)"),  
	_T("GetStockObject(SYSTEM_FIXED_FONT)"),  
	_T("GetStockObject(DEFAULT_PALETTE)"),  
	
	_T("CreatePen"),
	_T("CreatePenIndirect"),
	_T("ExtCreatePen"),
	
	_T("CreateBrushIndirect"),
	_T("CreateDIBPatternBrushPt"),
	_T("CreateHatchBrush"),
	_T("CreatePatternBrush"),
	_T("CreateSolidBrush"),
	_T("GetSysColorBrush"),
	
	_T("CreateFont"),
    _T("CreateFontIndirect"),

    _T("CreateEllipticRgn"),
    _T("CreateEllipticRgnIndirect"),
    _T("CreatePolygonRgn"),
    _T("CreatePolyPolygonRgn"),
    _T("CreateRectRgn"),
    _T("CreateRectRgnIndirect"),
    _T("CreateRoundRectRgn"),
    _T("ExtCreateRegion"),
	_T("PathToRegion"),
    
	_T("CreateDC"),
    _T("CreateIC"),
    _T("CreateCompatibleDC"),

    _T("CreatePalette"),
    _T("CreateHalftonePalette"),

    _T("CreateBitmap"),
    _T("CreateBitmapIndirect"),
    _T("CreateCompatibleBitmap"),
    _T("CreateDIBitmap"),
    _T("CreatDIBSection"),
    _T("CreateDiscardableBitmap"),

    _T("CreateEnhMetaFile")
};


const LOGPEN   logpen   = 
	{ PS_SOLID, 2, RGB(0x55, 0x55, 0x55) };

const LOGBRUSH logbrush = 
	{ BS_HATCHED, RGB(0xAA, 0x55, 0xFF), HS_VERTICAL };

const LOGFONT  logfont  = 
	{ 20, 0, 0, 0, FW_NORMAL,
      FALSE, FALSE, FALSE,
      ANSI_CHARSET,
      OUT_TT_PRECIS, 
      CLIP_DEFAULT_PRECIS,
      PROOF_QUALITY,
      VARIABLE_PITCH | FF_ROMAN,
      _T("Times Roman") };

const LOGPALETTE logpalette = 
	{ 0x300, 1, { 0x44, 0x55, 0x66, PC_EXPLICIT } };

const RECT rect = 
	{ 10, 20, 400, 200 };

const POINT polygon[]  = 
	{ { 100, 100 }, { 100, 200 }, { 200, 200 }, { 200, 100 }, { 100, 100 } };

const POINT ppolygon[] = 
	{	{ 100, 100 }, { 100, 200 }, { 200, 200 }, { 200, 100 }, { 100, 100 },
		{ 150, 150 }, { 150, 250 }, { 250, 250 }, { 250, 150 }, { 150, 150 }
	};	

const int vertex[] = 
	{ 5, 5 };

typedef struct
	{
		BITMAPINFOHEADER bmiHeader; 
		unsigned char    bmiBits[8*3*8];
	} CSmallDib;

// 8x8 24 bit DIB
const CSmallDib dib = 
	{ { sizeof(BITMAPINFOHEADER), 8, 8, 1, 24,  BI_RGB,
        8*3*8, 0, 0, 0, 0
      }, 

      { 0,0,0, 1,1,1, 2,2,2, 3,3,3, 4,4,4, 5,5,5, 6,6,6, 7,7,7,
        1,1,1, 2,2,2, 3,3,3, 4,4,4, 5,5,5, 6,6,6, 7,7,7, 0,0,0, 
        2,2,2, 3,3,3, 4,4,4, 5,5,5, 6,6,6, 7,7,7, 0,0,0, 1,1,1, 
        3,3,3, 4,4,4, 5,5,5, 6,6,6, 7,7,7, 0,0,0, 1,1,1, 2,2,2, 
        4,4,4, 5,5,5, 6,6,6, 7,7,7, 0,0,0, 1,1,1, 2,2,2, 3,3,3, 
        5,5,5, 6,6,6, 7,7,7, 0,0,0, 1,1,1, 2,2,2, 3,3,3, 4,4,4, 
        6,6,6, 7,7,7, 0,0,0, 1,1,1, 2,2,2, 3,3,3, 4,4,4, 5,5,5, 
        7,7,7, 0,0,0, 1,1,1, 2,2,2, 3,3,3, 4,4,4, 5,5,5, 6,6,6 
	}
	};

unsigned bits[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

const BITMAP bitmap =
	{ 0, 8, 8, 2, 1, 1, bits }; 
 
typedef struct SimpleRegion
{
	RGNDATAHEADER header;
	RECT		  rect;
} SimpleRegion;

const SimpleRegion region =
	{
		{ sizeof(RGNDATAHEADER),
          RDH_RECTANGLES,
		  1,
		  4,
		{ 0, 0, 100, 100 }
		},
		{ 10, 10, 90, 90 }
	};

HGDIOBJ CreateObject(int cmd, HDC hDC, int seq)
{
	void * dibbits;

	switch (cmd)
	{
		case obj_BLACK_BRUSH:		
			return GetStockObject(BLACK_BRUSH);
		
		case obj_DKGRAY_BRUSH:		
			return GetStockObject(DKGRAY_BRUSH);
		
		case obj_GRAY_BRUSH:		
			return GetStockObject(GRAY_BRUSH);
		
		case obj_HOLLOW_BRUSH:		
			return GetStockObject(HOLLOW_BRUSH);
		
		case obj_LTGRAY_BRUSH:		
			return GetStockObject(LTGRAY_BRUSH);
		
		case obj_NULL_BRUSH:		
			return GetStockObject(NULL_BRUSH);
		
		case obj_WHITE_BRUSH:  
			return GetStockObject(WHITE_BRUSH);
		
		case obj_BLACK_PEN:  
			return GetStockObject(BLACK_PEN);
		
		case obj_NULL_PEN:  
			return GetStockObject(NULL_PEN);
		
		case obj_WHITE_PEN:  
			return GetStockObject(WHITE_PEN);
		
		case obj_ANSI_FIXED_FONT: 
			return GetStockObject(ANSI_FIXED_FONT);
		
		case obj_ANSI_VAR_FONT: 
			return GetStockObject(ANSI_VAR_FONT);
		
		case obj_DEVICE_DEFAULT_FONT: 
			return GetStockObject(DEVICE_DEFAULT_FONT);
		
		case obj_DEFAULT_GUI_FONT: 
			return GetStockObject(DEFAULT_GUI_FONT);
		
		case obj_OEM_FIXED_FONT: 
			return GetStockObject(OEM_FIXED_FONT);
		
		case obj_SYSTEM_FONT: 
			return GetStockObject(SYSTEM_FONT);
		
		case obj_SYSTEM_FIXED_FONT: 
			return GetStockObject(SYSTEM_FIXED_FONT);
		
		case obj_DEFAULT_PALETTE: 
			return GetStockObject(DEFAULT_PALETTE);
	
		case obj_CreatePen: 
			return CreatePen(PS_SOLID, 2, RGB(0xAA, 0xAA, 0xAA));
		
		case obj_CreatePenIndirect: 
			return CreatePenIndirect(& logpen);
        
		case obj_ExtCreatePen: 
			{
				DWORD dwStyle[13] = { 1, 9, 6, 1, 1, 4, 
					                  1, 9, 6, 1, 1, 4, 1};

				return ExtCreatePen(PS_GEOMETRIC | PS_USERSTYLE, 5, & logbrush, 13, dwStyle);
			}

        case obj_CreateBrushIndirect: 
			return CreateBrushIndirect(& logbrush);
        
		case obj_CreateDIBPatternBrushPt: 
			return CreateDIBPatternBrushPt(& dib, DIB_RGB_COLORS); 
        
		case obj_CreateHatchBrush: 
			return CreateHatchBrush(HS_CROSS, RGB(0xAA, 0xFF, 0x55));
        
		case obj_CreatePatternBrush: 
		{
			HBITMAP hClose = LoadBitmap(NULL, MAKEINTRESOURCE(OBM_CLOSE));
			
			HGDIOBJ hResult = CreatePatternBrush(hClose);

			DeleteObject(hClose);

			return hResult;
        }

		case obj_CreateSolidBrush: 
			return CreateSolidBrush(RGB(0x22, 0x33, 0x44));
        
		case obj_GetSysColorBrush: 
			return GetSysColorBrush(seq);
	
        case obj_CreateFont: 
			return CreateFont(10, 0, 0, 0, FW_NORMAL,
                       FALSE, FALSE, FALSE, ANSI_CHARSET,
                       OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                       PROOF_QUALITY, VARIABLE_PITCH | FF_ROMAN, "Times Roman");
        
		case obj_CreateFontIndirect: 
			{ 
				HGDIOBJ hFont = CreateFontIndirect(& logfont);

             // trying to understand pUser
			 /*	CGDITable table;
			  	GDITableCell cell = table[hFont];

			  	HGDIOBJ hOld = SelectObject(hDC, hFont);
				
			  	GDITableCell cell1 = table[hFont];

				int width[26];
			  	GetCharWidth(hDC, 'A', 'Z', width);
				
			  	GDITableCell cell2 = table[hFont];
				
			  	SelectObject(hDC, hOld);
              */
				return hFont;
			}
        
		case obj_CreateEllipticRgn: 
			return CreateEllipticRgn(0, 0, 100, 100);
        
		case obj_CreateEllipticRgnIndirect: 
			return CreateEllipticRgnIndirect(& rect);
        
		case obj_CreatePolygonRgn: 
			return CreatePolygonRgn(polygon, sizeof(polygon)/ sizeof(polygon[0]), ALTERNATE);
        
		case obj_CreatePolyPolygonRgn: 
			return CreatePolyPolygonRgn(ppolygon, vertex, 2, ALTERNATE);
        
		case obj_CreateRectRgn: 
			return CreateRectRgn(10, 10, 200, 142);
        
		case obj_CreateRectRgnIndirect: 
			return CreateRectRgnIndirect(& rect);
        
		case obj_CreateRoundRectRgn: 
			return CreateRoundRectRgn(10, 10, 200, 400, 5, 5);
        
		case obj_ExtCreateRegion: 
			return ExtCreateRegion(NULL, sizeof(region), (RGNDATA *) & region);

		case obj_PatchToRegion: 
			{
				BeginPath(hDC);
				MoveToEx(hDC, 100, 200, NULL);
				LineTo(hDC, 150, 250);		//  *       *
				LineTo(hDC, 200, 200);      //  * *   * *
				LineTo(hDC, 200, 300);      //  *   *   *
				LineTo(hDC, 100, 300);      //  *       * 
				LineTo(hDC, 100, 200);      //  *********
				CloseFigure(hDC);
				EndPath(hDC);
				return PathToRegion(hDC);
			}

	    case obj_CreateDC: 
			return CreateDC("DISPLAY", NULL, NULL, NULL);
        
		case obj_CreateIC: 
			return CreateIC("DISPLAY", NULL, NULL, NULL);
        
		case obj_CreateCompatibleDC: 
			return CreateCompatibleDC(hDC);

        case obj_CreatePalette: 
			return CreatePalette(& logpalette);
        
		case obj_CreateHalftonePalette: 
			return CreateHalftonePalette(hDC);

        case obj_CreateBitmap: 
			return CreateBitmap(10, 10, 1, 1, dib.bmiBits);
        
		case obj_CreateBitmapIndirect: 
			return CreateBitmapIndirect(& bitmap);
        
		case obj_CreateCompatibleBitmap: 
			return CreateCompatibleBitmap(hDC, 100, 100);
        
		case obj_CreateDIBitmap: 
			return CreateDIBitmap(hDC, & dib.bmiHeader, 0, NULL, NULL, DIB_RGB_COLORS);
        
		case obj_CreateDIBSection: 
			return CreateDIBSection(hDC, (BITMAPINFO *) & dib, 
					    DIB_RGB_COLORS, & dibbits, NULL, 0);

        case obj_CreateDiscardableBitmap: 
			return CreateDiscardableBitmap(hDC, 10, 10);

        case obj_CreateEnhMetaFile: 
			return CreateEnhMetaFile(hDC, NULL, & rect, "In Memory metafile");
	}

	return (HGDIOBJ) 0xFFFFFFFF;
}

