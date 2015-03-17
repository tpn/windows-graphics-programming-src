#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : creator.h			                                                 //
//  Description: Testing GDI obejct creation                                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

HGDIOBJ CreateObject(int cmd, HDC hDC=NULL, int seq=0);

typedef enum
{
	obj_BLACK_BRUSH,
	obj_DKGRAY_BRUSH,
	obj_GRAY_BRUSH,
	obj_HOLLOW_BRUSH,
	obj_LTGRAY_BRUSH,
	obj_NULL_BRUSH,
	obj_WHITE_BRUSH,
	obj_BLACK_PEN,
	obj_NULL_PEN,
	obj_WHITE_PEN,
	obj_ANSI_FIXED_FONT,
	obj_ANSI_VAR_FONT,
	obj_DEVICE_DEFAULT_FONT,
	obj_DEFAULT_GUI_FONT,
	obj_OEM_FIXED_FONT,
	obj_SYSTEM_FONT,
	obj_SYSTEM_FIXED_FONT,
	obj_DEFAULT_PALETTE,
	
	obj_CreatePen,
	obj_CreatePenIndirect,
	obj_ExtCreatePen,

	obj_CreateBrushIndirect,
	obj_CreateDIBPatternBrushPt,
	obj_CreateHatchBrush,
	obj_CreatePatternBrush,
	obj_CreateSolidBrush,
	obj_GetSysColorBrush,

	obj_CreateFont,
	obj_CreateFontIndirect,
        
	obj_CreateEllipticRgn,
	obj_CreateEllipticRgnIndirect,
	obj_CreatePolygonRgn,
	obj_CreatePolyPolygonRgn,
	obj_CreateRectRgn,
	obj_CreateRectRgnIndirect,
	obj_CreateRoundRectRgn,
	obj_ExtCreateRegion,
	obj_PatchToRegion,

	obj_CreateDC,
	obj_CreateIC,
	obj_CreateCompatibleDC,

	obj_CreatePalette,
	obj_CreateHalftonePalette,

    obj_CreateBitmap,
	obj_CreateBitmapIndirect,
	obj_CreateCompatibleBitmap,
	obj_CreateDIBitmap,
	obj_CreateDIBSection,

	obj_CreateDiscardableBitmap,

    obj_CreateEnhMetaFile,

	nTotalCommands
};

extern const LPCTSTR Commands [nTotalCommands];