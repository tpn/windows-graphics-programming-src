//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : playspl.cpp						                                 //
//  Description: Hook for Win9x gdiPlaySpoolStream                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <string.h>

#include "spehon32.h"
#include "playspl.h"
#include "resource.h"

extern "C"
HDC WINAPI gdiPlaySpoolStream(
    LPSTR   lpszDevice,
    LPSTR   lpszOutput,
    LPSTR   lpszSpoolFile,
    DWORD   JobId,
    LPDWORD lpcbBuf,
    HDC     hDC
);


/*
  Header: 1A 1B 1B 53 70 6F 6F 6C					     ...Spool
  DC:	  05 00 00 02 len          data^(len-8)  	
  STD:    53 54 44 02 len		   num1 num2 title^(len-16)
  {
    STP:    53 54 50 02 len        data^(len-8)
    FTM:    46 54 4D 02 len        spool_file name^(len-8)
    ENP:    45 4E 50 02 len        data^(len-8)
  }
  END     45 4E 44 02 len data^(len-8)
*/

/*
000: 1A 1B 1B 53 70 6F 6F 6C                          ...Spool
008: 05 00 00 02 A2 01 00 00
010: 48 50 20 44 65 73 6B 4A  65 74 20 38 37 30 20 53 HP DeskJet 870 S
020: 65 72 69 65 73 00 00 00  00 00 00 00 00 00 00 00 eries...........
030: 00 04 37 03 94 00 06 01  03 FF 80 07 01 00 01 00 ..7.............
040: 00 00 00 00 64 00 01 00  01 00 2C 01 02 00 01 00 ....d.....,.....
050: 2C 01 01 00 00 00 00 00  00 00 00 00 00 00 00 00 ,...............
060: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ................
070: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ................
080: 00 00 00 00 00 00 00 00  00 00 00 00 01 00 00 00 ................
090: 03 00 00 00 01 00 00 00  01 00 00 00 00 00 00 00 ................
0A0: 00 00 00 00 48 50 20 44  65 73 6B 4A 65 74 20 38 ....HP DeskJet 8
0B0: 37 30 20 53 65 72 69 65  73 00 00 00 00 00 00 00 70 Series.......
0C0: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ................
0D0: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ................
0E0: 00 00 00 00 4C 50 54 31  00 00 00 00 00 00 00 00 ....LPT1........
0F0: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ................
100: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ................
110: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ................
120: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ................
130: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ................
140: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ................
150: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ................
160: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 B3 0D ................
170: B0 09 01 00 04 00 02 00  01 00 00 00 00 00 00 00 ................
180: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ................
190: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 ................
1A0: 00 00 00 00 00 00 2C 01  2C 01

1AA: 53 54 44 02 31 00 00 00  10 00 00 00 2B 00 00 00 STD.1.
1BA: 4D 69 63 72 6F 73 6F 66  74 20 57 6F 72 64 20 2D Microsoft Word -
1CA: 20 52 65 66 65 72 2E 64  6F 63 00 4C 50 54 31 3A  Refer.doc.LPT1:
1DA: 00

1DB: 53 54 50 02 10 00 00 00  00 00 02 00 00 00 00 00 STP
1EB: 46 54 4D 02 25 00 00 00  43 3A 5C 77 69 6E 64 6F FTM
1FB: 77 73 5C 54 45 4D 50 5C  7E 45 4D 46 32 32 33 44
20B: 2E 54 4D 50 00
210: 45 4E 50 02 08 00 00 00                          ENP

218: 53 54 50 02 10 00 00 00  01 00 02 00 00 00 00 00 STP
228: 46 54 4D 02 25 00 00 00  43 3A 5C 77 69 6E 64 6F FTM
238: 77 73 5C 54 45 4D 50 5C  7E 45 4D 46 32 34 36 31
248: 2E 54 4D 50 00
24D: 45 4E 50 02 08 00 00 00                          ENP

255: 53 54 50 02 10 00 00 00  02 00 02 00 00 00 00 00 STP
265: 46 54 4D 02 25 00 00 00  43 3A 5C 77 69 6E 64 6F FTM
275: 77 73 5C 54 45 4D 50 5C  7E 45 4D 46 32 35 31 37
285: 2E 54 4D 50 00
28A: 45 4E 50 02 08 00 00 00                          ENP

292: 53 54 50 02 10 00 00 00  03 00 02 00 00 00 00 00 STP
2A2: 46 54 4D 02 25 00 00 00  43 3A 5C 77 69 6E 64 6F FTM
2B2: 77 73 5C 54 45 4D 50 5C  7E 45 4D 46 32 37 30 30
2C2: 2E 54 4D 50 00
2C7: 45 4E 50 02 08 00 00 00                          ENP

2CF: 53 54 50 02 10 00 00 00  04 00 02 00 00 00 00 00 STP
2DF: 46 54 4D 02 25 00 00 00  43 3A 5C 77 69 6E 64 6F FTM
2EF: 77 73 5C 54 45 4D 50 5C  7E 45 4D 46 32 41 35 33
2FF: 2E 54 4D 50 00
304: 45 4E 50 02 08 00 00 00                          ENP

30C: 53 54 50 02 10 00 00 00  05 00 02 00 00 00 00 00 STP
31C: 46 54 4D 02 25 00 00 00  43 3A 5C 77 69 6E 64 6F FTM
32C: 77 73 5C 54 45 4D 50 5C  7E 45 4D 46 31 33 34 35
33C: 2E 54 4D 50 00
341: 45 4E 50 02 08 00 00 00                          ENP

349: 45 4E 44 02 08 00 00 00                          END
351:
*/




int Slient = FALSE;

void Report_Dword(int id, DWORD val)
{
	HWND hwnd_EmfScope;

	if (Slient) return;

	hwnd_EmfScope = (HWND) GetProp(hwnd_Spooler, Prop_Client);

	if (IsWindow(hwnd_EmfScope))
		SendMessage(hwnd_EmfScope, WM_USER, id, val);
}

void CopyData(unsigned typ, void *buffer, int length)
{
	HWND hwnd_EmfScope;

	hwnd_EmfScope = (HWND) GetProp(hwnd_Spooler, Prop_Client);

	if (IsWindow(hwnd_EmfScope))
	{
		COPYDATASTRUCT cd;

		cd.dwData = typ;
		cd.cbData = length;
		cd.lpData = buffer;

		SendMessage(hwnd_EmfScope, WM_COPYDATA, (WPARAM)hwnd_Spooler, (LPARAM)&cd);
	}
}


void Report_String(int id, const char *filename)
{
	HWND  hwnd_EmfScope;
	
	// check for special ItsMe tag to ignore
	if (id==id_title)
		Slient = strncmp(filename, ItsMe, sizeof(ItsMe))==0;
	
	if (Slient) return;

	hwnd_EmfScope = (HWND) GetProp(hwnd_Spooler, Prop_Client); 

	if (IsWindow(hwnd_EmfScope)) 
	{
		DWORD curname[32];
		int   i;

		memset(curname, 0, sizeof(curname));
		strcpy((char *)curname, filename);

		for (i=0; curname[i]; i++)
			SendMessage(hwnd_EmfScope, WM_USER, id, curname[i]);
		
		SendMessage(hwnd_EmfScope, WM_USER, id, 0);
	}		
}


BOOL ListEmfFiles(DWORD *lpcbBuf, LPSTR lpszSpoolFile)
{
	BOOL HasEmf = FALSE;

	// if lpcbBuf!=NULL, lpszSpoolFile is the .spl file data
	if (lpcbBuf)
	{
		DWORD len = *lpcbBuf;

		// check for valid pointer
		if (len>=8 && (!IsBadReadPtr(lpszSpoolFile, len)))
		{
			DWORD    *p;
			BOOL     more;

			// check if we still have page to despool, may reach 'END'
			if (strncmp(lpszSpoolFile, "\x1A\x1B\x1BSpool", 8)==0)
			{
				p = (DWORD *)(lpszSpoolFile + 8);         // first record;
			}
			else
				p = (DWORD *)lpszSpoolFile;

			do 
			{
				more = TRUE;

				if ((*p==REC_DC) || (*p==REC_STD))
					CopyData(*p, p, p[1]);
				
				switch (*p)
				{
					case REC_DC:
					case REC_STP:
					case REC_ENP: 
						p = (DWORD *)((char *)p + p[1]);
						break;

					case REC_FTM:
						Report_String(id_emffile, (LPCTSTR) (p+2)); 
						p = (DWORD *)((char *)p + p[1]);
						HasEmf = TRUE;
						more = FALSE;					// one page at a time, we will be called again
						break;

					case REC_STD:
						Report_String(id_title, (LPCTSTR)(p+4));
						p = (DWORD *)((char *)p + p[1]);
						break;
							
					case REC_END: 
					default:
						more = FALSE;
						break;
				}
			}
			while (more);
		}
	}
	else
	{
		// lpszSpoolFile should be actual .spl file name			
		if (lpszSpoolFile)
		{
			Report_String(id_emffile, lpszSpoolFile);
			HasEmf = TRUE;
		}
	}

	return HasEmf;
}


inline void Report_Activity(int act)
{
	Report_Dword(id_activity, act);
}


HDC WINAPI MygdiPlaySpoolStream(
    LPSTR   lpszDevice,
    LPSTR   lpszOutput,
    LPSTR   lpszSpoolFile,
    DWORD   JobId,
    LPDWORD lpcbBuf,
    HDC     hDC
)
{
	HDC   rslt;
	DWORD timing;
	
	if (lpszDevice)
	{		
		Report_String(id_device, lpszDevice);
		timing = ListEmfFiles(lpcbBuf, lpszSpoolFile);

		Report_Dword(id_activity, act_despool); 
	}
	else
	{
		Report_Dword(id_activity, act_delete);

		timing = 0;
	}
	
	if (timing)
		timing = GetTickCount();

	rslt  = gdiPlaySpoolStream(lpszDevice, lpszOutput, lpszSpoolFile, JobId, lpcbBuf, hDC);

	if (timing)
		Report_Dword(id_time, GetTickCount() - timing);
	
	Report_Dword(id_activity, act_done);

	return rslt;
}


