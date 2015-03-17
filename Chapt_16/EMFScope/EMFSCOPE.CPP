//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : emfscope.cpp						                                 //
//  Description: EMFScope, main program                                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <assert.h>
#include <commctrl.h>

#include "Winpp.h"

#include "spehon32\\spehon32.h"
#include "resource.h"

#include "Emfscope.h"

#include "Canvas.h"
#include "Toolbar.h"


KEmfScope EmfScope;


void KEmfScope::LinkClientServer(void)
{
	SetProp(hwnd_Spooler, Prop_Client, (HANDLE) ToolBar->m_hWnd);
}


void KEmfScope::UnlinkClientServer(void)
{
	RemoveProp(hwnd_Spooler, Prop_Client);
}


void KEmfScope::WakeUp(void)
{
	RECT rect;

	GetClientRect(m_hWnd, &rect);
					
	if ( (rect.right<default_w) || (rect.bottom<default_h) )
		SetWindowPos(m_hWnd, NULL, 0, 0,
			 extra_w + default_w,
			 extra_h + default_h,
			 SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOMOVE);
}


BOOL KEmfScope::InstallSpehon32(HINSTANCE hInstance)
{
	if ( GetVersion() < 0x80000000)                // Windows NT
		return TRUE;

	BOOL spehon32_installed = FALSE;

	// try to fin Win95 Spooler main Window
	hwnd_Spooler  = FindWindow(__TEXT("SpoolProcessClass"), __TEXT("Spooler Process"));
	hwnd_Spehon32 = NULL;

	if (hwnd_Spooler==NULL)
	{
		HCURSOR oldCursor;
		int     i;

		// try to load spooler if can't be found
		WinExec("Spool32.exe", SW_SHOWNORMAL);
		
		oldCursor = SetCursor(LoadCursor(hInstance, IDC_WAIT));

		// wait up to 1 second for the Spooler Window to appear, should we ?
		for (i=0; i<10; i++)
		{
			hwnd_Spooler = FindWindow(__TEXT("SpoolProcessClass"), __TEXT("Spooler Process"));
			if (hwnd_Spooler) break;

			Sleep(100);
		}

		SetCursor(oldCursor);
	}
	else
	{
		hwnd_Spehon32 = (int) GetProp(hwnd_Spooler, Prop_Server);

		if (hwnd_Spehon32==id_seed)
			spehon32_installed = TRUE;
	}


	if (hwnd_Spooler==NULL)
	{
		char title[64];

		strcpy(title, LoadStringTemp(IDS_APPTITLE));
		
		MessageBox(0, LoadStringTemp(IDS_SPOOLERMISSING), title, MB_OK);
		
		return FALSE;
	}
	else
	{
		LinkClientServer();

		// install message hook if it's not installed yet
		if (!spehon32_installed)
			// set message hook from within spehon32.dll
			SetSpoolerHook(GetWindowThreadProcessId(hwnd_Spooler, NULL), 
						   hwnd_Spooler);

		return TRUE;
	}
}

const int gap        = 2;
const int margin     = 2;

LRESULT KEmfScope::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
			{
				HMENU hSys  = GetSystemMenu(hWnd, FALSE);
				RECT  rect;
	
				m_hWnd    = hWnd;
				hMainMenu = LoadMenu(hinst_EmfScope, MAKEINTRESOURCE(IDR_MAIN));

				InsertMenu(hSys, SC_CLOSE, MF_BYCOMMAND | MF_SEPARATOR, 0, 0);

				ToolBar = new KToolBar(hinst_EmfScope, hWnd);
				
				GetClientRect(hWnd, &rect);
				default_w = rect.right - rect.left;
				default_h = rect.bottom - rect.top;

				Canvas  = new KCanvasWindow (hinst_EmfScope, hWnd, 
						   					2, 2+ToolBar->n_height+2, 
											unmapx(default_w-mapx(2*margin)), 
											unmapy(default_h-mapy(2*margin-gap-ToolBar->n_height)));
				
				// let toolbar communicate scale to Canvas window
				ToolBar->SetCanvas(Canvas);

				GetWindowRect(hWnd, &rect);
				
				extra_w = rect.right - rect.left - default_w;
				extra_h = rect.bottom - rect.top - default_h;

				SetWindowPos(hWnd, NULL, 0, 0,
							 extra_w + mapx(ToolBar->n_width  + margin*2),
							 extra_h + mapy(ToolBar->n_height + margin*2),
							 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
			}
			break;

		
		case WM_SIZE:
			if (Canvas)
			{
				RECT rect;

				GetClientRect(hWnd, &rect);

				SetWindowPos(Canvas->m_hWnd, NULL, 0, 0,
							 rect.right -rect.left-mapx(2*2), 
							 rect.bottom-rect.top-mapy(2*3+ToolBar->n_height),
							 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
			}							
			break;

		
		case WM_DESTROY:
			DestroyMenu(hMainMenu);
			PostQuitMessage(0);
			break;
		
		case WM_SPOOLERSTATUS:
			{
				char temp[32];
				wsprintf(temp, "WM_SPOOLERSTATUS %d %d", wParam, lParam);
				OutputDebugString(temp);
				
    			DWORD pcbNeeded;
				DWORD pcReturned;
				BOOL b= EnumJobs(NULL, 0, 10, 1, NULL, 0, &pcbNeeded, &pcReturned);
			}
			break;

		case WM_CLOSE:
			ToolBar->SaveSettings();
			UnlinkClientServer();
			// break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}


KEmfScope::~KEmfScope(void)
{
	if ( ToolBar )
	{
		delete ToolBar;   
		ToolBar = NULL;
	}

	if ( Canvas )
	{
		delete Canvas;	  
		Canvas  = NULL;
	}
}


HWND KEmfScope::CreateMainWindow(HINSTANCE hInst)
{
	hinst_EmfScope = hInst;

	Createwindow("EmfScope",
                  IDI_COLIVE,
                  0,
                  LoadStringTemp(IDS_APPTITLE), 
		          WS_VISIBLE | WS_OVERLAPPEDWINDOW,
				  CW_USEDEFAULT, 0,
				  CW_USEDEFAULT, 0, 
				  NULL,
                  hInst,
                  NULL, 0, GetSysColorBrush(COLOR_MENU));

	if (!InstallSpehon32(hInst))
		return NULL;

	return m_hWnd;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpszCmdLine, int nCmdShow)
{
	InitCommonControls();

	if ( !EmfScope.CreateMainWindow(hInstance) )
		return FALSE;

	return EmfScope.MessageLoop(SW_SHOWNORMAL);
}
