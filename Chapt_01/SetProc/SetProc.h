//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : setproc.h						                                     //
//  Description: API hooking through import/export table, Chapter 1                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "..\..\include\pehack.h"

int WINAPI MyMessageBoxA(HWND hWnd, LPCSTR pText, LPCSTR pCaption, 
						 UINT uType)
{
	WCHAR wText[MAX_PATH];
	WCHAR wCaption[MAX_PATH];

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pText, 
		-1, wText,    MAX_PATH);
	wcscat(wText, L" - intercepted");

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pCaption,
		-1, wCaption, MAX_PATH);
	wcscat(wCaption, L" - intercepted");

	return MessageBoxW(hWnd, wText, wCaption, uType);
}
 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	{
		KPEFile pe(hInstance);
	
		pe.SetImportAddress("user32.dll", "MessageBoxA", (FARPROC) MyMessageBoxA);

		MessageBoxA(NULL, "Test", "SetImportAddress", MB_OK);
	}

	HMODULE hUser = GetModuleHandle("user32.dll");

	KPEFile user32(hUser);

	FARPROC oldproc = GetProcAddress(hUser, "MessageBoxA");

	user32.SetExportAddress("MessageBoxA", (FARPROC) MyMessageBoxA);

	FARPROC newproc = GetProcAddress(hUser, "MessageBoxA");

	char temp[64];
	wsprintf(temp, "GetProcAddress(MessageBoxA)\n"
		"changes from %x to %x", oldproc, newproc);
	MessageBoxA(NULL, temp, "SetExportAddress", MB_OK);

	return 0;
}