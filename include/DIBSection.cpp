//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dibsection.cpp						                                 //
//  Description: KDIBSection class, EMF rendering, AirBrush                          //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "DIBSection.h"
#include "BitmapInfo.h"

// Copy from DIB section color table to DIB color table
UINT KDIBSection::GetColorTable(void)
{
	int width, height;

	if ( (GetDepth()>8) || ! Prepare(width, height) ) // create memory DC
		return 0;

	return GetDIBColorTable(m_hMemDC, 0, m_nClrUsed, m_pRGBQUAD);
}


// Copy from DIB's color table to DIB section's color table
UINT KDIBSection::SetColorTable(void)
{
	int width, height;

	if ( (GetDepth()>8) || ! Prepare(width, height) ) // create memory DC
		return 0;

	return SetDIBColorTable(m_hMemDC, 0, m_nClrUsed, m_pRGBQUAD);
}


void KDIBSection::DecodeDIBSectionFormat(TCHAR desp[])
{
	DIBSECTION dibsec;

	if ( GetObject(m_hBitmap, sizeof(DIBSECTION), & dibsec) )
	{
		KDIB::DecodeDIBFormat(desp);
		_tcscat(desp, _T("   "));
		DecodeDDB(GetBitmap(), desp + _tcslen(desp));
	}
	else
		_tcscpy(desp, _T("Invalid DIB Section"));
}


BOOL KDIBSection::CreateDIBSection(HDC hDC, CONST BITMAPINFO * pBMI, UINT iUsage, HANDLE hSection, DWORD dwOffset)
{
	PVOID pBits = NULL;
	
	HBITMAP hBmp = ::CreateDIBSection(hDC, pBMI, iUsage, & pBits, hSection, dwOffset);

	int typ = GetObjectType(hBmp);
	int size = GetObject(hBmp, 0, NULL);

	if ( hBmp )
	{
		ReleaseDDB();	// free the previous object
		ReleaseDIB();	
		
		m_hBitmap = hBmp;	// Use DDB handle to store DIB Section Handle

//		MEMORY_BASIC_INFORMATION mbi;
//		VirtualQuery(pBits, & mbi, sizeof(mbi));

		int nColor = GetDIBColorCount(pBMI->bmiHeader);
		int nSize  = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * nColor;
		
		BITMAPINFO * pDIB = (BITMAPINFO *) new BYTE[nSize];

		if ( pDIB==NULL )
			return FALSE;

		memcpy(pDIB, pBMI, nSize);	// copy header and color table

		AttachDIB(pDIB, (PBYTE) pBits, DIB_BMI_NEEDFREE);
		GetColorTable();
		return TRUE;
	}
	else
		return FALSE;
}


// Search for the largest DIBSection 
HBITMAP LargestDIBSection(BITMAPINFO * pBMI)
{
	HBITMAP hBmp;

	int mins = 1;			// 1	   pixel
	int maxs = 1024 * 128;	// 16 Giga pixels

	while ( true ) // search for largest DDB
	{
		int mid = (mins + maxs)/2;

		pBMI->bmiHeader.biWidth = mid;
		pBMI->bmiHeader.biHeight= mid;

		hBmp = CreateDIBSection(NULL, pBMI, DIB_RGB_COLORS, NULL, NULL, NULL);

		if ( hBmp )
		{
			pBMI->bmiHeader.biWidth = mid+1;
			pBMI->bmiHeader.biHeight= mid+1;

			HBITMAP h = CreateDIBSection(NULL, pBMI, DIB_RGB_COLORS, NULL, NULL, NULL);

			if ( h==NULL )
				return hBmp;

			DeleteObject(h);
			DeleteObject(hBmp);

			mins = mid+1;
		}
		else
			maxs = mid;
	}

	return NULL;
}


// Draw a 3D rectangle frame with color shades
void Frame(HDC hDC, int nFrame, COLORREF crFrame, int left, int top, int right, int bottom)
{
	int red   = GetRValue(crFrame);
	int green = GetGValue(crFrame);
	int blue  = GetBValue(crFrame);
	RECT rect = { left, top, right, bottom };

	for (int i=0; i<nFrame; i++)
	{
		HBRUSH hBrush = CreateSolidBrush(RGB(red, green, blue));
		FrameRect(hDC, & rect, hBrush);
		DeleteObject(hBrush);

		if ( i<nFrame/2 )
		{
			red   = red   * 19/20;	// darker
			green = green * 19/20;
			blue  = blue  * 19/20;
		}
		else
		{
			red   = red   * 19/18; if ( red>255 )   red  = 255;	// lighter
			green = green * 19/18; if ( green>255 ) green= 255;
			blue  = blue  * 19/18; if ( blue>255 )  blue = 255;
		}

		InflateRect(&rect, -1, -1);
	}
}


// Window contents saver using DIBSECTION
BOOL SaveWindow(HWND hWnd, bool bClient, int nFrame, COLORREF crFrame)
{
	RECT wnd;

	if ( bClient )
	{
		if ( ! GetClientRect(hWnd, & wnd) )
			return FALSE;
	}
	else
	{
		if ( ! GetWindowRect(hWnd, & wnd) )
			return FALSE;
	}

	KBitmapInfo bmi;
	KDIBSection dibsec;

	bmi.SetFormat(wnd.right - wnd.left + nFrame * 2, wnd.bottom - wnd.top + nFrame *2, 24, BI_RGB);

	if ( dibsec.CreateDIBSection(NULL, bmi.GetBMI(), DIB_RGB_COLORS, NULL, NULL) )
	{
		int width, height;
		dibsec.Prepare(width, height); // creates memory DC, select dibsec into memDC

		if ( nFrame )
		{
			Frame(dibsec.m_hMemDC, nFrame, crFrame, 0, 0, width, height); 

			TCHAR Title[128];
			GetWindowText(hWnd, Title, sizeof(Title)/sizeof(Title[0]));
			SetBkMode(dibsec.m_hMemDC, TRANSPARENT);
			SetTextColor(dibsec.m_hMemDC, RGB(0xFF, 0xFF, 0xFF));
			TextOut(dibsec.m_hMemDC, nFrame, (nFrame-20)/2, Title, _tcslen(Title));
		}

		HDC hDC;
		if ( bClient )
			hDC = GetDC(hWnd);
		else
			hDC = GetWindowDC(hWnd);
		
		BitBlt(dibsec.m_hMemDC, nFrame, nFrame, width - nFrame * 2, height - nFrame * 2, hDC, 0, 0, SRCCOPY);
		ReleaseDC(hWnd, hDC);

		return dibsec.SaveFile(NULL);
	}
	return FALSE;
}


KTarga24::KTarga24()
{
	m_hFile	       = INVALID_HANDLE_VALUE;
	m_hFileMapping = INVALID_HANDLE_VALUE;
}


KTarga24::~KTarga24()
{
	ReleaseDDB();
	ReleaseDIB();

	if ( m_hFileMapping!=INVALID_HANDLE_VALUE )
		CloseHandle(m_hFileMapping);

	if ( m_hFile != INVALID_HANDLE_VALUE )
		CloseHandle(m_hFile);
}


BOOL KTarga24::Create(int width, int height, const TCHAR * pFileName)
{
	if ( width & 3 )	// avoid compatiblity problem with TGA
		return FALSE;

	ImageHeader tgaheader;

	memset(& tgaheader, 0, sizeof(tgaheader));
	tgaheader.IDLength  = sizeof(tgaheader.ID);
	tgaheader.ImgType   = 2;
	tgaheader.Width     = width;
	tgaheader.Height    = height;
	tgaheader.PixelSize = 24;
	strcpy(tgaheader.ID, "BitmapShop");

	m_hFile = CreateFile(pFileName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( m_hFile==INVALID_HANDLE_VALUE )
		return FALSE;

	int imagesize = (width*3+3)/4*4 * height;

	m_hFileMapping = CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, sizeof(tgaheader) + imagesize, NULL);
	if ( m_hFileMapping==INVALID_HANDLE_VALUE )
		return FALSE;

	DWORD dwWritten = NULL;
	WriteFile(m_hFile, & tgaheader, sizeof(tgaheader), &dwWritten, NULL);
	SetFilePointer(m_hFile, sizeof(tgaheader) + imagesize, 0, FILE_BEGIN);
	SetEndOfFile(m_hFile);

	KBitmapInfo bmi;

	bmi.SetFormat(width, height, 24, BI_RGB);

	return CreateDIBSection(NULL, bmi.GetBMI(), DIB_RGB_COLORS, m_hFileMapping, sizeof(tgaheader));
}


BOOL RenderEMF(HENHMETAFILE hemf, int width, int height, const TCHAR * tgaFileName)
{
	KTarga24 targa;

	int w = (width+3)/4*4;
	
	if ( targa.Create(w, height, tgaFileName) )
	{
		targa.Prepare(w, height);

		// DIBSECTION dibsec;
		// GetObject(targa.GetBitmap(), sizeof(DIBSECTION), & dibsec);

		BitBlt(targa.m_hMemDC, 0, 0, width, height, NULL, 0, 0, WHITENESS); // clear
		
		RECT rect = { 0, 0, width, height };

		for (int h=0; h<=height; h+=128 )
		{
			HRGN hRgn = CreateRectRgn(0, h, width, h+128);

			SelectClipRgn(targa.m_hMemDC, hRgn);
			DeleteObject(hRgn);
			
			if ( ! PlayEnhMetaFile(targa.m_hMemDC, hemf, &rect) ) 
				return FALSE;
		}
 	}
	
	return FALSE;
}


void KAirBrush::Apply(HDC hDC, int x, int y)
{
	BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

	AlphaBlend(hDC, x-m_nWidth/2, y-m_nHeight/2, m_nWidth, m_nHeight, m_hMemDC, 0, 0, m_nWidth, m_nHeight, blend);
}


void KAirBrush::Create(int width, int height, COLORREF color)
{
	Release();

	BYTE * pBits;
	BITMAPINFO Bmi = { { sizeof(BITMAPINFOHEADER), width, height, 1, 32, BI_RGB } };

	m_hBrush  = CreateDIBSection(NULL, & Bmi, DIB_RGB_COLORS, (void **) & pBits, NULL, NULL);
	m_hMemDC  = CreateCompatibleDC(NULL);
	m_hOld    = (HBITMAP) SelectObject(m_hMemDC, m_hBrush);
	m_nWidth  = width;
	m_nHeight = height;

	
	// solid color circle, on white background
	{	
		PatBlt(m_hMemDC, 0, 0, width, height, WHITENESS);

		HBRUSH hBrush = CreateSolidBrush(color);
		SelectObject(m_hMemDC, hBrush);
		SelectObject(m_hMemDC, GetStockObject(NULL_PEN));
		
		Ellipse(m_hMemDC, 0, 0, width, height);
		
		SelectObject(m_hMemDC, GetStockObject(WHITE_BRUSH));
		DeleteObject(hBrush);
	}
	
	BYTE * pPixel = pBits;

	for (int y=0; y<height; y++)
	for (int x=0; x<width;  x++, pPixel+=4)
	{
		int  dis   = (int) ( sqrt( (x-width/2) * (x-width/2) + (y-height/2) * (y-height/2) ) * 255 / (max(width, height)/2) );

		BYTE alpha = (BYTE) max(min(255-dis, 255), 0);

		pPixel[0] = pPixel[0] * alpha / 255;
		pPixel[1] = pPixel[1] * alpha / 255;
		pPixel[2] = pPixel[2] * alpha / 255;
		pPixel[3] = alpha;
	}
}

