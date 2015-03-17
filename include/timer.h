#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : timer.h						                                     //
//  Description: High-precision timer using Intel clock cycle count                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#pragma warning(disable : 4035)

inline unsigned __int64 GetCycleCount(void)
{
    _asm    _emit 0x0F
    _asm    _emit 0x31
}

class KTimer
{
    unsigned __int64  m_startcycle;

public:

    unsigned __int64  m_overhead;

    KTimer(void)
    {
        m_overhead = 0;
        Start();
        m_overhead = Stop();
    }
    
    void Start(void)
    {
        m_startcycle = GetCycleCount();
    }

    unsigned __int64 Stop(void)
    {
        return GetCycleCount()-m_startcycle-m_overhead;
    }
};
