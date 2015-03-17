#pragma
//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : gdi.h					                                             //
//  Description: GDI object implementation illustration, Chapter 3                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class _GdiObj
{
public:
	virtual int  GetObjectType(void) = 0;
	virtual int  GetObject(int cbBuffer, void * pBuffer) =0;
	virtual bool DeleteObject(void) = 0;
	virtual bool UnrealizeObject(void) = 0;
};


class _Pen : public _GdiObj
{
public:

	virtual int  GetObjectType(void)
	{
		return OBJ_PEN;
	}

	virtual int  GetObject(int cbBuffer, void * pBuffer) = 0;
	virtual bool DeleteObject(void) = 0;

	virtual bool UnrealizeObject(void)
	{
		return true;
	}
};

_Pen * _CreatePen(int fnPenStyle, int nWidth, COLORREF crColor);


