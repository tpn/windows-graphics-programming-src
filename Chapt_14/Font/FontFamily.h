#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : fontfamily.h					                                     //
//  Description: KEnumFontFaimy class                                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KEnumFontFamily
{
	typedef enum	{ MAX_LOGFONT = 256 };

	KListView * m_pList;

	int static CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, 
							   int FontType, LPARAM lParam) 
	{
		if ( lParam )
			return ((KEnumFontFamily *) lParam)->EnumProc(lpelfe, lpntme, FontType);
		else
			return FALSE;
	} 

public:
	LOGFONT		m_LogFont[MAX_LOGFONT];
	int			m_nLogFont;
	unsigned    m_nType;

	int EnumProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType);

	void EnumFontFamilies(HDC hdc, KListView * pList, 
		BYTE charset = DEFAULT_CHARSET, 
		TCHAR * FaceName = NULL, 
		unsigned type = RASTER_FONTTYPE | TRUETYPE_FONTTYPE | DEVICE_FONTTYPE);
};


class KListViewCanvas : public KCanvas
{
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	HMENU			m_hViewMenu;

	KListView		m_Fonts;
	KEnumFontFamily enumfont;
	int				m_bFamily;
	HWND			m_hFrame;

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KCanvas::GetWndClassEx(wc);
		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_FONT));
	}

public:

	KListViewCanvas(bool bFamily)
	{
		m_hViewMenu    = NULL;
		m_bFamily	   = bFamily;
	}

	bool Initialize(HINSTANCE hInstance, KStatusWindow * pStatus, HWND hWndFrame)
	{
		m_hFrame    = hWndFrame;
		m_hInst     = hInstance;
		m_pStatus   = pStatus;

		RegisterClass(_T("ListViewCanvas"), hInstance);
		
		return true;
	}

	void DecodeFontFile(const TCHAR * fontfile);
};
