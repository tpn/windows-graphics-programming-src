//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : diver.cpp				                                             //
//  Description: Spying DLL interface with spy control program                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#define DIVER

#include <windows.h>
#include <assert.h>
#include <tchar.h>

#include "..\..\include\Win.h"
#include "..\..\include\Profile.h"
#include "..\..\include\Decoder.h"
#include "..\..\include\atom.h"

#include "Function.h"
#include "Report.h"
#include "Stack.h"
#include "FuncTable.h"
#include "Diver.h"
#include "Dump.h"
#include "Format.h"

// Shared Data

#pragma data_seg("Shared")

HWND    h_Controller                    = NULL;
HHOOK   h_ShellHook                     = NULL;
LRESULT __declspec(dllexport) h_Reply   = 0;

#pragma data_seg()

#pragma comment(linker, "/section:Shared,rws")


KDump ApiLog;

int debug = TRUE;


class KMailBox : public KWindow
{
public:
    LRESULT ExecuteCommand(COPYDATASTRUCT * pcds)
    {
        assert(pcds);

        if (pcds==NULL)
            return 0;

        switch (pcds->dwData)
        {
            case C_LOADMODULE:
                {
                    KLoadModule *pm = KLoadModule::Parse(pcds);

                    return pFuncTable->LoadModule(pm->m_caller, pm->m_callee,
						pm->m_intrfc,
						pm->vtable,
						pm->queryinterface,
						pm->methodno);
                }
                break;

            case C_ADDFUNC:
                {
                    KAddFunc * pf = KAddFunc::Parse(pcds);

                    return pFuncTable->AddFunc(pf->m_ord, pf->m_name, pf->m_cls, pf->m_parano, pf->m_paratype,
								pf->m_kind, pf->m_oldaddress);
                }
                break;

            case C_INTERCEPT:
                {
                    KOptions * opt = KOptions::Parse(pcds);
                    
                    pFuncTable->SetOptions(opt->bLogCall, opt->bDispCall);

                    ProxyInit();

				//	DebugBreak();

                    int n1 = pFuncTable->InterceptWin32();
                    int n2 = pFuncTable->InterceptCom();
					int n3 = pFuncTable->InterceptSysCall();

					{
						TCHAR temp[32];

						wsprintf(temp, _T("intercepted %d %d %d"), n1, n2, n3);

						Send(M_TEXT, 0, 0, temp);
					}

                    return ( n1 * 1000 + n2 ) * 1000 + n3;
                }
                break;

            default:
                assert(FALSE);
        }

        return 0;
    }

    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            case WM_CREATE:
                m_hWnd = hWnd;
                break;

            case WM_COPYDATA:
                return ExecuteCommand( (COPYDATASTRUCT *) lParam);
    
                break;

            default:
                 return DefWindowProc(hWnd, uMsg, wParam, lParam);
        }

        return 0;
    }

};

IDecoder * pDecoderChain = NULL;


class KDiver 
{
	KMailBox	  MailBox;
	KBasicDecoder Decoder;
	
public:
	
	HINSTANCE hInstance;
	
    KDiver()
    {
        hInstance      = NULL;
        MailBox.m_hWnd = NULL;
    }

	IDecoder * InstallExternalDecoders(IDecoder * pChain, HINSTANCE hInst);

	BOOL Initialize(HINSTANCE hInst)
	{
		hInstance = hInst;
		
		pFuncTable->Initialize();

		Decoder.Initialize(pFuncTable);
		pDecoderChain = & Decoder;

		pDecoderChain = InstallExternalDecoders(pDecoderChain, hInst);

        ApiLog.BeginDump("c:\\pogy%03d.txt");

        MailBox.CreateEx(0, "MailBox", "MailBox", 
		                 WS_OVERLAPPEDWINDOW,
    	                 -100, 10, 0, 10, NULL, NULL, hInst);
	
        if (MailBox.m_hWnd)
           Send(M_MAILBOX, (int)MailBox.m_hWnd, 0, NULL);

		return TRUE;
	}

	BOOL Terminate(void)
	{
        ApiLog.EndDump();
        pFuncTable->Terminate();

        if (MailBox.m_hWnd)
            DestroyWindow(MailBox.m_hWnd);
		
        return TRUE;
	}

};


IDecoder * KDiver::InstallExternalDecoders(IDecoder * pChain, HINSTANCE hInst)
{
	KProfile pro;

	if ( ! pro.SetFileName(hInst, "Pogy.ini") )
		return pChain;

	TCHAR szSeq[32];
	_tcscpy(szSeq, "Decoder1");

	while ( pro.ReadString("Option", szSeq) )
	{
		// Decoder1=pogygdi.dll!_Create_GDI_Decoder@0
		TCHAR module[128];
		TCHAR name[128];

		module[0] = 0;
		name[0]   = 0;

		pro.ReadIdentifier(module, sizeof(module)/sizeof(TCHAR), '.');
		pro.ReadIdentifier(name,   sizeof(name)/sizeof(TCHAR), '@');

		if ( module[0] && name[0] )
		{
			HMODULE       hModule = LoadLibrary(module);
			DecoderCreator hProc;

			if ( hModule )
			{
				hProc = (DecoderCreator ) GetProcAddress(hModule, name);

				if ( hProc )
				{
					IDecoder * pNew = (*hProc)();

					if ( pNew && pNew->Initialize(pFuncTable) )
					{
						pNew->pNextDecoder = pChain;
						pChain = pNew;

						_tcscat(module, "!");
						_tcscat(module, name);
						_tcscat(module, " installed.");
						Send(M_TEXT, 0, 0, module);
					}
					else
					{
						wsprintf(name, "Decode %s initialization failed", module);
						Send(M_TEXT, 0, 0, name);
					}
				}
				else
				{
					wsprintf(module, "Unable to find %s.", name);
					Send(M_TEXT, 0, 0, module);
				}
			}
			else
			{
				wsprintf(name, "Unable to load %s.", module);
				Send(M_TEXT, 0, 0, name);
			}
		}

		szSeq[7] ++; // 1..9 only
	}

	return pChain;
}


KDiver	Diver;

KStack *pStack = NULL;

HINSTANCE hInstance;


BOOL __declspec(dllexport) StartDiver(void)
{
	static BOOL bDiverStarted = FALSE;

	if ( bDiverStarted )
		return TRUE;

	bDiverStarted = TRUE;

	if ( h_Controller==NULL )
		h_Controller = FindWindow(NULL, "Pogy");

    pStack = new KStack;

 	if ( ! Diver.Initialize(hInstance) )
	    return FALSE;

    return pStack!=NULL;
}


BOOL StopDiver(void)
{
	try
	{
		if ( pStack )
			Diver.Terminate();
		
		//  delete pStack;         // assert in destructor could cause skipping code.

		//  pStack = NULL;
	}
	catch (...)
	{
		return FALSE;
	}

    return TRUE;
}            			


int WINAPI DllMain ( HINSTANCE hInst,
                     ULONG  ul_reason_being_called,
                     LPVOID lpReserved
                   )
{
    switch (ul_reason_being_called)
    {
        case DLL_PROCESS_ATTACH:
            hInstance = hInst;
            break;

        case DLL_PROCESS_DETACH:
			StopDiver();
    }

	return TRUE;
}


LRESULT CALLBACK ShellProc( int    nCode,	// hook code
						    WPARAM wParam,	// event-specific information
						    LPARAM lParam 	// undefined
						  )
{
    TCHAR sModule[MAX_PATH];

    switch (nCode)
    {
        case HSHELL_WINDOWCREATED:
			GetModuleFileName(NULL, sModule, sizeof(sModule)/sizeof(TCHAR));

			h_Reply = 0;
			Send(M_CREATE, wParam, 0, sModule);
            
			if ( h_Reply==REPLY_DIVERSTART )
				StartDiver();
            break;

        case HSHELL_WINDOWDESTROYED:
	        GetModuleFileName(NULL, sModule, sizeof(sModule)/sizeof(TCHAR));

            Send(M_DESTROY, wParam, 0, sModule);
            
            break;
    }

    assert(h_ShellHook);

    if (h_ShellHook)
        return CallNextHookEx(h_ShellHook, nCode, wParam, lParam);
    else
        return FALSE;
}


void _declspec(dllexport) SetupDiver(int nOpt, HWND hWnd)
{
    switch (nOpt)
    {
        case Diver_SetController:
            h_Controller = hWnd;
            break;

        case Diver_Install:
    		assert(h_ShellHook==NULL);
	    	h_ShellHook  = SetWindowsHookEx(WH_SHELL, (HOOKPROC) ShellProc, hInstance, 0);
            if ( hWnd )
                h_Controller = hWnd;
            break;

        case Diver_UnInstall:
	    	assert(h_ShellHook!=NULL);
		    UnhookWindowsHookEx(h_ShellHook);

		    h_ShellHook	= NULL;
            break;
 	}
}