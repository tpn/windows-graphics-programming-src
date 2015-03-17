//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : imagemodule.cpp		                                             //
//  Description: Query debug symbols using ImageHlp API                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <imagehlp.h>
#include <assert.h>
#include <wdbgexts.h>

#include "ImageModule.h"
#include "Host.h"


KImageModule::KImageModule()
{
    static  int nLastProcess = 1961;

    m_pidi     = NULL;
    m_hProcess = (HANDLE) (nLastProcess ++);
}


KImageModule::~KImageModule()
{
    Unload();
}


bool KImageModule::Load(char * filename, char *sympath)
{
	m_pidi = MapDebugInformation(NULL, filename, sympath, 0); 

	if ( m_pidi==NULL ) 
        return false;

    if ( !SymInitialize(m_hProcess, sympath, FALSE) ) 
        return false;
	
	if ( !SymLoadModule(m_hProcess, NULL, filename, 0, (DWORD)m_pidi->ReservedMappedBase, 0 ) ) 
        return false;

    IMAGEHLP_MODULE im;
    im.SizeOfStruct = sizeof(im);

    SymGetModuleInfo( m_hProcess, (DWORD)m_pidi->ReservedMappedBase, &im );
	
	theHost.ExtOutput("""%s"" loaded.\n", im.LoadedImageName);
    return true;
}


void KImageModule::Unload(void)
{
    if ( m_pidi )
    {
	    SymUnloadModule(m_hProcess, (DWORD)m_pidi->ReservedMappedBase);
        SymCleanup(m_hProcess);
        UnmapDebugInformation(m_pidi);

        m_pidi = NULL;
    }
} 


const IMAGEHLP_SYMBOL * KImageModule::ImageGetSymbol(const char * name)
{
	char localname[MAX_PATH];

    memset(m_is, 0, sizeof(m_is));
    m_is[0].SizeOfStruct  = sizeof(IMAGEHLP_SYMBOL);
    m_is[0].MaxNameLength = sizeof(m_is) - sizeof(m_is[0]);
    
	// The silly implementation in imagehlp.dll will try to change the '!' in name
	// to 0, which generates an access violation, because name would came from read-only
	// constant data. Make a local copy to solve the problem
	strcpy(localname, name);
    
	if ( SymGetSymFromName(m_hProcess, localname, m_is) )
    {
        m_is[0].Address += (unsigned) m_pidi->ImageBase - (unsigned) m_pidi->ReservedMappedBase;
        
        return & m_is[0];
    }
    else
    {
        DWORD err = GetLastError();
        return NULL;
    }
}
