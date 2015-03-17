#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : apipage.h				                                             //
//  Description: KApiPage class                                                      //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

extern KApiTable ApiTable;

class KApiPage : public KPropertySheetPage
{
    virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    KListView  List;
    HINSTANCE  hInst;
    HWND       m_hWnd;
};

