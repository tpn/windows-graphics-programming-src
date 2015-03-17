#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : pagecancas.h					                                     //
//  Description: Multiple page, device independent layout                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include "win.h"
#include "canvas.h"
#include "scrollcanvas.h"

// 1440 = LCM(72, 96, 120, 360)
// 7200 = LCM(72, 96, 120, 360, 600)

#ifdef NT_ONLY
typedef enum { ONEINCH = 7200 };
#else
typedef enum { ONEINCH = 1440 };
#endif

class KSurface
{
public:
	typedef enum { 	BASE_DPI   = ONEINCH,
					MARGIN_X   = 16,
					MARGIN_Y   = 16
				};

	KOutputSetup m_OutputSetup;

	int	  m_nSurfaceWidth;   // total surface width  in pixel
	int   m_nSurfaceHeight;  // total surface height in pixel

	SIZE  m_Paper;		// in BASE_DPI
	RECT  m_Margin;		// in BASE_DPI
	RECT  m_MinMargin;	// in BASE_DPI

	int   m_nZoom;		// 100 for 1:1
	int	  m_nScale;		// GetDeviceCaps(hDC, LOGPIXELSX) * zoom / 100
	
	int px(int x, int col) // from base_dpi to screen
	{
		return ( x + m_Paper.cx * col ) * m_nScale / BASE_DPI + MARGIN_X * (col+1);
	}

	int py(int y, int row) // from base_dpi to screen
	{
		return ( y + m_Paper.cy * row ) * m_nScale / BASE_DPI + MARGIN_Y * (row+1);
	}

public:

	int GetDrawableWidth(void)
	{
		return m_Paper.cx - m_Margin.left - m_Margin.right;
	}
	
	int GetDrawableHeight(void) 
	{
		return m_Paper.cy - m_Margin.top  - m_Margin.bottom;
	}
	
	virtual int GetColumnCount(void)
	{
		return 1;
	}

	virtual int  GetPageCount(void)
	{
		return 1;	// single page
	}

	virtual const TCHAR * GetDocumentName(void)
	{
		return _T("KSurface - Document");
	}

	virtual void DrawPaper(HDC hDC, const RECT * rcPaint, int col, int row);
	virtual void CalculateSize(void);
	virtual void SetPaper(void);
	virtual void RefreshPaper(void);

	virtual void UponDrawPage(HDC hDC, const RECT * rcPaint, int width, int height, int pageno=1);
	virtual bool UponSetZoom(int zoom);
	virtual void UponInitialize(HWND hWnd);
	virtual void UponDraw(HDC hDC, const RECT * rcPaint);
	virtual bool UponPrint(HDC hDC, const TCHAR * pDocName);
	virtual bool UponFilePrint(void);
	virtual bool UponFilePageSetup(void);
};


///////////////////////////////////////////////////////////


class KPageCanvas : public KScrollCanvas, public KSurface
{
public:
	typedef enum { View_NoChange = 0, View_Redraw = 1, View_Resize = 2 };

protected:
	HMENU		m_hViewMenu;
	int			m_nMenuID;
	HBRUSH		m_hBackground;
	
	int			m_nPixelWidth;
	int			m_nPixelHeight;

	virtual void OnDraw(HDC hdc, const RECT * rcPaint)
	{
		UponDraw(hdc, rcPaint);
	}

	void GetDimension(void) // synchronize with KSurface
	{
		m_nPixelWidth  = m_nSurfaceWidth;
		m_nPixelHeight = m_nSurfaceHeight;
	}

	int SetZoom(int zoom)
	{
		if ( UponSetZoom(zoom) )
		{
			GetDimension();
			return View_Resize;
		}
		else
			return View_NoChange;
	}

public:

	virtual const TCHAR * GetClassName(void) const
	{
		return _T("KPageCanvasCanvas");
	}

	int GetPixelWidth(void) const
	{
		return m_nPixelWidth;
	}

	int GetPixelHeight(void) const
	{
		return m_nPixelHeight;
	}

	virtual int  OnCommand(int cmd, HWND hWnd) = 0;
	virtual void OnCreate(void);	
	
	virtual int  OnKey(int vkey)
	{
		return View_NoChange;
	}

	void Response(int rslt);

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void GetWndClassEx(WNDCLASSEX & wc);
	
public:

	bool Initialize(HINSTANCE hInstance, KStatusWindow * pStatus, int menuid);

	KPageCanvas(HBRUSH hBackground = NULL);

	virtual ~KPageCanvas()
	{
	}
};
