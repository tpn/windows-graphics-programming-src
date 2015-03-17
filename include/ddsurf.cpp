//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : ddsurf.cpp						                                     //
//  Description: DirectDraw surface wrapper                                          //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <ddraw.h>
#include <d3d.h>

#include "basicdib.h"
#include "ddsurf.h"
#include "ddwrap.h"

KDDSurface::KDDSurface()
{
	m_pSurface = NULL;
	m_hDC      = NULL;

    memset(& m_ddsd, 0, sizeof(m_ddsd));
   	m_ddsd.dwSize = sizeof(m_ddsd);
}

void KDDSurface::Discharge(void) // release before destructor
{
	ReleaseDC();
    SAFE_RELEASE(m_pSurface);
}


BYTE * KDDSurface::LockSurface(RECT * pRect)
{
    if ( FAILED(m_pSurface->Lock(pRect, & m_ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)) )
		return NULL;
	else
		return (BYTE *) m_ddsd.lpSurface;
}

const DDSURFACEDESC2 * KDDSurface::GetSurfaceDesc(void)
{
	if ( SUCCEEDED(m_pSurface->GetSurfaceDesc(& m_ddsd)) )
		return & m_ddsd;
	else
		return NULL;
}

HRESULT KDDSurface::Unlock(RECT * pRect)
{
	m_ddsd.lpSurface = NULL;  // make it unavailable

	return m_pSurface->Unlock(pRect);
}


HRESULT KDDSurface::RestoreSurface(void) // restore if lost
{
	if ( m_pSurface )
        if ( m_pSurface->IsLost() )
			return m_pSurface->Restore();
		else
			return DD_OK;
	else
		return E_FAIL;
}


HRESULT KDDSurface::GetDC(void)
{
    return m_pSurface->GetDC(&m_hDC);
}


HRESULT KDDSurface::ReleaseDC(void)
{
	if ( m_hDC==NULL )
		return S_OK;
	
	HRESULT hr = m_pSurface->ReleaseDC(m_hDC);
	m_hDC = NULL;
	return hr;
}

HRESULT KDDSurface::CreatePrimarySurface(IDirectDraw7 * pDD, int nBufferCount)
{
	if ( nBufferCount==0 )
	{
		m_ddsd.dwFlags			 = DDSD_CAPS;
		m_ddsd.ddsCaps.dwCaps	 = DDSCAPS_PRIMARYSURFACE;
	}
	else
	{
		m_ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		m_ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX |
								   DDSCAPS_VIDEOMEMORY;
		m_ddsd.dwBackBufferCount = nBufferCount;
	}
	
	return pDD->CreateSurface(& m_ddsd, & m_pSurface, NULL);
}

HRESULT SetPixelFormat(DDPIXELFORMAT & pixelformat, int bpp)
{
	memset(& pixelformat, 0, sizeof(pixelformat));

	pixelformat.dwSize  = sizeof(pixelformat);

	switch ( bpp )
	{
		case 1 : 
			pixelformat.dwFlags				= DDPF_RGB | DDPF_PALETTEINDEXED1; 
			pixelformat.dwRGBBitCount		= 1;
			break;
		
		case 2 : 
			pixelformat.dwFlags			    = DDPF_RGB | DDPF_PALETTEINDEXED2; 
			pixelformat.dwRGBBitCount		= 2;
			break;
		
		case 4 : 
			pixelformat.dwFlags				= DDPF_RGB | DDPF_PALETTEINDEXED4; 
			pixelformat.dwRGBBitCount		= 4;
			break;
		
		case 8 : 
			pixelformat.dwFlags				= DDPF_RGB | DDPF_PALETTEINDEXED8; 
			pixelformat.dwRGBBitCount		= 8;
			break;
    
		case 15: // 1-5-5-5
			pixelformat.dwFlags				= DDPF_RGB | DDPF_ALPHAPIXELS;
			pixelformat.dwRGBBitCount		= 16;
			pixelformat.dwRGBAlphaBitMask   = 0x00008000;
			pixelformat.dwRBitMask			= 0x00007C00;
			pixelformat.dwGBitMask			= 0x000003E0;
			pixelformat.dwBBitMask			= 0x0000001F;
			break;

		case 16: // 0-5-6-5
			pixelformat.dwFlags				= DDPF_RGB;
			pixelformat.dwRGBBitCount		= 16;
			pixelformat.dwRGBAlphaBitMask   = 0x00000000;
			pixelformat.dwRBitMask			= 0x0000F800;
			pixelformat.dwGBitMask			= 0x000007E0;
			pixelformat.dwBBitMask			= 0x0000001F;
			break;
		
		case 24: // 0-8-8-8
			pixelformat.dwFlags				= DDPF_RGB;
			pixelformat.dwRGBBitCount		= 24;
			pixelformat.dwRGBAlphaBitMask   = 0x00000000;
			pixelformat.dwRBitMask			= 0x00FF0000;
			pixelformat.dwGBitMask			= 0x0000FF00;
			pixelformat.dwBBitMask			= 0x000000FF;
			break;

		case 32: // 8-8-8-8
			pixelformat.dwFlags				= DDPF_RGB | DDPF_ALPHAPIXELS;
			pixelformat.dwRGBBitCount		= 32;
			pixelformat.dwRGBAlphaBitMask   = 0xFF000000;
			pixelformat.dwRBitMask			= 0x00FF0000;
			pixelformat.dwGBitMask			= 0x0000FF00;
			pixelformat.dwBBitMask			= 0x000000FF;
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}

const DWORD MEMFLAGS[] =
{
	0,
	DDSCAPS_SYSTEMMEMORY,
	DDSCAPS_NONLOCALVIDMEM | DDSCAPS_VIDEOMEMORY,
	DDSCAPS_LOCALVIDMEM    | DDSCAPS_VIDEOMEMORY
};

HRESULT KOffScreenSurface::CreateOffScreenSurfaceBpp(IDirectDraw7 * pDD, int width, int height, int bpp, int mem)
{
	m_ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
	m_ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | MEMFLAGS[mem];
	m_ddsd.dwWidth        = width;
	m_ddsd.dwHeight       = height;

	if ( SUCCEEDED(SetPixelFormat(m_ddsd.ddpfPixelFormat, bpp)) )
		return pDD->CreateSurface(& m_ddsd, & m_pSurface, NULL);
	else
		return E_FAIL;
}


HRESULT KOffScreenSurface::CreateOffScreenSurface(IDirectDraw7 * pDD, int width, int height, int mem)
{
	m_ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	m_ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE | MEMFLAGS[mem];
	m_ddsd.dwWidth        = width;
	m_ddsd.dwHeight       = height;

	return pDD->CreateSurface(& m_ddsd, & m_pSurface, NULL);
}


HRESULT CALLBACK TextureCallback(DDPIXELFORMAT* pddpf, void * param)
{
	// find a simple >=16-bpp texture format
    if ( (pddpf->dwFlags & (DDPF_LUMINANCE|DDPF_BUMPLUMINANCE|DDPF_BUMPDUDV|DDPF_ALPHAPIXELS))==0 )
    if ( (pddpf->dwFourCC == 0) && (pddpf->dwRGBBitCount>=16) )
	{
	    memcpy(param, pddpf, sizeof(DDPIXELFORMAT) );
		return DDENUMRET_CANCEL; // stop search
	}

    return DDENUMRET_OK; // continue
}


HRESULT KOffScreenSurface::CreateTextureSurface(IDirect3DDevice7 * pD3DDevice, IDirectDraw7 * pDD, unsigned width, unsigned height)
{
    // query device caps
    D3DDEVICEDESC7 ddDesc;

	HRESULT hr = pD3DDevice->GetCaps(&ddDesc);
	if ( FAILED(hr) )
        return hr;

    m_ddsd.dwFlags         = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH |
                             DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
    m_ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
    m_ddsd.dwWidth         = width;
    m_ddsd.dwHeight        = height;

    // Turn on texture management for hardware devices
    if ( (ddDesc.deviceGUID == IID_IDirect3DHALDevice) || 
		 (ddDesc.deviceGUID == IID_IDirect3DTnLHalDevice) )
		m_ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
    else
        m_ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

    // Adjust width and height, if the driver requires it
    if ( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2 )
    {
        for ( m_ddsd.dwWidth=1;  width  > m_ddsd.dwWidth;   m_ddsd.dwWidth<<=1 );
        for ( m_ddsd.dwHeight=1; height > m_ddsd.dwHeight; m_ddsd.dwHeight<<=1 );
    }
    
	if ( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
    {
        if ( m_ddsd.dwWidth > m_ddsd.dwHeight ) 
			m_ddsd.dwHeight = m_ddsd.dwWidth;
        else
			m_ddsd.dwWidth  = m_ddsd.dwHeight;
    }

	memset(& m_ddsd.ddpfPixelFormat, 0, sizeof(m_ddsd.ddpfPixelFormat));
    pD3DDevice->EnumTextureFormats(TextureCallback, & m_ddsd.ddpfPixelFormat);
    
	if ( m_ddsd.ddpfPixelFormat.dwRGBBitCount )
		return pDD->CreateSurface( & m_ddsd, & m_pSurface, NULL );
	else
        return E_FAIL;
}

HRESULT KDDSurface::DrawBitmap(const BITMAPINFO * pDIB, int x, int y, int w, int h)
{
	if ( SUCCEEDED(GetDC()) )
    {
		StretchDIBits(m_hDC, x, y, w, h,
			0, 0, pDIB->bmiHeader.biWidth, pDIB->bmiHeader.biHeight, 
			& pDIB->bmiColors[GetDIBColorCount(pDIB)], pDIB, DIB_RGB_COLORS, SRCCOPY);

        return ReleaseDC();
    }
	else
		return E_FAIL;
}

HRESULT KOffScreenSurface::CreateTextureSurface(IDirect3DDevice7 * pD3DDevice, IDirectDraw7 * pDD, const BITMAPINFO * pDIB)
{
	if ( pDIB==NULL )
		return E_FAIL;

	HRESULT hr = CreateTextureSurface(pD3DDevice, pDD, pDIB->bmiHeader.biWidth, pDIB->bmiHeader.biHeight);
	if ( FAILED(hr) )
		return hr;

	return DrawBitmap(pDIB, 0, 0, m_ddsd.dwWidth, m_ddsd.dwHeight);
}


HRESULT KOffScreenSurface::CreateTextureSurface(IDirect3DDevice7 * pD3DDevice, IDirectDraw7 * pDD, const TCHAR * pFileName)
{
	BITMAPINFO * pDIB = LoadBMPFile(pFileName);

	if ( pDIB )
	{
		HRESULT hr = CreateTextureSurface(pD3DDevice, pDD, pDIB);
	
		delete [] (BYTE *) pDIB;

		return hr;
	}
	else
		return E_FAIL;
}


HRESULT KOffScreenSurface::CreateBitmapSurface(IDirectDraw7 * pDD, const BITMAPINFO * pDIB, int mem)
{
	if ( pDIB==NULL )
		return E_FAIL;

	HRESULT hr = CreateOffScreenSurface(pDD, pDIB->bmiHeader.biWidth, abs(pDIB->bmiHeader.biHeight), mem);
	if ( FAILED(hr) )
		return hr;

	return DrawBitmap(pDIB, 0, 0, m_ddsd.dwWidth, m_ddsd.dwHeight);
}


HRESULT KOffScreenSurface::CreateBitmapSurface(IDirectDraw7 * pDD, const TCHAR * pFileName, int mem)
{
	BITMAPINFO * pDIB = LoadBMPFile(pFileName);

	if ( pDIB )
	{
		HRESULT hr = CreateBitmapSurface(pDD, pDIB, mem);
	
		delete [] (BYTE *) pDIB;

		return hr;
	}
	else
		return E_FAIL;
}


// Return first Z-buffer format found
HRESULT WINAPI EnumZBufferCallBack(DDPIXELFORMAT * pddpf, void * pResult)
{
    if ( pddpf->dwFlags == DDPF_ZBUFFER )
    {
        memcpy(pResult, pddpf, sizeof(DDPIXELFORMAT));
        return D3DENUMRET_CANCEL;
    }
    else
        return D3DENUMRET_OK;
}


HRESULT KOffScreenSurface::CreateZBuffer(IDirect3D7 * pD3D, IDirectDraw7 * pDD, REFCLSID riidDevice,
			 int width, int height)
{
    memset(& m_ddsd.ddpfPixelFormat, 0, sizeof(DDPIXELFORMAT));

    pD3D->EnumZBufferFormats(riidDevice, EnumZBufferCallBack, & m_ddsd.ddpfPixelFormat);

    if ( m_ddsd.ddpfPixelFormat.dwSize==sizeof(DDPIXELFORMAT) )
    {
        m_ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
        m_ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
        m_ddsd.dwWidth        = width;
        m_ddsd.dwHeight       = height;

        return pDD->CreateSurface(& m_ddsd, & m_pSurface, NULL);
    }
    else
        return E_FAIL;
}

/////////////////////////////////////////////

HRESULT KDDSurface::SetClipper(IDirectDraw7 * pDD, HWND hWnd)
{
	IDirectDrawClipper * pClipper;

	HRESULT hr = pDD->CreateClipper(0, & pClipper, NULL);
	if ( FAILED( hr ) )
		return hr;

	pClipper->SetHWnd(0, hWnd);
    
	m_pSurface->SetClipper(pClipper);

	return pClipper->Release();
}


HRESULT KDDSurface::FillColor(int x0, int y0, int x1, int y1, DWORD fillcolor)
{
	DDBLTFX fx;
	fx.dwSize	   = sizeof(fx);
	fx.dwFillColor = fillcolor;

	RECT rc  = { x0, y0, x1, y1 };
			
	return m_pSurface->Blt(& rc, NULL, NULL, DDBLT_COLORFILL, & fx);
}


DWORD KDDSurface::ColorMatch(BYTE red, BYTE green, BYTE blue)
{
	if ( m_ddsd.ddpfPixelFormat.dwSize==0 ) // not initialized
		GetSurfaceDesc();					// get surface description with pixelformat

	const DDPIXELFORMAT & pf = m_ddsd.ddpfPixelFormat;

	if ( pf.dwFlags & DDPF_RGB ) 
	{
		// x-5-5-5
		if ( (pf.dwRBitMask	== 0x7C00) && (pf.dwGBitMask == 0x03E0) && (pf.dwBBitMask==0x001F) )
			return ((red>>3)<<10) | ((green>>3)<<5) | (blue>>3);

		// 0-5-6-5
		if ( (pf.dwRBitMask	== 0xF800) && (pf.dwGBitMask == 0x07E0) && (pf.dwBBitMask==0x001F) )
			return ((red>>3)<<11) | ((green>>2)<<5) | (blue>>3);

		// x-8-8-8
		if ( (pf.dwRBitMask	== 0xFF0000) && (pf.dwGBitMask == 0xFF00) && (pf.dwBBitMask==0xFF) )
			return (red<<16) | (green<<8) | blue;
	}

	DWORD rslt = 0;

	if ( SUCCEEDED(GetDC()) )								// get GDI DC
	{
		int typ      = GetObjectType(m_hDC);
		HBITMAP hBmp = (HBITMAP) GetCurrentObject(m_hDC, OBJ_BITMAP);
		DIBSECTION dibsec;
		GetObject(hBmp, sizeof(dibsec), & dibsec);

		COLORREF old = ::GetPixel(m_hDC, 0, 0);				// save original pixel
		::SetPixel(m_hDC, 0, 0, RGB(red, green, blue));		// put RGB pixel
		ReleaseDC();

		const DWORD * pSurface = (DWORD *) LockSurface();   // lock surface
 
		if ( pSurface )
		{
			rslt = * pSurface;							    // read first DWORD
			if ( pf.dwRGBBitCount < 32 )
				rslt &=  (1 << pf.dwRGBBitCount) - 1;		// truncate to its bpp
			Unlock();										// unlock surface
		}
		else
			assert(false);
	
		GetDC();
		::SetPixel(m_hDC, 0, 0, old);							// put original pixel back
		ReleaseDC();										// release GDI DC
	}
	else
		assert(false);

	return rslt;
}

HRESULT KDDSurface::BitBlt(int x, int y, int w, int h, IDirectDrawSurface7 * pSrc, DWORD flag)
{
	RECT rc = { x, y, x+w, y+h };

	return m_pSurface->Blt(& rc, pSrc, NULL, flag, NULL);
}


HRESULT KDDSurface::SetSourceColorKey(DWORD color)
{
	DDCOLORKEY key;

	key.dwColorSpaceLowValue  = color;
	key.dwColorSpaceHighValue = color;
	
	return m_pSurface->SetColorKey(DDCKEY_SRCBLT, & key);
}

DWORD KDDSurface::GetPixel(int x, int y)
{
	KLockedSurface frame;
		
	if ( frame.Initialize(* this) )
	{
		DWORD rslt = frame.GetPixel(x, y);
		Unlock();
		return rslt;
	}
	else
		return 0;
}


BOOL KDDSurface::SetPixel(int x, int y, DWORD color)
{
	KLockedSurface frame;
	
	if ( frame.Initialize(* this) )
	{
		frame.SetPixel(x, y, color);
		Unlock();
		return TRUE;
	}
	else
		return FALSE;
}


void KLockedSurface::Line(int x0, int y0, int x1, int y1, DWORD color)
{
	int bps       = (bpp+7) / 8;					  // bytes-per-pixel
	BYTE * pPixel = pSurface + pitch * y0 + bps * x0; // first pixel address

	int error;						// error
	int d_pixel_pos, d_error_pos;   // adjustment to pixel address and error when error>=0
	int d_pixel_neg, d_error_neg;	// adjustment to pixel address and error when error<0
	int dots;						// number of dots to draw

	{
		int dx, dy, inc_x, inc_y;

		if ( x1 > x0 )
		{ 	dx = x1 - x0; inc_x =  bps; }
		else
		{	dx = x0 - x1; inc_x = -bps; }

		if ( y1 > y0 )
		{ 	dy = y1 - y0; inc_y =  pitch; }
		else
		{	dy = y0 - y1; inc_y = -pitch; }

		d_pixel_pos = inc_x + inc_y;    // move x and y
		d_error_pos = (dy - dx) * 2;
	
		if ( d_error_pos < 0 ) // x dominant
		{
			dots        = dx;
			error       = dy*2 - dx;
			d_pixel_neg = inc_x;		// move x only
			d_error_neg = dy * 2;
		}
		else
		{
			dots        = dy;
			error       = dx*2 - dy;
			d_error_pos = - d_error_pos;
			d_pixel_neg = inc_y;		// move y only
			d_error_neg = dx * 2;
		}
	}

	switch ( bps )
	{
		case 1:
			for (; dots>=0; dots--)			// 8-bpp pixel loop
			{
				pPixel[0] = (BYTE) color;	// draw 8-bpp pixel
			
				if ( error>=0 )
				{  pPixel += d_pixel_pos; error  += d_error_pos; }
				else
				{  pPixel += d_pixel_neg; error  += d_error_neg; }
			}
			break;

		case 2:
			for (; dots>=0; dots--)			// 16-bpp pixel loop
			{
				* (WORD *) pPixel = (WORD) color;	// draw 16-bpp pixel
			
				if ( error>=0 )
				{  pPixel += d_pixel_pos; error  += d_error_pos; }
				else
				{  pPixel += d_pixel_neg; error  += d_error_neg; }
			}
			break;

		case 3:
			for (; dots>=0; dots--)			// 24-bpp pixel loop
			{
				* (RGBTRIPLE *) pPixel = * (RGBTRIPLE *) & color;	// draw 24-bpp pixel
			
				if ( error>=0 )
				{  pPixel += d_pixel_pos; error  += d_error_pos; }
				else
				{  pPixel += d_pixel_neg; error  += d_error_neg; }
			}
			break;

		case 4:
			for (; dots>=0; dots--)			// 32-bpp pixel loop
			{
				* (DWORD *) pPixel = color;	// draw 32-bpp pixel
			
				if ( error>=0 )
				{  pPixel += d_pixel_pos; error  += d_error_pos; }
				else
				{  pPixel += d_pixel_neg; error  += d_error_neg; }
			}
			break;
	}
}

// 32-bpp Line drawing
/*		 eax : color
         ebx : dots
	     ecx : error
		 edx : pPixel
		 esi : d_error_pos
		 edi : d_error_neg
		 ebp : d_pixel_neg
		 	
			test	ebx, ebx			if ( dots<0 ) goto _finish;
			jl		_finish
			mov		eax, color			eax = color
			inc		ebx					dots ++;
_repeat:	test	ecx, ecx			
			mov		[edx], eax			* (DWORD *) pPixel = color;
			jl		_elsepart			if ( error < 0 ) goto _elsepart
			add		edx, d_pixel_pos    pPixel += d_pixel_pos
			add		ecx, esi            error  += d_error_pos
			jmp		_next			    goto _next
_elsepart:	add		edx, ebp            pPixel += d_pixel_neg
			add		ecx, edi            error  += d_error_neg
_next:		dec		ebx                 dots --;
			jne		_repeat             if ( dots!=0 ) goto _repeat
_finish:
*/

// Bresenham algorithm
BOOL KDDSurface::Line(int x0, int y0, int x1, int y1, DWORD color)
{
	KLockedSurface frame;
		
	if ( frame.Initialize(* this) )
	{
		frame.Line(x0, y0, x1, y1, color);
		Unlock();
		return TRUE;
	}
	else
		return FALSE;
}

BOOL KDDSurface::FillRgn(HRGN hRgn, DWORD color)
{
	RGNDATA * pRegion = GetClipRegionData(hRgn);

	if ( pRegion==NULL )
		return FALSE;

	const RECT * pRect = (const RECT *) pRegion->Buffer;
	
	for (unsigned i=0; i<pRegion->rdh.nCount; i++)
	{
		FillColor(pRect->left, pRect->top, pRect->right, pRect->bottom, color);
		pRect ++;
	}

	delete [] (BYTE *) pRegion;

	return TRUE;
}
