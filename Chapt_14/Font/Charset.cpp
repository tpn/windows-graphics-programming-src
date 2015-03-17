//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : charset.cpp					                                     //
//  Description: KCharSetView class                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <assert.h>

#include "..\..\include\Win.h"
#include "..\..\include\Canvas.h"
#include "..\..\include\ScrollCanvas.h"
#include "..\..\include\MVC.h"
#include "..\..\include\logfont.h"

#include "CharSet.h"
#include "Resource.h"

class KDBCSMap
{
	WCHAR	m_map[4096];
	int		m_size;

public:
	KDBCSMap(void)
	{
		m_size = 0;
	}

	void Add(WCHAR from, WCHAR to);

	int GetSize(void) const
	{
		return m_size;
	}

	WCHAR GetRowFirst(int row)
	{
		return m_map[row];
	}
};

void KDBCSMap::Add(WCHAR from, WCHAR to)
{
	for (unsigned f=from; f<=to; f+=16)
	{
		if ( m_size < 4096 )
			m_map[m_size++] = (WCHAR) f;
		else
			assert(FALSE);
	}
}

KDBCSMap Map_932;
KDBCSMap Map_936;
KDBCSMap Map_949;
KDBCSMap Map_950;
KDBCSMap Map_UC;

void InitMaps(void)
{
	int high;

	// Japanese
	for (high = 0x8100; high <= 0xFC00; high += 256)
	{
		if ( (high>0x8400) && (high<0x8700) ) 
			continue;

		if ( (high>0x9F00) && (high<0xE000) )
			continue;

		if ( (high>0xEA00) && (high<0xED00) )
			continue;

		if ( (high>0xEE00) && (high<0xFA00) )
			continue;

		int minlow = 0x40;
		int maxlow = 0xF0;

		switch ( high )
		{
			case 0x8300: maxlow = 0xD0; break;
			case 0x8400: maxlow = 0xB0; break;
			case 0x8700: maxlow = 0x90; break;
			case 0x8800: minlow = 0x90; break;
			
			case 0xEA00: maxlow = 0xA0; break;	
			case 0xFC00: maxlow = 0x40; break;
		}
		Map_932.Add(high + minlow, high + maxlow);
	}

	// Simplified Chinese
	for (high = 0xA100; high <= 0xF700; high += 256)
	{
		if ( (high>0xA900) && (high<0xB000) )
			continue;

		int minlow = 0xA0;
		int maxlow = 0xF0;

		switch ( high )
		{
			case 0xA200: minlow = 0xB0; break;
			case 0xA600: maxlow = 0xD0; break;
			case 0xA800: 
			case 0xA900: maxlow = 0xE0; break;
		}
		Map_936.Add(high + minlow, high + maxlow);

	}

	// Traditional Chinese
	for (high = 0xA100; high <= 0xF900; high += 256)
	{
		if ( (high>0xC600) && (high<0xC900) )
			continue;
		
		Map_950.Add(high + 0x40, high + 0x70);

		int maxlow = 0xF0;

		switch ( high )
		{
			case 0xA300: maxlow = 0xB0; break;
			case 0xC600: continue;
		}

		Map_950.Add(high + 0xA0, high + maxlow);
	}

	// Korean
	for (high = 0x8100; high <= 0xFD00; high+= 256)
	{
		if ( high==0xC900 )
			continue;

		int minlow = 0x40;
		int maxlow = 0xF0;

		if ( high > 0xC700 )
			minlow = 0xA0;
		
		switch ( high )
		{
			case 0xA200: 
			case 0xA600: 
			case 0xA700: 
				maxlow = 0xE0; 
				break;
			
			case 0xAD00: 
			case 0xAE00: 
			case 0xAF00: 
				maxlow = 0xA0; 
				break;

			case 0xC600:
				Map_949.Add(high + 0x40, high + 0x50);
				minlow = 0xA0;
		}

		Map_949.Add(high + minlow, high + maxlow);
	}

	Map_UC.Add(0x0100, 0x04F0);
	Map_UC.Add(0x0530, 0x06F0);
	Map_UC.Add(0x0900, 0x0D70);
	Map_UC.Add(0x0E00, 0x0FB0);
	Map_UC.Add(0x10A0, 0x11F0);
	Map_UC.Add(0x1E00, 0x27B0);
	Map_UC.Add(0x3000, 0x3190);
	Map_UC.Add(0x3200, 0x33F0);
	Map_UC.Add(0x4E00, 0x9FF0);
	Map_UC.Add(0xAC00, 0xD7A0);
	Map_UC.Add(0xD800, 0xFDF0);
	Map_UC.Add(0xFE20, 0xFFF0);
/*
Unassigned Blocks
0500..052F    (48) {General Scripts Area}
0700..08FF   (512) {General Scripts Area}
0D80..0DFF   (128) {General Scripts Area}
0FC0..109F   (224) {General Scripts Area}
1200..1DFF  (3072) {General Scripts Area}
27C0..2FFF  (2112) {Symbols Area}
31A0..31FF    (96) {CJK Phonetics and Symbols Area}
3400..4DFF  (6656) {NO AREA}
A000..ABFF  (3072) {NO AREA}
D7B0..D7FF    (80) {NO AREA}
FE00..FE1F    (32) {Compatibility Area and Specials}
 


Assigned Blocks 
0220..024F    (48) [Latin Extended-B]
02F0..02FF    (16) [Spacing Modifier Letters]
0350..035F    (16) [Combining Diacritical Marks]
0AF0..0AFF    (16) [Gujarati]
0C70..0C7F    (16) [Telugu]
0CF0..0CFF    (16) [Kannada]
0D70..0D7F    (16) [Malayalam]
0E60..0E7F    (32) [Thai]
0EE0..0EFF    (32) [Lao]
2050..205F    (16) [General Punctuation]
2090..209F    (16) [Superscripts and Subscripts]
20B0..20CF    (32) [Currency Symbols]
20F0..20FF    (16) [Combining Marks for Symbols]
2140..214F    (16) [Letterlike Symbols]
21F0..21FF    (16) [Arrows]
2380..23FF   (128) [Miscellaneous Technical]
2430..243F    (16) [Control Pictures]
2450..245F    (16) [Optical Character Recognition]
24F0..24FF    (16) [Enclosed Alphanumerics]
25F0..25FF    (16) [Geometric Shapes]
2670..26FF   (144) [Miscellaneous Symbols]
3250..325F    (16) [Enclosed CJK Letters and Months]
9FB0..9FFF    (80) [CJK Unified Ideographs]
FA30..FAFF   (208) [CJK Compatibility Ideographs]
FBC0..FBCF    (16) [Arabic Presentation Forms-A]
FD40..FD4F    (16) [Arabic Presentation Forms-A]
FDD0..FDEF    (32) [Arabic Presentation Forms-A]
 */
}
 

BOOL CALLBACK EnumCodePagesProc(LPTSTR lpCodePageString)
{
	return TRUE;
}



KCharSetView::KCharSetView(void)
{
	InitMaps();

	m_width		   = 40;
	m_height	   = 36;
	m_font		   = 32;
	m_row		   = 16;

	m_nCharSet     = DEFAULT_CHARSET;
	m_hAnsiFont	   = NULL;

	m_nPixelWidth  = 16 * m_width + 100;
	m_nPixelHeight = m_row * m_height + 100;

	EnumSystemCodePages(EnumCodePagesProc, CP_SUPPORTED);

	KLogFont ansifont(26, "Times New Roman");
	ansifont.SetCharSet(DEFAULT_CHARSET);
				
	m_hAnsiFont = ansifont.Create();
}


int KCharSetView::OnCommand(int cmd, HWND hWnd)
{
	BYTE cs = m_nCharSet;
	int	 row = 16;
	int	 width = m_width;

	switch ( cmd )
	{
		case IDM_CP_ANSI:			cs = ANSI_CHARSET;			break;
		case IDM_CP_ARABIC:			cs = ARABIC_CHARSET;		break;
		case IDM_CP_BALTIC:			cs = BALTIC_CHARSET;		break;
		case IDM_CP_EASTEUROPE:		cs = EASTEUROPE_CHARSET;	break;
		case IDM_CP_OEM:			cs = OEM_CHARSET;			break;
		case IDM_CP_MAC:			cs = MAC_CHARSET;			break;
		case IDM_CP_RUSSIAN:		cs = RUSSIAN_CHARSET;		break;
		case IDM_CP_SYMBOL:			cs = SYMBOL_CHARSET;		break;
		case IDM_CP_TURKISH:		cs = TURKISH_CHARSET;		break;
		case IDM_CP_GREEK:			cs = GREEK_CHARSET;			break;
		case IDM_CP_HEBREW:			cs = HEBREW_CHARSET;		break;
		case IDM_CP_THAI:			cs = THAI_CHARSET;			break;
		case IDM_CP_VIETNAMESE:		cs = VIETNAMESE_CHARSET;	break;

		case IDM_CP_CHINESEBIG5:	cs = CHINESEBIG5_CHARSET;	row += Map_950.GetSize(); break; // (0xF9-0xA1+1)*10; break;
		case IDM_CP_GB2312:			cs = GB2312_CHARSET;		row += Map_936.GetSize(); break; // (0xF7-0xA1+1)* 6; break;
		case IDM_CP_SHIFTJIS:		cs = SHIFTJIS_CHARSET;		row += Map_932.GetSize(); break; // (0xFC-0x81+1)*12; break;
		case IDM_CP_HANGUL:			cs = HANGUL_CHARSET;		row += Map_949.GetSize(); break; // (0xFD-0x81+1)*12; break;
					
		case IDM_CP_UNICODE:		cs = UNICODE_CHARSET;       row += Map_UC.GetSize(); break; // 256 * 16; break;

		case IDM_VIEW_SMALL:		width = 20; break;
		case IDM_VIEW_MEDIUM:		width = 40; break;
		case IDM_VIEW_LARGE:		width = 60; break;
	}

	int rslt = View_NoChange;
			
	if ( row != m_row )
	{
		m_row          = row;
		m_nPixelWidth  = 16 * m_width + 100;
		m_nPixelHeight = row * m_height + 100;
		rslt           = View_Resize;
	}

	if ( cs!= m_nCharSet )
	{
		m_nCharSet = cs;
		
		rslt |= View_Redraw;
	}

	if ( width != m_width )
	{
		m_width   = width;
		m_height  = width - 4;
		m_font    = width - 8;

		m_nPixelWidth  = 16 * m_width + 100;
		m_nPixelHeight = m_row * m_height + 100;
		
		rslt = View_Redraw | View_Resize;
	}

	return rslt;
}


void KCharSetView::DispRow(HDC hDC, int x0, int y0, int y, WCHAR ch, int length)
{
	RECT rect = { x0, 
				  y0 + y  * m_height,
				  x0 + 15 * m_width  + m_width-1,
				  y0 + y  * m_height + m_height-1 };
	
	if ( ! RectVisible(hDC, & rect) )
		return;

	char temp[10];

	if ( length==1 )
		wsprintf(temp, "%02X", ch & 0xFF);
	else 
		wsprintf(temp, "%04X", ch & 0xFFFF);
	
	int align = SetTextAlign(hDC, TA_RIGHT);

	HGDIOBJ hOld = SelectObject(hDC, m_hAnsiFont);

	TextOut(hDC, rect.left-5, rect.top, temp, strlen(temp));
	SelectObject(hDC, hOld);

	SetTextAlign(hDC, align);

	for (int x=0; x<16; x++)
	{
		rect.right = rect.left + m_width - 1;		
		
		if ( length==1 )
		{
			char c = (char) (ch + x);
			
			DrawTextA(hDC, & c, 1, & rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
		else if ( length==2 )
		{
			char c[2] = { HIBYTE(ch), LOBYTE(ch) + x };

			DrawTextA(hDC, c, 2, & rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
		else
		{
			WCHAR c = ch + x;

			DrawTextW(hDC, & c, 1, & rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}

		rect.left = rect.right + 1;
	}
}


int KCharSetView::OnDraw(HDC hdc, const RECT * rcPaint)
{
	KLogFont logfont(m_font, "Times New Roman");

	if ( m_nCharSet!=UNICODE_CHARSET )
		logfont.SetCharSet(m_nCharSet);
	else
		logfont.SetCharSet(GB2312_CHARSET);

	HFONT hFont = logfont.Create();
	HGDIOBJ hOld = SelectObject(hdc, hFont);

	int x0 = 120; int y0 = 30;

	for (int i=0; i<=m_row; i++)
	{
		MoveToEx(hdc, x0 + 0,          y0 + i*m_height, NULL); 
		LineTo(hdc,   x0 + 16*m_width, y0 + i*m_height);

		if ( i<=16 )
		{
			MoveToEx(hdc, x0 + i*m_width, y0 + 0, NULL);  
			  LineTo(hdc, x0 + i*m_width, y0 + m_row*m_height);
		}
	}

	for (int y=0; y<16; y++)
		DispRow(hdc, x0, y0, y, y * 16, 1);
	
//	const int MapBIG5[] = { 4, 5, 6, 7, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF };

	if ( m_row>16 )
		for (y=0; y<(m_row-16); y++)
			switch ( m_nCharSet )
			{
				case GB2312_CHARSET:
				//	DispRow(hdc, x0, 30+16 * m_height, y, 0xA1 + y/6, 0xA0 + (y%6)*16, 2);
					DispRow(hdc, x0, 30+16 * m_height, y, Map_936.GetRowFirst(y), 2);
					break;

				case SHIFTJIS_CHARSET:
					DispRow(hdc, x0, 30+16 * m_height, y, Map_932.GetRowFirst(y), 2);
				//	DispRow(hdc, x0, 30+16 * m_height, y, 0x81 + y/12, 0x40 + (y%12)*16, 2);
					break;

				case CHINESEBIG5_CHARSET:
					DispRow(hdc, x0, 30+16 * m_height, y, Map_950.GetRowFirst(y), 2);
				//	DispRow(hdc, x0, 30+16 * m_height, y, 0xA1 + y/10, MapBIG5[y%10]*16, 2);
					break;

				case HANGUL_CHARSET:
//					DispRow(hdc, x0, 30+16 * m_height, y, 0xA1 + y/12, 0x40 + (y%12)*16, 2);
					DispRow(hdc, x0, 30+16 * m_height, y, Map_949.GetRowFirst(y), 2);
					break;

				case UNICODE_CHARSET:
					DispRow(hdc, x0, 30+16 * m_height, y, Map_UC.GetRowFirst(y), 3);
			}

	SelectObject(hdc, hOld);
	DeleteObject(hFont);

	return View_NoChange;
}

