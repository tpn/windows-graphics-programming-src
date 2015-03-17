//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : canvas.h							                                 //
//  Description: EMFScope canvas window                                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KInputFile;

class KCanvasWindow : public KWindow
{
	HENHMETAFILE  hMetaFile;
	KInputFile   *hNTMetaFile;

	ENHMETAHEADER MetaHeader;
	
	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	char     CurEmfFileName[MAX_PATH];
	
	void GetDisplayRect(RECT *r);

public:
	int	     scale;
	COLORREF canvas_backcolor;
	int      canvas_delay;

	int	     page_margin;
	HINSTANCE hInstance;

	KCanvasWindow(HINSTANCE hInst, HWND hWnd, int x, int y, int w, int h);

	void  SetDelay(int delay);
	void  SetScaleColor(int newscale, COLORREF backcolor);
	BOOL  UnloadEmfFile(const char *filename);
	BOOL  LoadEmfFile(const char *filename);
	BOOL  Print(void);
	BOOL  Exam(void);
	
	const char *OpenFile(void);
};

