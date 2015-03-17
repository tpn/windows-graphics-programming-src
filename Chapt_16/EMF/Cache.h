#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : cache.h						                                     //
//  Description: KCache class: remove duplication			                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KSignature
{
	long         m_size;
	const void * m_data;
	long		 m_seq;

public:

	KSignature()
	{
		m_size = 0;
		m_data = NULL;
		m_seq  = 0;
	}

	long Match(long size, const void * data)
	{
		if ( (m_size==size) && (memcmp(m_data, data, size)==0) )
			return m_seq;
		else
			return -1;
	}

	void Set(long size, const void * data, long seq)
	{
		m_size = size;
		m_data = data;
		m_seq  = seq;
	}
};


class KCache  
{
	typedef enum { CacheSize = 64 };

	KSignature m_Cache[CacheSize];
	long	   m_total;

public:
	KCache()
	{
		m_total = 0;
	}

	bool Match(long size, const void * data, long & rslt);
};

