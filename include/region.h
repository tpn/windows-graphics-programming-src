#pragma

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : region.h						                                     //
//  Description: GDI region wrapper                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KRegion
{
public:
	int       m_nRegionSize;
	int       m_nRectCount;
	RECT *    m_pRect;
	RGNDATA * m_pRegion;

	KRegion()
	{
		m_nRegionSize = 0;
		m_nRectCount  = 0;
		m_pRegion     = NULL;
		m_pRect       = NULL;
	}

	void Reset(void)
	{
		if ( m_pRegion )
			delete [] (char *) m_pRegion;

		m_pRegion     = NULL;
		m_nRegionSize = 0;
		m_nRectCount  = 0;
		m_pRect       = NULL;
	}

	~KRegion()
	{
		Reset();
	}

	BOOL GetRegionData(HRGN hRgn);
	HRGN CreateRegion(XFORM * pXForm);
};


BOOL KRegion::GetRegionData(HRGN hRgn)
{
	Reset();

	m_nRegionSize = ::GetRegionData(hRgn, 0, NULL);
	if ( m_nRegionSize==0 )
		return FALSE;

	m_pRegion = (RGNDATA *) new char[m_nRegionSize];
	if ( m_pRegion==NULL )
		return FALSE;

	::GetRegionData(hRgn, m_nRegionSize, m_pRegion);

	m_nRectCount = m_pRegion->rdh.nCount;
	m_pRect      = (RECT *) & m_pRegion->Buffer;

	return TRUE;
}


HRGN KRegion::CreateRegion(XFORM * pXForm)
{
	return ExtCreateRegion(pXForm, m_nRegionSize, m_pRegion);
}

