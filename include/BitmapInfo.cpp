//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : bitmapinfo.cpp						                                 //
//  Description: Wrapper for BITMAPINFO structure                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <tchar.h>

#include "BitmapInfo.h"

const RGBQUAD StandardColorTable16[] = {
	{ 0x00, 0x00, 0x00, 0x00 }, 
	{ 0x00, 0x00, 0x80, 0x00 },
	{ 0x00, 0x80, 0x00, 0x00 },
	{ 0x00, 0x80, 0x80, 0x00 },
	{ 0x80, 0x00, 0x00, 0x00 },
	{ 0x80, 0x00, 0x80, 0x00 },
	{ 0x80, 0x80, 0x00, 0x00 },
	{ 0xC0, 0xC0, 0xC0, 0x00 },
	{ 0x80, 0x80, 0x80, 0x00 },
	{ 0x00, 0x00, 0xFF, 0x00 },
	{ 0x00, 0xFF, 0x00, 0x00 },
	{ 0x00, 0xFF, 0xFF, 0x00 },
	{ 0xFF, 0x00, 0x00, 0x00 },
	{ 0xFF, 0x00, 0xFF, 0x00 },
	{ 0xFF, 0xFF, 0x00, 0x00 },
	{ 0xFF, 0xFF, 0xFF, 0x00 }
};

// R G B
const DWORD StandardMask555[] =  { 0x007C00, 0x0003E0, 0x00001F };
const DWORD StandardMask565[] =  { 0x00F800, 0x0007E0, 0x00001F };
const DWORD StandardMask888[] =  { 0xFF0000, 0x00FF00, 0x0000FF };


void KBitmapInfo::SetFormat(int width, int height, int bitcount, int compression)
{
	// fill out BITMAPINFOHEADER based on size and required format
	memset(&m_dibinfo, 0, sizeof(m_dibinfo));

	m_dibinfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	m_dibinfo.bmiHeader.biWidth       = width;
	m_dibinfo.bmiHeader.biHeight      = height;
	m_dibinfo.bmiHeader.biPlanes      = 1;
	m_dibinfo.bmiHeader.biBitCount    = bitcount;
	m_dibinfo.bmiHeader.biCompression = compression;

	// set up standard palette
	switch ( bitcount )
	{
		case 2:
			m_dibinfo.bmiColors[0].rgbRed   = 0;	// black	
			m_dibinfo.bmiColors[0].rgbGreen = 0;
			m_dibinfo.bmiColors[0].rgbBlue  = 0;
			m_dibinfo.bmiColors[1].rgbRed   = 0xFF; // white
			m_dibinfo.bmiColors[1].rgbGreen = 0xFF;
			m_dibinfo.bmiColors[1].rgbBlue  = 0xFF;

		case 4:
			memcpy(m_dibinfo.bmiColors, StandardColorTable16, 16 * sizeof(RGBQUAD));
			break;

		case 8: // halftone palette with 6*6*6=216 colors
		{
			for (int blue =0; blue <6; blue ++)
			for (int green=0; green<6; green++)
			for (int red  =0; red  <6; red  ++)
			{
				int i = (blue * 6 + green) * 6 + red;

				m_dibinfo.bmiColors[i].rgbRed   = red   * 55;
				m_dibinfo.bmiColors[i].rgbGreen = green * 55;
				m_dibinfo.bmiColors[i].rgbBlue  = blue  * 55;
			}
			break;
		}

		case 16:
			if ( compression==BI_BITFIELDS )
				memcpy(m_dibinfo.bmiColors, StandardMask555, sizeof(StandardMask555));
			break;

		case 32:
			if ( compression==BI_BITFIELDS )
				memcpy(m_dibinfo.bmiColors, StandardMask888, sizeof(StandardMask888));
			break;
	}
}


void KBitmapInfo::SetGrayScale(DWORD mask)
{
	int nColor = 1 << m_dibinfo.bmiHeader.biBitCount;

	for (int i=0; i<nColor; i++)
	{
		m_dibinfo.bmiColors[i].rgbRed		= i & GetRValue(mask);
		m_dibinfo.bmiColors[i].rgbGreen		= i & GetGValue(mask);
		m_dibinfo.bmiColors[i].rgbBlue		= i & GetBValue(mask);
		m_dibinfo.bmiColors[i].rgbReserved  = 0;
	}		
}


BITMAPINFO * KBitmapInfo::CopyBMI(void) const
{
	int nColor;

	if ( m_dibinfo.bmiHeader.biCompression==BI_BITFIELDS )
		nColor = 3;
	else if ( m_dibinfo.bmiHeader.biBitCount <= 8 )
		nColor = 1 << m_dibinfo.bmiHeader.biBitCount;
	else
		nColor = 0;

	int nSize = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nColor;

	BITMAPINFO * pBMI = (BITMAPINFO *) new BYTE[nSize];

	if ( pBMI )
		memcpy(pBMI, & m_dibinfo, nSize);
	
	return pBMI;
}