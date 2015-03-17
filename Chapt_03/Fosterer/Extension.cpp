//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : extension.cpp			                                             //
//  Description: Interface with debugger extension                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <imagehlp.h> 
#include <richedit.h>
#include <wdbgexts.h>

#include "ImageModule.h"
#include "Extension.h"
#include "Host.h"

bool KExtensionRoutine::Attach(HINSTANCE hModule, const char * name)
{
    assert(hModule);

    m_funcname = name;
    m_funcaddr = (ExtensionRoutine) GetProcAddress(hModule, name);

    return m_funcaddr != NULL;
}


bool KExtensionRoutine::Help(void)
{
    if ( m_funcaddr )
    {
        m_funcaddr(0, 0, 0, 1, "-?");
        return true;
    }
    else
        return false;
}


bool KExtensionRoutine::Call(const char * args, HANDLE proc, HANDLE thread, ULONG pc, ULONG processor)
{
    if ( m_funcaddr )
    {
		// NULL is not be handled well by the extension DLL
		if ( args==NULL )
			args = "";

		try
		{
			m_funcaddr(proc, thread, pc, processor, args);
		}
		catch (...)
		{
			MessageBox(NULL, m_funcname, "Exception in", MB_OK);
		}

        return true;
    }
    else
	{
		assert(false);

        return false;
	}
}


bool KGDIExtension::Load(const void * ExtensionAPI, const char * filename)
{
	// check OS version
	char temp[MAX_PATH];
	
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	GetVersionEx(& osvi);

	theHost.ExtOutput("Windows OS v%d.%d, build %d %s\n", 
		osvi.dwMajorVersion, osvi.dwMinorVersion,
		LOWORD(osvi.dwBuildNumber), osvi.szCSDVersion);
	
    hGDI = LoadLibrary(filename);

    if ( hGDI==NULL )
		return false;
    
	temp[0] = '"';
	GetModuleFileName(hGDI, temp+1, sizeof(temp));
	strcat(temp, "\" loaded.\n");
	theHost.ExtOutput(temp);

	PWINDBG_EXTENSION_DLL_INIT pInit = (PWINDBG_EXTENSION_DLL_INIT) 
		GetProcAddress(hGDI, "WinDbgExtensionDllInit");

    if ( pInit )
        pInit((WINDBG_EXTENSION_APIS *)ExtensionAPI, MajorVersion, LOWORD(osvi.dwBuildNumber));
    else
        return false;

    PWINDBG_CHECK_VERSION pCheck = (PWINDBG_CHECK_VERSION) GetProcAddress(hGDI, "CheckVersion");

    if ( pCheck )
        pCheck();
    else
        return false;

	return true;
}


bool KGDIExtension::Do(const char *command)
{
	if ( command && strlen(command) )
	{
		// skip to non space character
		while ( isspace(* command) )
			command ++;

		char cmd[MAX_PATH];
		strcpy(cmd, command);

		// find space to seperate command and parameters
		char * para = strchr(cmd, ' ');  

	  	theHost.ExtOutput("%s\n", command);

		if ( para )
			* para++ = NULL;  // 0 terminate command
		else
			para = "";

		KExtensionRoutine routine;

		if ( routine.Attach(hGDI, cmd) )
		{
			theHost.hProcess = m_hProcess;
			routine.Call(para);
			return true;
		}
		else
			theHost.ExtOutput("Unknown command: %s\n", cmd);
	}

	return false;
}
