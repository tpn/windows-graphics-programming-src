//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : format.h				                                             //
//  Description: KBasicDecoder.h                                                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#ifndef INC_FORMAT

#define INC_FORMAT

#include "..\..\include\Decoder.h"

extern ATOM atom_void;
extern ATOM atom_unknown;
extern ATOM atom_returnaddr;

extern ATOM atom_char;
extern ATOM atom_BYTE;

extern ATOM atom_short;
extern ATOM atom_WORD;

extern ATOM atom_COLORREF;

extern ATOM atom_int;
extern ATOM atom_long;
extern ATOM atom_unsigned;
extern ATOM atom_DWORD;
extern ATOM atom_BOOL;

extern ATOM atom_this;
extern ATOM atom_LPVOID;
extern ATOM atom_LPSTR;
extern ATOM atom_LPWSTR;

extern ATOM atom_HGDIOBJ;
extern ATOM atom_HDC;
extern ATOM atom_HPEN;
extern ATOM atom_HBRUSH;
extern ATOM atom_HBITMAP;
extern ATOM atom_HPALETTE;
extern ATOM atom_HRGN;
extern ATOM atom_HFONT;


class KBasicDecoder : public IDecoder
{
	typedef enum { 	MAX_PAGES = 256	};
	
	unsigned	  m_modulebase[MAX_PAGES];	// 1 Kb
	const char *  m_modulename[MAX_PAGES];	// 1 Kb
	unsigned	  m_modulepage[MAX_PAGES];  // 1 Kb
	int			  m_pageno;

	void TranslateAddress(unsigned addr, char * szBuffer);
	
	IAtomTable * m_pAtomTable;
	
public:
	KBasicDecoder()
	{
		pNextDecoder = NULL;
		m_pAtomTable = NULL;
	
		m_pageno = 0;
	}

	virtual bool Initialize(IAtomTable * pAtomTable);
	virtual int  Decode(ATOM typ, const void * pValue, char * szBuffer, int nBufferSize);
};


void MainDecoder(char buffer[], int limit, ATOM typ, unsigned value);

#endif 

