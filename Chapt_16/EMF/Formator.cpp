//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : formator.cpp						                                 //
//  Description: Text output stream with Win32 data type support                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <fstream.h>

#include "OutPipe.h"
#include "Formator.h"


void KWinPipe::AddColor(COLORREF color)
{
	if ( color & 0xFF000000 )
		WriteHex(color);
	else
	{
		Write("RGB("); WriteHex(GetRValue(color));
		Put(',');      WriteHex(GetGValue(color));
		Put(',');      WriteHex(GetBValue(color));
		Put(')');
	}
}


const DicItem Dic_HatchStyles[] =
{
	pair(HS_BDIAGONAL), 
	pair(HS_CROSS), 
	pair(HS_DIAGCROSS), 
	pair(HS_FDIAGONAL), 
	pair(HS_HORIZONTAL), 
	pair(HS_VERTICAL),
	dummyitem
};
 

const DicItem Dic_BrushStyle [] =
{
	pair(BS_SOLID),
	pair(BS_NULL),
	pair(BS_HOLLOW),
	pair(BS_HATCHED),
	pair(BS_PATTERN),
	pair(BS_INDEXED),
	pair(BS_DIBPATTERN),
	pair(BS_DIBPATTERNPT),
	pair(BS_PATTERN8X8),
	pair(BS_DIBPATTERN8X8),
	pair(BS_MONOPATTERN),
	dummyitem
};


// EMRCREATEBRUSHINDIRECT: BS_SOLID, BS_HOLLOW, BS_NULL, or BS_HATCHED value. 
void KWinPipe::AddBrush(const LOGBRUSH * logbrush, bool create)
{
	assert(logbrush);

	if ( create )
		switch ( logbrush->lbStyle )
		{
			case BS_SOLID:
				if ( logbrush->lbColor == 0 )
					Write("GetStockObject(BLACK_BRUSH)");
				else if ( logbrush->lbColor == RGB(0xFF, 0xFF, 0xFF) )
					Write("GetStockObject(WHITE_BRUSH)");
				else
					Format(_T("CreateSolidBrush(#c)"), (const long *) & logbrush->lbColor);
				break;

			case BS_NULL:
				Write("GetStockObject(NULL_BRUSH)");
				break;

			case BS_HATCHED:
				Write("CreateHatchBrush(");
				Write(logbrush->lbHatch, Dic_HatchStyles);
				Format(_T(", #c)"), (const long *) & logbrush->lbColor);
				break;

			default:
				assert(false);
		}
	else
	{
		Format(_T("{ %L, #c"), logbrush, Dic_BrushStyle);
		Format(_T(",%L}"), & logbrush->lbHatch, Dic_HatchStyles);
	}
}


void KWinPipe::AddRegion(unsigned long size, const RGNDATA * rgn)
{
	assert(rgn);

	if ( rgn->rdh.nCount == 1 )
		Format(_T("hRegion = CreateRectRgn(%d,%d,%d,%d);"), rgn->Buffer);
	else
	{
		assert( rgn->rdh.nCount != 0);

		Format(_T("const RECT Rgn_%n[] = "), NULL);
		
		WriteArray(rgn, rgn->rdh.nCount + 2, sizeof(RECT));
		Write(";");

		Newline();
		
		long size = sizeof(RGNDATAHEADER) + rgn->rdh.nCount * 4;
		Format(_T("hRegion = ExtCreateRegion(NULL, %d, (const RGNDATA *) Rgn_%n);"), & size);
	}
}

	
const DicItem Dic_StockObjects[] =
{
	pair(WHITE_BRUSH),
	pair(LTGRAY_BRUSH),
	pair(GRAY_BRUSH),
	pair(DKGRAY_BRUSH),
	pair(BLACK_BRUSH),
	pair(NULL_BRUSH),
	pair(HOLLOW_BRUSH),
	pair(WHITE_PEN),
	pair(BLACK_PEN),
	pair(NULL_PEN),
	pair(OEM_FIXED_FONT),
	pair(ANSI_FIXED_FONT),
	pair(ANSI_VAR_FONT),
	pair(SYSTEM_FONT),
	pair(DEVICE_DEFAULT_FONT),
	pair(DEFAULT_PALETTE),
	pair(SYSTEM_FIXED_FONT),
	pair(DEFAULT_GUI_FONT),
    dummyitem
};


const long * KWinPipe::Pound(char tag, const long * data, const DicItem * dic)
{
	long len = 0;

	switch ( tag )
	{
		case 'B': Write("(const BITMAPINFO *) & BMI_"); WriteDec(m_curBMI); // BITMAPINFO reference
				  break;

		case 'b': Write("Bits_"); WriteDec(m_curDIB);  // DIB bits reference
				  break;
	
		case 'C': AddBrush((const LOGBRUSH *) data, true);  len=sizeof(LOGBRUSH); break;
		case 'R': AddBrush((const LOGBRUSH *) data, false); len=sizeof(LOGBRUSH); break;

		case 'c': AddColor(data[0]); len=0; 	   // colorref	
				  break;

		case 'o': if ( * data > 0 )			  // decimal object index or stock object, move
				  {
					  Write("hObj[");
					  WriteDec(data[0]);
					  Put(']');
				  }
				  else
				  {
					  Write("GetStockObject(");
					  Write(data[0] & 0xFFFF, Dic_StockObjects);
					  Put(')');
				  }	
				  len = 4;
  				  break;

		case 'p': WriteArray(& data[5], data[4]*2, sizeof(short)); break;
		case 'P': WriteArray(& data[5], data[4]*2, sizeof(long));  break;

		default: assert(false);
	}

	return (const long *) ( (const char *) data + len);
}


bool KWinPipe::SaveDIB(const BITMAPINFO * bmi, long bmisize, const void * bits, long bitsize)
{
	assert( (unsigned) bmi + bmisize == (unsigned) bits);

	if ( m_PackedDIBCache.Match(bmisize+bitsize, bmi, m_curPackedDIB) )
		return false;

	ofstream dib;

	TCHAR name[MAX_PATH];

	wsprintf(name, _T("c:\\dib_%d.bmp"), m_curPackedDIB+1);
#ifndef _UNICODE
	dib.open(name);
#endif
	dib.setmode(filebuf::binary);
	
	BITMAPFILEHEADER header;
	memset(& header, 0, sizeof(header));
	header.bfType    = 0x4D42;
	header.bfSize    = sizeof(header) + bmisize + bitsize;
	header.bfOffBits = sizeof(header) + bmisize;

	dib.write((const char *)& header, sizeof(header));
	dib.write((const char *)bmi,    bmisize);
	dib.write((const char *)bits,    bitsize);
	dib.close();

	wsprintf(name, _T("static KDIB Dib_%d; Dib_%d.Load(IDB_BITMAP%d);"),
		m_curPackedDIB+1, m_curPackedDIB+1, m_curPackedDIB+1);

	Write(name);

	return true;
}


bool KWinPipe::AddDIB(const void * header, 
					   long bmioffset, long bmisize,
				       long bitoffset, long bitsize,
					   TCHAR format[], bool pack)
{
	const BITMAPINFO * bmi = (const BITMAPINFO *) ((const char *) header + bmioffset);
	const char       * bit = (const char *) header + bitoffset;

	wsprintf(format, _T("%dx%dx%d"), bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight, 
		bmi->bmiHeader.biBitCount);
	
	if ( m_bOpened )
		return SaveDIB(bmi, bmisize, bit, bitsize);
	else
		return false;
/*
	if ( ( bitsize > 1024 ) || pack ) // write to external file
	{
		SaveDIB(bmi, bmisize, bit, bitsize);
		return;
	}
	
	// bmisize may be too big sometimes
	int palettesize = ( bmisize - sizeof(BITMAPINFOHEADER) ) / sizeof(bmi->bmiColors[0]);

	if ( palettesize > (1 << bmi->bmiHeader.biBitCount) )
		palettesize = 1 << bmi->bmiHeader.biBitCount;

	if ( pack || ! m_BMICache.Match(bmisize, bmi, m_curBMI) )
	{
		Newline();

		if ( pack )
		{
			m_curBMI ++;
			Write("const DIB_");
			WriteDec((long) ( 1 << bmi->bmiHeader.biBitCount) );
		}
		else if ( palettesize<=1 )
			Write("const BITMAPINFO");
		else
		{
			Write("const BITMAPINFO");
			WriteDec((long) palettesize);
		}

		Write(" BMI_"); 
		WriteDec(m_curBMI);
		
		Format(" = { { %d, %d,%d,%D,%D,%d,%d,%d,%d,%d,%d }", bmi);

		if ( bmisize > sizeof(BITMAPINFOHEADER) )
		{
			Write(", { ");
			
			for (int i=0; i<palettesize; i++)
			{
				if (i)
					Put(',');

				WriteArray(& bmi->bmiColors[i], 4, 1);
			}
			Put('}');
		}

		if ( pack )
			Put(' ');
		else
			Write(" };");
	}

	if ( pack || ! m_DIBCache.Match(bitsize, bit, m_curDIB) )
	{
		if (pack)
			m_curDIB++;
		else
		{
			Newline();

			Format("const unsigned long Bits_%d[]=", & m_curDIB);
		}

		WriteArray((const char *) header + bitoffset, bitsize/4, 4, false);
		Put(';');
	}
*/
}
