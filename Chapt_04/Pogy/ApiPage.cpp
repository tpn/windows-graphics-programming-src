//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : apipage.cpp			                                             //
//  Description: KApiPage class                                                      //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <commctrl.h>
#include <assert.h>
#include <stdio.h>
#include <tchar.h>

#include "..\..\include\win.h"
#include "..\..\include\listview.h"
#include "..\..\include\property.h"

#include "..\Diver\Report.h"

#include "Resource.h"
#include "ApiTable.h"
#include "ApiPage.h"


KApiTable ApiTable;


BOOL KApiPage::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
		case WM_INITDIALOG:
			m_hWnd    = hWnd;

            List.FromDlgItem(hWnd, IDC_LIST);
                
            List.AddIcon(LVSIL_SMALL,  hInst, IDI_API);
            List.AddIcon(LVSIL_SMALL,  hInst, IDI_COM);
			List.AddIcon(LVSIL_SMALL,  hInst, IDI_SYSCALL);

            List.AddColumn(0,  80, _T("Class"));
            List.AddColumn(1,  80, _T("Interface"));
            List.AddColumn(2, 200, _T("Function"));
			List.AddColumn(3,  80, _T("SysCall"));

            if (ApiTable.AddTargets(hInst, GetDlgItem(hWnd, IDC_TARGET)) )
            {
                SendDlgItemMessage(hWnd, IDC_TARGET, CB_SETCURSEL, 0, 0);
                ApiTable.Add2ListView(List);
            }
                                
            return TRUE;

        case WM_COMMAND:
            if ( wParam==MAKEWPARAM(IDC_TARGET, CBN_SELCHANGE) )
            {
                if (ApiTable.Initialize(hInst, SendDlgItemMessage(hWnd, IDC_TARGET, CB_GETCURSEL, 0, 0)))
                    ApiTable.Add2ListView(List);

                return TRUE;
            }
    }
        
    return FALSE;
}


