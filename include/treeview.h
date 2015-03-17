#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : treeview.h						                                     //
//  Description: Tree view wrapper, with data decoding                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

// Use a single character to desribe the format of a structure, which can be inserted 
// into a TreeView

// { <tag> <format> }
// <tag>::= g |   // dword, dword, dword, dword
//          8 |   // dword, dword
//          4 |   // dword
//          2 |   // unsigned short
//          1 |   // unsigned char
//          &     // address of input data

// Example:
//
//const TCHAR Format_LOGFONT [] =
//    "4lfHeight: %d\0"
//	"4lfWidth:  %d\0"
//	"4lfEscapement: %d\0"
//	"4lfOrientation: %d\0"
//	"4lfWeight: %d\0"
//  "1lfItalic: %d\0"
//	"1lfUnderline: %d\0"
//	"1lfStrikeOut: %d\0"
//  "1lfCharSet: %d\0"
//	"1lfOutPrecision: %d\0"
//	"1lfClipPrecision: %d\0"
//	"1lfQuality: %d\0"
//	"1lfPitchAndFamily: 0x%x\0"
//	"&lfFaceName: %s\0";

typedef struct
{
	unsigned	  mask;
	const TCHAR * desp;
} term;


class KTreeView
{
public:
	HWND		m_hWnd;

	void Create(HWND hParent, int id, int x, int y, int width, int height, HINSTANCE hInst);

	void DeleteAllItems(void)
    {
        TreeView_DeleteAllItems(m_hWnd);
	}		

	HTREEITEM InsertItem(HTREEITEM hLast, HTREEITEM hParent, const TCHAR * mess);

	HTREEITEM InsertItem(const TCHAR * mess)
	{
		return InsertItem(TVI_LAST, TVI_ROOT, mess);
	}

	HTREEITEM InsertTree(HTREEITEM hLast, HTREEITEM hParent, 
				   const TCHAR * rootname, const TCHAR * pField, 
				   const void * data);

	void AddFlags(HTREEITEM hRoot, DWORD flags, const term * Dict);
};

