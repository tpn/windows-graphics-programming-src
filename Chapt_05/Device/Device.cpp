//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : device.cpp				                                             //
//  Description: Examine Device Context, Chapter 5                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define NOCRYPT

#include <windows.h>
#include <tchar.h>
#include "resource.h"

#include "..\\..\\include\\property.h"
#include "DevPage.h"


class KMain : public KPropertySheet
{
	KDevicePage * m_Device;

public: 

	KMain(void)
	{
		m_Device = NULL;
	}

	~KMain(void)
	{
		if ( m_Device )
		{
			delete m_Device;
			m_Device = NULL;
		}
	}

	void Run(HINSTANCE hInst)
	{
        HPROPSHEETPAGE hPage[3];

		m_Device = new KDevicePage(hInst);
		hPage[0] = m_Device->createPropertySheetPage(hInst, IDD_DEVICEPAGE);

		propertySheet(hInst, NULL, 0, 1, hPage, "Device");
	}
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	KMain    main;
    
/*	HDC hDC1 = CreateDCW(L"DISPLAY", NULL, NULL, NULL);

	HDC hDC2 = CreateDCW(L"DISPLAY", L"\\\\.\\DISPLAY1", NULL, NULL);
	HDC hDC3 = CreateDCW(NULL, L"\\\\.\\DISPLAY1", NULL, NULL);
	HDC hDC4 = CreateDCW(NULL, L"\\\\.\\DISPLAY3", NULL, NULL);

	DEVMODEW Dm;

	Dm.dmSize        = sizeof(Dm);
	Dm.dmDriverExtra = 0;

	HDC hDC5 = CreateDCW(NULL, L"HP DeskJet 895Cxi", NULL, & Dm);
	
	DeleteDC(hDC1);
*/
	InitCommonControls();

	main.Run(hInstance);

	return TRUE;
}	
