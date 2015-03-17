//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : emfview.cpp					                                     //
//  Description: KEMFView class								                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define INITGUID
#define _WIN32_WINNT 0x0500
#define WINVER		 0x0500
#define STRICT

#pragma pack(push, 4)
#include <windows.h>
#pragma pack(pop)

#include <tchar.h>
#include <assert.h>
#include <fstream.h>
#include <commctrl.h>
#include <fstream.h>

#include "..\..\include\win.h"
#include "..\..\include\canvas.h"
#include "..\..\include\mmfile.h"
#include "..\..\include\treeview.h"
#include "..\..\include\emf.h"
#include "..\..\include\filedialog.h"
#include "..\..\include\LogWindow.h"

#include "resource.h"
#include "EMFView.h"
#include "EmfDC.h"


int SimpleEnumerateEMF(HENHMETAFILE hEmf, ofstream & stream)
{
	int size = GetEnhMetaFileBits(hEmf, 0, NULL);

	if ( size<=0 )
		return 0;

	BYTE * pBuffer = new BYTE[size];

	if ( pBuffer==NULL )
		return 0;

	GetEnhMetaFileBits(hEmf, size, pBuffer);

	const EMR * emr = (const EMR *) pBuffer;

	TCHAR mess[MAX_PATH];

	int recno = 0;

	// enumuerate all EMF records
	while ( (emr->iType>=EMR_MIN) && (emr->iType<=EMR_MAX) )
	{
		recno ++;

		wsprintf(mess, "%3d: EMR_%03d (%4d bytes)\n", recno, emr->iType, emr->nSize);

		stream << mess;

		if ( emr->iType== EMR_EOF )
			break;

		emr = (const EMR *) ( ( const char * ) emr + emr->nSize );
	}

	delete [] pBuffer;

	return recno;
}

/////////////////////////////////////////////////////

class KTraceEMF : public KEnumEMF
{
	int			  m_nSeq;
	KEmfDC		  m_emfdc;

	int			  m_value[32];
	HGDIOBJ		  m_object[8];
	FLOAT		  m_float[8];

	// virtual function to process every EMF record, return 0 to terminate
	virtual int ProcessRecord(HDC hDC, HANDLETABLE * pHTable, const ENHMETARECORD * pEMFR, int nObj)
	{
		CompareDC(hDC);

		m_pLog->Log("%4d: %08x %3d % 6d ", m_nSeq++, pEMFR, pEMFR->iType, pEMFR->nSize);

		m_pLog->Log(m_emfdc.DecodeRecord((const EMR *) pEMFR));
		m_pLog->Log("\r\n");

		return PlayEnhMetaFileRecord(hDC, pHTable, pEMFR, nObj);
	}

public:
	KLogWindow  * m_pLog;

	void CompareDC(HDC hDC);

	void Show(bool bShow)
	{
		m_pLog->ShowWindow(bShow);
	}

	KTraceEMF(HINSTANCE hInst)
	{
		m_pLog = new KLogWindow; // allocated here, deallocated in WM_NCDESTROY handling

		m_pLog->Create(hInst, "EMF Trace");

		m_nSeq = 1;

		memset(m_value,  0xCD, sizeof(m_value));
		memset(m_object, 0xCD, sizeof(m_object));
		memset(m_float,  0xCD, sizeof(m_float));
	}
};


#define Compare(value, exp, mess) if (value!=exp) { value = exp; m_pLog->Log(mess, value); }

void KTraceEMF::CompareDC(HDC hDC)
{
	Compare(m_value[0],  GetMapMode(hDC),						"MapMode      : %d\r\n");
	Compare(m_value[1],  GetGraphicsMode(hDC),					"GraphicsMode : %d\r\n");
	
	XFORM xm;
	GetWorldTransform(hDC, & xm);
	Compare(m_float[0],  xm.eM11,								"WT.eM11      : %8.5f\r\n");
	Compare(m_float[1],  xm.eM12,								"WT.eM12      : %8.5f\r\n");
	Compare(m_float[2],  xm.eM21,								"WT.eM21      : %8.5f\r\n");
	Compare(m_float[3],  xm.eM22,								"WT.eM22      : %8.5f\r\n");
	Compare(m_float[4],  xm.eDx,								"WT.eDx       : %8.5f\r\n");
	Compare(m_float[5],  xm.eDy,								"WT.eDy       : %8.5f\r\n");

	Compare(m_value[2],  GetBkMode(hDC),						"BkMode       : %d\r\n");
	Compare(m_value[3],  GetROP2(hDC),							"ROP2         : %d\r\n");
	Compare(m_value[4],  ((int)GetTextAlign(hDC)),				"TextAlign    : 0x%x\r\n");

	Compare(m_object[0], GetCurrentObject(hDC, OBJ_PEN),		"Pen          : 0x%08x\r\n");
	Compare(m_object[1], GetCurrentObject(hDC, OBJ_BRUSH),		"Brush        : 0x%08x\r\n");
	Compare(m_object[2], GetCurrentObject(hDC, OBJ_FONT),		"Font         : 0x%08x\r\n");
	Compare(m_object[3], GetCurrentObject(hDC, OBJ_PAL),		"Palette      : 0x%08x\r\n");
	Compare(m_object[4], GetCurrentObject(hDC, OBJ_COLORSPACE),	"ColorSpace   : 0x%08x\r\n");
	Compare(m_object[5], GetCurrentObject(hDC, OBJ_BITMAP),		"Bitmap       : 0x%08x\r\n");
}


/////////////////////////////////////////////////////////////////
inline void MaptoGray(COLORREF & cr)
{
	if ( (cr & 0xFF000000) != PALETTEINDEX(0) ) // not paletteindex
	{
		BYTE gray = ( GetRValue(cr) * 77 + GetGValue(cr) * 150 + GetBValue(cr) * 29 + 128 ) / 256;
	
		cr = (cr & 0xFF000000) | RGB(gray, gray, gray);
	}
}


class KGrayEMF : public KEnumEMF
{
	// virtual function to process every EMF record, return 0 to terminate
	virtual int ProcessRecord(HDC hDC, HANDLETABLE * pHTable, const ENHMETARECORD * pEMFR, int nObj)
	{
		int rslt;

		switch ( pEMFR->iType )
		{
			case EMR_CREATEBRUSHINDIRECT:
				{
					EMRCREATEBRUSHINDIRECT cbi;

					cbi = * (const EMRCREATEBRUSHINDIRECT *) pEMFR;
					MaptoGray(cbi.lb.lbColor);
				
					rslt = PlayEnhMetaFileRecord(hDC, pHTable, (const ENHMETARECORD *) & cbi, nObj);
				}
				break;

			case EMR_CREATEPEN:
				{
					EMRCREATEPEN cp;

					cp = * (const EMRCREATEPEN *) pEMFR;
					MaptoGray(cp.lopn.lopnColor);

					rslt = PlayEnhMetaFileRecord(hDC, pHTable, (const ENHMETARECORD *) & cp, nObj);
				}
				break;

			case EMR_SETTEXTCOLOR:
			case EMR_SETBKCOLOR:
				{
					EMRSETTEXTCOLOR stc;

					stc = * (const EMRSETTEXTCOLOR *) pEMFR;
					MaptoGray(stc.crColor);
				
					rslt = PlayEnhMetaFileRecord(hDC, pHTable, (const ENHMETARECORD *) & stc, nObj);
				}
				break;
			
			case EMR_RESERVED_105:
			case EMR_RESERVED_106:
			case EMR_RESERVED_107:
			case EMR_RESERVED_108:
			case EMR_RESERVED_109:
			case EMR_RESERVED_110:
			case EMR_RESERVED_119:
			case EMR_RESERVED_120:
				rslt = PlayEnhMetaFileRecord(hDC, pHTable, pEMFR, nObj);
				break;

			default:
				rslt = PlayEnhMetaFileRecord(hDC, pHTable, pEMFR, nObj);
		}

		return rslt;
	}
};

/*

105 ESCAPE
106 ESCAPE
107 STARTDOC
108 SMALLTEXTOUT
109 FORCEUFIMAPPING
110 NAMEDESCAPE
111 COLORCORRECTPALETTE
112 SETICMPROFILE
113 RSETICMPROFILE
114 ALPHABLEND
115 SETLAYOUT
116 TRANSPARENTBLT
117 ? 
118 GRADIENTFILL
119 SETLINKEDUFIS
120 MRSETTEXTJUSTIFICATION
121 COLORMATCHTOTARGET
122 CREATECOLORSPACEW
*/

//////////////////////////////////////////////////////////

class KDelayEMF : public KEnumEMF
{
	int m_delay;

	// virtual function to process every EMF record, return 0 to terminate
	virtual int ProcessRecord(HDC hDC, HANDLETABLE * pHTable, const ENHMETARECORD * pEMFR, int nObj)
	{
		Sleep(m_delay);

//		MSG msg;

//		while ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}

		return PlayEnhMetaFileRecord(hDC, pHTable, pEMFR, nObj);
	}

public:
	KDelayEMF(int delay)
	{
		m_delay = delay;
	}
};


///////////////////////////////////////////////////////////

void KEMFPanel::SetZoom(int newzoom, bool bForce)
{
	if ( ! bForce )
	{
		if ( newzoom==m_zoom )
			return;

		m_zoom = newzoom;
	}

	InvalidateRect(m_hWnd, NULL, TRUE);
}


void KEMFPanel::OnDraw(HDC hDC)
{
	HPALETTE hOld = NULL;

	if ( m_hPalette )
	{
		hOld = SelectPalette(hDC, m_hPalette, FALSE);
		int no = RealizePalette(hDC);

		no ++;
	}

	switch ( m_mode )
	{
		case 1:
			{
				KDelayEMF delay(5);

				DisplayEMF(hDC, m_hEmf, 5, 5, m_zoom, m_zoom, & delay, true);
			}
			break;

		case 2:
			{
				KGrayEMF gray;

				DisplayEMF(hDC, m_hEmf, 5, 5, m_zoom, m_zoom, & gray, true);
			}
			break;

		case 3:
			{
				KTraceEMF trace((HINSTANCE) GetWindowLong(WindowFromDC(hDC), GWL_HINSTANCE));

				trace.Show(false);

				HPALETTE hPal = CreateHalftonePalette(hDC);
				HPALETTE hOld = SelectPalette(hDC, hPal, TRUE);
				
				trace.m_pLog->Log("/////////////// Before Drawing //////////////\r\n");
				trace.CompareDC(hDC);

				trace.m_pLog->Log("/////////////// Starting Drawing //////////////\r\n");
				DisplayEMF(hDC, m_hEmf, 5, 5, m_zoom, m_zoom, & trace, true);

				trace.m_pLog->Log("/////////////// After Drawing //////////////\r\n");
				trace.CompareDC(hDC);
				
				SelectPalette(hDC, hOld, TRUE);
				DeleteObject(hPal);

				trace.Show(true);
			}
			break;

		default:
			DisplayEMF(hDC, m_hEmf, 5, 5, m_zoom, m_zoom, NULL, true);
	}
	m_mode = 0; // reset

	if ( hOld )
		SelectPalette(hDC, hOld, TRUE);
}


bool KEMFView::Initialize(HINSTANCE hInstance, KStatusWindow * pStatus, HWND hWndFrame, 
						  HENHMETAFILE hEmf, const TCHAR * pFileName)
{
	m_hFrame    = hWndFrame;
	m_hInst     = hInstance;
	m_pStatus   = pStatus;
	
	m_emfpanel.m_hEmf = hEmf;

	if ( pFileName )
		_tcscpy(m_FileName, pFileName);

	RegisterClass(_T("EMFViewClass"), hInstance);
		
	return true;
}


void Map10umToLogical(HDC hDC, RECT & rect)
{
	POINT * pPoint = (POINT *) & rect;

	// convert from 0.01 mm to pixels for current device
	for (int i=0; i<2; i++)
	{	
		int       t = GetDeviceCaps(hDC, HORZSIZE) * 100; 
		pPoint[i].x = ( pPoint[i].x * GetDeviceCaps(hDC, HORZRES) + t/2 ) / t;

		   	      t = GetDeviceCaps(hDC, VERTSIZE) * 100;
		pPoint[i].y = ( pPoint[i].y * GetDeviceCaps(hDC, VERTRES) + t/2 ) / t;
	}

	// convert to logical coordinate space
	DPtoLP(hDC, pPoint, 2);
}


HENHMETAFILE FilterEMF(HENHMETAFILE hEmf, KEnumEMF & filter)
{
	ENHMETAHEADER emh;

	GetEnhMetaFileHeader(hEmf, sizeof(emh), & emh);

	RECT rcFrame;
	memcpy(& rcFrame, & emh.rclFrame, sizeof(RECT));

	HDC hDC = QuerySaveEMFFile("Filtered EMF\0", & rcFrame, NULL);

	if ( hDC==NULL )
		return NULL;
	
	Map10umToLogical(hDC, rcFrame);

	filter.EnumEMF(hDC, hEmf, & rcFrame);
		
	return CloseEnhMetaFile(hDC);
}


LRESULT KEMFView::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd		= hWnd;
			m_hViewMenu = LoadMenu(m_hInst, MAKEINTRESOURCE(IDR_VIEWER));

			{
				RECT rect;

				GetClientRect(m_hWnd, & rect);
				 m_emftree.Create(hWnd, 101, 0, 0, m_nDivision, rect.bottom, m_hInst);

				m_emfpanel.CreateEx(WS_EX_CLIENTEDGE, "EMFPanel", NULL, WS_VISIBLE | WS_BORDER | WS_CHILD, 
						m_nDivision+3, 0, rect.right - m_nDivision+3, rect.bottom, hWnd, NULL, m_hInst);

				{
					KEmfDC emfdc;

					if ( m_emfpanel.m_hEmf )
						emfdc.DeCompile(NULL, m_emfpanel.m_hEmf, & m_emftree);
					else
						emfdc.DeCompile(NULL, m_FileName, & m_emftree, m_emfpanel.m_hEmf);
				}

				// palette message handling is in KWindow class, which needs m_hPalette to be set
				{
					HDC hDC = GetDC(hWnd);
					
					m_hPalette = GetEMFPalette(m_emfpanel.m_hEmf, hDC);

					m_emfpanel.m_hPalette = m_hPalette;
					ReleaseDC(hWnd, hDC);
				}
			}

			return 0;

		case WM_SIZE:
			MoveWindow( m_emftree.m_hWnd,  0, 0, m_nDivision, HIWORD(lParam), TRUE);	
			MoveWindow(m_emfpanel.m_hWnd, m_nDivision+3, 0, LOWORD(lParam)-m_nDivision+3, HIWORD(lParam), TRUE);	
			return 0;

		case WM_MOUSEMOVE: // handle split window adjustment
			if ( wParam & MK_LBUTTON )
			{
				if ( GetCapture() != hWnd )
					SetCapture(hWnd);
			
				RECT rect;
				GetClientRect(hWnd, & rect);

				m_nDivision = LOWORD(lParam);
				SendMessage(hWnd, WM_SIZE, 0, MAKELONG(rect.right, rect.bottom));
			}
			else
				if ( GetCapture() == hWnd )
					ReleaseCapture();
			return 0;

		case WM_COMMAND:
			switch ( LOWORD(wParam) )
			{
				case IDM_FILE_SAVETEXT:
					{
						KFileDialog fd;
						ofstream    out;

						if ( fd.GetSaveFileName(NULL, "txt", "Text Files") )
						{
							out.open(fd.m_TitleName);
							SimpleEnumerateEMF(m_emfpanel.m_hEmf, out);
							out.close();
						}
						return 0;
					}

				case IDM_FILE_DECOMPILE:
					{
						KFileDialog fd;

						if ( fd.GetSaveFileName(NULL, "cpp", "C++ Programs") )
						{
							KEmfDC dc;
							dc.DeCompile(fd.m_TitleName, m_emfpanel.m_hEmf, NULL);
						}
					}
					return 0;

				case IDM_FILE_FILTER:
					{
						KGrayEMF gray;

						HENHMETAFILE hEmf = FilterEMF(m_emfpanel.m_hEmf, gray);
						DeleteEnhMetaFile(hEmf);
					}
					break;

				case IDM_VIEW_DELAY:
					{
						m_emfpanel.m_mode = 1;
						m_emfpanel.SetZoom(0, true);
					}
					return 0;


				case IDM_VIEW_GRAY:
					{
						m_emfpanel.m_mode = 2;
						m_emfpanel.SetZoom(0, true);
					}
					return 0;

				case IDM_VIEW_TRACE:
					{
						m_emfpanel.m_mode = 3;
						m_emfpanel.SetZoom(0, true);
					}
					return 0;

				case IDM_VIEW_ZOOM50:
					m_emfpanel.SetZoom(50); return 0;

				case IDM_VIEW_ZOOM100:
					m_emfpanel.SetZoom(100); return 0;

				case IDM_VIEW_ZOOM200:
					m_emfpanel.SetZoom(200); return 0;
			}

		case WM_PALETTECHANGED:
			return OnPaletteChanged(hWnd, wParam);

		case WM_QUERYNEWPALETTE:
			return OnQueryNewPalette();
			
		default:
			return CommonMDIChildProc(hWnd, uMsg, wParam, lParam, m_hViewMenu, 4);
	}
}
