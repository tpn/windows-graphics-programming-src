//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : ddiproxy.cpp	                                                     //
//  Description: Proxy routines for device driver entry points                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT
#define _WIN32_WINNT 0x0500
#define WINNT

#include <windows.h>
#include <winddi.h>

#include "DDISpy.h"

typedef struct
{
	PFN   pProxy;
	PFN   pReal;
}   PROXYFN;

#define DrvSetPalette			NULL
#define DrvDitherColor			NULL
#define DrvDeleteDeviceBitmap	NULL
#define DrvCreateDeviceBitmap	NULL
#define DrvOffset				NULL
#define DrvAssertMode			NULL
#define DrvRealizeBrush			NULL

PROXYFN DDI_Proxy [] = 
{
	(PFN) DrvEnablePDEV,			NULL,
	(PFN) DrvCompletePDEV,			NULL,
	(PFN) DrvDisablePDEV,			NULL,
    (PFN) DrvEnableSurface,			NULL,
    
	(PFN) DrvDisableSurface,		NULL,
    (PFN) DrvAssertMode,			NULL,
    (PFN) DrvOffset,				NULL,
    (PFN) DrvResetPDEV,				NULL,
    
	(PFN) DrvDisableDriver,			NULL,
	(PFN) NULL,						NULL,
    (PFN) DrvCreateDeviceBitmap,	NULL,
    (PFN) DrvDeleteDeviceBitmap,	NULL,
    
	(PFN) DrvRealizeBrush,			NULL,
    (PFN) DrvDitherColor,			NULL,
    (PFN) DrvStrokePath,			NULL,
    (PFN) DrvFillPath,				NULL,
    
	(PFN) DrvStrokeAndFillPath,		NULL,
    (PFN) DrvPaint,					NULL,
    (PFN) DrvBitBlt,				NULL,
    (PFN) DrvCopyBits,				NULL,
    
	(PFN) DrvStretchBlt,			NULL,
	(PFN) NULL,						NULL,
    (PFN) DrvSetPalette,			NULL,
    (PFN) DrvTextOut,				NULL,
};

unsigned char Buffer[BUFFER_SIZE];
int			  BufferPos;

void DDISpy_Start(unsigned fntable, int count)
{
	unsigned * pFuncTable = (unsigned *) fntable;

	memset(Buffer, 0, sizeof(Buffer));
	BufferPos = 0;

	for (int i=0; i<count; i++)
		if ( pFuncTable[i] > 0xa0000000  ) // either in win32k.sys or device driver
		if ( DDI_Proxy[i].pProxy != NULL ) // we have a proxy
		{
			DDI_Proxy[i].pReal = (PFN) pFuncTable[i];		// actual function to call

			pFuncTable[i] = (unsigned) DDI_Proxy[i].pProxy;	// hack it !!!
		}
}

void DDISpy_Stop(unsigned fntable, int count)
{
	unsigned * pFuncTable = (unsigned *) fntable;

	for (int i=0; i<count; i++)
		if ( pFuncTable[i] > 0xa0000000  ) // either in win32k.sys or device driver
		if ( DDI_Proxy[i].pProxy != NULL ) // we have a proxy
		{
			pFuncTable[i] = (unsigned) DDI_Proxy[i].pReal;	// reWrite it !!!
		}
}

/*
BOOL APIENTRY DrvEnableDriver(ULONG iEngineVersion, ULONG cj, DRVENABLEDATA *pded)
{
    return FALSE; 
}
*/


void Write(unsigned ch)
{
	Buffer[BufferPos] = ch;
	BufferPos = (BufferPos + 1) % BUFFER_SIZE;
}

int call = 0;

void Write(const char * str)
{
	call ++;

	Write((char)(call / 1000 + '0')); 
	Write((char)(call / 100 % 10 + '0'));
	Write((char)(call / 10 % 10  + '0'));
	Write((char)(call      % 10 + '0'));
	Write(' ');

	while ( * str )
	{
		Write( * str );
		str ++;
	}
	Write(' ');
}


#define Call(name)  (*(PFN_ ## name) DDI_Proxy[INDEX_ ## name].pReal)

void APIENTRY DrvDisableDriver(void)
{
	Write("DisableDriver");
	
	Call(DrvDisableDriver)();
}


DHPDEV APIENTRY DrvEnablePDEV(DEVMODEW *pdm,
              LPWSTR    pwszLogAddress,
              ULONG     cPat,
              HSURF    *phsurfPatterns,
              ULONG     cjCaps,
              ULONG    *pdevcaps,
              ULONG     cjDevInfo,
              DEVINFO  *pdi,
              HDEV      hdev,
              PWSTR     pwszDeviceName,
              HANDLE    hDriver)
{
	Write("EnablePDEV");

	return Call(DrvEnablePDEV) (pdm, pwszLogAddress, cPat, phsurfPatterns, 
		cjCaps, (GDIINFO *) pdevcaps, cjDevInfo, pdi, hdev, pwszDeviceName, hDriver);
}


void APIENTRY DrvCompletePDEV(DHPDEV dhpdev, HDEV hdev)
{
	Write("CompletePDEV");

	Call(DrvCompletePDEV) (dhpdev, hdev);
}


BOOL APIENTRY DrvResetPDEV(DHPDEV dhpdevOld, DHPDEV dhpdevNew)
{
	Write("ResetPDEV");

	return Call(DrvResetPDEV) (dhpdevOld, dhpdevNew);
}


void APIENTRY DrvDisablePDEV(DHPDEV dhpdev)
{
	Write("DisablePDEV");

	Call(DrvDisablePDEV) (dhpdev);
}


HSURF APIENTRY DrvEnableSurface(DHPDEV dhpdev)
{
	Write("EnableSurface");

	return Call(DrvEnableSurface) (dhpdev);
}


void APIENTRY DrvDisableSurface(DHPDEV dhpdev)
{
	Write("DisableSurface");

	Call(DrvDisableSurface) (dhpdev);
}

BOOL APIENTRY DrvStrokePath(SURFOBJ   *pso,
							PATHOBJ   *ppo,
							CLIPOBJ   *pco,
                            XFORMOBJ  *pxo, 
							BRUSHOBJ  *pbo,
							POINTL    *pptlBrushOrg,
                            LINEATTRS *plineattrs,
							MIX        mix)
{
	Write("StrokePath");

	return Call(DrvStrokePath) (pso, ppo, pco, pxo, pbo, pptlBrushOrg, plineattrs, mix);
}


BOOL APIENTRY DrvFillPath(SURFOBJ  *pso,
						  PATHOBJ  *ppo,
						  CLIPOBJ  *pco,
						  BRUSHOBJ *pbo,
                          POINTL   *pptlBrushOrg,
						  MIX       mix,
						  FLONG     flOptions)
{
	Write("FillPath");

	return Call(DrvFillPath) (pso, ppo, pco, pbo, pptlBrushOrg, mix, flOptions);
}


BOOL APIENTRY DrvStrokeAndFillPath(SURFOBJ *pso,PATHOBJ *ppo,CLIPOBJ *pco,
                                   XFORMOBJ *pxo,BRUSHOBJ *pboStroke,
                                   LINEATTRS *plineattrs,BRUSHOBJ *pboFill,
                                   POINTL *pptlBrushOrg,MIX mixFill,FLONG flOptions)
{
	Write("StrokeAndFillPath");

	return Call(DrvStrokeAndFillPath) (pso, ppo, pco, pxo, pboStroke, plineattrs,
		pboFill, pptlBrushOrg, mixFill, flOptions);
}


BOOL APIENTRY DrvLineTo(SURFOBJ *pso, CLIPOBJ *pco, BRUSHOBJ *pbo, LONG x1, 
                        LONG y1, LONG x2, LONG y2, RECTL *prclBounds, MIX mix)
{
	Write("LineTo");

	return Call(DrvLineTo)(pso, pco, pbo, x1, y1, x2, y2, prclBounds, mix);
}


BOOL APIENTRY DrvPaint(SURFOBJ  *pso, 
					   CLIPOBJ  *pco, 
					   BRUSHOBJ *pbo,
					   POINTL   *pptlBrushOrg,
					   MIX       mix)
{
	Write("Paint");

	return Call(DrvPaint) (pso, pco, pbo, pptlBrushOrg, mix);
}


BOOL APIENTRY DrvBitBlt(SURFOBJ  *psoTrg,
						SURFOBJ  *psoSrc,
						SURFOBJ  *psoMask,
                        CLIPOBJ  *pco,
						XLATEOBJ *pxlo,
						RECTL    *prclTrg,
						POINTL   *pptlSrc,
                        POINTL   *pptlMask,
						BRUSHOBJ *pbo,
						POINTL   *pptlBrush,
						ROP4      rop4)
{
	Write("BitBlt");

	return Call(DrvBitBlt) (psoTrg, psoSrc, psoMask, pco, pxlo, prclTrg, 
		pptlSrc, pptlMask, pbo, pptlBrush, rop4);
}


BOOL APIENTRY DrvCopyBits(SURFOBJ  *psoDest,
						  SURFOBJ  *psoSrc,
						  CLIPOBJ  *pco,
                          XLATEOBJ *pxlo,
						  RECTL    *prclDest,
						  POINTL   *pptlSrc)
{
	Write("CopyBits");

	return Call(DrvCopyBits)(psoDest, psoSrc, pco, pxlo, prclDest, pptlSrc);
}


BOOL APIENTRY DrvStretchBlt(SURFOBJ         *psoDest,
							SURFOBJ         *psoSrc,
							SURFOBJ         *psoMask,
                            CLIPOBJ         *pco,
							XLATEOBJ        *pxlo,
							COLORADJUSTMENT *pca,
                            POINTL          *pptlHTOrg,
							RECTL           *prclDest,
                            RECTL           *prclSrc,
                            POINTL          *pptlMask,
							ULONG            iMode)
{
	Write("StretchBlt");

	return Call(DrvStretchBlt) (psoDest, psoSrc, psoMask, pco, pxlo, pca, pptlHTOrg,
		prclDest, prclSrc, pptlMask, iMode);
}


BOOL APIENTRY DrvTextOut(SURFOBJ  *pso,
           STROBJ   *pstro,
           FONTOBJ  *pfo,
           CLIPOBJ  *pco,
           RECTL    *prclExtra,
           RECTL    *prclOpaque,
           BRUSHOBJ *pboFore,
           BRUSHOBJ *pboOpaque,
           POINTL   *pptlOrg,
           MIX       mix)
{
	Write("TextOut");

	return Call(DrvTextOut) (pso, pstro, pfo, pco, prclExtra, prclOpaque, 
		pboFore, pboOpaque, pptlOrg, mix);
}
