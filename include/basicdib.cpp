//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : basicdib.cpp						                                 //
//  Description: Basic DIB handling                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "basicdib.h"


BITMAPINFO * LoadBMP(HMODULE hModule, const TCHAR * pResName)
{
	HRSRC   hRes = FindResource(hModule, pResName, RT_BITMAP);

	if ( hRes==NULL )
		return NULL;

	HGLOBAL hGlb = LoadResource(hModule, hRes);

	if ( hGlb==NULL )
		return NULL;

	return (BITMAPINFO *) LockResource(hGlb);
}


int GetDIBColorCount(const BITMAPINFO * pDIB)
{
	if ( pDIB->bmiHeader.biBitCount <= 8 )
		if ( pDIB->bmiHeader.biClrUsed )
			return pDIB->bmiHeader.biClrUsed;
		else
			return 1 << pDIB->bmiHeader.biBitCount;
	else if ( pDIB->bmiHeader.biCompression==BI_BITFIELDS )
		return 3 + pDIB->bmiHeader.biClrUsed;
	else
		return pDIB->bmiHeader.biClrUsed;
}


BITMAPINFO * LoadBMPFile(const TCHAR * pFileName)
{
	if ( pFileName==NULL )
		return NULL;

	HANDLE handle = CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if ( handle == INVALID_HANDLE_VALUE )
		return NULL;

    BITMAPFILEHEADER bmFH;

	DWORD dwRead = 0;
	ReadFile(handle, & bmFH, sizeof(bmFH), & dwRead, NULL);

	BITMAPINFO * pDIB = NULL;

	if ( (bmFH.bfType == 0x4D42) && (bmFH.bfSize<=GetFileSize(handle, NULL)) )
	{
		pDIB = (BITMAPINFO *) new BYTE[bmFH.bfSize];
		
		if ( pDIB )
			ReadFile(handle, pDIB, bmFH.bfSize, & dwRead, NULL);
	}
	CloseHandle(handle);

	return pDIB;
}

// 0.299 * red + 0.587 * green + 0.114 * blue 
inline void MaptoGray(BYTE & red, BYTE & green, BYTE & blue)
{	
	red   = ( red * 77 + green * 150 + blue * 29 + 128 ) / 256;	
	green = red;	
	blue  = red;
}

// map a 1-,2-,4-,8-bpp DIB to grayscale
BOOL MaptoGray(BITMAPINFO * pDIB)
{    
	if ( pDIB->bmiHeader.biBitCount>8 ) // reject high-color, true-color
		return FALSE;

	int nColor = pDIB->bmiHeader.biClrUsed;    // color used
	
	if ( nColor==0 )						   // zero for all	
		nColor = 1 << pDIB->bmiHeader.biBitCount;    
	
	for (int i=0; i<nColor; i++)			   // map each color to gray
		MaptoGray(pDIB->bmiColors[i].rgbRed,
				  pDIB->bmiColors[i].rgbGreen,
				  pDIB->bmiColors[i].rgbBlue);

	return TRUE;
}
