//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : opcode.cpp  			                                             //
//  Description: Intel instruction map                                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include "OpCode.h"

// Map for the first byte of Intel instructions
const OpCode OpCodeMap[256] =
{	// 00
	{ _ADD,		_Eb,	_Gb	},
	{ _ADD,		_Ev,	_Gv	},
	{ _ADD,		_Gb,	_Eb },
	{ _ADD,		_Gv,	_Ev },
	{ _ADD,		_AL,	_Ib },
	{ _ADD,		_eAX,	_Iv	},
	{ _PUSH,	_ES			},
	{ _POP,		_ES			},

	// 08
	{ _OR, 		_Eb,	_Gb	},
	{ _OR, 		_Ev,	_Gv	},
	{ _OR, 		_Gb,	_Eb },
	{ _OR, 		_Gv,	_Ev },
	{ _OR, 		_AL,	_Ib },
	{ _OR, 		_eAX,	_Iv	},
	{ _PUSH,	_CS			},
	{ _ESCAPE_0F			},

	// 10
	{ _ADC,		_Eb,	_Gb	},
	{ _ADC,		_Ev,	_Gv	},
	{ _ADC,		_Gb,	_Eb },
	{ _ADC,		_Gv,	_Ev },
	{ _ADC,		_AL,	_Ib },
	{ _ADC,		_eAX,	_Iv	},
	{ _PUSH,	_SS			},
	{ _POP,		_SS			},

	// 18
	{ _SBB,		_Eb,	_Gb	},
	{ _SBB,		_Ev,	_Gv	},
	{ _SBB,		_Gb,	_Eb },
	{ _SBB,		_Gv,	_Ev },
	{ _SBB,		_AL,	_Ib },
	{ _SBB,		_eAX,	_Iv	},
	{ _PUSH,	_DS			},
	{ _POP,		_DS			},

	// 20
	{ _AND,		_Eb,	_Gb	},
	{ _AND,		_Ev,	_Gv	},
	{ _AND,		_Gb,	_Eb },
	{ _AND,		_Gv,	_Ev },
	{ _AND,		_AL,	_Ib },
	{ _AND,		_eAX,	_Iv	},
	{ _SEG, 	_ES			},
	{ _DAA					},

	// 28
	{ _SUB,		_Eb,	_Gb	},
	{ _SUB,		_Ev,	_Gv	},
	{ _SUB,		_Gb,	_Eb },
	{ _SUB,		_Gv,	_Ev },
	{ _SUB,		_AL,	_Ib },
	{ _SUB,		_eAX,	_Iv	},
	{ _SEG, 	_CS			},
	{ _DAS					},

	// 30
	{ _XOR,		_Eb,	_Gb	},
	{ _XOR,		_Ev,	_Gv	},
	{ _XOR,		_Gb,	_Eb },
	{ _XOR,		_Gv,	_Ev },
	{ _XOR,		_AL,	_Ib },
	{ _XOR,		_eAX,	_Iv	},
	{ _SEG, 	_SS			},
	{ _AAA					},

	// 38
	{ _CMP,		_Eb,	_Gb	},
	{ _CMP,		_Ev,	_Gv	},
	{ _CMP,		_Gb,	_Eb },
	{ _CMP,		_Gv,	_Ev },
	{ _CMP,		_AL,	_Ib },
	{ _CMP,		_eAX,	_Iv	},
	{ _SEG, 	_DS			},
	{ _AAS					},

	// 40
	{ _INC,		_eAX		},
	{ _INC,		_eCX		},
	{ _INC,		_eDX		},
	{ _INC,		_eBX		},
	{ _INC,		_eSP		},
	{ _INC,		_eBP		},
	{ _INC,		_eSI		},
	{ _INC,		_eDI		},

	// 48
	{ _DEC,		_eAX		},
	{ _DEC,		_eCX		},
	{ _DEC,		_eDX		},
	{ _DEC,		_eBX		},
	{ _DEC,		_eSP		},
	{ _DEC,		_eBP		},
	{ _DEC,		_eSI		},
	{ _DEC,		_eDI		},

	// 50
	{ _PUSH,	_eAX		},
	{ _PUSH,	_eCX		},
	{ _PUSH,	_eDX		},
	{ _PUSH,	_eBX		},
	{ _PUSH,	_eSP		},
	{ _PUSH,	_eBP		},
	{ _PUSH,	_eSI		},
	{ _PUSH,	_eDI		},

	// 58
	{ _POP, 	_eAX		},
	{ _POP, 	_eCX		},
	{ _POP, 	_eDX		},
	{ _POP, 	_eBX		},
	{ _POP, 	_eSP		},
	{ _POP, 	_eBP		},
	{ _POP, 	_eSI		},
	{ _POP, 	_eDI		},

	// 60
	{ _PUSHA				},
	{ _POPA					},
	{ _BOUND,   _Gv,	_Ma	},
	{ _ARPL,	_Ew,	_Gw },
	{ _SEG,		_FS			},
	{ _SEG,		_GS			},
	{ _OPNDSIZE				},
	{ _ADDRSIZE				},

	// 68
	{ _PUSH,	_Iv						},
	{ _IMUL,	_Gv,	_Ev,	_Iv		},
	{ _PUSH,	_Ib			},
	{ _IMUL,	_Gv,	_Ev,	_Iv		},
	{ _INSB,	_Yb,	_Dx },
	{ _INS, 	_Yv,	_Dx	},
	{ _OUTSB,	_Dx,    _Xb },
	{ _OUT,		_Dx,    _Xv },

	// 70
	{ _JO,		_Jb			},
	{ _JNO,		_Jb			},
	{ _JB,		_Jb			},
	{ _JNB,		_Jb			},
	{ _JZ,		_Jb			},
	{ _JNZ,		_Jb			},
	{ _JBE,		_Jb			},
	{ _JNBE,	_Jb			},

	// 78
	{ _JS,		_Jb			},
	{ _JNS,		_Jb			},
	{ _JP,		_Jb			},
	{ _JNP,		_Jb			},
	{ _JL,		_Jb			},
	{ _JNL,		_Jb			},
	{ _JLE,		_Jb			},
	{ _JNLE,	_Jb			},

	// 80
	{ _Grp1,	_Eb,	_Ib },
	{ _Grp1,	_Ev,	_Iv },
	{ _Grp1,	_Ev,    _Ib	},
	{ _Grp1,	_Eb,	_Ib },
	{ _TEST,	_Eb,    _Gb },
	{ _TEST,    _Ev,	_Gv },
	{ _XCHG,	_Eb,    _Gb },
	{ _XCHG,	_Ev,    _Gv },

	// 88
	{ _MOV,		_Eb,	_Gb },
	{ _MOV,		_Ev,	_Gv	},
	{ _MOV,		_Gb,    _Eb },
	{ _MOV,		_Gv,	_Ev },
	{ _MOV,		_Ew,	_Sw	},
	{ _LEA,		_Gv,	_M  },
	{ _MOV,		_Sw,	_Ew },
	{ _POP,		_Ev			},

	// 90
	{ _NOP						},
	{ _XCHG,	_eAX,	_eCX	},
	{ _XCHG,	_eAX,	_eDX	},
	{ _XCHG,	_eAX,	_eBX	},
	{ _XCHG,	_eAX,	_eSP	},
	{ _XCHG,	_eAX,	_eBP	},
	{ _XCHG,	_eAX,	_eSI	},
	{ _XCHG,	_eAX,	_eDI	},

	// 98
	{ _CBW						},
	{ _CWD						},
	{ _CALL,	_Ap				},
	{ _WAIT						},
	{ _PUSHF,	_Fv				},
	{ _POPF,	_Fv				},
	{ _SAHF						},
	{ _LAHF						},

	// A0
	{ _MOV,		_AL,	_Ob		},
	{ _MOV,		_eAX,	_Ov		},
	{ _MOV,		_Ob,	_AL		},
	{ _MOV,		_Ov,	_eAX	},
	{ _MOVSB,	_Xb,	_Yb		},
	{ _MOVSWD,	_Xv,	_Yv		},
	{ _CMPSB,	_Xb,	_Yb		},
	{ _CMPSWD,	_Xv,	_Yv		},

	// A8
	{ _TEST,	_AL,	_Ib		},
	{ _TEST,	_eAX,	_Iv		},
	{ _STOSB,	_Yb,	_AL		},
	{ _STOSWD,	_Yv,	_eAX	},
	{ _LODSB,	_AL,	_Xb		},
	{ _LODSWD,	_eAX,	_Xv		},
	{ _SCASB,	_AL,	_Xb		},
	{ _SCASWD,	_eAX,	_Xv		},

	// B0
	{ _MOV,		_AL,	_Ib		},
	{ _MOV,		_CL,	_Ib		},
	{ _MOV,		_DL,	_Ib		},
	{ _MOV,		_BL,	_Ib		},
	{ _MOV,		_AH,	_Ib		},
	{ _MOV,		_CH,	_Ib		},
	{ _MOV,		_DH,	_Ib		},
	{ _MOV,		_BH,	_Ib		},

	// B8
	{ _MOV,		_eAX,	_Iv		},
	{ _MOV,		_eCX,	_Iv		},
	{ _MOV,		_eDX,	_Iv		},
	{ _MOV,		_eBX,	_Iv		},
	{ _MOV,		_eSP,	_Iv		},
	{ _MOV,		_eBP,	_Iv		},
	{ _MOV,		_eSI,	_Iv		},
	{ _MOV,		_eDI,	_Iv		},

	// C0
	{ _Grp2,	_Eb,	_Ib		},
	{ _Grp2,	_Ev,	_Ib		},
	{ _RET,		_Iw				},
	{ _RET						},
	{ _LES,		_Gv,	_Mp		},
	{ _LDS,		_Gv,	_Mp		},
	{ _MOV,		_Eb,	_Ib		},
	{ _MOV,		_Ev,	_Iv		},

	// C8
	{ _ENTER,	_Iw,	_Ib		},
	{ _LEAVE					},
	{ _RETFAR,	_Iw				},
	{ _RETFAR					},
	{ _INT3						},
	{ _INT,		_Ib				},
	{ _INTO						},
	{ _IRET						},

	// D0
	{ _Grp2,	_Eb,	_1		},
	{ _Grp2,	_Ev,	_1		},
	{ _Grp2,	_Eb,	_CL		},
	{ _Grp2,	_Ev,	_CL		},
	{ _AAM						},
	{ _AAD						},
	{ _INVALID					},
	{ _XLAT						},

	// D8
	{ _ESC						},
	{ _ESC						},
	{ _ESC						},
	{ _ESC						},
	{ _ESC						},
	{ _ESC						},
	{ _ESC						},
	{ _ESC						},

	// E0
	{ _LOOPNE,	_Jb				},
	{ _LOOPE,	_Jb				},
	{ _LOOP,	_Jb				},
	{ _JCXZ,	_Jb				},
	{ _IN,		_AL,	_Ib		},
	{ _IN,		_eAX,	_Iv		},
	{ _OUT,		_Ib,	_AL		},
	{ _OUT,		_Iv,	_eAX	},

	// E8
	{ _CALL,	_Jv				},
	{ _JMP,		_Jv				},
	{ _JMP,		_Ap				},
	{ _JMP,		_Jb				},
	{ _IN,		_AL,	_DX		},
	{ _IN,		_eAX,	_DX		},
	{ _OUT,		_DX,	_AL		},
	{ _OUT,		_DX,	_eAX	},

	// F0
	{ _LOCK						},
	{ _INVALID					},
	{ _REPNE					},
	{ _REP						},
	{ _HLT						},
	{ _CMC						},
	{ _Grp3b					},
	{ _Grp3v					},

	// F8
	{ _CLC						},
	{ _STC						},
	{ _CLI						},
	{ _STI						},
	{ _CLD						},
	{ _STD						},
	{ _Grp4						},
	{ _Grp5						}
};


// Map for instructions start with 0x0F
const OpCode OpCodeMap_0F[256] =
{
	// 0F 00
	{ _Grp6						},
	{ _Grp7						},
	{ _LAR,		_Gv,	_Ew		},
	{ _LSL,		_Gv,	_Ew		},
	{ _INVALID					},
	{ _INVALID					},
	{ _CLTS						},
	{ _INVALID					},

	// 0F 08
	{ _INVALID					},
	{ _WBINVD					},
	{ _INVALID					},
	{ _UD2						},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	
	// 0F 10
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},

	// 0F 18
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},

	// 0F 20
	{ _MOV,		_Rd,	_Cd		},
	{ _MOV,		_Rd,	_Dd		},
	{ _MOV,		_Cd,	_Rd		},
	{ _MOV,		_Dd,	_Rd		},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	
	// 0F 28
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},

	// 0F 30
	{ _WRMSR					},
	{ _RDTSC					},
	{ _RDMSR					},
	{ _RDPMC					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},

	// 0F 38
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},

	// 0F 40
	{ _CMOVO,	_Gv,	_Ev		},
	{ _CMOVNO,	_Gv,	_Ev		},
	{ _CMOVB,	_Gv,	_Ev		},
	{ _CMOVAE,	_Gv,	_Ev		},
	{ _CMOVE,	_Gv,	_Ev		},
	{ _CMOVNE,	_Gv,	_Ev		},
	{ _CMOVBE,	_Gv,	_Ev		},
	{ _CMOVA,	_Gv,	_Ev		},

	// 0F 48
	{ _CMOVS,	_Gv,	_Ev		},
	{ _CMOVNS,	_Gv,	_Ev		},
	{ _CMOVP,	_Gv,	_Ev		},
	{ _CMOVNP,	_Gv,	_Ev		},
	{ _CMOVL,	_Gv,	_Ev		},
	{ _CMOVGE,	_Gv,	_Ev		},
	{ _CMOVLE,	_Gv,	_Ev		},
	{ _CMOVG,	_Gv,	_Ev		},
	
	// 0F 50
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},

	// 0F 58
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},

	// 0F 60
	{ _PUNPCKLBW,	_Pq,	_Qd		},
	{ _PUNPCKLWD,	_Pq,	_Qd		},
	{ _PUNPCKLDQ,	_Pq,	_Qd		},
	{ _PACKUSDW,	_Pq,	_Qd		},
	{ _PCMPGTB,		_Pq,	_Qd		},
	{ _PCMPGTW,		_Pq,	_Qd		},
	{ _PCMPGTD,		_Pq,	_Qd		},
	{ _PACKSSWB,	_Pq,	_Qd		},

	// 0F 68
	{ _PUNPCKHBW,	_Pq,	_Qd		},
	{ _PUNPCKHWD,	_Pq,	_Qd		},
	{ _PUNPCKHDQ,	_Pq,	_Qd		},
	{ _PACKSSDW,	_Pq,	_Qd		},
	{ _INVALID						},
	{ _INVALID						},
	{ _MOVD,		_Pq,	_Qd		},
	{ _MOVQ,		_Pq,	_Qd		},

	// 0F 70
	{ _INVALID						},
	{ _GrpA							},
	{ _GrpA							},
	{ _GrpA							},
	{ _PCMPEQB,		_Pq,	_Qd		},
	{ _PCMPEQW,		_Pq,	_Qd		},
	{ _PCMPEQD,		_Pd,	_Qd		},
	{ _EMMS							},

	// 0F 78
	{ _INVALID						},
	{ _INVALID						},
	{ _INVALID						},
	{ _INVALID						},
	{ _INVALID						},
	{ _INVALID						},
	{ _MOVD,		_Pd,	_Ed		},
	{ _MOVQ,		_Pq,	_Eq		},

	// 0F 80
	{ _JO,			_Jv				},
	{ _JNO,			_Jv				},
	{ _JB,			_Jv				},
	{ _JNB,			_Jv				},
	{ _JZ,			_Jv				},
	{ _JNZ,			_Jv				},
	{ _JBE,			_Jv				},
	{ _JNBE,		_Jv				},

	// 0F 88
	{ _JS,			_Jv				},
	{ _JNS,			_Jv				},
	{ _JP,			_Jv				},
	{ _JNP,			_Jv				},
	{ _JL,			_Jv				},
	{ _JNL,			_Jv				},
	{ _JLE,			_Jv				},
	{ _JNLE,		_Jv				},

	// 0F 90
	{ _SETO,		_Eb				},
	{ _SETNO,		_Eb				},
	{ _SETB,		_Eb				},
	{ _SETNB,		_Eb				},
	{ _SETZ,		_Eb				},
	{ _SETNZ,		_Eb				},
	{ _SETBE,		_Eb				},
	{ _SETNBE,		_Eb				},

	// 0F 98
	{ _SETS,		_Eb				},
	{ _SETNS,		_Eb				},
	{ _SETP,		_Eb				},
	{ _SETNP,		_Eb				},
	{ _SETL,		_Eb				},
	{ _SETNL,		_Eb				},
	{ _SETLE,		_Eb				},
	{ _SETNLE,		_Eb				},

	// 0F A0
	{ _PUSH,		_FS				},
	{ _POP,			_FS				},
	{ _CPUID						},
	{ _BT,			_Ev,	_Gv		},
	{ _SHLD,		_Ev,	_Gv,	_Ib	},
	{ _SHLD,		_Ev,	_Gv,	_CL	},
	{ _INVALID						},
	{ _INVALID						},

	// 0F A8
	{ _PUSH,		_GS				},
	{ _POP,			_GS				},
	{ _RSM							},
	{ _BTS,			_Ev,	_Gv		},
	{ _SHRD,		_Ev,	_Gv,	_Ib },
	{ _SHRD,		_Ev,	_Gv,	_CL },
	{ _INVALID						},
	{ _IMUL,		_Gv,	_Ev		},

	// 0F B0
	{ _CMPXCHG,		_Eb,	_Gb		},
	{ _CMPXCHG,		_Ev,	_Gv		},
	{ _LSS,			_Mp				},
	{ _BTR,			_Ev,	_Gv		},
	{ _LFS,			_Mp				},
	{ _LGS,			_Mp				},
	{ _MOVZX,		_Gv,	_Eb		},
	{ _MOVZX,		_Gv,	_Ew		},

	// 0F B8
	{ _INVALID						},
	{ _INVALID						},
	{ _Grp8,		_Ev,	_Ib		},
	{ _BTC,			_Ev,	_Gv		},
	{ _BSF,			_Gv,	_Ev		},
	{ _BSR,			_Gv,	_Ev		},
	{ _MOVSX,		_Gv,	_Eb		},
	{ _MOVSX,		_Gv,	_Ew		},

	// 0F C0
	{ _XADD,		_Eb,	_Gb		},
	{ _XADD,		_Ev,	_Gv		},
	{ _INVALID						},
	{ _INVALID						},
	{ _INVALID						},
	{ _INVALID						},
	{ _INVALID						},
	{ _Grp9,  						},

	// 0F C8
	{ _BSWAP,		_EAX			},
	{ _BSWAP,		_ECX			},
	{ _BSWAP,		_EDX			},
	{ _BSWAP,		_EBX			},
	{ _BSWAP,		_ESP			},
	{ _BSWAP,		_EBP			},
	{ _BSWAP,		_ESI			},
	{ _BSWAP,		_EDI			},
	
	// 0F D0
	{ _INVALID						},
	{ _PSRLW,		_Pq,	_Qd		},
	{ _PSRLD,		_Pq,	_Qd		},
	{ _PSRLQ,		_Pq,	_Qd		},
	{ _INVALID						},
	{ _PMULLW,		_Pq,	_Qd		},
	{ _INVALID						},
	{ _INVALID						},

	// 0F D8
	{ _PSUBUSB,		_Pq,	_Qq		},
	{ _PSUBUSW,		_Pq,	_Qq		},
	{ _INVALID						},
	{ _PAND,		_Pq,	_Qq		},
	{ _PADDUSB,		_Pq,	_Qq		},
	{ _PADDUSW,		_Pq,	_Qq		},
	{ _INVALID						},
	{ _PANDN,		_Pq,	_Qq		},

	// 0F E0
	{ _INVALID						},
	{ _PSRAW,		_Pq,	_Qd		},
	{ _PSRLD,		_Pq,	_Qd		},
	{ _PSRLQ,		_Pq,	_Qd		},
	{ _INVALID						},
	{ _PMULLW,		_Pq,	_Qd		},
	{ _INVALID						},
	{ _INVALID						},

	// 0F E8
	{ _PSUBSB,		_Pq,	_Qq		},
	{ _PSUBSW,		_Pq,	_Qq		},
	{ _INVALID						},
	{ _POR,			_Pq,	_Qq		},
	{ _PADDSB,		_Pq,	_Qq		},
	{ _PADDSW,		_Pq,	_Qq		},
	{ _INVALID						},
	{ _PXOR,		_Pq,	_Qq		},

	// 0F F0
	{ _INVALID						},
	{ _PSLLW,		_Pq,	_Qd		},
	{ _PSLLD,		_Pq,	_Qd		},
	{ _PSLLQ,		_Pq,	_Qd		},
	{ _INVALID						},
	{ _PMADDWD,		_Pq,	_Qd		},
	{ _INVALID						},
	{ _INVALID						},

	// 0F F8
	{ _PSUBB,		_Pq,	_Qq		},
	{ _PSUBW,		_Pq,	_Qq		},
	{ _PSUBD,		_Pq,	_Qq		},
	{ _INVALID						},
	{ _PADDB,		_Pq,	_Qq		},
	{ _PADDW,		_Pq,	_Qq		},
	{ _PADDD,		_Pq,	_Qq		}
};


// Group 1 opcode
const OpCode OpCodeMap_Grp1[8] =
{
	{ _ADD						},
	{ _OR						},
	{ _ADC						},
	{ _SBB						},
	{ _AND						},
	{ _SUB						},
	{ _XOR						},
	{ _CMP						}
};


// Group 2 opcode
const OpCode OpCodeMap_Grp2[8] = 
{
	{ _ROL						},
	{ _ROR						},
	{ _RCL						},
	{ _RCR						},
	{ _SHL						},
	{ _SHR						},
	{ _INVALID					},
	{ _SAR						}
};


// Group 3b opcode
const OpCode OpCodeMap_Grp3b[8] =
{
	{ _TEST,	_Eb,	_Ib		},
	{ _INVALID					},
	{ _NOT						},
	{ _NEG						},
	{ _MUL,		_AL				},
	{ _IMUL,	_AL				},
	{ _DIV,		_AL				},
	{ _IDIV,	_AL				}
};


// Group 3v opcode
const OpCode OpCodeMap_Grp3v[8] =
{
	{ _TEST,	_Ev,	_Iv		},
	{ _INVALID					},
	{ _NOT						},
	{ _NEG						},
	{ _MUL,		_eAX			},
	{ _IMUL,	_eAX			},
	{ _DIV,		_eAX			},
	{ _IDIV,	_eAX			}
};


// Group 4 opcode
const OpCode OpCodeMap_Grp4[8] =
{
	{ _INC,		_Eb				},
	{ _DEC,		_Eb				},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					}
};


// Group 5 opcode
const OpCode OpCodeMap_Grp5[8] =
{
	{ _INC,		_Ev				},
	{ _DEC,		_Ev				},
	{ _CALL,	_Ev				},
	{ _CALL,	_Ep				},
	{ _JMP,		_Ev				},
	{ _JMP,		_Ep				},
	{ _PUSH,	_Ev				},
	{ _INVALID					}
};


// group 6 opcode
const OpCode OpCodeMap_Grp6[8] =
{
	{ _SLDT,	_Ew				},
	{ _STR,		_Ew				},
	{ _LLDT,	_Ew				},
	{ _LTR,		_Ew				},
	{ _VERR,	_Ew				},
	{ _VERW,	_Ew				},
	{ _INVALID					},
	{ _INVALID					}
};


// group 7 opcode
const OpCode OpCodeMap_Grp7[8] =
{
	{ _SGDT,	_Ms				},
	{ _SIDT,	_Ms				},
	{ _LGDT,	_Ms				},
	{ _LIDT,	_Ms				},
	{ _SMSW,	_Ew				},
	{ _INVALID					},
	{ _LMSW,	_Ew				},
	{ _INVLPG					}
};


// group 8 opcode
const OpCode OpCodeMap_Grp8[8] =
{
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _BT						},
	{ _BTS						},
	{ _BTR						},
	{ _BTC						}
};


// group 9 opcode
const OpCode OpCodeMap_Grp9[8] =
{
	{ _INVALID					},
	{ _CMPXCH8,	_BMq			},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					},
	{ _INVALID					}
};	


// group A opcode
const OpCode OpCodeMap_GrpA[8] =
{
	{ _INVALID					},
	{ _INVALID					},
	{ _PSRLDWQ,	_Pq,	_Ib		},
	{ _INVALID					},
	{ _PSRADW,	_Pq,	_Ib		},
	{ _INVALID					},
	{ _PSLLDWQ,	_Pq,	_Ib		},
	{ _INVALID					}
};

