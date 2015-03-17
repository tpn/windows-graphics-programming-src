//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : host.cpp				                                             //
//  Description: KHost class                                                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#include <winioctl.h>
#include <imagehlp.h> 
#include <wdbgexts.h>

#include "ImageModule.h"
#include "Host.h"


ULONG KPeriscopeClient::Read(void * dst, const void * src, unsigned len)
{
    unsigned      cmd[2] = { (unsigned) src, len };
    unsigned long dwRead = 0;
   
    IoControl(IOCTL_PERISCOPE, cmd, sizeof(cmd), dst, len, &dwRead);
   
    return dwRead;
}


void KHost::WndOutput(HWND hWnd, const char * format, va_list argptr)
{
    char buffer[1024];
    
    vsprintf(buffer, format, argptr);

	if ( hWnd )
	{
		SendMessage(hWnd, EM_SETSEL, 0xFFFFFF, 0xFFFFFF);
		SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM) buffer);
	}
	else
		OutputDebugString(buffer);
}


void KHost::Log(const char * format, ...)
{
    va_list ap;

	va_start(ap, format);
	WndOutput(hwndLog, format, ap);
	va_end(ap);
}


void KHost::ExtOutput(const char * format, ...)
{
	va_list ap;
    
	va_start(ap, format);
	WndOutput(hwndOutput, format, ap);
	va_end(ap);
}


unsigned KHost::ExtGetExpression(const char * expr)
{
    if ( (expr==NULL) || strlen(expr)==0 )
    {
        assert(false);
        return 0;
    }

    if ( (expr[0]>='0') && (expr[0]<='9') ) // hex number
    {
        DWORD number;

        sscanf(expr, "%x", & number);
        return number;
    }

    if ( pWin32k )
    {
        const IMAGEHLP_SYMBOL * pis;
		
		if ( expr[0]=='&' )		// skip the first &
			pis = pWin32k->ImageGetSymbol(expr+1);
		else
			pis = pWin32k->ImageGetSymbol(expr);

		if ( pis )
		{
			DWORD addr = pis->Address;  // ??? //
        
			Log("GetExpression(%s)=%08lx\n", expr, addr);
            
			return addr;
		}	
    }

    ExtOutput("Unknown GetExpression(""%s"")\n", expr); 
    throw "Unknown Expression";

    return 0;
}


bool KHost::ExtReadProcessMemory(const void * address, 
	 unsigned * buffer, unsigned  count, 
	 unsigned long * bytesread)
{
	if ( pScope )
	{
		ULONG dwRead = 0;

		if ( (unsigned) address >= 0x80000000 )
		{
	
/*			if ( (count==264) || (count==1552) )
				for (int i=4; i<count/4; i++)
					buffer[i] = i*4;

			else
*/	
			dwRead = pScope->Read(buffer, address, count);

//			if ( count==264 )
//				for (int i=count/4-1; i>(count/4-10); i--)
//					buffer[i] = i * 4;
		}
		else
	
/*			if ( count==264 )
				for (int i=0; i<count/4; i++)
					buffer[i] = 0xff000000+ i*4;
			else
*/	
			ReadProcessMemory(hProcess, address, buffer, count, & dwRead);

		if ( bytesread )
			* bytesread = dwRead;
		
		if ( (unsigned) address >= 0x80000000 )
			Log("ReadKRam(%08x, %d)=", address, count);
		else
			Log("ReadURam(%x, %08x, %d)=", hProcess, address, count);
			
		int len = min(4, count/4);

		for (int i=0; i<len; i++)
			Log("%08x ", buffer[i]);
			
		Log("\n");

		return dwRead == count;
	}
	else
	{
		assert(false);
		return false;
	}
}

KHost theHost;


void WDBGAPI ExtOutputRoutine(PCSTR format, ...)
{
	va_list ap;
    va_start(ap, format);

	theHost.WndOutput(theHost.hwndOutput, format, ap);
    
    va_end(ap);
}


ULONG WDBGAPI ExtGetExpression(PCSTR expr)
{
	return theHost.ExtGetExpression(expr);
}


void WDBGAPI ExtGetSymbol(PVOID offset, PUCHAR  pchBuffer, PULONG  pDisplacement)
{
	throw "GetSymbol not implemented";
}


ULONG WDBGAPI ExtDisAsm(PULONG lpOffset, PCSTR  lpBuffer, ULONG  fShowEffectiveAddress)
{
	throw "Disam not implemented";
	return FALSE;
}


ULONG WDBGAPI ExtCheckControl_C(VOID)
{
	return FALSE;
}


ULONG WDBGAPI ExtReadProcessMemory(ULONG address, 
	PVOID buffer, ULONG  count, PULONG bytesread)
{
	return theHost.ExtReadProcessMemory(
		(const void *)address, (unsigned *) buffer, count, bytesread);
}


ULONG WDBGAPI ExtWriteProcessMemory(ULONG offset, 
	LPCVOID lpBuffer, ULONG cb, PULONG  lpcbBytesWritten)
{
	throw "WriteProcessMemory not implemented";
	
	return 0;
}


ULONG WDBGAPI ExtGetThreadContext(ULONG Processor, PCONTEXT lpContext, ULONG cbSizeOfContext)
{
	throw "GetThreadContext not implemented";
	return 0;
}


ULONG WDBGAPI ExtSetThreadContext(ULONG Processor,
    PCONTEXT lpContext, ULONG cbSizeOfContext)
{
	throw "SetThreadContext not implemented";
	return 0;
}


ULONG WDBGAPI ExtIOCTL(USHORT IoctlType, PVOID lpvData, ULONG cbSize)
{
	throw "IOCTL not implemented";
	return 0;
}


ULONG ExtStackTrace(ULONG FramePointer,
    ULONG StackPointer, ULONG ProgramCounter,
    PEXTSTACKTRACE StackFrames, ULONG Frames)
{
	throw "ExtStackTrace not implemented";
	return 0;
}


WINDBG_EXTENSION_APIS ExtensionAPI = 
{
    sizeof(WINDBG_EXTENSION_APIS),

	ExtOutputRoutine,
    ExtGetExpression,
    ExtGetSymbol,
	ExtDisAsm,
    ExtCheckControl_C,
    ExtReadProcessMemory,
	ExtWriteProcessMemory,
	ExtGetThreadContext,
	ExtSetThreadContext,
	ExtIOCTL,
	ExtStackTrace
};
