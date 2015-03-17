//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : progview.h						                                     //
//  Description: KProgrampageCanvas window class                                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

int   CountLine(const char * text, int size);

class KProgramPageCanvas : public KPageCanvas
{
	char * m_pBuffer;
	int    m_nSize;

	HFONT		  m_hFont;
	int			  m_nLineCount;
	int			  m_nLinePerPage;
	KTextFormator m_formator;
	
	void SyntaxHighlight(HDC hDC, int x, int y, const TCHAR * mess);

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KPageCanvas::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_PRINT));
	}

public:
	
	virtual void OnCreate(void);
	virtual void RefreshPaper(void);

	virtual int GetColumnCount(void)
	{
		RECT rect;

		GetClientRect(m_hWnd, & rect);

		int clientwidth = rect.right - rect.left - MARGIN_X;
		int pagewidth   = m_Paper.cx * m_nScale / BASE_DPI + MARGIN_X;

		return max(1, clientwidth / pagewidth);
	}

	virtual int GetPageCount(void)
	{
		if ( m_hFont==NULL )
			return 1;
		else
		{
			m_nLinePerPage = (int) (GetDrawableHeight() / m_formator.GetLinespace());

			if ( m_nLinePerPage<=0 )
				return 1;

			return ( m_nLineCount + m_nLinePerPage-1 ) / m_nLinePerPage;
		}
	}

	virtual void UponDrawPage(HDC hDC, const RECT * rcPaint, int width, int height, int pageno);
	
	virtual int OnCommand(int cmd, HWND hWnd);
	
	KProgramPageCanvas(char * pBuffer, int size, HBRUSH hBackground) : KPageCanvas(hBackground)
	{
		m_pBuffer	 = pBuffer;
		m_nSize		 = size;
		m_nLineCount = CountLine(pBuffer, size);

		m_hFont	 	 = NULL;
	}

	~KProgramPageCanvas()
	{
		if ( m_pBuffer )
		{
			delete [] m_pBuffer;
			m_pBuffer = NULL;
		}

		if ( m_hFont )
			DeleteObject(m_hFont);
	}
};

