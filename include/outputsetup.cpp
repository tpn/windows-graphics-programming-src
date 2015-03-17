//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : outputsetup.cpp   				                                     //
//  Description: KOutputSetup class: printer setup and page setup                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <commdlg.h>
#include <winspool.h>
#include <tchar.h>

#include "outputsetup.h"
#include "dialog.h"
#include "listview.h"

void KOutputSetup::Release(void)
{
	if ( m_pd.hDevMode )
	{
		GlobalFree(m_pd.hDevMode);
		m_pd.hDevMode = NULL;
	}
			
	if ( m_pd.hDevNames )
	{
		GlobalFree(m_pd.hDevNames);
		m_pd.hDevNames = NULL;
	}

	DeletePrinterDC();
}


void KOutputSetup::DeletePrinterDC(void)
{
	if ( m_pd.hDC )
	{
		DeleteObject(m_pd.hDC);
		m_pd.hDC = NULL;
	}
}


KOutputSetup::KOutputSetup(void)
{
	memset (&m_pd, 0, sizeof(PRINTDLG));
	m_pd.lStructSize = sizeof(PRINTDLG);
//	m_pd.nMinPage = 1;
//	m_pd.nMaxPage = 100;

	memset(& m_psd, 0, sizeof(m_psd));
	m_psd.lStructSize = sizeof(m_psd);
}

void KOutputSetup::SetDefault(HWND hwndOwner, int minpage, int maxpage)
{
	m_pd.hwndOwner   = hwndOwner;

	PrintDialog(PD_RETURNDEFAULT);

	m_pd.nFromPage = minpage;
	m_pd.nToPage   = maxpage;
	m_pd.nMinPage  = minpage;
	m_pd.nMaxPage  = maxpage;

	m_psd.hwndOwner  = hwndOwner;

	m_psd.rtMargin.left  = 1250; // 1.25 inch
	m_psd.rtMargin.right = 1250; // 1.25 inch
	m_psd.rtMargin.top   = 1000; // 1.25 inch
	m_psd.rtMargin.bottom= 1000; // 1.25 inch

	HDC hDC = CreatePrinterDC();

	int dpix = GetDeviceCaps(hDC, LOGPIXELSX);
	int dpiy = GetDeviceCaps(hDC, LOGPIXELSY);

	m_psd.ptPaperSize.x     = GetDeviceCaps(hDC, PHYSICALWIDTH)  * 1000 / dpix;
	m_psd.ptPaperSize.y     = GetDeviceCaps(hDC, PHYSICALHEIGHT) * 1000 / dpiy;

	m_psd.rtMinMargin.left  = GetDeviceCaps(hDC, PHYSICALOFFSETX) * 1000 / dpix;
	m_psd.rtMinMargin.top   = GetDeviceCaps(hDC, PHYSICALOFFSETY) * 1000 / dpiy;

	m_psd.rtMinMargin.right = m_psd.ptPaperSize.x - m_psd.rtMinMargin.left - GetDeviceCaps(hDC, HORZRES) * 1000 / dpix;
	m_psd.rtMinMargin.bottom= m_psd.ptPaperSize.y - m_psd.rtMinMargin.top  - GetDeviceCaps(hDC, VERTRES) * 1000 / dpiy;
		
	DeleteObject(hDC);
//  PageSetup(PSD_RETURNDEFAULT);
}

	
KOutputSetup::~KOutputSetup(void)
{
	Release();
}


int KOutputSetup::PrintDialog(DWORD flag)
{
	m_pd.Flags = flag;
	return PrintDlg(&m_pd);
}


BOOL KOutputSetup::PageSetup(DWORD flag)
{
	m_psd.hDevMode  = m_pd.hDevMode; 
	m_psd.hDevNames = m_pd.hDevNames;
	m_psd.Flags     = flag | PSD_INTHOUSANDTHSOFINCHES | PSD_MARGINS;

	return PageSetupDlg(& m_psd);
}


const TCHAR * KOutputSetup::GetDriverName(void) const
{
	const DEVNAMES * pDevNames = (DEVNAMES *) GlobalLock(m_pd.hDevNames);

	if ( pDevNames )
		return (const TCHAR *) ( (const char *) pDevNames + pDevNames->wDriverOffset );
	else
		return NULL;
}


const TCHAR * KOutputSetup::GetDeviceName(void) const
{
	const DEVNAMES * pDevNames = (DEVNAMES *) GlobalLock(m_pd.hDevNames);

	if ( pDevNames )
		return (const TCHAR *) ( (const char *) pDevNames + pDevNames->wDeviceOffset );
	else
		return NULL;
}


const TCHAR * KOutputSetup::GetOutputName(void) const
{
	const DEVNAMES * pDevNames = (DEVNAMES *) GlobalLock(m_pd.hDevNames);

	if ( pDevNames )
		return (const TCHAR *) ( (const char *) pDevNames + pDevNames->wOutputOffset );
	else
		return NULL;
}


HDC KOutputSetup::CreatePrinterDC(void)
{
	return CreateDC(NULL, GetDeviceName(), NULL, GetDevMode());
}


/////////////////////////////////////////////////////////

void * EnumeratePrinters(DWORD flag, LPTSTR name, DWORD level, DWORD & nPrinters)
{
	DWORD cbNeeded;

	nPrinters = 0;
	EnumPrinters(flag, name, level, NULL, 0, & cbNeeded, & nPrinters);

	BYTE * pPrnInfo = new BYTE[cbNeeded];

	if ( pPrnInfo )
		EnumPrinters(flag, name, level, (BYTE *) pPrnInfo, cbNeeded, 
			& cbNeeded, & nPrinters);

	return pPrnInfo;
}


void ListPrinters(HWND hWnd, int message)
{
	DWORD nPrinters;

	PRINTER_INFO_5 * pInfo5 = (PRINTER_INFO_5 *) EnumeratePrinters(PRINTER_ENUM_LOCAL, NULL, 5, nPrinters);

	if ( pInfo5 )
	{
		for (unsigned i=0; i<nPrinters; i++)
			SendMessage(hWnd, message, 0, (LPARAM) pInfo5[i].pPrinterName);

		delete [] (BYTE *) pInfo5;
	}

	PRINTER_INFO_1 * pInfo1 = (PRINTER_INFO_1 *) EnumeratePrinters(PRINTER_ENUM_CONNECTIONS, NULL, 1, nPrinters);

	if ( pInfo1 )
	{
		for (unsigned i=0; i<nPrinters; i++)
			SendMessage(hWnd, message, 0, (LPARAM) pInfo1[i].pName);

		delete [] (BYTE *) pInfo1;
	}
}


DEVMODE * GetDEVMODE(TCHAR * PrinterName, int nPrompt)
{
	HANDLE hPrinter;

	if ( !OpenPrinter(PrinterName, &hPrinter, NULL) )
		return NULL;

	// A zero for last param returns the size of buffer needed.
	int nSize = DocumentProperties(NULL, hPrinter, PrinterName, NULL, NULL, 0);
 
	DEVMODE * pDevMode = (DEVMODE *) new char[nSize];
	
	if ( pDevMode==NULL )
		return NULL;

	// ask driver to initialize a devmode structure
	DocumentProperties(NULL, hPrinter, PrinterName, pDevMode, NULL, DM_OUT_BUFFER);

	// show property sheet to allow user modification
	BOOL rslt = TRUE;
	switch ( nPrompt )
	{
		case 1:
			rslt = AdvancedDocumentProperties(NULL, hPrinter, PrinterName, pDevMode, pDevMode) == IDOK;
			break;

		case 2:
			rslt = ( DocumentProperties(NULL, hPrinter, PrinterName, pDevMode, pDevMode, 
					    DM_IN_PROMPT | DM_OUT_BUFFER | DM_IN_BUFFER ) == IDOK );
			break;
	}

	ClosePrinter(hPrinter);

	if ( rslt )
		return pDevMode;
	else
	{
		delete [] (BYTE *) pDevMode;
		return NULL;
	}
}

class KProperty : public KDialog
{
	KListView      m_list;
	KOutputSetup * m_pSetup;
	int			 * m_pControlID;

	void AddRow(const TCHAR * one, const TCHAR * two)
	{
		m_list.AddItem(0, one);
		m_list.AddItem(1, two);
	}

	void AddRow(const TCHAR * one, const TCHAR * format, int p1, int p2=0, int p3=0, int p4=0)
	{
		TCHAR temp[64];

		wsprintf(temp, format, p1, p2, p3, p4);
		
		m_list.AddItem(0, one);
		m_list.AddItem(1, temp);
	}

	void ShowAttributes(void)
	{
		m_list.FromDlgItem(m_hWnd, m_pControlID[0]);

		RECT rect;
		GetWindowRect(m_list.GetHWND(), & rect);
		int width = rect.right - rect.left;

		m_list.AddColumn(0, width/2-5, _T("Attribute"));
		m_list.AddColumn(1, width/2-5, _T("Value"));

		AddRow(_T("hDevNames->Driver Name"), m_pSetup->GetDriverName());
		AddRow(_T("           Device Name"), m_pSetup->GetDeviceName());
		AddRow(_T("           Output Name"), m_pSetup->GetOutputName());

		const DEVMODE * pDevMode = m_pSetup->GetDevMode();

		AddRow(_T("hDevMode->dmDeviceName"),    (const TCHAR *) pDevMode->dmDeviceName);
		AddRow(_T("          dmSpecVersion"),   _T("0x%x"),			pDevMode->dmSpecVersion);
		AddRow(_T("          dmDriverVersion"), _T("0x%x"),			pDevMode->dmDriverVersion);
		AddRow(_T("          dmSize"),          _T("%d bytes"),		pDevMode->dmSize);
		AddRow(_T("          dmDriverExtra"),   _T("%d bytes"),		pDevMode->dmDriverExtra);

		AddRow(_T("          dmOrientation"),   _T("%d"),			pDevMode->dmOrientation);
		AddRow(_T("          dmPaperSize"),     _T("%d"),			pDevMode->dmPaperSize);
		AddRow(_T("          dmPaperLength"),   _T("%d x 100 um"),  pDevMode->dmPaperLength);
		AddRow(_T("          dmPaperWidth"),    _T("%d x 100 um"),  pDevMode->dmPaperWidth);
		AddRow(_T("          dmScale"),         _T("%d %"),         pDevMode->dmScale);
		AddRow(_T("          dmCopies"),        _T("%d copies"),    pDevMode->dmCopies);
		AddRow(_T("          dmDefaultSource"), _T("%d"),           pDevMode->dmDefaultSource);
		AddRow(_T("          dmPrintQuality"),  _T("%d"),           pDevMode->dmPrintQuality);
		AddRow(_T("          dmColor"),         _T("%d"),           pDevMode->dmColor);
		AddRow(_T("          dmDuplex"),        _T("%d"),           pDevMode->dmDuplex);
		AddRow(_T("          dmYResolution"),   _T("%d"),           pDevMode->dmYResolution);
		AddRow(_T("          dmTTOption"),      _T("%d"),           pDevMode->dmTTOption);
		AddRow(_T("          dmCollate"),       _T("%d"),           pDevMode->dmCollate);
		AddRow(_T("          dmFormName"),      (const TCHAR *) pDevMode->dmFormName);

		AddRow(_T("          dmLogPixels"),     _T("%d dpi"),       pDevMode->dmLogPixels);
		AddRow(_T("          dmBitsPerPel"),    _T("%d bpp"),       pDevMode->dmBitsPerPel);
		AddRow(_T("          dmPelsWidth"),     _T("%d pixels"),    pDevMode->dmPelsWidth);
		AddRow(_T("          dmPelsHeight"),    _T("%d pixels"),    pDevMode->dmPelsHeight);
		AddRow(_T("          dmNup"),           _T("%d"),           pDevMode->dmNup);

		AddRow(_T("          dmICMMethod"),     _T("%d"),           pDevMode->dmICMMethod);
		AddRow(_T("          dmICMIntent"),     _T("%d"),           pDevMode->dmICMIntent);
		AddRow(_T("          dmMediaType"),     _T("%d"),           pDevMode->dmMediaType);
		AddRow(_T("          dmDitherType"),    _T("%d"),           pDevMode->dmDitherType);

		POINT sz;
		m_pSetup->GetPaperSize(sz);
		AddRow(_T("PAGESETUPDLG.ptPaperSize"),  _T("{ %d, %d }"),   sz.x, sz.y);

		RECT margin;
		m_pSetup->GetMinMargin(margin);
		AddRow(_T("             rtMinMargin"), _T("( %d, %d, %d, %d }"), 
			margin.left,  margin.top, margin.right, margin.bottom);

		m_pSetup->GetMargin(margin);
		AddRow(_T("             rtMargin"), _T("( %d, %d, %d, %d }"), 
			margin.left,  margin.top, margin.right, margin.bottom);

		HDC hDC = m_pSetup->CreatePrinterDC();

		AddDeviceCaps(hDC, _T("HORZSIZE"),   _T("%d mm"), HORZSIZE);
		AddDeviceCaps(hDC, _T("VERTSIZE"),   _T("%d mm"), VERTSIZE);
		AddDeviceCaps(hDC, _T("HORZRES"),    _T("%d pixel"), HORZRES);
		AddDeviceCaps(hDC, _T("VERTRES"),    _T("%d pixel"), VERTRES);
		AddDeviceCaps(hDC, _T("LOGPIXELSX"), _T("%d dpi"), LOGPIXELSX);
		AddDeviceCaps(hDC, _T("LOGPIXELSY"), _T("%d dpi"), LOGPIXELSY);
		AddDeviceCaps(hDC, _T("BITSPIXEL"),  _T("%d bpp"), BITSPIXEL);
		AddDeviceCaps(hDC, _T("PLANES"),     _T("%d"),     PLANES);

		AddDeviceCaps(hDC, _T("PHYSICALWIDTH"),   _T("%d pixels"), PHYSICALWIDTH); 
		AddDeviceCaps(hDC, _T("PHYSICALHEIGHT"),  _T("%d pixels"), PHYSICALHEIGHT);
		AddDeviceCaps(hDC, _T("PHYSICALOFFSETX"), _T("%d pixels"), PHYSICALOFFSETX);
		AddDeviceCaps(hDC, _T("PHYSICALOFFSETY"), _T("%d pixels"), PHYSICALOFFSETY);

		{
			DWORD ul = CHECKJPEGFORMAT;
								
			if ( ExtEscape(hDC, QUERYESCSUPPORT,  sizeof(ul), (char *) &ul, 0, 0) > 0)
				AddDeviceCaps(hDC, _T("JPEG support"), _T("Yes"), 0);
			else
				AddDeviceCaps(hDC, _T("JPEG support"), _T("No"), 0);
		}
		
		DeleteDC(hDC);
	}

	void ShowProperties(int type)
	{
		HWND hWnd = GetDlgItem(m_hWnd, m_pControlID[2]);

		int index = SendMessage(hWnd, LB_GETCURSEL, 0, 0);

		if ( index<0 )
			index = 0;

		TCHAR PrinterName[MAX_PATH];

		PrinterName[0] = 0;

		SendMessage(hWnd, LB_GETTEXT, index, (LPARAM) PrinterName);

		if ( PrinterName[0]==0 )
			return;

		switch ( type )
		{
			case 0:
				{
					HANDLE hPrinter;
					OpenPrinter(PrinterName, & hPrinter, NULL);
					PrinterProperties(m_hWnd, hPrinter);
					ClosePrinter(hPrinter);
				}
				break;

			case 1:
			case 2:
				{
					DEVMODE * pDevMode = GetDEVMODE(PrinterName, type);

					if ( pDevMode )
						delete [] (BYTE *) pDevMode;
				}
		}
	}

	void AddDeviceCaps(HDC hDC, const TCHAR * name, const TCHAR * format, int index)
	{
		int value = GetDeviceCaps(hDC, index);

		TCHAR temp[128];
		wsprintf(temp, _T("GetDeviceCaps(%s)"), name);

		AddRow(temp, format, value);
	}

    virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
    	switch (uMsg)
	    {
		    case WM_INITDIALOG:
				m_hWnd = hWnd;

				ShowAttributes();
				
				{
					TCHAR temp[MAX_PATH];

					DWORD size = MAX_PATH;
					GetDefaultPrinter(temp, & size);
					SetDlgItemText(hWnd, m_pControlID[1], temp);
				}

				ListPrinters(GetDlgItem(hWnd, m_pControlID[2]));
			    return TRUE;

		    case WM_COMMAND:
				if ( LOWORD(wParam)==m_pControlID[3] )
					ShowProperties(0);
				else if ( LOWORD(wParam)==m_pControlID[4] )
					ShowProperties(1);
				else if ( LOWORD(wParam)==m_pControlID[5] )
					ShowProperties(2);
				else switch ( LOWORD(wParam) )
				{
					case IDOK: 
						EndDialog(hWnd, LOWORD(wParam));
						return TRUE;
				}
	    }

	    return FALSE;
    }

public:
	
	KProperty(KOutputSetup & setup, int nControlID[])
	{
		m_pSetup		 = & setup;
		m_pControlID     = nControlID;
	}
};


void ShowProperty(KOutputSetup & setup, HINSTANCE hInst, int nControlID[], int dialogres)
{
	KProperty property(setup, nControlID);

	property.Dialogbox(hInst, MAKEINTRESOURCE(dialogres));
}
