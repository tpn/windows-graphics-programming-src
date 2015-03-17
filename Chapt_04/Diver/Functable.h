//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : functable				                                             //
//  Description: KMyFuncTable class                                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KMyFuncTable : public KFuncTable
{
    void FuncEntryCallBack(KRoutineInfo *routine, EntryInfo *info);
    void  FuncExitCallBack(KRoutineInfo *routine, ExitInfo *info, unsigned leavetime, int depth);
};
