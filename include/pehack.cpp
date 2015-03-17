//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : pehack.cpp						                                     //
//  Description: API hooking through import/export table                             //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "pehack.h"

KPEFile::KPEFile(HMODULE hModule)
{
	pModule = (const char *) hModule;

	if ( IsBadReadPtr(pModule, sizeof(IMAGE_DOS_HEADER)) )
	{
		pDOSHeader = NULL;
		pNTHeader  = NULL;
	}
	else
	{
		pDOSHeader = (PIMAGE_DOS_HEADER) pModule;

		if ( IsBadReadPtr(RVA2Ptr(pDOSHeader->e_lfanew), 
			sizeof(IMAGE_NT_HEADERS)) )
			pNTHeader = NULL;
		else
			pNTHeader = (PIMAGE_NT_HEADERS) RVA2Ptr(pDOSHeader->
				e_lfanew);
	}
}


// returns address of a PE directory
const void * KPEFile::GetDirectory(int id)
{
	return RVA2Ptr(pNTHeader->OptionalHeader.DataDirectory[id].
		VirtualAddress);
}


// returns PIMAGE_IMPORT_DESCRIPTOR for an imported module
PIMAGE_IMPORT_DESCRIPTOR KPEFile::GetImportDescriptor(
	LPCSTR pDllName)
{
	// first IMAGE_IMPORT_DESCRIPTOR
	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR) 
		GetDirectory(IMAGE_DIRECTORY_ENTRY_IMPORT);
	
	if ( pImport==NULL )
		return NULL;

	while ( pImport->FirstThunk )
	{
		if ( stricmp(pDllName, RVA2Ptr(pImport->Name))==0 )
			return pImport;

		// move to next imported module
		pImport ++;
	}

	return NULL;
}


// returns address of __imp__xxx variable for an import function
const unsigned * KPEFile::GetFunctionPtr(
	PIMAGE_IMPORT_DESCRIPTOR pImport, LPCSTR pProcName)
{
	PIMAGE_THUNK_DATA pThunk;

	pThunk = (PIMAGE_THUNK_DATA) RVA2Ptr(pImport->
		OriginalFirstThunk);

	for (int i=0; pThunk->u1.Function; i++)
	{
		bool match;

		if ( pThunk->u1.Ordinal & 0x80000000 )	// by ordinal
			match = (pThunk->u1.Ordinal & 0xFFFF) == 
			         ((DWORD) pProcName);
		else
			match = stricmp(pProcName, RVA2Ptr((unsigned) 
			         pThunk->u1.AddressOfData)+2) == 0;

		if ( match )
			return (unsigned *) RVA2Ptr(pImport->FirstThunk)+i;
		
		pThunk ++;
	}

	return NULL;
}


BOOL HackWriteProcessMemory(HANDLE hProcess, void * pDest, void * pSource, DWORD nSize, DWORD * pWritten)
{
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
			return FALSE;
		}
    }

	* pWritten = nSize;

	return TRUE;
}

FARPROC KPEFile::SetImportAddress(LPCSTR pDllName, 
	LPCSTR pProcName, FARPROC pNewProc)
{
	PIMAGE_IMPORT_DESCRIPTOR pImport = 
		GetImportDescriptor(pDllName);

	if ( pImport )
	{
		const unsigned * pfn = GetFunctionPtr(pImport, pProcName);

		if ( IsBadReadPtr(pfn, sizeof(DWORD)) )
			return NULL;

		// read the original function address
		FARPROC oldproc = (FARPROC) * pfn;

		DWORD dwWritten;

		// overwrite with new function address
		HackWriteProcessMemory(GetCurrentProcess(), (void *) pfn, 
			& pNewProc, sizeof(DWORD), & dwWritten);

		return oldproc;
	}
	else
		return NULL;
}


FARPROC KPEFile::SetExportAddress(LPCSTR pProcName, 
	FARPROC pNewProc)
{
	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)
		GetDirectory(IMAGE_DIRECTORY_ENTRY_EXPORT);

	if ( pExport==NULL )
		return NULL;

	unsigned ord = 0;

	if ( (unsigned) pProcName < 0xFFFF ) // ordinal ?
		ord = (unsigned) pProcName;
	else
	{
		const DWORD * pNames = (const DWORD *) RVA2Ptr(pExport->AddressOfNames);
		const WORD  * pOrds  = (const WORD  *) RVA2Ptr(pExport->AddressOfNameOrdinals);
		
		// find the entry with the function name
		for (unsigned i=0; i<pExport->AddressOfNames; i++)
			if ( stricmp(pProcName, RVA2Ptr(pNames[i]))==0 )
			{
				// get the corresponding ordinal
				ord = pExport->Base + pOrds[i];
				break;
			}
	}

	if ( (ord<pExport->Base) || (ord>pExport->NumberOfFunctions) )
		return NULL;

	// use ordinal to get the address where export RVA is stored
	DWORD * pRVA = (DWORD *) RVA2Ptr(pExport->AddressOfFunctions) +
		ord - pExport->Base;

	// read original function address
	DWORD rslt = * pRVA;

	DWORD dwWritten = 0;
	DWORD newRVA = (DWORD) pNewProc - (DWORD) pModule;
	HackWriteProcessMemory(GetCurrentProcess(), pRVA, & newRVA, sizeof(DWORD), & dwWritten);

	return (FARPROC) RVA2Ptr(rslt);
}
