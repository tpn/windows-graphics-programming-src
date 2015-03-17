//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : pogygdi.cpp			                                             //
//  Description: KGDIDecoder class                                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <assert.h>
#include <string.h>
#include <tchar.h>

#include "..\..\include\Lookup.h"
#include "..\..\include\Decoder.h"


class KGDIDecoder : public IDecoder
{
	ATOM atom_COLORREF;

	ATOM atom_HGDIOBJ;
	ATOM atom_HDC;
	ATOM atom_HPEN;
	ATOM atom_HBRUSH;
	ATOM atom_HBITMAP;
	ATOM atom_HPALETTE;
	ATOM atom_HRGN;
	ATOM atom_HFONT;
	ATOM atom_PLOGFONTW;

public:
	KGDIDecoder()
	{
		pNextDecoder = NULL;
	}

	virtual bool Initialize(IAtomTable * pAtomTable);
	virtual int  Decode(ATOM typ, const void * pValue, char * szBuffer, int nBufferSize);
};


bool KGDIDecoder::Initialize(IAtomTable * pAtomTable)
{
	if ( pAtomTable==NULL )
		return false;

	atom_COLORREF  = pAtomTable->AddAtom("COLORREF");
	
	atom_HGDIOBJ   = pAtomTable->AddAtom("HGDIOBJ");
	atom_HDC	   = pAtomTable->AddAtom("HDC");
	atom_HPEN	   = pAtomTable->AddAtom("HPEN");
	atom_HBRUSH	   = pAtomTable->AddAtom("HBRUSH");
	atom_HBITMAP   = pAtomTable->AddAtom("HBITMAP");
	atom_HPALETTE  = pAtomTable->AddAtom("HPALETTE");
	atom_HRGN	   = pAtomTable->AddAtom("HRGN");
	atom_HFONT	   = pAtomTable->AddAtom("HFONT");
	atom_PLOGFONTW = pAtomTable->AddAtom("LOGFONTW*");

	return true;
}


typedef enum
{
	DEF_TYPE		= 0x00,
	DC_TYPE			= 0x01,
	DD_DRAW_TYPE	= 0x02,
	DD_SURF_TYPE	= 0x03,
	RGN_TYPE		= 0x04,
	SURF_TYPE		= 0x05,
	CLIOBJ_TYPE		= 0x06,
	PATH_TYPE		= 0x07,

	PAL_TYPE		= 0x08,
	ICMCS_TYPE		= 0x09,
	LFONT_TYPE		= 0x0A,
	RFONT_TYPE		= 0x0B,
	PFE_TYPE		= 0x0C,
	PFT_TYPE		= 0x0D,
	ICMCXT_TYPE		= 0x0E,
	ICMDLL_TYPE		= 0x0F,

	BRUSH_TYPE		= 0x10,
	D3D_HANDLE_TYPE = 0x11,
	DD_VPORT_TYPE	= 0x12,
	SPACE_TYPE		= 0x13,
	DD_MOTION_TYPE	= 0x14,
	META_TYPE		= 0x15,
	EFSTATE_TYPE	= 0x16,
	BMFD_TYPE		= 0x17,
	
	VTFD_TYPE		= 0x18,
	TTFD_TYPE		= 0x19,
	RC_TYPE			= 0x1A,
	TEMP_TYPE		= 0x1B,
	DRVOBJ_TYPE		= 0x1C,
	DCIOBJ_TYPE		= 0x1D,
	SPOOL_TYPE		= 0x1E,

	EMF_TYPE		= 0x21,
	PEN_TYPE		= 0x30,
	EXTPEN_TYPE		= 0x50
}	GdiObjectType;
	

const DicItem Dic_GdiObjectType[] =
{
	twin(DEF_TYPE),
	item(DC_TYPE,		"HDC"),
	twin(DD_DRAW_TYPE),
	twin(DD_SURF_TYPE),
	item(RGN_TYPE,		"HRGN"),
	item(SURF_TYPE,		"HBITMAP"),
	twin(CLIOBJ_TYPE),
	item(PATH_TYPE,		"HPATH"),

	item(PAL_TYPE,		"HPALETTE"),
	twin(ICMCS_TYPE),
	item(LFONT_TYPE,	"HFONT"),
	twin(RFONT_TYPE),
	twin(PFE_TYPE),
	twin(PFT_TYPE),
	twin(ICMCXT_TYPE),
	twin(ICMDLL_TYPE),

	item(BRUSH_TYPE,	"HBRUSH"),
	twin(D3D_HANDLE_TYPE),
	twin(DD_VPORT_TYPE),
	twin(SPACE_TYPE),
	twin(DD_MOTION_TYPE),
	twin(META_TYPE),
	twin(EFSTATE_TYPE),
	twin(BMFD_TYPE),
	
	twin(VTFD_TYPE),
	twin(TTFD_TYPE),
	twin(RC_TYPE),
	twin(TEMP_TYPE),
	twin(DRVOBJ_TYPE),
	twin(DCIOBJ_TYPE),
	twin(SPOOL_TYPE),

	item(EMF_TYPE,		"HENHMETAFILE"),
	item(PEN_TYPE,		"HPEN"),
	item(EXTPEN_TYPE,	"HEXTPEN"),

	dummyitem
};


const DicItem Dic_COLORREF[] =
{
	item(RGB(0x00, 0x00, 0x00),	"BLACK"),
	item(RGB(0xFF, 0xFF, 0xFF),	"WHITE"),
	item(RGB(0xFF, 0x00, 0x00),	"RED"  ),
	item(RGB(0x00, 0xFF, 0x00),	"GREEN"),
	item(RGB(0x00, 0x00, 0xFF),	"BLUE"),

	dummyitem
};


int KGDIDecoder::Decode(ATOM typ, const void * pValue, char * szBuffer, int nBufferSize)
{
	unsigned data = * (unsigned *) pValue;

	if ( typ==atom_COLORREF )
	{
		if ( ! Lookup( data, Dic_COLORREF, szBuffer) )
			wsprintf(szBuffer, "%06x", data);

		return 4;
	}

	if ( (typ==atom_HDC)    || (typ==atom_HGDIOBJ)  || (typ==atom_HPEN) ||
		 (typ==atom_HBRUSH) || (typ==atom_HPALETTE) || (typ==atom_HRGN) ||
		 (typ==atom_HFONT) )
	{         
		TCHAR temp[32];
		
		unsigned objtyp = (data >> 16) & 0xFF;

		if ( ! Lookup( objtyp & 0x7F, Dic_GdiObjectType, temp) )
			_tcscpy(temp, "HGDIOBJ");
		
		if ( objtyp & 0x80 )  // stock object
			wsprintf(szBuffer, "(S%s)%x", temp, data & 0xFFFF);
		else
			wsprintf(szBuffer, "(%s)%x", temp, data & 0xFFFF);
		
		return 4;
	}

	if ( typ==atom_PLOGFONTW )
	{
		LOGFONTW * pLogFont = (LOGFONTW *) data;

		if ( ! IsBadReadPtr(pLogFont, sizeof(LOGFONTW)) )
		{
			wsprintf(szBuffer, "& LOGFONTW{%d,%d,...,%ws}", 
				pLogFont->lfHeight, pLogFont->lfWidth, pLogFont->lfFaceName);

			return 4;
		}
	}

	// unhandled
	return 0;
}


KGDIDecoder GDIDecoder;


extern "C" __declspec(dllexport) IDecoder * WINAPI Create_GDI_Decoder(void)
{
	return & GDIDecoder;
}