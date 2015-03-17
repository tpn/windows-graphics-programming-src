//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : memdump.cpp					                                     //
//  Description: Memory dumper, Chapter 3                                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <fstream.h>
#include <stdio.h>

#include "MemDump.h"

void KMemDump::DumpLine(unsigned char * p, unsigned offset, int unitsize)
{
	if ( unitsize==1 )
	{
		wsprintf(m_line, "%08lx: "
						 "%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx "
					     "%02lx %02lx %02lx %02lx %02lx %02lx %02lx %02lx",
						p + offset, 
						p[0], p[1],  p[2],  p[3],  p[4],  p[5],  p[6],  p[7],
						p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);

	}
	else if ( unitsize==2 )
	{
		unsigned short * q = (unsigned short *) p;

		wsprintf(m_line, "%08lx: "
				        "%04lx %04lx %04lx %04lx %04lx %04lx %04lx %04lx",
						p + offset, 
						q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7]);
	}
	else
	{
		unsigned long * r = (unsigned long *) p;

		wsprintf(m_line, "%08lx: "
				        "%08lx %08lx %08lx %08lx",
						p + offset, 
						r[0], r[1], r[2], r[3]);
	}

    char s[2] = { 0 };

    strcat(m_line, "  ");
    for (int i=0; i<16; i++)
    {
        if ( isprint(p[i]) )
            s[0] = p[i];
        else
            s[0] = '.';

        strcat(m_line, s);
    }
        
    strcat(m_line, "\r\n");
}


void KMemDump::OpenDump(void)
{
   	TCHAR TempPath[MAX_PATH];

	GetTempPath(sizeof(TempPath), TempPath);
	
	GetTempFileName(TempPath, "Mem", 0000, m_filename);

    assert(m_stream==NULL);

	m_stream = new ofstream;
    assert(m_stream);

	m_stream->open(m_filename);
}


void KMemDump::Newline(void)
{
    assert(m_stream);

    * m_stream << "\n";
}


void KMemDump::Writeln(const TCHAR * text)
{
    assert(m_stream);

    * m_stream << text;
    * m_stream << "\n";
}


void KMemDump::CloseDump(void)
{
   	TCHAR TempPath[MAX_PATH];

	GetTempPath(sizeof(TempPath), TempPath);

    assert(m_stream);

	m_stream->close();

	wsprintf(TempPath, "notepad %s", m_filename);
	WinExec(TempPath, SW_NORMAL);

    m_stream = NULL;
}


void KMemDump::Dump(unsigned char * start, unsigned offset, int size, int unitsize)
{
    if ( offset==0 )
    {
	    HANDLE hHeaps[10];
	    int no = GetProcessHeaps(10, hHeaps);
	
        // walk the heap if it is a heap
	    for (int i=0; i<no; i++)
		    if ( start == hHeaps[i] )
		    {
			    PROCESS_HEAP_ENTRY entry;

			    entry.lpData = NULL;

			    while ( HeapWalk(start, & entry) )
			    {
				    wsprintf(m_line, "%x %d+%d bytes %x\r\n", 
                        entry.lpData, entry.cbData,
                        entry.cbOverhead, entry.iRegionIndex);
				
                    * m_stream << m_line;
			    }
			    * m_stream << "\r\n";
			
			    break;
		    }			
    }

	* m_stream << size;
	* m_stream << " bytes\n";
	while (size>0)
	{
		DumpLine(start, offset, unitsize);
		start += 16;
		size  -= 16;

		* m_stream << m_line;
	}
}
