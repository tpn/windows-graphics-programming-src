//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : ddb.cpp						                                     //
//  Description: Device Dependent Bitmap Wrapping Class                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "DDB.h"

// quey size, prepare memory DC, select bitmap into memory DC
bool KDDB::Prepare(int & width, int & height)
{
	BITMAP bmp;

	if ( ! GetObject(m_hBitmap, sizeof(bmp), & bmp) )
		return false;

	width  = bmp.bmWidth;
	height = bmp.bmHeight;

	if ( m_hMemDC==NULL )	// ensure memdc is created
	{
		HDC hDC = GetDC(NULL);
		m_hMemDC = CreateCompatibleDC(hDC);
		ReleaseDC(NULL, hDC);

		m_hOldBmp = (HBITMAP) SelectObject(m_hMemDC, m_hBitmap);
	}

	return true;
}

// Convert color DDB into monochrome mask based on a back ground color
HBITMAP KDDB::CreateMask(COLORREF crBackGround, HDC hMaskDC)
{
	int width, height;

	if ( ! Prepare(width, height) )
		return NULL;

	HBITMAP hMask   = CreateBitmap(width, height, 1, 1, NULL);
	HBITMAP hOld    = (HBITMAP) SelectObject(hMaskDC, hMask);

	SetBkColor(m_hMemDC, crBackGround);
	BitBlt(hMaskDC, 0, 0, width, height, m_hMemDC, 0, 0, SRCCOPY);

	return hOld;
}

// Relase resource
void KDDB::ReleaseDDB(void)
{
	if ( m_hMemDC )
	{
		SelectObject(m_hMemDC, m_hOldBmp);
		DeleteObject(m_hMemDC);
		m_hMemDC = NULL;
	}

	if ( m_hBitmap )
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}

	m_hOldBmp = NULL;
}


BOOL KDDB::Attach(HBITMAP hBmp)
{
	if ( hBmp==NULL )
		return FALSE;

	if ( m_hOldBmp )	// deselected m_hBitmap
	{
		SelectObject(m_hMemDC, m_hOldBmp);
		m_hOldBmp = NULL;
	}

	if ( m_hBitmap )	// delete current bitmap
		DeleteObject(m_hBitmap);

	m_hBitmap = hBmp;	// replace with new one

	if ( m_hMemDC )		// select if has memory DC
	{
		m_hOldBmp = (HBITMAP) SelectObject(m_hMemDC, m_hBitmap);
		return m_hOldBmp != NULL;
	}
	else
		return TRUE;
}


BOOL KDDB::Draw(HDC hDC, int x0, int y0, int w, int h, DWORD rop, int opt)
{
	int bmpwidth, bmpheight;

	if ( ! Prepare(bmpwidth, bmpheight) )
		return FALSE;

	switch ( opt )
	{
		case draw_normal:
			return BitBlt(hDC, x0, y0, bmpwidth, bmpheight, m_hMemDC, 0, 0, rop);

		case draw_center:
			return BitBlt(hDC, x0 + (w-bmpwidth)/2, y0 + ( h-bmpheight)/2,
				bmpwidth, bmpheight, m_hMemDC, 0, 0, rop);
			break;

		case draw_tile:
		{
			for (int j=0; j<h; j+= bmpheight)
			for (int i=0; i<w; i+= bmpwidth)
				if ( ! BitBlt(hDC, x0+i, y0+j, bmpwidth, bmpheight, m_hMemDC, 0, 0, rop) )
					return FALSE;

			return TRUE;
		}
		break;

		case draw_stretch:
			return StretchBlt(hDC, x0, y0, w, h, m_hMemDC, 0, 0, bmpwidth, bmpheight, rop);

		case draw_stretchprop:
		{
			int ww = w;
			int hh = h;

			if ( w * bmpheight < h * bmpwidth )	// w/bmWidth is the mimimum scale
				hh = bmpheight * w / bmpwidth;
			else
				ww = bmpwidth  * h / bmpheight;

			// propertional scaling and centering
			return StretchBlt(hDC, x0 + (w-ww)/2, y0 + (h-hh)/2, ww, hh, m_hMemDC, 0, 0, bmpwidth, bmpheight, rop);
		}

		default:
			return FALSE;
	}
}

// Generate a text description of DDB format
void DecodeDDB(HGDIOBJ hBmp, TCHAR mess[])
{
	BITMAP  bmp;

	if ( GetObject(hBmp, sizeof(bmp), & bmp) )
	{
		wsprintf(mess, _T("%dx%dx%dx%d w=%d, 0x%x"), bmp.bmWidth, bmp.bmHeight,
			bmp.bmPlanes, bmp.bmBitsPixel, bmp.bmWidthBytes, bmp.bmBits);

		int size = bmp.bmWidthBytes * bmp.bmHeight;

		if ( size < 1024 )
			wsprintf(mess+_tcslen(mess), _T(", %d b"), size);
		else if ( size < 1024 * 1024 )
			wsprintf(mess+_tcslen(mess), _T(", %d,%03d b"), size/1024, size%1024);
		else
			wsprintf(mess+_tcslen(mess), _T(", %d,%03d,%03d b"), size/1024/1024, size/1024%1024, size%1024);
	}
	else
		_tcscpy(mess, _T("Failed"));
}

// Search for the largest DDB compatible with a DC
HBITMAP LargestDDB(HDC hDC)
{
	HBITMAP hBmp;

	int mins = 1;			// 1	   pixel
	int maxs = 1024 * 128;	// 16 Giga pixels

	while ( true ) // search for largest DDB
	{
		int mid = (mins + maxs)/2;

		hBmp = CreateCompatibleBitmap(hDC, mid, mid);

		if ( hBmp )
		{
			HBITMAP h = CreateCompatibleBitmap(hDC, mid+1, mid+1);

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


// Create a monochrome mask bitmap from a source DC
BOOL KDDBMask::Create(HDC hDC, int nX, int nY, int nWidth, int nHeight, UINT crTransparent)
{
	Release();

	RECT rect = { nX, nY, nX + nWidth, nY + nHeight };
	LPtoDP(hDC, (POINT *) & rect, 2);								  
	
	m_nMaskWidth  = abs(rect.right - rect.left);
	m_nMaskHeight = abs(rect.bottom - rect.top);					  // get real size

	m_hMemDC = CreateCompatibleDC(hDC);
	m_hMask  = CreateBitmap(m_nMaskWidth, m_nMaskHeight, 1, 1, NULL); // monochrome bitmap
	m_hOld   = (HBITMAP) SelectObject(m_hMemDC, m_hMask);

	COLORREF oldBk = SetBkColor(hDC, crTransparent);	// map crTransparent to 1, white
	BOOL rslt = StretchBlt(m_hMemDC, 0, 0, m_nMaskWidth, m_nMaskHeight, hDC, nX, nY, nWidth, nHeight, SRCCOPY);
	SetBkColor(hDC, oldBk);

	return rslt;
}
	

BOOL KDDBMask::ApplyMask(HDC hDC, int nX, int nY, int nWidth, int nHeight, DWORD rop)
{
	COLORREF oldFore = SetTextColor(hDC, RGB(0, 0, 0));			// Foreground Black
	COLORREF oldBack = SetBkColor(hDC,   RGB(255, 255, 255));	// Background White

	BOOL rslt = StretchBlt(hDC, nX, nY, nWidth, nHeight, m_hMemDC, 0, 0, m_nMaskWidth, m_nMaskHeight, rop);

	SetTextColor(hDC, oldFore);
	SetBkColor(hDC,   oldBack);

	return rslt;
}


// D=D^S, D=D & Mask, D=D^S	--> if (Mask==1) D else S
BOOL KDDBMask::TransBlt(HDC hdcDest, int nDx0, int nDy0, int nDw, int nDh,
						HDC hdcSrc,  int nSx0, int nSy0, int nSw, int nSh)
{
	StretchBlt(hdcDest, nDx0, nDy0, nDw, nDh, hdcSrc, nSx0, nSy0, nSw, nSh, SRCINVERT);		// D^S
	
	ApplyMask(hdcDest, nDx0, nDy0, nDw, nDh, SRCAND);	// if trans D^S else 0

	return StretchBlt(hdcDest, nDx0, nDy0, nDw, nDh, hdcSrc, nSx0, nSy0, nSw, nSh, SRCINVERT);	// if trans D else S
}


BOOL KDDBMask::TransBlt_FlickFree(HDC hdcDest, int nDx0, int nDy0, int nDw, int nDh,
								  HDC hdcSrc,  int nSx0, int nSy0, int nSw, int nSh)
{
	StretchBlt(hdcSrc, nSx0, nSy0, nSw, nSh, 
		       hdcDest, nDx0, nDy0, nDw, nDh, SRCINVERT); // D^S

	ApplyMask(hdcSrc, nSx0, nSy0, nSw, nSh,	0x220000); // if trans 0 else D^S

	return StretchBlt(hdcDest, nDx0, nDy0, nDw, nDh, 		// if trans D else S
		hdcSrc, nSx0, nSy0, nSw, nSh, SRCINVERT);
}


BOOL G_TransparentBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
				    HDC hdcSrc,  int nXOriginSrc,  int nYOriginSrc,  int nWidthSrc,  int nHeightSrc,
					UINT crTransparent)
{
	KDDBMask mask;

	mask.Create(hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, crTransparent);

	return mask.TransBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest,
				hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc);
}

