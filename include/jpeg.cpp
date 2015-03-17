//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : jpeg.cpp						                                     //
//  Description: KPicture class, wrapper for JPEG encoding/decoding                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#define XMD_H

#include "jpeg.h"
#include ".\jlib\jpeglib.h"

void KPicture::Release(void)
{
	if ( m_pBMI )
	{
		delete [] (BYTE *) m_pBMI;
		m_pBMI = NULL;
	}

	if ( m_pBits )
	{
		delete [] (BYTE *) m_pBits;
		m_pBits = NULL;
	}
}


int KPicture::Allocate(int width, int height, int channels, bool bBits)
{
	Release();

	switch ( channels )
	{
		case 3:
			m_pBMI = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFOHEADER)];
			break;

		case 1:
			m_pBMI = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)];
			{
				for (int i=0; i<256; i++)
				{
					m_pBMI->bmiColors[i].rgbRed		 = i;
					m_pBMI->bmiColors[i].rgbGreen	 = i;
					m_pBMI->bmiColors[i].rgbBlue     = i;
					m_pBMI->bmiColors[i].rgbReserved = 0;
				}
			}
			break;

		default:
			return 0;
	}

	if ( m_pBMI==NULL )
		return 0;

	memset(m_pBMI, 0, sizeof(BITMAPINFOHEADER));
	m_pBMI->bmiHeader.biSize     = sizeof(m_pBMI->bmiHeader);
	m_pBMI->bmiHeader.biWidth    = width;
	m_pBMI->bmiHeader.biHeight   = height;
	m_pBMI->bmiHeader.biBitCount = channels * 8;
	m_pBMI->bmiHeader.biPlanes   = 1;

	// allocate bitmap
	int	bps = (width * channels + 3) /4 * 4;

	if ( bBits )
		m_pBits = (BYTE *) new BYTE[bps * height];

	return bps;
}


BOOL KPicture::DecodeJPEG(const void * jpegimage, int jpegsize)
{
	try
	{
		struct jpeg_decompress_struct dinfo;

		jpeg_error_mgr jerr;
		dinfo.err  = & jerr;

		dinfo.jpeg_create_decompress();
		jpeg_const_src(&dinfo, (const BYTE *) jpegimage, jpegsize);
		dinfo.jpeg_read_header(TRUE);
		dinfo.jpeg_start_decompress();

		int	bps = Allocate(dinfo.image_width, dinfo.image_height, dinfo.out_color_components, true);

		if ( m_pBits==NULL )
			return FALSE;

		for (int h=dinfo.image_height-1; h>=0; h--) // bottom-up
		{
			BYTE * addr = m_pBits + bps * h; 
			dinfo.jpeg_read_scanlines(& addr, 1);
		}
		
		dinfo.jpeg_finish_decompress();
		dinfo.jpeg_destroy_decompress();

		m_pJPEG     = (BYTE *) jpegimage;
		m_nJPEGSize = jpegsize;
	}
	catch (...)
	{
		return FALSE;
	}
	
	return TRUE;
}


BOOL KPicture::QueryJPEG(const void * jpegimage, int jpegsize)
{
	try
	{
		struct jpeg_decompress_struct dinfo;

		jpeg_error_mgr jerr;
		dinfo.err  = & jerr;

		dinfo.jpeg_create_decompress();

		jpeg_const_src(&dinfo, (const BYTE *) jpegimage, jpegsize);

		dinfo.jpeg_read_header(TRUE);

		Allocate(dinfo.image_width, dinfo.image_height, dinfo.out_color_components, false);

		dinfo.jpeg_destroy_decompress();
	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}


BOOL KPicture::LoadJPEGFile(const TCHAR * filename)
{
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL,
	      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	int size = GetFileSize(hFile, NULL);
	BYTE * pImage = new BYTE[size];

	BOOL rslt = FALSE;

	if ( pImage )
	{
		DWORD dwRead;
		ReadFile(hFile, pImage, size, & dwRead, NULL);

		rslt = DecodeJPEG(pImage, size);
		
	//	delete [] pImage;
	}	

	CloseHandle(hFile);

	return rslt;
}

BOOL KPicture::SaveJPEGFile(const TCHAR * filename, int quality)
{
	if ( (m_pBMI==NULL) || (m_pBits==NULL) )
		return FALSE;

	FILE * outfile = fopen(filename, "wb");

	if  ( outfile==NULL ) 
	{
		assert(false);
		return FALSE;
	}

	try
	{
		struct jpeg_compress_struct cinfo;

		jpeg_error_mgr jerr;
		cinfo.err = &jerr;

		jpeg_create_compress(&cinfo);
		jpeg_stdio_dest(&cinfo, outfile);

		cinfo.image_width  =     m_pBMI->bmiHeader.biWidth;
		cinfo.image_height = abs(m_pBMI->bmiHeader.biHeight);
		
		if ( m_pBMI->bmiHeader.biBitCount==24 ) 
		{
			cinfo.input_components = 3;
			cinfo.in_color_space   = JCS_RGB;
		} 
		else if ( m_pBMI->bmiHeader.biBitCount==8 ) 
		{
			cinfo.input_components = 1;
			cinfo.in_color_space   = JCS_GRAYSCALE;
		}
		else
			return FALSE;

		jpeg_set_defaults(&cinfo);
  		jpeg_set_quality(&cinfo, quality, TRUE);
		jpeg_start_compress(&cinfo, TRUE);

		int bps = ( m_pBMI->bmiHeader.biWidth * cinfo.input_components + 3 ) / 4 * 4;
		
		for (int y=abs(m_pBMI->bmiHeader.biHeight)-1; y>=0; y++)
		{
			const BYTE * BGR = m_pBits + bps * y;

			jpeg_write_scanlines(&cinfo, (BYTE **) & BGR, 1);
		}

		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);
	}
	catch (...)
	{
	}

	fclose(outfile);

	return TRUE;
}

