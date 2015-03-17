#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : checkmark.h					                                     //
//  Description: Use DDB as menu checkmarks                                          //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KCheckMark
{
protected:
	typedef enum { MAXSUBIMAGES = 50 };

	HBITMAP m_hBmp;
	int		m_nSubImageId[MAXSUBIMAGES];
	HBITMAP m_hSubImage  [MAXSUBIMAGES];
	int     m_nUsed;

public:	
	KCheckMark()
	{
		m_hBmp  = NULL;
		m_nUsed = 0;
	}

	~KCheckMark();

	void    AddBitmap(int id, HBITMAP hBmp);
	void    LoadToolbar(HMODULE hModule, int resid, bool transparent=false);
	HBITMAP GetSubImage(int id);
	BOOL    SetCheckMarks(HMENU hMenu, UINT uPos, UINT uFlags, int unchecked, int checked);
};
