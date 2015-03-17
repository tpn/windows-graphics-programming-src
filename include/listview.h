#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : listview.h						                                     //
//  Description: List view wrapper                                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include <commctrl.h>

// List View
class KListView
{
    HWND       m_hWnd;
    
    HIMAGELIST hImage_Normal, hImage_Small, hImage_State;
    int        nRow;

public:

    HWND GetHWND(void) const
	{
		return m_hWnd;
	}

    KListView()
    {
        m_hWnd        = NULL;
    
        hImage_Normal = NULL;
        hImage_Small  = NULL;
        hImage_State  = NULL;

        nRow          = -1;
    }

    void DeleteAll(void)
    {
        nRow = -1;

        ListView_DeleteAllItems(m_hWnd);
    }

    void FromDlgItem(HWND hWnd, int id);

    void AddIcon(int iImageList, HINSTANCE hInst, int iIcon);

    void AddColumn(int col, int width, const TCHAR *title, BOOL left=TRUE);
        
    void AddItem(int col, const TCHAR *text, int iImage=-1);
    void AddItem(int col, int value, int iImage=-1);

    int GetNextItem(int from, int option)
    {
        return ListView_GetNextItem(m_hWnd, from, option);
    }
    
    void GetItemText(int item, int subitem, TCHAR *rslt, int maxlen)
    {
        ListView_GetItemText(m_hWnd, item, subitem, rslt, maxlen);
    }

	void SetItem(int row, int col, int value);

	HWND Create(HWND hParent, int id, int left, int top, int width, int height, HINSTANCE hInst);
};

