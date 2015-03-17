#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : toolbar.h				                                             //
//  Description: Toolbar wrapper                                                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include <commctrl.h>

// Toolbar window
class KToolbar
{
protected:
	HWND		m_hToolTip;
	UINT		m_ControlID;
	
	HINSTANCE   m_ResInstance;
	UINT		m_ResId;
public:

	HWND		m_hWnd;

	KToolbar()
	{
		m_hWnd		= NULL;
		m_hToolTip  = NULL;
		m_ControlID = 0;

		m_ResInstance = NULL;
		m_ResId       = 0;
	}

	void Create(HWND hParent, HINSTANCE hInstance, UINT nControlID, const TBBUTTON * pButtons, int nCount);
	void Resize(HWND hParent, int width, int height);
};

