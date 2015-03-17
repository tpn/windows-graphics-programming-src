//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : devpage.cpp					                                     //
//  Description: KDevicePage class		                                             //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define NOCRYPT
#define WINVER       0x0500

#include <windows.h>
#include <tchar.h>

#include "resource.h"

#include "..\..\include\property.h"
#include "..\..\include\listview.h"
#include "..\..\include\outputsetup.h"

#include "DevPage.h"
#include "DevCaps.h"
#include "DCAttr.h"

int FrameBufferSize(int width, int height, int bpp)
{
	int bytepp = ( bpp + 7 ) / 8;	             // bytes per pixel
	int byteps = ( width * bytepp + 3 ) / 4 * 4; // bytes per scanline

	return height * byteps;						 // bytes per frame buffer
}


void AddDisplaySettings(HWND hList, LPCTSTR pszDeviceName)
{
	DEVMODE  dm;

	dm.dmSize		 = sizeof(DEVMODE);
	dm.dmDriverExtra = 0;

	SendMessage(hList, LB_RESETCONTENT, 0, 0);

	for (unsigned i=0;
	     EnumDisplaySettings(pszDeviceName, i, & dm);
		 i++ )
	{
		TCHAR szTemp[MAX_PATH];

		wsprintf(szTemp, _T("%d by %d, %d bits, %d Hz, %d Kb"), 
			dm.dmPelsWidth, 
			dm.dmPelsHeight, 
			dm.dmBitsPerPel,
			dm.dmDisplayFrequency,
			FrameBufferSize(dm.dmPelsWidth, dm.dmPelsHeight, dm.dmBitsPerPel) / 1024
			);

		SendMessage(hList, LB_ADDSTRING, 0, (LPARAM) szTemp);
	}
}


BOOL KDevicePage::OnInitDialog(HWND hWnd)
{
	HWND hList = GetDlgItem(hWnd, IDC_DEVICE);

	SendMessage(hList, CB_RESETCONTENT, 0, 0);
	ListPrinters(hList, CB_ADDSTRING);
 	SendMessage(hList, CB_SETCURSEL, 0, 0);

	m_infolist.FromDlgItem(hWnd, IDC_INFO2);
	m_infolist.AddColumn(0, 100, "Field");
	m_infolist.AddColumn(1, 200, "Values");
		
	OnDeviceChange(hWnd);

    return TRUE;
}


inline void AddRow(KListView & list, const TCHAR * col1, const TCHAR * col2)
{
	list.AddItem(0, col1);
	list.AddItem(1, col2);
}


BOOL KDevicePage::OnDeviceChange(HWND hWnd)
{
	TCHAR szDeviceName[MAX_PATH];

	GetDlgItemText(hWnd, IDC_DEVICE, szDeviceName, MAX_PATH);

	HANDLE hPrinter;
	OpenPrinter(szDeviceName, & hPrinter, NULL);

	if ( m_pInfo2 )
		delete [] (BYTE *) m_pInfo2;

	DWORD cbNeeded;

	GetPrinter(hPrinter, 2, NULL, 0, & cbNeeded);

	if ( cbNeeded>0 )
	{
		m_pInfo2 = (PRINTER_INFO_2 * ) new BYTE[cbNeeded];

		GetPrinter(hPrinter, 2, (BYTE *) m_pInfo2, cbNeeded, & cbNeeded);

		m_infolist.DeleteAll();

		AddRow(m_infolist, "Driver Name",		m_pInfo2->pDriverName);
		AddRow(m_infolist, "Printer Name",		m_pInfo2->pPrinterName);
		AddRow(m_infolist, "Share Name",		m_pInfo2->pShareName);
		AddRow(m_infolist, "Port Name",			m_pInfo2->pPortName);
		AddRow(m_infolist, "Driver Name",		m_pInfo2->pDriverName);
		AddRow(m_infolist, "Comment",			m_pInfo2->pComment);
		AddRow(m_infolist, "Location",			m_pInfo2->pLocation);
		AddRow(m_infolist, "Separator Page",	m_pInfo2->pSepFile);
		AddRow(m_infolist, "Print Processor",	m_pInfo2->pPrintProcessor);
		AddRow(m_infolist, "Data type",			m_pInfo2->pDatatype);
		AddRow(m_infolist, "Parameters",		m_pInfo2->pParameters);
	}

	ClosePrinter(hPrinter);

	return TRUE;
}



BOOL KDevicePage::OnDeviceCaps(HWND hWnd)
{
	HDC hDC = CreateDC(m_pInfo2->pDriverName, m_pInfo2->pPrinterName, 
					m_pInfo2->pPortName, m_pInfo2->pDevMode);

	KDeviceCaps caps(hDC);

	caps.Dialogbox(m_hInstance, MAKEINTRESOURCE(IDD_DEVICECAPS));
	
	DeleteDC(hDC);

	return TRUE;
}


BOOL KDevicePage::OnDCAttributes(HWND hWnd)
{
	HDC hDC = CreateDC(m_pInfo2->pDriverName, m_pInfo2->pPrinterName, 
					m_pInfo2->pPortName, m_pInfo2->pDevMode);

	KDCAttributes attr(hDC);

	attr.Dialogbox(m_hInstance, MAKEINTRESOURCE(IDD_DCATTRIBUTES), hWnd);
	
	DeleteDC(hDC);

	return TRUE;
}


BOOL KDevicePage::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
	    case WM_INITDIALOG:
			return OnInitDialog(hWnd);

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDC_DEVICE:
					if ( HIWORD(wParam)==CBN_SELCHANGE )
						return OnDeviceChange(hWnd);
					break;

				case IDC_DEVICECAPS:
					return OnDeviceCaps(hWnd);

				case IDC_DCATTR:
					return OnDCAttributes(hWnd);
			}
    }
    
    return FALSE;
}
