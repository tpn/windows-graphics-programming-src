//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : colorspace.cpp					                                     //
//  Description: Color space demo program, Chapter 7                                 //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0500

#include <windows.h> 
#include <assert.h>
#include <tchar.h>

#include "..\..\include\win.h"
#include "..\..\include\Canvas.h"
#include "..\..\include\Status.h"
#include "..\..\include\FrameWnd.h"
#include "..\..\include\Affine.h"
#include "..\..\include\Color.h"
#include "..\..\include\guires.h"

#include "Resource.h"

class KMyCanvas : public KCanvas
{
	virtual void    OnDraw(HDC hDC, const RECT * rcPaint);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	int				m_colorspace;
	HDC				m_hDC;

public:

	BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	KMyCanvas()
	{
		m_colorspace = IDM_COLORSPACE_HLS;
		m_hDC		 = NULL;
	}
};


BOOL KMyCanvas::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch ( LOWORD(wParam) )
	{
		case IDM_COLORSPACE_RGB:
		case IDM_COLORSPACE_RGB2HLS:
		case IDM_COLORSPACE_HLS:
		case IDM_COLORSPACE_DEFPAL:
		case IDM_COLORSPACE_HFTPAL:
			m_colorspace = LOWORD(wParam);
			InvalidateRect(m_hWnd, NULL, TRUE);
			::UpdateWindow(m_hWnd);
			return TRUE;

		case IDM_FILE_EXIT:
			DestroyWindow(GetParent(m_hWnd));
			return TRUE;
	}

	return FALSE;	// not processed
}


LRESULT KMyCanvas::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr;

	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd = hWnd;
			m_hDC  = GetDC(m_hWnd);
			return 0;

		case WM_MOUSEMOVE:
			{
				COLORREF color = GetPixel(m_hDC, LOWORD(lParam), HIWORD(lParam));

				TCHAR temp[32];

				wsprintf(temp, "%d RGB(%d,%d,%d)", color>>24, GetRValue(color), GetGValue(color), GetBValue(color));

				m_pStatus->SetText(0, temp);
			}
			return 0;

		case WM_PAINT:
			{
				KGUIResource res;

				PAINTSTRUCT ps; 

				HDC hDC = BeginPaint(m_hWnd, &ps);

				OnDraw(hDC, &ps.rcPaint);

				EndPaint(m_hWnd, &ps);
			}
			return 0;

		default:
			lr = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return lr;
}


void ColorRect(HDC hDC, COLORREF c0, COLORREF dx, COLORREF dy, bool drawline=false)
{
	for (int x=0; x<256; x++)
	for (int y=0; y<256; y++)
		SetPixelV(hDC, x, y, c0 | (x * dx) | (y * dy));

	if ( drawline )
	{
		MoveToEx(hDC, 0, 0, NULL); LineTo(hDC, 0, 255); LineTo(hDC, 255, 255);
		MoveToEx(hDC, 0, 0, NULL); LineTo(hDC, 255, 0); LineTo(hDC, 255, 255);
//		LineTo(hDC, 0, 0);
	}
}

void RGBCube2HLSHexagon(HDC hDC)
{
	KAffine affine;
	SetGraphicsMode(hDC, GM_ADVANCED);

	FLOAT r = 254;					 // even number < 255
	FLOAT x = r / 2;                 // cos(60);
	FLOAT y = (FLOAT) (r * 1.732/2); // sin(60);
				
	SetViewportOrgEx(hDC, (int)r + 40, (int)y + 40, NULL); // hexagon center

	// Red = 255
	affine.MapTri(0,0, 255,0, 0,255, r,0, x,y,   x,-y);
	SetWorldTransform(hDC, & affine.m_xm);
	ColorRect(hDC, PALETTERGB(0xFF, 0, 0), RGB(0, 1, 0), RGB(0, 0, 1), true);

	// Green = 255
	affine.MapTri(0,0, 255,0, 0,255, -x,y, x,y,   -r,0);
	SetWorldTransform(hDC, & affine.m_xm);
	ColorRect(hDC, PALETTERGB(0, 0xFF, 0), RGB(1, 0, 0), RGB(0, 0, 1), true);

	// Blue = 255
	affine.MapTri(0,0, 255,0, 0,255, -x,-y, -r,0,  x,-y);
	SetWorldTransform(hDC, & affine.m_xm);
	ColorRect(hDC, PALETTERGB(0, 0, 0xFF), RGB(0, 1, 0), RGB(1, 0, 0), true);
}


void RGBCube(HDC hDC)
{
	int r, g, b;

	MoveToEx(hDC, -20, -20, NULL);	
	LineTo(hDC, 128, 128);			
	LineTo(hDC, 128+256+30, 128);
	MoveToEx(hDC, 128, 128, NULL);
	LineTo(hDC, 128, 128+256+30);
	TextOut(hDC, -20, -20, "Red COLORREF(255,0,0)", 3+18);
	TextOut(hDC, 128+256+30, 128, "Green COLORREF(0,255,0)", 5+18);
	TextOut(hDC, 128, 128+256+30, "Blue COLORREF(0,0,255)", 4+18);

	// Red = 255
	ColorRect(hDC, PALETTERGB(0xFF, 0, 0), RGB(0, 1, 0), RGB(0, 0, 1));

	// Blue = 255
	for (g=0; g<256; g++)
	for (r=0; r<256; r+=2)
	{
		COLORREF cr = SetPixel(hDC, g+128-r/2, 255+128-r/2, PALETTERGB(r, g, 0xFF));

//		int x = g + 128-r/2; 
//		int y = 255+128-r/2;

//		SetPixel(hDC, x+300, y, GetPixel(hDC, x, y));
	}

	// Green = 255
	for (b=0; b<256; b++)
	for (r=0; r<256; r+=2)
		SetPixelV(hDC, 255+128-r/2, b+128-r/2, PALETTERGB(r, 0xFF, b));
}


void RGBCube_Affine(HDC hDC)
{
	KAffine affine;
	SetGraphicsMode(hDC, GM_ADVANCED);

	MoveToEx(hDC, -20, -20, NULL);	
	LineTo(hDC, 128, 128);			
	LineTo(hDC, 128+256+30, 128);
	MoveToEx(hDC, 128, 128, NULL);
	LineTo(hDC, 128, 128+256+30);
	TextOut(hDC, -20, -20, "Red COLORREF(255,0,0)", 3+18);
	TextOut(hDC, 128+256+30, 128, "Green COLORREF(0,255,0)", 5+18);
	TextOut(hDC, 128, 128+256+30, "Blue COLORREF(0,0,255)", 4+18);

	// Red = 255
	ColorRect(hDC, PALETTERGB(0xFF, 0, 0), RGB(0, 1, 0), RGB(0, 0, 1));

	// Blue = 255
	affine.MapTri(0,       0,       255,     0,       0, 255, 
		          128,     255+128, 255+128, 255+128, 0, 255 );
	SetWorldTransform(hDC, & affine.m_xm);
	ColorRect(hDC, PALETTERGB(0, 0, 0xFF), RGB(0, 1, 0), RGB(1, 0, 0));

	// Green = 255
	affine.MapTri(0,       0,   255, 0, 0, 255,
		          255+128, 128, 255, 0, 255+128, 255+128);
	SetWorldTransform(hDC, & affine.m_xm);

	ColorRect(hDC, PALETTERGB(0, 0xFF, 0), RGB(1, 0, 0), RGB(0, 0, 1));
}


void HLSColorPalette(HDC hDC, int scale, KColor & selection)
{
	KColor c;

	for (int hue=0; hue<360; hue++)
	for (int sat=0; sat<=scale; sat++)
	{
		c.hue        = hue;
		c.lightness  = 0.5 ; 
		c.saturation = ((double) sat)/scale;
		c.ToRGB();

		SetPixelV(hDC, hue, sat, PALETTERGB(c.red, c.green, c.blue));

	//	SetPixel(hDC, hue+400, sat, PALETTERGB(0,0,0) | GetPixel(hDC, hue, sat));
	}

	for (int l=0; l<=scale; l++)
	{
		c = selection;
		c.lightness = ((double)l)/scale;
		c.ToRGB();

//		char temp[32];
//		wsprintf(temp, "%d %d %d\n", c.red, c.green, c.blue);
//		OutputDebugString(temp);

		for (int x=0; x<64; x++)
			SetPixelV(hDC, scale+20+x, l, PALETTERGB(c.red, c.green, c.blue));
	}
}


void ShowHalftonePalette(HDC hDC, bool bHalftone)
{
	HPALETTE hPalette = (HPALETTE) GetCurrentObject(hDC, OBJ_PAL);
	
	if ( bHalftone )
		hPalette = CreateHalftonePalette(hDC);

	PALETTEENTRY entry[256];

	int num = GetPaletteEntries(hPalette, 0, 256, entry);

	HPALETTE hOld = SelectPalette(hDC, hPalette, FALSE);

	if ( GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE )
		RealizePalette(hDC);

	for (int j=0; j<(num+15)/16; j++)
	for (int i=0; i<16; i++)
	{
		for (int y=0; y<24; y++)
		for (int x=0; x<24; x++)
			SetPixelV(hDC, i*25+x, j*25+y, PALETTEINDEX(j*16+i));
	}

	SelectPalette(hDC, hOld, FALSE);

	if ( bHalftone )
		DeleteObject(hPalette);
}


void KMyCanvas::OnDraw(HDC hDC, const RECT * rcPaint)
{
	RECT rect;
	int  rslt;

	GetClientRect(m_hWnd, & rect);

	rslt = SetMapMode(hDC, MM_ANISOTROPIC);
	rslt = SetWindowExtEx(hDC, 1, 1, NULL);
	rslt = SetViewportExtEx(hDC, 1, -1, NULL);
	SetViewportOrgEx(hDC, 40, rect.bottom-40, NULL);
	// origin at bottom, left corner

	switch ( m_colorspace )
	{
		case IDM_COLORSPACE_RGB:
			{
				HPALETTE hPalette = CreateHalftonePalette(hDC);
				HPALETTE hOld     = SelectPalette(hDC, hPalette, FALSE);
				RealizePalette(hDC);

				RGBCube(hDC);

				SelectObject(hDC, hOld);
				DeleteObject(hPalette);
			}
			break;

		case IDM_COLORSPACE_RGB2HLS:
			{
				HPALETTE hPalette = CreateHalftonePalette(hDC);
				HPALETTE hOld     = SelectPalette(hDC, hPalette, FALSE);
				RealizePalette(hDC);

				RGBCube2HLSHexagon(hDC);

				SelectObject(hDC, hOld);
				DeleteObject(hPalette);
			}
			break;

		case IDM_COLORSPACE_HLS:
			{
				KColor color;

				color.red   = 56;
				color.green = 75;
				color.blue  = 167;
				color.ToHLS();

				HPALETTE hPalette = CreateHalftonePalette(hDC);
				HPALETTE hOld     = SelectPalette(hDC, hPalette, FALSE);
				RealizePalette(hDC);

				HLSColorPalette(hDC, 360, color);

				SelectObject(hDC, hOld);
				DeleteObject(hPalette);
			}
			break;

		case IDM_COLORSPACE_DEFPAL:
		case IDM_COLORSPACE_HFTPAL:
			ShowHalftonePalette(hDC, m_colorspace==IDM_COLORSPACE_HFTPAL);
			break;
	}
}


class KLogoFrame : public KFrame
{
	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KFrame::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH));
	}

public:
	KLogoFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
		KToolbar * pToolbar, KCanvas * pCanvas, KStatusWindow * pStatus) :
			KFrame(hInstance, pButtons, nCount, pToolbar, pCanvas, pStatus)
	{
	}

};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KMyCanvas     canvas;
	KStatusWindow status;

	KLogoFrame frame(hInst, NULL, 0, NULL, & canvas, & status);

	frame.CreateEx(0, _T("ClassName"), _T("ColorSpace"),
		WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}
