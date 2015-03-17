//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : report.h				                                             //
//  Description: Send report to spy control program                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

typedef enum
{   
    M_CREATE,
    M_DESTROY,
    M_ENTER,
    M_LEAVE,
    M_ENTERSUMMARY,
    M_LEAVESUMMARY,
    M_MAILBOX,
	M_TEXT
} ReportMessageId;


class ReportMessage
{
public:
    unsigned m_para;
    unsigned m_time;
    unsigned m_tick;
    TCHAR    m_text[256];
};

LRESULT Send(unsigned id, unsigned para, unsigned time, const char *messtext);
