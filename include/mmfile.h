#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : mmfile.h   					                                     //
//  Description: Memory-mapped file                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KMemoryMappedFile
{
    HANDLE                m_hFile;
    HANDLE                m_hMapping;

public:
    const unsigned char * m_View;

    KMemoryMappedFile()
    {
        m_hFile    = INVALID_HANDLE_VALUE;
        m_hMapping = NULL;
        m_View     = NULL;
    }

    ~KMemoryMappedFile();
    bool Open(LPCTSTR szFileName);
};
