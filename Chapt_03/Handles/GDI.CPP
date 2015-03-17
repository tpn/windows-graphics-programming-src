//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : gdi.cpp					                                         //
//  Description: GDI object implementation illustration, Chapter 3                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define  STRICT
#define  WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "gdi.h"

class _RealPen : public _Pen
{
	LOGPEN m_LogPen;

public:
	_RealPen(int fnPenStyle, int nWidth, COLORREF crColor)
	{
		m_LogPen.lopnStyle   = fnPenStyle;
		m_LogPen.lopnWidth.x = nWidth;
		m_LogPen.lopnWidth.y = 0;
		m_LogPen.lopnColor   = crColor;
	}

	int GetObject(int cbBuffer, void * pBuffer)
	{
		if ( pBuffer==NULL )
			return sizeof(LOGPEN);
		else if ( cbBuffer>=sizeof(m_LogPen) )
		{
			memcpy(pBuffer, & m_LogPen, sizeof(m_LogPen));
			return sizeof(LOGPEN);
		}
		else
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return 0;
		}
	}

	bool DeleteObject(void)
	{
		if ( this )
		{
			delete this;
			return true;
		}
		else
			return false;
	}
};


_Pen * _CreatePen(int fnPenStyle, int nWidth, COLORREF crColor)
{
	return new _RealPen(fnPenStyle, nWidth, crColor);
}

void Test(void)
{
	_Pen * pPen = _CreatePen(PS_SOLID, 1, RGB(0, 0, 0xFF));
	////
	pPen->DeleteObject();
}