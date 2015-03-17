#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : lookup.h						                                     //
//  Description: index to string mapping                                             //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

typedef struct
{ 
	unsigned      key;
	unsigned long mask;
    const TCHAR * name;
} DicItem;


#define twin(name)		    { name, 0xffffffff, TEXT(#name) }
#define pair(name)		    { name, 0xffffffff, TEXT(#name) }
#define item(key, name)     { key,  0xffffffff, name        }
#define tripod(name, mask)  { name, mask,       TEXT(#name) }
#define dummyitem           { 0,    0xffffffff, NULL        }

bool         Lookup(unsigned index, const DicItem *dic, TCHAR * szResult);
const TCHAR *Lookup(unsigned index, const DicItem *dic);

unsigned     Decode(const TCHAR *item, const DicItem *dic);

