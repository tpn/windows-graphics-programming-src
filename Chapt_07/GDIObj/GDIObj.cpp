//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : gdiobj.cpp						                                     //
//  Description: GDI object table monitoring tool, Windows NT/2000, Chapter 7        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <vector>
#include <stdio.h>
#include <psapi.h> 
#include <tchar.h>

void GetProcessName(DWORD processID, TCHAR szProcessName[])
{
    _tcscpy(szProcessName, _T("unknown"));

    // Get a handle to the process.
    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                   FALSE, processID );

    if ( hProcess==NULL )
		return;
    
	HMODULE hMod;
    DWORD cbNeeded;

    if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
       GetModuleBaseName( hProcess, hMod, szProcessName, MAX_PATH );

    CloseHandle( hProcess );
}


#include "..\..\include\Dialog.h"
#include "..\..\include\ListView.h"
#include "..\..\include\GDITable.h"

#include "resource.h"

typedef struct
{
	DWORD	processid;
	int     nObject[7];
	int     nNew   [7];
}	ProcessInfo;

using namespace std;

class KGDIObjectTable : public KDialog
{
	KListView   m_process;
	KGDITable   m_gditable;

	vector<ProcessInfo> m_info;

	void UpdateTable(void);

    virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
    	switch (uMsg)
	    {
		    case WM_INITDIALOG:
			    m_hWnd = hWnd;

				m_process.FromDlgItem(hWnd, IDC_PROCESS);

				m_process.AddColumn(0, 40,  "PID");
				m_process.AddColumn(1, 100, "Process");

				m_process.AddColumn(2, 50, "Total");
				m_process.AddColumn(3, 40, "DC");
				m_process.AddColumn(4, 50, "Region");
				m_process.AddColumn(5, 45, "Bitmap");
				m_process.AddColumn(6, 50, "Palette");
				m_process.AddColumn(7, 40, "Font");
				m_process.AddColumn(8, 45, "Brush");
				m_process.AddColumn(9, 45, "Other");

				UpdateTable();
				SetTimer(hWnd, 101, 1000, NULL);
				return TRUE;

		    case WM_COMMAND:
				switch ( LOWORD(wParam) )
				{
					case IDOK:
						EndDialog(hWnd, 1);
						return TRUE;
				}

			case WM_TIMER:
				UpdateTable();
	    }

	    return FALSE;
    }

};

typedef enum
{
	DEF_TYPE    = 0,
	DC_TYPE     = 1,
	UNUSED1     = 2,
	UNUSED2     = 3,
	RGN_TYPE    = 4,
	SURF_TYPE   = 5,
	CLIOBJ_TYPE = 6,
	PATH_TYPE   = 7,
	PAL_TYPE    = 8,
	ICMLCS_TYPE = 9,
	LFONT_TYPE  = 10,
	RFONT_TYPE  = 11,
	PFE_TYPE    = 12,
	PFT_TYPE    = 13,
	ICMCXF_TYPE = 14,
	ICMDLL_TYPE = 15,
	BRUSH_TYPE  = 16,
	UNUSED3     = 17,
	UNUSED4     = 18,
	SPACE_TYPE  = 19,
	UNUSED5     = 20,
	META_TYPE   = 21,
	EFSTATE_TYPE= 22,
	BMFD_TYPE   = 23,
	VTFD_TYPE   = 24,
	TTFD_TYPE   = 25,
	RC_TYPE     = 26,
	TEMP_TYPE   = 27,
	DRVOBJ_TYPE = 28,
	DCIOBJ_TYPE = 29,
	SPOOL_TYPE  = 30
};

const int MAXGDIHANDLE = 16384;

void KGDIObjectTable::UpdateTable(void)
{
	// clear nNew array for an updating
	for (int i=0; i<m_info.size(); i++)
		memset(m_info[i].nNew, 0, sizeof(m_info[i].nNew));

	for (i=0; i<MAXGDIHANDLE; i++) // all GDI handles
	{
		GDITableCell cell = m_gditable[i];
		cell._nProcess = m_gditable.GetProcess(i);
		cell._nCount   = m_gditable.GetCount(i);

		// if kernel object point is invalid, object is deleted
		if ( (unsigned) cell.pKernel < 0x80000000 )
			continue;

		// search for the processid in m_info table
		int k = -1;
		
		for (int j=0; j<m_info.size(); j++)
			if ( m_info[j].processid == cell._nProcess )
			{
				k = j;
				break;
			}

		if ( k==-1) 
		{
			ProcessInfo pi = { 0 };

			m_info.push_back(pi);
			k = m_info.size() - 1;
			
			m_info[k].processid = cell._nProcess;
		
			TCHAR szPath[MAX_PATH];
			GetProcessName(m_info[k].processid, szPath);
			// new process in listview
			m_process.AddItem(0, m_info[k].processid);
			m_process.AddItem(1, szPath);
		}

		switch ( cell.nType & 0x1F ) // 0..31
		{
			case DC_TYPE   : m_info[k].nNew[0] ++; break;
			case RGN_TYPE  : m_info[k].nNew[1] ++; break;
			case SURF_TYPE : m_info[k].nNew[2] ++; break;
			case PAL_TYPE  : m_info[k].nNew[3] ++; break;
			case LFONT_TYPE: m_info[k].nNew[4] ++; break;
			case BRUSH_TYPE: m_info[k].nNew[5] ++; break;
			default       :  m_info[k].nNew[6] ++; break;
		}
	}

	// update any changed process
	for (i=0; i<m_info.size(); i++)
		// if changed
		if ( memcmp(m_info[i].nObject, m_info[i].nNew, sizeof(m_info[i].nNew)) )
		{
			memcpy(m_info[i].nObject, m_info[i].nNew, sizeof(m_info[i].nNew));

			m_process.SetItem(i, 2, 
				m_info[i].nObject[0] + m_info[i].nObject[1] +
				m_info[i].nObject[2] + m_info[i].nObject[3] +
				m_info[i].nObject[4] + m_info[i].nObject[5] +
				m_info[i].nObject[6]);
		
			m_process.SetItem(i, 3, m_info[i].nObject[0]);
			m_process.SetItem(i, 4, m_info[i].nObject[1]);
			m_process.SetItem(i, 5, m_info[i].nObject[2]);
			m_process.SetItem(i, 6, m_info[i].nObject[3]);
			m_process.SetItem(i, 7, m_info[i].nObject[4]);
			m_process.SetItem(i, 8, m_info[i].nObject[5]);
			m_process.SetItem(i, 9, m_info[i].nObject[6]);
		}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	KGDIObjectTable gdiobj;

	return gdiobj.Dialogbox(hInstance, MAKEINTRESOURCE(IDD_GDIOBJ));
}