//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : imageview.cpp					                                     //
//  Description: KImageCanvas window calss                                           //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#pragma pack(push, 4)
#include <windows.h>
#pragma pack(pop)

#include <assert.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <commdlg.h>

#include "..\..\include\win.h"
#include "..\..\include\outputsetup.h"
#include "..\..\include\pagecanvas.h"
#include "..\..\include\jpeg.h"

#include "resource.h"
#include "imageview.h"


KImageCanvas::~KImageCanvas()
{
	if ( m_pPicture )
		delete [] (BYTE *) m_pPicture;
}

BOOL StretchJPEG(HDC hDC, int x, int y, int w, int h, void * pJPEGImage, unsigned nJPEGSize, int width, int height);

void KImageCanvas::UponDrawPage(HDC hDC, const RECT * rcPaint, int width, int height, int pageno)
{
	if ( (m_pPicture==NULL) && (m_pPicture->m_pBMI==NULL) )
		return;

	int sw   = m_pPicture->GetWidth();
	int sh   = m_pPicture->GetHeight();
	
	int dpix = sw * ONEINCH / width;
	int dpiy = sh * ONEINCH / height;
	
	int dpi  = max(dpix, dpiy);

	int dispwidth  = sw * ONEINCH / dpi;
	int dispheight = sh * ONEINCH / dpi;
	
	SetStretchBltMode(hDC, STRETCH_HALFTONE);

	int x = ( width- dispwidth)/2;
	int y = (height-dispheight)/2;

	if ( StretchJPEG(hDC, x, y, dispwidth, dispheight, 
				m_pPicture->m_pJPEG, m_pPicture->m_nJPEGSize, sw, sh ) )
		return;

	StretchDIBits(hDC, x, y, dispwidth, dispheight, 0, 0, sw, sh, 
		m_pPicture->m_pBits, m_pPicture->m_pBMI, DIB_RGB_COLORS, SRCCOPY);
}

int KImageCanvas::OnCommand(int cmd, HWND hWnd)
{
	switch ( cmd )
	{
		case IDM_VIEW_ZOOM500  : return SetZoom(500);
		case IDM_VIEW_ZOOM400  : return SetZoom(400);
		case IDM_VIEW_ZOOM200  : return SetZoom(200); 
		case IDM_VIEW_ZOOM150  : return SetZoom(150); 
		case IDM_VIEW_ZOOM100  : return SetZoom(100);
		case IDM_VIEW_ZOOM75   : return SetZoom( 75);
		case IDM_VIEW_ZOOM50   : return SetZoom( 50);
		case IDM_VIEW_ZOOM25   : return SetZoom( 25);
		case IDM_VIEW_ZOOM10   : return SetZoom( 10);

		case IDM_FILE_PRINT    : UponFilePrint();     GetDimension(); return View_Resize;
		case IDM_FILE_PAGESETUP: UponFilePageSetup(); GetDimension(); return View_Resize;

		case IDM_FILE_PROPERTY :
			{
				int nControlID[] = { IDC_LIST, IDC_DEFAULT, IDC_PRINTERS, 
					IDC_PRINTERPROPERTIES, IDC_ADVANCEDDOCUMENTPROPERTIES, IDC_DOCUMENTPROPERTIES };
			
				ShowProperty(m_OutputSetup, m_hInst, nControlID, IDD_PROPERTY);
			}
	}

	return View_NoChange;
}


BOOL StretchJPEG(HDC hDC, int x, int y, int w, int h, void * pJPEGImage, unsigned nJPEGSize, int width, int height)
{
	DWORD esc = CHECKJPEGFORMAT;
	if ( ExtEscape(hDC, QUERYESCSUPPORT, sizeof(esc), (char *) &esc, 0, 0) <=0 )
		return FALSE;
	
	DWORD rslt = 0;
	if ( ExtEscape(hDC, CHECKJPEGFORMAT, nJPEGSize, (char *) pJPEGImage, sizeof(rslt), (char *) &rslt) <=0 )
		return FALSE;

	if ( rslt!=1 )
		return FALSE;

	BITMAPINFO bmi;

    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =   width;
    bmi.bmiHeader.biHeight      = - height; // top-down image
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 0;
    bmi.bmiHeader.biCompression = BI_JPEG;
    bmi.bmiHeader.biSizeImage   = nJPEGSize;

	return GDI_ERROR != StretchDIBits(hDC, x, y, w, h, 0, 0, width, height, pJPEGImage, & bmi, DIB_RGB_COLORS, SRCCOPY);
}
