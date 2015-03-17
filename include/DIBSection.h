#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dibsection.h	                                                     //
//  Description: DIB section wrapper, EMF rendering, air brush                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include "Dib.h"
#include "DDB.h"

class KDIBSection : public KDIB, public KDDB
{
public:
	KDIBSection()
	{
	}

	virtual ~KDIBSection()
	{
	}

	BOOL CreateDIBSection(HDC hDC, CONST BITMAPINFO * pBMI, UINT iUsage, HANDLE hSection, DWORD dwOffset);
	UINT GetColorTable(void);
	UINT SetColorTable(void);

	void DecodeDIBSectionFormat(TCHAR desp[]);
};

HBITMAP LargestDIBSection(BITMAPINFO * pBMI);

void	Frame(HDC hDC, int nFrame, COLORREF crFrame, int left, int top, int right, int bottom);

BOOL	SaveWindow(HWND hWnd, bool bClient, int nFrame, COLORREF crFrame);


// Targa 24-bpp Image using DIB Section
class KTarga24 : public KDIBSection
{
#pragma pack(push,1)

	typedef struct	{
		BYTE IDLength;	  // 00: length of Identifier string
		BYTE CoMapType;	  // 01  0 = no map  
		BYTE ImgType;	  // 02  2 = TGA_RGB
		WORD Index;		  // 03  index of first color map entry
		WORD Length;	  // 05	 number of entries in color map
		BYTE CoSize;	  // 07  size of color map entry 	
		WORD X_Org;       // 08  0
		WORD Y_Org;       // 0A  0
		WORD Width;       // 0C  width
		WORD Height;      // 0E  height
		BYTE PixelSize;   // 10  pixel size
		BYTE AttBits;     // 11  0
		char ID[14];	  // 12  space filler to make sure ImageHeader is DWORD aligned
	}	ImageHeader;
#pragma pack(pop)
	
	HANDLE      m_hFile;
	HANDLE      m_hFileMapping;

public:
	KTarga24();
	virtual ~KTarga24();

	BOOL Create(int width, int height, const TCHAR * filename);
};

// Render EMF using memory-mapped Targa file as DIB section
BOOL RenderEMF(HENHMETAFILE hemf, int width, int height, const TCHAR * tgaFileName);


// Using alpha channel to implement non-solid brush
class KAirBrush
{
	HBITMAP m_hBrush;
	HDC		m_hMemDC;
	HBITMAP m_hOld;
	int		m_nWidth;
	int	    m_nHeight;

	void Release(void)
	{
		SelectObject(m_hMemDC, m_hOld);
		DeleteObject(m_hMemDC);
		DeleteObject(m_hBrush);

		m_hOld = NULL; m_hMemDC = NULL; m_hBrush = NULL;
	}

public:
	KAirBrush()
	{
		m_hBrush = NULL;
		m_hMemDC = NULL;
		m_hOld   = NULL;
	}

	~KAirBrush()
	{
		Release();
	}

	void Create(int width, int height, COLORREF color);
	void Apply(HDC hDC, int x, int y);
};


