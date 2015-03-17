//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : atom.cpp				                                             //
//  Description: KAtomTable class                                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h> 
#include <string.h>
#include <assert.h>

#include "Decoder.h"
#include "atom.h"


KAtomTable::KAtomTable(void)
{
    m_bufpos = 2; // empty name is of offset 0
 
	m_buffer[0] = 0;
    m_buffer[1] = 0;

	m_names   = 1;
	m_name[0] = m_buffer;
}


ATOM KAtomTable::AddAtom(const char *name)
{
    // check for null or empty name, return 0 as index
    if ( (name==NULL) || (name[0]==0) )
        return (ATOM) 0;

	int len = strlen(name);

	// search for it
	for (int i=1; i<m_names; i++)
		if ( stricmp(name, m_name[i])==0 )
			return (ATOM) i;

    if ( m_bufpos+len >= MAX_BUFFER )
    {
        assert(false);
        return (ATOM) 0;
    }
           
    m_name[m_names] = m_buffer + m_bufpos;
    
	strcpy(m_buffer + m_bufpos, name);
    m_bufpos += len;
    m_buffer[m_bufpos++] = 0;

    return (ATOM) (m_names++);
}


