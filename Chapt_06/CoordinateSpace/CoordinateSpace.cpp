//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : coordinatespace.cpp			                                     //
//  Description: Coordinate space demo program, Chapter 6                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "..\..\include\Affine.h"
#include "..\..\include\win.h"
#include "..\..\include\Canvas.h"
#include "..\..\include\FrameWnd.h"
#include "..\..\include\Status.h"

#include "resource.h"


class KFixedString
{
	int	  m_len;

public:

	TCHAR m_buffer[MAX_PATH];

	KFixedString(void)
	{
		m_buffer[0] = 0;
		m_len       = 0;
	}

	void Add(TCHAR ch)
	{
		m_buffer[m_len++] = ch;
		m_buffer[m_len  ] = 0;
	}

	void Add(PCTSTR pStr)
	{
		_tcscat(m_buffer + m_len, pStr);

		m_len += _tcslen(pStr);
	}

	void Add(int i)
	{
		wsprintf(m_buffer + m_len, "%d ", i);
		m_len = _tcslen(m_buffer);
	}

	void Add(int x, int y)
	{
		wsprintf(m_buffer + m_len, "(%d, %d)", x, y);

		m_len = _tcslen(m_buffer);
	}
};


void DumpMapMode(HDC hDC)
{
	POINT p;
	SIZE  s;
	KFixedString str;

	str.Add( GetMapMode(hDC) );

	str.Add( GetDeviceCaps(hDC, HORZSIZE) );
	str.Add( GetDeviceCaps(hDC, VERTSIZE) );
	
	str.Add( GetDeviceCaps(hDC, HORZRES) );
	str.Add( GetDeviceCaps(hDC, VERTRES) );
	str.Add( GetDeviceCaps(hDC, LOGPIXELSX) );
	str.Add( GetDeviceCaps(hDC, LOGPIXELSY) );

	GetWindowOrgEx(hDC, & p); str.Add(p.x, p.y);
	GetWindowExtEx(hDC, & s); str.Add(s.cx, s.cy);

	GetViewportOrgEx(hDC, & p); str.Add(p.x, p.y);
	GetViewportExtEx(hDC, & s); str.Add(s.cx, s.cy);

	MessageBox(NULL, str.m_buffer, "Map mode", MB_OK);
}


void SetEnglish(HDC hDC)
{
	SetWindowOrgEx(hDC, 5, 5, NULL);
	SetViewportOrgEx(hDC, 10, 10, NULL);

	SetMapMode(hDC, MM_TEXT);
	DumpMapMode(hDC);
	
	SetMapMode(hDC, MM_ISOTROPIC);
	DumpMapMode(hDC);
	
	SetWindowExtEx(hDC, 3, 5, NULL);

	DumpMapMode(hDC);

	SetViewportExtEx(hDC, 5, 3, NULL);

	DumpMapMode(hDC);

	SetMapMode(hDC, MM_ANISOTROPIC);
	BOOL rslt = SetViewportExtEx(hDC, 1, 1, NULL);
	
	DumpMapMode(hDC);

	rslt = SetViewportExtEx(hDC, 0, 0, NULL);
	
	DumpMapMode(hDC);

}


// Demonstrate world transformation using dotted-lines
void Transform_DottedLine(HDC hDC, int width, int height)
{
	KAffine af;

	SetMapMode(hDC, MM_ANISOTROPIC);
	SetWindowExtEx  (hDC, 1, 1,				   NULL); // default
	SetWindowOrgEx  (hDC, 0, 0,				   NULL);
	
	SetViewportExtEx(hDC, 1, -1,			   NULL); // y axis goes up
	SetViewportOrgEx(hDC, width/2, height*3/4, NULL); // 1/2, 3/4

	SetGraphicsMode(hDC, GM_ADVANCED);				  // using world coordinate space	

	for (int i=0; i<=72*5; i++)
	{
		SetWorldTransform(hDC, & af.m_xm);		// set transformation 

		for (int x=0; x<=200; x+=3)				// dotted line
			SetPixel(hDC, x+50, 0, 0);

		af.Translate(5, 5);						// translate

		af.Scale((FLOAT) 0.98, (FLOAT) 0.98);	// scale 0.98
		af.Rotate(5);							// rotate
	}
}


// size x size square in logical coordinate space
void Face(HDC hDC, COLORREF color, int size)
{
	for (int x=0; x<size; x++)
	for (int y=0; y<size; y++)
		SetPixel(hDC, x, y, color);
}


// use three transformations to map square in logical coordinate space to faces of a 3D cube
void Draw_Cube(HDC hDC, int width, int height, int degree)
{
	KAffine af;

	SetMapMode(hDC, MM_ANISOTROPIC);
	SetWindowExtEx  (hDC, 1, 1,				   NULL); // default
	SetWindowOrgEx  (hDC, 0, 0,				   NULL);
	
	SetViewportExtEx(hDC, 1, -1,			   NULL); // y axis goes up
	SetViewportOrgEx(hDC, width/2, height*3/4, NULL); // 1/2, 3/4

	SetGraphicsMode(hDC, GM_ADVANCED);

	const float pi   = (float) 3.1415926;
	const int   size = 150;
	const float dx   = (float) (size * cos(pi*degree/180));
	const float dy   = (float) (size * sin(pi*degree/180));
	
	af.MapTri(0, 0, (float)size, 0, 0, (float)size,   0, 0, dx, dy, 0, (float)size);
	SetWorldTransform(hDC, & af.m_xm);

	Face(hDC, RGB(0xFF, 0, 0), size);

	af.MapTri(0, 0, (float)size, 0, 0, (float)size,   0, 0, -dx, dy, 0, (float)size);
	SetWorldTransform(hDC, & af.m_xm);
	Face(hDC, RGB(0, 0xFF, 0), size);

	af.MapTri(0, 0, (float)size, 0, 0, (float)size,   0, (float)size, dx, size + dy, -dx, size + dy);
	SetWorldTransform(hDC, & af.m_xm);
	Face(hDC, RGB(0, 0, 0xFF), size);
}	



class KRotate
{
	int m_x0, m_y0, m_dx, m_dy;

public:

	int m_ds;
	
	KRotate(int x0, int y0, int x1, int y1)
	{
		m_x0 = x0;
		m_y0 = y0;
		m_dx = x1 - x0;
		m_dy = y1 - y0;
		m_ds = (int) sqrt( m_dx * m_dx + m_dy * m_dy);
	}

	int RotateX(int x, int y)
	{
		return x * m_dx / m_ds - y * m_dy / m_ds;
	}

	int RotateY(int x, int y)
	{
		return x * m_dy / m_ds + y * m_dx / m_ds;
	}
};


// Draw a line with an arrow	
void Arrow(HDC hDC, int x0, int y0, int x1, int y1, int width, int height)
{
	MoveToEx(hDC, x0, y0, NULL);
	LineTo(hDC, x1, y1);

	KRotate rotate(x0, y0, x1, y1);

	POINT arrow[4] = 
	{ 
		x1, y1, 
		
		rotate.RotateX(rotate.m_ds-width,  height) + x0, 
		rotate.RotateY(rotate.m_ds-width,  height) + y0,
		
		rotate.RotateX(rotate.m_ds-width, -height) + x0, 
		rotate.RotateY(rotate.m_ds-width, -height) + y0,
		
		x1, y1
	};

	HGDIOBJ hOld = SelectObject(hDC, GetStockObject(BLACK_BRUSH));
	Polygon(hDC, arrow, 4);
	SelectObject(hDC, hOld);
}


// Convert size in points to device coordinate
void PointToDevice(HDC hDC, SIZE & size)
{
	int dpix = GetDeviceCaps(hDC, LOGPIXELSX);
	int dpiy = GetDeviceCaps(hDC, LOGPIXELSY);

	size.cx = size.cx * dpix / 72;
    size.cy = size.cy * dpiy / 72;
}


// Convert size in device coordinate to logical coordinate
void DeviceToLogical(HDC hDC, SIZE & size)
{
	POINT p[2] = { 0, 0, size.cx, size.cy };

	DPtoLP(hDC, p, 2);

	size.cx = abs(p[1].x - p[0].x);
	size.cy = abs(p[1].y - p[0].y);
}


// Generic algorithm for axes display: width height in device coordinate
void ShowAxes(HDC hDC, int width, int height)
{
	POINT corner[2];

	// display axes for the area [10, 10, width-10, height-10]
	corner[0].x = 10;
	corner[0].y = 10;
	corner[1].x = width - 10;
	corner[1].y = height - 10;

	// covert to logical coordinate
	DPtoLP(hDC, corner, 2);

	// we need to display a 10x2 point arrow, convert to device coordinate, then to logical
	SIZE s = { 10, 2 };
	PointToDevice(hDC, s);
	DeviceToLogical(hDC, s);

	// X-axis
	Arrow(hDC, min(corner[0].x, corner[1].x), 0, 
		       max(corner[0].x, corner[1].x), 0, s.cx, s.cy);

	// Y-axis
	Arrow(hDC, 0, min(corner[0].y, corner[1].y), 
		       0, max(corner[0].y, corner[1].y), s.cx, s.cy);
}


// Decode world transformation and mapping mode
void DecodeTransform(HDC hDC)
{
	POINT windoworg;   GetWindowOrgEx(hDC, & windoworg);
	SIZE  windowext;   GetWindowExtEx(hDC, & windowext);

	POINT viewportorg; GetViewportOrgEx(hDC, & viewportorg);
	SIZE  viewportext; GetViewportExtEx(hDC, & viewportext);

	TCHAR formulax[128];
	TCHAR formulay[128];

	wsprintf(formulax, "x' = (x-%d) %d / %d + %d", windoworg.x, viewportext.cx, windowext.cx, viewportorg.x);
	wsprintf(formulay, "y' = (y-%d) %d / %d + %d", windoworg.y, viewportext.cy, windowext.cy, viewportorg.y);

	POINT p[2] = { 10, 10, 10, 35 };
	DPtoLP(hDC, p, 2);

	TextOut(hDC, p[0].x, p[0].y, formulax, _tcslen(formulax));
	TextOut(hDC, p[1].x, p[1].y, formulay, _tcslen(formulay));
}


// a scene (screen, page) with drawing and message handling
class KScene
{
public:
	virtual  ~KScene()
	{
	}

	virtual void          OnDraw(HDC hDC)                         = 0;
	virtual void          OnMouseMove(int x, int y, TCHAR mess[]) = 0;
	virtual const TCHAR * GetTitle(void)                          = 0;
};


// Mapping mode demostration
class KMapMode : public KScene
{
	HWND m_hWnd;
	int  m_width;
	int  m_height;
	int  m_mapmode;
	int  m_dpix;
	int  m_dpiy;

	// setup a mapping mode with origin in the center
	void Setup(HDC hDC)
	{
		RECT rect;

		GetClientRect(m_hWnd, & rect);
		m_width  = rect.right;
		m_height = rect.bottom;

		SetMapMode(hDC, m_mapmode);

		if ( m_dpix )
		{
			SetViewportExtEx(hDC, GetDeviceCaps(hDC, LOGPIXELSX), GetDeviceCaps(hDC, LOGPIXELSY), NULL);
			  SetWindowExtEx(hDC, m_dpix,                         m_dpiy,						  NULL);
		}

		SetWindowOrgEx(hDC,   0,         0,          NULL);
		SetViewportOrgEx(hDC, m_width/2, m_height/2, NULL);
	}

	// draw axes
	void OnDraw(HDC hDC)
	{
		Setup(hDC);
		
		ShowAxes(hDC, m_width, m_height);
		DecodeTransform(hDC);

		SelectObject(hDC, GetStockObject(NULL_BRUSH));
		Ellipse(hDC, -100, -100, 100, 100);
	}

	// display device coordinate and logical coordinate
	void OnMouseMove(int x, int y, TCHAR mess[])
	{
		POINT p = { x, y };

		wsprintf(mess, _T("Client: %d x %d"), p.x, p.y);

		HDC hDC = GetDC(m_hWnd);

		Setup(hDC);
		DPtoLP(hDC, & p, 1);
		wsprintf(mess + _tcslen(mess), ", Logical: %d x %d", p.x, p.y);

		ReleaseDC(m_hWnd, hDC);
	}

public:
	KMapMode(HWND hWnd, int mapmode, int dpix=0, int dpiy=0)
	{
		m_hWnd    = hWnd;
		m_mapmode = mapmode;
		m_dpix    = dpix;
		m_dpiy    = dpiy;
	}

	const TCHAR * GetTitle(void)
	{
		switch ( m_mapmode )
		{
			case MM_TEXT	   : return _T("MappingMode: MM_TEXT");
			case MM_LOENGLISH  : return _T("MappingMode: MM_LOENGLISH");
			case MM_HIENGLISH  : return _T("MappingMode: MM_HIENGLISH");
			case MM_LOMETRIC   : return _T("MappingMode: MM_LOMETRIC");
			case MM_HIMETRIC   : return _T("MappingMode: MM_HIMETRIC");
			case MM_TWIPS      : return _T("MappingMode: MM_TWIPS");
			case MM_ISOTROPIC  : return _T("MappingMode: MM_ISOTROPIC");
			case MM_ANISOTROPIC: return _T("MappingMode: MM_ANISOTROPIC");
		}

		return NULL;
	}

};



class KMyCanvas : public KCanvas
{
	int      mode;
	KScene * scene;

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		memset(& wc, 0, sizeof(wc));

		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = CS_HREDRAW | CS_VREDRAW;  // repaint when resize
		wc.lpfnWndProc   = WindowProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;       
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = NULL;
		wc.hIconSm       = NULL;
	}

public:
	KMyCanvas()
	{
		mode  = 0;
		scene = NULL;
	}

	virtual void OnDraw(HDC hDC, const RECT * rcPaint)
	{
		if ( scene )
			scene->OnDraw(hDC);
		else
		{
			RECT rect;

			GetClientRect(m_hWnd, & rect);
	
			switch (mode )
			{
				case IDM_VIEW_DOTLINE:
					Transform_DottedLine(hDC, rect.right, rect.bottom);
					break;

				case IDM_VIEW_CUBE30:
					Draw_Cube(hDC, rect.right, rect.bottom, 30);
					break;

				case IDM_VIEW_CUBE45:
					Draw_Cube(hDC, rect.right, rect.bottom, 45);
					break;
			}
		}
	}

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam)
	{
		if ( mode != LOWORD(wParam) )
		{
			mode = LOWORD(wParam);
		
			if ( scene )
			{
				delete scene;
				scene = NULL;
			}

			switch ( mode )
			{
				case IDM_VIEW_MMTEXT     :   scene = new KMapMode(m_hWnd, MM_TEXT);        break;
				case IDM_VIEW_MMLOENGLISH:   scene = new KMapMode(m_hWnd, MM_LOENGLISH);   break;
				case IDM_VIEW_MMHIENGLISH:   scene = new KMapMode(m_hWnd, MM_HIENGLISH);   break;
				case IDM_VIEW_MMLOMETRIC:    scene = new KMapMode(m_hWnd, MM_LOMETRIC);    break;
				case IDM_VIEW_MMHIMETRIC:    scene = new KMapMode(m_hWnd, MM_HIMETRIC);    break;
				case IDM_VIEW_MMTWIPS:	     scene = new KMapMode(m_hWnd, MM_TWIPS);       break;
				case IDM_VIEW_MMISOTROPIC:   scene = new KMapMode(m_hWnd, MM_ISOTROPIC, 300, 300);   break;
				case IDM_VIEW_MMANISOTROPIC: scene = new KMapMode(m_hWnd, MM_ANISOTROPIC, 600, 300); break;
			}

			if ( scene )
				m_pStatus->SetText(0, scene->GetTitle());
			
			InvalidateRect(m_hWnd, NULL, TRUE);
		}
		
		return TRUE;
	}

	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if ( uMsg==WM_MOUSEMOVE )
		{
			TCHAR temp[64];

			temp[0] = 0;
			if ( scene )
				scene->OnMouseMove(LOWORD(lParam), HIWORD(lParam), temp);

			if ( temp[0] )
				m_pStatus->SetText(1, temp);

			return 0;
		}
		else
			return KCanvas::WndProc(hWnd, uMsg, wParam, lParam);
	}
};

class KMyFrame : public KFrame
{
	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KFrame::GetWndClassEx(wc);
		wc.hIcon  = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH));
	}

public:
	KMyFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
			KToolbar * pToolbar, KCanvas * pCanvas, KStatusWindow * pStatus) :
		KFrame(hInstance, pButtons, nCount, pToolbar, pCanvas, pStatus)
	{
	}

};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KMyCanvas		canvas;
	KStatusWindow   status;

	KMyFrame		frame(hInst, NULL, 0, NULL, & canvas, & status);
	
	frame.CreateEx(0, _T("CoorinateSpace"), _T("Coordinate Space"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();
    frame.MessageLoop();

	return 0;
}