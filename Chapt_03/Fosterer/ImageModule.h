#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : imagemodule.h					                                     //
//  Description: Debug symbol query using ImageHlp API                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KImageModule  
{
    HANDLE                   m_hProcess;   
    IMAGEHLP_SYMBOL          m_is[10];    // leave enough space for the name

public:
	KImageModule();
	virtual ~KImageModule();
    
	PIMAGE_DEBUG_INFORMATION m_pidi;

    bool Load(char * filename, char * sympath);
    void Unload(void);

    const IMAGEHLP_SYMBOL * ImageGetSymbol(const char * name);
};

