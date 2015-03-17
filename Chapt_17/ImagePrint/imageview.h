//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : imageview.h					                                     //
//  Description: KImageCanvas class                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KPicture;

class KImageCanvas : public KPageCanvas
{
	KPicture * m_pPicture;

	void GetWndClassEx(WNDCLASSEX & wc)
	{
		KPageCanvas::GetWndClassEx(wc);

		wc.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_PRINT));
	}

public:
	
	virtual void UponDrawPage(HDC hDC, const RECT * rcPaint, int width, int height, int pageno);
	
	virtual int OnCommand(int cmd, HWND hWnd);
	
	KImageCanvas(KPicture * pPicture, HBRUSH hBackground) : KPageCanvas(hBackground)
	{
		m_pPicture = pPicture;
	}

	~KImageCanvas();
};

