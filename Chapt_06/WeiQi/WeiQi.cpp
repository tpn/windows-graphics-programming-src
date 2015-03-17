//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : weiqi.cpp						                                     //
//  Description: Sample SDI program, scrolling, zooming, Chapter 6                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>

#include "..\..\include\win.h"
#include "..\..\include\Status.h"
#include "..\..\include\Canvas.h"
#include "..\..\include\ScrollCanvas.h"
#include "..\..\include\FrameWnd.h"

#include "resource.h"


class KWeiQiBoard : public KScrollCanvas
{
	virtual void    OnDraw(HDC hDC, const RECT * rcPaint);
	virtual BOOL    OnCommand(WPARAM wParam, LPARAM lParam);

	int    m_grids ;
	int    m_unitsize ;
	char * m_stones;

	int margin(void) const
	{
		return m_unitsize * m_zoommul / m_zoomdiv * 3 /2;
	}

	int pos(int n) const
	{
		return n * m_unitsize * m_zoommul / m_zoomdiv + margin();
	}

	void ResetGrid(int grid)
	{
		m_grids = grid;
		
		SetSize(pos(m_grids-1)+margin(), pos(m_grids-1)+margin(), m_unitsize, m_unitsize);
		InvalidateRect(m_hWnd, NULL, TRUE);
	}

public:

	KWeiQiBoard()
	{
		m_grids    = 19;
		m_unitsize = 20;
		m_stones   = "B20212223241404W3031323334251505";
		
		SetSize(pos(m_grids-1)+margin(), pos(m_grids-1)+margin(), m_unitsize, m_unitsize);
	}
};


void KWeiQiBoard::OnDraw(HDC hDC, const RECT * rcPaint)
{
#ifdef TEST_BRUSHORG
	
	SetBkMode(hDC, OPAQUE);
	SetBkColor(hDC, RGB(0xFF, 0xFF, 0));

	HBRUSH hWood = CreateHatchBrush(HS_DIAGCROSS, RGB(0, 0, 0xFF)); // PALETTERGB(193, 146, 57));
	
	POINT P = { 0, 0 };
	LPtoDP(hDC, &P, 1);

	SetBrushOrgEx(hDC, P.x, P.y, NULL);

	char temp[32];
	wsprintf(temp, "(%d, %d)", P.x, P.y);
	m_pStatus->SetText(0, temp);

#else

	HBRUSH hWood = CreateSolidBrush(PALETTERGB(193, 146, 57));

#endif

	SelectObject(hDC, hWood);
	PatBlt(hDC, pos(-1), pos(-1), pos(m_grids)-pos(-1), pos(m_grids)-pos(-1), PATCOPY);
	SelectObject(hDC, GetStockObject(BLACK_BRUSH));
	DeleteObject(hWood);

	for (int i=0; i<m_grids; i++)
	{
		MoveToEx(hDC, pos(0), pos(i), NULL); LineTo(hDC, pos(m_grids-1), pos(i));
		MoveToEx(hDC, pos(i), pos(0), NULL); LineTo(hDC, pos(i),         pos(m_grids-1));
	}

	if ( m_grids!=9 )
	{
		int starsize = 3 * m_zoommul / m_zoomdiv;

		for (int y=3; y<m_grids; y+=6 )
		for (int x=3; x<m_grids; x+=6 )
			Ellipse(hDC, pos(x)-starsize, pos(y)-starsize, pos(x)+starsize+1, pos(y)+starsize+1);
	}

	char * stone = m_stones;

	while ( * stone )
	{
		if ( * stone=='B' )
			SelectObject(hDC, GetStockObject(BLACK_BRUSH));
		else if ( * stone=='W' )
			SelectObject(hDC, GetStockObject(WHITE_BRUSH));
		else
		{
			int y = m_grids - 1 - ( stone[0] - '0' );
			int x = m_grids - 1 - ( stone[1] - '0' );
			stone ++;

			int size = 9 * m_zoommul / m_zoomdiv;
			
			Ellipse(hDC, pos(x)-size, pos(y)-size, pos(x)+size+1, pos(y)+size+1);
		}

		stone ++;
	}
}


BOOL KWeiQiBoard::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch ( LOWORD(wParam) )
	{
		case IDM_FILE_EXIT:  
			DestroyWindow(GetParent(m_hWnd));  
			return TRUE;
		
		case IDM_BOARD_9:    
			ResetGrid(9); 
			return TRUE;

		case IDM_BOARD_13:
			ResetGrid(13);
			return TRUE;

		case IDM_BOARD_19:
			ResetGrid(19);
			return TRUE;
	}

	return FALSE;
}


class KMyFrame : public KFrame
{
	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KFrame::GetWndClassEx(wc);
		wc.hIcon  = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_COLIVE));
	}

public:
	KMyFrame(HINSTANCE hInstance, const TBBUTTON * pButtons, int nCount,
			KToolbar * pToolbar, KCanvas * pCanvas, KStatusWindow * pStatus) :
		KFrame(hInstance, pButtons, nCount, pToolbar, pCanvas, pStatus)
	{
	}

};


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmd, int nShow)
{
	KWeiQiBoard   canvas;
	KStatusWindow status;
	KMyFrame      frame(hInst, NULL, 0, NULL, & canvas, & status);

	frame.CreateEx(0, _T("Go"), _T("WeiQi-Go-Baduk"),
		WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();
    frame.MessageLoop();

	return 0;
}
