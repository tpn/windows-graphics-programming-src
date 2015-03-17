#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : imagemodule.h				                                         //
//  Description: Using ImageHlp API for debug symbol querying                        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

// Debug symbol handling for a single module
class KImageModule  
{
	HANDLE                   m_hProcess;   
    
	bool					 m_bLoaded;
	DWORD					 m_imagebase_default; // default image base address in DLL/SYS
	DWORD					 m_imagebase_loaded;  // actual loaded image base
	DWORD					 m_symbolbase;		  // base address for loaded symbol	

	TCHAR					 m_modulename[64];

    IMAGEHLP_SYMBOL          m_is[10];    // leave enough space for the name
	HWND					 m_hOutput;
	LOADED_IMAGE			 m_image;

	bool					 m_bDecoratedNames;
	bool					 m_bShowFPO;
	int						 m_nCount;
	bool					 m_bForPogy;	// Chapter 4: APY Spy

public:
	KImageModule(HWND hOutput);
	virtual ~KImageModule();

    bool  Load(char * filename, char * sympath);
    void  Unload(void);

    const IMAGEHLP_SYMBOL * ImageGetSymbol(const char * name);
	bool  LoadSystemModule(char * module, char * extension);
	
	BOOL  EnumSymbolsCallback(LPSTR SymbolName, ULONG SymbolAddress, ULONG SymbolSize) ;

	void  ShowFPO(ULONG SymbolAddress);
	void  TranslateName(LPSTR SymbolName);

	const unsigned char * GetImagePointer(unsigned symbva);
	const unsigned char * Address2ImagePointer(unsigned addr);

	void  EnumerateSymbols(bool bForPogy);
	void  ShowSysCallTable(const char * tablename, unsigned base);

	void  va_Output(const char * format, va_list argptr);
	void  Output(const char * format, ...);

};

