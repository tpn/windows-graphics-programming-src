//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : status.cpp						                                     //
//  Description: Status window                                                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <assert.h>

#include "win.h"
#include "Status.h"

#pragma comment(lib, "comctl32.lib")


void KStatusWindow::Create(HWND hParent, UINT nControlID)
{
	m_ControlID = nControlID;
	m_hWnd      = CreateStatusWindow(WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP,
									 NULL, hParent, nControlID);
}


void KStatusWindow::Resize(HWND hParent, int width, int height)
{
	RECT Parent;
	RECT Self;
	int  parts[3];

	GetClientRect(hParent, & Parent);
	GetClientRect(m_hWnd,  & Self);

	// (width-60)/2, (width-60)/2, 40, 20
	parts[0] = (width - 60) / 2;
	parts[1] = width - 60;
	parts[2] = width - 20;
	SendMessage(m_hWnd, SB_SETPARTS, 3, (LPARAM) parts);

	MoveWindow(m_hWnd, 0, Parent.bottom-Self.bottom, width, height, TRUE);
}


void KStatusWindow::SetText(int pane, HINSTANCE hInst, int messid, int param)
{
	TCHAR pattern[MAX_PATH], message[MAX_PATH];

	LoadString(hInst, messid, pattern, sizeof(pattern));
	wsprintf(message, pattern, param);

	SendMessage(m_hWnd, SB_SETTEXT, pane, (LPARAM) message);
}


void KStatusWindow::SetText(int pane, LPCTSTR message)
{
	SendMessage(m_hWnd, SB_SETTEXT, pane, (LPARAM) message);
}
