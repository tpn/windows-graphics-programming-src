#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : emfdc.h						                                     //
//  Description: EMF decompiler								                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//


#include "Formator.h"


class KTreeView;

class KEmfDC : public KMemoryMappedFile 
{
	KofstreamPipe fmt;
	int			  m_nSeq;

	bool Decode(const EMR * emr, KTreeView * pTree);
	bool DeCompileBuffer(const TCHAR * outfile, const void * buffer, 
			KTreeView * pTree, HENHMETAFILE & hEmf);

public:
	
	const TCHAR * DecodeRecord(const EMR * emr);

	bool DeCompile(const TCHAR * outfile, const char * FileName, 
			 KTreeView * pTree, HENHMETAFILE & hEmf);
	bool DeCompile(const TCHAR * outline, HENHMETAFILE hEmf, 
			 KTreeView * pTree);
};

