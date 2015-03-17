#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : opcode.h				                                             //
//  Description: Intel instruction map                                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//


typedef enum
{
	_INVALID,

	// prefix
	_ADDRSIZE,
	_OPNDSIZE,
	_SEG,
	_LOCK,
	_REP,
	_REPNE,
	
	// instructions
	_before_instruction,
	_AAA,
	_AAD,
	_AAM,
	_AAS,
	_ADC,
	_ADD,
	_AND,
	_ARPL,
	
	_BOUND,
	_BSF,
	_BSR,
	_BSWAP,
	_BT,
	_BTC,
	_BTR,
	_BTS,
	
	_CALL,
	_CBW,
	_CLC,
	_CLD,
	_CLI,
	_CLTS,
	_CMC,
	_CMOVA,
	_CMOVAE,
	_CMOVB,
	_CMOVBE,
	_CMOVE,
	_CMOVG,
	_CMOVGE,
	_CMOVL,
	_CMOVLE,
	_CMOVNE,
	_CMOVNO,
	_CMOVNP,
	_CMOVNS,
	_CMOVO,
	_CMOVP,
	_CMOVS,
	_CMP,
	_CMPSB,
	_CMPSWD,
	_CMPXCH8,
	_CMPXCHG,
	_CPUID,
	_CWD,
	
	_DAA,
	_DAS,
	_DEC,
	_DIV,
	
	_EMMS,
	_ENTER,
	
	_HLT,
	
	_IDIV,
	_IMUL,
	_IN,
	_INC,
	_INS,
	_INSB,
	_INT,
	_INT3,
	_INTO,
	_INVLPG,
	_IRET,
	
	_JB,
	_JBE,
	_JCXZ,
	_JL,
	_JLE,
	_JMP,
	_JNB,
	_JNBE,
	_JNL,
	_JNLE,
	_JNO,
	_JNP,
	_JNS,
	_JNZ,
	_JO,
	_JP,
	_JS,
	_JZ,
	
	_LAHF,
	_LAR,
	_LDS,
	_LEA,
	_LEAVE,
	_LES,
	_LFS,
	_LGDT,
	_LGS,
	_LIDT,
	_LLDT,
	_LMSW,
	_LODSB,
	_LODSWD,
	_LOOP,
	_LOOPE,
	_LOOPNE,
	_LSL,
	_LSS,
	_LTR,
	
	_MOV,
	_MOVD,
	_MOVQ,
	_MOVSB,
	_MOVSWD,
	_MOVSX,
	_MOVZX,
	_MUL,
	
	_NEG,
	_NOP,
	_NOT,
	
	_OR,
	_OUT,
	_OUTSB,
	
	_PACKSSDW,
	_PACKSSWB,
	_PACKUSDW,
	_PADDB,
	_PADDD,
	_PADDSB,
	_PADDSW,
	_PADDUSB,
	_PADDUSW,
	_PADDW,
	_PAND,
	_PANDN,
	_PCMPEQB,
	_PCMPEQD,
	_PCMPEQW,
	_PCMPGTB,
	_PCMPGTD,
	_PCMPGTW,
	_PMADDWD,
	_PMULLW,
	_POP,
	_POPA,
	_POPF,
	_POR,
	_PSLLD,
	_PSLLDWQ,
	_PSLLQ,
	_PSLLW,
	_PSRADW,
	_PSRAW,
	_PSRLD,
	_PSRLDWQ,
	_PSRLQ,
	_PSRLW,
	_PSUBB,
	_PSUBD,
	_PSUBSB,
	_PSUBSW,
	_PSUBUSB,
	_PSUBUSW,
	_PSUBW,
	_PUNPCKHBW,
	_PUNPCKHDQ,
	_PUNPCKHWD,
	_PUNPCKLBW,
	_PUNPCKLDQ,
	_PUNPCKLWD,
	_PUSH,
	_PUSHA,
	_PUSHF,
	_PXOR,
	
	_RCL,
	_RCR,
	_RDMSR,
	_RDPMC,
	_RDTSC,
	_RET,
	_RETFAR,
	_ROL,
	_ROR,
	_RSM,
	
	_SAHF,
	_SAR,
	_SBB,
	_SCASB,
	_SCASWD,
	_SETB,
	_SETBE,
	_SETL,
	_SETLE,
	_SETNB,
	_SETNBE,
	_SETNL,
	_SETNLE,
	_SETNO,
	_SETNP,
	_SETNS,
	_SETNZ,
	_SETO,
	_SETP,
	_SETS,
	_SETZ,
	_SGDT,
	_SHL,
	_SHLD,
	_SHR,
	_SHRD,
	_SIDT,
	_SLDT,
	_SMSW,
	_STC,
	_STD,
	_STI,
	_STOSB,
	_STOSWD,
	_STR,
	_SUB,
	
	_TEST,
	
	_VERR,
	_VERW,
	
	_WAIT,
	_WBINVD,
	_WRMSR,
	
	_XADD,
	_XCHG,
	_XLAT,
	_XOR,
	_after_instruction,

	// Groups, Escapes
	_UD2,

	_ESC,
	_ESCAPE_0F,
	_Grp1,
	_Grp2,
	_Grp3b,
	_Grp3v,
	_Grp4,
	_Grp5,
	_Grp6,
	_Grp7,
	_Grp8,
	_Grp9,
	_GrpA,

	// constant
	_before_operand,
	_1,
	
	// registers
	_AL,
	_BL,
	_CL,
	_DL,
	_AH,
	_BH,
	_CH,
	_DH,

	_DX,

	_EAX,
	_EBX,
	_ECX,
	_EDX,
	_ESP,
	_EBP,
	_ESI,
	_EDI,

	_eAX,
	_eBX,
	_eCX,
	_eDX,

	_eSP,
	_eBP,
	_eSI,
	_eDI,

	_DS,
	_CS,
	_SS,
	_ES,
	_FS,
	_GS,
	
	// byte
	_Eb,
	_Gb,
	_Jb,
	_Ib,
	_Ob,
	_Yb,
	_Xb,
	
	// Double word
	_Cd,
	_Dd,
	_Ed,
	_Pd,
	_Qd,
	_Rd,
	
	// 32-bit or 48 bit pointer
  	_Ap,
	_Mp,
	
	// word or dword
	_Av,
	_Ev,
	_Fv,
	_Gv,
	_Jv,
	_Iv,
	_Ov,
	_Xv,
	_Yv,
	
	// word
	_Ew,
	_Gw,
	_Iw,
	_Rw,
	_Sw,
	
	_Dx,
	_Eq,
	_M,
	_Ma,
	_Pq,
	_Qq,
	_Ep,
	_Ms,
	_BMq,

	_after_operand
};


typedef struct
{
	unsigned short opcode;
	unsigned short opnd[3];
}   OpCode;


extern const OpCode OpCodeMap[256];
extern const OpCode OpCodeMap_0F[256];

extern const OpCode OpCodeMap_Grp1[8];
extern const OpCode OpCodeMap_Grp2[8];
extern const OpCode OpCodeMap_Grp3b[8];
extern const OpCode OpCodeMap_Grp3v[8];
extern const OpCode OpCodeMap_Grp4[8];
extern const OpCode OpCodeMap_Grp5[8];
extern const OpCode OpCodeMap_Grp6[8];
extern const OpCode OpCodeMap_Grp7[8];
extern const OpCode OpCodeMap_Grp8[8];
extern const OpCode OpCodeMap_Grp9[8];
extern const OpCode OpCodeMap_GrpA[8];