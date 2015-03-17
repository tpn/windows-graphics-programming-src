//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : filedialog.cpp						                                 //
//  Description: File common dialog box class                                        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <commdlg.h>

#include "filedialog.h"

// Example: "Bitmaps" "bmp"
// Example: "Bitmaps|Enhanced Metafiles" "bmp|emf"
void KFileDialog::SetupOFN(OPENFILENAME & ofn, TCHAR Filter[], HWND hWnd, const TCHAR * pExtension, const TCHAR * pClass)
{
	memset(& ofn, 0, sizeof(ofn));

	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= hWnd;
	ofn.lpstrFilter		= Filter; // _T("Bitmaps (*.bmp)\0*.bmp\0\0");
	ofn.lpstrFile		= m_FileName;
	ofn.nMaxFile		= MAX_PATH;
	ofn.lpstrFileTitle	= m_TitleName;
	ofn.nMaxFileTitle	= MAX_PATH;
	ofn.lpstrDefExt		= NULL; // _T("bmp");
	ofn.nFilterIndex	= 1;

	int len = 0;

	while ( (pExtension!=NULL) && (pClass!=NULL) )
	{
		TCHAR klass[MAX_PATH];
		TCHAR ext  [MAX_PATH];

		_tcscpy(klass, pClass);
		TCHAR * p = _tcschr(klass, '|');
		if ( p ) 
		{
			* p = 0;
			pClass = _tcschr(pClass, '|') + 1;
		}
		else 
			pClass = NULL;

		_tcscpy(ext, pExtension);
		p = _tcschr(ext, '|');
		if ( p ) 
		{
			* p = 0;
			pExtension = _tcschr(pExtension, '|') + 1;
		}
		else
			pExtension = NULL;

		Filter += wsprintf(Filter, _T("%s (*.%s)%c*."), klass, ext, 0);
		
		if ( ofn.lpstrDefExt==NULL )
			ofn.lpstrDefExt = Filter;

		Filter += wsprintf(Filter, _T("%s%c"), ext, 0);
	}

	m_FileName[0] = 0;
}


BOOL KFileDialog::GetOpenFileName(HWND hWnd, const TCHAR * pExtension, const TCHAR * pClass)
{
	OPENFILENAME ofn;
	TCHAR Filter[MAX_PATH];

	SetupOFN(ofn, Filter, hWnd, pExtension, pClass);

	ofn.Flags	= OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			
	return ::GetOpenFileName(&ofn);
}


BOOL KFileDialog::GetSaveFileName(HWND hWnd, const TCHAR * pExtension, const TCHAR * pClass)
{
	OPENFILENAME ofn;
	TCHAR Filter[MAX_PATH];

	SetupOFN(ofn, Filter, hWnd, pExtension, pClass);

	ofn.Flags  = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	
	return ::GetSaveFileName(&ofn);
}
