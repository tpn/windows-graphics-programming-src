//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : demodd.cpp						                                     //
//  Description: DirectDraw demo program, Chapter 18                                 //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500

#pragma pack(push, 4)
#include <windows.h>
#pragma pack(pop)

#define INITGUID

#include <ddraw.h>
#include <d3d.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>
#include <ddkernel.h>
#include <dvp.h>

#include "..\..\include\win.h"
#include "..\..\include\dialog.h"
#include "..\..\include\Status.h"
#include "..\..\include\Toolbar.h"
#include "..\..\include\logwindow.h"

#include "..\..\include\FrameWnd.h"
#include "..\..\include\mdichild.h"

#include "..\..\include\basicdib.h"
#include "..\..\include\ddsurf.h"
#include "..\..\include\ddwrap.h"

#include "resource.h"

void DumpSurface(KDDSurface & surface)
{
	const DDSURFACEDESC2 * desc = surface.GetSurfaceDesc();

	if ( desc )
	{
	}
}

////////////////////////////////////////////////////////////////
class KLineDemo : public KMDIChild, public KDirectDraw
{
	int m_nMode;

	void RegionDemo(void)
	{
		HRGN hRgn = CreateEllipticRgnIndirect(& m_rcDest);

		if ( hRgn )
		{
			m_primary.FillRgn(hRgn, m_primary.ColorMatch(0xFF, 0xFF, 0));
			DeleteObject(hRgn);
		}
	}

	void PixelDemo(void)
	{
		KLockedSurface frame;
	
		if ( ! frame.Initialize(m_primary) )
			return;

		for (int i=0; i<4096; i++)
			frame.SetPixel(
				m_rcDest.left + rand() % ( m_rcDest.right  - m_rcDest.left),
				m_rcDest.top  + rand() % ( m_rcDest.bottom - m_rcDest.top),
				m_primary.ColorMatch(rand() % 256, rand() % 256, rand() % 256));
				
		m_primary.Unlock();
	}

	void LineDemo(KDDSurface & surface, int x, int y, int Radius)
	{
		const int    N      = 19;
		const double theta  = 3.1415926 * 2 / N;
	
		// simple line drawing
		const COLORREF color[10] = { 
			RGB(0, 0, 0), RGB(255,0,0), RGB(0,255,0), RGB(0,0, 255),
			RGB(255,255,0), RGB(0, 255, 255), RGB(255, 255, 0),
			RGB(127, 255, 0), RGB(0, 127, 255), RGB(255, 0, 127)
		};
	
		DWORD dwColor[10];
		for (int i=0; i<10; i++)
			dwColor[i] = surface.ColorMatch(GetRValue(color[i]), GetGValue(color[i]), GetBValue(color[i]));

		KLockedSurface frame;
	
		if ( frame.Initialize(m_primary) )
		{
			for (int p=0; p<N; p++)
			for (int q=0; q<p; q++)
				frame.Line( (int)(x + Radius * sin(p * theta)), 
				            (int)(y + Radius * cos(p * theta)), 
						    (int)(x + Radius * sin(q * theta)), 
							(int)(y + Radius * cos(q * theta)),
							dwColor[min(p-q, N-p+q)]);

			m_primary.Unlock();
		}
	}

	void ClipDemo(void)
	{
		HRGN hUpdate = CreateRectRgn(0, 0, 1, 1);			
		GetUpdateRgn(m_hWnd, hUpdate, FALSE);				// update region
		OffsetRgn(hUpdate, m_rcDest.left, m_rcDest.top);	// screen coordinate

		HRGN hEllipse = CreateEllipticRgn(m_rcDest.left-20, m_rcDest.top-20, // big ellipse
							m_rcDest.right+20, m_rcDest.bottom+20);
		CombineRgn(hEllipse, hEllipse, hUpdate, RGN_AND);	// update region & ellipse
		DeleteObject(hUpdate);

		KRgnClipper clipper(m_pDD, m_primary, hEllipse);
		
		DeleteObject(hEllipse);

		m_primary.FillColor(m_rcDest.left-20, m_rcDest.top-20, 
							m_rcDest.right+20, m_rcDest.bottom+20,
							m_primary.ColorMatch(0, 0, 0xFF));
	}

    void OnDraw(void)
    {
		SetClientRect(m_hWnd);

		if ( (m_rcDest.left != m_rcDest.right) && (m_rcDest.top!=m_rcDest.bottom) )
			if ( m_nMode==0 )
			{
				RegionDemo();
	
				PixelDemo();
		
				LineDemo(m_primary,
					(m_rcDest.left+m_rcDest.right)/2, 
					(m_rcDest.top+m_rcDest.bottom)/2, 
					min(m_rcDest.right-m_rcDest.left, m_rcDest.bottom-m_rcDest.top)/2-10);
			}
			else
				ClipDemo();
    }

	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch( uMsg )
		{
			case WM_PAINT:
				OnDraw(); 
				ValidateRect(hWnd, NULL);
				return 0;

			case WM_CREATE:
				m_hWnd = hWnd;
				if ( FAILED(SetupDirectDraw(GetParent(hWnd), hWnd, false)) ) // initialize DirectDraw
				{
					MessageBox(NULL, _T("Unable to Initialize DirectDraw"), _T("KLineDemo"), MB_OK);
					CloseWindow(hWnd);
				}
				// continue

			default:
				return KMDIChild::WndProc(hWnd, uMsg, wParam, lParam);
		}
	}

public:
	KLineDemo(int mode)
	{
		m_nMode = mode;
	}
};


class KSpriteDemo : public KMDIChild, public KDirectDraw
{
	KOffScreenSurface m_background;
	POINT			  m_backpos;
	KOffScreenSurface m_sprite;
	POINT			  m_spritepos;
	KDDFont<128>	  m_font;

	HINSTANCE		  m_hInst;
	HWND			  m_hTop;

    void OnDraw(void);
	void OnCreate(void);

	void MoveSprite(int dx, int dy)
	{
		m_spritepos.x += dx * 5;
		m_spritepos.y += dy * 5;
		OnDraw();
	}
	
	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch( uMsg )
		{
			case WM_PAINT:
				OnDraw();
				ValidateRect(hWnd, NULL);
				return 0;

			case WM_KEYDOWN:
				switch ( wParam )
				{
					case VK_HOME : MoveSprite(-1, -1); break;
					case VK_UP   : MoveSprite( 0, -1); break;
					case VK_PRIOR: MoveSprite(+1, -1); break;

					case VK_LEFT : MoveSprite(-1,  0); break;
					case VK_RIGHT: MoveSprite( 1,  0); break;

					case VK_END  : MoveSprite(-1,  1); break;
					case VK_DOWN : MoveSprite( 0,  1); break;
					case VK_NEXT : MoveSprite( 1,  1); break;
				}
				return 0;

			case WM_CREATE:
				m_hWnd = hWnd;
				OnCreate();
				// fall through

			default:
				return KMDIChild::WndProc(hWnd, uMsg, wParam, lParam);
		}
	}

public:
	KSpriteDemo(HMODULE hModule, HWND hTop)
	{
		m_spritepos.x  = 0;
		m_spritepos.y  = 0;
		m_backpos.x    = 0;
		m_backpos.y    = 0;

		m_hInst = hModule;
		m_hTop  = hTop;
	}
};


void KSpriteDemo::OnCreate(void)
{
	if ( SUCCEEDED(SetupDirectDraw(m_hTop, m_hWnd, false)) ) // initialize DirectDraw
	{
		BITMAPINFO * pDIB = LoadBMP(m_hInst, MAKEINTRESOURCE(IDB_NIGHT));

		if ( pDIB )
			m_background.CreateBitmapSurface(m_pDD, pDIB);

		pDIB = LoadBMP(m_hInst, MAKEINTRESOURCE(IDB_PLANE));

		if ( pDIB )
		{
			m_sprite.CreateBitmapSurface(m_pDD, pDIB);
			m_sprite.SetSourceColorKey(0); // black
		}

		LOGFONT lf;

		memset(&lf, 0, sizeof(lf));
		lf.lfHeight   = - 36;
		lf.lfWeight   = FW_BOLD;
		lf.lfItalic   = TRUE;
		lf.lfQuality  = ANTIALIASED_QUALITY;
		_tcscpy(lf.lfFaceName, "Times New Roman");

		m_font.CreateFont(m_pDD, lf, ' ', 0x7F, RGB(0xFF, 0xFF, 0));
	}
	else
	{
		MessageBox(NULL, _T("Unable to Initialize DirectDraw"), _T("KSpriteDemo"), MB_OK);
		CloseWindow(m_hWnd);
	}
}

void KSpriteDemo::OnDraw(void)
{
	SetClientRect(m_hWnd);

	int dy = (m_rcDest.bottom - m_rcDest.top - m_background.GetHeight())/2;

	// draw area not covered by the background image
	if ( dy>0 )
	{
		DWORD color = m_primary.ColorMatch(0x80, 0x40, 0);

		m_primary.FillColor(m_rcDest.left, m_rcDest.top, m_rcDest.right, m_rcDest.top + dy, 
			color); // dark warm color

		color = m_primary.ColorMatch(0, 0x40, 0x80);
		m_primary.FillColor(m_rcDest.left, m_rcDest.bottom - dy-1, m_rcDest.right, m_rcDest.bottom, 
			color); // dark cool color
	}
	else
		dy = 0;

	// draw background image
		
	// if right edge of sprite is offscreen, move background to the left
	while ( (m_spritepos.x + m_sprite.GetWidth() + m_backpos.x) > (m_rcDest.right - m_rcDest.left) )
		m_backpos.x -= 100;

	// if left edge of sprite is offscreen, move background to the right
	while ( (m_spritepos.x + m_backpos.x) < 0 )
		m_backpos.x += 100;

	// make sure background position in within the right range
	m_backpos.x = max(m_backpos.x, m_rcDest.right - m_background.GetWidth() - m_rcDest.left);
	m_backpos.x = min(m_backpos.x, 0);

	m_primary.BitBlt(m_rcDest.left + m_backpos.x, 
					 m_rcDest.top  + m_backpos.y + dy, m_background);

	// draw sprite
	m_primary.BitBlt(m_rcDest.left + m_spritepos.x + m_backpos.x, 
					 m_rcDest.top  + m_spritepos.y + m_backpos.y, m_sprite, DDBLT_KEYSRC);

	m_font.TextOut(m_primary, m_rcDest.left+5, m_rcDest.top+1, "Hello, DirectDraw!");
}


void CheckInterface(KLogWindow * pLog, IUnknown * pInf, REFIID iid, const TCHAR * name)
{
	IUnknown * pNew;

	if ( SUCCEEDED(pInf->QueryInterface(iid, (void **) & pNew)) )
	{
		unsigned * pTable = * (unsigned * *) pNew;

		pLog->Log("Object at %x supports interface %s, at %x, vtable=%x\r\n", pInf, name, pNew, pTable);

		for (int i=0; i<40; i++)
		{
			void * p = (void *) pTable[i];
			
			pLog->Log("  vtable[%2d]= %x\r\n", i, p);

			if ( IsBadReadPtr(p, 4) || ! IsBadWritePtr(p, 4) )
				break;
		}

		pLog->Log("\r\n");
		pNew->Release();
	}
	else
		pLog->Log("Object at %x does not support interface %s\r\n", pInf, name);
}


void DemoCreateDirectDraw(KLogWindow * pLog)
{
	IDirectDraw7 * pDD = NULL;

    HRESULT hr = DirectDrawCreateEx(NULL, (void **) & pDD, IID_IDirectDraw7, NULL);

	if ( FAILED(hr) )
	{
		pLog->Log("DirectDrawCreateEx failed (%x)", hr);
		return;
	}

	CheckInterface(pLog, pDD, IID_IDirectDraw7, "IDirectDraw7");
	CheckInterface(pLog, pDD, IID_IDirectDraw4, "IDirectDraw4");
	CheckInterface(pLog, pDD, IID_IDirectDraw2, "IDirectDraw2");
	CheckInterface(pLog, pDD, IID_IDirectDraw,  "IDirectDraw" );
	CheckInterface(pLog, pDD, IID_IUnknown,     "IUnknown" );

	CheckInterface(pLog, pDD, IID_IDDVideoPortContainer,  "IDDVideoPortContainer" );
	CheckInterface(pLog, pDD, IID_IDirectDrawKernel,      "IID_IDirectDrawKernel" );

	CheckInterface(pLog, pDD, IID_IDirect3D7,   "IDirect3D7");
	CheckInterface(pLog, pDD, IID_IDirect3D3,   "IDirect3D3");
	
	pDD->Release();
	
}

void DemoCreateDirectDraw(HINSTANCE hInst)
{
	KLogWindow * pLog = new KLogWindow;

	pLog->Create(hInst, "CreateDirectDrawEx", LoadIcon(hInst, MAKEINTRESOURCE(IDI_GRAPH)));

	DemoCreateDirectDraw(pLog);
}

const int Translate[] =
{
	IDM_FILE_CLOSE,
	IDM_FILE_EXIT,
	IDM_WINDOW_TILE,
	IDM_WINDOW_CASCADE,
	IDM_WINDOW_ARRANGE,
	IDM_WINDOW_CLOSEALL
};


class KMyMDIFrame : public KMDIFrame
{
	HWND CreateMDIChild(KWindow * pWin, const TCHAR * klass, const TCHAR * title)
	{
		MDICREATESTRUCT mdic;

		mdic.szClass = klass;
		mdic.szTitle = title;
		mdic.hOwner  = m_hInst;
		mdic.x       = CW_USEDEFAULT;
		mdic.y       = CW_USEDEFAULT;
		mdic.cx      = CW_USEDEFAULT;
		mdic.cy      = CW_USEDEFAULT;
		mdic.style   = WS_VISIBLE | WS_BORDER;
		mdic.lParam  = (LPARAM) pWin;

		return (HWND) SendMessage(m_hMDIClient, WM_MDICREATE, 0, (LPARAM) & mdic);
	}

	BOOL CreateChild(const TCHAR * Title, KMDIChild * pChild)
	{
		if ( pChild )
		{
			if ( pChild->Initialize(m_hInst, IDR_DEMO) )
				return ( CreateMDIChild(pChild, pChild->GetClassName(), Title)!=NULL );

			delete pChild;
		}
		return FALSE;
	}

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam)
	{
		switch ( LOWORD(wParam) )
		{
			case IDM_FILE_PLA:
				CreateChild( _T("Pixels, Lines, and Areas"), new KLineDemo(0));
				return TRUE;

			case IDM_FILE_CLIP:
				CreateChild( _T("Region & Clipper"), new KLineDemo(1));
				return TRUE;

			case IDM_FILE_BITMAPTEXT:
				CreateChild( _T("Text, Bitmap and Sprite"), new KSpriteDemo(m_hInst, m_hWnd));
				return TRUE;

			case IDM_FILE_CREATEDIRECTDRAW:
				DemoCreateDirectDraw(m_hInst);
				return TRUE;
		}

		return FALSE;
	}

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KMDIFrame::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH));
	}

public:
	KMyMDIFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
		KToolbar * pToolbar, KStatusWindow * pStatus) :
		KMDIFrame(hInstance, pButtons, nCount, pToolbar, pStatus, Translate)
	{
	}
};


const TBBUTTON tbButtons[] =
{
	{ STD_FILENEW,	 IDM_FILE_NEW,   TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_FILENEW,  0 },
	{ STD_FILEOPEN,  IDM_FILE_OPEN,  TBSTATE_ENABLED, TBSTYLE_BUTTON, { 0, 0 }, IDS_FILEOPEN, 0 }
};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
	KToolbar      toolbar;
	KStatusWindow status;

	KMyMDIFrame frame(hInst, tbButtons, 2, & toolbar, & status);
	
	frame.CreateEx(0, _T("ClassName"), _T("DirectDraw Demo"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}
