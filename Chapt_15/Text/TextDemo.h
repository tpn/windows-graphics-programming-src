#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : textdemo.h						                                     //
//  Description: Text demo routines                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

void ZoomRect(HDC hDC, int x0, int y0, int x1, int y1, int dx, int dy, int zoom);

void Line(HDC hDC, int x0, int y0, int x1, int y1);
void Box(HDC hDC, int x0, int y0, int x1, int y1);
void disp(HDC hDC, int x, int y, const TCHAR * mess, int val);
void disp(HDC hDC, int x, int y, const TCHAR * mess);

void Demo_StockFonts(HDC hDC, const RECT * rcPaint);
void Demo_Term(HDC hDC, const RECT * rcPaint);
void Demo_CoordinateSystem(HDC hDC, const RECT * rcPaint);

void Test_LC(void);
void Test_Point(void);

void Demo_TextAlignment(HDC hDC, const RECT * rcPaint);
void Demo_RTL(HDC hDC, const RECT * rcPaint);
void Demo_Justification(HDC hDC, const RECT * rcPaint);
void Demo_CharacterWidth(HDC hDC, const RECT * rcPaint);
void Demo_CharacterPlacement(HDC HDC, const RECT * rcPaint);
void Demo_GlyphIndex(HDC hDC, const RECT * rcPaint);

void Demo_GlyphOutline(HDC hDC);

void Demo_TabbedTextOut(HDC hDC);
void Demo_Paragraph(HDC hDC, bool bPrecise);

void Demo_TextColor(HDC hDC, HINSTANCE hInst);
void Demo_TextStyle(HDC hDC);
void Demo_TextGeometry(HDC hDC);

void Demo_TextasBitmap(HDC hDC);
void Demo_TextasBitmap2(HDC hDC);

void Demo_TextasCurve(HDC hDC);
void Demo_TextasCurve2(HDC hDC);
