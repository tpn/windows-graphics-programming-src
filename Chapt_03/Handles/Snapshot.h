#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : snapshot.h				                                             //
//  Description: Virtual memory snap shot                                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include "crc.h"

// Memory Region
class KRegion
{
public:
    unsigned char * start;
    unsigned        size;
    unsigned        type;
    unsigned short  crc;
	unsigned short  lastcrc;
	int				count;
};

class KListView;


class KSnapShot
{
    typedef enum { nMaxRegionNo = 512 };

    KCRC    m_crc;

    KRegion m_Regions[nMaxRegionNo];
    int     m_nRegionNo;
	
	KRegion * FindRegion(unsigned char * regionstart, unsigned regionsize);

public:
    KSnapShot()
    {
        m_nRegionNo = 0;
	}

    void Shot(KListView * list);
	void ShowDetail(HINSTANCE hInst, unsigned start, unsigned size);
};


