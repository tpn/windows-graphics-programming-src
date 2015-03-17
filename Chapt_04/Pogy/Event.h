//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : event.h				                                             //
//  Description: KEventPage class                                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KEventPage : public KPropertySheetPage
{
    unsigned         StartTime;
    HWND             hMailBox;
    KSetupPogyPage * pSetupPage;

    virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:

    void Initialize(KSetupPogyPage * setuppage)
    {
        pSetupPage = setuppage;
    }
    
    HRESULT MonitorWindow(HWND hWnd, int oper, const char *owner);
};
