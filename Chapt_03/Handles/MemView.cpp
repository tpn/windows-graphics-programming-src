//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : memview.cpp				                                         //
//  Description: Memory dumper dialog box                                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <fstream.h>
#include <stdio.h>

#include "resource.h"
#include "dialog.h"

#include "MemView.h"

// CMemViewer class implementation

BOOL KMemViewer::OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_hWnd = hWnd;

	TCHAR temp[32];

	wsprintf(temp, "0x%08lx", m_start); SetDlgItemText(m_hWnd, IDC_REGIONSTART, temp);
	wsprintf(temp, "0x%lx", m_size);	SetDlgItemText(m_hWnd, IDC_REGIONSIZE,  temp);
		
	m_current  = m_start;
	m_pagesize = 128;

	m_hFont = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, "Courier New");
		
	SendDlgItemMessage(m_hWnd, IDC_DUMP, WM_SETFONT, (WPARAM) m_hFont, 0);
	
	assert( m_size % m_pagesize == 0);

    SendDlgItemMessage(m_hWnd, IDC_BYTE, BM_SETCHECK, BST_CHECKED, 0);
	DumpPage();

	return TRUE;
}
	

BOOL KMemViewer::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch ( LOWORD(wParam) )
	{
		case IDC_BYTE:
		case IDC_WORD:
		case IDC_DWORD:
			DumpPage();
			return TRUE;

		case IDC_DOWN:
			if ( m_current + m_pagesize >= m_start + m_size )
				return FALSE;
			m_current += m_pagesize;
			DumpPage();
			return TRUE;

		case IDC_UP:
			if ( m_current == m_start )
				return FALSE;
			m_current -= m_pagesize;
			DumpPage();
			return TRUE;

		case IDC_MEMORYDUMP:
            OpenDump();
			Dump(m_start, 0, m_size, UnitSize());
            CloseDump();
			return TRUE;

		case IDC_SEARCH:
			Search();
			return TRUE;

		case IDOK:
			EndDialog(m_hWnd, TRUE);
			return TRUE;
	}

	return FALSE;
}


int KMemViewer::UnitSize(void) const
{
    if ( SendDlgItemMessage(m_hWnd, IDC_BYTE, BM_GETCHECK, 0, 0) == BST_CHECKED)
        return sizeof(unsigned char);
    else if ( SendDlgItemMessage(m_hWnd, IDC_WORD, BM_GETCHECK, 0, 0) == BST_CHECKED)
        return sizeof(unsigned short);
    else
        return sizeof(unsigned long);
}


void KMemViewer::Search(void)
{
	unsigned char value[4];

	{
		TCHAR Target[32];
	
		GetDlgItemText(m_hWnd, IDC_TARGET, Target, sizeof(Target));

		sscanf(Target, "%x", value);
	}
	
	TCHAR TempPath[MAX_PATH], FileName[MAX_PATH];

	GetTempPath(sizeof(TempPath), TempPath);
	
	GetTempFileName(TempPath, "Mem", 0000, FileName);

	ofstream outfile;

	outfile.open(FileName);

	outfile << "Search for 0x";

	int size;

	if ( SendDlgItemMessage(m_hWnd, IDC_BYTE, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		wsprintf(TempPath, "%02x", value[0]);
		size = 1;
	}
	else if ( SendDlgItemMessage(m_hWnd, IDC_WORD, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		wsprintf(TempPath, "%04x", ((unsigned short *)value)[0]);
		size = 2;
	}
	else
	{
		wsprintf(TempPath, "%08x", ((unsigned long *)value)[0]);
		size = 4;
	}

	outfile << TempPath;

	wsprintf(TempPath, " in region start at %08lx, size %x bytes\n", m_start, m_size);

	outfile << TempPath;

	for (unsigned char * addr = m_start; (addr + size - 1) < (m_start + m_size); addr++)
	{
		for (int s=0; s<size; s++)
			if ( addr[s] != value[s] )
				goto miss;

		wsprintf(TempPath, "%08lx\r\n", addr);
		outfile << TempPath;

	miss: ;

	}		

	outfile.close();

	wsprintf(TempPath, "notepad %s", FileName);
	WinExec(TempPath, SW_NORMAL);

}


void KMemViewer::DumpPage(void)
{
	TCHAR buffer[1024];
	
	buffer[0] = 0;
	unsigned char * p = m_current;
	
	for (unsigned row=0; row<m_pagesize/16; row++, p+=16)
	{
		DumpLine(p, 0, UnitSize());
		strcat(buffer, m_line);
	}
	
	SetDlgItemText(m_hWnd, IDC_DUMP, buffer);
}
