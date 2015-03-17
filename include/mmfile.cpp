//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : mmfile.cpp 					                                     //
//  Description: Memory-mapped file                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "mmfile.h"

bool KMemoryMappedFile::Open(LPCTSTR szFileName)
{
	m_hFile = CreateFile(szFileName, GENERIC_READ, 
        FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( m_hFile == INVALID_HANDLE_VALUE )
		return false;

	m_hMapping = CreateFileMapping(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);

	if ( m_hMapping )
    {
		m_View = (const unsigned char *) MapViewOfFile(m_hMapping, FILE_MAP_READ, 0, 0, 0);

        return m_View != NULL;
    }
    else
        return false;
}


KMemoryMappedFile::~KMemoryMappedFile()
{
	if ( m_View )
	{
		UnmapViewOfFile(m_View);
		m_View = NULL;
	}

	if ( m_hMapping )
	{
		CloseHandle(m_hMapping);
		m_hMapping = NULL;
	}

	if ( m_hFile!=INVALID_HANDLE_VALUE )
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}
