//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : toolbar.cpp						                                 //
//  Description: EMFScope toolbar                                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <assert.h>

#include "Winpp.h"

#include "spehon32\\spehon32.h"
#include "resource.h"

#include "canvas.h"
#include "toolbar.h"
#include "Spoolfil.h"
#include "emfscope.h"	// for EmfScope.hMainMenu


KSpoolFile SpoolFile;


BOOL KToolBar::DlgProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
		case WM_INITDIALOG:
			m_hWnd = hWnd;
			FileList.Create(hCurInst, hWnd, GetSubMenu(EmfScope.hMainMenu, 2));
			return TRUE;
		
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDC_SCL_11:
				case IDC_SCL_12:
				case IDC_SCL_13:
				case IDC_SCL_14:
				case IDC_SCL_15:
				case IDC_SCL_16:
					SetScaleColor(LOWORD(wParam) - IDC_SCL_11 + 1, -1);
						
					CheckRadioButton(hWnd, IDC_SCL_11, IDC_SCL_16, LOWORD(wParam));
					return TRUE;
					
				case IDC_WHITE:
				case IDC_GRAY:
				case IDC_YELLOW:
					SetScaleColor(0, LOWORD(wParam)-IDC_WHITE);
					CheckRadioButton(hWnd, IDC_WHITE, IDC_YELLOW, LOWORD(wParam));
					break;

				case IDC_EMFLIST:
					if (HIWORD(wParam)==LBN_DBLCLK)
					{
						FileList.Open();
						return TRUE;
					}
					return FALSE;
					break;

				case IDC_PRINT:
					if (w_canvas)
						w_canvas->Print();
					return TRUE;

				case IDC_OPEN:
					if (w_canvas)
					{
						const char *filename;
						
						filename = w_canvas->OpenFile();

						if (filename)
							FileList.AddtoFileList(filename);
					}
					break;

				case IDC_SETUP:			Setup();				break;

				default:
					return FALSE;
			}

		case WM_USER:
			if (!PrivateMessage(wParam, lParam))
				return FALSE;

			if (b_showemf)
			{
				b_showemf = FALSE;

				if (w_canvas)
					w_canvas->LoadEmfFile(s_filename.string());
			}				
			return TRUE;
			break;
	
		case WM_COPYDATA:
			return SpoolFile.ReceiveData((COPYDATASTRUCT *)lParam);
			break;

		default:
			return FALSE;
	}
}


const COLORREF TinyPalette[] =
	{
		RGB(0xFF, 0xFF, 0xFF),    // white
		RGB(0x80, 0x80, 0x80),	  // gray
		RGB(0xFF, 0xFF, 0xC0)     // yellow+gray
	};


void KToolBar::SetScaleColor(int scale, int color)
{
	// if incoming scale==0, don't change, but communicate to Canvas window
	if (scale)
		n_scale = scale;

	if ( (color>=0) && (color<=2) )
		n_color = color;
	
	if (w_canvas)
		w_canvas->SetScaleColor(n_scale, TinyPalette[n_color]);
}


void KToolBar::SetCanvas(KCanvasWindow *canvas)
{
	w_canvas = canvas;

	FileList.SetCanvas(canvas);
	w_canvas->SetScaleColor(n_scale, TinyPalette[n_color]);
	w_canvas->SetDelay(n_delay);
}

const char nam_profile    [] = "emfscope.ini";

const char sec_emfscope95 [] = "EmfScope 95";
const char key_scale      [] = "Scale";
const char key_saveemf    [] = "SaveEmf";
const char key_midview    [] = "MidView";
const char key_backcolor  [] = "BackColor";
const char key_destdir    [] = "DestDir";
const char key_delay      [] = "Delay";

const char sec_recentfiles[] = "RecentFiles";

KToolBar::~KToolBar()
{
	DeleteObject(hi_print);
	DeleteObject(hi_open);
	DeleteObject(hi_setup);
	DeleteObject(hi_smoke);
}

const int max_recentfiles = 64;
const int max_delay       = 16;

KToolBar::KToolBar(HINSTANCE hInst, HWND hWnd)
{
	// HBITMAP h_11;
	RECT	rect;

	w_canvas    = NULL;

	hCurInst     = hInst;

	b_showemf   = FALSE;

	n_pageno    = 0;
	n_scale     = 1;
	n_color     = 0;

	hi_print    = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PRINT));
	hi_open     = LoadIcon(hInst, MAKEINTRESOURCE(IDI_OPEN));
	hi_setup    = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SETUP));

	hwnd_dialog   = Createdialog(hInst, IDD_TOOLBAR, hWnd);

	hwnd_activity = GetDlgItem(hwnd_dialog, IDC_ACTION);
	hwnd_title    = GetDlgItem(hwnd_dialog, IDC_TITLE);
	hwnd_time     = GetDlgItem(hwnd_dialog, IDC_TIME);

	SendDlgItemMessage(hwnd_dialog, IDC_OPEN,  BM_SETIMAGE, IMAGE_ICON, (LPARAM)hi_open);
	SendDlgItemMessage(hwnd_dialog, IDC_PRINT, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hi_print);
	SendDlgItemMessage(hwnd_dialog, IDC_SETUP, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hi_setup);
	SendDlgItemMessage(hwnd_dialog, IDC_SMOKE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hi_smoke);

	GetWindowRect(hwnd_dialog, &rect);
	n_height = unmapy(rect.bottom - rect.top);
	
	n_width  = rect.left;
	GetWindowRect(GetDlgItem(hwnd_dialog, IDC_SCALE), &rect);
	// report width up to the left boundary of scale group box to hide them first
	n_width  = unmapx(rect.left - n_width)- 6;
	
	
	// h_11 = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_11));
	// SendDlgItemMessage(hDlg, IDC_11, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)h_11);

	n_scale = GetPrivateProfileInt(sec_emfscope95, key_scale, -1, nam_profile);
	// check for first usage, or invalid value
	if ( (n_scale<1) || (n_scale>6) )
	{
		n_scale = 3;     // read default scale is 3
	}
	
	CheckRadioButton(hwnd_dialog, IDC_SCL_11, IDC_SCL_16, IDC_SCL_11 + n_scale - 1);

	b_SaveEmf = GetPrivateProfileInt(sec_emfscope95, key_saveemf, 1, nam_profile)==1;
	b_MidView = GetPrivateProfileInt(sec_emfscope95, key_midview, 1, nam_profile)==1;

	n_color = GetPrivateProfileInt(sec_emfscope95, key_backcolor, 2, nam_profile);
	n_color %= 3;		// 0, 1, 2
	CheckRadioButton(hwnd_dialog, IDC_WHITE, IDC_YELLOW, IDC_WHITE + n_color);

	n_delay = GetPrivateProfileInt(sec_emfscope95, key_delay, 0, nam_profile);
	if (n_delay>max_delay)
		n_delay = max_delay;

	if (!GetPrivateProfileString(sec_emfscope95, key_destdir, NULL,
								 DestDir, sizeof(DestDir), nam_profile))
		GetTempPath(sizeof(DestDir), DestDir);

	for (int i=1; i<=max_recentfiles; i++)
	{
		char     filename[256];
		OFSTRUCT ofs;
		int      len;
		char     key_no[5];

		wsprintf(key_no, "%d", i);

		len = GetPrivateProfileString(sec_recentfiles, key_no, NULL, 
			  						  filename, sizeof(filename), nam_profile);

		if (len==0) break;
		
		if (OpenFile(filename, &ofs, OF_EXIST)!=HFILE_ERROR)
			FileList.AddtoFileList(filename);
	}
}


BOOL IsValidDir(const char *path)
{
	char CurDir[MAX_PATH];
	BOOL rslt;

	GetCurrentDirectory(sizeof(CurDir), CurDir);

	rslt = SetCurrentDirectory(path);

	SetCurrentDirectory(CurDir);

	return rslt;
}


class KSetupDialog : public KModalDialog
{
public:
	KToolBar * toolbar;
	
	BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};


BOOL KSetupDialog::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			m_hWnd = hWnd;
			
			CheckDlgButton(IDC_SAVEEMF, toolbar->b_SaveEmf);
			CheckDlgButton(IDC_MIDVIEW, toolbar->b_MidView);
			SetDlgItemText(hWnd, IDC_DESTDIR, toolbar->DestDir);
		
			SendDlgItemMessage(hWnd, IDC_SPEED, TBM_SETRANGE, TRUE, MAKELPARAM(0, max_delay));
			SendDlgItemMessage(hWnd, IDC_SPEED, TBM_SETPOS, TRUE, toolbar->n_delay);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					{
						char NewDir[MAX_PATH];

						GetDlgItemText(hWnd, IDC_DESTDIR, NewDir, sizeof(NewDir));

						// if different
						if (strcmp(NewDir, toolbar->DestDir))
							if (IsValidDir(NewDir))
							{
								if (NewDir[strlen(NewDir)-1]!='\\')
									strcat(NewDir, "\\");
								strcpy(toolbar->DestDir, NewDir);
							}
							else
								MessageBox(0, "Invalid directory", LoadStringTemp(IDS_APPTITLE), MB_OK);
					}
						
					toolbar->b_SaveEmf = IsDlgButtonChecked(hWnd, IDC_SAVEEMF);
					toolbar->b_MidView = IsDlgButtonChecked(hWnd, IDC_MIDVIEW);

					EndDialog(hWnd, IDOK);
					break;

				default:
					return FALSE;
			}
			break;

		case WM_HSCROLL:
		{
			int delay = toolbar->n_delay;

			switch (LOWORD(wParam))
			{
				case TB_TOP          : delay =  0;              break;
				case TB_BOTTOM       : delay =  max_delay;      break;
				case TB_PAGEUP       : delay -= max_delay/4;    break;
			    case TB_PAGEDOWN     : delay += max_delay/4;    break;
			    case TB_LINEUP       : delay --;                break;
			    case TB_LINEDOWN     : delay ++;				break;
			    case TB_THUMBPOSITION: delay =  HIWORD(wParam);	break;
			}

			if (delay<0)
				delay = 0;
			else if (delay>max_delay)
				delay = max_delay;

			if (toolbar->n_delay!=delay)
			{
				toolbar->n_delay = delay;
				SendDlgItemMessage(hWnd, IDC_SPEED, TBM_SETPOS, TRUE, toolbar->n_delay);
			}
		}
        break;

		default:
			return FALSE;
	}

	return TRUE;
}


void KToolBar::Setup(void)
{
	KSetupDialog dlg;

	dlg.toolbar = this;
	dlg.Dialogbox(hCurInst, IDD_SETUP, m_hWnd);

	if (w_canvas)
		w_canvas->SetDelay(n_delay);
}


void KToolBar::SaveSettings(void)
{
	char number[12];
	int  count;

	wsprintf(number, "%d", n_scale);
	WritePrivateProfileString(sec_emfscope95, key_scale, number, nam_profile);

	wsprintf(number, "%d", b_SaveEmf);
	WritePrivateProfileString(sec_emfscope95, key_saveemf, number, nam_profile);

	wsprintf(number, "%d", b_MidView);
	WritePrivateProfileString(sec_emfscope95, key_midview, number, nam_profile);

	wsprintf(number, "%d", n_color);
	WritePrivateProfileString(sec_emfscope95, key_backcolor, number, nam_profile);

	WritePrivateProfileString(sec_emfscope95, key_destdir, DestDir, nam_profile);

	wsprintf(number, "%d", n_delay);
	WritePrivateProfileString(sec_emfscope95, key_delay, number, nam_profile);
	
	count = FileList.GetCount();
	
	for (int i = max(0, count-max_recentfiles); i<=count; i++)
	{
		wsprintf(number, "%d", i+1);

		WritePrivateProfileString(sec_recentfiles, number, 
			i==count ? NULL :	FileList.GetEmfFileName(i), 
			nam_profile);
	}

}


BOOL KToolBar::SaveEmfFile(const char *filename)
{
	FileList.AddtoFileList(filename);

	if (b_SaveEmf || b_MidView)
	{
		char *p;
		char newfilename[MAX_PATH];
		char name[5];
		BOOL rslt;

		strcpy(newfilename, filename);

		// find the actual file name within the full path
		p = newfilename;
		while (strchr(p, '\\')!=NULL)
			p = strchr(p, '\\') + 1;
		
		// p = & ~EMFxxxx.TMP
		if (strncmp(p, "~EMF", 4))
			return FALSE;

		*p = 0;       // newfilename will contain the path without the filename
		strncpy(name, p+4, 4); // xxxx
		name[4] = 0;

#ifndef KEEPOLDDIR
		strcpy(newfilename, DestDir);
#endif
		strcat(newfilename, name);
		strcat(newfilename, ".emf");  // path xxx.emf

		PrivateMessage(id_activity, act_copy);

		rslt = CopyFile(filename, newfilename, TRUE);

		if ( !rslt && (GetLastError()==ERROR_FILE_EXISTS) )
		if (MessageBox(NULL, 
					   "Fill already exists.\nDo you want to overwrite?",
					   "Save Emf File",
					   MB_YESNO | MB_ICONQUESTION) == IDYES)
			rslt = CopyFile(filename, newfilename, FALSE);
		
		if (rslt)
		{
			SpoolFile.GenDevFile(newfilename);

			strcpy(s_filename.string(), newfilename);
			FileList.ReplaceLastName(s_filename.string());

			b_showemf = b_MidView;
		}
		
		PrivateMessage(id_activity, act_done);

		return rslt;
	}
	else
		return FALSE;
}


void KToolBar::ReportTime(DWORD tick)
{
	char temp[256];

	n_pageno ++;

	wsprintf(temp, "%d. %s (%d.%02d s)", n_pageno, s_filename.string(), tick/1000, tick%1000/10);
	
	FileList.ReplaceLastName(temp);
}


BOOL KToolBar::PrivateMessage(int id, LPARAM lParam)
{
	switch (id)
	{
		case id_emffile:
			if (s_filename.Append(lParam))
				SaveEmfFile(s_filename.string());
			break;

		case id_title:
			if (s_title.Append(lParam))
				SetWindowText(hwnd_title, s_title.string());
			break;

		case id_device:
			if (s_device.Append(lParam))
			{
				char WinText[128];

				wsprintf(WinText, LoadStringTemp(IDS_APPTITLE), s_device.string());
				SetWindowText(GetParent(hwnd_dialog), WinText);
			}
			break;

		case id_activity:
			{
				const char *p = NULL;

				switch (lParam)
				{
					case act_done   : p = NULL;                        break;
					case act_despool: p = LoadStringTemp(IDS_DESPOOL); break;
					case act_delete : p = LoadStringTemp(IDS_DELETE);  break;
					case act_copy   : p = LoadStringTemp(IDS_COPY);    break; 							
				}
				SetWindowText(hwnd_activity, p);
			} 

			break;

		case id_time:
			ReportTime(lParam);
			break;

		default:
			return FALSE;
	}
	
	return TRUE;
}
