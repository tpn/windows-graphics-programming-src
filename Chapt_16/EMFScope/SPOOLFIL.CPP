//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : spoolfile.cpp						                                 //
//  Description: KSpoolFile class                                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <assert.h>

#include "Winpp.h"

#include "Spoolfil.h"
#include "spehon32\\spehon32.h"
#include "resource.h"


KSpoolFile::KSpoolFile()
{
	strcpy(DevData, "\x1A\x1B\x1BSpool");
			
	size_header  = 8;
	size_devmode = 0;
	size_std     = 0;
}


BOOL KSpoolFile::ReceiveData(COPYDATASTRUCT *cd)
{
	switch (cd->dwData)
	{
		case REC_DC:
			size_devmode = cd->cbData;
			memcpy(&DevData[size_header], cd->lpData, size_devmode);
			break;

		case REC_STD:
			size_std = cd->cbData;
			memcpy(&DevData[size_header+size_devmode], cd->lpData, size_std);
			break;

		default: 
			return FALSE;
	}

	return TRUE;
}

const char frame_stp [] = 
	{ 
		0x53, 0x54, 0x50, 0x02, 0x10, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00
	};

const char frame_frm [] =
	{
		0x46, 0x54, 0x4D, 0x02
	};

const char frame_enp [] =
	{
		0x45, 0x4E, 0x50, 0x02, 0x08, 0x00, 0x00, 0x00
	};

const char frame_end [] = 
	{
		0x45, 0x4E, 0x44, 0x02, 0x08, 0x00, 0x00, 0x00
	};


// get .dev file name from a .emf file name
const char *GetDevFileName(const char *emffilename)
{
	static char   devfilename[MAX_PATH];
	int           len;
	
	len = strlen(emffilename);

	if (len<4) 
		return NULL;
	
	if ( (strcmp(emffilename+len-4, ".emf")!=0) &&
		 (strcmp(emffilename+len-4, ".EMF")!=0) )
		return NULL;

	
	strcpy(devfilename, emffilename);
	devfilename[len-4] = 0;				// remove .emf extension
	strcat(devfilename, ".dev");

	return devfilename;
}


void KSpoolFile::GenDevFile(const char *emffilename)
{
	const char    *devfilename;
	unsigned long dwWritten;
	HANDLE        hdev;

	devfilename = GetDevFileName(emffilename);
	if (devfilename==NULL) 
		return;

	hdev = CreateFile(devfilename, GENERIC_WRITE, 0, NULL, CREATE_NEW,
					  FILE_ATTRIBUTE_NORMAL, NULL);

	if (hdev == INVALID_HANDLE_VALUE) 
		return;

	WriteFile(hdev, DevData, size_header + size_devmode + size_std, &dwWritten, NULL);
	CloseHandle(hdev);
}
