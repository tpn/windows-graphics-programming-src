//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : outpipe.cpp						                                 //
//  Description: KTextPipe: text-based output with formating                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN

#define STRICT

#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "OutPipe.h"


void KTextPipe::Newline(void)
{
	Put('\n');

	for (long i=0; i<m_indent; i++)
		Put('\t');

	m_linelen = 0;
}

	
void KTextPipe::Write(const char * str, long size)
{
	if ( str == NULL)
		return;

	while ( (size!=0) && (* str) )
	{
		Put(* str++);
		size --;
	}
}


void KTextPipe::Write(const WCHAR * str, long size)
{
	if ( str == NULL)
		return;

	while ( (size!=0) && (* str) )
	{
		Put((char)(* str ++));
		size --;
	}
}


void KTextPipe::PutQuote(char ch)
{
	switch (ch)
	{
		case '\\':
		case '\"': Put('\\'); Put(ch);	break;
		case '\n': Put('\\'); Put('n'); break;
		case '\r': Put('\\'); Put('r'); break;
		case '\t': Put('\\'); Put('t'); break; 
		default  : Put(ch);
	}
}


void KTextPipe::WriteString(const void * str, long size, bool unicode)
{
	if ( str == NULL )
	{
		Write("NULL");
		return;
	}

	Put('\"');

	if ( unicode )
	{
		const WCHAR * s = (const WCHAR * ) str;

		while ( (size != 0) && (*s) )
		{
			PutQuote((char)(*s++));
			size --;
		}
	}
	else
	{
		const char * s = (const char * ) str;

		while ( (size != 0) && (*s) )
		{
			PutQuote(*s++);
			size --;
		}
	}

	Put('\"');
}


void KTextPipe::WriteDec(unsigned long data)
{
	TCHAR temp[16];
	
	long len = 0;
	do 
	{
		temp[len++] = (char) ('0' + data % 10);
		data = data/10;
	}
	while (data!=0);

	while (len)
	{
		len --;
		Put(temp[len]);
	}
}


void KTextPipe::WriteDec(long data)
{
	if (data < 0)
	{
		Put('-');
		WriteDec((unsigned long) (0 - data));
	}
	else
		WriteDec((unsigned long) data);
}


void KTextPipe::WriteHex(unsigned long data)
{
	TCHAR temp[10];

	if (data<=9)
		Put((char)('0' + data));
	else
	{
		long len = 0;
		do
		{
			temp[len] = (char)('0' + data % 16);
		
			if (temp[len] > '9')
				temp[len]+= 'A' - '0' - 10;

			len ++;
			data /= 16;
		}
		while (data);

		Put('0');
		Put('x');
		while (len)
		{
			len --;
			Put(temp[len]);
		}
	}
}


void KTextPipe::WriteArray(const void * Array, long count, long elmsize, bool decimal)
{
	if ( ! m_bOpened )
		return;

	assert(Array);

	const char  * bArray = (const char *)  Array;
	const short * sArray = (const short *) Array;
	const long  * lArray = (const long *)  Array;
	const RECT  * rArray = (const RECT *)  Array;
	
	Put('{'); Put(' ');
	for (long i=0; i<count; i++)
	{
		if ( i )
		{
			Put(','); 
			
			if ( m_linelen > 78 )
				Newline();
			else
				Put(' ');
		}

		switch ( elmsize )
		{
			case 1:  
				if (decimal) 
					WriteDec((long) bArray[i]); 
				else 
					WriteHex(bArray[i] & 0xFF); 
				break;
			
			case 2:  
				if (decimal) 
					WriteDec((long) sArray[i]); 
				else 
					WriteHex(sArray[i] & 0xFFFF); 
				break;

			case 4:  
				if (decimal) 
					WriteDec(lArray[i]); 
				else 
					WriteHex(lArray[i]); 
				break;

			case sizeof(RECT):
				Write("{ "); WriteDec(rArray[i].left);
				Put(',');    WriteDec(rArray[i].top);
				Put(',');    WriteDec(rArray[i].right);
				Put(',');    WriteDec(rArray[i].bottom);
				Write(" }");
				break;

			default: assert(FALSE);
		}

	}

	Put(' '); Put('}');
}


void KTextPipe::Write(unsigned index, const DicItem *dic)
{
	Write(Lookup(index, dic));
}


const long * KTextPipe::Percent(TCHAR tag, const long * data, const DicItem * dic)
{
	static double dummy = sin(0.0); // just to link-in floating point support

	int len = 0;

	switch ( tag )
	{
		case '_': len = 4; break;			// skip 4 bytes

		case '0': WriteDec(data[0]); break; // decimal signed long
		case '1': WriteDec(data[1]); break;	// decimal signed long
		case '2': WriteDec(data[2]); break;	// decimal signed long
		case '3': WriteDec(data[3]); break;	// decimal signed long
		case '4': WriteDec(data[4]); break;	// decimal signed long
		case '5': WriteDec(data[5]); break;	// decimal signed long
		case '6': WriteDec(data[6]); break;	// decimal signed long
		case '7': WriteDec(data[7]); break;	// decimal signed long
		case '8': WriteDec(data[8]); break;	// decimal signed long
		case '9': WriteDec(data[9]); break;	// decimal signed long

		case 'b': WriteDec((long)(* ((unsigned char *) data)));	// signed char
				  len = 1;
				  break;

		case 'c': 
			{
				char ch = * data & 0xFF;

				if ( (ch>=' ') && (ch<=0x7F) )
					Put(ch);
				else
					Put('.');
				len = 1;
			}
			break;

		case 'd': WriteDec(                data[0]); len=4; break;  // decimal   signed long
		case 'u': WriteDec((unsigned long) data[0]); len=4; break;  // decimal unsigned long
		case 'x': WriteHex(                data[0]); len=4; break;  // hex     unsigned long

		case 'D': WriteDec((long)         (* ((short *)          data))); len=2; break; // signed short
		case 'U': WriteDec((unsigned long)(* ((unsigned short *) data))); len=2; break; // signed short
		case 'X': WriteHex(                * ((unsigned short *) data));  len=2; break; // unsigned short

		case 'S': WriteString(data, -1, true);   break; // wide string
		case 's': WriteString(data, -1, false);	 break; // ascii string

		case 'f': { TCHAR t[32]; float f = * (float *) data; _stprintf(t, _T("%8.5f"), f); Write(t); } // float
				  len = 4;
				  break;
							  
		case 'n': m_nCount++;					// sequence count: 1 1 2 2 ...
				  WriteDec((long)(m_nCount/2));
				  break;
	
		case 'm': m_mCount++;					// sequence count: 1 1 2 2 ...
				  WriteDec((long)(m_mCount/2));
				  break;
							  							  
		case 'L': Write(data[0], dic); len = 4; // dic
				  break;

		default : assert(false);
	}

	return (const long *) ( (const char *) data + len);
}


const long * KTextPipe::Pound(TCHAR tag, const long * data, const DicItem * dic)
{
	assert(false);

	return data;
}


void KTextPipe::Format(const TCHAR * pattern, const void * vdata, const DicItem * dic)
{
	if ( pattern == NULL )
		return;

	while ( * pattern )
	{
		switch ( * pattern )
		{
			case '\n':	
				Newline();		
				break;

			case '#' :
				pattern ++;
				vdata = Pound( * pattern, (const long *) vdata, dic);
				break;

			case '%' : 
				pattern ++;
				
				vdata = Percent( * pattern, (const long *) vdata, dic);
				break;

			default  :
				Put(* pattern);
		}

		pattern ++;
	}
}
