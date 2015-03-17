//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : emfview.h			                                                 //
//  Description: EMF displaying window                                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KEMFPanel : public KWindow
{
public:
	HENHMETAFILE    m_hEmf;
	HPALETTE		m_hPalette;
	int				m_zoom;
	int				m_mode;

	virtual void OnDraw(HDC hDC);

	KEMFPanel()
	{
		m_hEmf	    = NULL;
		m_hPalette  = NULL;
		m_zoom      = 100;
		m_mode	    = 0;
	}

	~KEMFPanel()
	{
		if ( m_hEmf )
		{
			DeleteEnhMetaFile(m_hEmf);
			m_hEmf = NULL;
		}
	}

	void SetZoom(int newzoom, bool bForce=false);

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		memset(& wc, 0, sizeof(wc));

		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = WindowProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;       
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH) (COLOR_INFOBK + 1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = NULL;
		wc.hIconSm       = NULL;
	}

};


class KEMFView : public KCanvas
{
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	HMENU			m_hViewMenu;
	HWND			m_hFrame;
	KTreeView		m_emftree;
	KEMFPanel		m_emfpanel;
	int				m_nDivision;
	TCHAR			m_FileName[MAX_PATH];

public:

	KEMFView()
	{
		m_hViewMenu    = NULL;
		m_hFrame       = NULL;
		m_nDivision    = 280;
	}

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		memset(& wc, 0, sizeof(wc));

		wc.cbSize        = sizeof(WNDCLASSEX);
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = WindowProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;       
		wc.hInstance     = NULL;
		wc.hIcon         = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_GRAPH));
		wc.hCursor       = LoadCursor(NULL, IDC_SIZEWE);
		wc.hbrBackground = (HBRUSH) (COLOR_WINDOWFRAME + 1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = NULL;
		wc.hIconSm       = NULL;
	}

	bool Initialize(HINSTANCE hInstance, KStatusWindow * pStatus, HWND hWndFrame, HENHMETAFILE hEmf,
		const TCHAR * pFileName);
};
