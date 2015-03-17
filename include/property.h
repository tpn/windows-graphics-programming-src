#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : property.h				                                             //
//  Description: Property sheet page and property sheet                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include <commctrl.h>

#include "dialog.h"

// Property Sheet Page
class KPropertySheetPage : protected KDialog
{
public:
    
    HPROPSHEETPAGE createPropertySheetPage(HINSTANCE hInst, int id_Template)
    {
        PROPSHEETPAGE psp;
        
        memset(& psp, 0, sizeof(psp));

        psp.dwSize      = sizeof(PROPSHEETPAGE);
        psp.dwFlags     = PSP_DEFAULT;
        psp.hInstance   = hInst;

        psp.pszTemplate = MAKEINTRESOURCE(id_Template);
        psp.pfnDlgProc  = DialogProc;
        psp.lParam      = (LPARAM) this;

        return CreatePropertySheetPage(&psp);
    }
};


// Property Sheet
class KPropertySheet
{
public:
    
    int propertySheet(HINSTANCE hInst, HWND hWnd, int id_Icon, int nPages, HPROPSHEETPAGE * hPages, const TCHAR *sCaption)
    {
        PROPSHEETHEADER psh;

        memset(& psh, 0, sizeof(PROPSHEETHEADER) );
        
        psh.dwSize      = sizeof(PROPSHEETHEADER);
        psh.dwFlags     = PSH_USEICONID | PSH_NOAPPLYNOW;
        psh.hInstance   = hInst;
        psh.hwndParent  = hWnd;
        psh.pszIcon     = MAKEINTRESOURCE(id_Icon);
        psh.nPages      = nPages;
        psh.phpage      = hPages;
        psh.pszCaption  = sCaption;
        psh.pfnCallback = NULL;

        return PropertySheet(& psh);
    }
};

