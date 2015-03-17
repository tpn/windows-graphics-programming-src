#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : profile.h				                                             //
//  Description: .ini file API wrapper                                               //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KProfile
{
    TCHAR         sFileName[MAX_PATH];
    const TCHAR * p;

public:
    TCHAR         sLine[32768];

    KProfile(void)
    {
        sFileName[0] = 0;
        sLine[0]     = 0;
        p            = sLine;
    }

    BOOL     SetFileName(HINSTANCE hInstance, const TCHAR *filename);

    int      ReadString(const TCHAR *sec, int no);
    int      ReadString(const TCHAR *sec, const TCHAR * key);
    int      ReadInt(const TCHAR *section, const TCHAR *key, int dflt) const;

    int      ReadSection(const TCHAR *sec)
    {
        sLine[0] = 0;
        sLine[1] = 0;
        p = sLine;

        return GetPrivateProfileSection(sec, sLine, sizeof(sLine)-2, sFileName);
    }

	void     SkipLine(void)
	{
		while ( *p!=0 )
			p ++;

		p++;
	}

    BOOL     Write(const TCHAR *section, const TCHAR *key, int value) const;
    BOOL     Write(const TCHAR *section, const TCHAR *key, const TCHAR *value) const;

    BOOL     ReadDelimiter(TCHAR ch);

    unsigned ReadHex(void);
    unsigned ReadDec(void);
    int      ReadIdentifier(TCHAR *rslt, int maxlength, TCHAR extra='_');
    BOOL     ReadGuid(GUID & guid);
};


