//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : spoolfile.h						                                 //
//  Description: Win9x Spool file			                                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

const int n_maxsplsize = 2048;

class KSpoolFile
{
	char DevData[n_maxsplsize];
		
	int  size_header;
	int  size_devmode;
	int  size_std;

public:
	KSpoolFile();

	BOOL ReceiveData(COPYDATASTRUCT *cd);
	void GenDevFile(const char *emffilename);
};	

extern KSpoolFile SpoolFile;

const char *GetDevFileName(const char *emffilename);


