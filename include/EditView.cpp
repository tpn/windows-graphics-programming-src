//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : editview.cpp						                                 //
//  Description: text window with hexdump support                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>

#include "win.h"
#include "Canvas.h"
#include "ScrollCanvas.h"
#include "EditView.h"
#include "DDB.h"
#include "Background.h"

class KCount
{
protected:
	DWORD Used[1024];

public:	
	int   nUsed;

	KCount()
	{
		nUsed = 0;
	}

	void Check(DWORD value);
};


void KCount::Check(DWORD value)
{
	if ( nUsed>=1024 )
		return;

	for (int i=0; i<nUsed; i++)
		if ( Used[i]==value )
			return;

	Used[nUsed++] = value;
}


void Append(HWND hWnd, const TCHAR * buffer)
{
	SendMessage(hWnd, EM_SETSEL, 0xFFFFFFF, 0xFFFFFFF);
	SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM) buffer);
}


void AppendHex(HWND hWnd, const BYTE * pBuffer, int size, int unit)
{
	TCHAR text[8192];

	wsprintf(text, _T("size %d bytes\r\n\r\n"), size);
	Append(hWnd, text);

	KCount count;

	int len = 0;

	for (int i=0; i<size; i+=16 )
	{
		wsprintf(text+len, _T("%05X: "), i); len += 7;

		if ( unit==4 )	// 32 bit
		{
			for (int j=0; j<16; j+=4) 
				if ( (i+j)<size )
				{
					DWORD value = (DWORD) * (DWORD *) (pBuffer+j);

					count.Check(value);

					wsprintf(text + len, _T("%08X "), value); len += 9;
				}
		}
		else			// 8 bit
		{
			for (int j=0; j<16; j++) 
				if ( (i+j)<size )
				{
					DWORD value = pBuffer[j];

					count.Check(value);
					wsprintf(text + len, _T("%02X "), value); len += 3;
				}
		}

		_tcscat(text + len, _T("\r\n")); len += 2;

		if ( len + MAX_PATH >= sizeof(text)/sizeof(TCHAR) )
		{
			text[len] = 0;
			Append(hWnd, text);
			len = 0;
		}
		
		pBuffer += 16;
	}

	text[len] = 0;
	Append(hWnd, text);

	wsprintf(text, _T("size %d %d bytes %d unique\r\n"), i, size, count.nUsed);
	Append(hWnd, text);
}


LRESULT KEditView::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_CREATE:
			m_hWnd		= hWnd;
			m_hEdit     = CreateWindow(_T("EDIT"), NULL, 
				WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | 
				ES_WANTRETURN | ES_MULTILINE | WS_HSCROLL | WS_VSCROLL,
				0, 0, 0, 0, m_hWnd, NULL, m_hInstance, NULL);

			SendMessage(m_hEdit, WM_SETFONT, (WPARAM) GetStockObject(ANSI_FIXED_FONT), 0);
			SendMessage(m_hEdit, EM_LIMITTEXT, 512 * 1024, 0);	// 512 Kb limit

//			m_pBackground = new CDDBBackground;
//			if ( m_pBackground )
//			{
//				m_pBackground->Attach(m_hEdit);
//			}				
			return 0;

		case WM_SIZE:
			MoveWindow(m_hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
			return 0;

//		case WM_DESTROY:
//			if ( m_pBackground )
//			{
//				m_pBackground->Detatch(m_hEdit);
//				delete m_pBackground;
//				m_pBackground = NULL;
//			}

		default:
			return CommonMDIChildProc(hWnd, uMsg, wParam, lParam, NULL, 0);
	}
}


bool KEditView::Initialize(HINSTANCE hInstance)
{
	m_hInstance = hInstance;

	RegisterClass(_T("EditView"), hInstance);
			
	return true;
}


void KEditView::Decode(DWORD m_Data, DWORD m_Type)
{
	TCHAR mess[128];

	if ( m_Type==1 )
	{
		HBITMAP hBmp = (HBITMAP) m_Data;

		wsprintf(mess, _T("DDB %x "), m_Data);
		DecodeDDB(hBmp, mess + _tcslen(mess));

		BITMAP bmp;
		GetObject(hBmp, sizeof(bmp), & bmp);

		int size = GetBitmapBits(hBmp, 0, NULL);
		BYTE * pBuffer = new BYTE[size];

		Append(m_hEdit, mess);
		Append(m_hEdit, _T("\r\n"));
			
		if ( pBuffer )
		{
			GetBitmapBits(hBmp, size, pBuffer);

			if ( bmp.bmBitsPixel==32 )
				AppendHex(m_hEdit, pBuffer, size, 4);
			else
				AppendHex(m_hEdit, pBuffer, size, 1);

			delete [] pBuffer;
		}

		mess[0] = 0;
	}
	else
		wsprintf(mess, _T("DIB %x "), m_Data);
		
	Append(m_hEdit, mess);
}
