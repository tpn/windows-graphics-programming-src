//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : progress.cpp						                                 //
//  Description: Progress dialog box                                                 //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <assert.h>

#include "Winpp.h"
#include "Progress.h"

void KProgress::SetRange(unsigned low, unsigned up)
{
	pos			= low;
	reportedpos = low;
	total		= up - low + 1;

	SendDlgItemMessage(m_hWnd, nIdc, PBM_SETRANGE,
			low, MAKELPARAM(0, up));

	SendDlgItemMessage(m_hWnd, nIdc, PBM_SETPOS, low, 0);
}


void KProgress::Move(void)
{
	pos ++;

    char temp[32];
    wsprintf(temp, "%d", pos);
    SetDlgItemText(m_hWnd, nIdcNumber, temp);

	if ( ((pos-reportedpos)>total/32) || (pos>=(total-1)) )
	{
		reportedpos = pos;

    //    char temp[32];
    //    wsprintf(temp, "%d", pos);
    //    SetDlgItemText(m_hWnd, nIdcNumber, temp);

		SendDlgItemMessage(m_hWnd, nIdc, PBM_SETPOS, pos, 0);
	}
}


BOOL KProgress::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			m_hWnd = hWnd;
			ShowWindow(hWnd, SW_NORMAL);
			SetFocus(hWnd);
			UpdateWindow(hWnd);
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam)==IDCANCEL)
			{
				SetAbort();
				return TRUE;
			}
			break;
	}

	return FALSE;
}


void KProgress::Create(HINSTANCE hInst, HWND hWnd, int idd, int idc, int idcnumber, int delay)
{
	bAbort     = FALSE;
	nDelay     = delay;
    nIdc       = idc;
    nIdcNumber = idcnumber;

	Createdialog(hInst, idd, hWnd);
}


void KProgress::Destroy(void)
{
	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}


BOOL KProgress::AbortDraw(void)
{
	MSG msg;

	if (!m_hWnd)
		return FALSE;

	while ( ! bAbort && PeekMessage(&msg, m_hWnd, 0, 0, PM_REMOVE) )
	{
		if ( !IsDialogMessage(m_hWnd, &msg) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return bAbort;
}
