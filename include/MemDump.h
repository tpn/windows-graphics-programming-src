#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : memdump.h					                                         //
//  Description: Memory dumper                                                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class ofstream;

class KMemDump
{
    TCHAR    m_filename[MAX_PATH];

    ofstream * m_stream;

public:
	TCHAR    m_line[128];
    
    void OpenDump(void);
    void CloseDump(void);

    void DumpLine(unsigned char * p, unsigned offset, int unitsize);
    void Dump(unsigned char * start, unsigned offset, int size, int unitsize);
    void Newline(void);
    void Writeln(const TCHAR * text);
    
	KMemDump()
	{
        m_stream = NULL;
	}
};


