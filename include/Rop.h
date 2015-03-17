#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : rop.h						                                         //
//  Description: Raster operation, MaskBlt, PlgBlt, TransparentBlt                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

bool inline RopNeedsNoDestination(int Rop)
{
	return ((Rop & 0xAA) >> 1) == (Rop & 0x55);
}

bool inline RopNeedsNoSource(int Rop)
{
	return ((Rop & 0xCC) >> 2) == (Rop & 0x33);
}

bool inline RopNeedsNoPattern(int Rop)
{
	return ((Rop & 0xF0) >> 4) == (Rop & 0x0F);
}

inline bool D_independent(DWORD rop)
{
	return ((0xAA & rop)>>1)== (0x55 & rop);
}

inline bool S_independent(DWORD rop)
{
	return ((0xCC & rop)>>2)== (0x33 & rop);
}

class KImage;

DWORD GetRopCode(BYTE index);

// ChennelSplit using CDIB class
void ChannelSplit(KImage & dib, HDC hDC);

// Create a grayscale image (DIBSection) from one of the RGB channels in a DIB
// Mask should be RGB(255, 0, 0), RGB(0, 255, 0), or RGB(0, 0, 255)
HBITMAP ChannelSplit(const BITMAPINFO * pBMI, const void * pBits, COLORREF Mask, HDC hMemDC);

// Gradually display a DIB on a destination surface in 4 steps
void FadeIn(HDC hDC, int x, int y, int w, int h, const BITMAPINFO * pBMI, const void * pBits);

// dx, dy, dw, dh defines a destination rectangle
// sw, sh is the dimension of source rectangle
// sx, sy is the starting point winthin the source bitmap, which will be tiled to sw x sh in size
BOOL StretchTile(HDC      hDC, int dx, int dy, int dw, int dh, 
				 HBITMAP hSrc, int sx, int sy, int sw, int sh, 
				 DWORD rop);

BOOL G_PlgBlt(HDC hdcDest, const POINT * pPoint, 
			HDC hdcSrc, int nXSrc, int nYSrc, int nWidth, int nHeight, 
			HBITMAP hbmMask, int xMask, int yMask);

BOOL AffineBlt(HDC hdcDest, const POINT * pPoint, 
			   HDC hdcSrc, int nXSrc, int nYSrc, int nWidth, int nHeight, DWORD rop);


BOOL AffineTile(HDC hdcDest, const POINT * pPoint, 
			    HBITMAP hSrc, int xMask, int yMask, int nWidth, int nHeight, DWORD rop);

BOOL C_PlgBlt(HDC hdcDest, const POINT * pPoint, 
			HDC hdcSrc, int nXSrc, int nYSrc, int nWidth, int nHeight, 
			HBITMAP hbmMask, int xMask, int yMask);


void DrawCube(HDC hDC, int x, int y, int dh, int dx, int dy, HDC hMemDC, int w, int h, HBITMAP hMask, bool bSimulate);

void MaskCube(HDC hDC, int size, int x, int y, int w, int h, HBITMAP hBmp, HDC hMemDC, bool mask, bool bSimulate);



BOOL G_MaskBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
			 HDC hdcSrc,  int nXSrc,  int nYSrc, 
			 HBITMAP hbmMask, int xMask, int yMask,
			 DWORD dwRop);

// Use TransparentBlt to draw Icon
void TransparentBltDrawIcon(HDC hDC, int x, int y, HICON hIcon);

// big pattern brush
void MaskBitmapNT(HDC hDC, int x, int y, int width, int height, HBITMAP hMask, HDC hMemDC);

void MaskBltDrawIcon(HDC hDC, int x, int y, HICON hIcon);
