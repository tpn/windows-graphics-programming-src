//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : toolbarb.cpp					                                     //
//  Description: Toolbar with bitmap switching                                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <commctrl.h>

#include "toolbar.h"
#include "toolbarb.h"
#include "PalLoadBitmap.h"

BOOL KToolbarB::SetBitmap(HINSTANCE hInstance, int resourceID)
{
	HPALETTE hPal = CreateSystemPalette();
	HBITMAP  hBmp = PaletteLoadBitmap(hInstance, MAKEINTRESOURCE(resourceID), hPal);
	DeleteObject(hPal);

	if ( hBmp )
	{
		TBREPLACEBITMAP rp;

		rp.hInstOld = m_ResInstance;
		rp.nIDOld   = m_ResId;
		rp.hInstNew = NULL;
		rp.nIDNew   = (UINT) hBmp;
		rp.nButtons = 40;

		SendMessage(m_hWnd, TB_REPLACEBITMAP, 0, (LPARAM) & rp);

		if ( m_ResInstance==NULL )
			DeleteObject( (HBITMAP) m_ResId);

		m_ResInstance = NULL;
		m_ResId       = (UINT) hBmp;

		return TRUE;
	}
	else
		return FALSE;
}

