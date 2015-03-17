//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : disasm.h				                                             //
//  Description: Intel machine code parser                                           //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#pragma once

#include "OpCode.h"

class KDisAsm  
{
	const char * m_funcname;
	bool		 m_error;

public:
	KDisAsm();
	virtual ~KDisAsm();

	int OperandLen(const OpCode * map, unsigned opcode, int opndsize, int & modrm);
	int Length(const unsigned char * code, const char * funcname);
};

int First5(const unsigned char * pProc, const char * funcname);