//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : fontembed.cpp					                                     //
//  Description: Font embedding demo program, Chapter 14                             //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <assert.h>

#include "resource.h"
#include "..\..\include\Dialog.h"


#define FR_HIDDEN   0x01
#define FR_MEM		0x02


BOOL RemoveFont(const TCHAR * fontname, int option, HANDLE hFont)
{
	if ( option & FR_MEM )
	{
		return RemoveFontMemResourceEx(hFont);
	}
	
	TCHAR ttffile[MAX_PATH];
	TCHAR fotfile[MAX_PATH];

	GetCurrentDirectory(MAX_PATH-1, ttffile);
	_tcscpy(fotfile, ttffile);

	wsprintf(ttffile + _tcslen(ttffile), "\\%s.ttf", fontname);
	wsprintf(fotfile + _tcslen(fotfile), "\\%s.fot", fontname);

	BOOL rslt;

	switch ( option )
	{
		case 0:
		case FR_HIDDEN:
			rslt = RemoveFontResource(fotfile);
			break;

		case FR_PRIVATE:
		case FR_NOT_ENUM:
		case FR_PRIVATE | FR_NOT_ENUM:
			rslt = RemoveFontResourceEx(fotfile, option, NULL);
			break;

		default:
			assert(false);
			rslt = FALSE;
	}

	if ( ! DeleteFile(fotfile) )
		rslt = FALSE;

	if ( ! DeleteFile(ttffile) )
		rslt = FALSE;

	return rslt;
}


HANDLE InstallFont(void * fontdata, unsigned fontsize, const TCHAR * fontname, int option)
{
	if ( option & FR_MEM )
	{
		DWORD num;

		return AddFontMemResourceEx(fontdata, fontsize, NULL, & num);
	}

	TCHAR ttffile[MAX_PATH];
	TCHAR fotfile[MAX_PATH];

	GetCurrentDirectory(MAX_PATH-1, ttffile);
	_tcscpy(fotfile, ttffile);

	wsprintf(ttffile + _tcslen(ttffile), "\\%s.ttf", fontname);
	wsprintf(fotfile + _tcslen(fotfile), "\\%s.fot", fontname);

	HANDLE hFile = CreateFile(ttffile, GENERIC_WRITE, 0, NULL,
						CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 0);

	if ( hFile==INVALID_HANDLE_VALUE )
		return NULL;

	DWORD dwWritten;
	WriteFile(hFile, fontdata, fontsize, & dwWritten, NULL);
	FlushFileBuffers(hFile);
	CloseHandle(hFile);

	if ( ! CreateScalableFontResource(option & FR_HIDDEN, fotfile, ttffile, NULL) )
		return NULL;

	switch ( option )
	{
		case 0:
		case FR_HIDDEN:
			return (HANDLE) AddFontResource(fotfile);

		case FR_PRIVATE:
		case FR_NOT_ENUM:
		case FR_PRIVATE | FR_NOT_ENUM:
			return (HANDLE) AddFontResourceEx(fotfile, option, NULL);

		default:
			assert(false);
			return NULL;
	}
}

const char key[4] = { 0x31, 0x41, 0x59, 0x26 }; // encryption key, pi

typedef struct
{
	char	Tag[4];			// font
	DWORD	size;			// size of attached font data
	char	FileName[56];
}	FontEmbed;


// Load/unload a document, install/uninstall the fonts embedded inside

int ProcessDocument(const TCHAR * docname, int option, bool load, HANDLE hfont[])
{
	HANDLE handle = CreateFile(docname, GENERIC_READ, FILE_SHARE_READ, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if ( handle == INVALID_HANDLE_VALUE )
		return 0;

	int       processed  = 0;
	DWORD	  dwRead;
	FontEmbed header;

	while ( ReadFile(handle, & header, sizeof(header), & dwRead, NULL) )
	{
		if ( dwRead!=sizeof(header) )
			break;

		if ( strncmp(header.Tag, "FONT", 4) )
			break;

		unsigned size = header.size - sizeof(header);

		BYTE * pData = new BYTE[size];

		if ( pData==NULL )
			break;

		ReadFile(handle, pData, size, & dwRead, NULL);

		// simple decryption
		for (unsigned i=0; i<size; i++)
			pData[i] ^= key[i % 4];

		if ( load )
			hfont[processed] = InstallFont(pData, size, header.FileName, option);
		else
			if ( ! RemoveFont(header.FileName, option, hfont[processed]) )
			{
				delete [] pData;
				break;
			}
		
		delete [] pData;

		if ( hfont[processed] )
			processed ++;
		else
			break;
	}

	CloseHandle(handle);

	return processed;
}

class KFileDialog
{
	void SetupOFN(OPENFILENAME & ofn, TCHAR Filter[], HWND hWnd, const TCHAR * pExtension, const TCHAR * pClass);

public:
	TCHAR	m_FileName[MAX_PATH];
	TCHAR	m_TitleName[MAX_PATH];
	
	// "bmp" "Bitmap Files"
	BOOL GetOpenFileName(HWND hWnd, const TCHAR * pExtension, const TCHAR * pClass);
	BOOL GetSaveFileName(HWND hWnd, const TCHAR * pExtension, const TCHAR * pClass);
};


void KFileDialog::SetupOFN(OPENFILENAME & ofn, TCHAR Filter[], HWND hWnd, const TCHAR * pExtension, const TCHAR * pClass)
{
	memset(& ofn, 0, sizeof(ofn));

	wsprintf(Filter, "%s (*.%s)%c*.%s%c", pClass, pExtension, 0, pExtension, 0);

	ofn.lStructSize		= sizeof(OPENFILENAME);
	ofn.hwndOwner		= hWnd;
	ofn.lpstrFilter		= Filter; // _T("Bitmaps (*.bmp)\0*.bmp\0\0");
	ofn.lpstrFile		= m_FileName;
	ofn.nMaxFile		= MAX_PATH;
	ofn.lpstrFileTitle	= m_TitleName;
	ofn.nMaxFileTitle	= MAX_PATH;
	ofn.lpstrDefExt		= pExtension; // _T("bmp");
	ofn.nFilterIndex	= 1;

	m_FileName[0] = 0;
}


BOOL KFileDialog::GetOpenFileName(HWND hWnd, const TCHAR * pExtension, const TCHAR * pClass)
{
	OPENFILENAME ofn;
	TCHAR Filter[MAX_PATH];

	SetupOFN(ofn, Filter, hWnd, pExtension, pClass);

	ofn.Flags	= OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			
	return ::GetOpenFileName(&ofn);
}


BOOL KFileDialog::GetSaveFileName(HWND hWnd, const TCHAR * pExtension, const TCHAR * pClass)
{
	OPENFILENAME ofn;
	TCHAR Filter[MAX_PATH];

	SetupOFN(ofn, Filter, hWnd, pExtension, pClass);

	ofn.Flags  = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	
	return ::GetSaveFileName(&ofn);
}

// Generate a EMF file with fonts embedded as GDI commands


BOOL GenerateDoc(const TCHAR * docname)
{
	HANDLE out = CreateFile(docname, GENERIC_WRITE, FILE_SHARE_READ, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( out==INVALID_HANDLE_VALUE )
		return FALSE;

	KFileDialog fi;

	while ( fi.GetOpenFileName(NULL, "ttf", "TrueType fonts") )
	{
		HANDLE handle = CreateFile(fi.m_TitleName, GENERIC_READ, FILE_SHARE_READ, 
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
		if ( handle == INVALID_HANDLE_VALUE )
			break;

		DWORD size = GetFileSize(handle, NULL);

		FontEmbed font;
		
		memset(&font, 0, sizeof(font));
		font.Tag[0] = 'F';
		font.Tag[1] = 'O';
		font.Tag[2] = 'N';
		font.Tag[3] = 'T';
		font.size   = sizeof(FontEmbed) + size;

		int len = strlen(fi.m_TitleName);

		for (int i=0; i<len; i++)
			if ( fi.m_TitleName[i]=='.')
				break;
			else
				font.FileName[i] = fi.m_TitleName[i];
		
		DWORD dwWritten, dwRead;

		// write header
		WriteFile(out, & font, sizeof(font), & dwWritten, NULL);

		char buffer[4096];

		// read, write the whole font file
		while ( ReadFile(handle, buffer, sizeof(buffer), & dwRead, NULL) )
		{
			if ( dwRead==0) 
				break;

			// simple encryption
			for (unsigned i=0; i<dwRead; i++)
				buffer[i] ^= key[i % 4];

			WriteFile(out, buffer, dwRead, & dwWritten, NULL);
		}

		CloseHandle(handle);
	}

	CloseHandle(out);

	return TRUE;
}

const TCHAR * FontList[] =
{
	"Euro Sign",
	"Ozzie Black",
	"Ozzie Black Italic",
	NULL
};


class KMyDialog : public KDialog
{
	HINSTANCE m_hInst;
	int		  m_nFontChange;
	HFONT	  m_font[10];
	int		  m_option;

	void GetOption(void)
	{			
		m_option = 0;

		if ( SendDlgItemMessage(m_hWnd, IDC_HIDDEN, BM_GETCHECK, 0, 0)==BST_CHECKED )
			m_option = FR_HIDDEN;

		if ( SendDlgItemMessage(m_hWnd, IDC_PRIVATE, BM_GETCHECK, 0, 0)==BST_CHECKED )
			m_option = FR_PRIVATE;

		if ( SendDlgItemMessage(m_hWnd, IDC_NOENUM, BM_GETCHECK, 0, 0)==BST_CHECKED )
			m_option = FR_NOT_ENUM;

		if ( SendDlgItemMessage(m_hWnd, IDC_MEM, BM_GETCHECK, 0, 0)==BST_CHECKED )
			m_option = FR_MEM;
	}

	void Update(void)
	{
		for (int i=0; FontList[i]; i++)
		{
			if ( m_font[i] )
				DeleteObject(m_font[i]);

			LOGFONT logfont = { - 20,
							 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
							 ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, 
							 PROOF_QUALITY, VARIABLE_PITCH };
			
			_tcscpy(logfont.lfFaceName, FontList[i]);

			if (i==2) 
				logfont.lfItalic = TRUE;

			if (i==0)
				logfont.lfCharSet = SYMBOL_CHARSET;

			m_font[i] = CreateFontIndirect(& logfont);

			HDC      hDC = GetDC(NULL);
			HGDIOBJ hOld = SelectObject(hDC, m_font[i]);

			OUTLINETEXTMETRIC outm[3];
			GetOutlineTextMetrics(hDC, sizeof(outm), outm);
			SelectObject(hDC, hOld);
			DeleteObject(hDC);

			TCHAR name[64];
			
			if ( i==0 )
				_tcscpy(name, "e");
			else
			{
				_tcscpy(name, (char *) outm + (int) outm[0].otmpFamilyName);
				_tcscat(name, " ");
				_tcscat(name, (char *) outm + (int) outm[0].otmpStyleName);
			}

			SetDlgItemText(m_hWnd, IDC_SAMPLE1 + i, name);

			SendDlgItemMessage(m_hWnd, IDC_SAMPLE1 + i, WM_SETFONT, (WPARAM) m_font[i], TRUE);
    	}
	}

	virtual BOOL OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		m_hWnd = hWnd;

		Update();

		return TRUE;
	}

	HANDLE hFont[32];

	virtual BOOL OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		switch ( LOWORD(wParam) )
		{
			case IDOK:
				EndDialog(hWnd, IDOK);
				return TRUE;

			case IDC_GENERATE:
				GenerateDoc("testdoc.tmp");
				return TRUE;

			case IDC_LOAD:
			{
				GetOption();
				int succeeded = ProcessDocument("testdoc.tmp", m_option, true, hFont);

				TCHAR mess[32];
				wsprintf(mess, "%d font(s) installed.", succeeded);
				MessageBox(NULL, mess, "Font Install", MB_OK);

				if ( succeeded )
                    if ( m_option==0 )
						SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
					else
						Update();
			}
				return TRUE;

			case IDC_UNLOAD:
			{
				GetOption();
				int succeeded = ProcessDocument("testdoc.tmp", m_option, false, hFont);

				TCHAR mess[32];
				wsprintf(mess, "%d font(s) uninstalled.", succeeded);
				MessageBox(NULL, mess, "Font Install", MB_OK);

				if ( succeeded )
					if ( m_option==0 )
						SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
					else
						Update();
			}
				return TRUE;
		}

		return FALSE;
	}

    virtual BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
    	switch (uMsg)
	    {
		    case WM_INITDIALOG:
			    return OnInitDialog(hWnd, wParam, lParam);

		    case WM_COMMAND:
			    return OnCommand(hWnd, wParam, lParam);

			case WM_FONTCHANGE:
				{
					TCHAR mess[32];

					wsprintf(mess, "WM_FONTCHANGE %d", ++m_nFontChange);
					SetDlgItemText(hWnd, IDC_MESSAGE, mess);

					Update();
				}
				return TRUE;
	    }

	    return FALSE;
    }

public:
	KMyDialog(HINSTANCE hInst)
	{
		m_hInst       = hInst;
		m_nFontChange = 0;
		m_option      = 0;

		memset(m_font, 0, sizeof(m_font));
	}
};


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	KMyDialog dialog(hInstance);

	return dialog.Dialogbox(hInstance, MAKEINTRESOURCE(IDD_FONT));
}