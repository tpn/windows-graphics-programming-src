//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : timer.cpp				                                             //
//  Description: High performance time count ( Pentium only )                        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>
#include <assert.h>

#include "Timer.h"

#define enable_asm  warning (disable : 4704)
#define disable_asm warning (default : 4704)


#define GetCycleCountInto(Low, High)             \
{                                                \
    _asm                _emit 0x0F               \
    _asm                _emit 0x31               \
    _asm                mov Low, eax             \
    _asm                mov High, edx            \
}                                                                

unsigned long  InitCycleHigh;
unsigned long  InitCycleLow;

unsigned NestedTimer::GetCPUSpeed(void)
{
    if ( CPUSpeed10 == CPUSpeed16 ) // not initialized
    {
        unsigned l0, h0;
        unsigned l1, h1;
            
        GetCycleCountInto(l0, h0);
    
        // delay for 800 ms
        Sleep(800);
        // for (unsigned t1=timeGetTime()+800; timeGetTime()<t1; ) 
    
        GetCycleCountInto(l1, h1);    
    
        CPUSpeed16 = (l1 - l0 + 5000)/50000;   // clock speed (Mhz) *16
        CPUSpeed10 = (l1 - l0 + 8000)/80000;   // clock speed (Mhz) *10
    }

    return CPUSpeed10;
}

#pragma enable_asm

void ResetTimer(void)
{   
    // ensure CPUSpeed10, CPUSpeed16 get calculated
    nTimer.GetCPUSpeed();
    
    GetCycleCountInto(InitCycleLow, InitCycleHigh); // starting cycle count
}

// get timer in micro seconds

// 2^32 microsecond = 4294,967,296 = 4294 sec = 71 minutes

#pragma enable_asm

unsigned __declspec(naked) GetMicroSecond(void)
{
    __asm       _emit 0x0F                      // RDTSC: read clock counter into EDX:EAX
    __asm       _emit 0x31                      // 32 bit EAX: 21 sec on 200 Mhz machine
                                                // EDX CycleHigh
                                                // EAX CycleLow
                                                     
    __asm       sub eax, InitCycleLow                                                     
    __asm       sbb edx, InitCycleHigh          // Cycle-=InitCycle                                        
                                                // relative to first GetCPUSpeed call           
        
    __asm       add eax, eax
    __asm       adc edx, edx
        
    __asm       add eax, eax
    __asm       adc edx, edx
                            
    __asm       add eax, eax
    __asm       adc edx, edx
                            
    __asm       add eax, eax
    __asm       adc edx, edx
                            
    __asm       div nTimer.CPUSpeed16            // EAX=[CycleHigh,CycleLow] / CPUSpeed
                                                 // rslt=EAX
    __asm       ret
}

#pragma disable_asm

// Nested Timer with overhead

NestedTimer nTimer;


void _stdcall timer_Uninitialize(void)   //     uninitialized the timer to avoid overflow
{   
    ResetTimer();
    nTimer.started = FALSE;
}
