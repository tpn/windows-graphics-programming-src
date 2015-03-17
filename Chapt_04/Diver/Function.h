#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : function.h				                                             //
//  Description: KFuncTable class                                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include "Format.h"
#include "Proxy.h"
#include "Module.h"

#define spy_summary     0x01
#define spy_loadlibrary 0x02

class KDumpData
{
public:
	TCHAR cppthis[10];
	TCHAR param[100];
};


class KRoutineInfo
{
public:
    int      next;
    
    unsigned funcid;
    unsigned rtnaddr;

    unsigned entertime;

    KDumpData dumpdata;
};


class KFunction
{   
public:
	unsigned char	f_module;		// index to module table
//	unsigned char   f_selection;	// spying selection
	unsigned char   f_parano;       // no of parameters
    unsigned char   f_kind;		    // WIN32API, COMMETHOD, SYSCALL
	unsigned char   f_hooked;        

    unsigned short  f_methodid;		// sequential method id within a COM interface	
	unsigned short  f_class;		// function classification
	ATOM            f_name;         // index into name table
	unsigned short  f_firstpara;    // index to parameter table

    unsigned char	f_stub[10];		// stub function
									// mov eax, index
									// jmp newaddress

    FARPROC         f_oldaddress;	// original function

    unsigned        f_totaltime;
    unsigned        f_callno;
    
    FARPROC			f_newaddress(void) const
	{
		return (FARPROC) (void *) f_stub;
	}

	void InitStub(int index, FARPROC newaddress);
};


class KParameter
{
public:
    ATOM  p_name;			// index into name table
    ATOM  p_type;			// index into name table
};


const int MAX_FUNC      = 1024;
const int MAX_PARA      = MAX_FUNC * 8;
    
class KFuncTable : public KModuleTable, public KAtomTable
{
	int        m_funcno;
    int        m_parano;
    int        m_lastfunc;
                
    int  DefPara(const char * paratype, const char *name = NULL);
        
public:

    BOOL       m_logcall;
    BOOL       m_dispcall;

	KFunction  m_func[MAX_FUNC];
    KParameter m_para[MAX_PARA];
    
    KFuncTable()
    {   
        memset(m_func, 0, sizeof(m_func));

        m_funcno  = 0;
        m_parano  = 0;
            
        m_logcall = TRUE;
        m_dispcall = TRUE;
    }
        
    void SetOptions(BOOL logcall, BOOL dispcall)
    {
        m_logcall  = logcall;
        m_dispcall = dispcall;
    }

    int GetParaNo(int f) const
    {
        return m_func[f].f_parano;
    }

    ATOM GetParaAtom(int f, int p) const
    {
        return m_para[m_func[f].f_firstpara + p].p_name;
    }

    ATOM GetParaTypeAtom(int f, int p) const
    {
        return m_para[m_func[f].f_firstpara + p].p_type;
    }

    const char *GetParaName(int f, int p) 
    {
        return GetAtomName(m_para[m_func[f].f_firstpara + p].p_name);
    }

    const char * GetParaTypeName(int f, int p)
    {
        return GetAtomName(m_para[m_func[f].f_firstpara + p].p_type);
    }

    const char *GetFuncName(int f) 
    {
        return GetAtomName(m_func[f].f_name);
    }

    FARPROC GetProcAddr(int funcid);
                        
    int AddFunc(unsigned ord, const char *name, unsigned opt, int parano, const char *paratype, int kind, const void * pOldAddr);
    
    void Initialize(void);
    
    void virtual Initialize_API(void)
    {
    }

    void virtual Terminate_API(void)
    {
    }

    void Terminate(void)
    {
        Terminate_API();
    }

	FARPROC LookupInterceptedAPI(FARPROC address, int & id);

    int  InterceptWin32(const char *Caller,
                        const char *Callee,
                        PIMAGE_THUNK_DATA pName,
                        PIMAGE_THUNK_DATA pThunk,
                        const char *baseAddress);
	
    int  InterceptWin32(const char *ModuleName, const char * baseAddress);

    int  InterceptWin32(void);
    int  InterceptCom(void);
	int  InterceptSysCall(void);

    void virtual FuncEntryCallBack(KRoutineInfo *routine, EntryInfo *info)
    {
    }

    void virtual FuncExitCallBack(KRoutineInfo *routine, ExitInfo *info, unsigned leavetime, int depth)
    {
    }        

};
