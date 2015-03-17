//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : ddbasic.cpp					                                     //
//  Description: Basic DirectDraw demo, Chapter 18                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>

#define INITGUID
#include <ddraw.h>
#include <d3d.h>

#include "..\..\include\win.h"
#include "..\..\include\ddsurf.h"
#include "..\..\include\ddwrap.h"

#include "resource.h"

BOOL PixelFillRect(KDDSurface & surface, int x, int y, int width, int height, DWORD dwColor[], int nColor)
{
	BYTE * pSurface = surface.LockSurface(NULL);
		
	const DDSURFACEDESC2 * pDesc = surface.GetSurfaceDesc();

	if ( pSurface )
	{
		int pitch   = surface.GetPitch();
		int byt     = pDesc->ddpfPixelFormat.dwRGBBitCount / 8; 
		
		for (int j=0; j<height; j++)
		{
			BYTE * pS   = pSurface + (y+j) * pitch + x * byt;
			DWORD color = dwColor[j % nColor];

			int i;

			switch ( byt )
			{
				case 1:
					memset(pS, color, width);
					break;

				case 2:
					for (i=0; i<width; i++)
					{
						* (unsigned short *) pS = (unsigned short) color;
						pS += sizeof(unsigned short);
					}
					break;

				case 3:
					for (i=0; i<width; i++)
					{
						* (RGBTRIPLE *) pS = * (RGBTRIPLE *) & color;
						pS += sizeof(RGBTRIPLE);
					}
					break;

				case 4:
					for (i=0; i<width; i++)
					{
						* (unsigned *) pS = color;
						pS += sizeof(unsigned);
					}
					break;
				
				default:
					return FALSE;
			}
		}

		surface.Unlock();
		return TRUE;
	}
	else
		return FALSE;
}


class KDDWin : public KWindow, public KDirectDraw
{
	HINSTANCE m_hInst;

	void OnNCPaint(void)
	{
		RECT rect;
		GetWindowRect(m_hWnd, & rect);

		DWORD dwColor[18];
		
		for (int i=0; i<18; i++)
			dwColor[i] = m_primary.ColorMatch(0, 0, 0x80 + abs(i-9)*12);

		PixelFillRect(m_primary, rect.left+24, rect.top+4, rect.right - 88 - rect.left, 18, dwColor, 18);

		BYTE * pSurface = m_primary.LockSurface(NULL); // just for address
		m_primary.Unlock(NULL);
	
		if ( SUCCEEDED(m_primary.GetDC()) )
		{
			TCHAR temp[MAX_PATH];
			const DDSURFACEDESC2 * pDesc = m_primary.GetSurfaceDesc();

			if ( pDesc )
				wsprintf(temp, "%dx%d %d-bpp, pitch=%d, lpSurface=0x%x", 
					pDesc->dwWidth, pDesc->dwHeight, pDesc->ddpfPixelFormat.dwRGBBitCount,
					pDesc->lPitch, pSurface);
			else
				strcpy(temp, "LockSurface failed");

			SetBkMode(m_primary, TRANSPARENT);
			SetTextColor(m_primary, RGB(0xFF, 0xFF, 0));
			TextOut(m_primary, rect.left+24, rect.top+4, temp, _tcslen(temp));
			
			m_primary.ReleaseDC();
		}
	}

    void OnDraw(void)
    {
		SetClientRect(m_hWnd);
		int n = min(m_rcDest.right-m_rcDest.left, m_rcDest.bottom-m_rcDest.top)/2;
		
		for (int i=0; i<n; i++)
		{
			DWORD color = m_primary.ColorMatch( 0xFF*(n-1-i)/(n-1), 0xFF*(n-1-i)/(n-1), 0xFF*i/(n-1) );

			m_primary.FillColor(m_rcDest.left+i, m_rcDest.top+i, m_rcDest.right-i, m_rcDest.bottom-i, color);
		}
    }

	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch( uMsg )
		{
			case WM_CREATE:
				m_hWnd = hWnd;
				if ( FAILED(SetupDirectDraw(GetParent(hWnd), hWnd, false)) ) // initialize DirectDraw
				{
					MessageBox(NULL, _T("Unable to Initialize DirectDraw"), _T("KDDWin"), MB_OK);
					CloseWindow(hWnd);
				}
				return 0;

			case WM_PAINT:
				OnDraw();
				ValidateRect(hWnd, NULL);
				return 0;

			case WM_NCPAINT:
				DefWindowProc(hWnd, uMsg, wParam, lParam);
				OnNCPaint();
				return 0;

			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;

			default:
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KWindow::GetWndClassEx(wc);
	
		wc.style |= (CS_HREDRAW | CS_VREDRAW);
		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH));
	}

public:
	KDDWin(HINSTANCE hInst)
	{
		m_hInst = hInst;
	}
};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KDDWin main(hInst);
	
	main.CreateEx(0, _T("ClassName"), _T("Basic DirectDraw"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, NULL, hInst);

    main.ShowWindow(nShow);
    main.UpdateWindow();
    main.MessageLoop();

	return 0;
}
