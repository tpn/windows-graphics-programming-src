//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : devcaps.cpp					                                     //
//  Description: KDeviceCaps class                                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define NOCRYPT
#define WINVER       0x0500

#include <windows.h>
#include <tchar.h>

#include "resource.h"

#include "..\\..\\include\\property.h"
#include "..\\..\\include\\listview.h"

#include "DevCaps.h"


BOOL KDeviceCaps::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uMsg )
	{
		case WM_INITDIALOG:
			m_List.FromDlgItem(hWnd, IDC_CAPS);

			return OnInitDialog();

		case WM_COMMAND:
			if ( LOWORD(wParam)==IDOK )
				EndDialog(hWnd, TRUE);

			return TRUE;
	}
		
	return FALSE;
}


void KDeviceCaps::OneCap(int index, const TCHAR * name, const TCHAR * format)
{
	TCHAR mess[64];

	wsprintf(mess, format, GetDeviceCaps(m_hDC, index));

	m_List.AddItem(0, name);
	m_List.AddItem(1, mess);
}


BOOL KDeviceCaps::OnInitDialog(void)
{
	m_List.AddColumn(0, 120, _T("Index"));
	m_List.AddColumn(1, 160, _T("Value"));

	OneCap(TECHNOLOGY,		_T("TECHNOLOGY"),		_T("%d"));

	OneCap(DRIVERVERSION,	_T("DRIVERVERSION"),	_T("0x%x"));
	
	OneCap(HORZSIZE,		_T("HORZSIZE"),			_T("%d mm"));
	OneCap(VERTSIZE,		_T("VERTSIZE"),			_T("%d mm"));
	
	OneCap(HORZRES,			_T("HORZRES"),			_T("%d pixels"));
	OneCap(VERTRES,			_T("VERTRES"),			_T("%d pixels"));

	OneCap(LOGPIXELSX,		_T("LOGPIXELSX"),		_T("%d dpi"));
	OneCap(LOGPIXELSY,		_T("LOGPIXELSY"),		_T("%d dpi"));

	OneCap(BITSPIXEL,		_T("BITSPIXEL"),		_T("%d bits"));
	OneCap(PLANES,          _T("PLANES"),			_T("%d planes"));
	OneCap(NUMBRUSHES,      _T("NUMBRUSHES"),		_T("%d"));
	OneCap(NUMPENS,         _T("NUMPENS"),			_T("%d"));
	OneCap(NUMMARKERS,      _T("NUMMARKERS"),		_T("%d"));
	OneCap(NUMFONTS,        _T("NUMFONTS"),			_T("%d"));
	OneCap(NUMCOLORS,       _T("NUMCOLORS"),		_T("%d"));

	OneCap(PDEVICESIZE,     _T("PDEVICESIZE"),		_T("%d"));
	OneCap(CURVECAPS,       _T("CURVECAPS"),		_T("0x%x"));
	OneCap(LINECAPS,        _T("LINECAPS"),			_T("0x%x"));

	OneCap(POLYGONALCAPS,   _T("POLYGONALCAPS"),	_T("0x%x"));
	OneCap(TEXTCAPS,        _T("TEXTCAPS"),			_T("0x%x"));
	OneCap(CLIPCAPS,		_T("CLIPCAPS"),			_T("0x%x"));
	OneCap(RASTERCAPS,		_T("RASTERCAPS"),		_T("0x%x"));

	OneCap(ASPECTX,			_T("ASPECTX"),			_T("%d"));
	OneCap(ASPECTY,			_T("ASPECTY"),			_T("%d"));
	OneCap(ASPECTXY,		_T("APSECTXY"),			_T("%d"));

	OneCap(SIZEPALETTE,		_T("SIZEPALETTE"),		_T("%d"));
	OneCap(NUMRESERVED,     _T("NUMRESERVED"),		_T("%d"));
	OneCap(COLORRES,		_T("COLORRES"),			_T("%d"));

	OneCap(PHYSICALWIDTH,   _T("PHYSICALWIDTH"),	_T("%d"));
	OneCap(PHYSICALHEIGHT,  _T("PHYSICALHEIGHT"),	_T("%d"));
	OneCap(PHYSICALOFFSETX, _T("PHYSICALOFFSETX"),	_T("%d"));
	OneCap(PHYSICALOFFSETY, _T("PHYSICALOFFSETY"),	_T("%d"));
	OneCap(SCALINGFACTORX,  _T("SCALINGFACTORX"),	_T("%d %"));
	OneCap(SCALINGFACTORY,  _T("SCALINGFACTORY"),	_T("%d %"));

	OneCap(VREFRESH,        _T("VREFRESH"),			_T("%d hz"));
	OneCap(DESKTOPVERTRES,  _T("DESKTOPVERTRES"),	_T("%d pixels"));
	OneCap(DESKTOPHORZRES,  _T("DESKTOPHORZRES"),	_T("%d pixels"));
	OneCap(BLTALIGNMENT,    _T("BLTALIGNMENT"),		_T("%d pixels"));
	OneCap(SHADEBLENDCAPS,  _T("SHADEBLENDCAPS"),	_T("0x%x"));
	OneCap(COLORMGMTCAPS,   _T("COLORMGMTCAPS"),	_T("0x%x"));

	return TRUE;
}

