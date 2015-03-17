//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : textperiscope.cpp	                                                 //
//  Description: Test program for periscope.sys kernel mode driver, Chapter 3        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winioctl.h>
#include <assert.h>
#include <tchar.h>

#include "device.h"
#include "..\Periscope\\Periscope.h"
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
	param.lpszIcon    = MAKEINTRESOURCE(IDI_GRAPH);

	return MessageBoxIndirect(&param);
}

class KPeriscopeClient : public KDevice
{
public:
	KPeriscopeClient(const TCHAR * DeviceName) : KDevice(DeviceName)
	{
	}

	bool Read(void * dst, const void * src, unsigned len);
};


bool KPeriscopeClient::Read(void * dst, const void * src, unsigned len)
{
	unsigned      cmd[2] = { (unsigned) src, len };
	unsigned long dwRead;
   
	return IoControl(IOCTL_PERISCOPE, cmd, sizeof(cmd), dst, len, &dwRead) && (dwRead==len);
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


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	KPeriscopeClient scope("PeriScope");

	TCHAR fullname[MAX_PATH];
	GetFullName(hInst, "periscope.sys", fullname);
	
	if ( scope.Load(fullname)==ERROR_SUCCESS )
	{
		unsigned char buf[256];

		scope.Read(buf, (void *) 0xa000004E, sizeof(buf));
		scope.Close();

		MyMessageBox(NULL, (char *) buf, "Mem[0xa000004e]", MB_OK);
	}
	else
		MyMessageBox(NULL, fullname, "Unable to load kernel mode driver", MB_OK);

	return 0;
}