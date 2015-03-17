//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : pogy.cpp   			                                             //
//  Description: Spy control program PropertySheet                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <commctrl.h>
#include <assert.h>
#include <stdio.h>

#include "..\..\include\win.h"
#include "..\..\include\listview.h"
#include "..\..\include\property.h"

#include "..\Diver\Diver.h"
#include "..\Diver\Report.h"

#include "Resource.h"

#include "ApiTable.h"
#include "ApiPage.h"
#include "Setup.h"
#include "Event.h"
#include "Aplet.h"

#include "Pogy.h"


class KPogyAplet : public KAplet
{
    KSetupPogyPage   SetupPogyPage;
    KApiPage         ApiPage;
    KEventPage       EventPage;
    
public:
    void InitProcess(HINSTANCE hInstance);
    void TermProcess(HINSTANCE hInstance);
    int  GetPropertySheetPages(HINSTANCE hInst, HPROPSHEETPAGE *hPage);
    void Initialize(HINSTANCE hInst);
};


void KPogyAplet::InitProcess(HINSTANCE hInstance)
{
    SetupPogyPage.ReadOptions(hInstance);

    SetupDiver(Diver_Install, NULL);
} 
    

void KPogyAplet::TermProcess(HINSTANCE hInstance)
{
    SetupDiver(Diver_UnInstall, NULL);

    SetupPogyPage.SaveOptions(hInstance);
}
    

int KPogyAplet::GetPropertySheetPages(HINSTANCE hInst, HPROPSHEETPAGE *hPage)
{
    hPage[0] =     EventPage.createPropertySheetPage(hInst, IDD_EVENT);
    hPage[1] = SetupPogyPage.createPropertySheetPage(hInst, IDD_SETUPPOGY);
    hPage[2] =       ApiPage.createPropertySheetPage(hInst, IDD_API);
    
    return 3;
}


void KPogyAplet::Initialize(HINSTANCE hInst)
{
    ApiPage.hInst = hInst;

    EventPage.Initialize(& SetupPogyPage);

    ApiTable.Initialize(hInst, 0);
}


KAplet *CreatePogy(void)
{
    return new KPogyAplet;
}

void DeletePogy(KAplet *aplet)
{
    delete (KPogyAplet *) aplet;
}


