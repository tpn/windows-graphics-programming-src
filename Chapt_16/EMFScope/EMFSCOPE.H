//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : emfscope.h							                                 //
//  Description: EMFScope main window		                                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//


class KCanvasWindow;
class KToolBar;

class KEmfScope : public KWindow
{
	LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		
	int default_w;
	int default_h;
	int extra_w;
	int extra_h;
				
public:
	KCanvasWindow  *Canvas;
	KToolBar       *ToolBar;
		
	HWND            hwnd_Spooler;
	int             hwnd_Spehon32;

	HINSTANCE       hinst_EmfScope;

	HMENU			hMainMenu;

	KEmfScope()
	{
		default_w = 200;
		default_h = 200;
			
		extra_w   = 20;
		extra_h   = 20;

		Canvas    = NULL;
		ToolBar   = NULL;
		hMainMenu = NULL;
	}

	~KEmfScope();

	HWND CreateMainWindow(HINSTANCE hInst);
	
	void LinkClientServer(void);
	void UnlinkClientServer(void);
		
	BOOL InstallSpehon32(HINSTANCE hInstance);
	void WakeUp(void);
};

extern KEmfScope EmfScope;


