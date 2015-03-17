//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : snapshot.cpp				                                         //
//  Description: Virtual memory snapshot                                             //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <stdio.h>

#include "listview.h"
#include "dialog.h"
#include "pefile.h"

#include "Snapshot.h"
#include "Resource.h"
#include "MemView.h"

typedef struct
{
    unsigned id;
    const char * key;
} Word;


void AddFlags(char * rslt, unsigned value, const Word * dic, int no);


const Word Protections[] = 
{
    { PAGE_READONLY,          "ro  " },
    { PAGE_READWRITE,         "rw  " }, 
    { PAGE_WRITECOPY,         "wc  " }, 
    { PAGE_EXECUTE,           "e   " }, 
    { PAGE_EXECUTE_READ,      "er  " }, 
    { PAGE_EXECUTE_READWRITE, "erw " },
    { PAGE_EXECUTE_WRITECOPY, "ewc " }, 
    { PAGE_GUARD,             "grd " }, 
    { PAGE_NOACCESS,          "nac " }, 
    { PAGE_NOCACHE,           "ncc " }
};    


void AddFlags(char * rslt, unsigned value, const Word * dic, int no)
{
    for (int i=0; i<no; i++)
        if ( value & dic[i].id )
            strcat(rslt, dic[i].key);
}


KRegion * KSnapShot::FindRegion(unsigned char * regionstart, unsigned regionsize)
{
	// search for a region with same start and size
	for (int r=0; r<m_nRegionNo; r++)
		if ( ( m_Regions[r].start == regionstart ) &&
		     ( m_Regions[r].size  == regionsize ) )
			 return & m_Regions[r];

	// add a new region
	if ( m_nRegionNo < nMaxRegionNo )
	{
		m_Regions[m_nRegionNo].start = regionstart;
		m_Regions[m_nRegionNo].size  = regionsize;
		m_Regions[m_nRegionNo].count = 0;

		return & m_Regions[m_nRegionNo++];
	}
	else
		return NULL;
}


void KSnapShot::Shot(KListView * list)
{
    unsigned char * start = NULL;
    MEMORY_BASIC_INFORMATION info;

	KPEFile  module;
	void   * lastmodule = NULL;
	
	typedef enum { nMaxHeaps = 10 };
	HANDLE   ProcHeaps[nMaxHeaps];

	int heaps = GetProcessHeaps(nMaxHeaps, ProcHeaps);

    while ( VirtualQuery(start, & info, sizeof(info)) )
    {
		KRegion * pRegion = NULL;

		// compute CRC for committed region
	    if (info.State == MEM_COMMIT)
		{
			pRegion = FindRegion(start, info.RegionSize);		
            if (pRegion)
            {
        		pRegion->type    = info.Type;
				pRegion->lastcrc = pRegion->crc;
                pRegion->crc     = m_crc.Update(0, start, info.RegionSize);
				pRegion->count ++;
            }
		}
		
		if (list)
		{
			TCHAR temp[MAX_PATH];
			const TCHAR * p ;

			if ( pRegion && pRegion->count>=2 )
			{
				wsprintf(temp, "%04x", pRegion->lastcrc);
				list->AddItem(0, temp, ( pRegion->lastcrc != pRegion->crc ) + 1);
			}
			else
				list->AddItem(0, "     ", 0);

			if ( pRegion )
			{
				wsprintf(temp, "%04x", pRegion->crc);
				list->AddItem(1, temp);
			}
			
			wsprintf(temp, "%08lx", info.BaseAddress);	list->AddItem(2, temp);
			wsprintf(temp, "%08lx", info.RegionSize);   list->AddItem(3, temp);

			switch (info.State)
			{
				case MEM_FREE:    p = "F "; break;
				case MEM_RESERVE: p = "R "; break;
				case MEM_COMMIT:  p = "C "; break;
				default:          p = "? ";   
			}
			strcpy(temp, p);

			if ( info.State != MEM_FREE)
			{
				switch (info.Type)
				{
					case MEM_IMAGE:   p = "I "; break;
					case MEM_MAPPED:  p = "M "; break;
					case MEM_PRIVATE: p = "P "; break;
					default:          p = "? ";
				}
				strcat(temp, p);

				AddFlags(temp, info.AllocationProtect, Protections, sizeof(Protections)/sizeof(Protections[0]));
			}
			list->AddItem(4, temp);

/*
			char t[MAX_PATH];
			wsprintf(t, "%8lx ", info.AllocationBase);
			strcat(temp, t);

			if (info.State != MEM_RESERVE)
				AddFlags(temp, info.Protect, Protections, sizeof(Protections)/sizeof(Protections[0]));
*/

			if (info.State != MEM_FREE )
				if ( GetModuleFileName((HINSTANCE) info.BaseAddress, temp, sizeof(temp)) )
				{
					if (lastmodule)
					{
						module.Unload();
						lastmodule = NULL;
					}
				
					if ( module.Load(temp) )
						lastmodule = info.BaseAddress;

					// remove the directory path, keep only the filename
					if ( strchr(temp, '\\') )
					{
						for (char *p = strchr(temp, '\\') + 1;
									   strchr(p, '\\');
								   p = strchr(p, '\\') + 1);
						list->AddItem(5, p);
					}
					else 
						list->AddItem(5, temp);
				}
				else
				{
					const char * p = NULL;

					if (lastmodule != NULL)
						p = module.GetSectionName((unsigned) info.BaseAddress - (unsigned) lastmodule);

					if (p == NULL)
						for (int h=0; h<heaps; h++)
							if ( info.BaseAddress == ProcHeaps[h] )
							{
								wsprintf(temp, "Heap %d", h+1);
								p = temp;
								break;
							}

					if (p == NULL)
						if ( ( (unsigned) (& p) >= (unsigned) info.BaseAddress ) &&
						     ( (unsigned) (& p) <  (unsigned) info.BaseAddress + info.RegionSize ) )
							p = "Stack";

					if ( p )
						list->AddItem(5, p);
				}
							
		}

        start += info.RegionSize;

        if (start == 0)
            break;
    }

	if (lastmodule)
		module.Unload();
}


void KSnapShot::ShowDetail(HINSTANCE hInst, unsigned start, unsigned size)
{
	KMemViewer memory(start, size);

	memory.Dialogbox(hInst, MAKEINTRESOURCE(IDD_MEMORY));
}


/*
void CSnapShot::Compare(CSnapShot & after)
{
	if (m_nRegionNo != after.m_nRegionNo)
		OutputDebugString("Region No different\n");

	for (int i=0; i<min(m_nRegionNo, after.m_nRegionNo); i++)
	{
		CRegion & r1 = m_Regions[i];
		CRegion & r2 = after.m_Regions[i];

		if ( ( r1.start != r2.start ) || ( r1.size  != r2.size  ) ||
		     ( r1.type  != r2.type  ) || ( r1.crc   != r2.crc   ) )
		{
			TCHAR temp[MAX_PATH];

			wsprintf(temp, "%3d { %8x %8x %8x %4x } -> { %8x %8x %8x %4x } \n",
							i, 
							r1.start, r1.size, r1.type, r1.crc,
							r2.start, r2.size, r2.type, r2.crc);
			OutputDebugString(temp);
		}			
	}
}
*/
