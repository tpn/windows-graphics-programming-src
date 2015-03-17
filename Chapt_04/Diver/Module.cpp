//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : module.cpp				                                             //
//  Description: KModuleTable class                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <memory.h>
#include <string.h>
#include <assert.h>

#include "Module.h"


BOOL KInterface::Hack(int n, FARPROC newfunc)
{
	DWORD cBytesWritten;
	
	WriteProcessMemory(GetCurrentProcess(),
					   & pvtbl[n],
					   & newfunc,
					   sizeof(newfunc),
					   &cBytesWritten);

	return cBytesWritten == sizeof(newfunc);
}


BOOL HackMethod(unsigned vtable, int n, FARPROC newfunc)
{
	DWORD cBytesWritten;
	
	WriteProcessMemory(GetCurrentProcess(),
					   (LPVOID) (vtable + n * 4),
					   & newfunc,
					   sizeof(newfunc),
					   &cBytesWritten);

	return cBytesWritten == sizeof(newfunc);
}


int KModuleTable::LookupModule(const char *caller, const char *callee)
{
    for (int i=0; i<m_moduleno; i++)
        if ( (stricmp(m_modules[i].m_caller, caller)==0) &&
             (stricmp(m_modules[i].m_callee, callee)==0) )
            return i;

    return -1;
}
                              

int KModuleTable::LoadModule(const char *caller, const char *callee, 
							 const char * intrfc,
							 unsigned vtable, unsigned queryinterface, int methodno)
{   
    // check if already loaded    
    m_lastmodule = LookupModule(caller, callee);
    if (m_lastmodule != -1)
        return m_lastmodule;
    
    // too many modules
    if (m_moduleno >= MAX_MODULE)
    {
        m_lastmodule = -1;
        return m_lastmodule;
    }        

    // add to module table    
	memset(& m_modules[m_moduleno], 0, sizeof(m_modules[m_moduleno]));

    m_modules[m_moduleno].m_handle      = LoadLibrary(callee); 

    Copy(m_modules[m_moduleno].m_caller, caller);
    Copy(m_modules[m_moduleno].m_callee, callee);
	Copy(m_modules[m_moduleno].m_intrfc, intrfc);

	m_modules[m_moduleno].m_vtable		   = vtable;
	m_modules[m_moduleno].m_queryinterface = queryinterface;
	m_modules[m_moduleno].m_methodno       = methodno;

//  char temp[64];
    
//  wsprintf(temp, "Loadlibrary(%s)=%x", callee, m_modules[m_moduleno].m_handle);
//  MessageBox(NULL, temp, "LoadModule", MB_OK);

    if (m_modules[m_moduleno].m_handle == NULL)
    {
        assert(FALSE);
        m_lastmodule = -1;
    }
    else        
        m_lastmodule = m_moduleno++;
    
    return m_lastmodule;
}


/*
int CModuleTable::LoadInterface(REFCLSID rclsid, REFIID riid)
{
    // check if already loaded
    for (int i=0; i<m_moduleno; i++)
    {
        if (m_modules[i].m_isinterface)
        if (m_modules[i].m_clsid == rclsid)
        if (m_modules[i].m_iid   == riid)
        {
            m_lastmodule = i;
            return m_lastmodule;
        }
    }

    // too many modules
    if (m_moduleno >= MAX_MODULE)
    {
        m_lastmodule = -1;
        return m_lastmodule;
    }

    // add to module table
    m_modules[m_moduleno].m_isinterface = TRUE;
    m_modules[m_moduleno].m_clsid       = rclsid;
    m_modules[m_moduleno].m_iid         = riid;
    m_modules[m_moduleno].m_iunknown    = NULL;

    HRESULT hr = CoCreateInstance(rclsid, NULL, CLSCTX_INPROC_SERVER, riid, 
								  (void **) & m_modules[m_moduleno].m_iunknown );

    if (SUCCEEDED(hr))
        m_lastmodule = m_moduleno ++;
    else
        m_lastmodule = -1;

    return m_lastmodule;
}
*/

void KModuleTable::FreeModules(void)
{
    for (int i=0; i<m_moduleno; i++)
     /* if (m_modules[i].m_isinterface)
        {
            ((IUnknown *) m_modules[i].m_iunknown)->Release();
        }
        else */
        {
            FreeLibrary(m_modules[i].m_handle);
        }
}
