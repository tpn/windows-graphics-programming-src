//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : filelist.h							                                 //
//  Description: EMF file list box                                                   //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

const int n_maxvisible = 5;

class KFileList : public KSubclass
{
private:

	HINSTANCE      hCurInst;
	HMENU		   hCurMenu;

	HWND		   hwnd_emflist;
	KCanvasWindow *w_canvas;
	int			   n_lastid;

    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
public:
	KFileList(void)
	{
		hCurInst	 = NULL;
		hCurMenu	 = NULL;
		hwnd_emflist = NULL;
		w_canvas     = NULL;

		n_lastid     = -1;
	}

	int  GetCount(void);
	void Create(HINSTANCE hInstance, HWND hDlg, HMENU hMenu);
	
	void SetCanvas(KCanvasWindow *hWnd)
	{
		w_canvas = hWnd;
	}

	void AddtoFileList(const char *filename);
	void ReplaceLastName(const char *name);

	const char *GetEmfFileName(int id);
	void TrackMenu(int id, int x, int y);

	void Open(void);
	void Remove(void);
	void Rename(void);
	void Delete(void);
	void Properties(void);
};

