#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : extension.h			                                             //
//  Description: Interface with debugger extension                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define FreeBuild    0x0F
#define CheckedBuild 0x0C

#define MajorVersion FreeBuild

typedef 
VOID (_stdcall * ExtensionRoutine)
    (HANDLE                 hCurrentProcess,
        HANDLE                 hCurrentThread,
        ULONG                  dwCurrentPc,   
        ULONG                  dwProcessor,   
        PCSTR                  args           
     );

class KExtensionRoutine
{
    const char      * m_funcname;
    ExtensionRoutine  m_funcaddr;

public:
    KExtensionRoutine()
    {
        m_funcname = NULL;
        m_funcaddr = NULL;
    }

    bool Attach(HINSTANCE hModule, const char * name);
    bool Help(void);
    bool Call(const char * args, HANDLE proc=NULL, HANDLE thread=NULL, ULONG pc=0, ULONG processor=1);
};

class KGDIExtension
{
    HANDLE    m_hProcess;
    HANDLE    m_hThread;
    ULONG     m_dwcurrentPc;
    ULONG     m_dwProcessor;

    HINSTANCE hGDI;

public:   

    KGDIExtension(void)
    {
        hGDI = NULL;

        m_hProcess    = 0;
        m_hThread     = 0;
        m_dwcurrentPc = 0;
        m_dwProcessor = 1;
    }
   
    bool Load(const void * ExtensionAPI, const char * filename);

	bool Do(const char * command);

    ~KGDIExtension()
    {
        if ( hGDI)
            FreeLibrary(hGDI);
    }

	void SetProcess(HANDLE process)
	{
		m_hProcess = process;
	}
};
