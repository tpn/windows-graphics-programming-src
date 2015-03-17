//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : panose.h						                                     //
//  Description: Panose font matching                                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KPanoseView : public KCanvas
{
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	HMENU			m_hViewMenu;
	HWND			m_hFrame;
	KListView		m_Fonts;

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KCanvas::GetWndClassEx(wc);
	
		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_TEXT));
	}

public:

	typedef enum    { MAX_FONTS = 256 };

	PANOSE			m_Panose[MAX_FONTS];
	TCHAR		    m_TypeFace[MAX_FONTS][32];
	int				m_nFonts;

	KPanoseView()
	{
		m_hViewMenu    = NULL;
		m_hFrame       = NULL;

		m_nFonts       = 0;
	}

	void AddFont(const TCHAR * fullname, HDC hDC);
	void ListPANOSE(void);

	bool Initialize(HINSTANCE hInstance, KStatusWindow * pStatus, HWND hWndFrame);
};
