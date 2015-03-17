//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : scrambler.cpp					                                     //
//  Description: Screen scrambler demo program, Chapter 10                           //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdlib.h>

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, PSTR, int)
{
	HDC hDC = GetDC(NULL);
	
	int width  = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);

	for (int i=0; i<2000; i++)
	{
		HBRUSH hBrush = CreateSolidBrush(RGB(rand()%256, rand()%256, rand()%256));
		SelectObject(hDC, hBrush);

		BOOL rslt = StretchBlt(hDC, rand() % width, rand() % height, -64, -64, 
							   hDC, rand() % width, rand() % height, 32, 32, (rand() % 256) << 16);
		SelectObject(hDC, GetStockObject(WHITE_BRUSH));
		DeleteObject(hBrush);
		Sleep(1);
	}

	ReleaseDC(NULL, hDC);

	RedrawWindow(NULL, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);
	return 0;
}