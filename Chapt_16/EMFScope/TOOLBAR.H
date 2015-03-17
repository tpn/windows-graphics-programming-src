//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : toolbar.h							                                 //
//  Description: Toolbar dialog box for EMFScope                                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include "filelist.h"

class KString
{
	DWORD store[64];    // 256 characters
	char  pos;

public:		
	KString(void)
	{
		pos = 0;
	}

	BOOL Append(DWORD val)
	{
		store[pos++] = val;

		if (val==0) 
		{
			pos = 0;
			return TRUE;
		}

		return FALSE;
	}

	char *string(void)
	{
		return (char *)store;
	}
};

class KToolBar : public KDialog
{
	BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	KString   s_title;
	KString   s_device;

	int       n_pageno;
	int       n_lastid;
	int       n_scale;
	int       n_color;
		
	HWND      hwnd_dialog;

	HWND      hwnd_activity;
	HWND      hwnd_title;
	HWND	  hwnd_time;
	HICON     hi_print;
	HICON     hi_open;
	HICON     hi_setup;
	HICON	  hi_smoke;

	HINSTANCE hCurInst;
		
	KFileList FileList;
		
	BOOL PrivateMessage(int id, LPARAM lParam);

	void Setup(void);

public:	
	BOOL	  b_MidView;
	BOOL	  b_SaveEmf;
	int		  n_delay;
	char	  DestDir[MAX_PATH];
		
	KCanvasWindow  *w_canvas;

	void SetCanvas(KCanvasWindow *canvas);

	KString   s_filename;
	BOOL      b_showemf;
		
	int       n_width;
	int       n_height;

	KToolBar(HINSTANCE hinst, HWND hwnd);
	~KToolBar();
		
	void SetScaleColor(int scale, int color);
	void SaveSettings(void);

private:		

	BOOL SaveEmfFile(const char *filename);
	void ReportTime(DWORD tick);
};

const char *GetDevFileName(const char *emffilename);