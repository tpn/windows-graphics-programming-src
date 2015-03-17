#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : ddb.h						                                         /
//  Description: Device depedent bitmap handling                                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KDDB
{
protected:
	HBITMAP	m_hBitmap;
	HBITMAP m_hOldBmp;
	
	void ReleaseDDB(void);

public:
	HDC		m_hMemDC;
	bool Prepare(int & width, int & height);

	typedef enum { draw_normal, draw_center, draw_tile, draw_stretch, draw_stretchprop };

	HBITMAP GetBitmap(void) const
	{
		return m_hBitmap;
	}

	KDDB()
	{
		m_hBitmap = NULL;
		m_hMemDC  = NULL;
		m_hOldBmp = NULL;
	}

	virtual ~KDDB()
	{
		ReleaseDDB();
	}

	BOOL Attach(HBITMAP hBmp);

	BOOL LoadBitmap(HINSTANCE hInst, int id)
	{
		return Attach ( ::LoadBitmap(hInst, MAKEINTRESOURCE(id)) );
	}

	BOOL Draw(HDC hDC, int x0, int y0, int w, int h, DWORD rop, int opt=draw_normal);
	HBITMAP CreateMask(COLORREF crBackGround, HDC hMaskDC);
};

void	DecodeDDB(HGDIOBJ hBmp, TCHAR mess[]);
HBITMAP LargestDDB(HDC hDC);

// class for simulating TransparentBlt

class KDDBMask
{
	HDC		m_hMemDC;
	HBITMAP	m_hMask;
	HBITMAP m_hOld;
	int		m_nMaskWidth;
	int		m_nMaskHeight;

	void Release(void)
	{
		if ( m_hMemDC )
		{
			SelectObject(m_hMemDC, m_hOld);
			DeleteObject(m_hMemDC);	
			m_hMemDC = NULL;
			m_hOld = NULL;
		}

		if ( m_hMask )
		{
			DeleteObject(m_hMask);	m_hMask  = NULL;
		}
	}

public:
	KDDBMask()
	{
		m_hMemDC = NULL;
		m_hMask  = NULL;
		m_hOld   = NULL;
	}

	~KDDBMask()
	{
		Release();
	}

	BOOL Create(HDC hDC, int nX, int nY, int nWidth, int nHeight, UINT crTransparent);
	BOOL ApplyMask(HDC HDC, int nX, int nY, int nWidth, int nHeight, DWORD Rop);
	
	BOOL TransBlt(HDC hdcDest, int nDx0, int nDy0, int nDw, int nDh,
			  	  HDC hdcSrc,  int nSx0, int nSy0, int nSw, int nSh);

	BOOL TransBlt_FlickFree(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
					HDC hdcSrc,  int nXOriginSrc,  int nYOriginSrc,  int nWidthSrc,  int nHeightSrc);
};

BOOL G_TransparentBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int nHeightDest,
				    HDC hdcSrc,  int nXOriginSrc,  int nYOriginSrc,  int nWidthSrc,  int nHeightSrc,
					UINT crTransparent);
