//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : pixelspeed.cpp					                                     //
//  Description: GDI pixel drawing speed test                                        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>

#include "..\..\include\timer.h"
#include "..\..\include\Dialog.h"
#include "..\..\include\ListView.h"

#include "resource.h"

class KQuickTimer : public KTimer
{
public:
	unsigned cpuspeed10;

	KQuickTimer()
	{
		Start(); 
		Sleep(1000); 
    
		cpuspeed10 = (unsigned)(Stop()/100000);
	}

	void Query(TCHAR speed[], TCHAR overhead[])
	{
		wsprintf(speed, "%d.%d Mhz", cpuspeed10 / 10,  cpuspeed10 % 10);
		wsprintf(overhead, "%d ns", (unsigned) m_overhead);
	}
};

class KSpeedTest : public KDialog
{
	KListView   report;
	KQuickTimer timer;
	unsigned    measurement[5];

	void Measure(void);

	void Report(unsigned time, TCHAR * pTest)
	{
		report.AddItem(0, pTest);
		report.AddItem(1, time);		// clock cycle count

		time = time * 10000 / timer.cpuspeed10; // ns
		report.AddItem(2, time);

		if ( time )
			report.AddItem(3, 1000 * 1000 * 1000 / time); // per-second
	}

    virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
    	switch (uMsg)
	    {
		    case WM_INITDIALOG:
			    m_hWnd = hWnd;

				report.FromDlgItem(hWnd, IDC_REPORT);

				report.AddColumn(0, 120, "Test");
				report.AddColumn(1,  80, "clock");
				report.AddColumn(2,  80, "nanosec");
				report.AddColumn(3,  80, "per-sec");
				
				{
					TCHAR speed[32], overhead[32];
					timer.Query(speed, overhead);
					SetDlgItemText(hWnd, IDC_CPU,      speed);
					SetDlgItemText(hWnd, IDC_OVERHEAD, overhead);
				}

				Measure();
				Report(measurement[0]/10000, "SetPixel(RGB)");
				Report(measurement[1]/10000, "SetPixel(PALETTERGB)");
				Report(measurement[2]/10000, "SetPixel(PALETTEINDEX)");
				Report(measurement[3]/10000, "SetPixelV(RGB)");
				Report(measurement[4]/10000, "GetPixel()");
	
				return TRUE;

		    case WM_COMMAND:
				switch ( LOWORD(wParam) )
				{
					case IDOK:
						EndDialog(hWnd, 1);
						return TRUE;

					case IDC_TU:
						;
				}
	    }

	    return FALSE;
    }

};


void KSpeedTest::Measure(void)
{
    HDC hDC = GetDC(NULL);

	HPALETTE hPalette = CreateHalftonePalette(hDC);
	HPALETTE hOld     = SelectPalette(hDC, hPalette, FALSE);
	RealizePalette(hDC);

	for (int test=0; test<5; test++)
	{
		int i, j;

		timer.Start(); 
    
		switch ( test )
		{
			case 0:
				for (i=0; i<100; i++)
				for (j=0; j<100; j++)
					SetPixel(hDC, i, j, RGB(i, j, i+i));
				break;

			case 1:
				for (i=0; i<100; i++)
				for (j=0; j<100; j++)
					SetPixel(hDC, i, j, PALETTERGB(i, j, i+i));
				break;

			case 2:
				for (i=0; i<100; i++)
				for (j=0; j<100; j++)
					SetPixel(hDC, i, j, PALETTEINDEX(i));
				break;

			case 3:
				for (i=0; i<100; i++)
				for (j=0; j<100; j++)
					SetPixelV(hDC, i, j, RGB(i, j, i+i));
				break;

			case 4:
				for (i=0; i<100; i++)
				for (j=0; j<100; j++)
					GetPixel(hDC, i, j);
				break;
		}

		measurement[test] = (unsigned) timer.Stop();
	}

	SelectObject(hDC, hOld);
	DeleteObject(hPalette);

	ReleaseDC(NULL, hDC);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	KSpeedTest speed;

	return speed.Dialogbox(hInstance, MAKEINTRESOURCE(IDD_SPEED));
}
