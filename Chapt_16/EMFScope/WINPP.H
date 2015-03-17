//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : winpp.h    						                                 //
//  Description: Another C++ window implementation using machine code                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include "Thunk.h"

int mapx(int x);
int mapy(int y);

int unmapx(int x);
int unmapy(int y);


const char *LoadStringTemp(int id);

class KSubclass
{
protected:
    WNDPROC OldWndProc;

private:
    ThunkData Thunk;

    virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        assert(OldWndProc);

        return CallWindowProc(OldWndProc, hWnd, uMsg, wParam, lParam);
    }
	
public:
    
    KSubclass()
    {
        ThunkInit(Thunk, this);
        OldWndProc = NULL;
    }

    void SubClass(HWND hWnd)
    {		
        OldWndProc = (WNDPROC) GetWindowLong(hWnd, GWL_WNDPROC);
		SetWindowLong(hWnd, GWL_WNDPROC, (LONG) ((void *) Thunk));
    }
};


class KWindow 
{
private:
    ThunkData Thunk;
    
    virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
	
public:
	HWND   m_hWnd;

	KWindow()
	{
        ThunkInit(Thunk, this);
		m_hWnd = NULL;
	}

    HWND Createwindow(LPCTSTR   lpClassName,
                      int       id_icon,
                      int       id_cursor,

                      LPCTSTR   lpWindowName,	
                      DWORD     dwStyle,	
                      int       x, 
                      int       y, 
                      int       w, 
                      int       h,
                      HWND      hWndParent,	
                      HINSTANCE hInstance,
                      HMENU     hMenu,
                      
                      DWORD     dwExStyle,
                      HBRUSH    hBrush);

	void Scroll(int message, WORD wPos, WORD wScrollType);

	int  MessageLoop(int nCmdShow);
};



class KDialog 
{
public:
    HWND m_hWnd;

private:
    ThunkData Thunk;

    virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg==WM_INITDIALOG)
        {
            m_hWnd = hWnd;
            return TRUE;
        }

        return FALSE;
    }

public:
	
    KDialog()
	{
        ThunkInit(Thunk, this);
		m_hWnd = NULL;
	}

	HWND Createdialog(HINSTANCE hinst, int idTemplate, HWND hwndOwner) const
	{
		return ::CreateDialog(hinst, 
							  MAKEINTRESOURCE(idTemplate),	
							  hwndOwner, 
							  (DLGPROC) (void *)Thunk);
	}

};


class KModalDialog 
{
public:
    HWND m_hWnd;

private:

    ThunkData Thunk;

    virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_INITDIALOG)
        {
            m_hWnd = hWnd;
            return TRUE;
        }

        return FALSE;
    }

public:

    KModalDialog()
	{
        ThunkInit(Thunk, this);
		m_hWnd = NULL;
	}

	int Dialogbox(HINSTANCE hinst, int idTemplate, HWND hwndOwner) const
	{
		return ::DialogBox(hinst, 
				           MAKEINTRESOURCE(idTemplate), 
				           hwndOwner, 
				           (DLGPROC) (void *) Thunk);
	}

	BOOL CheckDlgButton(int id, BOOL val) const
	{
		return ::CheckDlgButton(m_hWnd, id, val ? MF_CHECKED : MF_UNCHECKED);
	}
};
