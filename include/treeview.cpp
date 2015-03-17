//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : treeview.cpp						                                 //
//  Description: Tree View class                                                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <commctrl.h>

#include "treeview.h"

// { <tag> <format> }
// <tag>::= g |   // dword, dword, dword, dword
//          8 |   // dword, dword
//          4 |   // dword
//          2 |   // unsigned short
//          1 |   // unsigned char
//          &     // address of input data

// Class for formating structures into text string 
class KFormat
{
	const TCHAR * m_pFormat;
	const DWORD * m_pData;

public:
	TCHAR		  m_Output[MAX_PATH];

	KFormat(const TCHAR * pFormat, const void * pData)
	{
		m_pFormat = pFormat;
		m_pData   = (const DWORD *) pData;
	}

	bool Getline(void)
	{
		if ( (m_pFormat==NULL) || m_pFormat[0]==0 )
			return false;

		DWORD value = m_pData[0];

		int len = 0;

		switch ( m_pFormat[0] )
		{
			case 'g': len = 16; break;
			case '8': len =  8; break;
			case '4': len =  4; break;
			case '2': len =  2; value = value & 0xFFFF;  break;
			case '1': len =  1; value = value & 0xFF;    break;
			case '&': len =  0; value = (DWORD) m_pData; break;
			
			default:
				assert(false);
				return false;
		}					  

		wsprintf(m_Output, m_pFormat + 1, value, m_pData[1], m_pData[2], m_pData[3]);

		m_pFormat = m_pFormat + _tcslen(m_pFormat) + 1;
		m_pData   = (const DWORD *) ((const BYTE *) m_pData + len);
		
		return true;
	}
};


void KTreeView::Create(HWND hParent, int id, int x, int y, int width, int height, HINSTANCE hInst)
{
	m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TREEVIEW, _T(""),
					WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL | 
					TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT,
					x, y, width, height,
					hParent, (HMENU) id, hInst, NULL);
}


HTREEITEM KTreeView::InsertItem(HTREEITEM hLast, HTREEITEM hParent, const TCHAR * mess)
{
	TV_INSERTSTRUCT tv;

	tv.hInsertAfter = hLast;
	tv.item.mask    = TVIF_TEXT;
	tv.hParent      = hParent;
	tv.item.pszText = (TCHAR *) mess;

	return TreeView_InsertItem(m_hWnd, & tv);
}


HTREEITEM KTreeView::InsertTree(HTREEITEM hLast, HTREEITEM hParent, 
					 const TCHAR * rootname, const TCHAR * pField, const void * data)
{
	KFormat format(pField, data);

	if ( rootname )
	{
		hParent = InsertItem(hLast, hParent, rootname);
		hLast   = TVI_LAST;
	}
	
	while ( format.Getline() )
		hLast = InsertItem(hLast, hParent, format.m_Output);

	if ( rootname )
		return hParent;
	else
		return hLast;
}


void KTreeView::AddFlags(HTREEITEM hRoot, DWORD flags, const term * Dict)
{
	while ( Dict->mask && Dict->desp )
	{
		if ( Dict->mask & flags )
		{
			InsertItem(TVI_LAST, hRoot, Dict->desp);
			flags &= ~ Dict->mask;
		}

		Dict ++;
	}

	if ( flags )
	{
		TCHAR temp[32];
		wsprintf(temp, _T("unknown 0x%x"), flags);
		InsertItem(TVI_LAST, hRoot, temp);
	}
}


