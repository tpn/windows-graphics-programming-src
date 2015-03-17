//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : setup.cpp				                                             //
//  Description: KSetupPogyPage class                                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <commctrl.h>
#include <assert.h>
#include <stdio.h>
#include <tchar.h>

#include "..\..\include\win.h"
#include "..\..\include\property.h"
#include "..\..\include\Profile.h"

#include "Resource.h"
#include "Setup.h"

const TCHAR sec_option    [] = _T("Option");

const TCHAR key_logcall   [] = _T("LogCall");
const TCHAR key_dispcall  [] = _T("DispCall");


void KSetupPogyPage::SetButtons(void)
{
    if ( IsWindow(m_hWnd) )
    {
        SendDlgItemMessage(m_hWnd, IDC_LOGCALL,   BM_SETCHECK, bLogCall, 0);
        SendDlgItemMessage(m_hWnd, IDC_DISPCALL,  BM_SETCHECK, bDispCall, 0);
    }
}


void KSetupPogyPage::ReadButtons(void)
{
    if ( IsWindow(m_hWnd) )
    {
        bLogCall   = SendDlgItemMessage(m_hWnd, IDC_LOGCALL,  BM_GETCHECK, 0, 0) != 0;
        bDispCall  = SendDlgItemMessage(m_hWnd, IDC_DISPCALL, BM_GETCHECK, 0, 0) != 0;
    }
}


BOOL KSetupPogyPage::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:
            m_hWnd = hWnd;
            SetButtons();
            return TRUE;

        case WM_DESTROY:
            ReadButtons();
            m_hWnd = NULL;
            return TRUE;

        default:
            break;
    }

    return FALSE;
}


void KSetupPogyPage::ReadOptions(HINSTANCE hInstance)
{
    KProfile Profile;

    Profile.SetFileName(hInstance, _T("Pogy.ini"));
    
    bLogCall   = Profile.ReadInt(sec_option, key_logcall,  true) != 0;
    bDispCall  = Profile.ReadInt(sec_option, key_dispcall, true) != 0;
}


void KSetupPogyPage::SaveOptions(HINSTANCE hInstance)
{
    KProfile Profile;

    Profile.SetFileName(hInstance, _T("Pogy.ini"));

    Profile.Write(sec_option, key_logcall,   bLogCall);
    Profile.Write(sec_option, key_dispcall,  bDispCall);
}


