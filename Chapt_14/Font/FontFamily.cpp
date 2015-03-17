//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : fontfamily.cpp					                                     //
//  Description: Font family enumeration                                             //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define NOCRYPT

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <math.h>

#include "..\..\include\win.h"
#include "..\..\include\Canvas.h"
#include "..\..\include\ListView.h"
#include "..\..\include\LogWindow.h"

#include "Resource.h"
#include "TrueType.h"
#include "FontFamily.h"

typedef struct
{
	unsigned	mask;
	unsigned	flag;
	const char *name;
}	WordDef;


const WordDef NTM_Flags[]	=
{
	{	0xFFFFFFFF, NTM_ITALIC,			"Italic"				},
	{	0xFFFFFFFF, NTM_BOLD,			"Bold"					},
	{	0xFFFFFFFF, NTM_REGULAR,		"Regular"				},
	{	0xFFFFFFFF, NTM_NONNEGATIVE_AC,	"Nonnegative AC"		},
	{	0xFFFFFFFF, NTM_PS_OPENTYPE,	"Postscript OpenType"	},
	{	0xFFFFFFFF, NTM_TT_OPENTYPE,	"TrueType OpenType"		},
	{	0xFFFFFFFF, NTM_MULTIPLEMASTER,	"Multiple Master"		},
	{	0xFFFFFFFF, NTM_TYPE1,			"Type 1 Font"			},
	{	0xFFFFFFFF, NTM_DSIG,			"Digital Signature"		},
	{	0xFFFFFFFF, 0,					NULL					}
};


const WordDef NTM_Family[] = 
{
	{	0x0F,	DEFAULT_PITCH,		"Default Pitch"		},
	{	0x0F,	FIXED_PITCH,		"Fixed Pitch"		},
	{	0x0F,	VARIABLE_PITCH,		"Variable Pitch"	},
	{	0x0F,	MONO_FONT,			"Mono Font"			},
	
	{	0xFF0,	FF_DONTCARE,		"Dont Care"			},	
	{	0xFF0,	FF_DECORATIVE,		"Decorative"		},
	{	0xFF0,	FF_MODERN,			"Modern"			},
	{	0xFF0,	FF_ROMAN,			"Roman"				},
	{	0xFF0,	FF_SCRIPT,			"Script"			},
	{	0xFF0,	FF_SWISS,			"Swiss"				},
	{	0xFF0,	0,					NULL				}
};


void DecodeFlag(unsigned flag, const WordDef * dic, TCHAR * result)
{
	result[0] = 0;

	for (; dic->name; dic ++)
		if ( (flag & dic->mask)==dic->flag )
		{
			if ( result[0] )
				_tcscat(result, _T(", "));
			_tcscat(result, dic->name);
		}
}


void ListFonts(KListView * pList)
{
	const TCHAR Key_Fonts[] = _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");

	HKEY hKey;

	if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, Key_Fonts, 0, KEY_READ, & hKey)==ERROR_SUCCESS )
	{
		for (int i=0; ; i++)
		{
			TCHAR szValueName[MAX_PATH];
			BYTE  szValueData[MAX_PATH];

			DWORD nValueNameLen = MAX_PATH;
			DWORD nValueDataLen = MAX_PATH;
			DWORD dwType;

			if ( RegEnumValue(hKey, i, szValueName, & nValueNameLen, NULL,
					& dwType, szValueData, & nValueDataLen) != ERROR_SUCCESS )
				break;

			pList->AddItem(0, szValueName);
		    pList->AddItem(1, (const char *) szValueData);
		}
		RegCloseKey(hKey);
	}
}


int KEnumFontFamily::EnumProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType)
{
	if ( (FontType & m_nType)==0 )
		return TRUE;

	if ( m_nLogFont < MAX_LOGFONT )
		m_LogFont[m_nLogFont ++] = lpelfe->elfLogFont;

	m_pList->AddItem(0, (const char *) lpelfe->elfFullName);
    m_pList->AddItem(1, (const char *) lpelfe->elfScript);
	m_pList->AddItem(2, (const char *) lpelfe->elfStyle);
	m_pList->AddItem(3, (const char *) lpelfe->elfLogFont.lfFaceName);

	m_pList->AddItem(4, lpelfe->elfLogFont.lfHeight);
	m_pList->AddItem(5, lpelfe->elfLogFont.lfWidth);
	m_pList->AddItem(6, lpelfe->elfLogFont.lfWeight);

	TCHAR Result[MAX_PATH];

	DecodeFlag(lpntme->ntmTm.ntmFlags, NTM_Flags, Result);
	m_pList->AddItem(7, Result);

	DecodeFlag(lpelfe->elfLogFont.lfPitchAndFamily, NTM_Family, Result);
	m_pList->AddItem(8, Result);

	return TRUE;
}


void KEnumFontFamily::EnumFontFamilies(HDC hdc, KListView * pList, BYTE charset, 
									   TCHAR * FaceName, unsigned type)
{
	m_pList	   = pList;
	m_nLogFont = 0;
	m_nType    = type;

	LOGFONT lf;
	memset(& lf, 0, sizeof(lf));
	lf.lfCharSet		= charset;
	lf.lfFaceName[0]	= 0;

	if ( FaceName )
		_tcscpy(lf.lfFaceName, FaceName);

	lf.lfPitchAndFamily = 0;

    EnumFontFamiliesEx(hdc, & lf, (FONTENUMPROC) EnumFontFamExProc, (LPARAM) this, 0); 
}


void KListViewCanvas::DecodeFontFile(const TCHAR * fontfile)
{
	TCHAR fullname[MAX_PATH];

	if ( _tcschr(fontfile, ':') )
		_tcscpy(fullname, fontfile);
	else
	{
		GetWindowsDirectory(fullname, MAX_PATH);
		_tcscat(fullname, "\\Fonts\\");
		_tcscat(fullname, fontfile);
	}

	// ask frame window to create a new MDI child window to decode a font
	SendMessage(m_hFrame, WM_USER+1, 0, (LPARAM) fullname);
}


void UnicodeRange(LOGFONT * pLogFont, HINSTANCE hInstance)
{
	HFONT hFont  = CreateFontIndirect(pLogFont);
	HDC   hDC	 = GetDC(NULL);
	HGDIOBJ hOld = SelectObject(hDC, hFont);

	// query for size
	DWORD size = GetFontUnicodeRanges(hDC, NULL);

	GLYPHSET * pGlyphSet = (GLYPHSET *) new BYTE[size];

	// get real data
	pGlyphSet->cbThis = size;
	size = GetFontUnicodeRanges(hDC, pGlyphSet);

	KLogWindow * pLog = new KLogWindow;

	assert(pLog);

	pLog->Create(hInstance, "UNICODE Range");

	pLog->Log("%s \r\n", pLogFont->lfFaceName);

	pLog->Log("cbSize   %d\r\n",			pGlyphSet->cbThis);
	pLog->Log("flAccel  %d\r\n",			pGlyphSet->flAccel);
	pLog->Log("cGlyphsSupported %d\r\n",  pGlyphSet->cGlyphsSupported);
	pLog->Log("cRanges          %d\r\n",  pGlyphSet->cRanges);

	for (unsigned i=0; i<pGlyphSet->cRanges; i++)
		pLog->Log("%3d %04x..%04x (%d)\r\n", i, 
			pGlyphSet->ranges[i].wcLow, 
			pGlyphSet->ranges[i].wcLow + pGlyphSet->ranges[i].cGlyphs -1,
			pGlyphSet->ranges[i].cGlyphs);

	WORD gi[10];
	size = GetGlyphIndices(hDC, "A Quick Brown Fox", 10, gi, GGI_MARK_NONEXISTING_GLYPHS);

	delete [] (BYTE *) pGlyphSet;

	SelectObject(hDC, hOld);
	ReleaseDC(NULL, hDC);
	DeleteObject(hFont);
}


LRESULT KListViewCanvas::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd		= hWnd;
			m_hViewMenu = LoadMenu(m_hInst, MAKEINTRESOURCE(IDR_DIBVIEW));

			{
				RECT rect;

				GetClientRect(m_hWnd, & rect);
				m_Fonts.Create(hWnd, 101, 0, 0, rect.right, rect.bottom, m_hInst);
			}

//			m_Fonts.AddIcon(LVSIL_SMALL, m_hInst, IDI_EMPTY);
//          m_Fonts.AddIcon(LVSIL_SMALL, m_hInst, IDI_EQUAL);
//          m_Fonts.AddIcon(LVSIL_SMALL, m_hInst, IDI_CHANGE);

			if ( m_bFamily )
			{
				m_Fonts.AddColumn(0, 100, "Full Name");
				m_Fonts.AddColumn(1, 100, "Script");
				m_Fonts.AddColumn(2, 100, "Style");
	
			    m_Fonts.AddColumn(3, 80,  "Face Name");
		        m_Fonts.AddColumn(4, 60,  "Height");
				m_Fonts.AddColumn(5, 60,  "Width");
				m_Fonts.AddColumn(6, 60,  "Weight");
				m_Fonts.AddColumn(7, 130, "Flags");
				m_Fonts.AddColumn(8, 130, "Family");

				{
					HDC hdc = GetDC(NULL);
					enumfont.EnumFontFamilies(hdc, & m_Fonts, DEFAULT_CHARSET, NULL);
					ReleaseDC(NULL, hdc);
				}
			}
			else 
			{
				m_Fonts.AddColumn(0, 100, "Name");
				m_Fonts.AddColumn(1, 100, "File");

				ListFonts(& m_Fonts);
			}

			return 0;

		case WM_SIZE:
			MoveWindow(m_Fonts.GetHWND(), 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);	
			return 0;

		case WM_NOTIFY:
			if (wParam == 101)
			{
				NM_LISTVIEW * pInfo = (NM_LISTVIEW *) lParam;
				
				if ( (pInfo->hdr.code == NM_RCLICK) && (pInfo->iItem != -1) ) 
				{					
					POINT pt = pInfo->ptAction;
					
					ClientToScreen(m_hWnd, & pt);
					
					HMENU hMenu = LoadMenu(m_hInst, MAKEINTRESOURCE(IDR_POPUP));

					int id = TrackPopupMenu(GetSubMenu(hMenu, 0), TPM_RIGHTBUTTON | TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD | TPM_NONOTIFY,
								 pt.x, pt.y, 0, m_hWnd, NULL);

					DestroyMenu(hMenu);

					if ( id==IDM_POP_DECODE )
					{
						if ( m_bFamily )
						{
							// ask frame window to create a new MDI child window to decode a font
							SendMessage(m_hFrame, WM_USER+2, 0, (LPARAM) & enumfont.m_LogFont[pInfo->iItem]);
						}
						else
						{
							TCHAR fontname[MAX_PATH];

							m_Fonts.GetItemText(pInfo->iItem, 1, fontname, MAX_PATH);

							DecodeFontFile(fontname);
						}

						return TRUE;
					}

					if ( id==IDM_POP_UNICODERANGE )
					{
						if ( m_bFamily )
							UnicodeRange(& enumfont.m_LogFont[pInfo->iItem], m_hInst);
					
						return TRUE;
					}
				}
			}

		default:
			return CommonMDIChildProc(hWnd, uMsg, wParam, lParam, m_hViewMenu, 3);
	}
}
