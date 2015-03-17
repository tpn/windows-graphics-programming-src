//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : applet.h				                                             //
//  Description: Spy control program property sheet                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KAplet 
{
public:
    virtual void InitProcess(HINSTANCE hInstance) = 0;
    virtual void TermProcess(HINSTANCE hInstance) = 0;

    virtual int  GetPropertySheetPages(HINSTANCE hInst, HPROPSHEETPAGE *hPage) = 0;
    virtual void Initialize(HINSTANCE hInst) = 0;
};

