//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : jpeg.h							                                     //
//  Description: KPicture class, wrapper for JPEG encoding/decoding                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KPicture
{
	void Release(void);
	int	 Allocate(int width, int height, int channels, bool bBits=true);

public:
	BITMAPINFO * m_pBMI;
	BYTE	   * m_pBits;

	BYTE	   * m_pJPEG;
	int	         m_nJPEGSize;

	KPicture()
	{
		m_pBMI  = NULL;
		m_pBits = NULL;

		m_pJPEG     = NULL;
		m_nJPEGSize = 0;
	}

	~KPicture()
	{
		Release();

		if ( m_pJPEG )
			delete [] m_pJPEG;
	}

	int GetWidth(void) const
	{
		return m_pBMI->bmiHeader.biWidth;
	}

	int GetHeight(void) const
	{
		return m_pBMI->bmiHeader.biHeight;
	}

	BOOL DecodeJPEG(const void * jpegimage, int jpegsize);
	BOOL QueryJPEG(const void * jpegimage, int jpegsize);

	BOOL LoadJPEGFile(const TCHAR * filename);
	BOOL SaveJPEGFile(const TCHAR * fileName, int quality);
};