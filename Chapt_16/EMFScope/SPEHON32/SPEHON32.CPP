//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : spehon32.cpp						                                 //
//  Description: Spying DLL entry point                                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>

#define SPEHON32

#include "..\..\..\include\pehack.h"

#include "spehon32.h"
#include "playspl.h"

HINSTANCE h_instance;


BOOL InitializeAPISpy32(void)
{
	// if called by SPOOL32.EXE, SPOOLSS.DLL is already loaded
	HMODULE hModule = GetModuleHandle("SPOOLSS.DLL");
	
	if ( hModule )
	{
		KPEFile pe(hModule);
	
		return pe.SetImportAddress("GDI32.DLL", "gdiPlaySpoolStream", (FARPROC) MygdiPlaySpoolStream)!=NULL;
	}

	// if SPOOLSS.DLL not loaded, we are in EmfScope process
	return FALSE;
}


int WINAPI DllMain(HINSTANCE hInst, ULONG ul_reason_being_called, LPVOID lpReserved)
{
    switch (ul_reason_being_called)
    {
        case DLL_PROCESS_ATTACH:
            h_instance = hInst;

            InitializeAPISpy32();
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            break;
    }

    return 1;
}

#pragma data_seg("Shared")

HHOOK g_hhook       = NULL;
BOOL  g_firsttime   = TRUE;
HWND  hwnd_Spooler  = NULL;

#pragma data_seg()

#pragma comment(linker, "/section:Shared,rws")


LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (g_firsttime)
	{
		MSG *m;

		g_firsttime = FALSE;
		
		m = (MSG *)lParam;             // refer to help on GetMsgProc
		if (m->message==WM_NULL)
			hwnd_Spooler  = (HWND) m->wParam;

		// server does not have a window for the moment
		// set dialog window handler into spool32.exe USERDATA, so that loader find us
		if (hwnd_Spooler)
			SetProp(hwnd_Spooler, Prop_Server, (HANDLE) id_seed);
	}

	return CallNextHookEx(g_hhook, nCode, wParam, lParam);
}


LIBAPI BOOL WINAPI SetSpoolerHook(DWORD dwThreadId, HWND hwnd_Spooler)
{
	if (dwThreadId!=0)
	{
		if (g_hhook)
			return TRUE;

		g_hhook = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)GetMsgProc, h_instance, dwThreadId);

		if (g_hhook==NULL)
			return FALSE;

		// the hook was installed successfully; 
		// force a message to the thread's queue so the hook function gets called
		return PostThreadMessage(dwThreadId, WM_NULL, 
								 (WPARAM)hwnd_Spooler, 0);
	}
	else
	{
		BOOL rslt;

		if (g_hhook==NULL)
			return TRUE;
		
		rslt = UnhookWindowsHookEx(g_hhook);

		g_hhook = NULL;

		return rslt;
	}
}
