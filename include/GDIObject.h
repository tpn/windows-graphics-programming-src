#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : gdiobject.h					                                     //
//  Description: GDI object selection/deselection wrapper                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KGDIObject
{
	HGDIOBJ m_hOld;
	HDC		m_hDC;

public:

	HGDIOBJ m_hObj;

	KGDIObject(HDC hDC, HGDIOBJ hObj)
	{
		m_hDC  = hDC;
		m_hObj = hObj;
		m_hOld = SelectObject(hDC, hObj);

		assert(m_hDC);
		assert(m_hObj);
		assert(m_hOld);
	}

	~KGDIObject()
	{
		HGDIOBJ h = SelectObject(m_hDC, m_hOld);
	//	assert(h==m_hObj);

		DeleteObject(m_hObj);
	//	assert(GetObjectType(m_hObj)==0);
	}
};

