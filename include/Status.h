#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : status.h						                                     //
//  Description: Status window wrapper                                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

typedef enum
{
	pane_1,
	pane_2,
	pane_3
};


// Status window
class KStatusWindow
{
public:
	HWND m_hWnd;
	UINT m_ControlID;

	KStatusWindow()
	{
		m_hWnd		= NULL;
		m_ControlID = 0;
	}

	void Create(HWND hParent, UINT nControlID);
	void Resize(HWND hParent, int width, int height);
	void SetText(int pane, HINSTANCE hInst, int messid, int param=0);
	void SetText(int pane, LPCTSTR message);
};
