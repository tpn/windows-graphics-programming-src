#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dialog.h						                                     //
//  Description: Dialog box wrapper class                                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KDialog
{
    virtual BOOL OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		m_hWnd = hWnd;
		
		return TRUE;
	}
	
	virtual BOOL OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		return FALSE;
	}
    
    virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
    	switch (uMsg)
	    {
		    case WM_INITDIALOG:
			    return OnInitDialog(hWnd, wParam, lParam);

		    case WM_COMMAND:
			    return OnCommand(hWnd, wParam, lParam);
	    }

	    return FALSE;
    }

protected:	
    
	typedef enum { CDIALOG_MARKER = 31415927 };
    
	typedef struct
    {
        unsigned marker;
        KDialog * pDialog;
    } _DialogCreateRecord;
    
    static BOOL CALLBACK DialogProc(HWND hWnd,  UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	
	HWND   m_hWnd;
	
	KDialog()
	{
		m_hWnd = NULL;
	}

    int Dialogbox(HINSTANCE hInstance, LPCTSTR lpTemplate, HWND hWndParent = NULL)
    {
        _DialogCreateRecord ccr;

        ccr.marker  = CDIALOG_MARKER;
        ccr.pDialog = this;

		return ::DialogBoxParam(hInstance, lpTemplate, hWndParent, DialogProc, (LPARAM) & ccr);
    }
};
