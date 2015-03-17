//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : printerdevice.cpp				                                     //
//  Description: Printer device context demo program, Chapter 17                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define NOCRYPT

#include <windows.h>
#include <tchar.h>
#include "resource.h"

#include "..\\..\\include\\property.h"
#include "..\\..\\include\\listview.h"

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

		propertySheet(hInst, NULL, IDI_PRINT, 1, hPage, "Printer Device");
	}
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	KMain    main;
    
	InitCommonControls();

	main.Run(hInstance);

	return TRUE;
}	
