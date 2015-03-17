#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : stack.h				                                             //
//  Description: Multithread-safe shared stack                                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KStack
{
#define MAXSTACKSIZE 256
#define MAXTHREADNO  32

	int          nFree;
    int          FreeCell;
	KRoutineInfo Cell[MAXSTACKSIZE];

    CRITICAL_SECTION  cs;

    int          nThread;
    
    unsigned     Thread[MAXTHREADNO];
    int          Tos   [MAXTHREADNO];
    int          Depth [MAXTHREADNO];

public:
	KStack();
	~KStack();
	    
    int LookupThread(BOOL addifmissing);
    KRoutineInfo * Push(void);
    KRoutineInfo * Lookup(int & depth);
    void Pop(void);

};
