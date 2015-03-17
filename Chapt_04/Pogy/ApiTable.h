#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : apitable.h				                                             //
//  Description: KApiTable class                                                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include "..\..\include\Profile.h"
#include "..\..\include\atom.h"

class KMethod
{
public:
	void *		   pFunction;

    unsigned short nOpt;
    short		   nInterface;
	short		   nSysCallId;
    short		   nParaNo;
	
	ATOM		   aMethodName;
    ATOM		   aParaName; // type1 type2 type3 .. typen
};


class KInterface
{
public:
	ATOM	  aCaller;
	ATOM	  aCallee;
	ATOM	  aInterface;

	unsigned  vTable;
	unsigned  pQueryInterface;
	int		  nMethodNo;
	GUID	  iid;
};


class KTarget : public KAtomTable
{
    typedef enum
	{ 
		INTERFACETABLESIZE = 64,
		METHODTABLESIZE    = 1024
    }; 
	
	KInterface  InterfaceTable[INTERFACETABLESIZE];
	int		    InterfaceTable_nElement;

	KMethod		MethodTable[METHODTABLESIZE];
	int			MethodTable_nElement;

public:
	KTarget()
	{
		InterfaceTable_nElement = 0;
		   MethodTable_nElement = 0;
	}

    TCHAR TargetName[64];

	int  ParseAPISet(HINSTANCE hInstance, const char * szAPISetName, KMethod & Method);

    BOOL ParseMethod(KProfile & Profile, KMethod & Method);
	BOOL ParseInterface(KProfile & Profile, KMethod & Win32Method);

    void Initialize(HINSTANCE hInstance, const TCHAR *Target, const TCHAR *ProjectFile);
    int  Add2ListView(KListView & listview);
    int  InterceptTarget(HWND hSender, HWND hReceiver, BOOL logcall, BOOL dispcall);
    
};


const int MAXTARGETNO = 10;

class KApiTable
{
    KTarget * TargetList[MAXTARGETNO];    
    int       TargetNo;
    
    void ClearTargets(void);

public:

    KApiTable()
    {
        TargetNo = 0;
    }

    ~KApiTable()
    {
        ClearTargets();
    }

    const TCHAR *MainTarget(void);

    int Add2ListView(KListView & listview);
    int AddTargets(HINSTANCE hInstance, HWND hWnd);
    
    int Initialize(HINSTANCE hInstance, int projno);
    int InterceptApi(HWND hSender, HWND hReceiver, BOOL logcall, BOOL dispcall);
};

