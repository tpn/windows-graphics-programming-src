//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : queryddraw.cpp			                                             //
//  Description: Examine DirectX interfaces and their vtables                        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define NOCRYPT
#define CINTERFACE

#include <windows.h>
#include <ddraw.h>
#include <stdio.h>

// IDirectDraw 
// IDirectDraw2
// IDirectDraw4

// IDirectDrawSurface
// IDirectDrawSurface2
// IDirectDrawSurface3
// IDirectDrawSurface4
// IDirectDrawSurface7

// IDirectDrawPalette
// IDirectDrawClipper
// IDirectDrawColorControl
// IDirectDrawGammaControl


void DumpInterface(const char * name, const IID & iid, const void * vtbl, int size)
{
	static int first = 1;

	if ( first )
	{
		printf("[ddraw]\n");
		first = 0;
	}

	const unsigned * vTable = (const unsigned *) vtbl;

	printf("%08x %08x %d, %s\n", vTable, vTable[0], size/4, name);

	printf("         {%08x-%04x-%04x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x}\n",
		iid.Data1, iid.Data2, iid.Data3, 
		iid.Data4[0], iid.Data4[1], iid.Data4[2], iid.Data4[3],
		iid.Data4[4], iid.Data4[5], iid.Data4[6], iid.Data4[7]);
}


HRESULT QueryDirectDrawVTables(HWND hWnd)
{
	IDirectDraw * lpdd;

	HRESULT hr = DirectDrawCreate(NULL, & lpdd, NULL);

	if ( hr!=DD_OK )
		return hr;

	IDirectDraw_SetCooperativeLevel(lpdd, hWnd, DDSCL_NORMAL);
	IDirectDraw_SetDisplayMode(lpdd, 1152, 864, 24);

	DumpInterface("IID_IDirectDraw", IID_IDirectDraw, lpdd->lpVtbl, sizeof(* lpdd->lpVtbl) );

	{
		IDirectDraw2 * lpdd2;

		IDirectDraw_QueryInterface(lpdd, IID_IDirectDraw2, (void **) & lpdd2);

		DumpInterface("IID_IDirectDraw2", IID_IDirectDraw2, lpdd2->lpVtbl, sizeof(* lpdd2->lpVtbl));

		IDirectDraw2_Release(lpdd2);
	}

	{
		IDirectDraw4 * lpdd4;

		IDirectDraw_QueryInterface(lpdd, IID_IDirectDraw4, (void **) & lpdd4);

		DumpInterface("IID_IDirectDraw4", IID_IDirectDraw4, lpdd4->lpVtbl, sizeof(* lpdd4->lpVtbl));

		IDirectDraw4_Release(lpdd4);
	}

	{
		DDSURFACEDESC ddsd;
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		IDirectDrawSurface * p;

		hr = IDirectDraw_CreateSurface(lpdd, &ddsd, & p, NULL);

		if ( hr==DD_OK )
		{
			DumpInterface("IID_IDirectDrawSurface", IID_IDirectDrawSurface, p->lpVtbl, sizeof(* p->lpVtbl));

			IDirectDrawSurface_Release(p);
		}
	}

	{
		DDSURFACEDESC2 ddsd;
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		IDirectDrawSurface4 * p;

		IDirectDraw4 * lpdd4;

		IDirectDraw_QueryInterface(lpdd, IID_IDirectDraw4, (void **) & lpdd4);

		hr = IDirectDraw4_CreateSurface(lpdd4, &ddsd, & p, NULL);

		if ( hr==DD_OK )
		{
			DumpInterface("IID_IDirectDrawSurface4", IID_IDirectDrawSurface4, p->lpVtbl, sizeof(* p->lpVtbl));

			IDirectDrawSurface4_Release(p);
		}

		IDirectDraw4_Release(lpdd4);
	}


	IDirectDraw_Release(lpdd);

	return DD_OK;
}

#define DEF_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)  const GUID name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } };

DEF_GUID( IID_IDirectDraw,				0x6C14DB80,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60 );
DEF_GUID( IID_IDirectDraw2,             0xB3A6F3E0,0x2B43,0x11CF,0xA2,0xDE,0x00,0xAA,0x00,0xB9,0x33,0x56 );
DEF_GUID( IID_IDirectDraw4,             0x9c59509a,0x39bd,0x11d1,0x8c,0x4a,0x00,0xc0,0x4f,0xd9,0x30,0xc5 );
DEF_GUID( IID_IDirectDrawSurface,		0x6C14DB81,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60 );
DEF_GUID( IID_IDirectDrawSurface2,		0x57805885,0x6eec,0x11cf,0x94,0x41,0xa8,0x23,0x03,0xc1,0x0e,0x27 );
DEF_GUID( IID_IDirectDrawSurface3,      0xDA044E00,0x69B2,0x11D0,0xA1,0xD5,0x00,0xAA,0x00,0xB8,0xDF,0xBB );
DEF_GUID( IID_IDirectDrawSurface4,      0x0B2B8630,0xAD35,0x11D0,0x8E,0xA6,0x00,0x60,0x97,0x97,0xEA,0x5B );
DEF_GUID( IID_IDirectDrawSurface7,      0x06675a80,0x3b9b,0x11d2,0xb9,0x2f,0x00,0x60,0x97,0x97,0xea,0x5b );
DEF_GUID( IID_IDirectDrawPalette,		0x6C14DB84,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60 );
DEF_GUID( IID_IDirectDrawClipper,		0x6C14DB85,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60 );
DEF_GUID( IID_IDirectDrawColorControl, 	0x4B9F0EE0,0x0D7E,0x11D0,0x9B,0x06,0x00,0xA0,0xC9,0x03,0xA3,0xB8 );
DEF_GUID( IID_IDirectDrawGammaControl,  0x69C11C3E,0xB46B,0x11D1,0xAD,0x7A,0x00,0xC0,0x4F,0xC2,0x9B,0x4E );

int main(int argc, char * argv[], char * envp[])
{
	QueryDirectDrawVTables(GetDesktopWindow());
	
	return 0;
}



/*

DEFINE_GUID( IID_IDirectDrawSurface,		0x6C14DB81,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60 );
DEFINE_GUID( IID_IDirectDrawSurface2,		0x57805885,0x6eec,0x11cf,0x94,0x41,0xa8,0x23,0x03,0xc1,0x0e,0x27 );
DEFINE_GUID( IID_IDirectDrawSurface3,           0xDA044E00,0x69B2,0x11D0,0xA1,0xD5,0x00,0xAA,0x00,0xB8,0xDF,0xBB );
DEFINE_GUID( IID_IDirectDrawSurface4,           0x0B2B8630,0xAD35,0x11D0,0x8E,0xA6,0x00,0x60,0x97,0x97,0xEA,0x5B );
DEFINE_GUID( IID_IDirectDrawSurface7,           0x06675a80,0x3b9b,0x11d2,0xb9,0x2f,0x00,0x60,0x97,0x97,0xea,0x5b );
DEFINE_GUID( IID_IDirectDrawPalette,		0x6C14DB84,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60 );
DEFINE_GUID( IID_IDirectDrawClipper,		0x6C14DB85,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60 );
DEFINE_GUID( IID_IDirectDrawColorControl, 	0x4B9F0EE0,0x0D7E,0x11D0,0x9B,0x06,0x00,0xA0,0xC9,0x03,0xA3,0xB8 );
DEFINE_GUID( IID_IDirectDrawGammaControl,       0x69C11C3E,0xB46B,0x11D1,0xAD,0x7A,0x00,0xC0,0x4F,0xC2,0x9B,0x4E );
*/