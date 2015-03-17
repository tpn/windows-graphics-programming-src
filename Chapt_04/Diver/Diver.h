//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : diver.h				                                             //
//  Description: Spying DLL interface                                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define REPLY_DIVERSTART 101
#define REPLY_DIVERSTOP  102

#define FUNC_WIN32API	0
#define FUNC_COMMETHOD  1
#define FUNC_SYSCALL	2

BOOL StopDiver(void);

// Commands from outside

typedef enum
{
    C_LOADMODULE,
    C_ADDFUNC,
    C_INTERCEPT
} CommandMessageId;



class KLoadModule
{
public:

    char     m_caller[32];
    char     m_callee[32];
	char     m_intrfc[32];
	
	unsigned vtable;
	unsigned queryinterface;
	int		 methodno;
	GUID	 iid;

    static KLoadModule * Parse(COPYDATASTRUCT * pcds)
    {
        assert(pcds);
        assert(pcds->cbData == sizeof(KLoadModule));
        
        return (KLoadModule *) pcds->lpData;
    }

    
};


// const int MAXPARANO = 15;
                                    
class KAddFunc
{
public:

    unsigned short m_ord;
    unsigned short m_cls;
	unsigned short m_kind;
	const void *   m_oldaddress;

    char       m_name[64];
    int        m_parano;
    char	   m_paratype[128];

    static KAddFunc * Parse(COPYDATASTRUCT * pcds)
    {
        assert(pcds);
        assert(pcds->cbData == sizeof(KAddFunc));

        return (KAddFunc *) pcds->lpData;
    }

};


class KOptions
{
public:
    BOOL    bLogCall;
    BOOL    bDispCall;

    static KOptions * Parse(COPYDATASTRUCT * pcds)
    {
        assert(pcds);
        assert(pcds->cbData == sizeof(KOptions));

        return (KOptions *) pcds->lpData;
    }

};


class KPost
{
	int	 seq;

public:
    HWND hSender;
    HWND hReceiver;
	
    KPost(HWND Sender, HWND Receiver)
    {
        hSender   = Sender;
        hReceiver = Receiver;
		seq		  = 0;
    }
    
	int LoadModule(const char *caller, const char *callee,
					  const char *intfc, unsigned vtable, unsigned queryinterface,
					  int methodno, GUID & iid);
       
    int AddFunc(const char *name, unsigned cls, int parano, const char * paratype, int kind, const void * oldaddr);

    int Intercept(BOOL logcall, BOOL dispcall);
};


typedef enum
    {
        Diver_SetController,

        Diver_Install,
        Diver_UnInstall
    };


#ifndef DIVER

void __declspec(dllexport) SetupDiver(int nOpt, HWND hWnd);
BOOL __declspec(dllexport) StartDiver(void);

#endif