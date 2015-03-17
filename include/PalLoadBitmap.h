//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : palloadbitmap.h				                                     //
//  Description: Load a bitmap under a logical palette                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

BITMAPINFO * LoadDIB(HINSTANCE hInst, LPCTSTR pBitmapName, bool & bNeedFree);

HBITMAP PaletteLoadBitmap(HINSTANCE hInst, LPCTSTR pBitmapName, HPALETTE hPalette);

HPALETTE CreateSystemPalette(void);
