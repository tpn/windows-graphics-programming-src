//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : pefile.cpp					                                         //
//  Description: Portable Exectuable file class                                      //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_AND_MEAN

#include <windows.h>
#include <assert.h>
#include "pefile.h"


bool KPEFile::Load(const TCHAR * filename)
{
	m_pBaseAddress = (char *) LoadLibrary(filename);
//	m_pBaseAddress = (char *) LoadLibraryEx(filename, NULL, LOAD_LIBRARY_AS_DATAFILE);

//	if ((unsigned) m_pBaseAddress & 1 )
//		m_pBaseAddress --;

	if (m_pBaseAddress==NULL)
		return false;

	m_pDOSHeader    = (PIMAGE_DOS_HEADER) m_pBaseAddress;
    if ( IsBadReadPtr(m_pBaseAddress, sizeof(IMAGE_DOS_HEADER)) )
        return false;
    if ( m_pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE )
        return false;

	m_pNTHeader = (IMAGE_NT_HEADERS *) RealAddress(m_pDOSHeader->e_lfanew);
	return  m_pNTHeader->Signature == IMAGE_NT_SIGNATURE;
}


void KPEFile::Unload(void)
{
	if (m_pBaseAddress)
	{
		FreeLibrary((HINSTANCE) m_pBaseAddress);
		m_pBaseAddress = NULL;
	}
}


int KPEFile::EnumExported(void)
{
	IMAGE_EXPORT_DIRECTORY * pExportDir;
	
	pExportDir = (IMAGE_EXPORT_DIRECTORY *) RealAddress(
	                m_pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	if (pExportDir == NULL)
		return 0;
	
	DWORD * namerva = (DWORD *) RealAddress((unsigned) pExportDir->AddressOfNames);
	if (namerva == NULL)
		return 0;

	int count = 0;
	
	for (unsigned i=0; i<pExportDir->NumberOfNames; i++)
	{
		const char * pName = (const char *) RealAddress(namerva[i]);

		if (pName)
		{
			if ( !FuncEnumCallBack(pName) )
				break;

			count ++;
		}
	}

	return count;
}


int KPEFile::EnumSection(void)
{
	int sectionno                    = m_pNTHeader->FileHeader.NumberOfSections;
	IMAGE_SECTION_HEADER * pSections = IMAGE_FIRST_SECTION(m_pNTHeader);

	for (int i=0; i<sectionno; i++)
	{
		if ( !SectionEnumCallBack(pSections + i) )
			break;
	}
		
	return i;
}


const char * KPEFile::GetSectionName(unsigned offset)
{
	int sectionno                    = m_pNTHeader->FileHeader.NumberOfSections;
	IMAGE_SECTION_HEADER * pSections = IMAGE_FIRST_SECTION(m_pNTHeader);

	for (int i=0; i<sectionno; i++)
		if ( pSections[i].VirtualAddress == offset )
			return (const char *) pSections[i].Name;

	return NULL;
}