//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : kernelopt.h			                                             //
//  Description: Compiler/linker options to make a kernel mode driver                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#ifndef WINDDK_H

#define WINDDK_H

extern "C"
{

#undef WIN32

#define _X86_			    1 
#define i386			    1  
#define STD_CALL 
#define CONDITION_HANDLING  1 
#define NT_UP               1  
#define NT_INST             0 
#define WIN32			    100 
#define _NT1X_			    100 
#define WINNT			    1 
#define _WIN32_WINNT	    0x0400    
#define WIN32_LEAN_AND_MEAN 1 
#define DBG					1
#define DEVL				1 
#define FPO					0
#define _DLL				1 
#define _IDWBUILD  

#pragma message("Add DDK to VC include and library directories")

#include <ddk\ntddk.h>

}

#pragma check_stack(off)

#pragma comment(lib, "ntoskrnl.lib")
#pragma comment(linker, "-MERGE:.rdata=.text")
#pragma comment(linker, "-MERGE:.reloc=.text")
#pragma comment(linker, "-entry:DriverEntry")
#pragma comment(linker, "-IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4096")

// #pragma comment(linker, "-ALIGN:0x20")  // bad

// #pragma comment(linker, "/subsystem:native,4.00")
// #pragma comment(linker, "/driver")

// #pragma comment(linker, "/SECTION:INIT,d")
// #pragma comment(linker, "/OPT:REF")

// #pragma comment(linker, "-FORCE:MULTIPLE") 
// #pragma comment(linker, "-version:4.00") 
// #pragma comment(linker, "-osversion:4.00") 
// #pragma comment(linker, "-optidata") 
// #pragma comment(linker, "-FULLBUILD")

#endif