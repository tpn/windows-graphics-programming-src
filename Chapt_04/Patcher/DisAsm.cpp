//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : disasm.cpp				                                             //
//  Description: Intel machine code parser                                            //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <assert.h>

#include "DisAsm.h"
#include "OpCode.h"

KDisAsm::KDisAsm()
{

}

KDisAsm::~KDisAsm()
{

}


#define case4(a, b, c, d) case a: case b: case c: case d


int KDisAsm::OperandLen(const OpCode * map, unsigned opcode, int opndsize, int & modrm)
{
	int datalen = 0;

	for (int i=0; i<3; i++) 
		if (map[opcode].opnd[i] != 0)
		{
			unsigned data = map[opcode].opnd[i];

			assert ( (data>_before_operand) && (data<_after_operand) );

			switch ( data )
			{
				case 1:
				case _DX:
//				case _eA:
					
				case4(_AL, _BL, _CL, _DL):	
				case4(_AH, _BH, _CH, _DH):
				case4(_EAX,_EBX,_ECX,_EDX):
				case4(_ESP,_EBP,_ESI,_EDI):
				case4(_eAX,_eBX,_eCX,_eDX):

				case4(_eSP,_eBP,_eSI,_eDI):
				case4(_DS, _CS, _SS, _ES):
				case _FS:
				case _GS:

				case _Xv:
				case _Yv:
				case _Yb:
				case _Xb:
				case _Fv:
				case _M :
					break;

				case _Mp:
				case _Ap: datalen += 2 + opndsize; break;
				
				case _Av: datalen += opndsize; break;
	
				case _Ep: datalen += 2 + opndsize; 
					      modrm = 1; break;
					
				case _Cd:
				case _Dd:
				case _Dx:
				case _Ed:
				case _Ev:
				case _Eb:
				case _Ew:
				case _Eq:
				case _Gb:
				case _Gv:
				case _Gw: 
				case _Pd:
				case _Pq:
				case _Qd:
				case _Qq:
				case _Rd:
				case _Rw:
				case _Sw:
					modrm = 1; break; // modR/M
					break;
	
				case _Ob:
				case _Ib: datalen ++;   break;

				case _Jb: MessageBox(NULL, m_funcname, "Relative Instructions", MB_OK);
						  m_error = true;
					      datalen ++;   break;

				case _Iw: datalen += 2; break;
	
				case _Ov:
				case _Iv: datalen += opndsize; break;

				case _Jv: MessageBox(NULL, m_funcname, "Relative Instructions", MB_OK);
					      m_error = true;
					      datalen += opndsize; break;

				case _Ms: datalen = 6;			  break;
				case _Ma: datalen = opndsize * 2; break;

				case _BMq:					
				default :
					assert(false);
			}
		}

	return datalen;
}


int KDisAsm::Length(const unsigned char * code, const char * funcname)
{
	m_error    = false;
	m_funcname = funcname;

	int addr32 = true;
	int opnd32 = true;
	int len    = 0;

	unsigned b  = code[len++];
	unsigned op = OpCodeMap[b].opcode;

	// instruction prefix
	if ( (op==_LOCK) || (op==_REPNE) || (op==_REP) )
	{
		b  = code[len++];
		op = OpCodeMap[b].opcode;
	}

	// address prefix
	if ( op==_ADDRSIZE )
	{
		addr32 = ! addr32;
		
		b  = code[len++];
		op = OpCodeMap[b].opcode;
	}

	// operand prefix
	if ( op==_OPNDSIZE )
	{
		opnd32 = ! opnd32;
		
		b  = code[len++];
		op = OpCodeMap[b].opcode;
	}

	// segment override
	if ( op == _SEG )
	{
		b  = code[len++];
		op = OpCodeMap[b].opcode;
	}

	const OpCode * Map = OpCodeMap;

	if ( op == _ESCAPE_0F )
	{
		b   = code[len++];
		Map = OpCodeMap_0F;
		op  = Map[b].opcode;
	}

	int			   hasmodrm = 0;
	unsigned char  modrm    = code[len];

	if ( (op>_before_instruction) && (op<_after_instruction) )
	{
		len += OperandLen(Map, b, 2 + 2 * opnd32, hasmodrm);
	}
	else 
	{
		len += OperandLen(Map, b, 2 + 2 * opnd32, hasmodrm);

		switch ( op )
		{
			case _Grp1: 
				hasmodrm = 1; 
				len+= OperandLen(OpCodeMap_Grp1, (code[len] >> 3) & 7, 2 + 2 * opnd32, hasmodrm);
				break;

			case _Grp2: 
				hasmodrm = 1; 
				len+= OperandLen(OpCodeMap_Grp2, (code[len] >> 3) & 7, 2 + 2 * opnd32, hasmodrm);
				break;

			case _Grp3b: 
				hasmodrm = 1; 
				len+= OperandLen(OpCodeMap_Grp3b, (code[len] >> 3) & 7, 2 + 2 * opnd32, hasmodrm);
				break;

			case _Grp3v: 
				hasmodrm = 1; 
				len+= OperandLen(OpCodeMap_Grp3v, (code[len] >> 3) & 7, 2 + 2 * opnd32, hasmodrm);
				break;

			case _Grp4: 
				hasmodrm = 1; 
  				len+= OperandLen(OpCodeMap_Grp4, (code[len] >> 3) & 7, 2 + 2 * opnd32, hasmodrm);
				break;

			case _Grp5: 
				hasmodrm = 1; 
  				len+= OperandLen(OpCodeMap_Grp5, (code[len] >> 3) & 7, 2 + 2 * opnd32, hasmodrm);
				break;

			case _Grp6: 
				hasmodrm = 1; 
				len+= OperandLen(OpCodeMap_Grp6, (code[len] >> 3) & 7, 2 + 2 * opnd32, hasmodrm);
				break;

			case _Grp7: 
				hasmodrm = 1; 
				len+= OperandLen(OpCodeMap_Grp7, (code[len] >> 3) & 7, 2 + 2 * opnd32, hasmodrm);
				break;

			case _Grp8: 
				hasmodrm = 1; 
				len+= OperandLen(OpCodeMap_Grp8, (code[len] >> 3) & 7, 2 + 2 * opnd32, hasmodrm);
				break;

			case _Grp9: 
				hasmodrm = 1; 
				len+= OperandLen(OpCodeMap_Grp9, (code[len] >> 3) & 7, 2 + 2 * opnd32, hasmodrm);
				break;

			case _GrpA: 
				hasmodrm = 1; 
				len+= OperandLen(OpCodeMap_GrpA, (code[len] >> 3) & 7, 2 + 2 * opnd32, hasmodrm);
				break;

			default:
				assert(false);
		}
	}

	if ( hasmodrm )
	{
		int mod = (modrm >> 6) & 3;
		int r_m = modrm & 7;

		if ( addr32 )
		{
			switch ( mod )
			{
				case 0:
					if (r_m==4) len ++;		// sib
					if (r_m==5) len+=4;    // disp32
					break;

				case 1:
					len++;					// disp8
					if (r_m==4) len++;		// sib
					break;

				case 2:
					len+=4;					// disp32
					if (r_m==4) len++;		// sib
			}
		}
		else
		{
			switch ( mod )
			{
				case 0: 
					if (r_m==6) len += 2;	// disp16
					break;

				case 1:
					len += 1;				// disp8
					break;

				case 2:
					len += 2;				// disp16
					break;
			}
		}
	}

	if ( m_error )
		return 0;

	return len + hasmodrm;
}


KDisAsm	dis;


int First5(const unsigned char * pProc, const char * funcname)
{
	if ( pProc==NULL )
		return 0;

	int total = 0;
	while ( total < 5 )
	{
		int len = dis.Length(pProc, funcname);

		if ( len==0 )
			return 0;

		pProc += len;
		total += len;
	}

	return total;
}
