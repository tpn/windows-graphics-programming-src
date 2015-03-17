//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : device.cpp					                                         //
//  Description: KDevice class for HTML/BMP printer driver, Chapter 2                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define _WIN32_WINNT 0x0500 
//#define  WINVER      0x0500 
#define NOCRYPT

#define UNICODE
#define _UNICODE
#define _X86_ 1
#define WINNT 1 

#include <windows.h>
#include <winddi.h>

#include "Device.h"


const PAIR Pair_DDIFunction[] =
{
    INDEX_DrvEnablePDEV,       "DrvEnablePDEV",
    INDEX_DrvCompletePDEV,     "DrvCompletePDEV",
    INDEX_DrvResetPDEV,        "DrvResetPDEV",
    INDEX_DrvDisablePDEV,      "DrvDisablePDEV",
    INDEX_DrvEnableSurface,    "DrvEnableSurface",
    INDEX_DrvDisableSurface,   "DrvDisableSurface",

    INDEX_DrvStartDoc,         "DrvStartDoc",
    INDEX_DrvEndDoc,           "DrvEndDoc",
    INDEX_DrvStartPage,        "DrvStartPage",
    INDEX_DrvSendPage,         "DrvSendPage",

    INDEX_DrvStrokePath,       "DrvStrokePath",
    INDEX_DrvFillPath,         "DrvFillPath",
    INDEX_DrvStrokeAndFillPath,"DrvStrokeAndFillPath",
    INDEX_DrvLineTo,           "DrvLineTo",
    INDEX_DrvPaint,            "DrvPaint",
    INDEX_DrvBitBlt,           "DrvBitBlt",
    INDEX_DrvCopyBits,         "DrvCopyBits",
    INDEX_DrvStretchBlt,       "DrvStretchBlt",
    INDEX_DrvTextOut,          "DrvTextOut",
	0,						   NULL
};


void KDevice::Write(const char * pStr)
{
	DWORD dwWritten;

	EngWritePrinter(hSpooler, (void *) pStr, strlen(pStr), & dwWritten);
}


void KDevice::WriteW(const WCHAR * pwStr)
{
	int len = wcslen(pwStr);

	if ( len )
	{
		char  Ansi[512];
		DWORD dwLen;

		EngUnicodeToMultiByteN(Ansi, sizeof(Ansi),
							   & dwLen,
							   (WCHAR *) pwStr, (len + 1) * sizeof(WCHAR));

		EngWritePrinter(hSpooler, Ansi, dwLen, & dwLen);
	}
}


void KDevice::Writeln(const char * pStr)
{
	char crlf[2] = { 0x0D, 0x0A };

	DWORD dwWritten;

	if ( pStr )
		EngWritePrinter(hSpooler, (void *) pStr, strlen(pStr), & dwWritten);
	
	EngWritePrinter(hSpooler, crlf, 2, & dwWritten);
}


void KDevice::Write(DWORD index, const PAIR * pTable)
{
	while ( pTable->name )
		if ( index==pTable->index )
		{
			Write(pTable->name);
			return;
		}
		else
			pTable ++;

	Write("Unknown index");
}


void KDevice::WriteHex(unsigned val)
{
	char text[8];

	int i = 8;

	do
	{
		i --;
		text[i] = (val & 15) + '0';

		if ( text[i]>'9' )
			text[i] += 'a' - '9' -1;
		
		val /= 16;
	}
	while ( val!=0 );

	DWORD dwWritten;

	EngWritePrinter(hSpooler, text+i, 8-i, & dwWritten);
}


BOOL KDevice::StartDoc(LPCWSTR pwszDocName, const void * firstpara, int parano)
{
	if ( (this!=NULL) && (nNesting==0) )
	{
		nNesting = 1;

		Writeln("<html>");
		
		Writeln("<head>");
		Write("<title>");
		WriteW(pwszDocName);
		Writeln("</title>");
		Writeln("</head>");

		Writeln("<body bgcolor=""""#80B090""""><font size=1>");
		Writeln("<ol>");

		LogCall(INDEX_DrvStartDoc, firstpara, parano);
		
		return TRUE;
	}
	else
		return FALSE;
}


BOOL KDevice::EndDoc(const void * firstpara, int parano)
{
	if ( (this!=NULL) && (nNesting==1) )
    {
        nNesting = 0;

		LogCall(INDEX_DrvEndDoc, firstpara, parano);
		Writeln("</ol>");
		Writeln("</body>");
		Writeln("</html>");

		return TRUE;
	}
	else
		return FALSE;
}


BOOL KDevice::StartPage(const void * firstpara, int parano)
{
	LogCall(INDEX_DrvStartPage, firstpara, parano);

    if ( nNesting==1 )
    {
        nNesting ++;
        return TRUE;
	}
	else
		return FALSE;
}


BOOL KDevice::SendPage(const void * firstpara, int parano)
{
    if ( CallEngine(INDEX_DrvSendPage, firstpara, parano) )
    {
        nPages ++;
		nNesting = 1;
        
        return TRUE;
    }
    else
        return FALSE;
}


void KDevice::LogCall(int index, const void * firstpara, int parano)
{
	Write("<li>");
	Write(index, Pair_DDIFunction);
	Write("(");

	const unsigned * pDWORD = (const unsigned *) firstpara;

	for (int i=0; i<parano; i++)
	{
		WriteHex(pDWORD[i]);
		
		if ( i!=(parano-1) )
			Write(", ");
	}
	Writeln(")</li>");
}


BOOL KDevice::CallEngine(int index, const void * firstpara, int parano)
{
	if ( this==NULL )
	{
		EngSetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	LogCall(index, firstpara, parano);

	return nNesting==2;
}


void KDevice::DumpSurface(const SURFOBJ * pso)
{
	WCHAR szFileName[32] = L"\\??\\c:\\htmd_000.bmp"; // fully qualified name
	
	szFileName[12] = '0' + (nImage / 100);
	szFileName[13] = '0' + ((nImage / 10) % 10);
	szFileName[14] = '0' + (nImage % 10);
	nImage ++;

	ULONG_PTR   giFile = NULL;

	char * pFile = (char *) EngMapFile(szFileName, 
		sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pso->cjBits,
		& giFile);

	if ( pFile )
	{
		CopySurface(pFile, pso);
		EngUnmapFile(giFile);
	}
	else
		wcscpy(szFileName, L"failed.bmp");

	Write("<p><img src=\"");
	WriteW(szFileName+4);	// remove "\??\"
	Writeln("\"></p>");
}


char * KDevice::CopyBlock(char * pDest, void * pData, int size)
{
	if ( pDest )
	{
		memcpy(pDest, pData, size);
		return pDest + size;
	}
	else
	{
		DWORD dwWritten;
		
		EngWritePrinter(hSpooler, pData, size, &dwWritten);
		return NULL;
	}
}


void KDevice::CopySurface(char * pDest, const SURFOBJ * pso)
{
	// Write BITMAPFILEHEADER data.
	{
		BITMAPFILEHEADER bmfhead;

		bmfhead.bfType		= 0x4D42;
		bmfhead.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); 
		bmfhead.bfSize		= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pso->cjBits;
		bmfhead.bfReserved1 = 0;
		bmfhead.bfReserved2 = 0;

		pDest = CopyBlock(pDest, & bmfhead, sizeof(BITMAPFILEHEADER));
	}

	// Write BITMAPHEADER
	{
		BITMAPINFOHEADER bmihead;

		bmihead.biSize          = sizeof(BITMAPINFOHEADER);
		bmihead.biWidth         = pso->sizlBitmap.cx;
		bmihead.biHeight        = (pso->fjBitmap & BMF_TOPDOWN) ? - pso->sizlBitmap.cy 
			                                                    :   pso->sizlBitmap.cy;
		bmihead.biPlanes        = 1;
		bmihead.biBitCount      = 24;
		bmihead.biCompression   = BI_RGB;
		bmihead.biSizeImage     = 0;
		bmihead.biXPelsPerMeter = 0;
		bmihead.biYPelsPerMeter = 0;
		bmihead.biClrUsed       = 0;
		bmihead.biClrImportant  = 0;

		pDest = CopyBlock(pDest, &bmihead, sizeof(BITMAPINFOHEADER));
	}
    
    CopyBlock(pDest, pso->pvBits, pso->cjBits);
}

