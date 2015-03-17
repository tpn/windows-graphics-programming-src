//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : formator.h						                                     //
//  Description: Text output stream with Win32 data type support                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#pragma once

#include "..\..\include\lookup.h"
#include "Cache.h"
#include "OutPipe.h"


class KWinPipe : public KTextPipe
{
	KCache   m_DIBCache;
	long     m_curDIB;

	KCache   m_BMICache;
	long     m_curBMI;

	KCache   m_PackedDIBCache;

	long     m_curRegion;
	
public:
	long     m_curPackedDIB;

	KWinPipe()
	{
		m_curBMI = -1;
		m_curDIB = -1;
		m_curPackedDIB = -1 ;

		m_curRegion = 0;
	}

	void AddBrush(const LOGBRUSH * logbrush, bool create=true);
	void AddColor(COLORREF color);
	
	void AddRegion(unsigned long size, const RGNDATA * data);

	const long * Pound(char tag, const long * data, const DicItem * dic);

	bool SaveDIB(const BITMAPINFO * bmi, long bmisize, const void * bits, long bitsize);

	bool AddDIB(const void * header, 
		        long bmioffset, long bmisize,
				long bitoffset, long bitsize,
				TCHAR format[], bool pack = false);
};


class KofstreamPipe : public KWinPipe 
{
	ofstream m_stream;
	
public:

	TCHAR m_buffer[2048];
	int   m_nPos;

	KofstreamPipe(void)
	{
		m_nPos = 0;
	}

	bool Open(const TCHAR * name)
	{
#ifdef _UNICODE
		return false;
#else
		m_stream.open(name);
		m_bOpened = true;
		
		return true;
#endif
	}

	bool Close(void)
	{
		if (m_bOpened)
		{
			m_stream.close();
			m_bOpened = false;
		}

		return true;
	}

	void Clear(void)
	{
		m_nPos = 0;
		m_buffer[0] = 0;
	}

	bool Put(TCHAR ch)
	{
		if ( ch=='\r')     // ignored
			return true;

		if ( m_bOpened )
		{
			m_linelen ++;
			m_stream << ch;
		} 
		
		if ( m_nPos < (MAX_PATH-1) )
		{
			if ( ch=='\n' )
				Clear();
			else
			{
				m_buffer[m_nPos++] = ch;
				m_buffer[m_nPos] = 0;
			}

			return true;
		}			
		else
			return false;
	}

};


