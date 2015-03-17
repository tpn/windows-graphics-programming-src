//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : listview.cpp						                                 //
//  Description: KListView class implementation                                      //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>

#include "listview.h"

    
void KListView::FromDlgItem(HWND hWnd, int id)
{
    m_hWnd = GetDlgItem(hWnd, id);
    nRow   = -1;
}


HWND KListView::Create(HWND hParent, int id, int left, int top, int width, int height, HINSTANCE hInst)
{
	m_hWnd = CreateWindowEx(0, WC_LISTVIEW, _T(""), WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_BORDER, 
				left, top, width, height, hParent, (HMENU) id, hInst, NULL);
	nRow   = -1;

	return m_hWnd;
}


void KListView::AddIcon(int iImageList, HINSTANCE hInst, int iIcon)
{
    HIMAGELIST * phImage;

    switch (iImageList)
    {
        case LVSIL_NORMAL: 
            phImage = & hImage_Normal;
            break;

        case LVSIL_SMALL:
            phImage = & hImage_Small;
            break;

        case LVSIL_STATE:
            phImage = & hImage_State;
            break;

        default:
            assert(FALSE);
            return;
    }

    if (*phImage==NULL)
    {
        int size = (iImageList==LVSIL_SMALL) ? 16 : 32;

        *phImage = ImageList_Create(size, size, ILC_COLOR4 | ILC_MASK, 1, 1);
        ListView_SetImageList(m_hWnd, *phImage, iImageList);
    }

    ImageList_AddIcon(*phImage, LoadIcon(hInst, MAKEINTRESOURCE(iIcon)));
}


void KListView::AddColumn(int col, int width, const TCHAR *title, BOOL left)
{
    LV_COLUMN c;

    c.mask     = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    c.fmt      = left ? LVCFMT_LEFT : LVCFMT_RIGHT;
    c.cx       = width;
    c.pszText  = (TCHAR *) title;
    c.iSubItem = col;
                    
    ListView_InsertColumn(m_hWnd, col, &c);
}


void KListView::AddItem(int col, int value, int iImage)
{
	TCHAR temp[32];

	wsprintf(temp, _T("%d"), value);

	AddItem(col, temp, iImage);
}
    

void KListView::AddItem(int col, const TCHAR *text, int iImage)
{
    LV_ITEM item;

    if (iImage>=0)
        item.mask = LVIF_TEXT | LVIF_IMAGE;
    else
        item.mask = LVIF_TEXT;

    // if column is 0, adding a row
    if (col==0)
        nRow ++;

    item.pszText  = (TCHAR *) text;
    item.iItem    = nRow;
    item.iSubItem = col;
    item.iImage   = iImage;
        
    if (col==0)
        ListView_InsertItem(m_hWnd, &item);
    else
        ListView_SetItem(m_hWnd, &item);
}


void KListView::SetItem(int row, int col, int value)
{
    LV_ITEM item;

	TCHAR mess[32];
	wsprintf(mess, _T("%d"), value);

    item.mask     = LVIF_TEXT;
    item.pszText  = mess;
    item.iItem    = row;
    item.iSubItem = col;
        
    ListView_SetItem(m_hWnd, &item);
}
