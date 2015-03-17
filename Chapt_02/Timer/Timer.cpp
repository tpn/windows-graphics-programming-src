//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : timer.cpp						                                     //
//  Description: Compare timer accuracy, Chapter 2                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <mmsystem.h>

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

#pragma warning(disable : 4035 )

__int64 MyQueryCounter(void)
{
    _asm    _emit 0x0F
    _asm    _emit 0x31
}

__int64 MyQueryFrequency(void)
{
	__int64 start = MyQueryCounter();
	Sleep(1000);

	__int64 stop  = MyQueryCounter();

	return stop - start;
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	const int SAMPLES = 32;

	DWORD   t1[SAMPLES], t2[SAMPLES];
	__int64 t3[SAMPLES], t4[SAMPLES], freq3, freq4;

	TIMECAPS tc;
	timeGetDevCaps(&tc, sizeof(tc));
	timeBeginPeriod(1);
	
	QueryPerformanceFrequency((LARGE_INTEGER *) & freq3); freq3 /= 1000;
	freq4 = MyQueryFrequency();							  freq4 /= 1000;

	for (int i=0; i<SAMPLES; i++)
	{
		for (int j=0; j<200; j++)	// roughly 0.2 ms to 0.5ms delay
			DeleteObject(CreateSolidBrush(0));
		
		t1[i] = GetTickCount();
		t2[i] = timeGetTime();
		QueryPerformanceCounter((LARGE_INTEGER *) & t3[i]);
		t4[i] = MyQueryCounter();
	}
	timeEndPeriod(1);

	TCHAR buffer[1024];

	sprintf(buffer, _T("tick   mm %d Khz %5.1f Mhz %5.1f Mhz\n\n"), tc.wPeriodMin, freq3/1000.0, freq4/1000.0);
	
	for (i=0; i<SAMPLES; i++)
		wsprintf(buffer+ _tcslen(buffer), "%8d %8d %8d %8d\n", 
			(t1[i]-t1[0])*1000000, 
			(t2[i]-t2[0])*1000000, 
			(int)((t3[i]-t3[0])*1000000/freq3), (int)((t4[i]-t4[0])*1000000/freq4));

	MyMessageBox(NULL, buffer, "Timer Accuracy", MB_OK);

	return 0;
}