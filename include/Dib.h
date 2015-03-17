#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dib.h						                                         //
//  Description: Device independent bitmap                                           //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

typedef enum 
{
	DIB_1BPP,			//   2 color image, palette-based
	DIB_2BPP,			//   4 color image, palttte-based
	DIB_4BPP,			//  16 color image, palette-based
	DIB_4BPPRLE,		//  16 color image, palette-based, RLE compressed
	DIB_8BPP,			// 256 color image, palette-based 
	DIB_8BPPRLE,		// 256 color image, palette-based, RLE compressed

	DIB_16RGB555,		// 15 bit RGB color image, 5-5-5
	DIB_16RGB565,		// 16 bit RGB color image, 5-6-5, 1 bit unused
	DIB_24RGB888,		// 24 bit RGB color image, 8-8-8
	DIB_32RGB888,		// 32 bit RGB color image, 8-8-8, 8 bit unused

	DIB_32RGBA8888,		// 32 bit RGBA color image, 8-8-8-8

	DIB_16RGBbitfields,	// 16 bit RGB color image, non-standard bit masks, NT-only
	DIB_32RGBbitfields,	// 32 bit RGB color image, non-standard bit masks, NT-only

	DIB_JPEG,			// embedded JPEG image
	DIB_PNG				// embedded PNG image
}  DIBFormat;


const TCHAR * PixelFormatName(int id);

typedef enum 
{
	DIB_BMI_NEEDFREE   = 1,
	DIB_BMI_READONLY   = 2,
	DIB_BITS_NEEDFREE  = 4,
	DIB_BITS_READONLY  = 8	
};

class KDIB
{
//	template <class Transform>
//		friend 	bool TransformDIB(CDIB * dib, Transform map);

public:
	DIBFormat	 m_nImageFormat;    // pixel array format
	int			 m_Flags;		 	// DIB_BMI_NEEDFREE, ....
	BITMAPINFO * m_pBMI;			// Bitmap Information Header + mask + color table
	BYTE       * m_pBits;			// pixel array

	RGBTRIPLE  * m_pRGBTRIPLE;		// OS/2 DIB color table within m_pBMI
	RGBQUAD    * m_pRGBQUAD;		// V3,4,5 DIB color table within m_pBMI
	int			 m_nClrUsed;		// No of color in color table
	int			 m_nClrImpt;		// Real color used
	DWORD	   * m_pBitFields;		// 16, 32-bpp masks within m_pBMI
	
	int			 m_nWidth;			// image pixel width
	int			 m_nHeight;			// image pixel height, positive
	int			 m_nPlanes;			// plane count
	int			 m_nBitCount;		// bit per plane
	int			 m_nColorDepth;		// bit count * plane count
	int			 m_nImageSize;		// pixel array size

									// Precalculated values
	int			 m_nBPS;			// byte per scan line, per plane
	BYTE *       m_pOrigin;			// point to logical origin
	int			 m_nDelta;			// delta to next scan line

	KDIB();						// default constructor, empty image
	virtual ~KDIB();			// virtual destructor

	BOOL Create(int width, int height, int bitcount);

	bool AttachDIB(BITMAPINFO * pDIB, BYTE * pBits, int flags);
	bool LoadFile(const TCHAR * pFileName);
	bool LoadBitmap(HMODULE hModlue, LPCTSTR pBitmapName);
	void ReleaseDIB(void);			// release memory

	int  GetWidth(void)		  const	{ return m_nWidth;  }
	int  GetHeight(void)	  const	{ return m_nHeight; }
	int  GetDepth(void)		  const { return m_nColorDepth; }
	BITMAPINFO * GetBMI(void) const { return m_pBMI; }
	BYTE * GetBits(void)	  const { return m_pBits; }
	int  GetBPS(void)		  const { return m_nBPS; }

	bool IsCompressed(void) const 
	{
		return (m_nImageFormat == DIB_4BPPRLE) ||
			   (m_nImageFormat == DIB_8BPPRLE) ||
			   (m_nImageFormat == DIB_JPEG) ||
			   (m_nImageFormat == DIB_PNG);
	}

	int  GetColorCount(void) const;

	int DrawDIB(HDC hDC, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh, DWORD rop)
	{
		if ( m_pBMI )
			return ::StretchDIBits(hDC, dx, dy, dw, dh, sx, sy, sw, sh, 
				m_pBits, m_pBMI, DIB_RGB_COLORS, rop);
		else
			return GDI_ERROR;
	}

	int SetDIB(HDC hDC, int dx, int dy, int firstscan, int scanno, const BYTE * pBits)
	{
		if ( m_pBMI )
			return ::SetDIBitsToDevice(hDC, 
					dx, dy, m_nWidth, m_nHeight, 0, 0, firstscan, scanno, 
					pBits, m_pBMI, DIB_RGB_COLORS);
		else
			return GDI_ERROR;
	}

	HBITMAP ConvertToDDB(HDC hDC)
	{
		return CreateDIBitmap(hDC, & m_pBMI->bmiHeader, CBM_INIT, m_pBits,
					m_pBMI, DIB_RGB_COLORS);
	}

	void DecodeDIBFormat(TCHAR mess[]);
	BOOL SaveFile(const TCHAR * pFileName);

	// Pixel accessing
	DWORD GetPixelIndex(int x, int y) const;
	BOOL  SetPixelIndex(int x, int y, DWORD index);

	// transformation using GDI GetPixel/SetPixel
	BOOL PlgBltGetPixel(const POINT * pPoint, 
			HDC hSrc, int nXSrc, int nYSrc, int nWidth, int nHeight, HDC hDst);

	// transformation using GetPixelIndex/SetPixelIndex
	BOOL  PlgBlt(const POINT * pPoint, KDIB * pSrc, int nXSrc, int nYSrc, int nWidth, int nHeight);

	// transformation using inline function + integer only operations
	BOOL  PlgBlt24(const POINT * pPoint, KDIB * pSrc, int nXSrc, int nYSrc, int nWidth, int nHeight);

	typedef enum { method_gdi, method_direct, method_24bpp };
	HBITMAP TransformBitmap(XFORM * xm, COLORREF crBack, int method);
};

int GetDIBPixelSize(const BITMAPINFOHEADER & bmih);
int GetDIBColorCount(const BITMAPINFOHEADER & bmih);

BITMAPINFO * BitmapToDIB(HPALETTE hPal,	// palette for color conversion
				   HBITMAP  hBmp,							// DDB for convert
				   int		nBitCount, int nCompression);	// format wanted

int PixelFormat(HDC hdc);

void SaveWindowToDIB(HWND hWnd, int nBitCount, int nCompression);
BOOL SaveDIBToFile(const TCHAR * pFileName, const BITMAPINFO * pBMI, const void * pBits);


BOOL AlphaBlend3232(BITMAPINFO * pBMIDst, BYTE * pBitsDst, int dx, int dy, int w, int h,
				    BITMAPINFO * pBMISrc, BYTE * pBitsSrc, int sx, int sy,
				    BLENDFUNCTION blend);

// AlphaBlending a 32-bpp DIB to hdc
BOOL AlphaBlend(HDC hdc, int x0, int y0, const BITMAPINFO *pbmpInfo, const void * pAlphaImageData);
