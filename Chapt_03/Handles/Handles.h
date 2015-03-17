#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : handles.h					                                         //
//  Description: KHGDIOBJ page                                                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include "property.h"

// GDI Handle Table Property Sheet Page
class KHGDIOBJ : public KPropertySheetPage
{
	unsigned WM_SHAREHANDLE;
	
	HDC		m_hDCScreen;
    int		m_nCreator;
    int		m_nCopy;

    BOOL Generate(HWND rslt, bool search);
    BOOL OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	BOOL OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
    BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:

    KHGDIOBJ()
    {
        WM_SHAREHANDLE = 0xFFFFFFFF;
    }

};

LPCTSTR GetObjectTypeName(HGDIOBJ Handle);
