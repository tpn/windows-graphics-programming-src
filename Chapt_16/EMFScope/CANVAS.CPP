//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : canvas.cpp							                                 //
//  Description: Canvas window for EMFScope                                          //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <assert.h>

#include "Winpp.h"
#include "Progress.h"
#include "Stream.h"

#include "emfscope.h"
#include "canvas.h"
#include "spehon32\\spehon32.h"
#include "resource.h"
#include "toolbar.h"
#include "Spoolfil.h"
#include "Examemf.h"


#define maxx  MetaHeader.szlDevice.cx
#define maxy  MetaHeader.szlDevice.cy


KCanvasWindow::KCanvasWindow(HINSTANCE hInst, HWND hWnd, int x, int y, int w, int h)
{
	scale            = 1;
	hMetaFile        = NULL;
	hNTMetaFile      = NULL;

	page_margin      = 10;
	canvas_backcolor = RGB(0xFF, 0xFF, 0x80);
	hInstance        = hInst;

	Createwindow("CanvasClass",
                 IDI_COLIVE,
                 0,
                 "Canvas", 
		         WS_VISIBLE | WS_CHILD | WS_BORDER,
		         mapx(x), mapy(y), mapx(w), mapy(h), 
                 hWnd,
                 hInst,
                 NULL, 0, (HBRUSH) GetStockObject(WHITE_BRUSH) );
}

void KCanvasWindow::GetDisplayRect(RECT *r)
{
	int	xScroll = GetScrollPos  (m_hWnd, SB_HORZ);
	int yScroll = GetScrollPos  (m_hWnd, SB_VERT);

	r->left   =            - xScroll + page_margin;
	r->right  = maxx/scale - xScroll + page_margin;
	r->top    =            - yScroll + page_margin;
	r->bottom = maxy/scale - yScroll + page_margin;
}


int CALLBACK SlowPaintEMF(HDC hDC, HANDLETABLE *lpHTable,
						  ENHMETARECORD *lpEMFR, int nObj, LPVOID lpData)
{
	if (lpData)
	{
		KProgress *Progress = (KProgress *) lpData;

		if (Progress->Active())
			if (Progress->AbortDraw())
				Progress->Destroy();
			else
			{
				Progress->Delay();

				if (lpEMFR->iType==EMR_HEADER)
					Progress->SetRange(0, ((ENHMETAHEADER *)lpEMFR)->nRecords);
				else
					Progress->Move();
			}
	}
	
	return PlayEnhMetaFileRecord(hDC, lpHTable, lpEMFR, nObj);
}


LRESULT KCanvasWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
			m_hWnd = hWnd;
			break;

		case WM_PAINT:
			if (hMetaFile || hNTMetaFile)
			{
				PAINTSTRUCT ps;
				RECT        rect;
				HBRUSH      bbrush;

				BeginPaint(hWnd, &ps);
				
				GetDisplayRect(&rect);
				
				bbrush = CreateSolidBrush(canvas_backcolor);
				FillRect(ps.hdc, &rect, bbrush);
				DeleteObject(bbrush);

				if (hMetaFile)
					if (canvas_delay==0)
						PlayEnhMetaFile(ps.hdc, hMetaFile, &rect);
					else
					{
						KProgress Progress;

						Progress.Create(EmfScope.hinst_EmfScope, EmfScope.m_hWnd, IDD_PROGRESS, IDC_PROGRESS, IDC_NUMBER, canvas_delay);	

						EnumEnhMetaFile(ps.hdc, hMetaFile, 
										(ENHMFENUMPROC) SlowPaintEMF,
										& Progress, 
										&rect);

						Progress.Destroy();
						SetFocus(EmfScope.m_hWnd);
					}
				else if (hNTMetaFile)
					PlayNTEnhMetaFile(ps.hdc, hNTMetaFile, &rect);

				EndPaint(hWnd, &ps);
			}
			else
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;

		
		case WM_DESTROY:
			if (hMetaFile)
			{
				DeleteEnhMetaFile(hMetaFile);
				hMetaFile = NULL;
			}

			if (hNTMetaFile)
			{
				delete hNTMetaFile;
				hNTMetaFile = NULL;
			}
			break;

		
		case WM_HSCROLL:
		case WM_VSCROLL:
			Scroll (uMsg, HIWORD (wParam), LOWORD(wParam));
			break;

		case WM_SIZE:
			SetScaleColor(0, 0);
			break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}


void KCanvasWindow::SetDelay(int delay)
{
	if (canvas_delay != delay)
	{
		canvas_delay = delay;
		InvalidateRect(m_hWnd, NULL, TRUE);
	}
}


void KCanvasWindow::SetScaleColor(int newscale, COLORREF backcolor)
{
	RECT rect;

	if ((newscale==scale) && (canvas_backcolor==backcolor))
		return;

	if (backcolor)  // no black
		canvas_backcolor = backcolor;

	// newscale==0 will force repaint
	if (newscale) 
		scale = newscale;

	GetClientRect(m_hWnd, &rect);

	if ((maxx/scale+page_margin*2) < rect.right)
		ShowScrollBar(m_hWnd, SB_HORZ, FALSE);		   // no scroll bar needed	
	else
	{
		ShowScrollBar(m_hWnd, SB_HORZ, TRUE);

		EnableScrollBar(m_hWnd, SB_HORZ, ESB_ENABLE_BOTH);
		SetScrollRange(m_hWnd, SB_HORZ, 0, (maxx/scale+page_margin*2) - rect.right, FALSE);
	}
	// always reset scrool bar position, 
	// we need that inf WS_PAINT processing, even if we don't have show scroll bar
	SetScrollPos(m_hWnd, SB_HORZ, 0, FALSE);
	
	if ((maxy/scale+page_margin*2) < rect.bottom)
		ShowScrollBar(m_hWnd, SB_VERT, FALSE);
	else
	{	
		ShowScrollBar(m_hWnd, SB_VERT, TRUE);

		SetScrollRange(m_hWnd, SB_VERT, 0, (maxy/scale+page_margin*2) - rect.bottom, FALSE);
	}
	// always reset scrool bar position, 
	// we need that inf WS_PAINT processing, even if we don't have show scroll bar
	SetScrollPos(m_hWnd, SB_VERT, 0, FALSE);
	
	InvalidateRect(m_hWnd, NULL, TRUE);
}


BOOL KCanvasWindow::UnloadEmfFile(const char *filename)
{
	if ( strcmp(filename, CurEmfFileName) == 0 )
	{
		if (hMetaFile)
			DeleteEnhMetaFile(hMetaFile);
		hMetaFile = NULL;

		if (hNTMetaFile)
		{
			delete hNTMetaFile;
			hNTMetaFile = NULL;
		}

		CurEmfFileName[0] = 0;

		InvalidateRect(m_hWnd, NULL, TRUE);	// clear the window
		return TRUE;
	}
	else
		return FALSE;
}


BOOL KCanvasWindow::LoadEmfFile(const char *filename)
{
	HENHMETAFILE  hNew;
	int			  len;

	hNew = GetEnhMetaFile(filename);
	
	if (hNew!=NULL)							// standard EMF file
	{
		len = GetEnhMetaFileHeader(hNew, sizeof(MetaHeader), &MetaHeader);
		assert(len != 0);
	}
	else
	{
		KInputFile * ntemf = new KInputFile;

		if (ntemf==NULL)
			return FALSE;

		if (!ntemf->Open(filename))
			return FALSE;

		if (!CheckNTSpoolEmfFile(ntemf, & MetaHeader))
		{
			ntemf->Close();
			return FALSE;
		}

		if (hNTMetaFile)
			delete hNTMetaFile;

		hNTMetaFile = ntemf;
	}

	if (hMetaFile)
		DeleteEnhMetaFile(hMetaFile);
	hMetaFile = hNew;

	strcpy(CurEmfFileName, filename);
	
	// make sure canvas window in visible and big enough, by resizing it's parent
	EmfScope.WakeUp();
	SetScaleColor(0, 0);	// force redraw with current scale

	return TRUE;
}


const char szEmfFilter[] = "Enhanced Metafiles (*.emf)\0*.emf\0"
                           "Windows Metafiles (*.wmf)\0*.wmf\0"
						   "WinNT Enhanced Metafiles (*.spl)\0*.spl\0"
                           "All Files (*.*)\0*.*\0";


const char *KCanvasWindow::OpenFile(void)
{
    OPENFILENAME    ofn;
    static char     szFile[256];		// full filename with path, reply filename to caller
	char		    szFileTitle[256];	// filename without path

    strcpy(szFile, "");

	memset(&ofn, 0, sizeof(ofn));

    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = m_hWnd;
    ofn.lpstrFilter       = szEmfFilter;
    ofn.lpstrCustomFilter = (LPSTR) NULL;
    ofn.nMaxCustFilter    = 0L;
    ofn.nFilterIndex      = 1;
    ofn.lpstrFile         = szFile;
    ofn.nMaxFile          = sizeof(szFile);
    ofn.lpstrFileTitle    = szFileTitle;
    ofn.nMaxFileTitle     = sizeof(szFileTitle);
    ofn.lpstrInitialDir   = NULL;
    ofn.lpstrTitle        = "Load Metafile";
    ofn.Flags             = 0L;
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = "EMF";

    if (!GetOpenFileName(&ofn))
        return NULL;

	if (LoadEmfFile(szFile))
		return szFile;

	return NULL;
}


BOOL KCanvasWindow::Exam(void)
{
	HDC  dc;
	RECT rect;

	dc = GetDC(m_hWnd);

	rect.left   = 0;
	rect.right  = maxx-1;
	rect.top    = 0;
	rect.bottom = maxy-1;

//	EnumEnhMetaFile(dc, hMetaFile, (ENHMFENUMPROC) MyEnhMetaFileProc, this, &rect);
	ReleaseDC(m_hWnd, dc);
	return FALSE;
}


BOOL KCanvasWindow::Print(void)
{
	HDC             hDCPrinter;
    int             iEntries;
    
    if (hMetaFile == NULL) 
		return FALSE;

	// if (SpoolFile.PlaySpoolFile(CurEmfFileName, NULL, NULL, NULL))
	//	  return TRUE;

	{
		PRINTDLG        pd;
    
		memset(&pd, 0, sizeof(pd));
		pd.lStructSize = sizeof(PRINTDLG);
		pd.hwndOwner   = m_hWnd;
		pd.Flags       = PD_RETURNDC;
		pd.hInstance   = hInstance;

		if (!PrintDlg(&pd) || (pd.hDC==NULL)) 
			return FALSE;

		hDCPrinter = pd.hDC;
	}

	
	// EmfScope.UnlinkClientServer();

	
	{
		DOCINFO         DocInfo;
		char            title[128];

		// put a special tag before the title so spehon32 can ignore it
		strcpy(title, ItsMe);
		GetEnhMetaFileDescription(hMetaFile, sizeof(title)-strlen(title), title+strlen(title));

		memset(&DocInfo, 0, sizeof(DocInfo));
		DocInfo.cbSize      = sizeof(DOCINFO);
		DocInfo.lpszDocName = title;
		DocInfo.lpszOutput  = NULL;
		StartDoc(hDCPrinter, &DocInfo);
	}

    
	StartPage(hDCPrinter);

    
	iEntries = GetEnhMetaFilePaletteEntries(hMetaFile, 0, NULL);

    if (iEntries) 
	{
	    PLOGPALETTE     plogPal;
	    PBYTE           pjTmp;
		HPALETTE        hPal;

        if ((plogPal = (PLOGPALETTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                sizeof(DWORD) + sizeof(PALETTEENTRY)*iEntries )) == NULL) 
		{
            MessageBox(m_hWnd, "Failed in Creating Palette!", "Error", MB_OK);
        }

        plogPal->palVersion = 0x300;
        plogPal->palNumEntries = (WORD) iEntries;
        pjTmp = (PBYTE) plogPal;
        pjTmp += 8;

        GetEnhMetaFilePaletteEntries(hMetaFile, iEntries, (PPALETTEENTRY)pjTmp);
        hPal = CreatePalette(plogPal);
        GlobalFree(plogPal);

        SelectPalette(hDCPrinter, hPal, FALSE);
        RealizePalette(hDCPrinter);
    }


	{
        RECT rc;
		ENHMETAHEADER   EnhMetaHdr;
    
		GetEnhMetaFileHeader(hMetaFile, sizeof(ENHMETAHEADER), &EnhMetaHdr);

        rc.top    = 0;
		rc.left   = 0;
        rc.right  = EnhMetaHdr.szlDevice.cx;
        rc.bottom = EnhMetaHdr.szlDevice.cy;
        
		if (!PlayEnhMetaFile(hDCPrinter, hMetaFile, &rc)) 
		{
            char    text[128];

            wsprintf(text, "Fail in PlayEnhMetaFile! Error %ld\n", GetLastError());
            MessageBox(0, text, "Play", MB_OK);
		}
	}

    EndPage(hDCPrinter);
    EndDoc(hDCPrinter);

	// EmfScope.LinkClientServer();

	return TRUE;
}
