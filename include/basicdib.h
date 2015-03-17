#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : basicdib.h						                                     //
//  Description: Basic DIB handling                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

BITMAPINFO * LoadBMP(HMODULE hModule, const TCHAR * pResName);
BITMAPINFO * LoadBMPFile(const TCHAR * pFileName);

int			 GetDIBColorCount(const BITMAPINFO * pDIB);
BOOL		 MaptoGray(BITMAPINFO * pDIB);
