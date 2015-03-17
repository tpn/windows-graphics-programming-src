//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : memview.h					                                         //
//  Description: Memory dumper dialog box                                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#pragma once

#include "memdump.h"

class KMemViewer : public KDialog, public KMemDump
{
	unsigned char * m_start;
	unsigned char * m_current;
	
	unsigned   m_size;
	unsigned   m_pagesize;
	HFONT      m_hFont;
    
    int  UnitSize(void) const;

	void DumpPage(void);
	void Search(void);

	virtual BOOL OnInitDialog(HWND hWnd, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

public:
    
	KMemViewer(unsigned start, unsigned size)
	{
		m_start  = (unsigned char *) start;
		m_size   = size;
	}
};

