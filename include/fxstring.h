//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : fixstring.h					                                     //
//  Description: Template class for fixed size string                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

// Fixed size character string
template<int nMaxSize=256> 
class KString
{
	TCHAR Buffer[nMaxSize];
	int   nLength;

public:
	KString(void)
	{
		Buffer[0] = 0;
		nLength = 0;
	}

	int GetLength(void) const
	{
		return nLength;
	}

	operator LPCTSTR() const
	{
		return Buffer;
	}

	LPCTSTR Append(LPCTSTR tail)
	{
		lstrcat(Buffer+nLength, tail);
		nLength += lstrlen(tail);
		
		assert(nLength<nMaxSize);

		return Buffer;
	}

	LPCTSTR Append(LPCTSTR format, unsigned number)
	{
		wsprintf(Buffer+nLength, format, number);
		nLength = lstrlen(Buffer);
		
		assert(nLength<nMaxSize);

		return Buffer;
	}

	LPCTSTR Format(LPCTSTR format, unsigned number)
	{
		wsprintf(Buffer, format, number);
		nLength = lstrlen(Buffer);
		
		assert(nLength<nMaxSize);

		return Buffer;
	}

};
