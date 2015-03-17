//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : stream.cpp						                                     //
//  Description: KInputFile, KOutputFile                                             //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <assert.h>
#include <string.h>

#include "Stream.h"

BOOL KInputFile::Open(const TCHAR *filename)
{
	m_handle = CreateFile(filename, GENERIC_READ, 0, NULL,
					      OPEN_EXISTING,
					      FILE_ATTRIBUTE_NORMAL, NULL);
	
    if (m_handle==INVALID_HANDLE_VALUE) return FALSE;
            
	m_eof        = FALSE;
	m_bufpos     = 0;
	m_buflen	 = 0;
    m_fileoffset = 0;    

    return TRUE;
}                           


void KInputFile::Reset(void)        
{
	m_eof        = FALSE;
	m_bufpos     = 0;
	m_buflen     = 0;
	m_fileoffset = 0;

	SetFilePointer(m_handle, 0, 0, FILE_BEGIN);
}


void KInputFile::Close(void)
{
    if (m_handle!=INVALID_HANDLE_VALUE)
    {
		CloseHandle(m_handle);

        m_handle = INVALID_HANDLE_VALUE;
    }
}


BYTE KInputFile::readbyte(void)
{
    if (m_bufpos >= m_buflen)             // input buffer empty
    {
        if (m_eof)
            return c_EOF;

		ReadFile(m_handle, m_buffer, c_bufsize, &m_buflen, NULL); 
		m_fileoffset += m_buflen;

        m_bufpos = 0;
        if (m_buflen<=0) 
        {   
            m_eof=TRUE;
            return c_EOF;
        }            
    }
    
    return m_buffer[m_bufpos++]; 
}


DWORD KInputFile::readdword(void)
{
	DWORD *prslt;
	char   temp[4];

	if (m_buflen < m_bufpos + 4 )
	{
		for (int i=0; i<4; i++)
			temp[i] = readbyte();

		prslt = (DWORD *)temp;
	}
	else
	{
		prslt = (DWORD *) (& m_buffer[m_bufpos]);
		m_bufpos+= 4;
	}

	return *prslt;
}


int KInputFile::read(char *buffer, int len)
{
	int copied = 0;

	while (len)
	{
		int l = m_buflen - m_bufpos;

		if (l>0)				// copy from current buffer
		{
			if (l>len)
				l = len;

            if (buffer)
            {
			    memcpy(buffer, & m_buffer[m_bufpos], l);
			    buffer   += l;
            }
	
			copied   += l;
			m_bufpos += l;
			len      -= l;
		}
		else 
		{
			if ( buffer==NULL ) // skipping only
			{
				m_fileoffset += len;
				SetFilePointer(m_handle, m_fileoffset, NULL, FILE_BEGIN);	// move pointer

				return copied + len;										// m_buffer is still empty
			}

			ReadFile(m_handle, m_buffer, c_bufsize, &m_buflen, NULL);		// read a buffer-full of data
			m_fileoffset += m_buflen;

			m_bufpos = 0;
			
			if (m_buflen<=0)
			{
				m_eof = TRUE;
				break;
			}
		}
	}

	return copied;
}

/////////////////////////////////////////

BOOL KOutputFile::Open(const char far *filename)
{
    m_handle = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_NEW,
						  FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_handle==INVALID_HANDLE_VALUE) return FALSE;
            
    m_bufpos = 0;
        
    return TRUE;
}                           
        

void KOutputFile::Flush(void)
{   
    if (m_bufpos)
    {
		unsigned long dwWritten;

		WriteFile(m_handle, m_buffer, m_bufpos, &dwWritten, NULL);
        m_bufpos = 0;
    }        
}

void KOutputFile::Close(void)
{
    if (m_handle!=INVALID_HANDLE_VALUE)
    {
        Flush();
        CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
    }
}

void KOutputFile::write(BYTE b)
{   
    if (b=='\n')
    {   m_buffer[m_bufpos++ ] = '\r';
        if (m_bufpos >= c_bufsize)
            Flush();
    }
                
    m_buffer[m_bufpos++] = b;
    
    if (m_bufpos >= c_bufsize)             // input buffer empty
        Flush();
}

void KOutputFile::write(const char far *str)
{
    while (*str)
        write(*str++);
}

const char hexmap[] = "0123456789ABCDEF";

void KOutputFile::writehex(BYTE b)
{
    write(hexmap[ (b>>4) & 15]);
    write(hexmap[ b      & 15]);
}