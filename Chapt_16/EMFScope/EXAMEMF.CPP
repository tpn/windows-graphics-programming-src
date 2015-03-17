//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : examemf.cpp						                                 //
//  Description: Check EMF data format                                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <assert.h>

#include "Stream.h"
#include "Examemf.h"

const int MaxEmrField = 16384;         // 16K*sizeof(DWORD), maximum 64 Kb

typedef struct 
{
	DWORD   iType;                // Enhanced metafile record type
	DWORD   nSize;                // Length of the record in bytes.
			                          // This must be a multiple of 4.
	DWORD   Para[MaxEmrField-2];  // Parameter
} EmrRecord;


// check if input file is a valid NT spooler emf file
BOOL CheckNTSpoolEmfFile(KInputFile *emf, ENHMETAHEADER *header)
{
	EmrRecord rec;

	emf->Reset();

	// 0x00010000 len 
	emf->read((char *) & rec, 8);

	if (rec.iType!=0x10000) return FALSE;

	// appname document name port
	emf->read((char *) & rec.Para, rec.nSize-8);

	while (true)
	{
		// 0x000000001 EMF_Stream_len
		emf->read((char *) & rec, 8);
	
		if ( (rec.iType==1) || (rec.iType==12) ) break;

		if (rec.iType < 0xFFFF)
		{
			if ( rec.iType==2 )
				emf->read(NULL, rec.nSize);  // embedded font
			else
				emf->read(NULL, rec.nSize-8);
		}
	}
	
	emf->read((char *)header, sizeof(ENHMETAHEADER));

	return header->iType==1;
}


BOOL PlayNTEnhMetaFile(HDC hDC, KInputFile *emf, RECT *rect)
{
	EmrRecord rec;
	
	emf->Reset();
	// 0x00010000 len 
	emf->read((char *) & rec, 8);

	if (rec.iType!=0x10000) return FALSE;

	// appname document name port
	emf->read((char *) & rec.Para, rec.nSize-8);

	while (true)
	{
		// 0x000000001 EMF_Stream_len
		emf->read((char *) & rec, 8);
	
		if ( (rec.iType==1) || (rec.iType==12) ) break;

		if (rec.iType < 0xFFFF)
			if ( rec.iType==2 )
				emf->read(NULL, rec.nSize);  // embedded font
			else
				emf->read(NULL, rec.nSize-8);
	}
	

//	// 0x000000001 EMF_Stream_len
//	emf->read((char *) & rec, 8);
//	if (rec.iType!=1) return FALSE;

	if (hDC==NULL)
		return TRUE;

	byte * bits = (byte *) malloc(rec.nSize);
	if ( bits == NULL )
		return FALSE;

	emf->read((char *) bits, rec.nSize);
	HENHMETAFILE hEmf = SetEnhMetaFileBits(rec.nSize, bits);
	PlayEnhMetaFile(hDC, hEmf, rect);
	DeleteEnhMetaFile(hEmf);
	free(bits);

	return TRUE;
}