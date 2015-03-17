//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : octree.cpp						                                     //
//  Description: Octree color quantilization, color reduction                        //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <assert.h>

#include "DIB.h"
#include "Image.h"
#include "Octree.h"

/////////////////////////////////////////////////
//											   //
//	O C T R E E  Q U A N T I L I Z A T I O N   //
//										       //	
/////////////////////////////////////////////////

KNode::RemoveAll(void)
{
	for (int i=0; i<8; i++)
		if ( Child[i] )
		{	
			Child[i]->RemoveAll();
			Child[i] = NULL;
		}
	
	delete this;
}


int KNode::PickLeaves(RGBQUAD * pEntry, int * pFreq, int size)
{
	if ( size==0 )
		return 0;

	if ( IsLeaf )
	{
		* pFreq             = Pixels;

		pEntry->rgbRed      = ( SigmaRed   + Pixels/2 ) / Pixels;
		pEntry->rgbGreen    = ( SigmaGreen + Pixels/2 ) / Pixels;
		pEntry->rgbBlue     = ( SigmaBlue  + Pixels/2 ) / Pixels;
		pEntry->rgbReserved = 0;

		return 1;
	}
	else
	{
		int sum = 0;
	
		for (int i=0; i<8; i++)
			if ( Child[i] )
				sum += Child[i]->PickLeaves(pEntry+sum, pFreq+sum, size-sum);

		return sum;
	}
}


void KOctree::AddColor (BYTE r, BYTE g, BYTE b)
{
	KNode * pNode = pRoot;

	for (BYTE mask=0x80; mask!=0; mask>>=1) // follow the path until leaf node
	{
		// add pixel to it
		pNode->Pixels ++;
		pNode->SigmaRed   += r;
		pNode->SigmaGreen += g;
		pNode->SigmaBlue  += b;

		if ( pNode->IsLeaf )
			break;

		// take one bit each of RGB to form an index
		int index = ( (r & mask) ? 4 : 0 ) + ( (g & mask) ? 2 : 0 ) + ( (b & mask) ? 1 : 0 );
		
		// create a new node if it's a new branch
		if ( pNode->Child[index]==NULL )
		{
			pNode->Child[index] = new KNode(mask==2);
			TotalNode ++;

			if ( mask==2 )
				TotalLeaf ++;
		}

		// follow the path
		pNode = pNode->Child[index];
	}

	for (int threshold=1; TotalNode>MAXMODE; threshold++ )
		Reduce(pRoot, threshold);
}


// Combine node with leaf only child nodes, and no more than threshold pixels
// Combine leaf node with no more than threshold pixels, into closest sibling
void KOctree::Reduce(KNode * pTree, unsigned threshold)
{
	if ( pTree==NULL )
		return;

	bool childallleaf = true;

	// recursively call all non-leaf child nodes
	for (int i=0; i<8; i++)
		if ( pTree->Child[i] && ! pTree->Child[i]->IsLeaf )
		{
			Reduce(pTree->Child[i], threshold);

			if ( ! pTree->Child[i]->IsLeaf )
				childallleaf = false;
		}

	// if all children are leaves, combined is not big enough, combine them
	if ( childallleaf & (pTree->Pixels<=threshold) )
	{
		for (int i=0; i<8; i++)
			if ( pTree->Child[i] )
			{
				delete pTree->Child[i];
				pTree->Child[i] = NULL;
				TotalNode --;
				TotalLeaf --;
			}
	
		pTree->IsLeaf = true;
		TotalLeaf ++;

		return;
	}

	// merge small child leaf nodes
	for (i=0; i<8; i++)
		if ( pTree->Child[i] && pTree->Child[i]->IsLeaf && (pTree->Child[i]->Pixels<=threshold) )
		{
			KNode temp = * pTree->Child[i];

			delete pTree->Child[i];
			pTree->Child[i] = NULL;
			TotalNode --;
			TotalLeaf --;
		
			for (int j=0; j<8; j++)
				if ( pTree->Child[j] )
				{
					Merge(pTree->Child[j], temp);
					break;
				}
		}
}


void KOctree::Merge(KNode * pNode, KNode & target)
{
	while ( true )
	{
		pNode->Pixels     += target.Pixels;
		pNode->SigmaRed   += target.SigmaRed;
		pNode->SigmaGreen += target.SigmaGreen;
		pNode->SigmaBlue  += target.SigmaBlue;

		if ( pNode->IsLeaf )
			break;

		KNode * pChild = NULL;

		for (int i=0; i<8; i++)
			if ( pNode->Child[i] )
			{
				pChild = pNode->Child[i];
				break;
			}

		if ( pChild==NULL )
		{
			assert(FALSE);
			return;
		}
		else
			pNode = pChild;
	}
}


void KOctree::ReduceLeaves(int limit)
{
	for (unsigned threshold=1; TotalLeaf>limit; threshold++)
		Reduce(pRoot, threshold);
}


int KOctree::GenPalette(RGBQUAD entry[], int * pFreq, int size)
{
	ReduceLeaves(size);
	
	return pRoot->PickLeaves(entry, pFreq, size);
}


int GenPalette(BITMAPINFO * pDIB, RGBQUAD * pEntry, int * pFreq, int size)
{
	KImage		dib;
	KPaletteGen palgen;

	dib.AttachDIB(pDIB, NULL, 0);

	palgen.AddBitmap(dib);
	
	return palgen.GetPalette(pEntry, pFreq, size);
}


BITMAPINFO * KColorReduction::Convert8bpp(BITMAPINFO * pDIB)
{
	m_nBPS     = (pDIB->bmiHeader.biWidth + 3) / 4 * 4;	// scanline size for 8-bpp DIB
	
	int headsize = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

	BITMAPINFO * pNewDIB = (BITMAPINFO *) new BYTE[headsize + m_nBPS * abs(
		pDIB->bmiHeader.biHeight)];

	memset(pNewDIB, 0, headsize);

	pNewDIB->bmiHeader.biSize		 = sizeof(BITMAPINFOHEADER); 
	pNewDIB->bmiHeader.biWidth		 = pDIB->bmiHeader.biWidth; 
	pNewDIB->bmiHeader.biHeight		 = pDIB->bmiHeader.biHeight;
	pNewDIB->bmiHeader.biPlanes		 = 1; 
	pNewDIB->bmiHeader.biBitCount	 = 8; 
	pNewDIB->bmiHeader.biCompression = BI_RGB; 

	memset(pNewDIB->bmiColors, 0, 256 * sizeof(RGBQUAD));

	int freq[236];

	m_Matcher.Setup(GenPalette(pDIB, pNewDIB->bmiColors, freq, 236), pNewDIB->bmiColors);

	m_pBits  = (BYTE*) & pNewDIB->bmiColors[256];
		
	if ( pNewDIB==NULL )
		return NULL;

	KImage dib;

	dib.AttachDIB(pDIB, NULL, 0);

	DWORD t1 = GetTickCount();

	dib.PixelTransform(* this);
		
	TCHAR temp[32];

	wsprintf(temp, _T("Time %d"), GetTickCount() - t1);
	MessageBox(NULL, temp, _T("Convert8bpp"), MB_OK);

	return pNewDIB;
}


inline void ForwardDistribute(int error, int * curerror, int & nexterror)
{
	if ( (error<-2) || (error>2) ) // -2..2, not big enough
	{
		nexterror    = curerror[1] + error * 7 / 16;
	
		curerror[-1] += error * 3 / 16;		//            X   7/16
		curerror[ 0] += error * 5 / 16;     //    3/16  5/16  1/16
		curerror[ 1] += error     / 16;
	}
	else
		nexterror = curerror[1];
}

inline void BackwardDistribute(int error, int * curerror, int & nexterror)
{
	if ( (error<-2) || (error>2) ) // -2..2, not big enough
	{
		nexterror    = curerror[-1] + error * 7 / 16;
	
		curerror[ 1] += error * 3 / 16;		//    7/16   X  
		curerror[ 0] += error * 5 / 16;     //    1/16  5/16  3/16
		curerror[-1] += error     / 16;
	}
	else
		nexterror = curerror[-1];
}



BITMAPINFO * KErrorDiffusionColorReduction::Convert8bpp(BITMAPINFO * pDIB)
{
	int extwidth = pDIB->bmiHeader.biWidth + 2;

	int * error   = new int[extwidth*3];
	memset(error, 0, sizeof(int) * extwidth * 3);

	red_error   = error + 1;
	green_error = red_error   + extwidth;
	blue_error  = green_error + extwidth;

	BITMAPINFO * pNew = KColorReduction::Convert8bpp(pDIB);

	delete [] error;

	return pNew;
}


void KErrorDiffusionColorReduction::Map24bpp(BYTE * pBuffer, int width)
{
	int next_red, next_green, next_blue;

	if ( m_bForward )
	{
		next_red   =   red_error[0];
		next_green = green_error[0];
		next_blue  =  blue_error[0];

		for (int i=0; i<width; i++)
		{
			int red   = pBuffer[2];
			int green = pBuffer[1];
			int blue  = pBuffer[0];

			BYTE match = m_Matcher.ColorMatch( red+next_red, green+next_green, blue+next_blue );
			
			ForwardDistribute(red   - m_Matcher.m_Colors[match].rgbRed  , red_error  +i, next_red);
			ForwardDistribute(green - m_Matcher.m_Colors[match].rgbGreen, green_error+i, next_green);
			ForwardDistribute(blue  - m_Matcher.m_Colors[match].rgbBlue,  blue_error +i, next_blue);
			
			* m_pPixel ++= match;

			pBuffer += 3;
		}
	}
	else
	{
		next_red   =   red_error[width-1];
		next_green = green_error[width-1];
		next_blue  =  blue_error[width-1];

		pBuffer  += 3 * width - 3;
		m_pPixel += width - 1;

		for (int i=width-1; i>=0; i--)
		{
			int red   = pBuffer[2];
			int green = pBuffer[1];
			int blue  = pBuffer[0];

			BYTE match = m_Matcher.ColorMatch( red+next_red, green+next_green, blue+next_blue );
			
			BackwardDistribute(red   - m_Matcher.m_Colors[match].rgbRed  , red_error  +i, next_red);
			BackwardDistribute(green - m_Matcher.m_Colors[match].rgbGreen, green_error+i, next_green);
			BackwardDistribute(blue  - m_Matcher.m_Colors[match].rgbBlue,  blue_error +i, next_blue);
			
			* m_pPixel --= match;

			pBuffer -= 3;
		}
	}		
}


BITMAPINFO * Convert8bpp(BITMAPINFO * pDIB)
{
	KColorReduction cnv;

	return cnv.Convert8bpp(pDIB);
}

BITMAPINFO * Convert8bpp_ErrorDiffusion(BITMAPINFO * pDIB)
{
	KErrorDiffusionColorReduction cnv;

	return cnv.Convert8bpp(pDIB);
}


// Get to DIB color table
const BYTE * GetColorTable(const BITMAPINFO * pDIB, int & nSize, int & nColor)
{
	const BYTE * pRGB;

	if ( pDIB->bmiHeader.biSize==sizeof(BITMAPCOREHEADER) )	// OS/2 BMP format
	{
		pRGB   = (const BYTE *) pDIB + sizeof(BITMAPCOREHEADER);
		nSize  = sizeof(RGBTRIPLE);
		nColor = 1 << ((BITMAPCOREHEADER *) pDIB)->bcBitCount;
	}
	else 
	{
		nColor = 0;

		if ( pDIB->bmiHeader.biBitCount<=8 )
			nColor = 1 << pDIB->bmiHeader.biBitCount;

		if ( pDIB->bmiHeader.biClrUsed )
			nColor = pDIB->bmiHeader.biClrUsed;

		if ( pDIB->bmiHeader.biClrImportant )
			nColor = pDIB->bmiHeader.biClrImportant;

		pRGB  = (const BYTE *) & pDIB->bmiColors;
		nSize = sizeof(RGBQUAD);
		
		if ( pDIB->bmiHeader.biCompression==BI_BITFIELDS )
			pRGB += 3 * sizeof(RGBQUAD);
	}

	if ( nColor>256 )
		nColor = 256;

	if ( nColor==0 )
		return NULL;

	return pRGB;
}


HPALETTE LUTCreatePalette(const BYTE * pRGB, int nSize, int nColor)
{
	if ( (pRGB==NULL) || (nColor==0) )
		return NULL;

	LOGPALETTE * pLogPal = (LOGPALETTE *) new BYTE[sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * (nColor-1)];
	HPALETTE     hPal;

	if ( pLogPal )
	{
		pLogPal->palVersion    = 0x0300;
		pLogPal->palNumEntries = nColor;

		for (int i=0; i<nColor; i++)
		{
			pLogPal->palPalEntry[i].peBlue  = pRGB[0];
			pLogPal->palPalEntry[i].peGreen = pRGB[1];
			pLogPal->palPalEntry[i].peRed   = pRGB[2]; 
			pLogPal->palPalEntry[i].peFlags = 0; 

			pRGB += nSize;
		}

		hPal = CreatePalette(pLogPal);
	}

	delete [] (BYTE *) pLogPal;
	
	return hPal;
}


// Create palette from Color table in a DIB
HPALETTE CreateDIBPalette(const BITMAPINFO * pDIB)
{
	int	   nSize;
	int    nColor;

	const BYTE * pRGB = GetColorTable(pDIB, nSize, nColor);

	if ( pRGB==NULL )
	{
		RGBQUAD		 RGB[256];
		int			 freq[256];

		nColor = GenPalette((BITMAPINFO *) pDIB, RGB, freq, 236);

#ifdef _DEBUG
		for (int i=0; i<nColor; i++)
		{
			TCHAR temp[64];

			wsprintf(temp, _T("{ %d, %d, %d }, // %3d, %d\n"), RGB[i].rgbRed, RGB[i].rgbGreen, RGB[i].rgbBlue,
				i, freq[i]);
			OutputDebugString(temp);
		}
#endif

		return LUTCreatePalette((BYTE *) RGB, sizeof(RGBQUAD), nColor);
	}
	else
		return LUTCreatePalette(pRGB, nSize, nColor);
}


BITMAPINFO * IndexColorTable(BITMAPINFO * pDIB, HPALETTE hPal)
{
	int nSize;
	int nColor;

	const BYTE * pRGB = GetColorTable(pDIB, nSize, nColor);

	if ( pDIB->bmiHeader.biBitCount>8 )	// no change
		return pDIB;

	// allocate a new BITMAPINFO for modification
	BITMAPINFO * pNew = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*nColor];

	pNew->bmiHeader = pDIB->bmiHeader;

	WORD * pIndex = (WORD *) pNew->bmiColors;

	for (int i=0; i<nColor; i++, pRGB+=nSize)
		if ( hPal )
			pIndex[i] = GetNearestPaletteIndex(hPal, RGB(pRGB[2], pRGB[1], pRGB[0]));
		else
			pIndex[i] = i;

	return pNew;
}