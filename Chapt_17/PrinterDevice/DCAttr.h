//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dcattr.h 	                                                         //
//  Description: KDCAttributes dialog box                                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KDCAttributes : public KDialog
{
	HDC		  m_hDC;
	HWND	  m_hWnd;

	KListView m_List;

	BOOL OnInitDialog(void);

	BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Add(LPCTSTR pszAttribute, LPCTSTR pszFormat, ...);

	void DumpDC(HDC hDC);

public:
	
	KDCAttributes(HDC hDC)
	{
		m_hDC = hDC;
	}
};