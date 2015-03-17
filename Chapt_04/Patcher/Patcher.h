//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : patcher.h				                                             //
//  Description: API hacking by target relocation                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#pragma once

class KPatcher  
{
	typedef enum { MaxBufferSize = 16384 };

	char	  m_buffer[MaxBufferSize];
	int		  m_bufpos;

	void Asm(char ch)
	{
		m_buffer[m_bufpos++] = ch;
	}

	void Asm(unsigned long data)
	{
		* ( (unsigned long *) (m_buffer+m_bufpos) ) = data;
		m_bufpos += sizeof(unsigned long);
	}

	void AsmRel(void * ptr)
	{
		Asm((unsigned long ) ptr - (unsigned long) (m_buffer+m_bufpos+4));
	}

public:
	
	KPatcher()
	{
		m_bufpos = 0;
	}

	bool Patch(HMODULE hModule, const TCHAR * name, int funcid, void * pProxy,
		       unsigned long *pNewAddress);
};
