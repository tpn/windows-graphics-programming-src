//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : handles.cpp					                                     //
//  Description: GDI handles property page, Chapter 3                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT

#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <ddraw.h>
#include "resource.h"
#include "creator.h"
#include "handles.h"

const LPCTSTR Numbers[] = 
{
    _T("1"),
    _T("16"),
    _T("256"),
    _T("4096"),
	_T("65536")
};


const LPCTSTR ObjTypes[] =
{
    _T("Invalid Object"),
    _T("OBJ_PEN"),
    _T("OBJ_BRUSH"),
    _T("OBJ_DC"),
    _T("OBJ_METADC"),
    _T("OBJ_PAL"),
    _T("OBJ_FONT"),       
    _T("OBJ_BITMAP"),     
    _T("OBJ_REGION"),     
    _T("OBJ_METAFILE"),   
    _T("OBJ_MEMDC"),      
    _T("OBJ_EXTPEN"),     
    _T("OBJ_ENHMETADC"),  
    _T("OBJ_ENHMETAFILE")
};


LPCTSTR GetObjectTypeName(HGDIOBJ Handle)
{
	return ObjTypes[GetObjectType(Handle)];
}


BOOL KHGDIOBJ::Generate(HWND rslt, bool search)
{
    if (m_nCreator < 0) return FALSE;
    if (m_nCopy    < 0) return FALSE;

    int repeat = 1;
    for (int i=0; i<m_nCopy; i++)
        repeat *= 16;

    HGDIOBJ last = NULL;

    for (i=0; i<repeat; i++)
    {
        TCHAR temp[64];
		HGDIOBJ handle = CreateObject(m_nCreator, m_hDCScreen, i);

        if ( (handle == last) || (handle == (HGDIOBJ) 0xFFFFFFFF) )
            break;

        wsprintf(temp, "%08lx %s %d", handle, Commands[m_nCreator], i);
        SendMessage(rslt, LB_ADDSTRING, 0, (LPARAM) temp);
            
        last = handle;
    }

    if ( last)
	{
		// find all top level dialog box ( propertysheet )
		for (HWND hWnd = NULL; hWnd = FindWindowEx(NULL, hWnd, WC_DIALOG, NULL); )
		{
			// locate it's fisrt child ( propertypage 0 )
			HWND hChild = GetWindow(hWnd, GW_CHILD);

			// send a message to it
			if (hChild)
				SendMessage(hChild, WM_SHAREHANDLE, (WPARAM) last, GetCurrentProcessId());
		}
	}

    return TRUE;
}


HRESULT DirectDrawTest(HWND hWnd)
{
	LPDIRECTDRAW lpdd;

	HRESULT hr = DirectDrawCreate(NULL, & lpdd, NULL);

	DWORD i = GetCurrentThreadId();

	if ( hr == DD_OK )
	{
		LPDIRECTDRAW lpdd2;

		hr = DirectDrawCreate(NULL, & lpdd2, NULL);

		lpdd->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
	
		DDSURFACEDESC ddsd;
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	
		LPDIRECTDRAWSURFACE lpddsprimary;

		hr = lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL);
		if ( hr == DD_OK )
		{
			char mess[MAX_PATH];

			wsprintf(mess, 
				"DirectDraw object at %x, vtable at %x\n"
	            "DirectDraw surface object at %x, vtable at %x",
				lpdd, * (unsigned *) lpdd, 
				lpddsprimary, * (unsigned *) lpddsprimary);
			MessageBox(NULL, mess, "DirectDrawTest", MB_OK);

			lpddsprimary->Release();
		}

		lpdd->Release();
	}

	return hr;
}


BOOL KHGDIOBJ::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_hWnd		   = hWnd;
	m_hDCScreen    = GetDC(hWnd);
    WM_SHAREHANDLE = RegisterWindowMessage("WM_SHAREHANDLE");

    for (int i=0; i<nTotalCommands; i++)
		SendDlgItemMessage(hWnd, IDC_COMMAND, CB_ADDSTRING, 0, (LPARAM) Commands[i]);
    SendDlgItemMessage(hWnd, IDC_COMMAND, CB_SETCURSEL, 0, 0);

    for (i=0; i<sizeof(Numbers)/sizeof(Numbers[0]); i++)
		SendDlgItemMessage(hWnd, IDC_NUMBER, CB_ADDSTRING, 0, (LPARAM) Numbers[i]);
    SendDlgItemMessage(hWnd, IDC_NUMBER, CB_SETCURSEL, 0, 0);

	DirectDrawTest(hWnd);

	return TRUE;
}


BOOL KHGDIOBJ::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if (LOWORD(wParam) == IDC_CREATE)
    {
        m_nCreator = SendDlgItemMessage(hWnd, IDC_COMMAND, CB_GETCURSEL, 0, 0);
        m_nCopy    = SendDlgItemMessage(hWnd, IDC_NUMBER,  CB_GETCURSEL, 0, 0);

        Generate(GetDlgItem(hWnd, IDC_RESULT), FALSE);

        return TRUE;
    }

	return FALSE;
}


BOOL KHGDIOBJ::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if ( uMsg == WM_SHAREHANDLE )
    {
        TCHAR Mess[MAX_PATH];

        wsprintf(Mess, "Object created in process 0x%X, GetObjectType (0x%x) returns %d, %s",
                            lParam, wParam, GetObjectType((HGDIOBJ) wParam),
                            GetObjectTypeName((HGDIOBJ) wParam));

		if ( GetObjectType((HGDIOBJ) wParam)==OBJ_REGION )
			wsprintf(Mess+strlen(Mess), "rgnsize %d", GetRegionData((HRGN) wParam, 0, NULL));

        SetDlgItemText(hWnd, IDC_SHAREHANDLE, Mess);

        return TRUE;
    }

    switch (uMsg)
    {
	    case WM_INITDIALOG:
		    return OnInitDialog(hWnd, wParam, lParam);

	    case WM_COMMAND:
		    return OnCommand(hWnd, wParam, lParam);
    }
    
    return FALSE;
}

