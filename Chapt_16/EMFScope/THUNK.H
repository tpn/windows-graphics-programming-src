//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : thunk.h							                                 //
//  Description: Three Intel instruction for linking Win32 with C++                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

typedef unsigned char ThunkData[9];

void inline ThunkInit(ThunkData t, void * This)
{
	t[0]              = 0xB9;	       // mov ecx, 
	*((DWORD *)(t+1)) = (DWORD) This;  //          this
	*((DWORD *)(t+5)) = 0x20FF018B;    // mov eax, [ecx] 
							           // jmp      [eax]
}
