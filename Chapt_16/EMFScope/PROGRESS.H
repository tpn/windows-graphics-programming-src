//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : progress.h							                                 //
//  Description: Progress dialog box		                                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KProgress : public KDialog
{
	BOOL bAbort;
	
	int  pos;
	int  reportedpos;
	int  total;
	int  nDelay;
    int  nIdc;
    int  nIdcNumber;

	BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:

	KProgress()
	{
		bAbort		  = FALSE;
		pos           = 0;
		nDelay        = 0;
	}

	BOOL Active(void)
	{
		return m_hWnd!=NULL;
	}

	void SetAbort(void)
	{
		bAbort = TRUE;
	}

	void Delay(void)
	{
		if (nDelay)
			Sleep(nDelay * nDelay);
	}

	BOOL AbortDraw(void);
	void Create(HINSTANCE hInst, HWND hWnd, int idd, int idc, int idcnumber, int delay);
	void Destroy(void);

	void SetRange(unsigned low, unsigned up);
	void Move(void);
};


extern KProgress Progress;
