//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : examemf.h							                                 //
//  Description: EMF data analysis			                                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KInputFile;

BOOL CheckNTSpoolEmfFile(KInputFile *emf, ENHMETAHEADER *header);

BOOL PlayNTEnhMetaFile(HDC hDC, KInputFile *emf, RECT *rect);
