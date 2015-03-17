//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : function.cpp		                                                 //
//  Description: KFuncTable class                                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h> 
#include <string.h>
#include <assert.h>

#include "..\..\include\Decoder.h"
#include "..\..\include\atom.h"

#include "Function.h"
#include "Format.h"
#include "Report.h"
#include "Diver.h"
#include "..\Patcher\Patcher.h"

bool HackCopy(void * pDest, void * pSource, DWORD nSize);

/*
{
#ifdef WRITE_PROCESS

	static HANDLE hCurProcess = 0;

	DWORD dwWritten;

	bool rslt;
	
	if ( hCurProcess==0 )
		hCurProcess = GetCurrentProcess();

	rslt = WriteProcessMemory(hCurProcess, pDest, pSource, nSize, & dwWritten) != 0;

	return rslt && (dwWritten==nSize);

#else

	__try
	{
		memcpy(pDest, pSource, nSize);
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
		__try
		{
			MEMORY_BASIC_INFORMATION mbi;
            
			VirtualQuery(pDest, & mbi, sizeof(mbi));
			VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, & mbi.Protect);

			memcpy(pDest, pSource, nSize);
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			return false;
		}
    }

	return true;

#endif
}
*/


FARPROC KFuncTable::GetProcAddr(int funcid)
{
    int mod = m_func[funcid].f_module;

    switch ( m_func[funcid].f_kind )
	{
		case FUNC_COMMETHOD:
		{
			const unsigned * pFunc = (const unsigned *) VTable(mod);
			assert(pFunc);

			return (FARPROC) pFunc[m_func[funcid].f_methodid];
		}
		break;

		case FUNC_WIN32API:
	    {
		    HINSTANCE hModule = ModHandle(m_func[funcid].f_module);
			assert(hModule);

			FARPROC fp = GetProcAddress(hModule, GetFuncName(funcid));

//			assert(fp);

			if ( fp==NULL )
			{
				char temp[128];

				wsprintf(temp, "Not found module %d at %x, func %s at %x",
					m_func[funcid].f_module, hModule, GetFuncName(funcid), fp);

				Send(M_TEXT, 0, 0, temp);
			}

			return fp;
		}

		case FUNC_SYSCALL:
			return m_func[funcid].f_oldaddress;

		default:
			assert(false);
    }

	return NULL;
}


int KFuncTable::DefPara(const char * paratype, const char *name)
{
    if ( m_parano > MAX_PARA )
    {
        assert(0);
        return -1;
    }
    
    if ( (m_lastmodule<0) || (m_lastfunc<0) )
        return -1;
    
    m_func[m_lastfunc].f_parano ++;        
    
	int len = strlen(paratype);
	const char * pSpace = strchr(paratype, ' ');
	
	if ( pSpace )
		len = (int) pSpace - (int) paratype;

	char temp[64];

	assert(64>len);
	memcpy(temp, paratype, len);
	temp[len] = 0;

    m_para[m_parano].p_type = AddAtom(temp);
    m_para[m_parano].p_name = AddAtom(name);    
    
    return m_parano ++; 
}


void KFunction::InitStub(int index, FARPROC newaddress)
{
	f_stub[0]			      = 0x68;               // push
	*((int *)(f_stub+1))      = index;              // index
	f_stub[5]			      = 0xE9;               // jmp relative
	*((unsigned *)(f_stub+6)) = (unsigned) newaddress - (unsigned) (f_stub+10);
}


#define field_this 0

int KFuncTable::AddFunc(unsigned ord, const char *name, unsigned opt,
                        int     parano,
                        const char * paratype,
						int     kind,
						const void *  oldaddr)
{
    if ( (m_funcno > MAX_FUNC) || (m_lastmodule<0) )
    {
        assert(0);
        m_lastfunc = -1;

        return -1;
    }

	m_lastfunc = m_funcno ++;

    m_func[m_lastfunc].f_kind       = kind;
    m_func[m_lastfunc].f_module     = m_lastmodule;
    m_func[m_lastfunc].InitStub(m_lastfunc, (FARPROC) ProxyProlog);
	m_func[m_lastfunc].f_methodid   = ord;
    m_func[m_lastfunc].f_name       = AddAtom(name);
    m_func[m_lastfunc].f_parano     = 0;
    m_func[m_lastfunc].f_firstpara  = m_parano;
    m_func[m_lastfunc].f_class      = opt;
    m_func[m_lastfunc].f_hooked     = FALSE;

    m_func[m_lastfunc].f_totaltime  = 0;
    m_func[m_lastfunc].f_callno     = 0;

    for (int i=0; i<parano; i++)
	{
        DefPara(paratype);

		if ( i < (parano-1) )
		{
			paratype = strchr(paratype, ' ') + 1;

			if ( paratype==(const char *) 1 )
			{
				assert(false);

				paratype = NULL;
			}
		}
	}
    
	if ( oldaddr )
		m_func[m_lastfunc].f_oldaddress = (FARPROC) oldaddr;
	else
		m_func[m_lastfunc].f_oldaddress = GetProcAddr(m_lastfunc);
	
    return m_lastfunc;
}


void KFuncTable::Initialize(void)
{
    KModuleTable::Initialize();
    
    m_funcno  = 0;
    m_parano  = 0;
        
    Initialize_API();
}


FARPROC KFuncTable::LookupInterceptedAPI(FARPROC address, int & id)
{
    for (int i=0; i<m_funcno; i++)
    {
        if ( (m_func[i].f_kind==FUNC_WIN32API) && !m_func[i].f_hooked )
        if ( m_func[i].f_oldaddress == address )
        {
            id = i;
            return m_func[i].f_newaddress();
        }
    }

/*
    // If it's Chicago, and the app is being debugged (as this app is)
    // the loader doesn't fix up the calls to point directly at the
    // DLL's entry point.  Instead, the address in the .idata section
    // points to a PUSH xxxxxxxx / JMP yyyyyyyy stub.  The address in
    // xxxxxxxx points to another stub: PUSH aaaaaaaa / JMP bbbbbbbb.
    // The address in aaaaaaaa is the real address of the function in the
    // DLL.  This ugly code verifies we're looking at this stub setup,
    // and if so, grabs the real DLL entry point, and scans through
    // the InterceptedAPIArray list of addresses again.
    // ***WARNING*** ***WARNING*** ***WARNING*** ***WARNING*** 
    // This code is subject to change, and is current only as of 9/94.

	if ((GetVersion() & 0xC0000000) == 0xC0000000)
    {
        if ( (unsigned) address < 0x80000000 )  // Only shared, system DLLs
            return NULL;                        // have stubs
        
        if ( IsBadReadPtr(address, 9) || (*(PBYTE)address != 0x68)
             || (*((PBYTE)address+5) != 0xE9) )
            return NULL;

        FARPROC stubAddress = * (FARPROC *)((PBYTE) address+1);

        for (int i=0; i<m_funcno; i++ )
            if (!m_func[i].f_isvfunc)
            {
                FARPROC lunacy = m_func[i].f_oldaddress;
            
                if ( lunacy == stubAddress )
                    return m_func[i].f_newaddress();
            
                if ( !IsBadReadPtr(lunacy, 9) && 
				     (*(PBYTE)lunacy == 0x68) && 
				     (*((PBYTE)lunacy+5) == 0xE9) )
                {
                    lunacy = * (FARPROC *)( ((PBYTE)lunacy)+1);
                
				    if ( lunacy == stubAddress )
                        return m_func[i].f_newaddress();
                }
            }
    }
*/

    return NULL;
}

_declspec(naked) void SysCall_00(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret
}

_declspec(naked) void SysCall_04(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	4
}

_declspec(naked) void SysCall_08(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	8
}

_declspec(naked) void SysCall_0C(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x0c
}

_declspec(naked) void SysCall_10(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x10
}

_declspec(naked) void SysCall_14(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x14
}

_declspec(naked) void SysCall_18(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x18
}

_declspec(naked) void SysCall_1C(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x1C
}

_declspec(naked) void SysCall_20(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x20
}

_declspec(naked) void SysCall_24(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x24
}

_declspec(naked) void SysCall_28(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x28
}

_declspec(naked) void SysCall_2C(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x2C
}

_declspec(naked) void SysCall_30(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x30
}

_declspec(naked) void SysCall_34(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x34
}

_declspec(naked) void SysCall_38(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x38
}

_declspec(naked) void SysCall_3C(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x3C
}

_declspec(naked) void SysCall_40(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x40
}
_declspec(naked) void SysCall_44(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x44
}

_declspec(naked) void SysCall_48(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x48
}

_declspec(naked) void SysCall_4C(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x4C
}

_declspec(naked) void SysCall_50(void)
{
	__asm lea	edx, [esp+4]
	__asm int	0x2E
	__asm ret	0x50
}

const void * SysCall_Stub [] =
{
	SysCall_00,
	SysCall_04,
	SysCall_08,
	SysCall_0C,

	SysCall_10,
	SysCall_14,
	SysCall_18,
	SysCall_1C,

	SysCall_20,
	SysCall_24,
	SysCall_28,
	SysCall_2C,

	SysCall_30,
	SysCall_34,
	SysCall_38,
	SysCall_3C,

	SysCall_40,
	SysCall_44,
	SysCall_48,
	SysCall_4C,
	
	SysCall_50
};


int KFuncTable::InterceptSysCall(void)
{
	int no = 0;

	for (int i=0; i<m_funcno; i++)
        if ( (m_func[i].f_kind==FUNC_SYSCALL) && !m_func[i].f_hooked )
        {            
			unsigned char * p = (unsigned char *) m_func[i].f_oldaddress;

			// B8 DB 10 00 00     mov         eax,10DBh
			// 8D 54 24 04        lea         edx,[esp+4]
			// CD 2E              int         2Eh
			// C3                 ret

			// verify that the destination follows a system call pattern
			if ( ! IsBadReadPtr(p, 12) )
			if ( (p[0]==0xB8) && 
				 (p[5]==0x8D) && (p[6]==0x54) && (p[7]==0x24) && (p[8]==0x04) &&
				 (p[9]==0xCD) && (p[10]==0x2E) )
            {
				// let ProxyProlog return control to our SysCall_xx routines
				// f_parano counts return type, so decrease by 1
				m_func[i].f_oldaddress = (FARPROC) SysCall_Stub[m_func[i].f_parano-1];

				unsigned char Link[6];

				            Link[0] = 0xE9;
			* (unsigned *) (Link+1)	= (unsigned) m_func[i].f_newaddress() - 
									  (unsigned) ( p +10);
							Link[5] = 0x90;

				HackCopy(p+5, Link, 6);

                m_func[i].f_hooked = TRUE;
                no ++;
            }

        }

	return no;
}


int KFuncTable::InterceptCom(void)
{
    int no = 0;
    
    for (int i=0; i<m_funcno; i++)
        if ( (m_func[i].f_kind==FUNC_COMMETHOD) && !m_func[i].f_hooked )
        {            
            if ( HackMethod(VTable(m_func[i].f_module), 
				            m_func[i].f_methodid, 
							m_func[i].f_newaddress()) )
            {
                m_func[i].f_hooked = TRUE;
                no ++;
            }

        }

    return no;
}


int KFuncTable::InterceptWin32(const char        *Caller,
                               const char        *Callee,
                               PIMAGE_THUNK_DATA  pName,
                               PIMAGE_THUNK_DATA  pThunk,
                               const char        *baseAddress)
{
    int no = 0;
    
    // if module not added to module table, quit
    if (LookupModule(Caller, Callee) == -1)
        return 0;

    
    while ( pThunk->u1.Function )
    {
        int id;

        PBYTE   pFunctionName        = (PBYTE) ( baseAddress + (unsigned) pName->u1.ForwarderString + 2);
        FARPROC pInterceptedFunction = LookupInterceptedAPI((FARPROC) pThunk->u1.Function, id);

        if ( pInterceptedFunction )
			if ( HackCopy(& pThunk->u1.Function, & pInterceptedFunction, sizeof(FARPROC)) )
			{
				m_func[id].f_hooked = TRUE;
				no ++;
			}	
            
        pName ++;
        pThunk++;
    }

    return no;
}


int KFuncTable::InterceptWin32(const char *ModuleName, const char * baseAddress)
{
	int no = 0;

    PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER) baseAddress;
    
    if ( IsBadReadPtr(baseAddress, sizeof(PIMAGE_NT_HEADERS)) )
        return 0;
    
    if ( pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE )
        return 0;

    PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS) ( baseAddress + pDOSHeader->e_lfanew);
    if ( pNTHeader->Signature != IMAGE_NT_SIGNATURE )
        return 0;
    
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = 
		(PIMAGE_IMPORT_DESCRIPTOR) ( baseAddress + 
		pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress );
                        
    // Bail out if the RVA of the imports section is 0 (it doesn't exist)
    if ( pImportDesc == (PIMAGE_IMPORT_DESCRIPTOR) pNTHeader )
        return 0;

    while ( pImportDesc->Name )
    {
        no += InterceptWin32(ModuleName,
                             (const char *) ( baseAddress + pImportDesc->Name ),
                             (PIMAGE_THUNK_DATA) ( baseAddress + (unsigned) pImportDesc->OriginalFirstThunk),
                             (PIMAGE_THUNK_DATA) ( baseAddress + (unsigned) pImportDesc->FirstThunk),
                             baseAddress);
        pImportDesc++;
    }
    
    return no;
}


KPatcher directpatcher;

int KFuncTable::InterceptWin32(void)
{
    int no = 0;

//	DebugBreak();

    for (int i=0; i<m_funcno; i++)
        if ( (m_func[i].f_kind==FUNC_WIN32API) & !m_func[i].f_hooked )
        // if there is an unhooked win32 function
		{
			int module = m_func[i].f_module;

			// if caller and callee are the same module, patch function target
			if ( strcmp(CallerName(module), CalleeName(module))==0 )
			{
				if ( directpatcher.Patch(ModHandle(module), GetFuncName(i), 
										 i, ProxyProlog,
										 (unsigned long *) &  m_func[i].f_oldaddress) )
					no ++;
			}
			else
			{
				const char *caller  = CallerName(module);
				const char *address = (const char *) ModHandle(module);

				// if caller module is loaded into current process, hack in it.
				if (address)
					no += InterceptWin32(caller, address);
			}
		}

    return no;
}

