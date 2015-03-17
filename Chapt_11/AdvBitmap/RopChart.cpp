//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : ropchart.cpp					                                     //
//  Description: Raster operation chart                                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define NOCRYPT

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "..\..\include\Rop.h"

#define TextH  20
#define xx(j)  (j)*(8*scale+10*scale)+32                      
#define yy(i)  (i+2)*(8*scale+(20*3+23) & 0xFF8)+32


void DispText(HDC hdc, int x, int y, LPCSTR mess)
{   
    if (mess!=NULL)
        TextOut(hdc, x, y, mess, _tcslen(mess));
}


void DispRectangle(HDC hdc, RECT *rect, short offset, COLORREF color)
{
    HPEN pold;
    
    pold=(HPEN) SelectObject(hdc, CreatePen(PS_SOLID, 1, color));
    
	HGDIOBJ hOldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
	Rectangle(hdc, rect->left-offset, rect->top-offset, rect->right+offset, rect->bottom+offset);
    SelectObject(hdc, hOldBrush);

    DeleteObject(SelectObject(hdc, pold));            
}


void DispBmp(HDC hdc, HDC memdc, int x, int y, short scale, HBITMAP bmp, DWORD rop)
{
    BITMAP  bmpinfo;
    HGDIOBJ oldbmp;
    
    GetObject(bmp, sizeof(BITMAP), &bmpinfo);
    oldbmp = SelectObject(memdc, bmp);
    
    StretchBlt(hdc, 
               x, y, bmpinfo.bmWidth*scale, bmpinfo.bmHeight*scale, memdc, 
               0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, 
               rop);
               
    SelectObject(memdc, oldbmp);
}

const WORD Bit_Pattern    [] = { 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F };
const WORD Bit_Source     [] = { 0xCC, 0xCC, 0x33, 0x33, 0xCC, 0xCC, 0x33, 0x33 };
const WORD Bit_Destination[] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };


void TestRop3(HINSTANCE hInstance, HDC hDC)
{   
    HBITMAP Pbmp = CreateBitmap(8, 8, 1, 1, Bit_Pattern);
	HBITMAP Sbmp = CreateBitmap(8, 8, 1, 1, Bit_Source);
	HBITMAP Dbmp = CreateBitmap(8, 8, 1, 1, Bit_Destination);
	HBITMAP Rbmp = CreateBitmap(8, 8, 1, 1, NULL);
	
    HBRUSH  Pat  = CreatePatternBrush(Pbmp);	// pattern brush
    HDC		Src	 = CreateCompatibleDC(hDC);		// memdc for source
	HDC		Dst	 = CreateCompatibleDC(hDC);		// memdc for destination
	HDC     Rst  = CreateCompatibleDC(hDC);		// memdc for result

	SelectObject(Src, Sbmp);
	SelectObject(Dst, Dbmp);
	SelectObject(Rst, Pbmp);

	StretchBlt(hDC, 20,  20, 80, 80, Rst, 0, 0, 8, 8, SRCCOPY);
	StretchBlt(hDC, 20, 220, 80, 80, Src, 0, 0, 8, 8, SRCCOPY);
	StretchBlt(hDC, 20, 420, 80, 80, Dst, 0, 0, 8, 8, SRCCOPY);

	SetBkMode(hDC, TRANSPARENT);
	TextOut(hDC, 20, 105, "Pattern", 7);
	TextOut(hDC, 20, 305, "Source",  6);
	TextOut(hDC, 20, 505, "Destination", 11);

	SelectObject(Rst, Rbmp);
    SelectObject(Rst, Pat);
   	
	char mess[3];

	for (int i=0; i<16; i++)
	{
		wsprintf(mess, "%02X", i);
		TextOut(hDC, 140 + i*38, 10, mess, 2);

		wsprintf(mess, "%02X", i*16);
		TextOut(hDC, 115, 30+i*38, mess, 2);
	}

	for (int rop=0; rop<256; rop++)
    { 
		BitBlt(Rst, 0, 0, 8, 8, Dst, 0, 0, SRCCOPY);
		BitBlt(Rst, 0, 0, 8, 8, Src, 0, 0, GetRopCode(rop));

        StretchBlt(hDC, 140 + (rop%16)*38, 30 + (rop/16)*38, 32, 32, Rst, 0, 0, 8, 8, SRCCOPY);
    }

	DeleteObject(Src);
	DeleteObject(Dst);
	DeleteObject(Rst);
	DeleteObject(Pat);
    DeleteObject(Pbmp);
	DeleteObject(Sbmp);
	DeleteObject(Dbmp);
	DeleteObject(Rbmp);
}


void TestRop4(HINSTANCE hInstance, HDC hDC)
{   
    int      oldbk;
    COLORREF oldcl;
    HDC      memdc;
    BITMAP   bmpinfo;
//  char     mess[20];
    int      scale;
    int x,y;
    HBITMAP Pbmp, Sbmp, Dbmp;
    HBRUSH  hbr, hbrOld;          
    RECT    rect;
    
    memdc= CreateCompatibleDC(hDC);
//  Pbmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_PATTERN));
//  Dbmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_DESTINATION));
//  Sbmp = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_SOURCE));

    Pbmp = CreateBitmap(8, 8, 1, 1, Bit_Pattern);
	Sbmp = CreateBitmap(8, 8, 1, 1, Bit_Source);
	Dbmp = CreateBitmap(8, 8, 1, 1, Bit_Destination);
	
    hbr  = CreatePatternBrush(Pbmp);
    
    hbrOld = (HBRUSH)SelectObject(hDC, hbr);
    
    oldbk=SetBkMode(hDC, OPAQUE);
    oldcl=SetBkColor(hDC, RGB(255, 255, 255));
    
    GetObject(Sbmp, sizeof(BITMAP), &bmpinfo);
//  if (IsPrinting(hDC))
//      scale=2;
//  else
        scale=4;
    
    y=yy(-2);
    x=10;

    DispText(hDC, x, y+20,        "SelectObject(CreatePatternBrush(Pattern))");
    DispText(hDC, x, y+20*2,      "StretchBlt(Destination, SRCCOPY)");
    DispText(hDC, x, y+20*3,      "StretchBlt(Source, Rop3)");

    DispText(hDC, x=xx(7), y-20, "Pattern");
	DispBmp(hDC, memdc, x, y, scale, Pbmp, SRCCOPY);

    DispText(hDC, x=xx(10), y-20, "Source");    
    DispBmp(hDC, memdc, x, y, scale, Sbmp, SRCCOPY);

    DispText(hDC, x=xx(13), y-20, "Destination");
    DispBmp(hDC, memdc, x, y, scale, Dbmp, SRCCOPY);
       
    
    DispText(hDC, x=xx(4), y-TextH,      "P: 1 1 1 1  0 0 0 0");
    DispText(hDC, x,       y,            "S: 1 1 0 0  1 1 0 0");
    DispText(hDC, x,       y+TextH,      "D: 1 0 1 0  1 0 1 0");
    DispText(hDC, x,       y+TextH*2+10, "R: 1 1 1 0  0 0 1 0");

    y=yy(-1);
    DispText(hDC, x=xx(13),  y-TextH*2,  "RopNeedsNoDestination");
    DispText(hDC, x,        y-TextH,    "((Rop & 0xAA) >> 1) == (Rop & 0x55)");
    rect.left  = x;
    rect.top   = y;
    rect.right = rect.left + 32*scale;
    rect.bottom= rect.top  + 32*scale;
    DispRectangle(hDC, &rect, 0, RGB(0xFF,0x00, 0x00)); 
        
    DispText(hDC, x=xx(10),  y-TextH*2,  "RopNeedsNoSource");
    DispText(hDC, x,        y-TextH,    "((Rop & 0xCC) >> 2) == (Rop & 0x33)");
    rect.left  = x;
    rect.top   = y;
    rect.right = rect.left + 32*scale;
    rect.bottom= rect.top  + 32*scale;
    DispRectangle(hDC, &rect, 0, RGB(0x00, 0xFF, 0x00));
    
    DispText(hDC, x=xx(7), y-TextH*2,  "RopNeedsNoPattern");
    DispText(hDC, x,        y-TextH,    "((Rop & 0xF0) >> 4) == (Rop & 0x0F)");
    rect.left  = x;
    rect.top   = y;
    rect.right = rect.left + 32*scale;
    rect.bottom= rect.top  + 32*scale;
    DispRectangle(hDC, &rect, 0, RGB(0x00, 0x00, 0xFF));
/*    
    for (int i=0; i<16; i++)
    for (int j=0; j<16; j++)
    { 
		x   = xx(j); 
        y   = yy(i);
        unsigned char rop = i*16 + j;
    
        DispText(hDC, x, y-TextH*3-2, RopDict[rop].name);
        DispText(hDC, x, y-TextH*2-3, RopDict[rop].formula);
        
        wsprintf(mess,"%06lx",   RopDict[rop].ropcode);
        DispText(hDC, x, y-TextH-4, mess);
    
		POINT p = { x, y };
		LPtoDP(hDC, &p, 1);
		SetBrushOrgEx(hDC, p.x, p.y, NULL);

        DispBmp(hDC, memdc, x, y, scale, dbmp, SRCCOPY);
        DispBmp(hDC, memdc, x, y, scale, sbmp, RopDict[rop].ropcode);
        
        rect.left   = x;
        rect.top    = y;
        rect.right  = rect.left + bmpinfo.bmWidth*scale  ;
        rect.bottom = rect.top  + bmpinfo.bmHeight*scale ;
        
        if ( strchr(RopDict[rop].formula, 'D')==NULL )
			DispRectangle(hDC, &rect, 3, RGB(0xFF,0x00, 0x00)); 
        
        if ( strchr(RopDict[rop].formula,'S')==NULL )
			DispRectangle(hDC, &rect, 4, RGB(0x00, 0xFF, 0x00));
          
        if ( strchr(RopDict[rop].formula,'P')==NULL )
			DispRectangle(hDC, &rect, 5, RGB(0x00, 0x00, 0xFF));
    }
*/

	HBITMAP hBmp = CreateBitmap(8, 8, 1, 1, NULL);
	SelectObject(memdc, hBmp);
	SelectObject(memdc, hbr);

	HDC S = CreateCompatibleDC(hDC); SelectObject(S, Sbmp);
	HDC D = CreateCompatibleDC(hDC); SelectObject(D, Dbmp);
    
	for (int i=0; i<16; i++)
    for (int j=0; j<16; j++)
    { 
		x   = xx(0) + i*42; 
        y   = yy(0) + j*42;
        int rop = i*16 + j;

		BitBlt(memdc, 0, 0, 8, 8, D, 0, 0, SRCCOPY);
		BitBlt(memdc, 0, 0, 8, 8, S, 0, 0, GetRopCode(rop));

        StretchBlt(hDC, x, y, 32, 32, memdc, 0, 0, 8, 8, SRCCOPY);
        
        rect.left   = x;
        rect.top    = y;
        rect.right  = rect.left + 32;
        rect.bottom = rect.top  + 32;
        
        if ( RopNeedsNoDestination(rop) )
			DispRectangle(hDC, &rect, 3, RGB(0xFF,0x00, 0x00)); 
        
        if ( RopNeedsNoSource(rop) )
			DispRectangle(hDC, &rect, 4, RGB(0x00, 0xFF, 0x00));
          
        if ( RopNeedsNoPattern(rop) )
			DispRectangle(hDC, &rect, 5, RGB(0x00, 0x00, 0xFF));
    }
      

    SetBkMode(hDC, oldbk);
    SetBkColor(hDC, oldcl);
      
    DeleteObject(SelectObject(hDC, hbrOld));  
    DeleteObject(memdc);
    DeleteObject(Sbmp);
    DeleteObject(Dbmp);
    DeleteObject(Pbmp);

//  if ( IsPrinting(hDC) )
//    SetScrollSizes(MM_TEXT, CSize(xx(16), yy(16)), CSize(40, 40), CSize(8,8));
}

