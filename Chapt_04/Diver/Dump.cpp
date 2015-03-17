//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dump.cpp         		                                             //
//  Description: basic dumping class                                                 //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <io.h>

#include "Dump.h"
#include "Format.h"


void KDump::Newline(void)
{
	Write((char) 0x0D);
	Write((char) 0x0A);

	m_field = 0;
}


void KDump::Flush(void)
{
	if ( m_bufpos )
	{
		_lwrite(m_handle, m_buffer, m_bufpos);
		m_bufpos = 0;
    }
}


void KDump::Seperator(void)
{
	if ( m_field )
	{
		Write(',');
		Write(' ');
	}

	m_field ++;
}


void KDump::WriteField(const char * pStr)
{
	Seperator();
	Write(pStr, strlen(pStr));
}


void KDump::WriteField(LPCTSTR pStr, int nSize)
{
	Seperator();

	int len = strlen(pStr);

	Write(pStr, min(len, nSize));
}


void KDump::WriteField(unsigned value, ATOM typ)
{
	char buffer[256];

	buffer[0] = 0;
	MainDecoder(buffer, sizeof(buffer), typ, value);

	WriteField(buffer);
}


void KDump::Write(const void * pData, int len)
{    
    if ( m_bufpos + len > sizeof(m_buffer) )
        Flush();

    if ( len > sizeof(m_buffer) )
	{
		_lwrite(m_handle, (const char *) pData, len);
		return;
	}
    
    memcpy(m_buffer + m_bufpos, pData, len);
    m_bufpos += len;
}   


void KDump::Write(char ch)
{
	if ( m_bufpos + 1 > sizeof(m_buffer) )
		Flush();

	m_buffer[m_bufpos++] = ch;
}


int KDump::BeginDump(const char *pattern)
{ 
    int		 i;
	OFSTRUCT m_ofs;
	char	 m_filename[MAX_PATH];
	
    // search for next file in the sequence of ...01.dbf -> ...99.dbf
    for (i=0; i<100; i++)
    {  
        wsprintf(m_filename, pattern, i);
        
        m_handle = OpenFile(m_filename, &m_ofs, OF_EXIST);  
        if (m_handle==HFILE_ERROR) break;
    }    
    
    m_handle = OpenFile(m_filename, &m_ofs, OF_CREATE | OF_WRITE);
    
    if (m_handle==HFILE_ERROR) 
        return -1;

    m_bufpos = 0;
	m_field  = 0;

    return i;
}    


void KDump::EndDump(void)
{
//	Write((char) 0x1A);

    Flush();
	
	_lclose(m_handle);
    
	m_handle = HFILE_ERROR;
}

