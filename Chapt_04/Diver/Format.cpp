//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : format.cpp				                                             //
//  Description: KBasicDecoder class                                                 //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <assert.h>
#include <string.h>
#include <tchar.h>

#include "..\..\include\Decoder.h"
#include "..\..\include\atom.h"

#include "Format.h"
#include "Function.h"

ATOM atom_void;
ATOM atom_unknown;
ATOM atom_returnaddr;

ATOM atom_char;
ATOM atom_BYTE;

ATOM atom_short;
ATOM atom_WORD;

ATOM atom_COLORREF;

ATOM atom_int;
ATOM atom_long;
ATOM atom_unsigned;
ATOM atom_DWORD;
ATOM atom_UINT;
ATOM atom_BOOL;
ATOM atom_D;

ATOM atom_this;
ATOM atom_LPVOID;
ATOM atom_LPSTR;
ATOM atom_LPWSTR;

ATOM atom_HGDIOBJ;
ATOM atom_HDC;
ATOM atom_HPEN;
ATOM atom_HBRUSH;
ATOM atom_HBITMAP;
ATOM atom_HPALETTE;
ATOM atom_HRGN;
ATOM atom_HFONT;


bool KBasicDecoder::Initialize(IAtomTable * pAtomTable)
{
	if ( pAtomTable==NULL )
		return false;

	m_pAtomTable = pAtomTable;

	atom_void      = pAtomTable->AddAtom("void");
	atom_unknown   = pAtomTable->AddAtom("____");
	atom_returnaddr= pAtomTable->AddAtom("returnaddr");
	
	atom_char      = pAtomTable->AddAtom("char");
	atom_BYTE      = pAtomTable->AddAtom("BYTE");
	
	atom_short     = pAtomTable->AddAtom("short");
	atom_WORD      = pAtomTable->AddAtom("WORD");
	
	atom_COLORREF  = pAtomTable->AddAtom("COLORREF");
	
	atom_int       = pAtomTable->AddAtom("int");
	atom_long      = pAtomTable->AddAtom("long");
	atom_unsigned  = pAtomTable->AddAtom("unsigned");
	atom_DWORD     = pAtomTable->AddAtom("DWORD");
	atom_UINT      = pAtomTable->AddAtom("UINT");
	atom_BOOL	   = pAtomTable->AddAtom("BOOL");
	atom_D		   = pAtomTable->AddAtom("D");
	
	atom_this      = pAtomTable->AddAtom("this");
	atom_LPVOID    = pAtomTable->AddAtom("LPVOID");
	atom_LPSTR     = pAtomTable->AddAtom("LPSTR");
	atom_LPWSTR    = pAtomTable->AddAtom("LPWSTR");

	atom_HGDIOBJ   = pAtomTable->AddAtom("HGDIOBJ");
	atom_HDC	   = pAtomTable->AddAtom("HDC");
	atom_HPEN	   = pAtomTable->AddAtom("HPEN");
	atom_HBRUSH	   = pAtomTable->AddAtom("HBRUSH");
	atom_HBITMAP   = pAtomTable->AddAtom("HBITMAP");
	atom_HPALETTE  = pAtomTable->AddAtom("HPALETTE");
	atom_HRGN	   = pAtomTable->AddAtom("HRGN");
	atom_HFONT	   = pAtomTable->AddAtom("HFONT");

	return true;
}


int KBasicDecoder::Decode(ATOM typ, const void * pValue, char * szBuffer, int nBufferSize)
{
	unsigned data = * (unsigned *) pValue;

//	strcpy(szBuffer, m_pAtomTable->GetAtomName(typ));
//	szBuffer += strlen(szBuffer);

    if ( typ==atom_char )
    {
		* szBuffer++ = '\'';
        * szBuffer++ = (char) data;;
        * szBuffer++ = '\'';
        * szBuffer++ = 0;
        
		return 4;
	}

	if ( typ==atom_BYTE )
	{
		wsprintf(szBuffer, "%d", data & 0xFF);
		return 4;
	}

	if ( typ==atom_short )
	{
		data &= 0xFFFF;

		if ( data & 0x8000 )
			data |= 0xFFFF0000;

		wsprintf(szBuffer, "%d", data);

		return 4;
	}

	if ( typ==atom_WORD )
	{
		wsprintf(szBuffer, "%d", data & 0xFFFF);
		
		return 4;
	}

	if ( (typ==atom_long) || (typ==atom_int) )
	{
		wsprintf(szBuffer, "%d", data);
	
		return 4;
	}

	if ( (typ==atom_DWORD) || (typ==atom_UINT) )
	{
		wsprintf(szBuffer, "%u", data);

		return 4;
	}

	if ( typ==atom_D )
	{
		wsprintf(szBuffer, "0x%x", data);

		return 4;
	}

	if ( (typ==atom_LPVOID) || (typ==atom_this) )
	{
		if ( data==0 )
			strcpy(szBuffer, "NULL");
		else
			wsprintf(szBuffer, "0x%x", data);

		return 4;
	}

	if ( typ==atom_COLORREF )
	{
		if ( data==0 )
			strcpy(szBuffer, "BLACK");
		else if ( data==0xFFFFFF )
			strcpy(szBuffer, "WHITE");
		else
			wsprintf(szBuffer, "%06x", data);

		return 4;
	}

	if ( typ==atom_LPSTR )
	{
		if ( data==0 )
			strcpy(szBuffer, "NULL");
		else
		{
			* szBuffer='"';
			strcpy(szBuffer+1, (const char *) data);
			strcat(szBuffer, """");
		}

		return 4;
	}
	
	if ( typ==atom_BOOL )
	{
		if ( data==0 )
			strcpy(szBuffer, "FALSE");
		else if ( data==1 )
			strcpy(szBuffer, "TRUE");
		else
			wsprintf(szBuffer, "%d", data);

		return 4;
	}

	if ( (typ==atom_HDC)    || (typ==atom_HGDIOBJ)  || (typ==atom_HPEN) ||
		 (typ==atom_HBRUSH) || (typ==atom_HPALETTE) || (typ==atom_HRGN) ||
		 (typ==atom_HFONT) )
	{         
		wsprintf(szBuffer, "%x", data);
        
		return 4;
	}

	if ( typ==atom_returnaddr )
	{
		TranslateAddress(data, szBuffer);

		return 4;
	}

	// unhandled
	return 0;
}


void KBasicDecoder::TranslateAddress(unsigned addr, char * szBuffer)
{
	szBuffer[0] = 0;

	unsigned page = addr / 4096;	// 0..4b-1 -> 0..1Mb-1

	for (int i=0; i<m_pageno; i++)
		if ( m_modulepage[i]==page )
		{
			wsprintf(szBuffer, "%s+%x", m_modulename[i], addr - m_modulebase[i]);
			return;
		}


	MEMORY_BASIC_INFORMATION mbi;

	VirtualQuery((const void *) addr, & mbi, sizeof(mbi));

	TCHAR szModuleName[MAX_PATH];
	const TCHAR * pFileName;

	GetModuleFileName((HINSTANCE) mbi.AllocationBase, szModuleName, MAX_PATH);

	pFileName = szModuleName;

	while ( _tcschr(pFileName, '\\') )
		pFileName = _tcschr(pFileName, '\\') + 1;

	ATOM atm = m_pAtomTable->AddAtom(pFileName);

	if ( m_pageno==MAX_PAGES )
		m_pageno --;

	m_modulepage[m_pageno] = page;
	m_modulebase[m_pageno] = (unsigned) mbi.AllocationBase;
	m_modulename[m_pageno] = m_pAtomTable->GetAtomName(atm);

	wsprintf(szBuffer, "%s+%x", m_modulename[m_pageno], addr - m_modulebase[m_pageno]);
	
	m_pageno ++;
}

extern IDecoder   * pDecoderChain;
extern KFuncTable * pFuncTable;


int MainDecoder(ATOM typ, const void * pValue, char * szBuffer, int nBufferSize)
{
	IDecoder * pDecoder = pDecoderChain;

	while ( pDecoder!=NULL )
	{
		int size = pDecoder->Decode(typ, pValue, szBuffer, nBufferSize);

		if ( size!=0 )
			return size;

		pDecoder = pDecoder->pNextDecoder;
	}

	wsprintf(szBuffer, "%s(%x)", pFuncTable->GetAtomName(typ), * (unsigned *) pValue);

	return 4;
}


void MainDecoder(char buffer[], int limit, ATOM typ, unsigned value)
{
	int len = strlen(buffer);

	if ( len >= (limit-2) )
		return;

	if ( len )	// if not empty
	{
		buffer[len++] = ',';
		buffer[len++] = ' ';
	}

	MainDecoder(typ, & value, buffer+len, limit-len);
}
