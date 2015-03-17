//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : stack.cpp				                                             //
//  Description: Multithread-safe shared stack                                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <assert.h>

#include "..\..\include\Decoder.h"
#include "..\..\include\atom.h"

#include "Function.h"
#include "Stack.h"


KStack::KStack()
{
    int i;

    nThread    = 0;

	nFree      = MAXSTACKSIZE;
    FreeCell   = 0;
    InitializeCriticalSection(&cs);
        
    for (i=0; i<MAXSTACKSIZE; i++)
        Cell[i].next = i+1;
    Cell[MAXSTACKSIZE-1].next = -1;

    for (i=0; i<MAXTHREADNO; i++)
    {
        Depth[i] = 0;
        Tos[i]   = -1;
    }
}


KStack::~KStack()
{
	assert(nFree==MAXSTACKSIZE);

    DeleteCriticalSection(&cs);
}
    
    
int KStack::LookupThread(BOOL addifmissing)
{
    unsigned tid = GetCurrentThreadId();

    for (int t=0; t<nThread; t++)
        if ( Thread[t] == tid )
            return t;

    if (addifmissing)
    {
        EnterCriticalSection(&cs);
        int rslt = nThread++;
            
        Thread[rslt] = tid;
        LeaveCriticalSection(&cs);

        return rslt;
    }
    else
    {
        assert(FALSE);
        return -1;
    }
}
    	

KRoutineInfo * KStack::Push(void)
{
    // check if stack is full
    if (nFree == 0)
    {
	    assert(FALSE);
		return NULL;
	}
		
    int t = LookupThread(TRUE);

    assert(t>=0);

    EnterCriticalSection(&cs);

    int c = FreeCell;
    FreeCell = Cell[c].next;
    nFree --;

    LeaveCriticalSection(&cs);

    Cell[c].next = Tos[t];
    Tos[t]       = c;
    Depth[t] ++;
		
    return & Cell[c];
}
	
    
KRoutineInfo *KStack::Lookup(int & depth)
{
    if ( nFree == MAXSTACKSIZE )
    {
		assert(FALSE);
		return NULL;
	}
    
    int t = LookupThread(FALSE);

    if (t < 0)
        return NULL;

    depth = Depth[t];

    return & Cell[Tos[t]];
}


void KStack::Pop(void)
{
    if ( nFree == MAXSTACKSIZE )
        assert(FALSE);
    else
    {
        int t = LookupThread(FALSE);

        if (t >= 0)
        {
            int c = Tos[t];

            if (c >= 0)
            {
                Tos[t] = Cell[c].next;
                Depth[t] --;

                EnterCriticalSection(&cs);
                Cell[c].next = FreeCell;
                FreeCell     = c;
                nFree ++;
                LeaveCriticalSection(&cs);
            }
            else
                assert(FALSE);
        }
    }
}        


