#pragma 

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : pehack.h						                                     //
//  Description: API hooking through import/export table                             //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KPEFile
{
	PIMAGE_DOS_HEADER pDOSHeader;
	PIMAGE_NT_HEADERS pNTHeader;

public:
	const char		* pModule;

	const char * RVA2Ptr(unsigned rva)
	{
		if ( (pModule!=NULL) && rva )
			return pModule + rva;
		else
			return NULL;
	}

	KPEFile(HMODULE hModule);
	const void * GetDirectory(int id);
	PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor(LPCSTR pDllName);
	const unsigned * GetFunctionPtr(PIMAGE_IMPORT_DESCRIPTOR 
		pImport, LPCSTR pProcName);
	
	FARPROC SetImportAddress(LPCSTR pDllName, LPCSTR pProcName, 
		FARPROC pNewProc);

	FARPROC SetExportAddress(LPCSTR pProcName, FARPROC pNewProc);
};

