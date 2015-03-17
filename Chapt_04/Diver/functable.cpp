//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : functable.h 		                                                 //
//  Description: KMyFuncTable class                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <assert.h>
#include <tchar.h>

#include "..\..\include\Decoder.h"
#include "..\..\include\atom.h"

#include "Function.h"
#include "Timer.h"
#include "Dump.h"
#include "FuncTable.h"
#include "Diver.h"
#include "Report.h"
#include "Format.h"

//  16,		module
//  32,		function
//  10,		enter
//  10,		leave
//  10,		result

//  10,		this
//  100		parameters

KMyFuncTable FuncTable;

KFuncTable *pFuncTable = & FuncTable;

extern KDump ApiLog;


int RealTime(int clock)
{
    return MulDiv(clock, 2560, nTimer.GetCPUSpeed());
}


void KMyFuncTable::FuncEntryCallBack(KRoutineInfo *routine, EntryInfo *info)
{
    int        funcid = routine->funcid;
    const char  *name = GetFuncName(funcid);
    int i;

    if (m_logcall)
    {
        KDumpData & dd = routine->dumpdata;
        
	//	dd.cppthis[0] = 0;
		dd.param[0]   = 0;
    
    //  if ( strchr(name, '.') ) // com method: class.interface.function
    //  {
    //        // for COM method, this pointer is pushed as the first parameter
	//		MainDecoder(dd.cppthis, sizeof(dd.cppthis)/sizeof(TCHAR), atom_this, info->m_para[0]);
    //
    //      for (i = 1; i<GetParaNo(funcid); i++)
    //          MainDecoder(dd.param, sizeof(dd.param)/sizeof(TCHAR), GetParaTypeAtom(funcid, i), info->m_para[i]);
    //  }
    //  else
        {
            for (i = 1; i<GetParaNo(funcid); i++)
				MainDecoder(dd.param, sizeof(dd.param)/sizeof(TCHAR), GetParaTypeAtom(funcid, i), info->m_para[i-1]);
        }
    }

    m_func[funcid].f_callno ++;

    if (m_dispcall)
    {
        if (m_func[funcid].f_class & spy_summary)
        {
            if (((m_func[funcid].f_callno) % 1024)==1)
                Send(M_ENTERSUMMARY, m_func[funcid].f_callno, 0, name);
        }
        else    
            Send(M_ENTER, 0, 0, name);
    }
}


void KMyFuncTable::FuncExitCallBack(KRoutineInfo *routine, ExitInfo *info, unsigned leavetime, int depth)
{
    int funcid = routine->funcid;

    if (m_func[funcid].f_class & spy_summary)
    {
        m_func[funcid].f_totaltime += leavetime - routine->entertime;

        if (m_dispcall)
            if (((m_func[funcid].f_callno) % 1024)==1)
            {
                Send(M_LEAVESUMMARY, m_func[funcid].f_callno, RealTime(m_func[funcid].f_totaltime), GetFuncName(funcid));
                m_func[funcid].f_totaltime = 0;
            }
    }
    else
    {
        if (m_dispcall)
            Send(M_LEAVE, 0, RealTime(leavetime-routine->entertime), GetFuncName(funcid));
    }

    if (m_logcall)
    {
        KDumpData & dd = routine->dumpdata;

		// 1: depth
        ApiLog.WriteField(depth, atom_int);

		// 2: entering time
		ApiLog.WriteField(RealTime(routine->entertime), atom_int);

		// 3: leaving time
        ApiLog.WriteField(RealTime(leavetime),          atom_int);

		// 4: this pointer
	//	ApiLog.WriteField(dd.cppthis);

		// 5: result
		{
			ATOM typ = atom_void;

			if (GetParaNo(funcid)>=1) 
				typ = GetParaTypeAtom(funcid, 0);
        
			if ( typ != atom_void )
				ApiLog.WriteField(info->m_rslt, typ);
			else
				ApiLog.WriteField(_T(""));
		}

		// 6: caller address
		ApiLog.WriteField(routine->rtnaddr, atom_returnaddr);

		const TCHAR * name = GetFuncName(funcid);
	//	const TCHAR * pCOM = _tcschr(name, '.');

	//	// 7: function name
    //  if ( pCOM ) // com method: class.interface.function
	//		ApiLog.WriteField(name);				// class.interface.function
    //  else
        {
			TCHAR fullname[64];

			_tcscpy(fullname, CallerName(m_func[funcid].f_module));		// caller module
			_tcscat(fullname, "!");
			_tcscat(fullname, name);

			ApiLog.WriteField(fullname);								// function
		}

		// 8: parameters
		ApiLog.WriteField(dd.param);

		ApiLog.Newline();
    }

    // new dll may be loaded, do win32 api intercept if needed
    if (m_func[funcid].f_class & spy_loadlibrary)
        InterceptWin32();
}        
