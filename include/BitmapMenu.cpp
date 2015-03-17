//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : bitmapmenu.cpp						                                 //
//  Description: Adding DDB to menu                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define NOCRYPT

#include <windows.h>
#include <assert.h>
#include <tchar.h>


#include "BitmapMenu.h"

// process command: 0: not processed, 1: no change, 2: changed
int KBitmapMenu::OnCommand(int nCmdId)
{
	int n = nCmdId - m_nFirstCommand;

	if ( (n<0) || (n>=m_nBitmap) )
		return 0;

	if ( n==m_nChecked )
		return 1;

	CheckMenuItem(m_hMenu, m_nFirstCommand + m_nChecked, MF_BYCOMMAND | MF_UNCHECKED);
	m_nChecked = n;
	CheckMenuItem(m_hMenu, m_nFirstCommand + m_nChecked, MF_BYCOMMAND | MF_CHECKED);

	return 2;
}


void KBitmapMenu::AddToMenu(HMENU hMenu, int nCount, HMODULE hModule, const int nID[], int nFirstCommand)
{
	m_hMenu    = hMenu;
	m_nBitmap  = nCount;
	m_nChecked = 0;
	m_nFirstCommand = nFirstCommand;

	for (int i=0; i<nCount; i++)
	{
		m_hBitmap[i] = LoadBitmap(hModule, MAKEINTRESOURCE(nID[i]));

		if ( m_hBitmap[i] )
			AppendMenu(hMenu, MF_BITMAP, nFirstCommand + i, (LPCTSTR) m_hBitmap[i]);
	}
	
	CheckMenuItem(m_hMenu, m_nChecked + nFirstCommand, MF_BYCOMMAND | MF_CHECKED);
}

