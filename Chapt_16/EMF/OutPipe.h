#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : outpipe.h						                                     //
//  Description: KTextPipe: text-based output with formating                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include <tchar.h>

// generic output pipe

#include "..\..\include\lookup.h"

class KOutPipe  
{
protected:
	bool m_bOpened;

public:
	KOutPipe()
	{
		m_bOpened = false;
	}

	bool IsOpen(void) const
	{
		return m_bOpened;
	}

	virtual bool Close(void)
	{
		m_bOpened = false;
		return false;
	}

	virtual bool Open(const TCHAR * name) = 0;
	
	virtual bool Put(TCHAR ch) = 0;

	virtual ~KOutPipe()
	{
		if ( m_bOpened)
		{
			Close();
		}
	}

};


// c program text output pipe
class KTextPipe : public KOutPipe
{
protected:
	long m_indent;
	long m_nCount;
	long m_mCount;
	long m_linelen;

public:
	KTextPipe(void)
	{
		m_indent = 0;
		m_nCount = 1;
		m_mCount = 1;
		m_linelen = 0;
	}

	void Indent(long level)
	{
		m_indent += level;
	}

	void Newline(void);

	void PutQuote(char ch);

	void Write(const char  * str, long size= -1);
	void Write(const WCHAR * str, long size= -1);

	void WriteString(const void * str, long size, bool unicode);
	
	void WriteDec(long data);
	void WriteDec(unsigned long data);

	void WriteHex(unsigned long data);

	void WriteArray(const void * Array, long count, long elmsize, bool decimal=true);

	void Write(unsigned index, const DicItem *dic);

	virtual const long * Percent(TCHAR tag, const long * data, const DicItem * dic);
	virtual const long * Pound  (TCHAR tag, const long * data, const DicItem * dic);

	void Format(const TCHAR * pattern, const void * vdata, const DicItem * dic=NULL);
};


