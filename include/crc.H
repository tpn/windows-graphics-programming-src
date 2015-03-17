#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : crc.h					                                             //
//  Description: CRC check sum                                                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KCRC  
{
    unsigned short crctab[256];

    typedef enum
    {
                               //    		    CITT	XMODEM	ARC  	
        poly     = 0xA001,     // the poly:	    0x1021	0x1021	A001	
        initcrc  = 0x0000,     // init value:	-1	    0	    0	
        byteswap = true        // bit order:	false	true    true
    };

public:

	KCRC();

    unsigned short Update(unsigned short crc, unsigned char * buffer, int len);
};
