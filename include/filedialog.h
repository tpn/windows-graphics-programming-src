#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : filedialog.h				                                         //
//  Description: File common dialog box wrapper                                      //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KFileDialog
{
	void SetupOFN(OPENFILENAME & ofn, TCHAR Filter[], HWND hWnd, 
		const TCHAR * pExtension, const TCHAR * pClass);

public:
	TCHAR	m_FileName[MAX_PATH];
	TCHAR	m_TitleName[MAX_PATH];
	
	// "bmp" "Bitmap Files"
	BOOL GetOpenFileName(HWND hWnd, const TCHAR * pExtension, const TCHAR * pClass);
	BOOL GetSaveFileName(HWND hWnd, const TCHAR * pExtension, const TCHAR * pClass);
};

