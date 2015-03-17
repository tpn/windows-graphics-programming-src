//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : main.cpp					                                         //
//  Description: GDI Handles main program, Chapter 3                                 //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT

#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <commctrl.h>

#include "gditable.h"

#include "resource.h"
#include "creator.h"
#include "snapshot.h"
#include "handles.h"
#include "findtab.h"
#include "dcdtable.h"


class KMain : public KPropertySheet
{
	KHGDIOBJ            m_HGDIOBJ;
	KLocateGdiTablePage m_FindTable;
	KDecodeTablePage    m_DecodeTable;

public: 

	void Run(HINSTANCE hInst)
	{
		if ( ! m_DecodeTable.IsGDITableAccessible() )
			return;

        HPROPSHEETPAGE hPage[3];

		m_FindTable.Initialize(hInst);
		m_DecodeTable.m_hInst = hInst;

		hPage[0] = m_HGDIOBJ.createPropertySheetPage(hInst, IDD_DECODEHANDLE);
        hPage[1] = m_FindTable.createPropertySheetPage(hInst, IDD_LOCATETABLE);
		hPage[2] = m_DecodeTable.createPropertySheetPage(hInst, IDD_DECODETABLE);

		TCHAR Title[MAX_PATH];
        wsprintf(Title, "GDI Handles - (process 0x%X)", GetCurrentProcessId());
        
		propertySheet(hInst, NULL, IDI_GDIHANDLES, 3, hPage, Title);
	}

};

void Test(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	KMain    main;

	GetStockObject(BLACK_PEN);

    Test();

	InitCommonControls();


	main.Run(hInstance);

	return TRUE;
}	
