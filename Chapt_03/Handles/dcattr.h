//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dcattr.h					                                         //
//  Description: Device context data structure analysis, Chapter 3                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

typedef struct
{
	ULONG ul1;
    ULONG ul2;
}	FLOATOBJ;

typedef struct
{
	FLOATOBJ efM11;
	FLOATOBJ efM12;
	FLOATOBJ efM21;
	FLOATOBJ efM22;
	FLOATOBJ efDx;
	FLOATOBJ efDy;
	int		 fxDx;
	int		 fxDy;
	long	 flAccel;
}	MATRIX;


// Windows NT 4.0: 0x190 bytes
// Windows 2000  : 0x1C8 bytes
typedef struct 
{
	void *		pvLDC;					// 000
	ULONG		ulDirty;
	HBRUSH		hbrush;
	HPEN		hpen;

	COLORREF	crBackgroundClr;		// 010
	ULONG		ulBackgroundClr;
	COLORREF	crForegroundClr;
	ULONG		ulForegroundClr;

#if (_WIN32_WINNT >= 0x0500)
	unsigned	f20[4];					// 020
#endif	

	int			iCS_CP;					// 030
	int			iGraphicsMode;
	BYTE		jROP2;					// 038
	BYTE		jBkMode;
	BYTE		jFillMode;
	BYTE		jStretchBltMode;

	POINT		ptlCurrent;				// 03C
	POINTFX		ptfxCurrent;			// 044
	long		lBkMode;				// 04C

	long		lFillMode;				// 050
	long		lStretchBltMode;

#if (_WIN32_WINNT >= 0x0500)
	long		flFontMapper;			// 058
	long		lIcmMode;
	unsigned	hcmXform;				// 060
	HCOLORSPACE hColorSpace;
	unsigned	f68;
	unsigned	IcmBrushColor;
	unsigned	IcmPenColor;			// 070
	unsigned	f74;
#endif

	long		flTextAlign;			// 078
	long		lTextAlign;
	long		lTextExtra;				// 080
	long		lRelAbs;
	long		lBreakExtra;
	long		cBreak;

	HFONT		hlfntNew;				// 090
	MATRIX		mxWorldToDevice;		// 094
	MATRIX		mxDeviceToWorld;		// 0D0
	MATRIX		mxWorldToPage;			// 10C

	unsigned	f148[8];				// 148

	int			iMapMode;				// 168

#if (_WIN32_WINNT >= 0x0500)
	DWORD		dwLayout;				// 16c
	long		lWindowOrgx;			// 170
#endif
	
	POINT		ptlWindowOrg;			// 174
	SIZE		szlWindowExt;			// 17c
	POINT		ptlViewportOrg;			// 184
	SIZE		szlViewportExt;			// 18c

	long		flXform;				// 194
	SIZE		szlVirtualDevicePixel;	// 198
	SIZE		szlVirtualDeviceMm;		// 1a0
	POINT		ptlBrushOrigin;			// 1a8

	unsigned	f1b0[2];				// 1b0	
	RECT		VisRectRegion;			// 1b8
}	DCAttr;

