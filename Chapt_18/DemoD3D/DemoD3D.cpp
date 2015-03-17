//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : demod3d.cpp					                                     //
//  Description: Direct3D IM demo program, Chapter 18                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>

#define D3D_OVERLOADS
#include <math.h>
#include <d3d.h>
#include <d3dxmath.h>

#include "..\..\include\win.h"
#include "..\..\include\basicdib.h"
#include "..\..\include\ddsurf.h"
#include "..\..\include\ddwrap.h"

#include "resource.h"

class KDirect3DDemo : public KDirect3D
{
	KOffScreenSurface m_texture[4];

public:
	HRESULT OnRender(void);
	HRESULT OnInit(HINSTANCE hInst);
	HRESULT OnDischarge(void);
};


HRESULT DrawTriangle(IDirect3DDevice7 * pDevice, 
				 int x0, int y0, int z0, 
				 int x1, int y1, int z1, 
				 int x2, int y2, int z2)
{
	D3DVERTEX vertices[3];

	D3DVECTOR p1( (float)x0, (float)y0, (float)z0 );
	D3DVECTOR p2( (float)x1, (float)y1, (float)z1 );
	D3DVECTOR p3( (float)x2, (float)y2, (float)z2 );

	D3DVECTOR vNormal = Normalize(CrossProduct(p1-p2, p2-p3));
	
	// Initialize the 3 vertices for the front of the triangle
	vertices[0] = D3DVERTEX( p1, vNormal, 0.5f, 0.0f );
	vertices[1] = D3DVERTEX( p2, vNormal, 1.0f, 1.0f );
	vertices[2] = D3DVERTEX( p3, vNormal, 0.0f, 1.0f );

	return pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_VERTEX, vertices, 3, NULL);
}


HRESULT KDirect3DDemo::OnInit(HINSTANCE hInst)
{
    D3DMATERIAL7 mtrl;
    memset(&mtrl, 0, sizeof(mtrl));
    mtrl.ambient.r = 1.0f;
    mtrl.ambient.g = 1.0f;
    mtrl.ambient.b = 1.0f;
    m_pD3DDevice->SetMaterial( &mtrl );

	m_pD3DDevice->SetRenderState( D3DRENDERSTATE_AMBIENT, RGBA_MAKE(255, 255, 255, 0) );

    D3DMATRIX mat;
	memset(& mat, 0, sizeof(mat));
	mat._11 = mat._22 = mat._33 = mat._44 = 1.0f;
	
	// view matrix, on z-axis by 10 units.
	D3DMATRIX matView = mat;
	matView._43 = 10.0f;
    m_pD3DDevice->SetTransform( D3DTRANSFORMSTATE_VIEW, &matView );

	mat._11 =  2.0f;
	mat._22 =  2.0f;
	mat._34 =  1.0f;
	mat._43 = -0.1f;
	mat._44 =  0.0f;
    m_pD3DDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &mat);

	// enable Z-buffer
    m_pD3DDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, TRUE);

	for (int i=0; i<4; i++)
	{
		const int nResID[] = { IDB_TIGER, IDB_PANDA, IDB_WHALE, IDB_ELEPHANT };
	
		BITMAPINFO * pDIB = LoadBMP(hInst, MAKEINTRESOURCE(nResID[i]));
	
		if ( pDIB )
			m_texture[i].CreateTextureSurface(m_pD3DDevice, m_pDD, pDIB);
		else
			return E_FAIL;
	}

	m_bReady = true;

	return S_OK;
}


HRESULT KDirect3DDemo::OnDischarge(void)
{
	m_bReady = false;

	for (int i=0; i<4; i++)
		m_texture[i].Discharge();

	return S_OK;
}

HRESULT KDirect3DDemo::OnRender(void)
{
	double time = GetTickCount() / 2000.0;

    m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, RGBA_MAKE(0, 0, 0xff, 0), 1.0f, 0);

    if ( FAILED( m_pD3DDevice->BeginScene() ) )
		return E_FAIL;

	D3DMATRIX   matLocal;
	memset(& matLocal, 0, sizeof(matLocal));
	matLocal._11 = matLocal._33 = (FLOAT) cos( time );
	matLocal._13 = matLocal._31 = (FLOAT) sin( time );
	matLocal._22 = matLocal._44 = 1.0f;
	m_pD3DDevice->SetTransform( D3DTRANSFORMSTATE_WORLD, &matLocal );
		
	m_pD3DDevice->SetTexture( 0, m_texture[0] );
	DrawTriangle(m_pD3DDevice, 0, 3, 0,  3, -3,  0,  0, -3,  3);
	
	m_pD3DDevice->SetTexture( 0, m_texture[1] );
	DrawTriangle(m_pD3DDevice, 0, 3, 0,  0, -3, -3,  3, -3,  0);
    	
	m_pD3DDevice->SetTexture( 0, m_texture[2] );
	DrawTriangle(m_pD3DDevice, 0, 3, 0,  -3, -3,  0,  0, -3, -3);

	m_pD3DDevice->SetTexture( 0, m_texture[3] );
	DrawTriangle(m_pD3DDevice, 0, 3, 0,  0,  -3,  3,  -3, -3, 0);
	
	m_pD3DDevice->EndScene();

    return S_OK;
}


class KD3DWin : public KWindow, public KDirect3DDemo
{
	bool	  m_bActive;
	HINSTANCE m_hInst;

	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
	    switch( uMsg )
	    {
			case WM_CREATE:	
				m_hWnd    = hWnd;
				m_bActive = false;

				if ( FAILED( ReCreate(m_hInst, hWnd, hWnd) ) )
				{
					MyMessageBox(NULL, _T("Unable to initialize Direct 3D IM"), _T("DemoD3D"), MB_OK, IDI_GRAPH);

					CloseWindow(hWnd);
					PostQuitMessage(-1);
				}
				else
					SetTimer(hWnd, 101, 1, NULL);
				return 0;

			case WM_PAINT:
				ShowFrame(hWnd);
				break;

			case WM_SIZE:
				m_bActive = (SIZE_MAXHIDE!=wParam) && (SIZE_MINIMIZED!=wParam);

	            if ( m_bActive && FAILED(OnResize(m_hInst, LOWORD(lParam), HIWORD(lParam), hWnd, hWnd)) )
					CloseWindow(hWnd);
				break;

			case WM_TIMER:
				if ( m_bActive )
					Render(hWnd);
				return 0;

			case WM_DESTROY:
				KillTimer(hWnd, 101);
				Discharge();
				PostQuitMessage(0);
				return 0L;
		}

		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KWindow::GetWndClassEx(wc);
		wc.style |= (CS_HREDRAW | CS_VREDRAW);
		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH));
	}

public:

	KD3DWin(HINSTANCE hInst)
	{
		m_hInst = hInst;
	}
};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KD3DWin main(hInst);
	
	main.CreateEx(0, _T("ClassName"), _T("Basic Direct3D"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, NULL, hInst);

    main.ShowWindow(nShow);
    main.UpdateWindow();
	
	return main.MessageLoop();
}