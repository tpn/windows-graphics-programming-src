#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dcdtable.h					                                         //
//  Description: Decode GDI handle table paer                                        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include "property.h"
#include "listview.h"
#include "gditable.h"

class KDecodeTablePage : public KPropertySheetPage
{
	// overwriting
	BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	KListView m_Objects;

	void TestnCount(bool testdelete);
	void TestnCount(int obj, HDC hDC1, HDC hDC2, bool testdelete);
	void Query(void);
    void DumpGDITable(void);
	void TestpUserSolidBrush(void);
	void TestpUserRectRegion(void);

	void ShowDetails(unsigned index);
	void Modify(unsigned index);

	KGDITable m_GDITable;

public:

	HINSTANCE m_hInst;

	bool IsGDITableAccessible(void) const
	{
		return m_GDITable.IsGDITableAccessible();
	}

	KDecodeTablePage();
};
