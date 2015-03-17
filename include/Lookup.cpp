//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : lookup.cpp						                                     //
//  Description: index to string mapping                                             //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT
#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string.h>
#include <tchar.h>

#include "Lookup.h"


bool Lookup(unsigned index, const DicItem *dic, TCHAR * szResult)
{
    while (dic->name!=NULL)
    {
        if (dic->key==index)
		{
			_tcscpy(szResult, dic->name);
			return true;
		}
        dic++;            
    }            
    
    wsprintf(szResult, TEXT("Unknown(%lx)"), index);    
    
    return false;
}


const TCHAR *Lookup(unsigned index, const DicItem *dic)
{
    static TCHAR mess_Rslt[64];
    
	mess_Rslt[0] = 0;
    
	while (dic->name!=NULL)
    {
        if ((index & dic->mask) == dic->key)
		{
			if ( dic->mask == 0xffffffff)
				return dic->name;

			if ( mess_Rslt[0] )
			{
				lstrcat(mess_Rslt, _T(" | "));
				lstrcat(mess_Rslt, dic->name);
			}
			else
				lstrcat(mess_Rslt, dic->name);
		}
        
		dic++;            
    }            
    
	if ( mess_Rslt[0] == 0)
		wsprintf(mess_Rslt, TEXT("0x%x /*Unknown*/"), index);    
    
    return mess_Rslt;    
}


unsigned Decode(const TCHAR *item, const DicItem *dic)
{
    while (dic->name!=NULL)
    {
        if (_tcscmp(item, dic->name)==0)
            return dic->key;
        dic++;
    }
    
    return 0xFFFFFFFF;
}
