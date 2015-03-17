//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : progview.cpp					                                     //
//  Description: KProgramPageCanvas window                                           //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500
#define NOCRYPT

#pragma pack(push, 4)
#include <windows.h>
#pragma pack(pop)

#include <assert.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>

#include "..\..\include\win.h"
#include "..\..\include\FrameWnd.h"
#include "..\..\include\GDIObject.h"
#include "..\..\include\pen.h"
#include "..\..\include\filedialog.h"
#include "..\..\include\outputsetup.h"
#include "..\..\include\fonttext.h"
#include "..\..\include\pagecanvas.h"

#include "resource.h"
#include "progview.h"

typedef enum
{
	class_plain,
	class_keyword,
	class_comment,
	class_number,
	class_string,
	class_unknown
};

COLORREF crText[] =
{
	RGB(0x00, 0x00, 0x00),	// black
	RGB(0x00, 0x00, 0xFF),  // blue
	RGB(0x00, 0x80, 0x00),  // dark green
	RGB(0x80, 0x00, 0x00),  // dark red
	RGB(0x00, 0xF0, 0x80),  // dark blue
	RGB(0xFF, 0x00, 0x00)   // red
};

const char * keywordlist[] =
{   
	"#pragma",
	"#define",
	"#include",
	"#ifdef",
	"#ifndef",
	"#else",
	"#endif",

    "__asm",		"__assume",
    "__based",
    "__cdecl", 
    "__declspec",
    "__except",		"__export",
    "__far",		"__fastcall",	"__fortran",	"__finally"
    "__huge",														
	"__inline",     "__int8",		"__int16",		"__int32",      "__int64",
    "__interrupt",
    "__leave",      "__loadds",
    "__multiple_inheritance",
    "__near",
    "__pascal",        
    "__saveregs",	"__segment",	"__segname",	"__self",		"__single_inheritance",
    "__stdcall",
    "__try",
	"__uuidof",
    "__virtual_inheritance",
  
    "_asm",
    "_cdecl",
    "_export",
    "_far",
    "_huge",
    "_interrupt",
    "_loadds",
    "_near",
    "_pascal",
    "_saveregs",
	
    "asm",			"auto",
    "bool",			"break",										
    "case",			"catch",		"char",			"class", 		"const",		
	"const_cast",	"continue",
    "default",		"delete",		"dllexport",	"dllimport", 	"do",
	"double",		"dynamic_cast",
    "else",			"enum",			"explicit",		"extern",		
    "false",		"far",			"fastcall",		"float",		"for",			
	"friend",
    "goto",
    "huge",
    "if",			"inline",		"int",
    "long",
    "main",			"mutable",
    "naked",		"namespace",	"near",			"new",			"noreturn",
    "operator",
    "private",		"protected",	"public",
    "register",		"reinterpret_cast",				"return",
    "short",		"signed",		"sizeof",		"static", 		"static_cast",	
	"struct",		"switch",
    "template",		"this",			"thread",		"throw",		"true",			
	"try",			"typedef",		"typeid",		"typename",
    "union",		"using",		"unsigned",		"uuid",
	"virtual",		"void",			"volatile",
    "while",		"wmain"
};

bool IsKeyword(const TCHAR * word, unsigned len)
{
	for (int k=0; k<sizeof(keywordlist)/sizeof(keywordlist[0]); k++)
		if ( (word[0]==keywordlist[k][0])  && 
			 (len==strlen(keywordlist[k])) &&
			 (strncmp(word, keywordlist[k], len)==0) )
			 return true;

	return false;
}


int skipchar(const TCHAR * text)
{
    if ( text[0]=='\\' )     // escape
        switch ( text[1] )
        {
			case 'a' : case 'b' : case 'f' : case 'n' : 
			case 'r' : case 't' : case 'v' : case '?' : 
			case '\'': case '"' : case '\\':			
				return 2;
            
            case '0' : case '1' : case '2' : case '3' :
            case '4' : case '5' : case '6' : case '8' : 
				return 4;
                      
            case 'x': 
				return 3;
        }

    return 1;
}

void ColorText(const TCHAR * text, BYTE * flag)
{
	while ( text[0] )
		if ( (text[0]=='#') || (text[0]=='_') || (text[0]>='a') && (text[0]<='z') || (text[0]>='A') && (text[0]<='Z') )
		{
			int i = 1;
			
			while ( (text[i]=='_') || (text[i]>='a') && (text[i]<='z') || (text[i]>='A') && (text[i]<='Z') || (text[i]>='0') && (text[i]<='9') )
				i ++;

			memset(flag, class_plain, i);

			if ( IsKeyword(text, i) )
				memset(flag, class_keyword, i);
			else
				memset(flag, class_plain, i);
			
			text += i;
			flag += i;
		}
		else if ( (text[0]>='0') && (text[0]<='9') )
		{
			int i = 0;
			while ( (text[i]>='0') && (text[i]<='9') )
				i ++;

			memset(flag, class_number, i);
			text += i;
			flag += i;
		}
		else switch ( text[0] )
		{
			case '/':
				if ( text[1]=='/' )
				{
					memset(flag, class_comment, _tcslen(text));
					return;
				}

			case '{':
			case '}':
			case '(': 
			case ')':
			case '[':
			case ']':
			case '+':
			case '-':
			case '*':
			case '?':
			case ' ':
			case '<':
			case '>':
			case '.':
			case '=':
			case ';':
			case ',':
			case '&':
			case '~':
			case '|':
			case '^':
			case '!':
			case ':':
				* flag ++ = class_plain;
				text ++; 
				break;
		
			case '\'':
				{
					int len = skipchar(text+1) + 2;
					memset(flag, class_string, len);
					text += len; flag += len;
				}
				break;

			case '"':
				{
					int len = 1;
					while ( text[len] && text[len]!='"' )
					{
						len += skipchar(text+len);
					}

					if ( text[len]=='"' )
						len ++;

					memset(flag, class_string, len);
					text += len; flag += len;
				}
				break;

			default:
				* flag ++ = class_unknown;
				text ++;
		}
}


void KProgramPageCanvas::SyntaxHighlight(HDC hDC, int x, int y, const TCHAR * mess)
{
	BYTE  flag[MAX_PATH];

	int len = _tcslen(mess);

	assert(len < MAX_PATH-50);

	memset(flag, 0, MAX_PATH);
	ColorText(mess, flag);

	float width = 0.0;

	for (int k=0; k<len; )
	{
		int next = 1;
				
		while ( (k+next<len) && (flag[k]==flag[k+next]) )
			next ++;

		SetTextColor(hDC, crText[flag[k]]);

		m_formator.TextOut(hDC, (int) (x + width + 0.5), y, mess+k, next);

		float w, h;
		m_formator.GetTextExtent(hDC, mess+k, next, w, h);
		width += w;

		k += next;
	}
}

// getting lines from text buffer
class KGetline
{
	const char * m_text;
	int			 m_size;

public:
	char		 m_line[MAX_PATH];

	KGetline(const char * text, int size)
	{
		m_text = text;
		m_size = size;
	}

	int Nextline(void)
	{
		if ( m_size<=0 )
			return 0;

		int len = 0;

		while ( (m_size>0) && m_text[0] && (m_text[0]!='\r') ) // get char until CR
		{
			if ( m_text[0] == '\t' )
			{
				do
				{
					m_line[len++] = ' ';
				}
				while ( len % 4);
			}
			else
				m_line[len++] = m_text[0];

			m_text ++; m_size --;
		}

		if ( m_text[1]=='\n' ) // skip LF
		{
			m_size -= 2;
			m_text += 2;
		}
		else
		{
			m_size -= 1;
			m_text += 1;
		}

		m_line[len] = 0;

		return max(1, len);
	}
};

int CountLine(const char * text, int size)
{
	KGetline parser(text, size);

	int lineno = 0;
	while ( parser.Nextline() )
		lineno ++;

	return lineno;
}


void KProgramPageCanvas::UponDrawPage(HDC hDC, const RECT * rcPaint, int width, int height, int pageno)
{
	if ( rcPaint )	// skip painting is current page does not intersect with rcPaint
	{
		RECT rect = { 0, 0, width, height };

		LPtoDP(hDC, (POINT *) & rect, 2);

		if ( ! IntersectRect(& rect, rcPaint, & rect) )
			return;
	}

	HGDIOBJ hOld = SelectObject(hDC, m_hFont);

	SetBkMode(hDC, TRANSPARENT);
	SetTextAlign(hDC, TA_LEFT | TA_TOP);

	KGetline parser(m_pBuffer, m_nSize);

	int skip = pageno * m_nLinePerPage;

	for (int i=0; i<skip; i++)
		parser.Nextline();

	for (i=0; i<m_nLinePerPage; i++)
		if ( parser.Nextline() )
		{
			SyntaxHighlight(hDC, 0, 
				(int)(m_formator.GetLinespace() * i + 0.5), parser.m_line);
		}
		else
			break;

	SelectObject(hDC, hOld);
}

void KProgramPageCanvas::OnCreate(void)
{
	KPageCanvas::OnCreate();
}


void KProgramPageCanvas::RefreshPaper(void)
{
	KSurface::RefreshPaper();

	if ( m_hFont==NULL )
	{
		int pointsize = 9;

		KLogFont logfont(- pointsize * ONEINCH / 72, "Courier New");
		
		m_hFont = logfont.CreateFont();
	
		HDC hDC		 = GetDC(NULL);
		HGDIOBJ hOld = SelectObject(hDC, m_hFont);

		m_formator.SetupPixel(hDC, m_hFont, (float)(pointsize * ONEINCH/72));
		SelectObject(hDC, hOld);

		ReleaseDC(NULL, hDC);
	}
}

int KProgramPageCanvas::OnCommand(int cmd, HWND hWnd)
{
	switch ( cmd )
	{
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
