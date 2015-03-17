//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : htmldrv.cpp					                                     //
//  Description: HTML printer driver entry points, Chapter 2                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define _WIN32_WINNT 0x0500 
#define  WINVER      0x0500 
#define NOCRYPT

#define UNICODE
#define _UNICODE
#define _X86_ 1
#define WINNT 1 

#include <windows.h>

#pragma message("Add DDK to VC include and library directories")

#include <winddi.h>

#include "Device.h"
 
#pragma comment(lib, "win32k.lib")

const DRVFN DDI_Hooks[] =
{
    INDEX_DrvEnablePDEV,       (PFN) DrvEnablePDEV,
    INDEX_DrvCompletePDEV,     (PFN) DrvCompletePDEV,
    INDEX_DrvResetPDEV,        (PFN) DrvResetPDEV,
    INDEX_DrvDisablePDEV,      (PFN) DrvDisablePDEV,
    INDEX_DrvEnableSurface,    (PFN) DrvEnableSurface,
    INDEX_DrvDisableSurface,   (PFN) DrvDisableSurface,

    INDEX_DrvStartDoc,         (PFN) DrvStartDoc,
    INDEX_DrvEndDoc,           (PFN) DrvEndDoc,
    INDEX_DrvStartPage,        (PFN) DrvStartPage,
    INDEX_DrvSendPage,         (PFN) DrvSendPage,

    INDEX_DrvStrokePath,       (PFN) DrvStrokePath,
    INDEX_DrvFillPath,         (PFN) DrvFillPath,
    INDEX_DrvStrokeAndFillPath,(PFN) DrvStrokeAndFillPath,
    INDEX_DrvLineTo,           (PFN) DrvLineTo,
    INDEX_DrvPaint,            (PFN) DrvPaint,
    INDEX_DrvBitBlt,           (PFN) DrvBitBlt,
    INDEX_DrvCopyBits,         (PFN) DrvCopyBits,
    INDEX_DrvStretchBlt,       (PFN) DrvStretchBlt,
    INDEX_DrvTextOut,          (PFN) DrvTextOut
};

#define PaperWidth   85
#define PaperHeight 110
#define Dpi			 96


BOOL APIENTRY DrvEnableDriver(ULONG iEngineVersion, ULONG cj, DRVENABLEDATA *pded)
{
    // Validate parameters.
    if (iEngineVersion < DDI_DRIVER_VERSION)
    {
        EngSetLastError(ERROR_BAD_DRIVER_LEVEL);
        return FALSE;
    }

    if (cj < sizeof(DRVENABLEDATA))
    {
        EngSetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    pded->iDriverVersion = DDI_DRIVER_VERSION;
    pded->c              = sizeof(DDI_Hooks) / sizeof(DDI_Hooks[0]);
    pded->pdrvfn         = (DRVFN *) DDI_Hooks;

    return TRUE;
}


void APIENTRY DrvDisableDriver(void)
{
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
    if ( (cjCaps<sizeof(GDIINFO)) || (cjDevInfo<sizeof(DEVINFO)) )
    {
        EngSetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    KDevice * pDevice;
    
    // Allocate physical device object. Tag = HTMD
    pDevice = (KDevice *) EngAllocMem (FL_ZERO_MEMORY, sizeof(KDevice), 'DMTH');
    if (pDevice == NULL)
    {
        EngSetLastError(ERROR_OUTOFMEMORY);
        return NULL;
    }

	pDevice->Create();

    pDevice->hSpooler   = hDriver;
	pDevice->hPalette   = EngCreatePalette (PAL_BGR, 0, 0, 0, 0, 0);

    if (pdm == NULL || pdm->dmOrientation == DMORIENT_PORTRAIT)
	{
		pDevice->width  = PaperWidth;
		pDevice->height = PaperHeight;
	}
	else
	{
		pDevice->width  = PaperHeight;
		pDevice->height = PaperWidth;
	}

	// GDIINFO initialized by GDI before calling DrvEnablePDEV
	PGDIINFO pgdiinfo = (PGDIINFO) pdevcaps;
    
	memset(pgdiinfo, 0, sizeof(GDIINFO));

	pgdiinfo->ulVersion    = GDI_DRIVER_VERSION;
    pgdiinfo->ulTechnology = DT_RASPRINTER;
	pgdiinfo->ulHorzSize   = pDevice->width  * 254 / 10;
    pgdiinfo->ulVertSize   = pDevice->height * 254 / 10;
    pgdiinfo->ulHorzRes    = pDevice->width  * Dpi / 10;
    pgdiinfo->ulVertRes    = pDevice->height * Dpi / 10;

	pgdiinfo->cBitsPixel   = 24;
	pgdiinfo->cPlanes      = 1;
	pgdiinfo->ulNumColors  = 512;
	pgdiinfo->flRaster     = 0;
    pgdiinfo->ulLogPixelsX = Dpi;
	pgdiinfo->ulLogPixelsY = Dpi;

	pgdiinfo->ulAspectX    = 1000;
	pgdiinfo->ulAspectY    = 1000;
    pgdiinfo->ulAspectXY   = 1414;  
    
	pgdiinfo->xStyleStep   = 1;
	pgdiinfo->yStyleStep   = 1;
	pgdiinfo->denStyleStep = 10;
    
	pgdiinfo->ptlPhysOffset.x = Dpi / 4; // 0.25 inch
	pgdiinfo->ptlPhysOffset.y = Dpi / 4; // 0.25 inch
    pgdiinfo->szlPhysSize.cx  = pDevice->width  * Dpi / 10;
    pgdiinfo->szlPhysSize.cy  = pDevice->height * Dpi / 10;

	pgdiinfo->ulDevicePelsDPI = Dpi;
	pgdiinfo->ulPrimaryOrder  = PRIMARY_ORDER_ABC;
	pgdiinfo->ulHTPatternSize = HT_PATSIZE_DEFAULT;
	pgdiinfo->ulHTOutputFormat= HT_FORMAT_24BPP;
    pgdiinfo->flHTFlags       = HT_FLAG_HAS_BLACK_DYE;

	// DEVINFO, initialized by GDI, driver only set what it understands
    pdi->flGraphicsCaps = GCAPS_BEZIERS | 
						  GCAPS_GEOMETRICWIDE |
						  GCAPS_ALTERNATEFILL |
						  GCAPS_WINDINGFILL |
						  GCAPS_HALFTONE |
						  GCAPS_HORIZSTRIKE |
						  GCAPS_VERTSTRIKE |
						  GCAPS_OPAQUERECT |
						  GCAPS_VECTORFONT;
    pdi->iDitherFormat = BMF_24BPP;
	pdi->hpalDefault   = pDevice->hPalette;
    
	return (DHPDEV) pDevice;
}


void APIENTRY DrvCompletePDEV(DHPDEV dhpdev, HDEV hdev)
{
    KDevice * pDevice = (KDevice *)dhpdev;
    
	pDevice->hDevice = hdev;
}


BOOL APIENTRY DrvResetPDEV(DHPDEV dhpdevOld, DHPDEV dhpdevNew)
{
    return TRUE;
}


void APIENTRY DrvDisablePDEV(DHPDEV dhpdev)
{
    KDevice * pDevice = (KDevice *)dhpdev;

	if ( pDevice->hPalette )
		EngDeletePalette(pDevice->hPalette);

    EngFreeMem(pDevice);
}


HSURF APIENTRY DrvEnableSurface(DHPDEV dhpdev)
{
    KDevice * pDevice = (KDevice *) dhpdev;

    SIZEL sizl = { pDevice->width * Dpi / 10, pDevice->height * Dpi / 10 };
    
    pDevice->hSurface = (HSURF) EngCreateBitmap(sizl, sizl.cy, BMF_24BPP, 
		                            BMF_NOZEROINIT,  NULL);

    if (pDevice->hSurface == NULL)
        return NULL;
 
    EngAssociateSurface(pDevice->hSurface, pDevice->hDevice, 
		HOOK_BITBLT | HOOK_STRETCHBLT | HOOK_TEXTOUT | HOOK_PAINT | 
		HOOK_STROKEPATH | HOOK_FILLPATH | HOOK_STROKEANDFILLPATH | 
		HOOK_COPYBITS | HOOK_LINETO);

    return pDevice->hSurface;
}


void APIENTRY DrvDisableSurface(DHPDEV dhpdev)
{
    KDevice * pDevice = (KDevice *)dhpdev;

    EngDeleteSurface(pDevice->hSurface);
	pDevice->hSurface = NULL;
}


BOOL  APIENTRY DrvStartPage(SURFOBJ *pso)
{
    KDevice * pDevice = (KDevice *) pso->dhpdev;

    if ( pDevice->StartPage(& pso, 1) )
    {
		RECTL rect = { 0, 0, pDevice->width * Dpi / 10, pDevice->height * Dpi / 10 };
		
		EngEraseSurface(pso, & rect, 0xFFFFFF);

        return TRUE;
    }
    else
        return FALSE;
}

BOOL APIENTRY DrvStartDoc(SURFOBJ *pso, PWSTR pwszDocName, DWORD dwJobId)
{
    KDevice * pDevice = (KDevice *) pso->dhpdev;

	return pDevice->StartDoc(pwszDocName, & pso, 3);
}


BOOL APIENTRY DrvEndDoc(SURFOBJ *pso, FLONG fl)
{
    KDevice * pDevice = (KDevice *) pso->dhpdev;

    return pDevice->EndDoc(&pso, 2);
}


BOOL APIENTRY DrvSendPage(SURFOBJ *pso)
{
    KDevice * pDevice = (KDevice *) pso->dhpdev;

	if ( pDevice->SendPage(& pso, 1) )
	{
		pDevice->DumpSurface(pso);
		
		return TRUE;
	}
	else
		return FALSE;
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
    KDevice * pDevice = (KDevice *) pso->dhpdev;

    if ( pDevice->CallEngine(INDEX_DrvStrokePath, & pso, 8) )
		return EngStrokePath(pso, ppo, pco, pxo, pbo, pptlBrushOrg, plineattrs, mix);
    else
        return FALSE;
}


BOOL APIENTRY DrvFillPath(SURFOBJ  *pso,
						  PATHOBJ  *ppo,
						  CLIPOBJ  *pco,
						  BRUSHOBJ *pbo,
                          POINTL   *pptlBrushOrg,
						  MIX       mix,
						  FLONG     flOptions)
{
    KDevice * pDevice = (KDevice *) pso->dhpdev;

    if ( pDevice->CallEngine(INDEX_DrvFillPath, & pso, 7) )
        return EngFillPath(pso, ppo, pco, pbo, pptlBrushOrg, mix, flOptions);
    else
        return FALSE;
}


BOOL APIENTRY DrvStrokeAndFillPath(SURFOBJ *pso,PATHOBJ *ppo,CLIPOBJ *pco,
                                   XFORMOBJ *pxo,BRUSHOBJ *pboStroke,
                                   LINEATTRS *plineattrs,BRUSHOBJ *pboFill,
                                   POINTL *pptlBrushOrg,MIX mixFill,FLONG flOptions)
{
    KDevice * pDevice = (KDevice *) pso->dhpdev;

    if ( pDevice->CallEngine(INDEX_DrvStrokeAndFillPath, &pso, 10) )
        return EngStrokeAndFillPath(pso, ppo, pco, pxo, pboStroke, plineattrs, 
		           pboFill, pptlBrushOrg, mixFill, flOptions);
    else
        return FALSE;
}


BOOL APIENTRY DrvLineTo(SURFOBJ *pso, CLIPOBJ *pco, BRUSHOBJ *pbo, LONG x1, 
                        LONG y1, LONG x2, LONG y2, RECTL *prclBounds, MIX mix)
{
    KDevice * pDevice = (KDevice *) pso->dhpdev;

    if ( pDevice->CallEngine(INDEX_DrvLineTo, &pso, 9) )
        return EngLineTo(pso, pco, pbo, x1, y1, x2, y2, prclBounds, mix);
    else
        return FALSE;
}


BOOL APIENTRY DrvPaint(SURFOBJ  *pso, 
					   CLIPOBJ  *pco, 
					   BRUSHOBJ *pbo,
					   POINTL   *pptlBrushOrg,
					   MIX       mix)
{
    KDevice * pDevice = (KDevice *) pso->dhpdev;

    if ( pDevice->CallEngine(INDEX_DrvPaint, & pso, 5) )
        return EngPaint(pso, pco, pbo, pptlBrushOrg, mix);
    else
        return FALSE;
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
    KDevice * pDevice = (KDevice *) psoTrg->dhpdev;

    if ( pDevice->CallEngine(INDEX_DrvBitBlt, &psoTrg, 11) )
		return EngBitBlt(psoTrg, psoSrc, psoMask, pco, pxlo, prclTrg, 
		            pptlSrc, pptlMask, pbo, pptlBrush, rop4);
    else
        return FALSE;
}


BOOL APIENTRY DrvCopyBits(SURFOBJ  *psoDest,
						  SURFOBJ  *psoSrc,
						  CLIPOBJ  *pco,
                          XLATEOBJ *pxlo,
						  RECTL    *prclDest,
						  POINTL   *pptlSrc)
{
    KDevice * pDevice = (KDevice *) psoDest->dhpdev;

    if (pDevice->CallEngine(INDEX_DrvCopyBits, &psoDest, 6) )
        return EngCopyBits(psoDest, psoSrc, pco, pxlo, prclDest, pptlSrc);
    else
        return FALSE;
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
    KDevice * pDevice = (KDevice *) psoDest->dhpdev;

    if ( pDevice->CallEngine(INDEX_DrvStretchBlt, & psoDest, 11) )
        return EngStretchBlt(psoDest, psoSrc, psoMask, pco, pxlo, pca, 
		           pptlHTOrg, prclDest, prclSrc, pptlMask, iMode);
    else
        return FALSE;
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
    KDevice * pdevice = (KDevice *) pso->dhpdev;

    if ( pdevice->CallEngine(INDEX_DrvTextOut, &pso, 10) )
		return EngTextOut(pso, pstro, pfo, pco, prclExtra, 
		           prclOpaque, pboFore, pboOpaque, pptlOrg, mix);
    else
        return FALSE;
}
