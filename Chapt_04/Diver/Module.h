#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : module.h				                                             //
//  Description: KModuleTable                                                        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define Copy(dest, src) { dest[sizeof(dest)-1] = 0; strncpy(dest, src, sizeof(dest)-1); }

class KInterface
{        
    FARPROC *pvtbl;

public:
    
    FARPROC GetFuncAddr(int id)
    {
        return pvtbl[id];
    }

    BOOL Hack(int id, FARPROC newfunc);
};

BOOL HackMethod(unsigned vtable, int id, FARPROC newfunc);

                        //--------    Module Table    --------//
                        
const int MAX_MODULENAME = 32;
const int MAX_MODULE     = 32;                        

class KModule
{
public:
	unsigned    m_vtable;
    unsigned	m_queryinterface;
	int			m_methodno;
	
    char        m_caller[MAX_MODULENAME];     // caller module name, eg: clock.exe
    char        m_callee[MAX_MODULENAME];     // callee module name, eg: gdi32.dll 
	char		m_intrfc[MAX_MODULENAME];	  // interface name,     eg: IDirectDraw	
    
	HINSTANCE   m_handle;                     // module handle returned by LoadLibrary 

	BOOL m_isinterface(void) const
	{
		return m_intrfc[0] == 0;
	}
};


class KModuleTable
{
    KModule	   m_modules[MAX_MODULE];
    int        m_moduleno;

public:        
    int        m_lastmodule;

    const char *CalleeName(int id) const
    {
        return m_modules[id].m_callee;
    }

    const char *CallerName(int id) const
    {
        return m_modules[id].m_caller;
    }

    HINSTANCE ModHandle(int id) const
    {
        return m_modules[id].m_handle;
    }

    BOOL IsInterface(int id) const
    {
        return m_modules[id].m_isinterface();
    }

    unsigned VTable(int id) const
    {
        return m_modules[id].m_vtable;
    }
    
    void Initialize(void)
    {
        m_moduleno = 0;
    }
    
    int  LookupModule(const char *caller, const char *callee);
    
	int  LoadModule(const char *caller, const char *callee, const char * intrfc,
		              unsigned vtable, unsigned queryinterface, int methodno);
    
//	int  LoadInterface(REFCLSID rclsid, REFIID riid);

    void FreeModules(void);
};
