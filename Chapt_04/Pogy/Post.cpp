//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : post.cpp				                                             //
//  Description: Send information to spying DLL                                      //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <assert.h>

#include "..\Diver\Diver.h"

#define nCopy(dest, src) { dest[sizeof(dest)-1] = 0; strncpy(dest, src, sizeof(dest)-1); }


int KPost::LoadModule(const char *caller, const char *callee,
					  const char *intrfc, unsigned vtable, unsigned queryinterface,
					  int methodno, GUID & iid)
{
    KLoadModule    lm;
    memset(&lm, 0, sizeof(KLoadModule));

    nCopy(lm.m_caller, caller);
    nCopy(lm.m_callee, callee);
	nCopy(lm.m_intrfc, intrfc);

	lm.vtable		  = vtable;
	lm.queryinterface = queryinterface;
	lm.methodno       = methodno;
	lm.iid            = iid;

	COPYDATASTRUCT cds;

    cds.dwData     = C_LOADMODULE;
    cds.cbData     = sizeof(KLoadModule);
    cds.lpData     = & lm;

	seq = 0;
    return SendMessage(hReceiver, WM_COPYDATA, (WPARAM) hSender, (LPARAM) & cds);
}


int KPost::AddFunc(const char *name, unsigned cls, int parano, const char * paratype, int kind, const void * pOldAddr)
{
    KAddFunc       af;
    COPYDATASTRUCT cds;

	memset(&af, 0, sizeof(af));

    af.m_ord        = seq ++;
    af.m_cls        = cls;
    af.m_parano     = parano;
	af.m_kind       = kind;
	af.m_oldaddress = pOldAddr;

	if ( strlen(paratype) < sizeof(af.m_paratype) )
		strcpy(af.m_paratype, paratype);
	else
		assert(false);

//  assert(parano <= MAXPARANO);

    nCopy(af.m_name, name);

    cds.dwData   = C_ADDFUNC;
    cds.cbData   = sizeof(KAddFunc);
    cds.lpData   = & af;

    return SendMessage(hReceiver, WM_COPYDATA, (WPARAM) hSender, (LPARAM) & cds);
}


int KPost::Intercept(BOOL logcall, BOOL dispcall)
{
    COPYDATASTRUCT cds;
    KOptions       opt;

    opt.bLogCall  = logcall;
    opt.bDispCall = dispcall;

    cds.dwData    = C_INTERCEPT;
    cds.cbData    = sizeof(opt);
    cds.lpData    = & opt;

    return SendMessage(hReceiver, WM_COPYDATA, (WPARAM) hSender, (LPARAM) & cds);
}

