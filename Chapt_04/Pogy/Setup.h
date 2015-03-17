#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : setup.h				                                             //
//  Description: KSetupPogyPage class                                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KSetupPogyPage : public KPropertySheetPage
{
    virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:    

    bool bLogCall;
    bool bDispCall;

    void SetButtons(void);
    void ReadButtons(void);
    void ReadOptions(HINSTANCE hInstance);
    void SaveOptions(HINSTANCE hInstance);
};
