#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : pefile.h						                                     //
//  Description: PE file access                                                      //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

// PE File Access
class KPEFile
{
	char             * m_pBaseAddress;
	IMAGE_DOS_HEADER * m_pDOSHeader;
	IMAGE_NT_HEADERS * m_pNTHeader;

	void * RealAddress(unsigned rta)
	{
		if ( rta == 0)
			return NULL;
		else
			return m_pBaseAddress + rta;
	}

public:
	KPEFile(void)
	{
		m_pBaseAddress = NULL;
		m_pDOSHeader   = NULL;
		m_pNTHeader    = NULL;
	}

	bool Load(const TCHAR * filename);
	void Unload(void);

	virtual bool FuncEnumCallBack(const char * funcname)
	{
		return true;
	}

	virtual bool SectionEnumCallBack(IMAGE_SECTION_HEADER * pSection)
	{
		return true;
	}

	int  EnumExported(void);
	int  EnumSection(void);

	const char * GetSectionName(unsigned offset);
};

