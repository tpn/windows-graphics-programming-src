//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : crc.cpp					                                         //
//  Description: CRC checksum                                                        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include "CRC.h"

unsigned short KCRC::Update(unsigned short crc, unsigned char * buffer, int len )
{
    while ( len-- > 0 ) 
    {
        if ( byteswap )
	        crc = (crc << 8) ^ crctab[(crc>>8)     ^ * buffer++];
        else
	        crc = (crc >> 8) ^ crctab[(crc & 0xFF) ^ * buffer++]; 
    }

    return crc;
}


KCRC::KCRC()
{
    for (int b = 0; b <= 0xFF; b++ )
    {
        unsigned short v;
        int i;

        if (byteswap)
        {
    	    v = b << 8;
            for (i=0; i<8; i++)
	            v = (v & 0x8000) ? (v<<1) ^ poly : v << 1;
        }
        else
        {
            v = b;
	        for(i=0; i<8; i++)
	            v = v & 1 ? (v>>1) ^ poly : v >> 1;
        }

	    crctab[b] = v;
    }
}
