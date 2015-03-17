//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : device.h					                                         //
//  Description: KDevice class, HTML/BMP driver, Chapter 2                           //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

typedef struct 
{
    DWORD        index;
	const char * name;
}	PAIR;

class KDevice
{
	int		 nNesting;	// document, page
    int      nPages;	// no of pages printed

	void	 Write(const char * pStr);
	void	 WriteW(const WCHAR * pwStr);
	void	 WriteHex(unsigned val);
	void	 Writeln(const char * pStr = NULL);

	void	 Write(DWORD index, const PAIR * pTable);

	char   * CopyBlock(char * pDest, void * pData, int size);
	void	 CopySurface(char * pDest, const SURFOBJ * pso);
	
	void	 LogCall(int index, const void * para, int parano);

public:
	int		 width;		// width in inch * 10
	int		 height;	// height in inch * 10	

	HPALETTE hPalette;	// GDI need a palette even for 24 bpp
	HSURF    hSurface;	// Device managed standard surface
    HDEV     hDevice;	// GDI device handle.
    HANDLE   hSpooler;  // Spooler handle
	
	int		 nImage;

public:

	void Create(void)
	{
		nNesting  = 0;
		nPages    = 0;
		nImage    = 0;
	}
	
	void	 DumpSurface(const SURFOBJ * psoBM);

	BOOL	 CallEngine(int index, const void * para, int parano);

	BOOL	 StartDoc(LPCWSTR pszDocName, const void * firstpara, int parano);
	BOOL	 EndDoc(const void * firstpara, int parano);
	BOOL	 StartPage(const void * firstpara, int parano);
	BOOL	 SendPage(const void * firstpara, int parano);
};

