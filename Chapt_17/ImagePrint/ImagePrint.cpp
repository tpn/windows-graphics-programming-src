//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : imageprint.cpp					                                     //
//  Description: JPEG image displaying and printing                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define NOCRYPT

#pragma pack(push, 4)
#include <windows.h>
#pragma pack(pop)

#include <assert.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>

#include "..\..\include\win.h"
#include "..\..\include\Toolbar.h"
#include "..\..\include\Status.h"
#include "..\..\include\FrameWnd.h"
#include "..\..\include\filedialog.h"
#include "..\..\include\outputsetup.h"
#include "..\..\include\pagecanvas.h"

#include "..\..\include\jpeg.h"

#include "resource.h"
#include "imageview.h"

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
	void CreateMDIChild(KCanvas * canvas, const TCHAR * klass, const TCHAR * title)
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
		mdic.lParam  = (LPARAM) canvas;

		HWND hWnd = (HWND) SendMessage(m_hMDIClient, WM_MDICREATE, 0, (LPARAM) & mdic);
	}

	BOOL CreatePageCanvas(const TCHAR * Title, KPicture * pPicture)
	{
		KPageCanvas  * pCanvas;
		
		pCanvas = new KImageCanvas(pPicture, GetSysColorBrush(COLOR_BTNSHADOW));

		if ( pCanvas )
		{
			TCHAR FullTitle[MAX_PATH];

			wsprintf(FullTitle, "%s, %d by %d, 24-bit", Title, pPicture->GetWidth(), 
				pPicture->GetHeight());

			if ( pCanvas->Initialize(m_hInst, m_pStatus, IDR_DEMO) )
			{
				CreateMDIChild(pCanvas, pCanvas->GetClassName(), FullTitle);
				return TRUE;
			}

			delete pCanvas;
		}

		return FALSE;
	}

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam)
	{
		switch ( LOWORD(wParam) )
		{
			case IDM_FILE_OPEN:
			{
				KFileDialog fd;

				if ( ! fd.GetOpenFileName(m_hWnd, "jpg", "JPEG Images") )
					return TRUE;

				KPicture * pPicture = new KPicture;
				
				if ( pPicture->LoadJPEGFile(fd.m_TitleName) )
					CreatePageCanvas(fd.m_TitleName, pPicture);
				else
					delete pPicture;
			}
			return TRUE;
		}

		return FALSE;
	}

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return KMDIFrame::WndProc(hWnd, uMsg, wParam, lParam);
	}

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KMDIFrame::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_PRINT));
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
	
	frame.CreateEx(0, _T("ClassName"), _T("ImagePrint"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	    NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);

    frame.ShowWindow(nShow);
    frame.UpdateWindow();

    frame.MessageLoop();

	return 0;
}
