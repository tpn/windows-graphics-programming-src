//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : spheon32.h							                                 //
//  Description: Spying DLL entry point                                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#ifdef SPEHON32

#define LIBAPI __declspec(dllexport)

#else

#define LIBAPI __declspec(dllimport)

#endif

#define Prop_Server "Yuan"
#define Prop_Client "Feng"
#define ItsMe       "[EmfScope 95 1.01]" 

LIBAPI BOOL WINAPI SetSpoolerHook(DWORD dwThreadId, HWND hwnd_Spooler);


typedef enum
	{
		id_seed     = 0x01041961,
		id_emffile,
		id_time,
		id_activity,
		id_device,
		id_title
	} ReportIdentifier;

#define REC_DC  0x02000005
#define REC_END 0x02444E45
#define REC_STD 0x02445453
#define REC_FTM 0x024D5446
#define REC_ENP 0x02504E45
#define REC_STP 0x02505453

typedef enum 
	{	act_done, 
		act_despool, 
		act_delete, 
		act_copy 
	} Activity;

extern HWND      hwnd_Spooler;

