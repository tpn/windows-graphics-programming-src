//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : ddwrap.cpp						                                     //
//  Description: DirectDraw/Direct3D IM wrapper                                      //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <assert.h>

#include <ddraw.h>
#include <d3d.h>

#include "basicdib.h"
#include "ddsurf.h"
#include "ddwrap.h"

//////////////////////////////////////////////

KDirectDraw::KDirectDraw(void)
{
	m_pDD    = NULL;
}

HRESULT KDirectDraw::Discharge(void)
{
	m_primary.Discharge();

	SAFE_RELEASE(m_pDD);

	return S_OK;
}

void KDirectDraw::SetClientRect(HWND hWnd)
{
	GetClientRect(hWnd, & m_rcDest);
	ClientToScreen(hWnd, (POINT*)& m_rcDest.left);
	ClientToScreen(hWnd, (POINT*)& m_rcDest.right);
}


#define pDriverGUID NULL

HRESULT KDirectDraw::SetupDirectDraw(HWND hTop, HWND hWnd, int nBufferCount,
			 bool bFullScreen, int width, int height, int bpp)
{
    HRESULT hr = DirectDrawCreateEx(pDriverGUID, (void **) &m_pDD, IID_IDirectDraw7, NULL);
    if ( FAILED( hr ) )
        return hr;

	if ( bFullScreen )
		hr = m_pDD->SetCooperativeLevel(hTop, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE);
	else
		hr = m_pDD->SetCooperativeLevel(hTop, DDSCL_NORMAL);
    
	if ( FAILED(hr) )
        return hr;

	if ( bFullScreen )
	{
		hr = m_pDD->SetDisplayMode(width, height, bpp, 0, 0);
		if ( FAILED(hr) )
			return hr;

		SetRect(& m_rcDest, 0, 0, width, height);
	}
	else
		SetClientRect(hWnd);
	
    hr = m_primary.CreatePrimarySurface(m_pDD, nBufferCount);

	if ( FAILED(hr) )
		return hr;

	if ( ! bFullScreen )
		hr = m_primary.SetClipper(m_pDD, hWnd);

	return hr;
}


////////////////////////////////////////////////

HRESULT KDDPalette::LoadPalette(IDirectDraw7 * pDD, const BITMAPINFO * pDIB)
{
	PALETTEENTRY colortable[256];

	memset(colortable, 0, sizeof(colortable));
	
	for (int i=0; i<256; i++)
	{
		colortable[i].peRed   = i;
		colortable[i].peGreen = i;
		colortable[i].peBlue  = i;
	}

	if ( pDIB && (pDIB->bmiHeader.biBitCount<=8) )
	{
		int nColor = min(256, GetDIBColorCount(pDIB));

		const RGBQUAD * pQuad = (const RGBQUAD *) ((BYTE *) pDIB + pDIB->bmiHeader.biSize);

		for (i=0; i<nColor; i++)
		{
			colortable[i].peRed   = pQuad[i].rgbRed;
			colortable[i].peGreen = pQuad[i].rgbGreen;
			colortable[i].peBlue  = pQuad[i].rgbBlue;
		}
	}

	return pDD->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256, colortable, & m_pPalette, NULL);

}



RGNDATA * GetClipRegionData(HRGN hRgn)
{
	DWORD dwSize = GetRegionData(hRgn, 0, NULL);

	RGNDATA * pRgnData = (RGNDATA *) new BYTE[dwSize];

	if ( pRgnData )
		GetRegionData(hRgn, dwSize, pRgnData);
	
	return pRgnData;
}

HRGN GetClipRegion(IDirectDrawClipper * pClipper)
{
	DWORD dwSize = 0;

	if ( FAILED(pClipper->GetClipList(NULL, NULL, & dwSize)) )
		return NULL;

	RGNDATA * pClipData = (RGNDATA *) new BYTE[dwSize];

	if ( pClipData==NULL )
		return NULL;

	pClipper->GetClipList(NULL, pClipData, & dwSize);

	HRGN hRgn = ExtCreateRegion(NULL, dwSize, pClipData);

	delete [] (BYTE *) pClipData;

	return hRgn;
}

BOOL SetClipRegion(IDirectDrawClipper * pClipper, HRGN hRgn)
{
	RGNDATA * pRgnData = GetClipRegionData(hRgn);

	if ( pRgnData==NULL )
		return FALSE;

	HRESULT hr = pClipper->SetClipList(pRgnData, 0);

	delete (BYTE *) pRgnData;

	return SUCCEEDED(hr);
}


///////////////////////////////////////////////
    
//	return m_backsurface.RestoreSurface();

KDirect3D::KDirect3D(void)
{
	m_pD3D		 = NULL;
	m_pD3DDevice = NULL;
	m_bReady     = false;
}


HRESULT KDirect3D::Discharge(void)
{
	SAFE_RELEASE(m_pD3DDevice);
	m_backsurface.Discharge();
	    m_zbuffer.Discharge();

	SAFE_RELEASE(m_pD3D);
	
	return KDirectDraw::Discharge();
}



HRESULT KDirect3D::RestoreSurfaces(void)
{
	   m_backsurface.RestoreSurface();
		   m_zbuffer.RestoreSurface();
	return m_primary.RestoreSurface();
}


int GetDisplayBpp(IDirectDraw7 * pDD)
{
	DDSURFACEDESC2  ddsd;

	memset(& ddsd, 0, sizeof(ddsd));
	
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	
    if ( SUCCEEDED(pDD->GetDisplayMode(& ddsd)) )
        return ddsd.ddpfPixelFormat.dwRGBBitCount;
    else
        return 0;
}


HRESULT KDirect3D::SetupDirectDraw(HWND hTop, HWND hWnd, int nBufferCount,
								   bool bFullScreen, int width, int height, int bpp)
{
    HRESULT hr = KDirectDraw::SetupDirectDraw(hTop, hWnd, nBufferCount, bFullScreen, width, height, bpp);
    if ( FAILED( hr ) )
        return hr;
    
    // reject 8-bpp device
    if ( GetDisplayBpp(m_pDD)<=8 )
        return  DDERR_INVALIDMODE;
    
    // create back surface
    hr = m_backsurface.CreateOffScreenSurface(m_pDD, width, height);
	if ( FAILED(hr) )
		return hr;

//	m_backsurface.FillColor(0, 0, width, height, m_backsurface.ColorMatch(0xFF, 0xFF, 0xFF));
    
    // Query DirectDraw for access to Direct3D
    m_pDD->QueryInterface( IID_IDirect3D7, (void **) & m_pD3D );
    if ( FAILED(hr) )
        return hr;
    
    CLSID iidDevice = IID_IDirect3DHALDevice;

	// create Z-buffer
    hr = m_zbuffer.CreateZBuffer(m_pD3D, m_pDD, iidDevice, width, height);
    
	if ( FAILED(hr) )
	{
		iidDevice = IID_IDirect3DRGBDevice; 
	    hr = m_zbuffer.CreateZBuffer(m_pD3D, m_pDD, iidDevice, width, height);
    }

	if ( FAILED(hr) )
        return hr;

    // attach Z-buffer to back surface
    hr = m_backsurface.Attach(m_zbuffer);
    if ( FAILED(hr) )
        return hr;

    hr = m_pD3D->CreateDevice( iidDevice, m_backsurface, & m_pD3DDevice );
	if ( FAILED(hr) )
		return hr;
	
	{
	    D3DVIEWPORT7 vp = { 0, 0, width, height, (float)0.0, (float)1.0 };
    
		return m_pD3DDevice->SetViewport( &vp );
	}
}

HRESULT KDirect3D::ShowFrame(HWND hWnd)
{
	if ( m_bReady )
	{
		SetClientRect(hWnd);
		
		return m_primary.Blt(& m_rcDest, m_backsurface, NULL, DDBLT_WAIT);
	}
	else
		return S_OK;
}

HRESULT KDirect3D::Render(HWND hWnd)
{
	if ( ! m_bReady )
		return S_OK;

	HRESULT hr = OnRender();

	if ( FAILED(hr) )
		return hr;

    hr = ShowFrame(hWnd);
		
	if ( hr = DDERR_SURFACELOST )
		return RestoreSurfaces();
	else
		return hr;
}


HRESULT KDirect3D::ReCreate(HINSTANCE hInst, HWND hTop, HWND hWnd)
{
	if ( FAILED(OnDischarge()) )
		return E_FAIL;

	if ( FAILED( Discharge() ) ) // free all resources
		return E_FAIL;

	SetClientRect(hWnd);

	HRESULT hr = SetupDirectDraw(hTop, hWnd, 0, false, 
					m_rcDest.right  - m_rcDest.left, 
					m_rcDest.bottom - m_rcDest.top);

	if ( SUCCEEDED(hr) )
		return OnInit(hInst);
	else
		return hr;
}


HRESULT KDirect3D::OnResize(HINSTANCE hInst, int width, int height, HWND hTop, HWND hWnd)
{
	if ( ! m_bReady )
		return S_OK;

	if ( width ==(m_rcDest.right - m_rcDest.left) ) 
	if ( height==(m_rcDest.bottom - m_rcDest.top) )
		return S_OK;

#ifdef _DEBUG
	char temp[MAX_PATH];
	wsprintf(temp, "OnResize %d %d -> %d %d\n", 
		m_rcDest.right - m_rcDest.left, m_rcDest.bottom - m_rcDest.top,
		width, height);
	OutputDebugString(temp);
#endif

	return ReCreate(hInst, hTop, hWnd);
}
