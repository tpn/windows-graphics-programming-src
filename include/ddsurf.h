#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : ddsurf.h						                                     //
//  Description: DirectDraw surface wrapper                                          //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define SAFE_RELEASE(inf) { if ( inf ) { inf->Release(); inf = NULL; } }

// Wrapper for IDirectDrawSurface7 interface
class KDDSurface
{
protected:
	IDirectDrawSurface7 * m_pSurface;
	DDSURFACEDESC2        m_ddsd;
	HDC					  m_hDC;

public:

	KDDSurface();
	virtual void Discharge(void); // release before destructor

	virtual ~KDDSurface() // make sure everything is released
	{
		Discharge();
	}
	
	operator IDirectDrawSurface7 * & ()
	{
		return m_pSurface;
	}

	operator HDC ()
	{
		return m_hDC;
	}

	int GetWidth(void) const
	{
		return m_ddsd.dwWidth;
	}

	int GetHeight(void) const
	{
		return m_ddsd.dwHeight;
	}

	HRESULT CreatePrimarySurface(IDirectDraw7 * pDD, int nBackBuffer);
	
	const DDSURFACEDESC2 * GetSurfaceDesc(void);
	virtual HRESULT RestoreSurface(void); // restore surface if lost

	// DirectDraw Blting
	HRESULT  SetClipper(IDirectDraw7 * pDD, HWND hWnd);
	
	HRESULT Blt(LPRECT prDest, IDirectDrawSurface7 * pSrc, LPRECT prSrc, DWORD dwFlags, LPDDBLTFX pDDBltFx=NULL)
	{
		return m_pSurface->Blt(prDest, pSrc, prSrc, dwFlags, pDDBltFx);
	}

	DWORD   ColorMatch(BYTE red, BYTE green, BYTE blue);

	HRESULT FillColor(int x0, int y0, int x1, int y1, DWORD fillcolor);
	HRESULT BitBlt(int x, int y, int w, int h, IDirectDrawSurface7 * pSrc, DWORD flag=0);

	HRESULT BitBlt(int x, int y, KDDSurface & src, DWORD flag=0)
	{
		return BitBlt(x, y, src.GetWidth(), src.GetHeight(), src, flag);
	}

	HRESULT SetSourceColorKey(DWORD color);

	// Drawing using GDI device context
	HRESULT GetDC(void);		// Get DC device context handle
	HRESULT ReleaseDC(void);
	
	HRESULT DrawBitmap(const BITMAPINFO * pDIB, int dx, int dy, int dw, int dh);
	
	// Direct Pixel Access
	BYTE  * LockSurface(RECT * pRect=NULL);
	HRESULT Unlock(RECT * pRect=NULL);

	int GetPitch(void) const
	{
		return m_ddsd.lPitch;
	}

	DWORD GetPixel(int x, int y);
	BOOL  SetPixel(int x, int y, DWORD color);
	BOOL  Line(int x0, int y0, int x1, int y1, DWORD color);
	BOOL  FillRgn(HRGN hRgn, DWORD color);
};


class KLockedSurface
{
public:
	BYTE * pSurface;
	int    pitch;
	int    bpp;

	bool Initialize(KDDSurface & surface)
	{
		pSurface = surface.LockSurface(NULL);
		pitch    = surface.GetPitch();
		bpp      = surface.GetSurfaceDesc()->ddpfPixelFormat.dwRGBBitCount;
		
		return pSurface!=NULL;
	}

	BYTE & ByteAt(int x, int y)
	{
		BYTE * pPixel = (BYTE *) (pSurface + pitch * y);

		return pPixel[x];
	}

	WORD & WordAt(int x, int y)
	{
		WORD * pPixel = (WORD *) (pSurface + pitch * y);

		return pPixel[x];
	}

	RGBTRIPLE & RGBTripleAt(int x, int y)
	{
		RGBTRIPLE * pPixel = (RGBTRIPLE *) (pSurface + pitch * y);

		return pPixel[x];
	}

	DWORD & DWordAt(int x, int y)
	{
		DWORD * pPixel = (DWORD *) (pSurface + pitch * y);

		return pPixel[x];
	}

	BOOL SetPixel(int x, int y, DWORD color)
	{
		switch ( bpp )
		{
			case  8:      ByteAt(x, y) =  (BYTE) color; break;
			case 15:
			case 16:      WordAt(x, y) =  (WORD) color; break;
			case 24: RGBTripleAt(x, y) = * (RGBTRIPLE *) & color; break;
			case 32:     DWordAt(x, y) = (DWORD) color; break;
			default: return FALSE;
		}

		return TRUE;
	}

	DWORD GetPixel(int x, int y)
	{
		switch ( bpp )
		{
			case  8:  return ByteAt(x, y);
			case 15:
			case 16:  return WordAt(x, y);
			case 32:  return DWordAt(x, y);
			case 24: 
				{
					DWORD rslt = 0;
					
					* (RGBTRIPLE *) rslt = RGBTripleAt(x, y); 
					return rslt;
				}
		}

		return 0;
	}

	void Line(int x0, int y0, int x1, int y1, DWORD color);
};

/////////////////////////////////////////////////////////////

typedef enum
{
	mem_default,
	mem_system,
	mem_nonlocalvideo,
	mem_localvideo
};

class KOffScreenSurface : public KDDSurface
{
public:
	HRESULT CreateOffScreenSurface(IDirectDraw7 * pDD, int width, int height, int mem=mem_default);
	HRESULT CreateOffScreenSurfaceBpp(IDirectDraw7 * pDD, int width, int height, int bpp, int mem=mem_default);
	
	HRESULT CreateBitmapSurface(IDirectDraw7 * pDD, const BITMAPINFO * pDIB, int mem=mem_default);
	HRESULT CreateBitmapSurface(IDirectDraw7 * pDD, const TCHAR * pFileName, int mem=mem_default);

	HRESULT CreateTextureSurface(IDirect3DDevice7 * pD3DDevice, IDirectDraw7 * pDD, unsigned width, unsigned height);
	HRESULT	CreateTextureSurface(IDirect3DDevice7 * pD3DDevice, IDirectDraw7 * pDD, const BITMAPINFO * pDIB);
	HRESULT	CreateTextureSurface(IDirect3DDevice7 * pD3DDevice, IDirectDraw7 * pDD, const TCHAR * pFileName);

    HRESULT CreateZBuffer(IDirect3D7 * pD3D, IDirectDraw7 * pDD, REFCLSID riidDevice, int width, int height);

    HRESULT Attach(IDirectDrawSurface7 * pAttach)
    {
        return m_pSurface->AddAttachedSurface(pAttach);
    }
};	

template <int MaxChar>
class KDDFont : public KOffScreenSurface
{
	int      m_offset [MaxChar];  // A width
	int      m_advance[MaxChar];  // A + B + C

	int		 m_pos	  [MaxChar];  // horizontal position within font surface
	int      m_width  [MaxChar];  // - min(A, 0) + B - min(C,0)

	unsigned m_firstchar;
	int		 m_nChar;

public:
	
	HRESULT CreateFont(IDirectDraw7 * pDD, const LOGFONT & lf, unsigned firstchar, unsigned lastchar, COLORREF crColor);
	int     TextOut(IDirectDrawSurface7 * pSurface, int x, int y, const TCHAR * mess, int nChar=0);
};

template <int MaxChar>
HRESULT KDDFont<MaxChar>::CreateFont(IDirectDraw7 * pDD, const LOGFONT & lf, unsigned firstchar, unsigned lastchar, COLORREF crColor)
{
	m_firstchar = firstchar;
	m_nChar     = lastchar - firstchar + 1;
	
	if ( m_nChar > MaxChar )
		return E_INVALIDARG;
	
	HFONT hFont = CreateFontIndirect(&lf);

	if ( hFont==NULL )
		return E_INVALIDARG;

	HRESULT hr;
	ABC     abc[MaxChar];

	int height;
	{
		HDC hDC = ::GetDC(NULL);

		if ( hDC )
		{
			HGDIOBJ hOld = SelectObject(hDC, hFont);

			TEXTMETRIC tm;

			GetTextMetrics(hDC, & tm);
			height = tm.tmHeight;

			if ( GetCharABCWidths(hDC, firstchar, lastchar, abc) )
				hr = S_OK;
			else
				hr = E_INVALIDARG;

			SelectObject(hDC, hOld);
			::ReleaseDC(NULL, hDC);
		}
	}

	if ( SUCCEEDED(hr) )
	{
		int width = 0;
		
		for (int i=0; i<m_nChar; i++)
		{
			m_offset[i]  = abc[i].abcA;
			m_width[i]   =  - min(abc[i].abcA, 0) + abc[i].abcB - min(abc[i].abcC, 0);
			m_advance[i] = abc[i].abcA + abc[i].abcB + abc[i].abcC;
			
			width += m_width[i];
		}

		hr = CreateOffScreenSurface(pDD, width, height);

		if ( SUCCEEDED(hr) )
		{
			GetDC();

			int x = 0;
			
			PatBlt(m_hDC, 0, 0, GetWidth(), GetHeight(), BLACKNESS); // black
			
			SetBkMode(m_hDC, TRANSPARENT);
			SetTextColor(m_hDC, crColor); // white forground

			HGDIOBJ hOld = SelectObject(m_hDC, hFont);
			SetTextAlign(m_hDC, TA_TOP | TA_LEFT);
			
			for (int i=0; i<m_nChar; i++)
			{
				TCHAR ch = firstchar + i;

				m_pos[i] = x;
				::TextOut(m_hDC, x-m_offset[i], 0, & ch, 1);
				x += m_width[i];
			}

			SelectObject(m_hDC, hOld);
			ReleaseDC();

			SetSourceColorKey(0); // black as source color key
		}
	}

	DeleteObject(hFont);

	return hr;
};


template<int MaxChar>
int KDDFont<MaxChar>::TextOut(IDirectDrawSurface7 * pDest, int x, int y, const TCHAR * mess, int nChar)
{
	if ( nChar<=0 )
		nChar = _tcslen(mess);

	for (int i=0; i<nChar; i++)
	{
		int ch = mess[i] - m_firstchar;

		if ( (ch<0) || (ch>m_nChar) )
			ch = 0;

		RECT dst = { x + m_offset[ch],			     y, 
			         x + m_offset[ch] + m_width[ch], y + GetHeight() };

		RECT src = { m_pos[ch], 0, m_pos[ch] + m_width[ch], GetHeight() };

		pDest->Blt(& dst, m_pSurface, & src, DDBLT_KEYSRC, NULL);

		x += m_advance[ch];
	}

	return x;
}

