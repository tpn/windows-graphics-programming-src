//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : report.cpp				                                             //
//  Description: Send report to spy control program                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <tchar.h>

#include "Report.h"

// Report to Client

extern HWND h_Controller;

LRESULT Send(unsigned id, unsigned para, unsigned time, const TCHAR *messtext)
{
    COPYDATASTRUCT cds;
    ReportMessage  rpt;

    memset(& rpt, 0, sizeof(rpt));

    rpt.m_para = para;
    rpt.m_time = time;
    rpt.m_tick = GetTickCount();

    if ( messtext )
        _tcsncpy(rpt.m_text, messtext, sizeof(rpt.m_text)/sizeof(TCHAR)-1);

    cds.dwData = id;
    cds.cbData = sizeof(rpt);
    cds.lpData = (void *) & rpt;

    return SendMessage(h_Controller, WM_COPYDATA, NULL, (LPARAM) & cds);
}
