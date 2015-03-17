//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : cache.cpp						                                     //
//  Description: KCache class: remove duplication			                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <memory.h>

#include "Cache.h"


bool KCache::Match(long size, const void * data, long & rslt)
{
	for (int i=0; i<CacheSize; i++)
	{
		rslt = m_Cache[i].Match(size, data);
		
		if ( rslt>=0 )
			return true;
	}

	rslt = m_total ++;
	
	m_Cache[rslt % CacheSize].Set(size, data, rslt);

	return false;
}

