//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : filelist.cpp						                                 //
//  Description: EMF file list box                                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <assert.h>
#include <commctrl.h>

#include "Winpp.h"

#include "resource.h"
#include "canvas.h"
#include "spoolfil.h"

#include "filelist.h"


// Subclassing Emflist list box to provide context menu

LRESULT KFileList::WndProc(HWND hWnd, UINT uMsg,
					 	   WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_RBUTTONDOWN:
			{
				DWORD rslt = SendMessage(hWnd, LB_ITEMFROMPOINT, 0, lParam);

				if ( HIWORD(rslt) == 0 ) // inside client area
				{
					POINT pt;

					pt.x = LOWORD(lParam);
					pt.y = HIWORD(lParam);
					ClientToScreen(hWnd, & pt);

					TrackMenu(LOWORD(rslt), pt.x, pt.y);
				}
			}
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				// message from emflist context menu
				case IDM_OPEN:			Open();			break;
				case IDM_RENAME:		Rename();		break;
				case IDM_PROPERTIES:	Properties();	break;
				case IDM_DELETE:		Delete();		break;
				case IDM_REMOVE:		Remove();		break;
			}
	}
				
	return CallWindowProc(OldWndProc, hWnd, uMsg, wParam, lParam);
}


void KFileList::Create(HINSTANCE hInstance, HWND hDlg, HMENU hMenu)
{
	hCurInst	 = hInstance;
	hCurMenu	 = hMenu;
	hwnd_emflist = GetDlgItem(hDlg, IDC_EMFLIST);
	
	n_lastid   = -1;

	// subclass emflist listbox
	if (hwnd_emflist)
        SubClass(hwnd_emflist);
}


void KFileList::AddtoFileList(const char *filename)
{
	n_lastid = SendMessage(hwnd_emflist, LB_ADDSTRING, 0, (LPARAM)filename); 
		
	if (n_lastid>=n_maxvisible)
		SendMessage(hwnd_emflist, LB_SETTOPINDEX, n_lastid-n_maxvisible, 0);
}


void KFileList::ReplaceLastName(const char *name)
{
	SendMessage(hwnd_emflist, LB_DELETESTRING, n_lastid, 0);
	SendMessage(hwnd_emflist, LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) name);
	
	if (n_lastid>=n_maxvisible)
		SendMessage(hwnd_emflist, LB_SETTOPINDEX, n_lastid-n_maxvisible, 0);
}


int KFileList::GetCount(void)
{
	return SendMessage(hwnd_emflist, LB_GETCOUNT, 0, 0);
}


// fetch emf file name from list box
const char *KFileList::GetEmfFileName(int id)
{
	char *pname;
	char *pend;
	static char desp[256];

	SendMessage(hwnd_emflist, LB_GETTEXT, id, (LPARAM)(LPCTSTR)desp);
	// the string is in the format: page. filename (time)
	// or filename
			
	pname = strchr(desp, ' ');
	if (pname)
	{
		pname++;
				
		pend = strchr(pname, ' ');

		if (pend)
			*pend = 0;

		return pname;
	}
	else
		return desp;
}


void KFileList::Open(void)
{
	int  i = SendMessage(hwnd_emflist, LB_GETCURSEL, 0, 0);

	if (i != LB_ERR)
		w_canvas->LoadEmfFile(GetEmfFileName(i));
}

//	HMENU hMenu = GetSubMenu(EmfScope.hMainMenu, 2);

void KFileList::TrackMenu(int id, int x, int y)
{
	TrackPopupMenu(hCurMenu, TPM_RIGHTBUTTON | TPM_TOPALIGN | TPM_LEFTALIGN,
		x, y, 0, hwnd_emflist, NULL);
}


void KFileList::Remove(void)
{
	int i = SendMessage(hwnd_emflist, LB_GETCURSEL, 0, 0);

	if (i != LB_ERR)
		SendMessage(hwnd_emflist, LB_DELETESTRING, i, 0);
}


// emflist rename dialog box
class KRenameDialog : public KModalDialog
{
public:
	char OldName[128];
	char NewName[128];
	
	BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};


BOOL KRenameDialog::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			m_hWnd = hWnd;
			SetDlgItemText(hWnd, IDC_OLDNAME, OldName);
			SetDlgItemText(hWnd, IDC_NEWNAME, OldName);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					GetDlgItemText(hWnd, IDC_NEWNAME, NewName, sizeof(NewName) - 1);
					NewName[sizeof(NewName)-1] = 0;
					EndDialog(hWnd, IDOK);
					break;

				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;

				default:
					return FALSE;
			}
			break;


		default:
			return FALSE;
	}

	return TRUE;
}


void KFileList::Rename(void)
{
	int i = SendMessage(hwnd_emflist, LB_GETCURSEL, 0, 0);

	if (i != LB_ERR)
	{
		KRenameDialog dlg;

		strcpy(dlg.OldName, GetEmfFileName(i));
		
		if (dlg.Dialogbox(hCurInst, IDD_RENAME, GetParent(hwnd_emflist)) == IDOK)
		{
			// remove file from canvas window
			w_canvas->UnloadEmfFile(dlg.OldName);

			if (MoveFile(dlg.OldName, dlg.NewName))
			{
				// delete old string
				SendMessage(hwnd_emflist, LB_DELETESTRING, i, 0);
				// insert new string
				SendMessage(hwnd_emflist, LB_INSERTSTRING, i, (LPARAM) (LPCSTR) dlg.NewName);

				const char *name = GetDevFileName(dlg.OldName);

				// rename associated .dev file
				if (name != NULL)
				{
					strcpy(dlg.OldName, name);

					name = GetDevFileName(dlg.NewName);
					if (name != NULL)
						MoveFile(dlg.OldName, name);
				}
			}
			else
				MessageBeep(MB_ICONEXCLAMATION);
		}
	}
}


class KPropertyDialog : public KModalDialog
{
public:
	
	BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
    	switch (uMsg)
	    {
		    case WM_INITDIALOG:
			    m_hWnd = hWnd;
    			break;

            case WM_COMMAND:
			    switch (LOWORD(wParam))
			    {
				    case IDOK:
					    EndDialog(hWnd, IDOK);
					    break;

				    case IDCANCEL:
					    EndDialog(hWnd, IDCANCEL);
					    break;

				    default:
					    return FALSE;
			    }
			    break;

	    	default:
		    	return FALSE;
	    }

	    return TRUE;
    }
};


void KFileList::Properties(void)
{
	int i = SendMessage(hwnd_emflist, LB_GETCURSEL, 0, 0);

	if (i != LB_ERR)
	{
        KPropertyDialog dlg;

        dlg.Dialogbox(hCurInst, IDD_PROPERTIES, GetParent(hwnd_emflist));
	}
}


void KFileList::Delete(void)
{
	int i = SendMessage(hwnd_emflist, LB_GETCURSEL, 0, 0);

	if (i != LB_ERR)
	{
		char   temp[128];
		LPCSTR name = GetEmfFileName(i);

		wsprintf(temp, LoadStringTemp(IDS_CONFIRMDELETE), (LPCSTR) name);
		
		if (MessageBox(GetParent(hwnd_emflist), temp, LoadStringTemp(IDS_APPTITLE), MB_OKCANCEL) == IDOK)
		{
			// remove file from canvas window
			w_canvas->UnloadEmfFile(name);

			if (DeleteFile(name))
			{
				name = GetDevFileName(name);

				if (name)
					DeleteFile(name);

				// remove string from listbox
				SendMessage(hwnd_emflist, LB_DELETESTRING, i, 0);
			}
			else
				MessageBeep(MB_ICONEXCLAMATION);
		}
	}
}
