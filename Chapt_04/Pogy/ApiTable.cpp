//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : apitable.cpp			                                             //
//  Description: KTarget class                                                       //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <commctrl.h>
#include <assert.h>
#include <tchar.h>

#include "..\..\include\listview.h"

#include "..\Diver\Diver.h"

#include "ApiTable.h"

const TCHAR sec_target    [] = _T("Target");
const TCHAR sec_module    [] = _T("Module");


BOOL KTarget::ParseMethod(KProfile & Profile, KMethod & Method)
{
    char     sName[64];
    char sTypeName[64];

    // result_type method_name(type1, type2, ... typen), opt

    Method.nOpt    = 0;
    Method.nParaNo = 1;

    // result_type method_name(type1, type2, ... typen), opt
    if ( ! Profile.ReadIdentifier(sTypeName, sizeof(sTypeName)) )
		return FALSE;

	TCHAR szParaName[MAX_PATH];

	_tcscpy(szParaName, sTypeName);

    Profile.ReadIdentifier(sName, sizeof(sName));
                    
    // (type1, type2, ... typen) [, <hex_address>, <hex_syscallid>
    if (  Profile.ReadDelimiter('(') )
    if (! Profile.ReadDelimiter(')') )
    {
		do
        {
            if ( Profile.ReadIdentifier(sTypeName, sizeof(sTypeName), '*') )
            //  if (Method.nParaNo < MAXPARANO)
				{
					if ( Method.nParaNo )
						strcat(szParaName, " ");

                    strcat(szParaName, sTypeName);
					Method.nParaNo ++;
				}
            //  else
            //      assert(FALSE);
        }
        while (Profile.ReadDelimiter(','));

        Profile.ReadDelimiter(')');

		Method.pFunction  = (void *) Profile.ReadHex();
		Method.nSysCallId =			 Profile.ReadHex();
    }
	
	Method.aParaName   = AddAtom(szParaName);
	Method.aMethodName = AddAtom(sName);

	if ( MethodTable_nElement < METHODTABLESIZE )
		MethodTable[MethodTable_nElement++] = Method;
	else
		MessageBox(NULL, "Too many functions/methods", "Pogy", MB_OK);
	
	Profile.SkipLine();

    return TRUE;
}


// hex_vtable hex_queryinterface dec_methodno {guid} interface_name
BOOL KTarget::ParseInterface(KProfile & Profile, KMethod & Method)
{
	KInterface inf;

	memset(&inf, 0, sizeof(inf));

	inf.aCaller         = InterfaceTable[Method.nInterface].aCaller;
	inf.aCallee         = InterfaceTable[Method.nInterface].aCallee;
	inf.vTable          = Profile.ReadHex();
	inf.pQueryInterface = Profile.ReadHex();
	inf.nMethodNo       = Profile.ReadDec();
	
	if ( ! Profile.ReadGuid(inf.iid) )
		return FALSE;

	TCHAR sName[64];
	
	// interface name
	if ( ! Profile.ReadIdentifier(sName, sizeof(sName)/sizeof(TCHAR)) )
		return FALSE;

	inf.aInterface = AddAtom(sName);

	InterfaceTable[InterfaceTable_nElement++] = inf;  // add to interface table

	return TRUE;
}


int KTarget::ParseAPISet(HINSTANCE hInstance, const char * szAPISetName, KMethod & Method)
{
	KProfile pro;
	TCHAR sName[MAX_PATH];

	_tcscpy(sName, szAPISetName);
	_tcscat(sName, ".ini");

	if ( ! pro.SetFileName(hInstance, sName) )
		return 0;

	int no = 0;
	
	// WINAPI
	if ( pro.ReadSection(szAPISetName) )
		while ( ParseMethod(pro, Method) )
		{
			no ++;
		}

	_tcscpy(sName, "COM_");
	_tcscat(sName, szAPISetName);	// eg: [COM_ddraw]

	if ( pro.ReadSection(sName) )
	{
		int infno = 0;
		
		// parse all the interfaces first
		while ( ParseInterface(pro, Method) )
		{
			pro.SkipLine();
			infno ++;
		}

		// then load all their methods
		while ( infno )
		{
			int i = InterfaceTable_nElement - infno;

			Method.nInterface = i;

			if ( pro.ReadSection(GetAtomName(InterfaceTable[i].aInterface)) )
				while ( ParseMethod(pro, Method) )
				{
					no ++;
				}

			infno --;
		}
	}

	return no;
}


void KTarget::Initialize(HINSTANCE hInstance, const TCHAR *Target, const TCHAR *ProjectFile)
{
    KProfile Profile;

    TCHAR    sName[64];
    KMethod  Method;

    _tcscpy(TargetName, Target);

    InterfaceTable_nElement = 0;
    MethodTable_nElement = 0;

    if ( ! Profile.SetFileName(hInstance, ProjectFile) )
	{
		assert(FALSE);
		return;
	}		

    // read module list
	Profile.ReadSection(sec_module);

	while ( Profile.ReadIdentifier(sName, sizeof(sName), '.') )
	{
		KInterface inf;

		memset(&inf, 0, sizeof(inf));
		inf.aCaller = AddAtom(sName);

		Profile.ReadIdentifier(sName, sizeof(sName), '.');
		inf.aCallee = AddAtom(sName);
		
		Method.nInterface = InterfaceTable_nElement;

		InterfaceTable[InterfaceTable_nElement++] = inf;	// add to interface table
		
		Profile.ReadIdentifier(sName, sizeof(sName), '.');

		ParseAPISet(hInstance, sName, Method);

		Profile.SkipLine();
	}
}


int KTarget::Add2ListView(KListView & listview)
{
    KMethod * m = MethodTable;

    for (int i=0; i<MethodTable_nElement; i++, m++)
    {
		KInterface & inf = InterfaceTable[m->nInterface];

        if ( inf.aInterface==0 )      // win32 api
        {
			if ( m->nSysCallId )
				listview.AddItem(0, GetAtomName(inf.aCaller), 2);
			else
				listview.AddItem(0, GetAtomName(inf.aCaller), 0);
            
			listview.AddItem(1, GetAtomName(inf.aCallee));
        }
        else                                                    // Com method
        {
            listview.AddItem(0, GetAtomName(inf.aCallee), 1);
            listview.AddItem(1, GetAtomName(inf.aInterface));
        }

        listview.AddItem(2, GetAtomName(m->aMethodName));

		if ( m->nSysCallId )
		{
			char temp[6];

			wsprintf(temp, "%x", m->nSysCallId);
			
			listview.AddItem(3, temp);
		}
    }
    
    return MethodTable_nElement;
}


int KTarget::InterceptTarget(HWND hSender, HWND hReceiver, BOOL logcall, BOOL dispcall)
{
    KPost Post(hSender, hReceiver);

    int  curInterface = -1;
    int  curModule    = -1;
    BOOL isWin32API   = FALSE;

    for (int m=0; m<MethodTable_nElement; m++)
    {
        if ( MethodTable[m].nInterface != curInterface )
        {
			curInterface = MethodTable[m].nInterface;

            KInterface & inf = InterfaceTable[curInterface];

			isWin32API = inf.aInterface==0;

            curModule = Post.LoadModule(GetAtomName(inf.aCaller),
                                        GetAtomName(inf.aCallee),
										GetAtomName(inf.aInterface),
										inf.vTable,
										inf.pQueryInterface,
										inf.nMethodNo,
										inf.iid);
        }

        if (curModule >= 0)
            if (isWin32API)
            {
				if ( MethodTable[m].pFunction )
					Post.AddFunc(GetAtomName(MethodTable[m].aMethodName), 
                             MethodTable[m].nOpt,
                             MethodTable[m].nParaNo,
                             GetAtomName(MethodTable[m].aParaName),
							 FUNC_SYSCALL, MethodTable[m].pFunction);
				else
					Post.AddFunc(GetAtomName(MethodTable[m].aMethodName), 
                             MethodTable[m].nOpt,
                             MethodTable[m].nParaNo,
                             GetAtomName(MethodTable[m].aParaName),
							 FUNC_WIN32API, NULL);
            }
            else
            {
                Post.AddFunc(GetAtomName(MethodTable[m].aMethodName), 
							 MethodTable[m].nOpt,
                             MethodTable[m].nParaNo,
                             GetAtomName(MethodTable[m].aParaName),
							 FUNC_COMMETHOD, NULL);
            }
    }

    return Post.Intercept(logcall, dispcall);
}


//-------------------------------------------//

int KApiTable::AddTargets(HINSTANCE hInstance, HWND hWnd)
{
    KProfile Profile;
    int      no;

    Profile.SetFileName(hInstance, _T("Pogy.ini"));
    
    no = 0;
    for (int i=1; Profile.ReadString(sec_target, i); i++)
    {
        TCHAR TargetList[MAX_PATH];

        TargetList[0] = 0;

        // target_name ( ini_file ) { , target_name ( ini_file ) }
        while ( TRUE )
        {
            BOOL cont = FALSE;
            char Target[64];

            if ( Profile.ReadIdentifier(Target, sizeof(Target)/sizeof(TCHAR), '.') )
            if ( Profile.ReadDelimiter('(') )
            if ( Profile.ReadIdentifier(NULL, 0, '.') )
            if ( Profile.ReadDelimiter(')') )
            {
                cont = TRUE;

                if ( strlen(TargetList) )
                    strcat(TargetList, ", ");
                strcat(TargetList, Target);
            }

            if ( !cont || ! Profile.ReadDelimiter(',') )
                break;
        }
        
        if ( strlen(TargetList) )
        {
            SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) TargetList);
            no ++;
        }
        else
            break;
    }

    return no;
}


void KApiTable::ClearTargets(void)
{
    for (int i=0; i<TargetNo; i++)
        if (TargetList[i])
        {
            delete TargetList[i];
            TargetList[i] = NULL;
        }

    TargetNo = 0;
}


int KApiTable::Initialize(HINSTANCE hInstance, int projno)
{
    KProfile Profile;

    ClearTargets();

    Profile.SetFileName(hInstance, _T("Pogy.ini"));

    if ( Profile.ReadString(sec_target, projno+1) )
        // target_name ( ini_file ) { , target_name ( ini_file ) }
        while ( TargetNo < MAXTARGETNO)
        {
            BOOL cont = FALSE;
            TCHAR Target[64];
            TCHAR IniFile[64];

            if ( Profile.ReadIdentifier(Target, sizeof(Target)/sizeof(TCHAR), '.') )
            if ( Profile.ReadDelimiter('(') )
            if ( Profile.ReadIdentifier(IniFile, sizeof(IniFile)/sizeof(TCHAR), '.') )
            if ( Profile.ReadDelimiter(')') )
            {
                TargetList[TargetNo] = new KTarget;
                    
                TargetList[TargetNo]->Initialize(hInstance, Target, IniFile);
                TargetNo++;

                cont = TRUE;
            }

            if ( !cont || ! Profile.ReadDelimiter(',') )
                break;
        }

    return TRUE;
}


const char * KApiTable::MainTarget(void)
{
    if ( TargetNo )
        return TargetList[0]->TargetName;
    else
        return NULL;
}


int KApiTable::Add2ListView(KListView & listview)
{
    listview.DeleteAll();

    for (int i=0, sum=0; i<TargetNo; i++)
        sum += TargetList[i]->Add2ListView(listview);
    
    return sum;
}


int KApiTable::InterceptApi(HWND hSender, HWND hReceiver, BOOL logcall, BOOL dispcall)
{
    if ( TargetNo )
        return TargetList[0]->InterceptTarget(hSender, hReceiver, logcall, dispcall);
    else
        return FALSE;
}
