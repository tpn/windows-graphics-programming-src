//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : atom.h				                                                 //
//  Description: KAtomTable class                                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include "Decoder.h"

class KAtomTable : public IAtomTable
{
	typedef enum 
	{
		MAX_BUFFER = 32768,
		MAX_NAMES  =  2048
	};

	int       m_bufpos;
	int		  m_names;

	char         m_buffer[MAX_BUFFER];
	const char * m_name[MAX_NAMES];

public:
	
	KAtomTable(void);

	virtual ATOM AddAtom(const char * name);

	virtual const char * GetAtomName(ATOM atom)
	{
		return m_name[(int)atom];
	}
};


