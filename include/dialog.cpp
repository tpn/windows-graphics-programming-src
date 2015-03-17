//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dialog.cpp						                                     //
//  Description: KDialog class                                                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <prsht.h>

#include "dialog.h"


BOOL CALLBACK KDialog::DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    KDialog * pDialog;
        
    if ( uMsg==WM_INITDIALOG )
    {
        PROPSHEETPAGE       * pPSP = (PROPSHEETPAGE *) lParam;
        _DialogCreateRecord * pDCR = (_DialogCreateRecord *) lParam;

        // check for property sheet page, PROPSHEETPAGE struct should be passed
        if ( ! IsBadReadPtr(pPSP, sizeof(PROPSHEETPAGE)) && (pPSP->dwSize == sizeof(PROPSHEETPAGE)) )
        {
            pDialog = (KDialog *) pPSP->lParam;

            assert( ! IsBadReadPtr(pDialog, sizeof(KDialog)) );
            SetWindowLong(hWnd, GWL_USERDATA, (LONG) pDialog);
        }
		// check fir our dialog box creation call, _DialogCreateRecord struct should be passed
        else if ( ! IsBadReadPtr(pDCR, sizeof(_DialogCreateRecord)) && (pDCR->marker==CDIALOG_MARKER) )
        {
            pDialog = pDCR->pDialog;

            assert( ! IsBadReadPtr(pDialog, sizeof(KDialog)) );
            SetWindowLong(hWnd, GWL_USERDATA, (LONG) pDialog);
        }
		else
			assert(false);
    }       
    else
	{
		// For messages sent after WM_INITDIALOG, pDialog should point to a KDialog instance.
		// But there are 2 messages known to be sent before it: WM_SETFONT, WM_NOTIFYFORMAT. WM_QUERYUISTATE
		// Our implementation can't pass it to the right KDialog::DlgProc.

#ifndef WM_QUERYUISTATE
#define WM_QUERYUISTATE 0x129
#endif

		switch ( uMsg )
		{
			case WM_SETFONT:
			case WM_NOTIFYFORMAT:
			case WM_QUERYUISTATE:
			case 131:
				return FALSE;
		}

        pDialog = (KDialog *) GetWindowLong(hWnd, GWL_USERDATA);

		assert( ! IsBadReadPtr(pDialog, sizeof(KDialog)) );
	}

    if ( pDialog )
        return pDialog->DlgProc(hWnd, uMsg, wParam, lParam); // dispatch to KDialog virtual function
    else
        return FALSE;
}