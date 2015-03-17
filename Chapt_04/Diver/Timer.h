//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : timer.h				                                             //
//  Description: High performance timer count ( Pentium only )                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

void        ResetTimer(void);
unsigned    GetMicroSecond(void);

class NestedTimer
{    
    unsigned overhead_start_low;   
    DWORD    overhead_start_high;  

    unsigned CPUSpeed10;   // 1/10 Mhz
    unsigned CPUSpeed16;   // 1/16 Mhz

public:        
    BOOL  started;

    NestedTimer()
    {
        CPUSpeed10 = 1000;
        CPUSpeed16 = 1000;

        started    = FALSE;
    }
        
    unsigned GetCPUSpeed(void);
        
};

extern NestedTimer nTimer;


