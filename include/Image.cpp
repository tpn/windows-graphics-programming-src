//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : image.cpp	   					                                     //
//  Description: Generic Image Processing Classes                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <math.h>
#include <tchar.h>

#include "DIB.h"
#include "Image.h"

//////////////////////////////////////////////////
//												//
//           C O N V O L U T I O N				//
//												//
//////////////////////////////////////////////////

bool KImage::Convolution(KFilter * pFilter)
{
	BYTE * pDestBits = new BYTE[m_nImageSize];

	if ( pDestBits==NULL )
		return false;

	for (int y=0; y<m_nHeight; y++)
	{
		unsigned char * pBuffer = (unsigned char *) m_pBits   + m_nBPS * y;
		unsigned char * pDest   = (unsigned char *) pDestBits + m_nBPS * y;

		if (  (y>=pFilter->GetHalf()) && (y<(m_nHeight- pFilter->GetHalf())) )
			switch ( m_nImageFormat )
			{
				case DIB_8BPP:
					pFilter->Filter8bpp(pDest, pBuffer, m_nWidth, m_nBPS);
					break;

				case DIB_24RGB888:	// 24-bpp RGB
					pFilter->Filter24bpp(pDest, pBuffer, m_nWidth, m_nBPS);
					break;

				case DIB_32RGBA8888: // 32-bpp RGBA
				case DIB_32RGB888:   // 32-bpp RGB
					pFilter->Filter32bpp(pDest, pBuffer, m_nWidth, m_nBPS);
					break;

				default:
					delete [] pDestBits;
					return false;
			}
		else
			memcpy(pDest, pBuffer, m_nBPS); // copy unprocessed scanlines
	}
	
	memcpy(m_pBits, pDestBits, m_nImageSize); // overwrite source pixel array
	delete [] pDestBits;

	return true;
}

//////////////////////////////////////////////////
//												//
//   P I X E L   T R A N S F O R M A T I O N    //
//												//
//////////////////////////////////////////////////

BITMAPINFO * KImage::SplitChannel(Operator oper)
{
	KChannel channel;

	return channel.Split(* this, oper);
}


bool KImage::PixelTransform(KPixelMapper & map)
{
	if ( m_pRGBTRIPLE )
		map.SetColorTable((BYTE *) m_pRGBTRIPLE, sizeof(RGBTRIPLE), m_nClrUsed);
	else if ( m_pRGBQUAD )
		map.SetColorTable((BYTE *) m_pRGBQUAD, sizeof(RGBQUAD), m_nClrUsed);

	for (int y=0; y<m_nHeight; y++)
	{
		unsigned char * pBuffer = (unsigned char *) m_pBits + m_nBPS * y;
	
		if ( ! map.StartLine(y) )
			break;

		switch ( m_nImageFormat )
		{
			case DIB_1BPP:
				map.Map1bpp(pBuffer, m_nWidth);
				break;

			case DIB_2BPP:
				map.Map2bpp(pBuffer, m_nWidth);
				break;

			case DIB_4BPP:
				map.Map4bpp(pBuffer, m_nWidth);
				break;

			case DIB_8BPP:
				map.Map8bpp(pBuffer, m_nWidth);
				break;

			case DIB_16RGB555: 	// 15 bit RGB color image, 5-5-5, 1 bit unused
				map.Map555(pBuffer, m_nWidth);
				break;

			case DIB_16RGB565:	// 16 bit RGB color image, 5-6-5
				map.Map565(pBuffer, m_nWidth);
				break;

			case DIB_24RGB888:	// 24-bpp RGB
				map.Map24bpp(pBuffer, m_nWidth);
				break;

			case DIB_32RGBA8888: // 32-bpp RGBA
			case DIB_32RGB888:   // 32-bpp RGB
				map.Map32bpp(pBuffer, m_nWidth);
				break;

			default:
				return false;
		}
	}
	
	return true;
}

//////////////////////////////////////////////////
//												//
//   C O L O R   T R A N S F O R M A T I O N    //
//												//
//////////////////////////////////////////////////

template <class Dummy>
bool ColorTransform(KDIB * dib, Dummy map)
{
	// support OS/2 DIB color table: 1-bpp, 4-bpp, 8-bpp, include RLE compression
	if ( dib->m_pRGBTRIPLE )
	{
		for (int i=0; i<dib->m_nClrUsed; i++)
			map(dib->m_pRGBTRIPLE[i].rgbtRed, dib->m_pRGBTRIPLE[i].rgbtGreen, dib->m_pRGBTRIPLE[i].rgbtBlue);

		return true;
	}

	// support Windows DIB color table: 1-bpp, 2-bpp, 4-bpp, 8-bpp, include RLE compression
	if ( dib->m_pRGBQUAD )
	{
		for (int i=0; i<dib->m_nClrUsed; i++)
			map(dib->m_pRGBQUAD[i].rgbRed, dib->m_pRGBQUAD[i].rgbGreen, dib->m_pRGBQUAD[i].rgbBlue);

		return true;
	}
	
	for (int y=0; y<dib->m_nHeight; y++)
	{
		int width = dib->m_nWidth;
		unsigned char * pBuffer = (unsigned char *) dib->m_pBits + dib->m_nBPS * y;
		
		switch ( dib->m_nImageFormat )
		{
			case DIB_16RGB555: 	// 15 bit RGB color image, 5-5-5, 1 bit unused
				for (; width>0; width--)
				{
					BYTE red   = ( (* (WORD *) pBuffer) & 0x7C00 ) >> 7;
					BYTE green = ( (* (WORD *) pBuffer) & 0x03E0 ) >> 2;
					BYTE blue  = ( (* (WORD *) pBuffer) & 0x001F ) << 3;
					
					map( red, green, blue );
					
					* ( WORD *) pBuffer = ( ( red >> 3 ) << 10 ) | ( ( green >> 3 ) << 5 ) | ( blue >> 3 );
					
					pBuffer += 2;
				}
				break;

			case DIB_16RGB565:	// 16 bit RGB color image, 5-6-5
				for (; width>0; width--)
				{
					const DWORD StandardMask565[] =  { 0x00F800, 0x0007E0, 0x00001F };

					BYTE red   = ( (* (WORD *) pBuffer) & 0xF800 ) >> 8;
					BYTE green = ( (* (WORD *) pBuffer) & 0x07E0 ) >> 3;
					BYTE blue  = ( (* (WORD *) pBuffer) & 0x001F ) << 3;
					
					map( red, green, blue );
					
					* ( WORD *) pBuffer = ( ( red >> 3 ) << 11 ) | ( ( green >> 2 ) << 5 ) | ( blue >> 3 );
					
					pBuffer += 2;
				}
				break;

			case DIB_24RGB888:	// 24-bpp RGB
				for (; width>0; width--)
				{
					map( pBuffer[2], pBuffer[1], pBuffer[0] );
					pBuffer += 3;
				}
				break;

			case DIB_32RGBA8888: // 32-bpp RGBA
			case DIB_32RGB888:   // 32-bpp RGB
				for (; width>0; width--)
				{
					map( pBuffer[2], pBuffer[1], pBuffer[0] );
					pBuffer += 4;
				}
				break;

			default:
				return false;
		}
	}
	
	return true;
}


bool KImage::ToGreyScale(void)
{
	return ColorTransform(this, MaptoGray);
}


BYTE   redGammaRamp[256];
BYTE greenGammaRamp[256];
BYTE  blueGammaRamp[256];

inline void MapGamma(BYTE & red, BYTE & green, BYTE & blue)
{
	red   =   redGammaRamp[red];
	green = greenGammaRamp[green];
	blue  =  blueGammaRamp[blue];
}


BYTE gamma(double g, int index)
{
	return min(255, (int) ( (255.0 * pow(index/255.0, 1.0/g)) + 0.5 ) ); 
} 
    
bool KImage::GammaCorrect(double redgamma, double greengamma, double bluegamma)
{
	for (int i=0; i<256; i++)
	{
		  redGammaRamp[i] = gamma(  redgamma, i);
		greenGammaRamp[i] = gamma(greengamma, i);
		 blueGammaRamp[i] = gamma( bluegamma, i);
	}

	return ColorTransform(this, MapGamma);
}


bool KImage::Lighten(void)
{
	return ColorTransform(this, LightenColor);
}


//////////////////////////////////////////////////////////////


void KPixelMapper::Map1bpp(BYTE * pBuffer, int width)
{
	BYTE mask  = 0x80;
	int  shift = 7;

	for (; width>0; width--)
	{
		BYTE index = ( ( pBuffer[0] & mask ) >> shift ) & 0x1;

		if ( MapIndex(index) )
			pBuffer[0] =  ( pBuffer[0] & ~ mask ) || (( index & 0x0F) << shift);

		mask  >>= 1;
		shift -= 1;

		if ( mask==0 )
		{
			pBuffer ++; mask = 0x80; shift = 7;
		}
	}
}


void KPixelMapper::Map2bpp(BYTE * pBuffer, int width)
{
	BYTE mask  = 0xC0;
	int  shift = 6;

	for (; width>0; width--)
	{
		BYTE index = ( ( pBuffer[0] & mask ) >> shift ) & 0x3;

		if ( MapIndex(index) )
			pBuffer[0] =  ( pBuffer[0] & ~ mask ) || (( index & 0x0F) << shift);

		mask  >>= 2;
		shift -= 2;

		if ( mask==0 )
		{
			pBuffer ++; mask = 0xC0; shift = 6;
		}
	}
}


void KPixelMapper::Map4bpp(BYTE * pBuffer, int width)
{
	BYTE mask  = 0xF0;
	int  shift = 4;

	for (; width>0; width--)
	{
		BYTE index = ( ( pBuffer[0] & mask ) >> shift ) & 0x0F;

		if ( MapIndex(index) )
			pBuffer[0] =  ( pBuffer[0] & ~ mask ) || (( index & 0x0F) << shift);

		mask  = ~ mask;
		shift = 4 - shift;

		if ( mask==0xF0 )
			pBuffer ++;
	}
}


void KPixelMapper:: Map8bpp(BYTE * pBuffer, int width)
{
	for (; width>0; width--)
	{
		MapIndex(pBuffer[0]);
		pBuffer ++;
	}
}


void KPixelMapper::Map555(BYTE * pBuffer, int width)
{
	for (; width>0; width--)
	{
		BYTE red   = ( (* (WORD *) pBuffer) & 0x7C00 ) >> 7;
		BYTE green = ( (* (WORD *) pBuffer) & 0x03E0 ) >> 2;
		BYTE blue  = ( (* (WORD *) pBuffer) & 0x001F ) << 3;
					
		if ( MapRGB( red, green, blue ) )
			* ( WORD *) pBuffer = ( ( red >> 3 ) << 10 ) | ( ( green >> 3 ) << 5 ) | ( blue >> 3 );
					
		pBuffer += 2;
	}
}


void KPixelMapper::Map565(BYTE * pBuffer, int width)
{
	for (; width>0; width--)
	{
		BYTE red   = ( (* (WORD *) pBuffer) & 0xF800 ) >> 8;
		BYTE green = ( (* (WORD *) pBuffer) & 0x07E0 ) >> 3;
		BYTE blue  = ( (* (WORD *) pBuffer) & 0x001F ) << 3;
					
		if ( MapRGB( red, green, blue ) )
			* ( WORD *) pBuffer = ( ( red >> 3 ) << 11 ) | ( ( green >> 2 ) << 5 ) | ( blue >> 3 );
					
		pBuffer += 2;
	}
}


void KPixelMapper::Map24bpp(BYTE * pBuffer, int width)
{
	for (; width>0; width--)
	{
		MapRGB( pBuffer[2], pBuffer[1], pBuffer[0] );
		pBuffer += 3;
	}
}


void KPixelMapper::Map32bpp(BYTE * pBuffer, int width)
{
	for (; width>0; width--)
	{
		MapRGB( pBuffer[2], pBuffer[1], pBuffer[0] );
		pBuffer += 4;
	}
}


BITMAPINFO * KChannel::Split(KImage & dib, Operator oper)
{
	m_Operator = oper;
	m_nBPS     = (dib.GetWidth() + 3) / 4 * 4;	// scanline size for 8-bpp DIB
	
	int headsize = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

	BITMAPINFO * pNewDIB = (BITMAPINFO *) new BYTE[headsize + m_nBPS * abs(dib.GetHeight())];

	memset(pNewDIB, 0, headsize);

	pNewDIB->bmiHeader.biSize		 = sizeof(BITMAPINFOHEADER); 
    pNewDIB->bmiHeader.biWidth		 = dib.GetWidth(); 
    pNewDIB->bmiHeader.biHeight		 = dib.GetHeight(); 
    pNewDIB->bmiHeader.biPlanes		 = 1; 
    pNewDIB->bmiHeader.biBitCount	 = 8; 
    pNewDIB->bmiHeader.biCompression = BI_RGB; 

	for (int c=0; c<256; c++)
	{
		pNewDIB->bmiColors[c].rgbRed   = c;
		pNewDIB->bmiColors[c].rgbGreen = c;
		pNewDIB->bmiColors[c].rgbBlue  = c;
	}

	m_pBits  = (BYTE*) & pNewDIB->bmiColors[256];
	
	if ( pNewDIB==NULL )
		return NULL;

	dib.PixelTransform(* this);

	return pNewDIB;
}


void KHistogram::Sample(KImage & dib)
{
	memset(m_FreqRed,   0, sizeof(m_FreqRed));
	memset(m_FreqGreen, 0, sizeof(m_FreqGreen));
	memset(m_FreqBlue,  0, sizeof(m_FreqBlue));
	memset(m_FreqGray,  0, sizeof(m_FreqGray));
	
	dib.PixelTransform(* this);
}


void KFilter::Filter8bpp(BYTE * pDest, BYTE * pSource, int nWidth, int dy)
{
	memcpy(pDest, pSource, m_nHalf); 
	pDest   += m_nHalf; 
	pSource += m_nHalf;

	for (int i=nWidth - 2 * m_nHalf; i>0; i--)
		* pDest ++ = Kernel(pSource++, 1, dy);
		
	memcpy(pDest, pSource, m_nHalf); 
}


void KFilter::Filter24bpp(BYTE * pDest, BYTE * pSource, int nWidth, int dy)
{
	memcpy(pDest, pSource, m_nHalf * 3);
	pDest   += m_nHalf * 3;
	pSource += m_nHalf * 3;

	for (int i=nWidth - 2 * m_nHalf; i>0; i--)
	{
		* pDest ++ = Kernel(pSource++, 3, dy);
		* pDest ++ = Kernel(pSource++, 3, dy);
		* pDest ++ = Kernel(pSource++, 3, dy);
	}

	memcpy(pDest, pSource, m_nHalf * 3);
}


void KFilter::Filter32bpp(BYTE * pDest, BYTE * pSource, int nWidth, int dy)
{
	memcpy(pDest, pSource, m_nHalf * 4);
	pDest   += m_nHalf * 4;
	pSource += m_nHalf * 4;

	for (int i=nWidth - 2 * m_nHalf; i>0; i--)
	{
		* pDest ++ = Kernel(pSource++, 4, dy);
		* pDest ++ = Kernel(pSource++, 4, dy);
		* pDest ++ = Kernel(pSource++, 4, dy);
		* pDest ++ = * pSource++;				// copy alpha channel
	}

	memcpy(pDest, pSource, m_nHalf * 4);
}


void Describe33Filter(HDC hDC, int x, int y, const int * matrix, TCHAR * name, int weight, int add)
{
	SelectObject(hDC, GetStockObject(SYSTEM_FIXED_FONT));

	TextOut(hDC, x, y, name, _tcslen(name)); 

	if ( matrix )
	{
		y+=20;

		TCHAR number[32];

		for (int i=0; i<3; i++)
		{
			wsprintf(number, _T("%c |%2d %2d %2d|"), 
				(i==1) ? '*' : ' ',
				matrix[i*3], matrix[i*3+1], matrix[i*3+2]);
			TextOut(hDC, x + 5, y + i * 15, number, _tcslen(number));
		}

		if ( add )
			wsprintf(number, _T("/ %d + %d"), weight, add);
		else
			wsprintf(number, _T("/ %d"), weight);
	
		TextOut(hDC, x+5, y + 50, number, _tcslen(number));
	}
}
	

TCHAR szSmooth[]        = _T("Smooth");
TCHAR szGuasianSmooth[] = _T("Guasian Smooth");
TCHAR szSharpening[]    = _T("Sharpening");
TCHAR szLaplasian[]     = _T("Laplasian");
TCHAR szEmboss135[]     = _T("Emboss 135°");
TCHAR szEmboss90[]      = _T("Emboss 90° 50%");

K33Filter< 1,  1,  1,  1,  1,  1,  1,  1,  1, 9,   0, false, szSmooth        > filter33_smooth;
K33Filter< 0,  1,  0,  1,  4,  1,  0,  1,  0, 8,   0, false, szGuasianSmooth > filter33_guasiansmooth;
K33Filter< 0, -1,  0, -1,  9, -1,  0, -1,  0, 5,   0, true,  szSharpening    > filter33_sharpening;
K33Filter<-1, -1, -1, -1,  8, -1, -1, -1, -1, 1, 128, true,  szLaplasian     > filter33_laplasian;
K33Filter< 1,  0,  0,  0,  0,  0,  0,  0, -1, 1, 128, true,  szEmboss135     > filter33_emboss135;
K33Filter< 0,  1,  0,  0,  0,  0,  0, -1,  0, 2, 128, true,  szEmboss90      > filter33_emboss90;

KDilation filter_dilation;
KErosion  filter_erosion;
KOutline  filter_outline;

KFilter * StockFilters[] = {
	NULL,
	& filter33_smooth,
	& filter33_guasiansmooth,
	& filter33_sharpening,
	& filter33_laplasian,
	& filter33_emboss135,
	& filter33_emboss90,
	& filter_dilation,
	& filter_erosion,
	& filter_outline
};

