#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : outlinetextmetric.h			                                     //
//  Description: Wrapper for OUTLINETEXTMETRIC structure                             //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KOutlineTextMetric
{
public:
	OUTLINETEXTMETRIC * m_pOtm;

	KOutlineTextMetric(HDC hDC)
	{
		int size = GetOutlineTextMetrics(hDC, 0, NULL);  // query size

		if ( size )
		{
			m_pOtm = (OUTLINETEXTMETRIC *) new BYTE[size];   // allocation
			GetOutlineTextMetrics(hDC, size, m_pOtm);        // query data

			// make sure structure alignment is right, OUTLINETEXTMETIRC is 4-byte aligned
			assert( (((unsigned) & m_pOtm->otmFiller - (unsigned) m_pOtm) %4) == 0);
		}
		else
			m_pOtm = NULL;
	}

	~KOutlineTextMetric()
	{
		if ( m_pOtm )
		{
			delete [] (BYTE *) m_pOtm;
			m_pOtm = NULL;
		}
	}

	const TCHAR * GetName(PSTR nOffset) const
	{
		if ( m_pOtm )
			return (const TCHAR *) ((BYTE *) m_pOtm + (int) nOffset);
		else
			return NULL;
	}
};
