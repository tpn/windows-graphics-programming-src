#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : host.h					                                             //
//  Description: KHost class				                                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//


#include "device.h"
#include "..\Periscope\\Periscope.h"


class KPeriscopeClient : public KDevice
{
public:
	KPeriscopeClient(const TCHAR * DeviceName) : KDevice(DeviceName)
	{
	}

	ULONG Read(void * dst, const void * src, unsigned len);
};


class KImageModule;


class KHost
{
public:
	
	KImageModule	 * pWin32k;
	KPeriscopeClient * pScope;
	HWND			   hwndOutput;
	HWND			   hwndLog;
	HANDLE			   hProcess;

	KHost()
	{
		pWin32k    = NULL;
		pScope     = NULL;
		hwndOutput = NULL;
		hwndLog    = NULL;
		hProcess   = NULL;
	}

	void WndOutput(HWND hWnd, const char * format, va_list argptr);
	void Log(const char * format, ...);
	
	void     ExtOutput(const char * format, ...);
	unsigned ExtGetExpression(const char * expr);
	bool     ExtReadProcessMemory(const void * address, 
				unsigned * buffer, unsigned count, 
				unsigned long * bytesread);
};

extern KHost				 theHost;
extern WINDBG_EXTENSION_APIS ExtensionAPI;
