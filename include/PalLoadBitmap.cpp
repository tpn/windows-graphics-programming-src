//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : palloadbitmap.cpp				                                     //
//  Description: Load a bitmap under a logical palette                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>

#include "palloadbitmap.h"

static int GetDIBColorCount(const BITMAPINFOHEADER & bmih)
{
	if ( bmih.biBitCount <= 8 )
		if ( bmih.biClrUsed )
			return bmih.biClrUsed;
		else
			return 1 << bmih.biBitCount;
	else if ( bmih.biCompression==BI_BITFIELDS )
		return 3 + bmih.biClrUsed;
	else
		return bmih.biClrUsed;
}

static BYTE * GetDIBPixelArray(BITMAPINFO * pDIB)
{
	return (BYTE *) & pDIB->bmiColors[GetDIBColorCount(pDIB->bmiHeader)];
}


BITMAPINFO * LoadDIB(HINSTANCE hInst, LPCTSTR pBitmapName, bool & bNeedFree)
{
	HRSRC	     hRes = FindResource(hInst, pBitmapName, RT_BITMAP);
	BITMAPINFO * pDIB;
	
	if ( hRes )
	{
		HGLOBAL hGlobal = LoadResource(hInst, hRes);
		pDIB = (BITMAPINFO *) LockResource(hGlobal);

		bNeedFree = false;
	}
	else
	{
		HANDLE handle = CreateFile(pBitmapName, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
		if ( handle == INVALID_HANDLE_VALUE )
			return NULL;

		BITMAPFILEHEADER bmFH;

		DWORD dwRead = 0;
		ReadFile(handle, & bmFH, sizeof(bmFH), & dwRead, NULL);

		if ( (bmFH.bfType == 0x4D42) && (bmFH.bfSize<=GetFileSize(handle, NULL)) )
		{
			pDIB = (BITMAPINFO *) new BYTE[bmFH.bfSize];
		
			if ( pDIB )
			{
				bNeedFree = true;
				ReadFile(handle, pDIB, bmFH.bfSize, & dwRead, NULL);
			}
		}
		CloseHandle(handle);
	}

	return pDIB;
}


HBITMAP PaletteLoadBitmap(HINSTANCE hInst, LPCTSTR pBitmapName, HPALETTE hPalette)
{
	bool bDIBNeedFree;
	BITMAPINFO * pDIB = LoadDIB(hInst, pBitmapName, bDIBNeedFree);

	int width     = pDIB->bmiHeader.biWidth;
	int height    = pDIB->bmiHeader.biHeight;

	HDC hMemDC    = CreateCompatibleDC(NULL);
	HBITMAP hBmp  = CreateBitmap(width, height, GetDeviceCaps(hMemDC, PLANES), GetDeviceCaps(hMemDC, BITSPIXEL), NULL);
	
	HGDIOBJ hOldBmp = SelectObject(hMemDC, hBmp);

	HPALETTE hOld = SelectPalette(hMemDC, hPalette, FALSE);
	RealizePalette(hMemDC);

	SetStretchBltMode(hMemDC, HALFTONE);
	StretchDIBits(hMemDC, 0, 0, width, height, 0, 0, width, height, GetDIBPixelArray(pDIB), pDIB, DIB_RGB_COLORS, SRCCOPY);

	SelectPalette(hMemDC, hOld, FALSE);
	SelectObject(hMemDC, hOldBmp);
	DeleteObject(hMemDC);
	
	if ( bDIBNeedFree )
		delete [] (BYTE *) pDIB;

	return hBmp;
}


HPALETTE CreateSystemPalette(void)
{
	LOGPALETTE * pLogPal = (LOGPALETTE *) new char[sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * 255];

	pLogPal->palVersion    = 0x300;
	pLogPal->palNumEntries = 256;

	HDC hDC = GetDC(NULL);

	GetSystemPaletteEntries(hDC, 0, 256, pLogPal->palPalEntry);
	
	ReleaseDC(NULL, hDC);

	HPALETTE hPal = CreatePalette(pLogPal);
	delete [] (char *) pLogPal;

	return hPal;
}
