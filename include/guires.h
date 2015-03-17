#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : guires.h						                                     //
//  Description: Query USER/GDI resources, Windows 2000                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KGUIResource
{
	int m_gdi, m_user;

public:
	KGUIResource()
	{
		m_gdi  = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
		m_user = GetGuiResources(GetCurrentProcess(), GR_USEROBJECTS);
	}

	~KGUIResource()
	{
		int gdi  = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
		int user = GetGuiResources(GetCurrentProcess(), GR_USEROBJECTS);

		if ( (m_gdi==gdi) && (m_user==user) )
			return;
		
		char temp[64];
		wsprintf(temp, "ResourceDifference: gdi(%d->%d) user(%d->%d)\n", m_gdi, gdi, m_user, user);
		OutputDebugString(temp);
	}
};
				
