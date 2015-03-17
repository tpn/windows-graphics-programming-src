//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : gditable.cpp					                                     //
//  Description: Accesing GDI handle table                                           //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <assert.h>
#include <tchar.h>

#include "gditable.h"

 
KGDITable::KGDITable()
{
	typedef unsigned (CALLBACK * Proc0) (void);

    HMODULE hGDI = GetModuleHandle(_T("GDI32.dll"));
    assert(hGDI);

	Proc0 pGdiQueryTable = (Proc0) GetProcAddress(hGDI, "GdiQueryTable");
    assert(pGdiQueryTable);

	if ( pGdiQueryTable )
		pGDITable = (GDITableCell *) pGdiQueryTable();
	else
	{
		pGDITable = NULL;
	
		MessageBox(NULL, "Unable to locate handle table", "KGDITable", MB_OK | MB_ICONSTOP);
		PostQuitMessage(-1);
	}

	DWORD dwVersion = GetVersion();

    if (dwVersion < 0x80000000)                // Windows NT/2000
        m_nVersion = LOBYTE(LOWORD(dwVersion));
	else
		m_nVersion = 0;
}
