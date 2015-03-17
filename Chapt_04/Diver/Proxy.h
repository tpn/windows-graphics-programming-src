//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : proxy.h   			                                                 //
//  Description: API hooking gluing declarations                                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

typedef struct
{
    unsigned m_flag;
    unsigned m_edx;
    unsigned m_ecx;
    unsigned m_ebx;
    unsigned m_eax;
	
    unsigned m_funcid;
	unsigned m_rtnads;
	unsigned m_para[32];
} EntryInfo;


typedef struct
{
	unsigned m_rslt;
} ExitInfo;


void ProxyInit(void);

void ProxyProlog(void);

class KStack;
class KFuncTable;

extern KStack       * pStack;
extern KFuncTable   * pFuncTable;

