#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : ddwrap.h						                                     //
//  Description: DirectDraw/Direct3D wrapper                                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

//////////////////////////////////////////

class KDDPalette
{
protected:
	IDirectDrawPalette   * m_pPalette;

public:
	KDDPalette()
	{
		m_pPalette = NULL;
	}

	~KDDPalette()
	{
		SAFE_RELEASE(m_pPalette);
	}

	HRESULT LoadPalette(IDirectDraw7 * pDD, const BITMAPINFO * pDIB);
};

/////////////////////////////////////////

// Wrapper for IDirectDraw7 interface, supporting primary surface
class KDirectDraw
{
protected:
	IDirectDraw7  *	m_pDD;
	
	RECT			m_rcDest;    // destination rectangle
    KDDSurface	    m_primary;

	virtual HRESULT Discharge(void);

public:

    KDirectDraw(void);
	
	virtual ~KDirectDraw(void)
	{
		Discharge();
	}

	void SetClientRect(HWND hWnd);

	virtual HRESULT SetupDirectDraw(HWND hTop, HWND hWnd, int nBufferCount=0,
						bool bFullScreen = false, int width=0, int height=0, int bpp=0);
};

//////////////////////////////////

RGNDATA * GetClipRegionData(HRGN hRgn);
HRGN      GetClipRegion(IDirectDrawClipper * pClipper);
BOOL      SetClipRegion(IDirectDrawClipper * pClipper, HRGN hRgn);

class KRgnClipper
{
	IDirectDrawClipper  * m_pNew;
	IDirectDrawClipper  * m_pOld;
	IDirectDrawSurface7 * m_pSrf;

public:
	KRgnClipper(IDirectDraw7 * pDD, IDirectDrawSurface7 * pSrf, HRGN hRgn)
	{
		pDD->CreateClipper(0, & m_pNew, NULL); // create new clipper

		SetClipRegion(m_pNew, hRgn);           // set clip list from region

		m_pSrf = pSrf;
		pSrf->GetClipper(& m_pOld);            // get old clipper
		pSrf->SetClipper(m_pNew);              // replace with new clipper
	}
	
	~KRgnClipper()
	{
		m_pSrf->SetClipper(m_pOld);	           // restore old clipper
		m_pOld->Release();                     // release old clipper
		m_pNew->Release();                     // release new clipper
	}
};

//////////////////////////////////////////////////////

// Wrapper for IDirect3D7, IDirect3DDevice7

class KDirect3D : public KDirectDraw
{
protected:
	IDirect3D7       *	m_pD3D;
	IDirect3DDevice7 *  m_pD3DDevice;

	KOffScreenSurface	m_backsurface;
	KOffScreenSurface   m_zbuffer;
	bool				m_bReady;

	virtual HRESULT  Discharge(void);

	virtual HRESULT OnRender(void)
	{
		return S_OK;
	}

	virtual HRESULT OnInit(HINSTANCE hInst)
	{
		m_bReady = true;

		return S_OK;
	}

	virtual HRESULT OnDischarge(void)
	{
		m_bReady = false;
		return S_OK;
	}
	
public:

    KDirect3D(void);
	
	~KDirect3D(void)
	{
		Discharge();
	}

	virtual HRESULT SetupDirectDraw(HWND hWnd, HWND hTop, int nBufferCount=0,
				bool bFullScreen=false, int width=0, int height=0, int bpp=0);

	virtual HRESULT ShowFrame(HWND hWnd);
	virtual HRESULT RestoreSurfaces(void);

	virtual HRESULT Render(HWND hWnd);
	virtual HRESULT ReCreate(HINSTANCE hInst, HWND hTop, HWND hWnd);
	virtual HRESULT OnResize(HINSTANCE hInst, int width, int height, HWND hTop, HWND hWnd);
};
