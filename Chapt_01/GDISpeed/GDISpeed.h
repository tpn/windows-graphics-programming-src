//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : gdispeed.h				                                             //
//  Description: GDI speed test demo, Chapter 1                                      //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>

#include "..\..\include\timer.h"
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
	param.lpszIcon    = MAKEINTRESOURCE(IDI_TU);

	return MessageBoxIndirect(&param);
}

const void * LoadRes(HMODULE hModule, int resid, const TCHAR * typ, int & size)
{
	HRSRC   hRes = FindResource(hModule, MAKEINTRESOURCE(resid), typ);

	if ( hRes==NULL )
		return NULL;

	HGLOBAL hGlb = LoadResource(hModule, hRes);

	if ( hGlb==NULL )
		return NULL;

	size = SizeofResource(hModule, hRes);

	return LockResource(hGlb);
}

const void * LoadOneIcon(HMODULE hModule, int resid, int width, int height, int & size)
{
	const void * pResource = LoadRes(hModule, resid, RT_GROUP_ICON, size); 

	if ( pResource==NULL )
		return NULL;
 
	int nID = LookupIconIdFromDirectoryEx((BYTE *) pResource, TRUE, width, height, LR_DEFAULTCOLOR); 

	return LoadRes(hModule, nID, RT_ICON, size);
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmd, int nShow)
{
    KTimer timer;
    TCHAR  mess[128];

    timer.Start(); 
	Sleep(1000); 
    unsigned cpuspeed10 = (unsigned)(timer.Stop()/100000);
    
    timer.Start(); 
    CreateSolidBrush(RGB(0xAA, 0xAA, 0xAA));
    unsigned time = (unsigned) timer.Stop();

    wsprintf(mess, _T("CPU speed       %d.%d mhz\n")
        _T("KTimer overhead %d clock cycles\n")
        _T("CreateSolidBrush %d clock cycles %d ns"),
        cpuspeed10 / 10,  cpuspeed10 % 10,
        (unsigned) timer.m_overhead,
        time, time * 10000 / cpuspeed10);
        
    MyMessageBox(NULL, mess, _T("How fast is GDI?"), MB_OK);

	int size;

	const BYTE * pData = (const BYTE *) LoadOneIcon(hInst, IDI_TU, 16, 16, size);

	if ( pData )
	{
		for (int i=0; i<size; i++)
		{
			char temp[16];

			wsprintf(temp, "0x%02x, ", pData[i]);
			OutputDebugString(temp);
			if ( (i % 16)==15 )
				OutputDebugString("\n");
		}

		OutputDebugString("\n");
	}

    return 0;
}
