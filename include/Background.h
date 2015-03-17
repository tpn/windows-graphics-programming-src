#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : background.h					                                     //
//  Description: Bitmap window background, DDB background                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

// abstract class for overwriting background drawing
class KBackground
{
	WNDPROC m_OldProc;

	virtual LRESULT EraseBackground(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK BackGroundWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	KBackground()
	{
		m_OldProc = NULL;
	}

	virtual ~KBackground()
	{
	}

	BOOL Attach(HWND hWnd);
	BOOL Detatch(HWND hWnd);
};

// concrete class for overwriting background drawing using DDB
class KDDBBackground : public KBackground
{
	KDDB	m_DDB;
	int		m_nStyle;

	virtual LRESULT EraseBackground(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	KDDBBackground()
	{
		m_nStyle = KDDB::draw_tile;
	}

	void SetStyle(int style)
	{
		m_nStyle = style;
	}

	void SetBitmap(HMODULE hModule, int nRes)
	{
		m_DDB.LoadBitmap(hModule, nRes);
	}
};

