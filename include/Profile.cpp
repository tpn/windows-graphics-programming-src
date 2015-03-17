//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : profile.cpp 		                                                 //
//  Description: .ini file access                                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#define STRICT
#include <windows.h>
#include <assert.h>

#include "Profile.h"


BOOL KProfile::SetFileName(HINSTANCE hInstance, const TCHAR *filename)
{
    OFSTRUCT ofs;

    if (hInstance)
    {
        GetModuleFileName(hInstance, sFileName, sizeof(sFileName));

        TCHAR * p = sFileName;

        while ( strchr(p, ':') || strchr(p, '\\') )
            if ( strchr(p, ':') )
                p = strchr(p, ':') + 1;
            else
                p = strchr(p, '\\') + 1;

        strcpy(p, filename);
    }
    else
        strcpy(sFileName, filename);

    return OpenFile(sFileName, &ofs, OF_EXIST) != HFILE_ERROR;
}


int KProfile::ReadString(const TCHAR *section, int no)
{
    TCHAR key_no[10];

    wsprintf(key_no, "%d", no);

    p = sLine;

    return GetPrivateProfileString(section, key_no, NULL, sLine, sizeof(sLine), sFileName);
}


int KProfile::ReadString(const TCHAR *section, const TCHAR *key)
{
    p = sLine;

    sLine[0] = 0;
    return GetPrivateProfileString(section, key, NULL, sLine, sizeof(sLine), sFileName);
}


int KProfile::ReadInt(const TCHAR *section, const TCHAR *key, int dflt) const
{
    return GetPrivateProfileInt(section, key, dflt, sFileName);
}


BOOL KProfile::Write(const TCHAR *section, const TCHAR *key, int value) const
{
    TCHAR temp[10];

    wsprintf(temp, "%d", value);
    
    return WritePrivateProfileString(section, key, temp, sFileName);
}


BOOL KProfile::Write(const TCHAR *section, const TCHAR *key, const TCHAR *value) const
{
    return WritePrivateProfileString(section, key, value, sFileName);
}


BOOL KProfile::ReadDelimiter(TCHAR ch)
{
    // skip white space 
    while ( p && isspace(*p) )
        p ++;

    if ( p && (*p == ch) )
    {
        p ++;
        return TRUE;
    }
    else
        return FALSE;
}


// read the next <nondex> <hexdigit> 
unsigned KProfile::ReadHex(void)
{
    unsigned rslt = 0;

	if ( p )
	{
		while ( *p && !isxdigit(*p) )
			p ++;

		while ( isxdigit(*p) )
		{
			if (*p<='9')
				rslt = (rslt << 4) | (*p - '0');
			else
				rslt = (rslt << 4) | ( (*p - 'A') % 32 + 10 );
			p++;
		}
	}

    return rslt;
}


// read the next decimal number
unsigned KProfile::ReadDec(void)
{
    unsigned rslt = 0;

    if ( p )
    {
        while ( *p && !isdigit(*p) )
            p ++;

        while ( isdigit(*p) )
        {
            rslt = rslt * 10 + ( *p - '0' );
            p ++;
        }
    }

    return rslt;
}


// read the next identifier
int KProfile::ReadIdentifier(TCHAR *name, int maxlength, TCHAR extra)
{
    int len = 0;

    if ( p )
    {
        // skip until first identifier character
        while ( *p && ! isalpha(*p) && (*p!='_') )
            p ++;

        while ( isalpha(*p) || isdigit(*p) || (*p=='_') || (*p==extra) )
        {
            if (name)
            {
                if ( len < maxlength )
                    name[len++] = *p;
            }
            else
                len ++;
            
            p ++;
        }
    }

    if (name)
      name[len] = 0;

    return len;
}


// read the next: {03B034D1-68AF-11D1-AB-9F-F4-1A-FD-C0-00-00}
BOOL KProfile::ReadGuid(GUID & guid)
{
    if ( ReadDelimiter('{') )
    {
        guid.Data1 = ReadHex();   ReadDelimiter('-');
        guid.Data2 = ReadHex();   ReadDelimiter('-');
        guid.Data3 = ReadHex();   ReadDelimiter('-');

        ReadDelimiter('-');
        
        for (int i=0; i<8; i++)
        {
            guid.Data4[i] = ReadHex();

            if (i<7)
                ReadDelimiter('-');
        }

        return ReadDelimiter('}');
    }
    else
        return FALSE;
}


