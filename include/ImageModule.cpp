//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : imagemodule.cpp				                                     //
//  Description: Using ImageHlp API for debug symbol querying                        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define NOCRYPT
#define WIN32_LEAN_AND_MEAN

// the newer imagehlp.h includes <wintrust.h>, the two macros fakes the include
#define WINTRUST_H
#define LPWIN_CERTIFICATE LPVOID

#include <windows.h>
#include <imagehlp.h>
#include <assert.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "ImageModule.h"

KImageModule::KImageModule(HWND hOutput)
{
    m_hProcess		  = GetCurrentProcess();
	m_hOutput		  = hOutput;

	m_bDecoratedNames = false;
	m_bShowFPO		  = false;
	m_bForPogy		  = false;
	m_bLoaded		  = false;
}


KImageModule::~KImageModule()
{
    Unload();
}


bool KImageModule::Load(char * filename, char *sympath)
{
	_tcscpy(m_modulename, filename);

	memset(& m_image, 0, sizeof(m_image));
	
	m_imagebase_loaded = (DWORD) GetModuleHandle(filename);

	if ( m_imagebase_loaded ) // module is already loaded, for example GDI32.DLL
	{
		m_imagebase_default = m_imagebase_loaded;
		m_bLoaded			= false;

		PIMAGE_NT_HEADERS pNTHeader = ImageNtHeader((void *)m_imagebase_loaded);

		Output("%s already loaded at 0x%x %s\n", filename, m_imagebase_loaded,
			ctime( (time_t *) & pNTHeader->FileHeader.TimeDateStamp));
	}
	else
	{
		if ( MapAndLoad(filename, NULL, & m_image, FALSE, TRUE) )
			Output("%s loaded at 0x%x %s\n", m_image.ModuleName, m_image.MappedAddress, 
				ctime( (time_t *) & m_image.FileHeader->FileHeader.TimeDateStamp));
		else
		{
			Output("Unable to load %s\n", filename);

			return false;
		}

		m_imagebase_loaded  = (DWORD) m_image.MappedAddress;
		m_imagebase_default = m_image.FileHeader->OptionalHeader.ImageBase;
		m_bLoaded           = true;
    }

    
	if ( !SymInitialize(m_hProcess, sympath, FALSE) ) 
	{
		Output("SymInitialize failed\n\n");
        return false;
	}

	
	m_symbolbase = SymLoadModule(m_hProcess, NULL, filename, 0, m_imagebase_loaded, 0 );
	
	if ( m_symbolbase==0 )
	{
		Output("SymLoadModule failed\n\n");
        return false;
	}

    IMAGEHLP_MODULE im;
    im.SizeOfStruct = sizeof(im);

    SymGetModuleInfo( m_hProcess, m_symbolbase, &im );
	
	Output("""%s"" loaded. %s\n", im.LoadedImageName, ctime((time_t *) & im.TimeDateStamp));
	
	return true;
}


void KImageModule::Unload(void)
{
    SymUnloadModule(m_hProcess, m_symbolbase);
    SymCleanup(m_hProcess);

	if ( m_image.ModuleName )
		UnMapAndLoad(& m_image);
} 


const IMAGEHLP_SYMBOL * KImageModule::ImageGetSymbol(const char * name)
{
	char localname[MAX_PATH];

    memset(m_is, 0, sizeof(m_is));
    m_is[0].SizeOfStruct  = sizeof(IMAGEHLP_SYMBOL);
    m_is[0].MaxNameLength = sizeof(m_is) - sizeof(m_is[0]);
    
	// The silly implementation in imagehlp.dll will try to change the '!' in name
	// to 0, which generates an access violation, because name would came from read-only
	// constant data. Make a local copy to solve the problem
	strcpy(localname, name);
    
	if ( SymGetSymFromName(m_hProcess, localname, m_is) )
    {
		// On Win2K RC1, m_is[0].Address is a valid address, m_pidi->ReservedMappedBase=NULL
		if ( m_symbolbase )
			m_is[0].Address += m_imagebase_default - m_symbolbase;
        
        return & m_is[0];
    }
    else
    {
        DWORD err = GetLastError();
        return NULL;
    }
}
 

bool KImageModule::LoadSystemModule(char * module, char* extension)
{
	char SymbolDir[MAX_PATH];

	GetSystemDirectory(SymbolDir, sizeof(SymbolDir)); // c:\winnt\system32

	char * p = strchr(SymbolDir, '\\');     // first '\\'
	while ( p && strchr(p+1, '\\') )       // last '\\'	
		p = strchr(p+1, '\\');

	if ( p )								// c:\winnt
		* p = NULL;

	strcat(SymbolDir, "\\symbols\\");
	strcat(SymbolDir, extension);

	return Load(module, SymbolDir);
}


void KImageModule::ShowFPO(ULONG SymbolAddress)
{
	if ( m_bShowFPO )
	{
		const FPO_DATA * pFPO = (const FPO_DATA *) SymFunctionTableAccess(m_hProcess, SymbolAddress);
		
		if ( pFPO )
		{
			Output("         ");
			Output("s=%4d l=%2d, p=%2d, e=%2d, r=%2d, s=%d, b=%d, f=%d\n",
					pFPO->cbProcSize,
					pFPO->cdwLocals,
					pFPO->cdwParams,
					pFPO->cbProlog,
					pFPO->cbRegs,
					pFPO->fHasSEH,
					pFPO->fUseBP,
					pFPO->cbFrame);
		}
	}
}


void KImageModule::TranslateName(LPSTR SymbolName)
{
	// If "decorated" names were specified, and if the name is "decorated," 
	// undecorate it so that a human readable version can be displayed. 
	
	if ( m_bDecoratedNames /* && ('?' == *SymbolName) */ ) 
	{ 
		char szUndecoratedName[0x400]; // Make symbol name buffers for the 
		char szDecoratedName[0x400]; // decorated & undecorated versions 

		// Make a copy of the original SymbolName, so that we can modify it 

		lstrcpy( szDecoratedName, SymbolName ); 

		PSTR pEnd = szDecoratedName + lstrlen( szDecoratedName ); 

		// Strip everything off the end until we reach a 'Z' 

		//	while ( (pEnd > szDecoratedName) && (*pEnd != 'Z') ) 
		//		*pEnd-- = 0; 

		// Call the IMAGEHLP function to undecorate the name 

		if ( 0 != UnDecorateSymbolName( szDecoratedName, szUndecoratedName, 
			sizeof(szUndecoratedName), UNDNAME_COMPLETE | UNDNAME_32_BIT_DECODE ) ) 
		{ 
			// End the output line with the undecorated name 
			Output("         %s\n", szUndecoratedName ); 
		} 
	} 
}


// translate symbol va address symbva to RVA symbva - mappedBase
// translate RVA to pointer within the loaded image
const unsigned char * KImageModule::GetImagePointer(unsigned symbva)
{
	assert(m_imagebase_loaded);

	return (const unsigned char *) 
		ImageRvaToVa(m_image.FileHeader, (void *) m_imagebase_loaded, 
		                symbva - m_imagebase_default, NULL);
}

// translate actual loaded pointer to to pointer within the loaded image
const unsigned char * KImageModule::Address2ImagePointer(unsigned addr)
{
	assert(m_imagebase_loaded);

	return (const unsigned char *) 
		ImageRvaToVa(m_image.FileHeader, (void *) m_imagebase_loaded, 
		                addr - m_image.FileHeader->OptionalHeader.ImageBase, NULL);
}


BOOL KImageModule::EnumSymbolsCallback(LPSTR SymbolName, ULONG SymbolAddress, ULONG SymbolSize) 
{ 
	unsigned callid = 0;
	unsigned parano = 0;

	// translate symbol address SymbolAddress to RVA SymbolAddress - mappedBase
	// translate RVA to pointer within the loaded image
	const unsigned char * p;
	
	if ( m_bLoaded )
		p = GetImagePointer(SymbolAddress);
	else
		p = (const unsigned char *) SymbolAddress;

	// Checking for system service call pattern
	if ( ! IsBadReadPtr(p, 14) )
	if ( (p[0]==0xB8) &&												  // mov eax, <callid>
		 (p[5]==0x8D) && (p[6]==0x54) && (p[7]==0x24) && (p[8]==0x04) &&  // lea edx, [esp+4]
		 (p[9]==0xCD) && (p[10]==0x2E) )								  // int 2E
	{
		callid = * (unsigned *) (p+1);
			
		if ( p[11]==0xC2 )												  // ret <parasize>	
		    parano = * (unsigned short *) (p+12) / 4;
	}
	
	if ( callid )
	{
		const IMAGEHLP_SYMBOL * pSymbol = ImageGetSymbol(SymbolName);

		// print out the RVA, and the symbol name passed to us. 
		if ( m_bForPogy )
		{				
			Output( "D %s(", SymbolName);
			for (unsigned i=0; i<parano; i++)
			{
				Output("D");
				if ( i != (parano-1) )
					Output(",");
			}
			Output("), %08X, %x\n", pSymbol->Address, callid);
		}
		else
		{
			Output( "syscall(0x%04x, %2d) %08X %s!%s\n", callid, parano, pSymbol->Address, m_modulename, SymbolName); 
	
			ShowFPO(SymbolAddress);
			TranslateName(SymbolName);
		}
		m_nCount ++;
	}

	return TRUE; 
} 


BOOL CALLBACK EnumSymbolsCallback(LPSTR SymbolName, ULONG SymbolAddress, ULONG SymbolSize, 
								  PVOID UserContext ) 
{
	assert( ! IsBadReadPtr(UserContext, sizeof(KImageModule)) );

	return ((KImageModule *) UserContext)->EnumSymbolsCallback(SymbolName, SymbolAddress, SymbolSize);
}


void KImageModule::EnumerateSymbols(bool bForPogy)
{
	m_nCount  = 0;
	m_bForPogy = bForPogy;

	SymEnumerateSymbols(m_hProcess, m_imagebase_loaded, ::EnumSymbolsCallback, this);

	Output("%d total syscalls found\n\n", m_nCount);
}


void KImageModule::ShowSysCallTable(const char * tablename, unsigned base)
{
	const IMAGEHLP_SYMBOL * pSymbol = ImageGetSymbol(tablename);

	if ( pSymbol==NULL )
		Output("Unable to locate symbol %s\n", tablename);
	else
	{
		int count = base;

		const unsigned * p = (const unsigned *) Address2ImagePointer(pSymbol->Address);

		while ( ! IsBadReadPtr(p, sizeof(unsigned)) )
		{
			unsigned q = * p;

			if ( q > (unsigned) (m_image.FileHeader->OptionalHeader.ImageBase) )
			{
				DWORD           displacement;
			//	IMAGEHLP_SYMBOL symbol;

			    memset(m_is, 0, sizeof(m_is));
			    m_is[0].SizeOfStruct  = sizeof(IMAGEHLP_SYMBOL);
				m_is[0].MaxNameLength = sizeof(m_is) - sizeof(m_is[0]);
    
				unsigned q1 = q;

				// W2k RC1 imagehlp does not need translation
				if ( m_symbolbase )
					q1 = q1 - m_imagebase_default + m_symbolbase;

        		if ( SymGetSymFromAddr(m_hProcess, q1, & displacement, m_is) )
				{
					IMAGEHLP_LINE line;

					line.SizeOfStruct = sizeof(line);

					if ( SymGetLineFromAddr(m_hProcess, q1, & displacement, & line) )
					{
					}

					Output("syscall(%4x) %s\n", count, m_is[0].Name);
					count ++;
				}
			}
			else
				break;
			p++;
		}

		Output("%d system calls found\n", count - base);
	}
}


void KImageModule::va_Output(const char * format, va_list argptr)
{
    char buffer[1024];
    
    vsprintf(buffer, format, argptr);

	if ( m_hOutput )
	{
		SendMessage(m_hOutput, EM_SETSEL, 0xFFFFFF, 0xFFFFFF);
		SendMessage(m_hOutput, EM_REPLACESEL, 0, (LPARAM) buffer);
	}
	else
		OutputDebugString(buffer);
}


void KImageModule::Output(const char * format, ...)
{
    va_list ap;

	va_start(ap, format);
	va_Output(format, ap);
	va_end(ap);
}

