//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : syscall.cpp					                                     //
//  Description: Listing system service calls, and call routines, Chapter 2          //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <imagehlp.h> 
#include <richedit.h>
#include <stdio.h>
#include <commctrl.h>

#include "resource.h"
#include "..\..\include\win.h"
#include "..\..\include\ImageModule.h"


class KMainWindow : public KWindow
{
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HINSTANCE m_hInst;

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KWindow::GetWndClassEx(wc);
		wc.hIcon  = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH));
	}

public:

	HWND	  m_Output;

	KMainWindow(HINSTANCE hInst)
	{
		m_hInst   = hInst;
		m_Output  = NULL;
	}

	void ListSysCalls(char * module, char * extension, bool bForPogy=false);
	void SysCallTable(char * module, char * extension, const char * tablename, unsigned base);
};


void KMainWindow::ListSysCalls(char * module, char * extension, bool bForPogy)
{
    KImageModule mod(m_Output);

	mod.LoadSystemModule(module, extension);
	mod.EnumerateSymbols(bForPogy);
}


void KMainWindow::SysCallTable(char * module, char * extension, const char * tablename, unsigned base)
{
	KImageModule mod(m_Output);

	mod.LoadSystemModule(module, extension);
	mod.ShowSysCallTable(tablename, base);
}


LRESULT KMainWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
			{
				RECT rect;

				m_hWnd  = hWnd;
			
				GetClientRect(m_hWnd, & rect);

				m_Output = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, NULL, 
					WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL |
					ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOVSCROLL | ES_WANTRETURN,
					0, 0, rect.right, rect.bottom,
					m_hWnd, NULL, m_hInst, NULL);

				assert(m_Output);
				SendMessage(m_Output, WM_SETFONT, (WPARAM) GetStockObject(ANSI_FIXED_FONT), FALSE);;
			}
			return 0;

		case WM_SIZE:
			{
				int width	   = LOWORD(lParam);
				int height	   = HIWORD(lParam);

				MoveWindow(m_Output, 0, 0, width, height, TRUE);
			}
			return 0;

		case WM_PAINT:
			{
				PAINTSTRUCT ps; 
                
				BeginPaint(m_hWnd, &ps);
				EndPaint(m_hWnd, &ps);
			}
			return 0;

		case WM_COMMAND:

			switch ( LOWORD(wParam) )
			{
				case ID_EXIT:
					DestroyWindow(hWnd);
					return 0;

				case ID_GDI_POGY:
					ListSysCalls(_T("gdi32.dll"), _T("dll"), true);
					return 0;

				case ID_GDI_SYSCALL:
					ListSysCalls(_T("gdi32.dll"), _T("dll"));
					return 0;

				case ID_USER_SYSCALL:
					ListSysCalls(_T("user32.dll"), _T("dll"));
					return 0;

				case ID_NTDLL_SYSCALL:
					ListSysCalls(_T("ntdll.dll"), _T("dll"));
					return 0;

				case ID_WIN32K_SYSCALLTAB:
					SysCallTable(_T("win32k.sys"), _T("sys"), _T("W32pServiceTable"), 0x1000);
					return 0;

				case ID_NTOSKRNL_SYSCALLTAB:
					SysCallTable(_T("ntoskrnl.exe"), _T("exe"), _T("KiServiceTable"), 0);
					return 0;

			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmd, int nShow)
{
    if ( LoadLibrary(_T("RICHED20.DLL"))==NULL )
	{
		MessageBox(NULL, _T("Unable to load RICHED20.DLL"), _T("SysCall"), MB_OK);
		return -1;
	}
	
	KMainWindow win(hInst);
    
    win.CreateEx(0, _T("SysCall"), _T("SysCall"),
	             WS_OVERLAPPEDWINDOW,
	             CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	             NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);
        
    win.ShowWindow(nShow);
    win.UpdateWindow();
    win.MessageLoop();

	return 0;
}

