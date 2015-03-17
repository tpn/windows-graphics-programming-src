//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : emf.cpp						                                     //
//  Description: Generic enhanced metafile handling                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <math.h>
#include <commdlg.h>
#include <stdio.h>

#include "emf.h"
#include "filedialog.h"


// map a rectangle in logical coordinate to 0.01 mm units
void Map10um(HDC hDC, RECT & rect)
{
	int widthmm    = GetDeviceCaps(hDC, HORZSIZE);
	int heightmm   = GetDeviceCaps(hDC, VERTSIZE);
	int widthpixel = GetDeviceCaps(hDC, HORZRES);
	int heightpixel= GetDeviceCaps(hDC, VERTRES);

	LPtoDP(hDC, (POINT *) & rect, 2);

	rect.left   = ( rect.left   * widthmm  * 100 + widthpixel/2)  /  widthpixel;
	rect.right  = ( rect.right  * widthmm  * 100 + widthpixel/2)  /  widthpixel;
	rect.top    = ( rect.top    * heightmm * 100 + heightpixel/2) / heightpixel;
	rect.bottom = ( rect.bottom * heightmm * 100 + heightpixel/2) / heightpixel;
}


// load an EMF attached as a resource, use RCDATA as type
HENHMETAFILE LoadEMF(HMODULE hModule, LPCTSTR pName)
{
	HRSRC hRsc       = FindResource(hModule, pName, RT_RCDATA);
		
	if ( hRsc==NULL )
		return NULL;

	HGLOBAL hResData = LoadResource(hModule, hRsc);
	LPVOID  pEmf     = LockResource(hResData);

	return SetEnhMetaFileBits(SizeofResource(hModule, hRsc), (const BYTE *) pEmf);
}


// query for EMF dimension, convert to logical coordinate
void GetEMFDimension(HDC hDC, HENHMETAFILE hEmf, int & width, int & height)
{
	ENHMETAHEADER emfh[5]; // big enough for description
	GetEnhMetaFileHeader(hEmf, sizeof(emfh), emfh);

	// picture size in 0.01 mm
	width  = emfh[0].rclFrame.right  - emfh[0].rclFrame.left;
	height = emfh[0].rclFrame.bottom - emfh[0].rclFrame.top;

	// convert to pixels for current device
	int  t = GetDeviceCaps(hDC, HORZSIZE) * 100; 
	width  = ( width  * GetDeviceCaps(hDC, HORZRES) + t/2 ) / t;

	     t = GetDeviceCaps(hDC, VERTSIZE) * 100;
	height = ( height * GetDeviceCaps(hDC, VERTRES) + t/2 ) / t;

	RECT rect = { 0, 0, width, height };

	// convert to logical coordinate space
	DPtoLP(hDC, (POINT *) & rect, 2);

	width  = abs(rect.right - rect.left);
	height = abs(rect.bottom - rect.top);
}


// displaying EMF at a given ratio
BOOL DisplayEMF(HDC hDC, HENHMETAFILE hEmf, int x, int y, int scalex, int scaley, KEnumEMF * enumerator, bool showxform)
{
	int width, height;
	GetEMFDimension(hDC, hEmf, width, height);

	RECT rect = { x, y, 
		x + (width  * scalex + 50)/100, 
		y + (height * scaley + 50)/100 };

	BOOL rslt;

	if ( enumerator )
		rslt = enumerator->EnumEMF(hDC, hEmf, & rect);
	else
		rslt = PlayEnhMetaFile(hDC, hEmf, & rect);

	if ( showxform )
	{
		XFORM xform;

		if ( GetPlayTransformation(hEmf, & rect, xform) )
		{
			TCHAR mess[128];

			_stprintf(mess, _T("x' = x * %8.5f + y * %8.5f + %8.5f"), xform.eM11, xform.eM21, xform.eDx);
			TextOut(hDC, x, rect.bottom + 10, mess, _tcslen(mess));

			_stprintf(mess, _T("y' = x * %8.5f + y * %8.5f + %8.5f"), xform.eM12, xform.eM22, xform.eDy);
			TextOut(hDC, x, rect.bottom + 40, mess, _tcslen(mess));
		}
	}

	return rslt;
}


HDC QuerySaveEMFFile(const TCHAR * description, const RECT * rcFrame, TCHAR szFileName[])
{
	KFileDialog fd;

	if ( fd.GetSaveFileName(NULL, _T("emf"), _T("Enhanced Metafiles")) )
	{
		if ( szFileName )
			_tcscpy(szFileName, fd.m_TitleName);

		return CreateEnhMetaFile(NULL, fd.m_TitleName, rcFrame, description);
	}
	else
		return NULL;
}


HENHMETAFILE QueryOpenEMFFile(TCHAR szFileName[])
{
	KFileDialog fd;

	if ( fd.GetOpenFileName(NULL, _T("emf|spl"), _T("Enhanced Metafiles|Win NT/2000 EMF Spool Files")) )
	{
		if ( szFileName )
			_tcscpy(szFileName, fd.m_TitleName);
		
		return  GetEnhMetaFile(fd.m_TitleName);
	}
	else
		return NULL;
}


HENHMETAFILE PasteFromClipboard(HWND hWnd)
{
	HENHMETAFILE hEmf = NULL;

	if ( OpenClipboard(hWnd) )
	{
		hEmf = (HENHMETAFILE) GetClipboardData(CF_ENHMETAFILE);

		if ( hEmf )
			hEmf = CopyEnhMetaFile(hEmf, NULL);

		CloseClipboard();
	}

	return hEmf;
}


void CopyToClipboard(HWND hWnd, HENHMETAFILE hEmf)
{
	if ( OpenClipboard(hWnd) )
	{
		EmptyClipboard();
		SetClipboardData(CF_ENHMETAFILE, hEmf);
		CloseClipboard();
	}
}



HPALETTE GetEMFPalette(HENHMETAFILE hEmf, HDC hDC)
{
	// query number of entries
	int no = GetEnhMetaFilePaletteEntries(hEmf, 0, NULL);

	if ( no<=0 )
		return NULL;

	// allocation
	LOGPALETTE * pLogPalette = (LOGPALETTE *) new BYTE[sizeof(LOGPALETTE) + (no-1) * sizeof(PALETTEENTRY)];

	pLogPalette->palVersion    = 0x300;
	pLogPalette->palNumEntries = no;

	// get real data
	GetEnhMetaFilePaletteEntries(hEmf, no, pLogPalette->palPalEntry);

	HPALETTE hPal = CreatePalette(pLogPalette);

	delete [] (BYTE *) pLogPalette;

	return hPal;
}


// Transformation to map recording device's device coordinate space to destination device's logical coordinate space
BOOL GetPlayTransformation(HENHMETAFILE hEmf, const RECT * rcPic, XFORM & xformPlay)
{
	ENHMETAHEADER emfh; 
	
	if ( GetEnhMetaFileHeader(hEmf, sizeof(emfh), & emfh)<=0 )
		return FALSE;

	try
	{
		// frame rectangle size in 0.01 mm -> 1 mm -> percentage -> device pixels
		double sx0 = emfh.rclFrame.left   / 100.0 / emfh.szlMillimeters.cx * emfh.szlDevice.cx;
		double sy0 = emfh.rclFrame.top    / 100.0 / emfh.szlMillimeters.cy * emfh.szlDevice.cy;
		double sx1 = emfh.rclFrame.right  / 100.0 / emfh.szlMillimeters.cx * emfh.szlDevice.cx;
		double sy1 = emfh.rclFrame.bottom / 100.0 / emfh.szlMillimeters.cy * emfh.szlDevice.cy;

		// source to destination ratio
		double rx  = (rcPic->right  - rcPic->left) / ( sx1 - sx0 );
		double ry  = (rcPic->bottom - rcPic->top)  / ( sy1 - sy0 );

		// x' = x * eM11 + y * eM21 + eDx
		// y' = x * eM12 + y * eM22 + eDy
		xformPlay.eM11 = (float) rx;
		xformPlay.eM21 = (float) 0;
		xformPlay.eDx  = (float) (- sx0 * rx + rcPic->left);

		xformPlay.eM12 = (float) 0;
		xformPlay.eM22 = (float) ry;
		xformPlay.eDy  = (float) (- sy0 * ry + rcPic->top);
	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}
