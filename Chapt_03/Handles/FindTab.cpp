//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : findtab.cpp					                                     //
//  Description: Locate GDI handle table, Chapter 3                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <stdio.h>

#include "pefile.h"

#include "resource.h"
#include "findTab.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KLocateGdiTablePage::KLocateGdiTablePage()
{

}

KLocateGdiTablePage::~KLocateGdiTablePage()
{

}


typedef unsigned (CALLBACK * Proc0) (void);


void KLocateGdiTablePage::GdiQueryTable(void)
{
    Proc0 p = (Proc0) GetProcAddress(GetModuleHandle("GDI32.DLL"), "GdiQueryTable");

    if (p)
    {
        TCHAR temp[32];

        wsprintf(temp, "%8lX", p());
        MessageBox(NULL, temp, "GdiQueryTable() returns", MB_OK);
    }
}


BOOL KLocateGdiTablePage::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
	    case WM_INITDIALOG:
			m_hWnd = hWnd;
            
			m_Regions.FromDlgItem(hWnd, IDC_REGIONS);
			
			m_Regions.AddIcon(LVSIL_SMALL, m_hInst, IDI_EMPTY);
            m_Regions.AddIcon(LVSIL_SMALL, m_hInst, IDI_EQUAL);
            m_Regions.AddIcon(LVSIL_SMALL, m_hInst, IDI_CHANGE);

			m_Regions.AddColumn(0, 62, "oldCRC");
			m_Regions.AddColumn(1, 46, "CRC");
                
            m_Regions.AddColumn(2, 76, "Base");
            m_Regions.AddColumn(3, 76, "Size");
            m_Regions.AddColumn(4, 60, "Type");
            m_Regions.AddColumn(5, 100, "Module");
			return TRUE;
		
		case WM_COMMAND:
			if (LOWORD(wParam) == IDC_QUERY)
			{
				m_Regions.DeleteAll();

				m_snapshot.Shot(& m_Regions);
				return TRUE;
			}
            else if ( LOWORD(wParam)==IDC_GDIQUERY )
            {
                GdiQueryTable();
                return TRUE;
            }
			break;

		case WM_NOTIFY:
			if (wParam == IDC_REGIONS)
			{
				NM_LISTVIEW * pInfo = (NM_LISTVIEW *) lParam;
				
				if ( (pInfo->hdr.code == NM_DBLCLK) && (pInfo->iItem != -1) )  
				{
					TCHAR Start[16], Size[16];

					m_Regions.GetItemText(pInfo->iItem, 2, Start, sizeof(Start));
					m_Regions.GetItemText(pInfo->iItem, 3, Size,  sizeof(Size));

					unsigned nStart, nSize;

					sscanf(Start, "%x", & nStart);
					sscanf(Size,  "%x", & nSize);

					m_snapshot.ShowDetail(m_hInst, nStart, nSize);
					return TRUE;
				}
			}
			break;

		case WM_DESTROY:
			m_Regions.DeleteAll();
			return TRUE;
    }
    
    return FALSE;
}

