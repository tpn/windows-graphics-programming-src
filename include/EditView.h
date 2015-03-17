#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : editview.h						                                     //
//  Description: EDIT window with Win32 data type decoding                           //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KEditView : public KScrollCanvas
{
	HWND		  m_hEdit;
	HINSTANCE	  m_hInstance;

	virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:

	KEditView(void)
	{
	}

	bool Initialize(HINSTANCE hInstance);
	void Decode(DWORD m_Data, DWORD m_Type);
};

