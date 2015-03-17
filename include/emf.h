#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : emf.h					                                             //
//  Description: Enhanced metafile handling                                          //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KEnumEMF
{
	// virtual function to process every EMF record, return 0 to terminate
	virtual int ProcessRecord(HDC hDC, HANDLETABLE * pHTable, const ENHMETARECORD * pEMFR, int nObj)
	{
		return 0;
	}

	// typedef int (CALLBACK* ENHMFENUMPROC)(HDC, HANDLETABLE FAR*, CONST ENHMETARECORD *, int, LPARAM);
	
	// static callback function, dispatch to virtual function ProcessRecord
	static int CALLBACK EMFProc(HDC hDC, HANDLETABLE * pHTable, 
		const ENHMETARECORD * pEMFR, int nObj, LPARAM lpData)
	{
		KEnumEMF * pObj = (KEnumEMF *) lpData;

		if ( IsBadWritePtr(pObj, sizeof(KEnumEMF)) )
		{
			assert(false);
			return 0;
		}

		return pObj->ProcessRecord(hDC, pHTable, pEMFR, nObj);
	}

public:
	
	BOOL EnumEMF(HDC hDC, HENHMETAFILE hemf, const RECT * lpRect)
	{
		return ::EnumEnhMetaFile(hDC, hemf, EMFProc, this, lpRect);
	}
};


// map a rectangle in logical coordinate to 0.01 mm units
void Map10um(HDC hDC, RECT & rect);

// load EMF from resource
HENHMETAFILE LoadEMF(HMODULE hModule, LPCTSTR pName);

// get EMF dimension
void GetEMFDimension(HDC hDC, HENHMETAFILE hEmf, int & width, int & height);

// display EMF with a ratio
BOOL DisplayEMF(HDC hDC, HENHMETAFILE hEmf, int x, int y, int scalex, int scaley, 
				KEnumEMF * enumerator, bool showxform=false);

HDC QuerySaveEMFFile(const TCHAR * description, const RECT * rcFrame, TCHAR szFileName[]);

HENHMETAFILE QueryOpenEMFFile(TCHAR szFileName[]);

HENHMETAFILE PasteFromClipboard(HWND hWnd);
void CopyToClipboard(HWND hWnd, HENHMETAFILE hEmf);

HPALETTE GetEMFPalette(HENHMETAFILE hEmf, HDC hDC);

// Transformation to map recording device's device coordinate space to destination device's logical coordinate space
BOOL GetPlayTransformation(HENHMETAFILE hEmf, const RECT * rcPic, XFORM & xformPlay);


