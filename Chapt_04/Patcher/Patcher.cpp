//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : patcher.cpp			                                             //
//  Description: API hacking by target relocation                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>

#include "Patcher.h"
#include "DisAsm.h"

bool HackCopy(void * pDest, void * pSource, DWORD nSize)
{
#ifdef WRITE_PROCESS

	static HANDLE hCurProcess = 0;

	DWORD dwWritten;

	bool rslt;
	
	if ( hCurProcess==0 )
		hCurProcess = GetCurrentProcess();

	rslt = WriteProcessMemory(hCurProcess, pDest, pSource, nSize, & dwWritten) != 0;

	return rslt && (dwWritten==nSize);

#else

	__try
	{
		memcpy(pDest, pSource, nSize);
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
		__try
		{
			MEMORY_BASIC_INFORMATION mbi;
            
			VirtualQuery(pDest, & mbi, sizeof(mbi));
			VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, & mbi.Protect);

			memcpy(pDest, pSource, nSize);
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			return false;
		}
    }

	return true;

#endif
}

#pragma pack(1)

// the longest instruction possible, 15 bytes
typedef struct
{
	unsigned char opcode;
	unsigned long addr;
	unsigned char nop[10];

}   LongInstruction;


bool KPatcher::Patch(HMODULE hModule, const TCHAR * name, int funcid, 
					 void * pProxy, unsigned long * pNewAddress)
{
	unsigned char * pProc = (unsigned char *) GetProcAddress(hModule, name);

	if ( pProc==NULL )
		return false;

	// length of first a few instructions no less than 5 bytes
	int len = First5(pProc, name);

	if (len < 5)
		return false;

	void * start = & m_buffer[m_bufpos];		// remember stub starting address

	Asm((char) 0x68); // push					// push funcid
	Asm((unsigned long) funcid);

	Asm((char) 0xE9); // jmp					// jmp pProxy
	AsmRel(pProxy);						

	* pNewAddress = (unsigned long ) & m_buffer[m_bufpos];	// pProxy will transfer control to here

	// copy original function starting instructions	// original function prolog
	memcpy(& m_buffer[m_bufpos], pProc, len); 
	m_bufpos += len;
	
	// jump to instructions after the header		// jump prolog + len
	Asm((char) 0xE9); // jmp
	AsmRel(pProc + len);

	// change header to jump to code in m_buffer
	LongInstruction inst;
	memset(& inst, 0x90, sizeof(inst));
	inst.opcode = 0xE9;										// jump
	inst.addr   = (unsigned) start - (unsigned) (pProc+5);	// stub
	
	return HackCopy(pProc, & inst, len);
}

