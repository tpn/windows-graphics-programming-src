//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : main.cpp				                                             //
//  Description: Spy control program main program                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <commctrl.h>
#include <assert.h>

#include "..\..\include\win.h"
#include "..\..\include\property.h"

#include "Resource.h"
#include "Aplet.h"
#include "Pogy.h"


class KMain : public KPropertySheet
{
    typedef enum { MAXPAGES = 10 };

    HINSTANCE hInstance;
    KAplet   *Pogy;
    HWND      m_hWnd;

public:
    
    void Run(HINSTANCE hInst)
    {
        HPROPSHEETPAGE hPage[MAXPAGES];

        hInstance = hInst;
    
        Pogy = CreatePogy();

        Pogy->InitProcess(hInst);
        Pogy->Initialize(hInstance);

        int pn = Pogy->GetPropertySheetPages(hInst, hPage);
        
        propertySheet(hInstance, NULL, IDI_SPY, pn, hPage, "Pogy");

        Pogy->TermProcess(hInst);

        DeletePogy(Pogy);
    }
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine,	int nCmdShow)
{
    // listview needs it.
    InitCommonControls();

    KMain Main;
   
    Main.Run(hInstance);

    return TRUE;
}
