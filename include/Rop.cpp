//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : rop.cpp   						                                     //
//  Description: Bitmap raster operation, MaskBlt, PlgBlt, TransparentBlt            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <assert.h>

#include "DIB.h"
#include "Affine.h"
#include "BitmapInfo.h"
#include "DIBsection.h"
#include "GDIObject.h"
#include "Rop.h"

typedef struct
  { unsigned long  ropcode;
    char          *formula;
    char          *name;
  } RopItem;
    
   
const RopItem RopDict[256] =
{
    { 0x00000042, "0",          "BLACKNESS"     },
    { 0x00010289, "DPSoon",     NULL            },
    { 0x00020C89, "DPSona",     NULL            },
    { 0x000300AA, "PSon",       NULL            },  
    { 0x00040C88, "SDPona",     NULL            },
    { 0x000500A9, "DPon",       NULL            },
    { 0x00060865, "PDSxnon",    NULL            },
    { 0x000702C5, "PDSaon",     NULL            },
    { 0x00080F08, "SDPnaa",     NULL            },
    { 0x00090245, "PDSxon",     NULL            },
    { 0x000A0329, "DPna",       NULL            },
    { 0x000B0B2A, "PSDnaon",    NULL            },
    { 0x000C0324, "SPna",       NULL            },
    { 0x000D0B25, "PDSnaon",    NULL            },
    { 0x000E08A5, "PDSonon",    NULL            },
    { 0x000F0001, "Pn",         NULL            },
    
    { 0x00100C85, "PDSona",     NULL            },
    { 0x001100A6, "DSon",       "NOTSRCERASE"   },
    { 0x00120868, "SDPxnon",    NULL            },
    { 0x001302C8, "SDPaon",     NULL            },
    { 0x00140869, "DPSxnon",    NULL            },
    { 0x001502C9, "DPSaon",     NULL            },
    { 0x00165CCA, "PSDPSanaxx", NULL            },
    { 0x00171D54, "SSPxDSxaxn", NULL            },
    { 0x00180D59, "SPxPDxa",    NULL            },
    { 0x00191CC8, "SDPSanaxn",  NULL            },
    { 0x001A06C5, "PDSPaox",    NULL            },
    { 0x001B0768, "SDPSxaxn",   NULL            },
    { 0x001C06CA, "PSDPaox",    NULL            },
    { 0x001D0766, "DSPDxaxn",   NULL            },
    { 0x001E01A5, "PDSox",      NULL            },
    { 0x001F0385, "PDSoan",     NULL            },
    
    { 0x00200F09, "DPSnaa",     NULL            },
    { 0x00210248, "SDPxon",     NULL            },
    { 0x00220326, "DSna",       NULL            },
    { 0x00230B24, "SPDnaon",    NULL            },
    { 0x00240D55, "SPxDSxa",    NULL            },
    { 0x00251CC5, "PDSPanaxn",  NULL            },
    { 0x002606C8, "SDPSaox",    NULL            },
    { 0x00271868, "SDPSxnox",   NULL            },
    { 0x00280369, "DPSxa",      NULL            },
    { 0x002916CA, "PSDPSaoxxn", NULL            },
    { 0x002A0CC9, "DPSana",     NULL            },
    { 0x002B1D58, "SSPxPDxaxn", NULL            },
    { 0x002C0784, "SPDSoax",    NULL            },
    { 0x002D060A, "PSDnox",     NULL            },
    { 0x002E064A, "PSDPxox",    NULL            },
    { 0x002F0E2A, "PSDnoan",    NULL            },

    
    { 0x0030032A, "PSna",       NULL            },
    { 0x00310B28, "SDPnaon",    NULL            },
    { 0x00320688, "SDPSoox",    NULL            },
    { 0x00330008, "Sn",         "NOTSRCCOPY"    },
    { 0x003406C4, "SPDSaox",    NULL            },
    { 0x00351864, "SPDSxnox",   NULL            },
    { 0x003601A8, "SDPox",      NULL            },
    { 0x00370388, "SDPoan",     NULL            },
    { 0x0038078A, "PSDPoax",    NULL            },
    { 0x00390604, "SPDnox",     NULL            },
    { 0x003A0644, "SPDSxox",    NULL            },
    { 0x003B0E24, "SPDnoan",    NULL            },
    { 0x003C004A, "PSx",        NULL            },
    { 0x003D18A4, "SPDSonox",   NULL            },
    { 0x003E1B24, "SPDSnaox",   NULL            },
    { 0x003F00EA, "PSan",       NULL            },
    
    { 0x00400F0A, "PSDnaa",     NULL            },
    { 0x00410249, "DPSxon",     NULL            },
    { 0x00420D5D, "SDxPDxa",    NULL            },
    { 0x00431CC4, "SPDSanaxn",  NULL            },
    { 0x00440328, "SDna",       "SRCERASE"      },
    { 0x00450B29, "DPSnaon",    NULL            },
    { 0x004606C6, "DSPDaox",    NULL            },
    { 0x0047076A, "PSDPxaxn",   NULL            },
    { 0x00480368, "SDPxa",      NULL            },
    { 0x004916C5, "PDSPDaoxxn", NULL            },
    { 0x004A0789, "DPSDoax",    NULL            },
    { 0x004B0605, "PDSnox",     NULL            },
    { 0x004C0CC8, "SDPana",     NULL            },
    { 0x004D1954, "SSPxDSxoxn", NULL            },
    { 0x004E0645, "PDSPxox",    NULL            },
    { 0x004F0E25, "PDSnoan",    NULL            },
    
    { 0x00500325, "PDna",       NULL            },
    { 0x00510B26, "DSPnaon",    NULL            },
    { 0x005206C9, "DPSDaox",    NULL            },
    { 0x00530764, "SPDSxaxn",   NULL            },
    { 0x005408A9, "DPSonon",    NULL            },
    { 0x00550009, "Dn",         "DSTINVERT"     }, 
    { 0x005601A9, "DPSox",      NULL            },
    { 0x00570389, "DPSoan",     NULL            },
    { 0x00580785, "PDSPoax",    NULL            },
    { 0x00590609, "DPSnox",     NULL            },
    { 0x005A0049, "DPx",        "PATINVERT"     },
    { 0x005B18A9, "DPSDonox",   NULL            },
    { 0x005C0649, "DPSDxox",    NULL            },
    { 0x005D0E29, "DPSnoan",    NULL            },
    { 0x005E1B29, "DPSDnaox",   NULL            },
    { 0x005F00E9, "DPan",       NULL            },
    
    { 0x00600365, "PDSxa",      NULL            },
    { 0x006116C6, "DSPDSaoxxn", NULL            },
    { 0x00620786, "DSPDoax",    NULL            },
    { 0x00630608, "SDPnox",     NULL            },
    { 0x00640788, "SDPSoax",    NULL            },
    { 0x00650606, "DSPnox",     NULL            },
    { 0x00660046, "DSx",        "SRCINVERT"     },
    { 0x006718A8, "SDPSonox",   NULL            },
    { 0x006858A6, "DSPDSonoxxn",NULL            },
    { 0x00690145, "PDSxxn",     NULL            },
    { 0x006A01E9, "DPSax",      NULL            },
    { 0x006B178A, "PSDPSoaxxn", NULL            },
    { 0x006C01E8, "SDPax",      NULL            },
    { 0x006D1785, "PDSPDoaxxn", NULL            },
    { 0x006E1E28, "SDPSnoax",   NULL            },
    { 0x006F0C65, "PDSxnan",    NULL            },
    
    { 0x00700CC5, "PDSana",     NULL            },
    { 0x00711D5C, "SSDxPDxaxn", NULL            },
    { 0x00720648, "SDPSxox",    NULL            },
    { 0x00730E28, "SDPnoan",    NULL            },
    { 0x00740646, "DSPDxox",    NULL            },
    { 0x00750E26, "DSPnoan",    NULL            },
    { 0x00761B28, "SDPSnaox",   NULL            },
    { 0x007700E6, "DSan",       NULL            },
    { 0x007801E5, "PDSax",      NULL            },
    { 0x00791786, "DSPDSoaxxn", NULL            },
    { 0x007A1E29, "DPSDnoax",   NULL            },
    { 0x007B0C68, "SDPxnan",    NULL            },
    { 0x007C1E24, "SPDSnoax",   NULL            },
    { 0x007D0C69, "DPSxnan",    NULL            },
    { 0x007E0955, "SPxDSxo",    NULL            },
    { 0x007F03C9, "DPSaan",     NULL            },
    
    { 0x008003E9, "DPSaa",      NULL            },
    { 0x00810975, "SPxDSxon",   NULL            },
    { 0x00820C49, "DPSxna",     NULL            },
    { 0x00831E04, "SPDSnoaxn",  NULL            },
    { 0x00840C48, "SDPxna",     NULL            },
    { 0x00851E05, "PDSPnoaxn",  NULL            },
    { 0x008617A6, "DSPDSoaxx",  NULL            },
    { 0x008701C5, "PDSaxn",     NULL            },
    { 0x008800C6, "DSa",        "SRCAND"        },
    { 0x00891B08, "SDPSnaoxn",  NULL            },
    { 0x008A0E06, "DSPnoa",     NULL            },
    { 0x008B0666, "DSPDxoxn",   NULL            },
    { 0x008C0E08, "SDPnoa",     NULL            },
    { 0x008D0668, "SDPSxoxn",   NULL            },
    { 0x008E1D7C, "SSDxPDxax",  NULL            },
    { 0x008F0CE5, "PDSanan",    NULL            },
    
    { 0x00900C45, "PDSxna",     NULL            },
    { 0x00911E08, "SDPSnoaxn",  NULL            },
    { 0x009217A9, "DPSDPoaxx",  NULL            },
    { 0x009301C4, "SPDaxn",     NULL            },
    { 0x009417AA, "PSDPSoaxx",  NULL            },
    { 0x009501C9, "DPSaxn",     NULL            },
    { 0x00960169, "DPSxx",      NULL            },
    { 0x0097588A, "PSDPSonoxx", NULL            },
    { 0x00981888, "SDPSonoxn",  NULL            },
    { 0x00990066, "DSxn",       NULL            },
    { 0x009A0709, "DPSnax",     NULL            },
    { 0x009B07A8, "SDPSoaxn",   NULL            },
    { 0x009C0704, "SPDnax",     NULL            },
    { 0x009D07A6, "DSPDoaxn",   NULL            },
    { 0x009E16E6, "DSPDSaoxx",  NULL            },
    { 0x009F0345, "PDSxan",     NULL            },
    { 0x00A000C9, "DPa",        NULL            },
    { 0x00A11B05, "PDSPnaoxn",  NULL            },
    { 0x00A20E09, "DPSnoa",     NULL            },
    { 0x00A30669, "DPSDxoxn",   NULL            },
    { 0x00A41885, "PDSPonoxn",  NULL            },
    { 0x00A50065, "PDxn",       NULL            },
    { 0x00A60706, "DSPnax",     NULL            },
    { 0x00A707A5, "PDSPoaxn",   NULL            },
    { 0x00A803A9, "DPSoa",      NULL            },
    { 0x00A90189, "DPSoxn",     NULL            },
    { 0x00AA0029, "D",          NULL            },
    { 0x00AB0889, "DPSono",     NULL            },
    { 0x00AC0744, "SPDSxax",    NULL            },
    { 0x00AD06E9, "DPSDaoxn",   NULL            },
    { 0x00AE0B06, "DSPnao",     NULL            },
    { 0x00AF0229, "DPno",       NULL            },
    
    { 0x00B00E05, "PDSnoa",     NULL            },
    { 0x00B10665, "PDSPxoxn",   NULL            },
    { 0x00B21974, "SSPxDSxox",  NULL            },
    { 0x00B30CE8, "SDPanan",    NULL            },
    { 0x00B4070A, "PSDnax",     NULL            },
    { 0x00B507A9, "DPSDoaxn",   NULL            },
    { 0x00B616E9, "DPSDPaoxx",  NULL            },
    { 0x00B70348, "SDPxan",     NULL            },
    { 0x00B8074A, "PSDPxax",    NULL            },
    { 0x00B906E6, "DSPDaoxn",   NULL            },
    { 0x00BA0B09, "DPSnao",     NULL            },
    { 0x00BB0226, "DSno",       "MERGEPAINT"    },
    { 0x00BC1CE4, "SPDSanax",   NULL            },
    { 0x00BD0D7D, "SDxPDxan",   NULL            },
    { 0x00BE0269, "DPSxo",      NULL            },
    { 0x00BF08C9, "DPSano",     NULL            },
    
    { 0x00C000CA, "PSa",        "MERGECOPY"     },
    { 0x00C11B04, "SPDSnaoxn",  NULL            },
    { 0x00C21884, "SPDSonoxn",  NULL            },
    { 0x00C3006A, "PSxn",       NULL            },
    { 0x00C40E04, "SPDnoa",     NULL            },
    { 0x00C50664, "SPDSxoxn",   NULL            },
    { 0x00C60708, "SDPnax",     NULL            },
    { 0x00C707AA, "PSDPoaxn",   NULL            },
    { 0x00C803A8, "SDPoa",      NULL            },
    { 0x00C90184, "SPDoxn",     NULL            },
    { 0x00CA0749, "DPSDxax",    NULL            },
    { 0x00CB06E4, "SPDSaoxn",   NULL            },
    { 0x00CC0020, "S",          "SRCCOPY"       },
    { 0x00CD0888, "SDPono",     NULL            },
    { 0x00CE0B08, "SDPnao",     NULL            },
    { 0x00CF0224, "SPno",       NULL            },
    
    { 0x00D00E0A, "PSDnoa",     NULL            },
    { 0x00D1066A, "PSDPxoxn",   NULL            },
    { 0x00D20705, "PDSnax",     NULL            },
    { 0x00D307A4, "SPDSoaxn",   NULL            },
    { 0x00D41D78, "SSPxPDxax",  NULL            },
    { 0x00D50CE9, "DPSanan",    NULL            },
    { 0x00D616EA, "PSDPSaoxx",  NULL            },
    { 0x00D70349, "DPSxan",     NULL            },
    { 0x00D80745, "PDSPxax",    NULL            },
    { 0x00D906E8, "SDPSaoxn",   NULL            },
    { 0x00DA1CE9, "DPSDanax",   NULL            },
    { 0x00DB0D75, "SPxDSxan",   NULL            },
    { 0x00DC0B04, "SPDnao",     NULL            },
    { 0x00DD0228, "SDno",       NULL            },
    { 0x00DE0268, "SDPxo",      NULL            },
    { 0x00DF08C8, "SDPano",     NULL            },
    
    { 0x00E003A5, "PDSoa",      NULL            },
    { 0x00E10185, "PDSoxn",     NULL            },
    { 0x00E20746, "DSPDxax",    NULL            },
    { 0x00E306EA, "PSDPaoxn",   NULL            },
    { 0x00E40748, "SDPSxax",    NULL            },
    { 0x00E506E5, "PDSPaoxn",   NULL            },
    { 0x00E61CE8, "SDPSanax",   NULL            },
    { 0x00E70D79, "SPxPDxan",   NULL            },
    { 0x00E81D74, "SSPxDSxax",  NULL            },
    { 0x00E95CE6, "DSPDSanaxxn",NULL            },
    { 0x00EA02E9, "DPSao",      NULL            },
    { 0x00EB0849, "DPSxno",     NULL            },
    { 0x00EC02E8, "SDPao",      NULL            },
    { 0x00ED0848, "SDPxno",     NULL            },
    { 0x00EE0086, "DSo",        "SRCPAINT"      },
    { 0x00EF0A08, "SDPnoo",     NULL            },
    
    { 0x00F00021, "P",          "PATCOPY"       },
    { 0x00F10885, "PDSono",     NULL            },
    { 0x00F20B05, "PDSnao",     NULL            },
    { 0x00F3022A, "PSno",       NULL            },
    { 0x00F40B0A, "PSDnao",     NULL            },
    { 0x00F50225, "PDno",       NULL            },
    { 0x00F60265, "PDSxo",      NULL            },
    { 0x00F708C5, "PDSano",     NULL            },
    { 0x00F802E5, "PDSao",      NULL            },
    { 0x00F90845, "PDSxno",     NULL            },
    { 0x00FA0089, "DPo",        NULL            },
    { 0x00FB0A09, "DPSnoo",     "PATPAINT"      },
    { 0x00FC008A, "PSo",        NULL            },
    { 0x00FD0A0A, "PSDnoo",     NULL            },
    { 0x00FE02A9, "DPSoo",      NULL            },
    { 0x00FF0062, "1",          "WHITENESS"     }
};

DWORD GetRopCode(BYTE ropindex)
{
	return RopDict[ropindex].ropcode;
}


void ChannelSplit(KDIB & dib, HDC hDC)
{
	KBitmapInfo bmi;
	KDIBSection dibsec;
	int			width, height;

	bmi.SetFormat(dib.GetWidth(), dib.GetHeight(), 8, BI_RGB);
	bmi.SetGrayScale(RGB(0xFF, 0, 0));

	dibsec.CreateDIBSection(NULL, bmi.GetBMI(), DIB_RGB_COLORS, NULL, NULL);
	
	dibsec.Prepare(width, height);
	dibsec.SetColorTable();

	KGDIObject red(hDC, CreateSolidBrush(RGB(0xFF, 0, 0)));
	dib.DrawDIB(dibsec.m_hMemDC, 0, 0, width, height, 0, 0, width, height, MERGECOPY);

	RGBQUAD gray[256];
	for (int i=0; i<256; i++)
	{
		gray[i].rgbRed		= i;
		gray[i].rgbGreen	= i;
		gray[i].rgbBlue		= i;
		gray[i].rgbReserved = 0;
	}		
	SetDIBColorTable(dibsec.m_hMemDC, 0, 256, gray);

	BitBlt(hDC, 0, 0, width, height, dibsec.m_hMemDC, 0, 0, SRCCOPY);
}


// Create a grayscale image (DIBSection) from one of the RGB channels in a DIB
// Mask should be RGB(255, 0, 0), RGB(0, 255, 0), or RGB(0, 0, 255)
HBITMAP ChannelSplit(const BITMAPINFO * pBMI, const void * pBits, COLORREF Mask, HDC hMemDC)
{
	typedef struct { BITMAPINFOHEADER bmiHeader;
				     RGBQUAD		  bmiColor[256];
	} BMI8BPP;

	int width  = pBMI->bmiHeader.biWidth;
	int height = pBMI->bmiHeader.biHeight;

	BMI8BPP bmi8bpp;

	memset(&bmi8bpp, 0, sizeof(bmi8bpp));

	bmi8bpp.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmi8bpp.bmiHeader.biWidth       = width;
	bmi8bpp.bmiHeader.biHeight      = height;
	bmi8bpp.bmiHeader.biPlanes      = 1;
	bmi8bpp.bmiHeader.biBitCount    = 8;
	bmi8bpp.bmiHeader.biCompression = BI_RGB;

	for (int i=0; i<256; i++) // color table for one of the RGB channels
	{
		bmi8bpp.bmiColor[i].rgbRed	 = i & GetRValue(Mask);
		bmi8bpp.bmiColor[i].rgbGreen = i & GetGValue(Mask);
		bmi8bpp.bmiColor[i].rgbBlue	 = i & GetBValue(Mask);
	}		

	HBITMAP hRslt = CreateDIBSection(NULL, (BITMAPINFO *) & bmi8bpp, NULL, DIB_RGB_COLORS, NULL, NULL);
	
	if ( hRslt==NULL )
		return NULL;

	SelectObject(hMemDC, hRslt);
	
	HBRUSH hBrush = CreateSolidBrush(Mask);			// solid red, green, or blue
	HGDIOBJ hOld  = SelectObject(hMemDC, hBrush);
	
	StretchDIBits(hMemDC, 0, 0, width, height, 0, 0, width, height, pBits, pBMI, DIB_RGB_COLORS, MERGECOPY);

	for (i=0; i<256; i++)	// convert to real grayscale color table
	{
		bmi8bpp.bmiColor[i].rgbRed	 = i;
		bmi8bpp.bmiColor[i].rgbGreen = i;
		bmi8bpp.bmiColor[i].rgbBlue	 = i;
	}
	SetDIBColorTable(hMemDC, 0, 256, bmi8bpp.bmiColor);

	SelectObject(hMemDC, hOld);
	DeleteObject(hBrush);

	return hRslt;
}

// Gradually display a DIB on a destination surface in 4 steps
void FadeIn(HDC hDC, int x, int y, int w, int h, const BITMAPINFO * pBMI, const void * pBits)
{
	const WORD Mask11[8] = { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	const WORD Mask22[8] = { 0x88, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00 };
	const WORD Mask44[8] = { 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00, 0xAA, 0x00 };
	const WORD Mask88[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	
	const WORD * Mask[4] = { Mask11, Mask22, Mask44, Mask88 };

	for (int i=0; i<4; i++)
	{
		HBITMAP hMask = CreateBitmap(8, 8, 1, 1, Mask[i]);
		HBRUSH  hBrush= CreatePatternBrush(hMask);
		DeleteObject(hMask);

		HGDIOBJ hOld  = SelectObject(hDC, hBrush);
		// D^(P&(S^D)), if P then S else D
		StretchDIBits(hDC, x, y, w, h, 0, 0, w, h, pBits, pBMI, DIB_RGB_COLORS, 0xCA07A9);
				
		SelectObject(hDC, hOld);
		DeleteObject(hBrush);
	}
}

// dx, dy, dw, dh defines a destination rectangle
// sw, sh is the dimension of source rectangle
// sx, sy is the starting point winthin the source bitmap, which will be tiled to sw x sh in size
BOOL StretchTile(HDC      hDC, int dx, int dy, int dw, int dh, 
				 HBITMAP hSrc, int sx, int sy, int sw, int sh, 
				 DWORD rop)
{
	BITMAP bmp;

	if ( ! GetObject(hSrc, sizeof(BITMAP), & bmp) )
		return FALSE;
	
	HDC   hMemDC = CreateCompatibleDC(NULL);
	HGDIOBJ hOld = SelectObject(hMemDC, hSrc);
	
	int sy0 = sy % bmp.bmHeight;						// current tile y origin
	
	for (int y=0; y<sh; y+=(bmp.bmHeight - sy0))
	{
		int height = min(bmp.bmHeight - sy0, sh - y);	// current tile height
		int sx0    = sx % bmp.bmWidth;					// current tile x origin
		
		for (int x=0; x<sw; x+=(bmp.bmWidth  - sx0))
		{
			int width = min(bmp.bmWidth - sx0, sw - x); // current tile width
		
			StretchBlt(hDC,    dx+x*dw/sw, dy+y*dh/sh, dw*width/sw, dh*height/sh, 
					   hMemDC, sx0,        sy0,        width,       height, 
					   rop);
			sx0 = 0;	// after the first tile in a row, change to full tile width
		}
		sy0 = 0; // after the the first row, change to full tile height
	}

	SelectObject(hMemDC, hOld);
	DeleteObject(hMemDC);
		
	return TRUE;
}


BOOL G_PlgBlt(HDC hdcDest, const POINT * pPoint, 
			HDC hdcSrc, int nXSrc, int nYSrc, int nWidth, int nHeight, 
			HBITMAP hbmMask, int xMask, int yMask)
{
	KReverseAffine map(pPoint);

	if ( map.Simple() ) // no shear and rotation
	{
		int x = pPoint[0].x;
		int y = pPoint[0].y;
		int w = pPoint[1].x-pPoint[0].x;
		int h = pPoint[2].y-pPoint[0].y;

		if ( hbmMask ) // has a mask bitmap, if (M) the S else D, S ^ (~M & (S^D))
		{
		 	StretchBlt(hdcDest, x, y, w, h, hdcSrc, nXSrc, nYSrc, nWidth, nHeight, SRCINVERT);
		  StretchTile(hdcDest, x, y, w, h,  hbmMask, xMask, yMask, nWidth, nHeight, 0x220326);
			return StretchBlt(hdcDest, x, y, w, h, hdcSrc, nXSrc, nYSrc, nWidth, nHeight, SRCINVERT);
		}
		else
			return StretchBlt(hdcDest, x, y, w, h, hdcSrc, nXSrc, nYSrc, nWidth, nHeight, SRCCOPY);
	}

	map.Setup(nXSrc, nYSrc, nWidth, nHeight);

	HDC hdcMask   = NULL;
	int maskwidth = 0;
	int maskheight= 0;

	if ( hbmMask )
	{
		BITMAP bmp;
		GetObject(hbmMask, sizeof(bmp), & bmp);
	
		maskwidth  = bmp.bmWidth;
		maskheight = bmp.bmHeight;

		hdcMask = CreateCompatibleDC(NULL);
		SelectObject(hdcMask, hbmMask);
	}

	for (int dy=map.miny; dy<=map.maxy; dy++)
	for (int dx=map.minx; dx<=map.maxx; dx++)
	{
		float sx, sy;
		map.Map(dx, dy, sx, sy);

		if ( (sx>=nXSrc) && (sx<=(nXSrc+nWidth))  )
		if ( (sy>=nYSrc) && (sy<=(nYSrc+nHeight)) )
			if ( hbmMask )
			{
				if ( GetPixel(hdcMask, ((int)sx+xMask) % maskwidth, ((int)sy+yMask) % maskheight) )
					SetPixel(hdcDest, dx, dy, GetPixel(hdcSrc, (int)sx, (int)sy));
			}
			else
				SetPixel(hdcDest, dx, dy, GetPixel(hdcSrc, (int)sx, (int)sy));
	}

	if ( hdcMask )
		DeleteObject(hdcMask);

	return TRUE;
}


BOOL AffineBlt(HDC hdcDest, const POINT * pPoint, 
			   HDC hdcSrc, int nXSrc, int nYSrc, int nWidth, int nHeight, DWORD rop)
{
	KReverseAffine map(pPoint);

	if ( map.Simple() ) // no shear and rotation
		return StretchBlt(hdcDest, pPoint[0].x, pPoint[0].y, 
				   pPoint[1].x-pPoint[0].x, pPoint[2].y-pPoint[0].y, hdcSrc, nXSrc, nYSrc, nWidth, nHeight, rop);
	else
	{
		map.Setup(nXSrc, nYSrc, nWidth, nHeight);

		for (int dy=map.miny; dy<=map.maxy; dy++)
		for (int dx=map.minx; dx<=map.maxx; dx++)
		{
			float sx, sy;
			map.Map(dx, dy, sx, sy);

			if ( (sx>=nXSrc) && (sx<=(nXSrc+nWidth))  )
			if ( (sy>=nYSrc) && (sy<=(nYSrc+nHeight)) )
			{
				COLORREF s = GetPixel(hdcSrc, (int)sx, (int)sy);

				switch ( rop )
				{
					case SRCCOPY: 
						SetPixel(hdcDest, dx, dy, s); break;

					case SRCAND:
						SetPixel(hdcDest, dx, dy, s & GetPixel(hdcDest, dx, dy) ); break;

					case SRCINVERT:
						SetPixel(hdcDest, dx, dy, s ^ GetPixel(hdcDest, dx, dy) ); break;
					
					default:
						return FALSE;
				}
			}
		}

		return TRUE;
	}
}


BOOL AffineTile(HDC hdcDest, const POINT * pPoint, 
			    HBITMAP hSrc, int xMask, int yMask, int nWidth, int nHeight, DWORD rop)
{
	KReverseAffine map(pPoint);

	if ( map.Simple() ) // no shear and rotation
		return StretchTile(hdcDest, pPoint[0].x, pPoint[0].y, 
				   pPoint[1].x-pPoint[0].x, pPoint[2].y-pPoint[0].y, hSrc, xMask, yMask, nWidth, nHeight, rop);
	else
	{
		map.Setup(0, 0, nWidth, nHeight);

		BITMAP bmp;
		GetObject(hSrc, sizeof(bmp), & bmp);
		
		HDC hdcSrc = CreateCompatibleDC(NULL);
		SelectObject(hdcSrc, hSrc);

		for (int dy=map.miny; dy<=map.maxy; dy++)
		for (int dx=map.minx; dx<=map.maxx; dx++)
		{
			float sx, sy;
			map.Map(dx, dy, sx, sy);

			if ( (sx>=0) && (sx<=nWidth)  )
			if ( (sy>=0) && (sy<=nHeight) )
			{
				COLORREF s = GetPixel(hdcSrc, ((int)sx+xMask) % bmp.bmWidth, 
											  ((int)sy+yMask) % bmp.bmHeight);

				switch ( rop )
				{
					case SRCAND:
						SetPixel(hdcDest, dx, dy, s & GetPixel(hdcDest, dx, dy) ); break;

					case 0x220326: // ~S & D
						if ( s==RGB(0xFF, 0xFF, 0xFF) )
							SetPixel(hdcDest, dx, dy, RGB(0, 0, 0));
						else if ( s!=0 )
							SetPixel(hdcDest, dx, dy, (~s & GetPixel(hdcDest, dx, dy)) & 0xFFFFFF ); 
						break;

					default:
						DeleteObject(hdcSrc);
						return FALSE;
				}
			}
		}
		
		DeleteObject(hdcSrc);
		return TRUE;
	}
}

BOOL C_PlgBlt(HDC hdcDest, const POINT * pPoint, 
			HDC hdcSrc, int nXSrc, int nYSrc, int nWidth, int nHeight, 
			HBITMAP hbmMask, int xMask, int yMask)
{
	if ( hbmMask )	// has a mask bitmap, if (M) the S else D, S ^ (~M & (S^D))
	{
		        AffineBlt(hdcDest, pPoint, hdcSrc,  nXSrc, nYSrc, nWidth, nHeight, SRCINVERT);
		       AffineTile(hdcDest, pPoint, hbmMask, xMask, yMask, nWidth, nHeight, 0x220326);
		return  AffineBlt(hdcDest, pPoint, hdcSrc,  nXSrc, nYSrc, nWidth, nHeight, SRCINVERT);
	}
	else
		return AffineBlt(hdcDest, pPoint, hdcSrc, nXSrc, nYSrc, nWidth, nHeight, SRCCOPY);
}


void DrawCube(HDC hDC, int x, int y, int dh, int dx, int dy, HDC hMemDC, int w, int h, HBITMAP hMask, bool bSimulate)
{
	SetStretchBltMode(hDC, HALFTONE);

	//         6
	//    0         4
	//         1
	//    2         5
	//         3 
	POINT P[3] = { { x - dx, y - dy }, { x, y }, { x - dx, y - dy + dh } }; // 012
	POINT Q[3] = { { x, y }, { x + dx, y - dy }, { x, y + dh } };			// 143
	POINT R[3] = { { x - dx, y - dy }, { x, y - dy - dy }, { x, y } };		// 061

	if ( bSimulate )
	{
		G_PlgBlt(hDC, P, hMemDC, 0, 0, w, h, hMask, 0, 0);
		G_PlgBlt(hDC, Q, hMemDC, 0, 0, w, h, hMask, 0, 0);
		G_PlgBlt(hDC, R, hMemDC, 0, 0, w, h, hMask, 0, 0);
	}
	else
	{
		PlgBlt(hDC, P, hMemDC, 0, 0, w, h, hMask, 0, 0);
		PlgBlt(hDC, Q, hMemDC, 0, 0, w, h, hMask, 0, 0);
		PlgBlt(hDC, R, hMemDC, 0, 0, w, h, hMask, 0, 0);
	}
}

void MaskCube(HDC hDC, int size, int x, int y, int w, int h, HBITMAP hBmp, HDC hMemDC, bool mask, bool bSimulate)
{
	HBITMAP hMask = NULL;
	
	if ( mask )
	{
		hMask = CreateBitmap(w, h, 1, 1, NULL);
		SelectObject(hMemDC, hMask);
		PatBlt(hMemDC, 0, 0, w, h, BLACKNESS);
		RoundRect(hMemDC, 0, 0, w, h, w/2, h/2); // white
	}

	int dx  = size * 94 / 100; // cos(20)
	int dy  = size * 34 / 100; // sin(20)
	SelectObject(hMemDC, hBmp);
	DrawCube(hDC, x+dx, y+size, size, dx, dy, hMemDC, w, h, hMask, bSimulate);
	
	if ( hMask )
		DeleteObject(hMask);
}


BOOL TriBitBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, 
			   HDC hdcSrc, int nXSrc, int nYSrc,
			   HBITMAP hbmMask, int xMask, int yMask, 
			   DWORD rop1, DWORD rop2, DWORD rop3)
{
	HDC hMemDC = CreateCompatibleDC(hdcDest);
	SelectObject(hMemDC, hbmMask);

	if ( (rop1>>16)!=0xAA ) // not D
		BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, rop1);

	BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hMemDC, xMask, yMask, rop2);

	DeleteObject(hMemDC);

	if ( (rop3>>16)!=0xAA ) // not D
		return BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, rop3);
	else
		return TRUE;
}


BOOL G_MaskBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight,
			 HDC hdcSrc,  int nXSrc,  int nYSrc, 
			 HBITMAP hbmMask, int xMask, int yMask,
			 DWORD dwRop
)
{
	DWORD back = (dwRop >> 24) & 0xFF;
	DWORD fore = (dwRop >> 16) & 0xFF;

	if ( back==fore ) // foreground=background, hbmMask not needed
		return BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, dwRop & 0xFFFFFF);

	// if (M) D=fore(P,S,D) else D=back(P,S,D)
	
	if ( D_independent(back) )	// back independent of D
		return TriBitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, hbmMask, xMask, yMask,
			fore^back << 16,	// ( fore^back, fore^back )
			SRCAND,				// ( fore^back, 0 )
			(back^0xAA) << 16); // { fore, back }

	if ( D_independent(fore) )  // fore independent of D
		return TriBitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, hbmMask, xMask, yMask,
			(fore^back) << 16,  // ( fore^back, fore^back )
			0x22 << 16,			// ( 0, fore^back )
			(fore^0xAA) << 16); // { fore, back }

	// both foreground and background depend on D
	if ( S_independent(back) && S_independent(fore) )
		return TriBitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, NULL, 0, 0, hbmMask, xMask, yMask,
			0xAA << 16,			// ( D, D )
			( (fore & 0xCC) || (back & 0x33) ) << 16,
			0xAA << 16);
	
	// both foreground and background depend on D
	// either foregounr or background depends on S
	HBITMAP hTemp  = CreateCompatibleBitmap(hdcDest, nWidth, nHeight);
	HDC     hMemDC = CreateCompatibleDC(hdcDest);
	SelectObject(hMemDC, hTemp);
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hdcDest, nXDest, nYDest, SRCCOPY);
	SelectObject(hMemDC, GetCurrentObject(hdcDest, OBJ_BRUSH));
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hdcSrc,  nXSrc, nYSrc, back << 16); // hMemDC contains final background image

	BitBlt(hdcDest, 0, 0, nWidth, nHeight, hdcSrc, nXSrc, nYSrc, fore << 16);  // foreground image

	TriBitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, hMemDC, 0, 0, hbmMask, xMask, yMask,
			SRCINVERT,	// ( fore^back, fore^back )
			SRCAND,		// ( fore^back, 0 )
			SRCINVERT); // { fore, back }

	DeleteObject(hMemDC);
	DeleteObject(hTemp);
	
	return TRUE;
}

// Use TransparentBlt to draw Icon
/*
void TransparentBltDrawIcon(HDC hDC, int x, int y, HICON hIcon)
{
	ICONINFO iconinfo;
	GetIconInfo(hIcon, & iconinfo);

	BITMAP bmp;
	GetObject(iconinfo.hbmMask, sizeof(bmp), & bmp);

	HDC hMemDC = CreateCompatibleDC(NULL);
	HGDIOBJ hOld = SelectObject(hMemDC, iconinfo.hbmColor);

	COLORREF crTrans = GetPixel(hMemDC, 0, 0);

	TransparentBlt(hDC,    x, y, bmp.bmWidth, bmp.bmHeight, 
		           hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight,
		           crTrans);

	SelectObject(hMemDC, hOld);
	DeleteObject(iconinfo.hbmMask);
	DeleteObject(iconinfo.hbmColor);
	DeleteObject(hMemDC);
}
*/

// big pattern brush
void MaskBitmapNT(HDC hDC, int x, int y, int width, int height, HBITMAP hMask, HDC hMemDC)
{
	HBRUSH hBrush = CreatePatternBrush(hMask);
	HGDIOBJ hOld  = SelectObject(hDC, hBrush);

	POINT org = { x, y };
	LPtoDP(hDC, &org, 1);
	SetBrushOrgEx(hDC, org.x, org.y, NULL);

	BitBlt(hDC, x, y, width, height, hMemDC, 0, 0, 0x6C01E8); // S^(P&D)

	SelectObject(hDC, hOld);
	DeleteObject(hBrush);
}

void MaskBltDrawIcon(HDC hDC, int x, int y, HICON hIcon)
{
	ICONINFO iconinfo;
	GetIconInfo(hIcon, & iconinfo);

	BITMAP bmp;
	GetObject(iconinfo.hbmMask, sizeof(bmp), & bmp);

	HDC hMemDC = CreateCompatibleDC(NULL);
	HGDIOBJ hOld = SelectObject(hMemDC, iconinfo.hbmColor);

	MaskBlt(hDC, x, y, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, 
		iconinfo.hbmMask, 0, 0, MAKEROP4(SRCINVERT, SRCCOPY));

	SelectObject(hMemDC, hOld);
	DeleteObject(iconinfo.hbmMask);
	DeleteObject(iconinfo.hbmColor);
	DeleteObject(hMemDC);
}

