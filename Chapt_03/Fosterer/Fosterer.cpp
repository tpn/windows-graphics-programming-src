//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : fosterer.cpp			                                             //
//  Description: Hosting program for WinDBG debugger extension, Chapter 3            //
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
#include <wdbgexts.h>

#include "Extension.h"
#include "Host.h"
#include "ImageModule.h"
#include "win.h"
#include "resource.h"
#include "memdump.h"

class KLogWindow : public KWindow
{
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HINSTANCE m_hInst;
	HWND      m_Output;

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KWindow::GetWndClassEx(wc);
		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_SPY));
	}

public:
	KLogWindow(HINSTANCE hInst)
	{
		m_hInst   = hInst;
	}

	void Dump(const char * cmd);
};


void KLogWindow::Dump(const char * cmd)
{
	if ( cmd==NULL )
		return;

	//	ReadKRam(%08x, %d)
	//  ReadURam(%x, %08x, %d)

	unsigned addr;
	unsigned process;
	int		 length, len;
	char   * buffer;
	const char * p;

	if ( strncmp(cmd, "ReadKRam", 8)==0 )
	{
		p = strchr(cmd, '(') + 1; sscanf(p, "%x", & addr);
		p = strchr(cmd, ',') + 1; sscanf(p, "%d", & length);

		assert( addr>=0x80000000 );
		assert( length>0 );

		len = (length+15)/16*16;
		buffer = new char[len];
		assert( buffer );
		memset(buffer, 0, len);
		
		theHost.pScope->Read(buffer, (const void *) addr, length);
	}
	else if ( strncmp(cmd, "ReadURam", 8)==0 )
	{
		p = strchr(cmd, '(') + 1; sscanf(p, "%x", & process);
		p = strchr(p,   ',') + 1; sscanf(p, "%x", & addr);
		p = strchr(p,   ',') + 1; sscanf(p, "%d", & length);

		len = (length+15)/16*16;

		buffer = new char[len];
		assert(buffer);
		memset(buffer, 0, len);
		DWORD dwRead;
		ReadProcessMemory((HANDLE) process, (const void *) addr, buffer, length, & dwRead);
	}

	if ( buffer )
	{
		KMemDump dump;

		dump.OpenDump();
		dump.Dump((unsigned char *)buffer, addr - (unsigned) buffer, length, 4);
		dump.CloseDump();

//		if ( length==1548 )
//		{
//			TestDCObj(buffer);
//		}
		
		delete [] buffer;
	}
}


LRESULT KLogWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
				m_hWnd, (HMENU) 101, m_hInst, NULL);

			SendMessage(m_Output, EM_LIMITTEXT, 1024 * 1024, 0);
			SendMessage(m_Output, WM_SETFONT, (WPARAM) GetStockObject(ANSI_FIXED_FONT), FALSE);;

			theHost.hwndLog = m_Output;
		}
		return 0;

		case WM_PARENTNOTIFY:
			if ( LOWORD(wParam)==WM_RBUTTONDOWN )
			{
				char buffer[MAX_PATH];

				buffer[0] =0 ;
				SendMessage(m_Output, EM_GETSELTEXT, 0, (LPARAM) & buffer);
				Dump(buffer);
			}
			return 0;
		
		case WM_SIZE:
		{
			int width	   = LOWORD(lParam);
			int height	   = HIWORD(lParam);
	
			MoveWindow(m_Output, 0, 0, width, height, TRUE);
		}
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
};


class KMainWindow : public KWindow
{
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND	  m_Output;
	HWND	  m_Command;
	HWND	  m_Action;
	HWND	  m_Pid;

	DWORD	  m_curPid;
	HANDLE	  m_curProcess;

	HINSTANCE m_hInst;

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KWindow::GetWndClassEx(wc);
		wc.hbrBackground = GetSysColorBrush(COLOR_MENU);
		wc.hIcon         = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_SPY));
	}

public:

	KGDIExtension gdi;
	
	KMainWindow(HINSTANCE hInst)
	{
		m_hInst   = hInst;
		
		m_Output  = NULL;
		m_Command = NULL;
		m_Action  = NULL;
		m_Pid     = NULL;
	}

	HANDLE GetProcess(void);
};


HANDLE KMainWindow::GetProcess(void)
{
	char sPid[64];
	DWORD nPid;

	GetWindowText(m_Pid, sPid, sizeof(sPid));
	sscanf(sPid, "%x", & nPid);

	if ( nPid != m_curPid )
	{
		HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, nPid);

		if ( hProcess == NULL )
		{
			wsprintf(sPid, "Unable to open process %x", nPid);
			MyMessageBox(NULL, sPid, "Fosterer", MB_OK, IDI_SPY);
		}
		else
		{
			CloseHandle(m_curProcess);

			m_curPid = nPid;
			m_curProcess = hProcess;
		}
	}

	return m_curProcess;
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

				int textheight = GetSystemMetrics(SM_CYSIZE) * 13 / 10;

				m_curPid     = GetCurrentProcessId();
				m_curProcess = GetCurrentProcess();

				char id[32];
				wsprintf(id, "%x", m_curPid);
				
				m_Pid  = CreateWindowEx(0, "EDIT", id,
					WS_CHILD | WS_VISIBLE | WS_BORDER,
					8, 0, 80, textheight,
					m_hWnd, NULL, m_hInst, NULL);

				m_Action  = CreateWindowEx(0, "BUTTON", "Do",
					WS_CHILD | WS_VISIBLE | WS_BORDER,
					100, 0, 80, textheight,
					m_hWnd, (HMENU) IDOK, m_hInst, NULL);

				m_Command = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", NULL, 
					WS_CHILD | WS_VISIBLE | WS_BORDER, 
					200, 0, rect.right - 200, textheight, 
					m_hWnd, NULL, m_hInst, NULL);
				assert(m_Command);

				m_Output = CreateWindowEx(WS_EX_CLIENTEDGE, RICHEDIT_CLASS, NULL, 
					WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL |
					ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOVSCROLL | ES_WANTRETURN,
					0, textheight + 1, rect.right, rect.bottom - textheight - 1,
					m_hWnd, NULL, m_hInst, NULL);

				assert(m_Output);
				SendMessage(m_Output, WM_SETFONT, (WPARAM) GetStockObject(ANSI_FIXED_FONT), FALSE);;

				theHost.hwndOutput = m_Output; 
			}
			return 0;

		case WM_SIZE:
			{
				int width	   = LOWORD(lParam);
				int height	   = HIWORD(lParam);
				int textheight = GetSystemMetrics(SM_CYSIZE) * 13 / 10;

				MoveWindow(m_Pid,      10, 0,            80,          textheight, FALSE);
				MoveWindow(m_Action,  100, 0,            80,          textheight, FALSE);
				MoveWindow(m_Command, 200, 0,            width - 200, textheight, TRUE);
				MoveWindow(m_Output,  0,   textheight+1, width,       height - textheight-1, TRUE);
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
				case IDOK:
					{
						char text[MAX_PATH];

						GetWindowText(m_Command, text, sizeof(text));

						gdi.SetProcess(GetProcess());
						gdi.Do(text);
					}
					return 0;

				case ID_EXIT:
					DestroyWindow(hWnd);
					return 0;

				case ID_HELP_GDIKDX:
					gdi.Do("help");
					return 0;

				case ID_HELP_DUMPHMGR:
					gdi.Do("dumphmgr -?");
					return 0;

				case ID_HELP_DUMPOBJ:
					gdi.Do("dumpobj -?");
					return 0;

				case ID_CMD_DUMPHMGR:
					gdi.Do("dumphmgr");
					return 0;

				case ID_CMD_DUMPOBJ:
					gdi.Do("dumpobj");
					return 0;

				case ID_CMD_DPDEV:
					gdi.Do("dpdev");
					return 0;

				case ID_CMD_DLDEV:
					gdi.Do("dldev");
					return 0;

				case ID_CMD_DGDEV:
					gdi.Do("dgdev");
					return 0;
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


// 8x8 24-bit DIB
typedef struct
{
	BITMAPINFOHEADER bmiHeader; 
	unsigned char    bmiBits[8*3*8];
}  CSmallDib;


// 8x8 24 bit DIB
const CSmallDib dib = 
{ 
	{ sizeof(BITMAPINFOHEADER), 8, 8, 1, 24,  BI_RGB, 8*3*8, 0, 0, 0, 0 }, 

    {   0,0,0, 1,1,1, 2,2,2, 3,3,3, 4,4,4, 5,5,5, 6,6,6, 7,7,7,
        1,1,1, 2,2,2, 3,3,3, 4,4,4, 5,5,5, 6,6,6, 7,7,7, 0,0,0, 
        2,2,2, 3,3,3, 4,4,4, 5,5,5, 6,6,6, 7,7,7, 0,0,0, 1,1,1, 
        3,3,3, 4,4,4, 5,5,5, 6,6,6, 7,7,7, 0,0,0, 1,1,1, 2,2,2, 
        4,4,4, 5,5,5, 6,6,6, 7,7,7, 0,0,0, 1,1,1, 2,2,2, 3,3,3, 
        5,5,5, 6,6,6, 7,7,7, 0,0,0, 1,1,1, 2,2,2, 3,3,3, 4,4,4, 
        6,6,6, 7,7,7, 0,0,0, 1,1,1, 2,2,2, 3,3,3, 4,4,4, 5,5,5, 
        7,7,7, 0,0,0, 1,1,1, 2,2,2, 3,3,3, 4,4,4, 5,5,5, 6,6,6 
	}
};


long FIXARRAY[] =
{
          0x00000630, 0xfffffff8, 
          0x00000630, 0xfffffc89, 
          0x00000368, 0xfffff9c0, 
          0xfffffff8, 0xfffff9c0, 
          0xfffffc88, 0xfffff9c0, 
          0xfffff9c0, 0xfffffc89, 
          0xfffff9c0, 0xfffffff8, 
          0xfffff9c0, 0x00000367, 
          0xfffffc88, 0x00000630, 
          0xfffffff8, 0x00000630, 
          0x00000368, 0x00000630, 
          0x00000630, 0x00000367, 
          0x00000630, 0xfffffff8 
};


const POINT Points[3] = { {200, 50}, {250, 150}, {300, 50} };


void GDITest(void)
{
	ENUMLOGFONTEXW enumlog;
	EXTLOGFONTW extlog;
	
	memset(& enumlog, 0, sizeof(enumlog));
	memset(& extlog, 0, sizeof(extlog));

	int j = sizeof(enumlog);
	int k = sizeof(extlog);
	
	for (int i=0; i<sizeof(FIXARRAY)/sizeof(long); i++)
	{
		char t[20];

		long v = FIXARRAY[i];

		if ( v >= 0)
			t[0] = '+';
		else
		{
			t[0] = '-';
			v = - v;
		}

		sprintf(t+1, "%d + %d/16\n", v/16, v%16);
		OutputDebugString(t);
	}

	HDC hDC = GetDC(NULL);
	
	HBITMAP bmp1 = CreateBitmap(100, 100, 1, 1, NULL);
	HBITMAP bmp2 = CreateBitmap(100, 100, 1, 4, NULL);
	HBITMAP bmp3 = CreateBitmap(100, 100, 1, 8, NULL);
	HBITMAP bmp4 = CreateBitmap(100, 100, 3, 8, NULL);
	HBITMAP bmp5 = CreateBitmap(100, 100, 1, 24, NULL);

	void * dibbits;

	HBITMAP bmp6 = CreateDIBSection(hDC, (BITMAPINFO *) & dib, 
					    DIB_RGB_COLORS, & dibbits, NULL, 0);

	BeginPath(hDC);
	MoveToEx(hDC, 100, 100, NULL);
	LineTo(hDC, 150, 150);
	PolyBezierTo(hDC, & Points[0], 3);
	CloseFigure(hDC);
	Ellipse(hDC, -100, -100, 100, 100);
	EndPath(hDC);
	POINT points[20];
	BYTE  types[20];

	int n = GetPath(hDC, NULL, NULL, 0);
	n = GetPath(hDC, points, types, min(n, 20));

	SetBitmapDimensionEx(bmp1, 10000, 10000, NULL);
}

void GetFullName(HINSTANCE hInstance, const TCHAR * module, TCHAR fullname[])
{
	GetModuleFileName(hInstance, fullname, MAX_PATH);

	TCHAR * pName = fullname;

	while ( _tcschr(pName, ':') || _tcschr(pName, '\\') )
		if ( _tcschr(pName, ':') )
			pName = _tcschr(pName, ':') + 1;
		else
			pName = _tcschr(pName, '\\') + 1;

	if ( pName )
		_tcscpy(pName, module);
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmd, int nShow)
{
    if ( LoadLibrary("RICHED20.DLL")==NULL )
	{
		MyMessageBox(NULL, "Unable to load RICHED20.DLL", "Fosterer", MB_OK, IDI_SPY);
		return -1;
	}
	
	KMainWindow win(hInst);
	KLogWindow  log(hInst);
    
	log.CreateEx(0, "LogWindow", "LogWindow",
	             WS_OVERLAPPEDWINDOW,
	             CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	             NULL, NULL, hInst);
	log.ShowWindow(nShow);
    log.UpdateWindow();

    win.CreateEx(0, "Fosterer", "Fosterer",
	             WS_OVERLAPPEDWINDOW,
	             CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	             NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MAIN)), hInst);
        
    win.ShowWindow(nShow);
    win.UpdateWindow();

    KPeriscopeClient scope("PeriScope");
    KImageModule	 win32k;

	TCHAR fullname[MAX_PATH];
	GetFullName(hInst, "periscope.sys", fullname);

    if ( scope.Load(fullname)==ERROR_SUCCESS )
	{
		theHost.ExtOutput("Periscope loaded\n");

		try
		{
			char SysDir[MAX_PATH];

			GetSystemDirectory(SysDir, sizeof(SysDir)); // c:\winnt\system32

			char * p = strchr(SysDir, '\\');   // first '\\'
			while ( p && strchr(p+1, '\\') )   // last '\\'	
				p = strchr(p+1, '\\');

			if ( p )						   // c:\winnt
				* p = NULL;

			strcat(SysDir, "\\symbols\\sys");

			if ( win32k.Load("win32k.sys", SysDir) )
			{
				theHost.pWin32k = & win32k;
				theHost.pScope  = & scope;
	
				if ( ! win.gdi.Load(& ExtensionAPI, "gdikdx.dll") )
					MyMessageBox(NULL, "Unable to load GDI kernel extension GDIKDX.DLL.\n", "Fosterer", MB_OK, IDI_SPY);
			}
			else
				MyMessageBox(NULL, "Unable to load win32k.sys debug information", "Fosterer", MB_OK, IDI_SPY);
		}
		catch (...)
		{
			OutputDebugString("Exception in WinMain");
		}
	}
	else
		MyMessageBox(NULL, fullname, "Unable to load kernel mode driver", MB_OK, IDI_SPY);

    win.MessageLoop();

	DestroyWindow(log.m_hWnd);

	scope.Close();

	return 0;
}

