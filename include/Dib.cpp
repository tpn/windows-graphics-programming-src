//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dib.cpp						                                     //
//  Description: Device independent Bitmap Handling                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define WINVER       0x500
#define NOCRYPT
#define OEMRESOURCE

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "Color.h"
#include "Dib.h"
#include "Affine.h"
#include "BitmapInfo.h"
#include "filedialog.h"

const TCHAR * mess_DIBFormat[] =
{
	_T("DIB_1BPP"),			//   2 color image, palette-based
	_T("DIB_2BPP"),			//   4 color image, palttte-based
	_T("DIB_4BPP"),			//  16 color image, palette-based
	_T("DIB_4BPPRLE"),		//  16 color image, palette-based, RLE compressed
	_T("DIB_8BPP"),			// 256 color image, palette-based 
	_T("DIB_8BPPRLE"),		// 256 color image, palette-based, RLE compressed

	_T("DIB_16RGB555"),		// 15 bit RGB color image, 5-5-5
	_T("DIB_16RGB565"),		// 16 bit RGB color image, 5-6-5, 1 bit unused
	_T("DIB_24RGB888"),		// 24 bit RGB color image, 8-8-8
	_T("DIB_32RGB888"),		// 32 bit RGB color image, 8-8-8, 8 bit unused

	_T("DIB_32RGBA8888"),	// 32 bit RGBA color image, 8-8-8-8

	_T("DIB_16RGBbitfields"),	// 16 bit RGB color image, non-standard bit masks, NT-only
	_T("DIB_32RGBbitfields"),	// 32 bit RGB color image, non-standard bit masks, NT-only

	_T("DIB_JPEG"),				// embedded JPEG image
	_T("DIB_PNG")				// embedded PNG image
};

const TCHAR * PixelFormatName(int id)
{
	if ( (id>=DIB_1BPP) && (id<=DIB_PNG) )
		return mess_DIBFormat[id - DIB_1BPP];
	else
		return _T("Unknown Pixel Format");
}

KDIB::KDIB()
{
	m_pBMI         = NULL;
	m_pBits        = NULL;
	m_Flags		   = 0;

	m_nWidth       = 0;
	m_nHeight      = 0;
	m_nPlanes      = 1;
	m_nBitCount    = 1;
	m_nColorDepth  = 1;
	m_nImageSize   = 0;
	m_nImageFormat = DIB_1BPP;

	m_pRGBTRIPLE   = NULL;
	m_pRGBQUAD	   = NULL;
}

void KDIB::ReleaseDIB(void)
{
	if ( m_Flags & DIB_BMI_NEEDFREE )
	{
		delete [] (BYTE *) m_pBMI;
		m_Flags &= ~ DIB_BMI_NEEDFREE;
		m_pBMI   = NULL;
	}

	if ( m_Flags & DIB_BITS_NEEDFREE )
	{
		delete [] m_pBits;
		m_Flags &= ~ DIB_BITS_NEEDFREE;
		m_pBits = NULL;
	}
}

KDIB::~KDIB()
{
	try
	{
		ReleaseDIB();
	}
	catch (...)
	{
	}
}

bool KDIB::AttachDIB(BITMAPINFO * pDIB, BYTE * pBits, int flags)
{
	if ( IsBadReadPtr(pDIB, sizeof(BITMAPCOREHEADER)) )
		return false;

	ReleaseDIB();
	
	m_pBMI      = pDIB;
	m_Flags     = flags;

	DWORD size = * (DWORD *) pDIB; // always DWORD size, key to information header

	int compression;
	// gather information from bitmap information header structures
	switch ( size )
	{
		case sizeof(BITMAPCOREHEADER):
		{
			BITMAPCOREHEADER * pHeader = (BITMAPCOREHEADER *) pDIB;

			m_nWidth    = pHeader->bcWidth;
			m_nHeight   = pHeader->bcHeight;
			m_nPlanes   = pHeader->bcPlanes;
			m_nBitCount = pHeader->bcBitCount;
			m_nImageSize= 0;
			compression = BI_RGB;

			if ( m_nBitCount <= 8 )
			{
				m_nClrUsed   = 1 << m_nBitCount;
				m_nClrImpt   = m_nClrUsed;
				m_pRGBTRIPLE = (RGBTRIPLE *) ((BYTE *) m_pBMI + size);

				m_pBits      = (BYTE *) & m_pRGBTRIPLE[m_nClrUsed];
			}
			else
				m_pBits      = (BYTE *) m_pBMI + size;
			break;
		}

		case sizeof(BITMAPINFOHEADER):
		case sizeof(BITMAPV4HEADER):
		case sizeof(BITMAPV5HEADER):
		{
			BITMAPINFOHEADER * pHeader = & m_pBMI->bmiHeader;

			m_nWidth    = pHeader->biWidth;
			m_nHeight   = pHeader->biHeight;
			m_nPlanes   = pHeader->biPlanes;
			m_nBitCount = pHeader->biBitCount;
			m_nImageSize= pHeader->biSizeImage;
			compression = pHeader->biCompression;

			m_nClrUsed  = pHeader->biClrUsed;
			m_nClrImpt  = pHeader->biClrImportant;

			if ( m_nBitCount<=8 )
				if ( m_nClrUsed==0 )	// 0 means full color table
					m_nClrUsed = 1 << m_nBitCount;

			if ( m_nClrUsed )	// has a color table
			{
				if ( m_nClrImpt==0 )	// 0 means all important
					m_nClrImpt = m_nClrUsed;
			
				if ( compression==BI_BITFIELDS )
				{
					m_pBitFields = (DWORD *) ((BYTE *)pDIB+size);
					m_pRGBQUAD = (RGBQUAD *) ((BYTE *)pDIB+size + 3*sizeof(DWORD));
				}
				else
					m_pRGBQUAD = (RGBQUAD *) ((BYTE *)pDIB+size);

				m_pBits = (BYTE *) & m_pRGBQUAD[m_nClrUsed];
			}
			else
			{
				if ( compression==BI_BITFIELDS )
				{
					m_pBitFields = (DWORD *) ((BYTE *)pDIB+size);
					m_pBits      = (BYTE *) m_pBMI + size + 3 * sizeof(DWORD);
				}
				else
					m_pBits      = (BYTE *) m_pBMI + size;
			}
			break;
		}

		default:
			return false;
	}

	if ( pBits )
		m_pBits = pBits;

	// precalculate information DIB parameters
	m_nColorDepth = m_nPlanes * m_nBitCount;
	m_nBPS		  = (m_nWidth * m_nBitCount + 31) / 32 * 4;
	
	if (m_nHeight < 0 ) // top-down bitmap
	{
		m_nHeight = - m_nHeight;	// change to positive
		m_nDelta  = m_nBPS;			// forward
		m_pOrigin = m_pBits;		// scan0 .. scanN-1
	}
	else
	{
		m_nDelta  = - m_nBPS;		// backward
		m_pOrigin = m_pBits + (m_nHeight-1) * m_nBPS * m_nPlanes;	// scanN-1..scan0
	}

	if ( m_nImageSize==0 )
		m_nImageSize = m_nBPS * m_nPlanes * m_nHeight;

	// convert compression mode to image format
	switch ( m_nBitCount )
	{
		case 0:
			if ( compression==BI_JPEG )
				m_nImageFormat = DIB_JPEG;
			else if ( compression==BI_PNG )
				m_nImageFormat = DIB_PNG;
			else
				return false;

		case 1:
			m_nImageFormat = DIB_1BPP;
			break;

		case 2:
			m_nImageFormat = DIB_2BPP;
			break;

		case 4:
			if ( compression==BI_RLE4 )
				m_nImageFormat = DIB_4BPPRLE;
			else
				m_nImageFormat = DIB_4BPP;
			break;

		case 8:
			if ( compression==BI_RLE8 )
				m_nImageFormat = DIB_8BPPRLE;
			else
				m_nImageFormat = DIB_8BPP;
			break;
		
		case 16:
			if ( compression==BI_BITFIELDS )
				m_nImageFormat = DIB_16RGBbitfields;
			else
				m_nImageFormat = DIB_16RGB555; // see below
			break;
		
		case 24:
			m_nImageFormat = DIB_24RGB888;
			break;

		case 32:
			if ( compression == BI_BITFIELDS )
				m_nImageFormat = DIB_32RGBbitfields;
			else
				m_nImageFormat = DIB_32RGB888; // see below
			break;

		default:
			return false;
	}

	// try to understand bit fields
	if ( compression==BI_BITFIELDS )
	{
		DWORD red   = m_pBitFields[0];
		DWORD green = m_pBitFields[1];
		DWORD blue  = m_pBitFields[2];

		if (      (blue==0x001F) && (green==0x03E0) && (red==0x7C00) )
			m_nImageFormat = DIB_16RGB555;
		else if ( (blue==0x001F) && (green==0x07E0) && (red==0xF800) )
			m_nImageFormat = DIB_16RGB565;
		else if ( (blue==0x00FF) && (green==0xFF00) && (red==0xFF0000) )
			m_nImageFormat = DIB_32RGB888;
	}

	return true;
}


bool KDIB::LoadBitmap(HMODULE hModule, LPCTSTR pBitmapName)
{
	HRSRC   hRes = FindResource(hModule, pBitmapName, RT_BITMAP);

	if ( hRes==NULL )
		return false;

	HGLOBAL hGlb = LoadResource(hModule, hRes);

	if ( hGlb==NULL )
		return false;

	BITMAPINFO * pDIB = (BITMAPINFO *) LockResource(hGlb);

	if ( pDIB==NULL )
		return false;

	return AttachDIB(pDIB, NULL, DIB_BMI_READONLY | DIB_BITS_READONLY);
}


bool KDIB::LoadFile(const TCHAR * pFileName)
{
	if ( pFileName==NULL )
		return false;

	HANDLE handle = CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if ( handle == INVALID_HANDLE_VALUE )
		return false;

    BITMAPFILEHEADER bmFH;

	DWORD dwRead = 0;
	ReadFile(handle, & bmFH, sizeof(bmFH), & dwRead, NULL);

	if ( (bmFH.bfType == 0x4D42) && (bmFH.bfSize<=GetFileSize(handle, NULL)) )
	{
		BITMAPINFO * pDIB = (BITMAPINFO *) new BYTE[bmFH.bfSize];
		
		if ( pDIB )
		{
			ReadFile(handle, pDIB, bmFH.bfSize, & dwRead, NULL);
			CloseHandle(handle);

			return AttachDIB(pDIB, NULL, DIB_BMI_NEEDFREE);
		}
	}
	CloseHandle(handle);

	return false;
}


BOOL KDIB::SaveFile(const TCHAR * pFileName)
{
	if ( m_pBMI )
		return SaveDIBToFile(pFileName, m_pBMI, m_pBits);
	else
		return FALSE;
}

void KDIB::DecodeDIBFormat(TCHAR mess[])
{
	wsprintf(mess, _T("DIB %dx%dx%dx%d "), m_nWidth, m_nHeight, 
		m_nPlanes, m_nBitCount);

	if ( m_nImageSize < 1024 )
		wsprintf(mess+_tcslen(mess), _T(", %d b"), m_nImageSize);
	else if ( m_nImageSize < 1024 * 1024 )
		wsprintf(mess+_tcslen(mess), _T(", %d,%03d b"), m_nImageSize/1024, m_nImageSize%1024);
	else
		wsprintf(mess+_tcslen(mess), _T(", %d,%03d,%03d b"), m_nImageSize/1024/1024, m_nImageSize/1024%1024, 
			m_nImageSize%1024);

	_tcscat(mess, PixelFormatName(m_nImageFormat));
	_tcscat(mess, _T(" "));
}


int GetDIBPixelSize(const BITMAPINFOHEADER & bmih)
{
	if ( bmih.biSizeImage )
		return bmih.biSizeImage;
	else
		return ( bmih.biWidth * bmih.biBitCount + 31 ) / 32 * 4 * bmih.biPlanes * abs(bmih.biHeight);
}

int GetDIBColorCount(const BITMAPINFOHEADER & bmih)
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


BITMAPINFO * BitmapToDIB(HPALETTE hPal,	// palette for color conversion
				   HBITMAP  hBmp,							// DDB for convert
				   int		nBitCount, int nCompression)	// format wanted
{
	typedef struct
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD	 	     bmiColors[256+3];
	}	DIBINFO;

	BITMAP  ddbinfo;
	DIBINFO dibinfo;

	// retrieve DDB information
	if ( GetObject(hBmp, sizeof(BITMAP), & ddbinfo)==0 )
		return NULL;

	// fill out BITMAPINFOHEADER based on size and required format
	memset(&dibinfo, 0, sizeof(dibinfo));

	dibinfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	dibinfo.bmiHeader.biWidth       = ddbinfo.bmWidth;
	dibinfo.bmiHeader.biHeight      = ddbinfo.bmHeight;
	dibinfo.bmiHeader.biPlanes      = 1;
	dibinfo.bmiHeader.biBitCount    = nBitCount;
	dibinfo.bmiHeader.biCompression = nCompression;

	HDC     hDC = GetDC(NULL); // screen DC
	HGDIOBJ hpalOld;
	
	if ( hPal )
		hpalOld = SelectPalette(hDC, hPal, FALSE);
	else
		hpalOld = NULL;

	// query GDI for image size
	GetDIBits(hDC, hBmp, 0, ddbinfo.bmHeight, NULL, (BITMAPINFO *) & dibinfo, DIB_RGB_COLORS);

	int nInfoSize  = sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * GetDIBColorCount(dibinfo.bmiHeader);
	int nTotalSize = nInfoSize + GetDIBPixelSize(dibinfo.bmiHeader);

	BYTE * pDIB = new BYTE[nTotalSize];

	if ( pDIB )
	{
		memcpy(pDIB, & dibinfo, nInfoSize);
		
		if ( ddbinfo.bmHeight != GetDIBits(hDC, hBmp, 0, ddbinfo.bmHeight, pDIB + nInfoSize, (BITMAPINFO *) pDIB, DIB_RGB_COLORS) )
		{
			delete [] pDIB;
			pDIB = NULL;
		}
	}

	if ( hpalOld )
		SelectObject(hDC, hpalOld);

	ReleaseDC(NULL, hDC);

	return (BITMAPINFO *) pDIB;
}


int PixelFormat(HDC hdc)
{
	typedef struct
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD	 	     bmiColors[256+3];
	}	DIBINFO;

	DIBINFO dibinfo;

	HBITMAP hBmp = CreateCompatibleBitmap(hdc, 1, 1);
    
	if ( hBmp==NULL )
        return -1;

	memset(&dibinfo, 0, sizeof(dibinfo));

    dibinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

    // 1st call to get hdc biBitCount.
    GetDIBits(hdc, hBmp, 0, 1, NULL, (BITMAPINFO*) & dibinfo, DIB_RGB_COLORS);
       
    // 2nd calls to get color table or bitfields
    GetDIBits(hdc, hBmp, 0, 1, NULL, (BITMAPINFO*) & dibinfo, DIB_RGB_COLORS);
       
    DeleteObject(hBmp);

	// try to understand bit fields
	if ( dibinfo.bmiHeader.biBitCount==BI_BITFIELDS )
	{
		DWORD * pBitFields = (DWORD *) dibinfo.bmiColors;
		DWORD red   = pBitFields[0];
		DWORD green = pBitFields[1];
		DWORD blue  = pBitFields[2];

		if (      (blue==0x001F) && (green==0x03E0) && (red==0x7C00) )
			return DIB_16RGB555;
		else if ( (blue==0x001F) && (green==0x007E) && (red==0xF800) )
			return DIB_16RGB565;
		else if ( (blue==0x00FF) && (green==0xFF00) && (red==0xFF0000) )
			return DIB_32RGB888;
		else
			return -1;
	}

	switch ( dibinfo.bmiHeader.biBitCount )
	{
		case 1:	return DIB_1BPP;
		case 2: return DIB_2BPP;
		case 4:	return DIB_4BPP;
		case 8: return DIB_8BPP;
	   case 24: return DIB_24RGB888;
	   case 16: return DIB_16RGB555;
	   case 32: return DIB_32RGB888;

	   default: return -1;
	}
}


HBITMAP CaptureWindow(HWND hWnd)
{
	RECT wnd;

	if ( ! GetWindowRect(hWnd, & wnd) )
		return NULL;

	HDC hDC = GetWindowDC(hWnd);

	HBITMAP hBmp = CreateCompatibleBitmap(hDC, wnd.right - wnd.left, wnd.bottom - wnd.top);

	if ( hBmp )
	{
		HDC hMemDC   = CreateCompatibleDC(hDC);
		HGDIOBJ hOld = SelectObject(hMemDC, hBmp);

		BitBlt(hMemDC, 0, 0, wnd.right - wnd.left, wnd.bottom - wnd.top, 
			hDC, 0, 0, SRCCOPY);

		SelectObject(hMemDC, hOld);
		DeleteObject(hMemDC);
	}
	
	ReleaseDC(hWnd, hDC);

	return hBmp;
}


BOOL SaveDIBToFile(const TCHAR * pFileName, const BITMAPINFO * pBMI, const void * pBits)
{
	KFileDialog fi;

	if ( pFileName==NULL )
	{
		if ( ! fi.GetSaveFileName(NULL, _T("bmp"), _T("Bitmap Files")) )
			return FALSE;

		pFileName = fi.m_TitleName;
	}

	HANDLE handle = CreateFile(pFileName, GENERIC_WRITE, FILE_SHARE_READ, 
		NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if ( handle == INVALID_HANDLE_VALUE )
		return FALSE;

	BITMAPFILEHEADER bmFH;

	int nHeadSize = sizeof(BITMAPINFOHEADER) + 
						sizeof(RGBQUAD) * GetDIBColorCount(pBMI->bmiHeader);

	bmFH.bfType      = 0x4D42;
	bmFH.bfSize      = nHeadSize + GetDIBPixelSize(pBMI->bmiHeader);
	bmFH.bfReserved1 = 0;
	bmFH.bfReserved2 = 0;
	bmFH.bfOffBits   = nHeadSize + sizeof(BITMAPFILEHEADER);

	DWORD dwRead = 0;
	WriteFile(handle, & bmFH, sizeof(bmFH), & dwRead, NULL);

	if ( pBits==NULL ) // packed DIB
		pBits = (BYTE *) pBMI + nHeadSize;
	
	WriteFile(handle, pBMI,  nHeadSize,						   & dwRead, NULL);
	WriteFile(handle, pBits, GetDIBPixelSize(pBMI->bmiHeader), & dwRead, NULL);

	CloseHandle(handle);

	return TRUE;
}

void SaveWindowToDIB(HWND hWnd, int nBitCount, int nCompression)
{
	HBITMAP hBmp = CaptureWindow(hWnd);

	if ( hBmp )
	{
		BITMAPINFO * pDIB = BitmapToDIB(NULL, hBmp, nBitCount, nCompression);

		if ( pDIB )
		{
			SaveDIBToFile(NULL, pDIB, NULL);
			delete [] (BYTE *) pDIB;
		}

		DeleteObject(hBmp);
	}
}

const BYTE Shift1bpp[] = { 7,     6,     5,     4,     3,     2,     1,     0     };
const BYTE Mask1bpp [] = { ~0x80, ~0x40, ~0x20, ~0x10, ~0x08, ~0x04, ~0x02, ~0x01 };

const BYTE Shift2bpp[] = { 6,     4,     2,     0     };
const BYTE Mask2bpp [] = { ~0xC0, ~0x30, ~0x0C, ~0x03 };

const BYTE Shift4bpp[] = { 4, 0 };
const BYTE Mask4bpp [] = { ~0xF0, ~0x0F };


DWORD KDIB::GetPixelIndex(int x, int y) const
{
	if ( (x<0) || (x>=m_nWidth) )
		return -1;

	if ( (y<0) || (y>=m_nHeight) )
		return -1;

	BYTE * pPixel = m_pOrigin + y * m_nDelta;

	switch ( m_nImageFormat )
	{
		case DIB_1BPP:
			return ( pPixel[x/8] >> Shift1bpp[x%8] ) & 0x01;

		case DIB_2BPP:
			return ( pPixel[x/4] >> Shift2bpp[x%4] ) & 0x03;

		case DIB_4BPP:
			return ( pPixel[x/2] >> Shift4bpp[x%4] ) & 0x0F;

		case DIB_8BPP:
			return pPixel[x];

		case DIB_16RGB555:
		case DIB_16RGB565:
			return ((WORD *)pPixel)[x];

		case DIB_24RGB888:
			pPixel += x * 3;
			return (pPixel[0]) | (pPixel[1] << 8) | (pPixel[2] << 16);
		
		case DIB_32RGB888:
		case DIB_32RGBA8888:
			return ((DWORD *)pPixel)[x];
	}

	return -1;
}

BOOL  KDIB::SetPixelIndex(int x, int y, DWORD index)
{
	if ( (x<0) || (x>=m_nWidth) )
		return FALSE;

	if ( (y<0) || (y>=m_nHeight) )
		return FALSE;

	BYTE * pPixel = m_pOrigin + y * m_nDelta;
	
	switch ( m_nImageFormat )
	{
		case DIB_1BPP:
			pPixel[x/8] = (BYTE) ( ( pPixel[x/8] & Mask1bpp[x%8] ) | ( (index & 1) << Shift1bpp[x%8] ) );
			break;

		case DIB_2BPP:
			pPixel[x/4] = (BYTE) ( ( pPixel[x/4] & Mask2bpp[x%4] ) | ( (index & 3) << Shift2bpp[x%4] ) );
			break;

		case DIB_4BPP:
			pPixel[x/2] = (BYTE) ( ( pPixel[x/2] & Mask4bpp[x%2] ) | ( (index & 15) << Shift4bpp[x%2] ) );
			break;

		case DIB_8BPP:
			pPixel[x] = (BYTE) index;
			break;

		case DIB_16RGB555:
		case DIB_16RGB565:
			((WORD *)pPixel)[x] = (WORD) index;
			break;

		case DIB_24RGB888:
			((RGBTRIPLE *)pPixel)[x] = * ((RGBTRIPLE *) & index);
			break;

		case DIB_32RGB888:
		case DIB_32RGBA8888:
			((DWORD *)pPixel)[x] = index;
			break;

		default:
			return FALSE;
	}

	return TRUE;
}


BOOL KDIB::PlgBlt(const POINT * pPoint, 
				   KDIB * pSrc, int nXSrc, int nYSrc, int nWidth, int nHeight)
{
	KReverseAffine map(pPoint);

	map.Setup(nXSrc, nYSrc, nWidth, nHeight);

	for (int dy=map.miny; dy<=map.maxy; dy++)
	for (int dx=map.minx; dx<=map.maxx; dx++)
	{
		float sx, sy;
		map.Map(dx, dy, sx, sy);

		if ( (sx>=nXSrc) && (sx<(nXSrc+nWidth))  )
		if ( (sy>=nYSrc) && (sy<(nYSrc+nHeight)) )
			SetPixelIndex(dx, dy, pSrc->GetPixelIndex( (int)sx, (int)sy));
	}

	return TRUE;
}


BOOL KDIB::PlgBltGetPixel(const POINT * pPoint, 
				   HDC hSrc, int nXSrc, int nYSrc, int nWidth, int nHeight, HDC hDst)
{
	KReverseAffine map(pPoint);

	map.Setup(nXSrc, nYSrc, nWidth, nHeight);

	for (int dy=map.miny; dy<=map.maxy; dy++)
	for (int dx=map.minx; dx<=map.maxx; dx++)
	{
		float sx, sy;
		map.Map(dx, dy, sx, sy);

		if ( (sx>=nXSrc) && (sx<(nXSrc+nWidth))  )
		if ( (sy>=nYSrc) && (sy<(nYSrc+nHeight)) )
			SetPixel(hDst, dx, dy, GetPixel(hSrc, (int)sx, (int)sy));
	}

	return TRUE;
}


BOOL KDIB::PlgBlt24(const POINT * pPoint, 
				   KDIB * pSrc, int nXSrc, int nYSrc, int nWidth, int nHeight)
{
	// factor to change FLOAT to fixed point
	const int FACTOR = 65536;

	// generate reverse transformation from destination to source
	KReverseAffine map(pPoint);

	map.Setup(nXSrc, nYSrc, nWidth, nHeight);

	// make sure within destination bitmap dimension
	if ( map.minx < 0 )         map.minx = 0;
	if ( map.maxx > m_nWidth )  map.maxx = m_nWidth;

	if ( map.miny < 0 )         map.miny = 0;
	if ( map.maxy > m_nHeight ) map.maxy = m_nHeight;

	// source rectangle in fixed point
	int sminx = nXSrc * FACTOR;
	int sminy = nYSrc * FACTOR;
	int smaxx = ( nXSrc + nWidth  ) * FACTOR;
	int smaxy = ( nYSrc + nHeight ) * FACTOR;

	// transformation matrix in fixed point
	int m11 = (int) (map.m_xm.eM11 * FACTOR);
	int m12 = (int) (map.m_xm.eM12 * FACTOR);
	int m21 = (int) (map.m_xm.eM21 * FACTOR);
	int m22 = (int) (map.m_xm.eM22 * FACTOR);
	int mdx = (int) (map.m_xm.eDx  * FACTOR);
	int mdy = (int) (map.m_xm.eDy  * FACTOR);

	BYTE * SOrigin = pSrc->m_pOrigin;
	int    SDelta  = pSrc->m_nDelta;

	// in destination bitmap, scan from first to last scanline
	for (int dy=map.miny; dy<map.maxy; dy++)
	{
/*
		// search for the first pixel on the scanline within source rectangle
		int sx = m11 * map.minx + m21 * dy + mdx;
		int sy = m12 * map.minx + m22 * dy + mdy;

		for (int dx=map.minx; dx<map.maxx; dx++, sx+=m11, sy+=m12)
			// if within source rectangle
			if ( (sx>=sminx) && (sx<smaxx) )
			if ( (sy>=sminy) && (sy<smaxy) )
				break;

		int minx = dx;

		// search for the last pixel on the scaneline within source rectangle
		sx = m11 * map.maxx + m21 * dy + mdx;
		sy = m12 * map.maxx + m22 * dy + mdy;

		for (dx=map.maxx; dx>=map.minx; dx--, sx-=m11, sy-=m12)
			// if within source rectangle
			if ( (sx>=sminx) && (sx<smaxx) )
			if ( (sy>=sminy) && (sy<smaxy) )
				break;

		int maxx = dx;
*/
		// precalculate destination pixel address for first pixel
		BYTE * pDPixel = m_pOrigin + dy * m_nDelta + map.minx * 3;

		// source pixel address for the first pixel
		int sx = m11 * map.minx + m21 * dy + mdx;
		int sy = m12 * map.minx + m22 * dy + mdy;

		// go through each pixel on the scan line
		for (int dx=map.minx; dx<map.maxx; dx++, pDPixel+=3, sx+=m11, sy+=m12)
			if ( (sx>=sminx) && (sx<smaxx) )
			if ( (sy>=sminy) && (sy<smaxy) )
			{
				// source pixel address
				BYTE * pSPixel = SOrigin + (sy/FACTOR) * SDelta;

				// copy three bytes
				* ((RGBTRIPLE *)pDPixel) = ((RGBTRIPLE *)pSPixel)[sx/FACTOR];
			}
	}

	return TRUE;
}

BOOL KDIB::Create(int width, int height, int bitcount)
{
	KBitmapInfo dibinfo;

	dibinfo.SetFormat(width, height, bitcount, BI_RGB);

	int nBitsSize = ( width * bitcount + 31 ) /32 * 4 * height;
	BYTE * pBits = new BYTE[nBitsSize];
	
	if ( pBits )
		return AttachDIB(dibinfo.CopyBMI(), pBits, 0);
	else
		return FALSE;
}

void Map(XFORM * xm, int x, int y, int & rx, int & ry)
{
	rx = (int) ( xm->eM11 * x + xm->eM21 * y + xm->eDx );
	ry = (int) ( xm->eM12 * x + xm->eM22 * y + xm->eDy );
}

BOOL Invert(XFORM & xm)
{
	FLOAT det = xm.eM11 * xm.eM22 - xm.eM21 * xm.eM12;

	if ( det==0 )
		return FALSE;

	XFORM old = xm;

	xm.eM11 =   old.eM22 / det;
	xm.eM12 = - old.eM12 / det;
	xm.eM21 = - old.eM21 / det;
	xm.eM22 =   old.eM11 / det;
	
	xm.eDx  = - ( xm.eM11 * old.eDx + xm.eM21 * old.eDy );
	xm.eDy  = - ( xm.eM12 * old.eDx + xm.eM22 * old.eDy );

	return TRUE;
}

DWORD dibtick;

HBITMAP KDIB::TransformBitmap(XFORM * xm, COLORREF crBack, int method)
{
	int x0, y0, x1, y1, x2, y2, x3, y3;

	Map(xm, 0,        0,         x0, y0);  // 0    1
	Map(xm, m_nWidth, 0,         x1, y1);  //
	Map(xm, 0,        m_nHeight, x2, y2);  // 2    3
	Map(xm, m_nWidth, m_nHeight, x3, y3);

	int xmin, xmax;
	int ymin, ymax;

	minmax(x0, x1, x2, x3, xmin, xmax);
	minmax(y0, y1, y2, y3, ymin, ymax);

	int destwidth  = xmax - xmin;
	int destheight = ymax - ymin;

	KBitmapInfo dest;
	dest.SetFormat(destwidth, destheight, m_pBMI->bmiHeader.biBitCount, m_pBMI->bmiHeader.biCompression);

	if ( m_nClrUsed )
		memcpy(dest.m_dibinfo.bmiColors, m_pRGBQUAD, sizeof(RGBQUAD) * m_nClrUsed);

	// create destination DIB section
	BYTE * pBits;
	HBITMAP hBitmap = CreateDIBSection(NULL, dest.GetBMI(), DIB_RGB_COLORS, (void **) & pBits, NULL, NULL);
	if ( hBitmap==NULL )
	{
		assert(false);
		return NULL;
	}

	// For testing GDI GetPixel speed, we need a HDC for the source bitmap
	HBITMAP hSrcBmp = NULL;
	HDC     hSrcDC  = NULL;
	HGDIOBJ hSrcOld = NULL;

	if ( method==method_gdi )
	{
		hSrcBmp = CreateDIBSection(NULL, m_pBMI, DIB_RGB_COLORS, (void **) & pBits, NULL, NULL);
		assert(hSrcBmp);

		hSrcDC  = CreateCompatibleDC(NULL);
		hSrcOld = SelectObject(hSrcDC, hSrcBmp);

		// copy pixels from DIB to source DIB section
		DrawDIB(hSrcDC, 0, 0, m_nWidth, m_nHeight, 0, 0, m_nWidth, m_nHeight, SRCCOPY);
	}

	// clear DIB section to background color
	HDC     hDstDC  = CreateCompatibleDC(NULL);
	HGDIOBJ hDstOld = SelectObject(hDstDC, hBitmap);

	HBRUSH hBrush = CreateSolidBrush(crBack);
	RECT rect = { 0, 0,  destwidth, destheight };
	FillRect(hDstDC, & rect, hBrush);
	DeleteObject(hBrush);

	KDIB destDIB;
	destDIB.AttachDIB(dest.GetBMI(), pBits, 0);

	POINT P[3] = { { x0-xmin, y0-ymin }, { x1-xmin, y1-ymin }, { x2-xmin, y2-ymin } };

	dibtick = GetTickCount();
	
	if ( (m_nBitCount<=8) && (method==method_24bpp) )
		method = method_direct;

	switch ( method )
	{
		case method_gdi:
			destDIB.PlgBltGetPixel(P, hSrcDC, 0, 0, m_nWidth, m_nHeight, hDstDC);
			break;   

		case method_direct:	
			destDIB.PlgBlt(P, this, 0, 0, m_nWidth, m_nHeight);
			break;   

		case method_24bpp:
			destDIB.PlgBlt24(P, this, 0, 0, m_nWidth, m_nHeight);
			break;
	}

	dibtick = GetTickCount() - dibtick;

	SelectObject(hDstDC, hDstOld);
	DeleteObject(hDstDC);

	if ( method==method_gdi )
	{
		SelectObject(hSrcDC, hSrcOld);

		DeleteObject(hSrcDC);
		DeleteObject(hSrcBmp);
	}

	return hBitmap;
}


// Calculate DIB pixel offset
inline int GetOffset(BITMAPINFO * pBMI, int x, int y)
{
	if ( pBMI->bmiHeader.biHeight > 0 )	// for bottom up, reflect y
		y = pBMI->bmiHeader.biHeight - 1 - y;
	
	return ( pBMI->bmiHeader.biWidth * pBMI->bmiHeader.biBitCount + 31 ) / 32 * 4 * y + 
		   ( pBMI->bmiHeader.biBitCount / 8 ) * x;
}


// Alpha Blending between two 32-bpp DIBs
BOOL AlphaBlend3232(BITMAPINFO * pBMIDst, BYTE * pBitsDst, int dx, int dy, int w, int h,
				    BITMAPINFO * pBMISrc, BYTE * pBitsSrc, int sx, int sy,
				    BLENDFUNCTION blend)
{
	int alpha = blend.SourceConstantAlpha;	// constant alpha
	int beta  = 255 - alpha;				// constant beta
	int format;

	if ( blend.AlphaFormat==0 )
		format = 0;
	else if ( alpha==255 )
		format = 1;
	else 
		format = 2;

	for (int j=0; j<h; j++)
	{
		BYTE * D = pBitsDst + GetOffset(pBMIDst, dx, j + dy);
		BYTE * S = pBitsSrc + GetOffset(pBMISrc, sx, j + sy);
		
		int i;

		switch ( format )
		{
			case 0: // constant alpha only
				for (i=0; i<w; i++)
				{
					D[0] = ( S[0] * alpha + beta * D[0] + 127 ) / 255;
					D[1] = ( S[1] * alpha + beta * D[1] + 127 ) / 255;
					D[2] = ( S[2] * alpha + beta * D[2] + 127 ) / 255;
					D[3] = ( S[3] * alpha + beta * D[3] + 127 ) / 255;
					D += 4; S += 4;
				}
				break;

			case 1: // alpha channel only
				for (i=0; i<w; i++)
				{
					beta  = 255 - S[3];
					D[0] = S[0] + ( beta * D[0] + 127 ) / 255;
					D[1] = S[1] + ( beta * D[1] + 127 ) / 255;
					D[2] = S[2] + ( beta * D[2] + 127 ) / 255;
					D[3] = S[3] + ( beta * D[3] + 127 ) / 255;
					D += 4; S += 4;
				}
				break;
			
			case 2: // both constant alpha and alpha channel
				for (i=0; i<w; i++)
				{
					beta  = 255 - ( S[3] * alpha + 127 ) / 255;
					
					D[0] = ( S[0] * alpha + beta * D[0] + 127 ) / 255;
					D[1] = ( S[1] * alpha + beta * D[1] + 127 ) / 255;
					D[2] = ( S[2] * alpha + beta * D[2] + 127 ) / 255;
					D[3] = ( S[3] * alpha + beta * D[3] + 127 ) / 255;
					D += 4; S += 4;
				}
		}
	}

	return FALSE;
}


// AlphaBlending a 32-bpp DIB to hdc
BOOL AlphaBlend(HDC hdc, int x0, int y0, const BITMAPINFO *pbmpInfo, const void * pAlphaImageData)
{
	if ( pbmpInfo->bmiHeader.biBitCount!=32 )
		return FALSE;

	if ( pbmpInfo->bmiHeader.biCompression!=BI_RGB )
		return FALSE;

	int width  = pbmpInfo->bmiHeader.biWidth;
	int height = pbmpInfo->bmiHeader.biHeight;

	BITMAPINFOHEADER bmih = { sizeof(BITMAPINFOHEADER), width, height, 1, 24 };

	VOID * pBits;

	HBITMAP hBmp = CreateDIBSection(NULL, (BITMAPINFO *) & bmih, DIB_RGB_COLORS, & pBits, NULL, NULL);

	if ( hBmp==NULL )
		return FALSE;

	HDC hMemDC = CreateCompatibleDC(hdc);
	HGDIOBJ hOld = SelectObject(hMemDC, hBmp);

	BitBlt(hMemDC, 0, 0, width, height, hdc, x0, y0, SRCCOPY); // copy destination, not working on printer

	const BYTE * psrc = (const BYTE *) pAlphaImageData;
		  BYTE * pdst = (BYTE *) pBits;

	for (int y = 0; y < abs(height); y++)
	{
		for (int x = width; x>0; x--)
		{
			BYTE alpha  = psrc[3]; //Get alpha channel byte.
			BYTE salpha = 255 - alpha;

			pdst[0] = (BYTE)(( psrc[0] * alpha + pdst[0] * salpha) / 255);
			pdst[1] = (BYTE)(( psrc[1] * alpha + pdst[1] * salpha) / 255);
			pdst[2] = (BYTE)(( psrc[2] * alpha + pdst[2] * salpha) / 255);

			pdst += 3;
			psrc += 4;
		}

		pdst += width % 3;
	}

	BOOL rslt = BitBlt(hdc, x0, y0, width, height, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOld);
	DeleteObject(hBmp);
	DeleteObject(hMemDC);

	return rslt;
}
