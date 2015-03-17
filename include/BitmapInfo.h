#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : bitmapinfo.h					                                     //
//  Description: BITMAPINFO wrapper                                                 //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KBitmapInfo
{
public:

	typedef struct
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD	 	     bmiColors[256+3];
	}	DIBINFO;

	DIBINFO m_dibinfo;

	void SetFormat(int width, int height, int bitcount, int compression = BI_RGB);

	void SetGrayScale(DWORD mask);

	BITMAPINFO * GetBMI(void) const
	{
		return (BITMAPINFO *) & m_dibinfo;
	}

	BITMAPINFO * CopyBMI(void) const;
	
};

