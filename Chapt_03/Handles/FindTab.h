//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : findtabl.h					                                         //
//  Description: KLocateGdiTablePage class                                           //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//


#pragma once

#include "property.h"
#include "listview.h"
#include "snapshot.h"

class KLocateGdiTablePage: public KPropertySheetPage
{
	KListView m_Regions;
	HINSTANCE m_hInst;
	KSnapShot m_snapshot;
    
	BOOL      DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void      GdiQueryTable();

public:
    KLocateGdiTablePage();
	virtual ~KLocateGdiTablePage();

	void Initialize(HINSTANCE hInstance)
	{
		m_hInst = hInstance;
	}
};

