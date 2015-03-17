#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : gditable.h						                                     //
//  Description: NT/2000 GDI handle table access                                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

typedef struct
{
	void          *  pKernel;
	unsigned short  _nProcess; // NT/200 switch order for _nProcess, _nCount
	unsigned short  _nCount;
	unsigned short   nUpper;
	unsigned short   nType;
	void          *  pUser;
} GDITableCell;


// GDI Object Table
class KGDITable
{
	int			   m_nVersion;
	
	GDITableCell * pGDITable;

public:

	bool IsGDITableAccessible(void) const
	{
		return pGDITable != NULL;
	}

	KGDITable();

	GDITableCell operator[](HGDIOBJ hHandle) const
	{
		return pGDITable[ (unsigned) hHandle & 0xFFFF ];
	}

	GDITableCell operator[](unsigned nIndex) const
	{
		return pGDITable[ nIndex & 0xFFFF ];
	}

	unsigned short GetProcess(unsigned nIndex) const
	{
		const GDITableCell & cell = pGDITable[nIndex & 0xFFFF];

		if ( m_nVersion==5 )
			return cell._nProcess;
		else
			return cell._nCount;
	}

	unsigned short GetCount(unsigned nIndex) const
	{
		const GDITableCell & cell = pGDITable[nIndex & 0xFFFF];

		if ( m_nVersion==5 )
			return cell._nCount;
		else
			return cell._nProcess;
	}

	unsigned short GetProcess(HGDIOBJ nIndex) const
	{
		const GDITableCell & cell = pGDITable[(DWORD) nIndex & 0xFFFF];

		if ( m_nVersion==5 )
			return cell._nProcess;
		else
			return cell._nCount;
	}

	unsigned short GetCount(HGDIOBJ nIndex) const
	{
		const GDITableCell & cell = pGDITable[(DWORD) nIndex & 0xFFFF];

		if ( m_nVersion==5 )
			return cell._nCount;
		else
			return cell._nProcess;
	}

};
