#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : areafill.h					 	                                     //
//  Description: Gradient fill, pattern fill                                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

inline COLOR16 R16(COLORREF c) { return GetRValue(c)<<8; }
inline COLOR16 G16(COLORREF c) { return GetGValue(c)<<8; }
inline COLOR16 B16(COLORREF c) { return GetBValue(c)<<8; }

inline COLOR16 R16(COLORREF c0, COLORREF c1) { return ((GetRValue(c0)+GetRValue(c1))/2)<<8; }
inline COLOR16 G16(COLORREF c0, COLORREF c1) { return ((GetGValue(c0)+GetGValue(c1))/2)<<8; }
inline COLOR16 B16(COLORREF c0, COLORREF c1) { return ((GetBValue(c0)+GetBValue(c1))/2)<<8; }


BOOL GradientRectangle(HDC hDC, int x0, int y0, int x1, int y1, COLORREF c0, COLORREF c1, int angle);

BOOL SymGradientRectangle(HDC hDC, int x0, int y0, int x1, int y1, COLORREF c0, COLORREF c1, int angle);

BOOL CornerGradientRectangle(HDC hDC, int x0, int y0, int x1, int y1, COLORREF c0, COLORREF c1, int corner);

BOOL CenterGradientRectangle(HDC hDC, int x0, int y0, int x1, int y1, COLORREF c0, COLORREF c1);


void HLSGradientRectangle(HDC hDC, int x0, int y0, int x1, int y1, COLORREF cref0, COLORREF cref1, int nPart);


BOOL RadialGradientFill(HDC hDC, int x0, int y0, int x1, int y1, int r, COLORREF c0, COLORREF c1, int nPart);

void BrickPatternFill(HDC hDC, int x0, int y0, int x1, int y1, int width, int height);

void RoundRectButton(HDC hDC, int x0, int y0, int x1, int y1, int w, int d, COLORREF c1, COLORREF c0);
