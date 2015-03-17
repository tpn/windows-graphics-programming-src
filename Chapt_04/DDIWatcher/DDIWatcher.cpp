//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : ddiwatcher.cpp			                                             //
//  Description: Testing program for DDI spy, Chapter 4                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define NOCRYPT

#include <windows.h>
#include <winioctl.h>
#include <assert.h>
#include <tchar.h>

#include "..\..\include\device.h"
#include "..\DDISpy\\DDISpy.h"
#include "resource.h"

int MyMessageBox(HWND hWnd, const TCHAR * text, const TCHAR * caption, DWORD style)
{
	MSGBOXPARAMS param;

	memset(& param, 0, sizeof(param));
	param.cbSize	  = sizeof(param);
	param.hwndOwner   = hWnd;
	param.hInstance   = GetModuleHandle(NULL);
	param.lpszText    = text;
	param.lpszCaption = caption;
	param.dwStyle     = style | MB_USERICON;
	param.lpszIcon    = MAKEINTRESOURCE(IDI_SPY);

	return MessageBoxIndirect(&param);
}


BOOL ConfirmHack(const unsigned ddientry[], int nEntry)
{
	TCHAR temp[8192];

	temp[0] = 0;

	for (int i=0; i<nEntry; i++)
		if ( ddientry[i] !=0 )
			wsprintf(temp + _tcslen(temp), "DDI[%d]=0x%x\r\n", i, ddientry[i]);

	return MyMessageBox(NULL, temp, "Really hack into DDI?", MB_OKCANCEL) == IDOK;
}
	

class KDDIWatcher : public KDevice
{
public:
	KDDIWatcher(const TCHAR * DeviceName) : KDevice(DeviceName)
	{
	}

	bool Read(void * dst, const void * src, unsigned len);
};


bool KDDIWatcher::Read(void * dst, const void * src, unsigned len)
{
	unsigned      cmd[2] = { (unsigned) src, len };
	unsigned long dwRead;
   
	return IoControl(DDISPY_READ, cmd, sizeof(cmd), dst, len, &dwRead) && (dwRead==len);
}


BOOL Hack(KDDIWatcher & scope)
{
	unsigned buf[8192/4];

	HDC  hDC  = GetDC(NULL);

	typedef unsigned (CALLBACK * Proc0) (void);

	Proc0 pGdiQueryTable = (Proc0) GetProcAddress(GetModuleHandle("GDI32.DLL"), "GdiQueryTable");

	unsigned pGDITable = 0;

	if ( pGdiQueryTable )
		pGDITable = pGdiQueryTable();

	if ( (pGDITable==0) || (pGDITable % 4096) )
	{
		MyMessageBox(NULL, _T("Unable to locate GDI handle table"), _T("DDIWatcher"), MB_OK);
		return FALSE;
	}

	unsigned * addr = (unsigned *) (pGDITable + ((unsigned) hDC & 0xFFFF) * 16);
	
	addr = (unsigned *) addr[0];

	scope.Read(buf, addr, 40);

#ifdef NT4
	unsigned pdev    = buf[5];
	unsigned fntable = pdev + 0x3F4;
#else
	unsigned pdev    = buf[7];
	unsigned fntable = pdev + 0xB8C;
#endif

	scope.Read(buf, (void *) fntable, 89 * 4);

	if ( ! ConfirmHack(buf, 89) )
		return FALSE;

	unsigned      cmd[2] = { fntable, 25 };
	unsigned long dwRead;
   
	scope.IoControl(DDISPY_START, cmd, sizeof(cmd), buf, 100, &dwRead);

	TextOut(hDC, 10, 10, "Hello", 4);
	Rectangle(hDC, 0, 0, 100, 100);
	TextOut(hDC, -100, 10, "Why", 3);
	MoveToEx(hDC, 10, 20, NULL);
	LineTo(hDC, 100, 120);
	SetPixel(hDC, 0, 0, RGB(0xFF, 0, 0));
	GetPixel(hDC, 0, 0);
	Ellipse(hDC, 0, 0, 100, 100);

	scope.IoControl(DDISPY_END, cmd, sizeof(cmd), buf, 8, &dwRead);

	memset(buf, 0xff, sizeof(buf));
	cmd[1] = sizeof(buf);

	scope.IoControl(DDISPY_REPORT, cmd, sizeof(cmd), buf, sizeof(buf), &dwRead);

	ReleaseDC(NULL, hDC);

	MyMessageBox(NULL, (char *) buf, _T("DDI Watcher Report"), MB_OK);

	return TRUE;
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


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int)
{
	KDDIWatcher scope("DDISpy");

	TCHAR fullname[MAX_PATH];
	GetFullName(hInstance, "DDISpy.sys", fullname);

	if ( scope.Load(fullname)==ERROR_SUCCESS )
	{
		Hack(scope);
		
		scope.Close();
	}
	else
		MyMessageBox(NULL, fullname, "Unable to load kernel mode driver", MB_OK);

	return 0;
}