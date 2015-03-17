//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : choosefont.cpp					                                     //
//  Description: Customized font selection dialog box                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

// ChooseFont.cpp Customized Font Selection Dialog Box
#define STRICT
#define NOCRYPT

#pragma pack(push, 4)
#include <windows.h>
#pragma pack(pop)

#include <assert.h>
#include <tchar.h>
#include <math.h>
#include <commctrl.h>
#include <stdio.h>

#include "..\..\include\fonttext.h"
#include "..\..\include\treeview.h"
#include "..\..\include\OutlineMetric.h"

#include "ChooseFont.h"

// add a single string to a tree view

// { <tag> <format> }
// <tag>::= g |   // dword, dword, dword, dword
//          8 |   // dword, dword
//          4 |   // dword
//          2 |   // unsigned short
//          1 |   // unsigned char
//          &     // address of input data

const TCHAR Format_LOGFONT [] =
    "4lfHeight: %d\0"
	"4lfWidth:  %d\0"
	"4lfEscapement: %d\0"
	"4lfOrientation: %d\0"
	"4lfWeight: %d\0"
    "1lfItalic: %d\0"
	"1lfUnderline: %d\0"
	"1lfStrikeOut: %d\0"
	"1lfCharSet: %d\0"
	"1lfOutPrecision: %d\0"
	"1lfClipPrecision: %d\0"
	"1lfQuality: %d\0"
	"1lfPitchAndFamily: 0x%x\0"
	"&lfFaceName: %s\0";


const TCHAR Format_TEXTMETRIC [] =
	"4tmHeight: %d\0"
    "4tmAscent: %d\0"
    "4tmDescent: %d\0"
    "4tmInternalLeading: %d\0"
    "4tmExternalLeading: %d\0"
    "4tmAveCharWidth: %d\0"
    "4tmMaxCharWidth: %d\0"
    "4tmWeight: %d\0"
    "4tmOverhang: %d\0"
    "4tmDigitizedAspectX: %d\0"
    "4tmDigitizedAspectY: %d\0"
    "1tmFirstChar: %d\0"
    "1tmLastChar: %d\0"
    "1tmDefaultChar: %d\0"
    "1tmBreakChar: %d\0"
    "1tmItalic: %d\0"
    "1tmUnderlined: %d\0"
    "1tmStruckOut: %d\0"
    "1tmPitchAndFamily: 0x%x\0"
    "1tmCharSet: %d\0";

const TCHAR Format_PANOSE [] = 
	"1bFamilyType: %d\0"
	"1bSerifStyle: %d\0"
	"1bWeight: %d\0"
	"1bProportion: %d\0"
	"1bContrast: %d\0"
    "1bStrokeVariation: %d\0"
    "1bArmStyle: %d\0"
    "1bLetterform: %d\0"
    "1bMidline: %d\0"
    "1bXHeight: %d\0";

// partial
const TCHAR Format_OUTLINETEXTMETRIC1 [] =
    "4otmSize: %d\n";

const TCHAR Format_OUTLINETEXTMETRIC4 [] =
    "4otmfsSelection: 0x%x\0"
    "4otmfsType: %d\0"
	"4otmsCharSlopeRise: %d\0"
	"4otmsCharSlopeRun: %d\0"
	"4otmItalicAngle: %d\0"
	"4otmEMSquare: %d\0"
	"4otmAscent: %d\0"
	"4otmDescent: %d\0"
	"4otmLineGap: %d\0"
	"4otmsCapEmHeight: %d\0"
	"4otmsXHeight: %d\0"
	"gotmrcFontBox: {%d,%d,%d,%d}\0"
	"4otmMacAscent: %d\0"
	"4otmMacDescent: %d\0"
    "4otmMacLineGap: %d\0"
    "4otmusMinimumPPEM: %d\0"
    "8otmptSubscriptSize: {%d,%d}\0"
    "8otmptSubscriptOffset: {%d,%d}\0"
    "8otmptSuperscriptSize: (%d,%d}\0"
    "8otmptSuperscriptOffset: (%d,%d}\0"
    "4otmsStrikeoutSize: %d\0"
	"4otmsStrikeoutPosition: %d\0"
	"4otmsUnderscoreSize: %d\0"
	"4otmsUnderscorePosition: %d\0";
 
const TCHAR Format_FONTSIGNATURE [] =
	"4fsUsb[0]: 0x%08X\0"
	"4fsUsb[1]: 0x%08X\0"
	"4fsUsb[2]: 0x%08X\0"
	"4fsUsb[3]: 0x%08X\0"
	"4fsCsb[0]: 0x%08X\0"
	"4fsCsb[1]: 0x%08X\0";


HTREEITEM AddCharWidth(KTreeView & Tree, HDC hDC, const TCHAR * name, HTREEITEM hLast, int typ)
{
	hLast = Tree.InsertItem(hLast, TVI_ROOT, name);

	INT       intWidth[256];
	FLOAT     fltWidth[256];
	ABC       abcWidth[256];
	ABCFLOAT abcfWidth[256];

	UINT uFirst = ' ';
	UINT uLast  = 0x80;

	switch ( typ )
	{
		case 0:         GetCharWidth32(hDC, uFirst, uLast, intWidth);  break;
		case 1:      GetCharWidthFloat(hDC, uFirst, uLast, fltWidth);  break;
		case 2:       GetCharABCWidths(hDC, uFirst, uLast, abcWidth);  break;
		case 3:  GetCharABCWidthsFloat(hDC, uFirst, uLast, abcfWidth); break;
	}

	for (UINT ch=uFirst; ch<=uLast; ch++)
	{
		TCHAR mess[MAX_PATH];

		sprintf(mess, "%d '%c': ", ch, ch);

		TCHAR * num = mess + _tcslen(mess);

		switch ( typ )
		{
			case 0: sprintf(num, "%d",    intWidth[ch-uFirst]); break;
			case 1: sprintf(num, "%7.3f", fltWidth[ch-uFirst]); break;
			case 2: sprintf(num, "%d %d %d", abcWidth[ch-uFirst].abcA, abcWidth[ch-uFirst].abcB, abcWidth[ch-uFirst].abcC); break;
			case 3: sprintf(num, "%7.3f %7.3f %7.3f", abcfWidth[ch-uFirst].abcfA, abcfWidth[ch-uFirst].abcfB, abcfWidth[ch-uFirst].abcfC); break;
		}

		Tree.InsertItem(TVI_LAST, hLast, mess);
	}

	return hLast;
}


const term Dic_GCP [] =
{
	{	GCP_DBCS,		"GCP_DBCS"			},
	{	GCP_REORDER,	"GCP_REORDER"		},
	{	GCP_USEKERNING, "GCP_USEKERNING"	},
	{	GCP_GLYPHSHAPE, "GCP_GLYPHSHARE"	},
	{	GCP_LIGATE,		"GCP_LIGATE"		},
	{	GCP_DIACRITIC,	"GCP_DIACRITIC"		},
	{	GCP_KASHIDA,	"GCP_KASHIDA"		},
	{	GCP_ERROR,		"GCP_ERROR"			},
	{	GCP_JUSTIFY,	"GCP_JUSTIFY"		},
	{	FLI_GLYPHS,		"FLI_GLYPHS"		},
	{	0,				NULL				}
};


// Base on LOGFONT, create font, select font, decode font information in a TreeView
void DecodeFont(KTreeView & Tree, LOGFONT * pLf, int dpi)
{
	TCHAR t1[MAX_PATH], t2[MAX_PATH];

	HDC hDC     = GetDC(NULL);

	HFONT hFont = CreateFontIndirect(pLf);

	HTREEITEM hLast, h1;

	// LOGFONT
	hLast = Tree.InsertTree(TVI_LAST, TVI_ROOT, "LOGFONT", Format_LOGFONT, pLf);

	SelectObject(hDC, hFont);

	GetTextFace(hDC, MAX_PATH, t1);
	wsprintf(t2, "GetTextFace: %s", t1);

	// GetTextFace
    hLast = Tree.InsertItem(hLast,    TVI_ROOT, t2);

	// GetFontLangageInfo
	DWORD langinfo = GetFontLanguageInfo(hDC);

	wsprintf(t1, "GetFontLanguageInfo: 0x%x", langinfo);
	hLast = Tree.InsertItem(hLast, TVI_ROOT, t1);
	Tree.AddFlags(hLast, langinfo, Dic_GCP);

	// GetTextCharset
	wsprintf(t1, "GetTextCharset: 0x%x", GetTextCharset(hDC));
	hLast = Tree.InsertItem(hLast, TVI_ROOT, t1);

	{
		FONTSIGNATURE fs;
		GetTextCharsetInfo(hDC, & fs, 0);

		hLast = Tree.InsertTree(hLast, TVI_ROOT, "FONTSIGNATURE", Format_FONTSIGNATURE, & fs);
	}
	
	// TEXTMETRIC
	{
		TEXTMETRIC tm;
		GetTextMetrics(hDC, & tm);
		hLast = Tree.InsertTree(hLast, TVI_ROOT, "TEXTMETRIC", Format_TEXTMETRIC, & tm);
	}

	// OUTLINETEXTMETIRC
	{
		KOutlineTextMetric otm(hDC);

		if ( otm.m_pOtm )
		{
			// first field of OUTLINTTEXTMETRIC
			hLast = Tree.InsertTree(hLast, TVI_ROOT, "OUTLINETEXTMETRIC", Format_OUTLINETEXTMETRIC1, otm.m_pOtm);
	
			// otmTextMetrics
			h1    =	Tree.InsertTree(TVI_LAST, hLast, "otmTextMetrics: TEXTMETRIC", Format_TEXTMETRIC, & otm.m_pOtm->otmTextMetrics);

			// otmPanseNumber
			h1    =	Tree.InsertTree(h1,       hLast, "otmPanoseNumber: PANOSE",    Format_PANOSE,     & otm.m_pOtm->otmPanoseNumber);

			// main fields after PANOSE
			h1    = Tree.InsertTree(h1,       hLast, NULL,                         Format_OUTLINETEXTMETRIC4, & otm.m_pOtm->otmfsSelection);
	
			wsprintf(t1, "otmpFamilyName: %s", otm.GetName(otm.m_pOtm->otmpFamilyName));
			h1 = Tree.InsertItem(h1, hLast, t1);

			wsprintf(t1, "otmpFaceName: %s", otm.GetName(otm.m_pOtm->otmpFaceName));
			h1 = Tree.InsertItem(h1, hLast, t1);

			wsprintf(t1, "otmpStyleName: %s", otm.GetName(otm.m_pOtm->otmpStyleName));
			h1 = Tree.InsertItem(h1, hLast, t1);

			wsprintf(t1, "otmpFullName: %s", otm.GetName(otm.m_pOtm->otmpFullName));
			h1 = Tree.InsertItem(h1, hLast, t1);
		}
	}

	hLast = AddCharWidth(Tree, hDC, "GetCharWidth32",        hLast, 0);
	hLast = AddCharWidth(Tree, hDC, "GetCharWidthFloat",     hLast, 1);
	hLast = AddCharWidth(Tree, hDC, "GetCharABCWidths",      hLast, 2);
	hLast = AddCharWidth(Tree, hDC, "GetCharABCWidthsFloat", hLast, 3);

	// Kerning pairs
	{
		KKerningPair kerning(hDC);

		wsprintf(t1, "GetKerningPairs: %d pairs", kerning.m_nPairs);

		hLast = Tree.InsertItem(hLast, TVI_ROOT, t1);

		for (int i=0; i<kerning.m_nPairs; i++)
		{
			wsprintf(t1, "'%c' (%02x) '%c' (%02x) -> %d", 
				kerning.m_pKerningPairs[i].wFirst,  kerning.m_pKerningPairs[i].wFirst, 
				kerning.m_pKerningPairs[i].wSecond, kerning.m_pKerningPairs[i].wSecond, 
				kerning.m_pKerningPairs[i].iKernAmount);

			Tree.InsertItem(TVI_LAST, hLast, t1);
		}
	}

	// Cleanup
	SelectObject(hDC, GetStockObject(ANSI_FIXED_FONT));
	DeleteObject(hDC);
	DeleteObject(hFont);
}


void GenerateTree(HWND hDlg, KTreeView & Tree, int dpi)
{
	LOGFONT lf;

	SendMessage(hDlg, WM_CHOOSEFONT_GETLOGFONT, 0, (LPARAM) & lf);

	Tree.DeleteAllItems();
	DecodeFont(Tree, & lf, dpi);
}

/*
// Resolution setup dialog box: not used
class KSetup : public KDialog
{
	virtual BOOL OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		m_hWnd = hWnd;
	
		SendDlgItemMessage(hWnd, IDC_SCREEN, BM_SETCHECK, BST_CHECKED, 0);
		return TRUE;
	}
	
	virtual BOOL OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		switch ( LOWORD(wParam) )
		{
			case IDOK:
				{
					int rslt = 1;

					if ( SendDlgItemMessage(hWnd, IDC_120, BM_GETCHECK, 0, 0)==BST_CHECKED )
						rslt = 120;
					else if ( SendDlgItemMessage(hWnd, IDC_180, BM_GETCHECK, 0, 0)==BST_CHECKED )
						rslt = 180;
					else if ( SendDlgItemMessage(hWnd, IDC_300, BM_GETCHECK, 0, 0)==BST_CHECKED )
						rslt = 300;
					else if ( SendDlgItemMessage(hWnd, IDC_1800, BM_GETCHECK, 0, 0)==BST_CHECKED )
						rslt = 1800;

					EndDialog(hWnd, rslt);
					return TRUE;
				}
			
			case IDCANCEL:
				EndDialog(hWnd, 0);
				return TRUE;
		}

		return FALSE;
	}

};
*/

// Customized Font Selection Dialog Box
class KChooseFont
{
	typedef enum { MAX_DPINO= 5 };

	KTreeView m_Tree;
	HWND	  m_hButton[MAX_DPINO];
	int		  m_nDPI   [MAX_DPINO];

	// member hook function
	UINT HookProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
	{
		switch ( uiMsg )
		{
			case WM_INITDIALOG:
				{
					RECT rect;
					GetWindowRect(hDlg, & rect);
					// enlarge the window by 260 pixels on the right
					MoveWindow(hDlg, rect.left, rect.top, rect.right - rect.left + 260, 
						rect.bottom - rect.top, TRUE);

					// get new client area dimension
					GetClientRect(hDlg, & rect);

					// add a treeview child window on the right
					m_Tree.Create(hDlg, 1, rect.right - 270, 2, 266, rect.bottom - 4,
								(HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE));

					GetWindowRect(GetDlgItem(hDlg, 0x402), & rect);
					MapWindowPoints(NULL, hDlg, (POINT *) & rect, 2);
					
					HDC hDC = GetDC(NULL);
					m_nDPI[0] = GetDeviceCaps(hDC, LOGPIXELSX);
					DeleteObject(hDC);

					m_nDPI[1] =  120;
					m_nDPI[2] =  180;
					m_nDPI[3] =  300;
					m_nDPI[4] = 1800;

					ReleaseDC(NULL, hDC);

/*					for (int i=0; i<MAX_DPINO; i++)
					{
						rect.top    += 30;
						rect.bottom += 30;
					
						TCHAR temp[32];
						wsprintf(temp, "%d dpi", m_nDPI[i]);
						
						m_hButton[i] = CreateWindow("BUTTON", temp,
							WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
							rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
							hDlg, (HMENU) (1234+i), (HINSTANCE) GetWindowLong(hDlg, GWL_HINSTANCE), NULL);

						SendMessage(m_hButton[i], WM_SETFONT, SendDlgItemMessage(hDlg, 0x402, WM_GETFONT, 0, 0), TRUE);
					}
*/
				}
				return 1;

			case WM_COMMAND:
				if ( LOWORD(wParam)==0x402 ) // APPLY button
				{
					int dpi = m_nDPI[0];

/*					for (int i=0; i<MAX_DPINO; i++)
						if ( SendMessage(m_hButton[i], BM_GETCHECK, 0, 0)==BST_CHECKED )
						{
							dpi = m_nDPI[i];
							break;
						}
*/
					GenerateTree(hDlg, m_Tree, dpi);
					return 1;
				}
		}

		return 0;
	}

	// static hook function
	static UINT CALLBACK CFHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
	{
		if ( uiMsg == WM_INITDIALOG )
		{
			CHOOSEFONT * p = (CHOOSEFONT *) lParam;

			if ( IsBadWritePtr(p, sizeof(CHOOSEFONT)) )
				return 0;
		
			SetWindowLong(hdlg, GWL_USERDATA, p->lCustData);
		}

		KChooseFont * p = (KChooseFont *) GetWindowLong(hdlg, GWL_USERDATA);

		if ( IsBadWritePtr(p, sizeof(KChooseFont)) )
			return 0;
		else
			return p->HookProc(hdlg, uiMsg, wParam, lParam);
	}

public:
    CHOOSEFONT m_cf; 
    LOGFONT	   m_lf; 

	HFONT ChooseFont(HINSTANCE hInstance)
	{
	    // Initialize members of the CHOOSEFONT structure. 
		memset(& m_cf, 0, sizeof(m_cf));

		m_cf.lStructSize = sizeof(CHOOSEFONT); 
		m_cf.lpLogFont   = & m_lf; 
		m_cf.Flags	     = CF_APPLY | CF_BOTH | CF_EFFECTS | CF_ENABLEHOOK; 
		m_cf.lpfnHook    = CFHookProc;
		m_cf.rgbColors   = RGB(0,0,0); 
		m_cf.nFontType   = SCREEN_FONTTYPE; 
		m_cf.lCustData   = (LPARAM) this; // pass to hook function

//		KSetup setup;

//		int rslt = setup.Dialogbox(hInstance, MAKEINTRESOURCE(IDD_SETUP));

		if ( ::ChooseFont(&m_cf) ) 
			return CreateFontIndirect(& m_lf);
		else
			return NULL;
	}
};    


void FontSelection(HINSTANCE hInst)
{
	KChooseFont cf;

	HFONT hFont = cf.ChooseFont(hInst);

	DeleteObject(hFont);
}

