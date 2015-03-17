#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : outputsetup.h					                                     //
//  Description: Printer setup and page setup wrapper                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KOutputSetup
{
	PRINTDLG     m_pd;
	PAGESETUPDLG m_psd;

	void Release(void);

public:
	KOutputSetup(void);
	~KOutputSetup(void);

	void DeletePrinterDC(void);
	void SetDefault(HWND hwndOwner, int minpage, int maxpage);
	int  PrintDialog(DWORD flag);
	BOOL PageSetup(DWORD flag);

	void GetPaperSize(POINT & p) const
	{
		p = m_psd.ptPaperSize;
	}

	void GetMargin(RECT & rect) const
	{
		rect = m_psd.rtMargin;
	}

	void GetMinMargin(RECT & rect) const
	{
		rect = m_psd.rtMinMargin;
	}

	HDC GetPrinterDC(void) const
	{
		return m_pd.hDC;
	}

	DEVMODE * GetDevMode(void)
	{
		return (DEVMODE *) GlobalLock(m_pd.hDevMode);
	}

	const TCHAR * GetDriverName(void) const;
	const TCHAR * GetDeviceName(void) const;
	const TCHAR * GetOutputName(void) const;

	HDC CreatePrinterDC(void);
};

void ShowProperty(KOutputSetup & setup, HINSTANCE hInst, int nControlId[], int dialogres);

void ListPrinters(HWND hWnd, int mess = LB_ADDSTRING);