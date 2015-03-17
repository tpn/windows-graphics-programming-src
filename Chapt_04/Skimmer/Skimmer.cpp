//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : skimmer.cpp			                                             //
//  Description: Simple parser for Win32 header file                                 //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>


class KParser
{
	typedef enum { MAX_TYPES = 512,
				   MAX_NAMELEN = 64
				};
	HANDLE str;
	HANDLE hmap;
	char * view;
	
	int	   size;
	int	   pos;

	char   typnam[MAX_TYPES][MAX_NAMELEN];
	int	   types;

public:

	bool Open(const char * file)
	{
		str = CreateFile(file, 
					       GENERIC_READ, FILE_SHARE_READ, NULL,
					           OPEN_EXISTING,
					           FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

		if ( str == INVALID_HANDLE_VALUE )
			return false;

		size = GetFileSize(str, NULL);

		hmap = CreateFileMapping(str, NULL, PAGE_READONLY, 0, 0, NULL);

		if ( hmap )
			view = (char *) MapViewOfFile(hmap, FILE_MAP_READ, 0, 0, 0);

		ch = ' ';
		pos = 0;

		types = 0;

		return true;
	}

	char ch;
	char token[MAX_PATH];

	void Close(void)
	{
		UnmapViewOfFile(view);
		CloseHandle(hmap);
		CloseHandle(str);
	}

	bool Eof(void)
	{
		return pos >= size;
	}

	bool Equal(const char *check);

	void Nextch(void);
	void NextToken(void);

	void CheckType(char *word);
	void CheckTypes(char * line);
	void DumpTypes();
	
	void CheckAPI(void);
};


void KParser::CheckType(char * word)
{
	while ( * word==' ' )
		word ++;

	int len = strlen(word);

	while ( len && word[len-1]==' ' )
	{
		len --;
		word[len]=0;
	}

	if ( len==0 )
		return;

	for (int i=0; i<types; i++)
		if ( strcmp(word, typnam[i])==0 )
			return;

	strcpy(typnam[types], word);
	types ++;
}


void KParser::DumpTypes(void)
{
	printf("\n");
	printf("[Types]\n");

	for (int i=0; i<types; i++)
	{
		printf(typnam[i]);
	    printf("\n");
	}
}


// type func(t1,t2,...tn)
//      ^15
void KParser::CheckTypes(char * line)
{
	line[14] = 0;

	CheckType(line);

	line = strchr(line+15, '(');

	if ( line )
	{
		line ++;
		char * p;

		while ( p = strchr(line, ',') )
		{
			* p = 0;
			CheckType(line);

			line = p+1;
		}

		p = strchr(line, ')');

		if ( p )
		{
			* p = 0;
			CheckType(line);
		}
	}
}


void KParser::Nextch(void)
{
    do
    { 
		ch = * view ++; 
		pos ++;
	}
    while (ch=='\r');   // skip '\r'
    
    if ( ch=='\t' ) 
		ch=' ';         // map tab to space
}


void KParser::NextToken(void)
{
again:
	token[0] = 0;

	if ( Eof() )
		return;

	while ( ch==' ' )
		Nextch();

	if ( ch=='\n' )
	{
		token[0] = ch;
		token[1] = 0;

		Nextch();
		return;
	}
	
	if ( ch=='/' )
	{
		Nextch();
	
		if ( ch=='/' )				// single line commented
		{
			while ( ch!='\n') 
				Nextch();
			Nextch();
			goto again;
		}

		if ( ch=='*' )				// block comment /* ... */, nesting not allowed
		{
			Nextch();
			
		more:
			while ( ch!='*')
				Nextch();

			Nextch();
			if ( ch=='/' )
			{
				Nextch();
				goto again;
			}
			else
				goto more;
		}

		token[0] = '/';
		token[1] = 0;

		return;
	}

	int len = 0;
	
	// simple single character delimiters
	switch ( ch )
	{
		case '(':
		case ')':
		case ',':
		case ';':
		case '[':
		case ']':
		case '+':
		case '*': 
			token[len++] = ch;
			token[len] = 0;
			Nextch();
			return;
	}
	
	// identifiers or keywords
	if ( (ch=='_') || isalpha(ch) )
	{
		token[len++] = ch;
		Nextch();

		while ( (ch=='_') || isalpha(ch) || isdigit(ch) )
		{
			token[len++] = ch;
			Nextch();
		}

		token[len] = 0;
		return;
	}

	// others
	while ( (ch!=' ') && (ch!='\n') )
	{
		token[len++] = ch;
		Nextch();
	}

	token[len] = 0;
}


bool KParser::Equal(const char * check)
{
	return strcmp(token, check)==0;
}


// DECLARE_INTERFACE_( IDirectDraw, IUnknown )
//
//  STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
//  STDMETHOD_(ULONG,AddRef) (THIS)  PURE;

void KParser::CheckAPI(void)
{
	NextToken();

	// first token after a new line
	if ( Equal("DECLARE_INTERFACE_") )
	{
		NextToken();		// (
		NextToken();		// interface name

		printf("\n[%s]\n", token);

		return;
	}

	// <newline> WINGDIAPI
	char line[MAX_PATH];
	
	line[0] = 0;

	int isapi = 0;
	
	int identadmit = 100;

	while ( ! Eof() && (isapi || token[0]!='\n') )
	{
		// rejecting pragmas
		if ( Equal("#define") || Equal("#ifdef") || Equal("#ifndef") ||
			 Equal("#else") || Equal("#endif") || Equal("typedef") 
		   )
			return;

		// API signals
		if ( Equal("WINGDIAPI") || Equal("WINUSERAPI") || 
			 Equal("WINAPI") || Equal("APIENTRY") )
		{
			isapi = true;
			NextToken();
			continue;
		}

		//  STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj) PURE;
		if ( Equal("STDMETHOD") )
		{
			isapi = true;

			NextToken(); // (
			strcat(line, "HRESULT");
			while ( strlen(line)<15 )
				strcat(line, " ");

			NextToken();			// name
			strcat(line, token);
			NextToken();		// )

			NextToken();		// (
		}

		// STDMETHOD_(ULONG,AddRef) (THIS)  PURE;
		if ( Equal("STDMETHOD_") )
		{
			isapi = true;

			NextToken(); // (
			NextToken(); // type
			strcat(line, token);
			while ( strlen(line)<15 )
				strcat(line, " ");

			NextToken();			// ,
			NextToken();			// name
			strcat(line, token);
			NextToken();		// )

			NextToken();		// (
		}
	
		if ( Equal("THIS_") )
		{
			strcat(line, "THIS");
			strcpy(token, ",");
		}

		// skipped words
		if ( Equal("IN")	   || Equal("OUT") || 
			 Equal("CONST") || Equal("FAR")  || 
			 Equal("extern")   || Equal("PURE") ||
			 token[0]=='\n'     
           )
		{
			NextToken();
			continue;
		}

		// ; is the end
		if ( token[0] == ';' )
		{
			if ( isapi )
			{
				printf(line);
				printf("\n");

				CheckTypes(line);
			}

			NextToken();
			return;
		}

		char copy[MAX_PATH];
		strcpy(copy, token);

		NextToken();

		// before adding '(', add space to function name
		if ( token[0]=='(' )
			while ( strlen(line)<15 )
				strcat(line, " ");

		// skip parameter name
		if ( copy[0]=='_' || isalpha(copy[0]) )
		{
			if ( identadmit )
			{
				identadmit --;
				strcat(line, copy);
			}
		}
		else
			strcat(line, copy);

		// after '(' or ',', one identifier only
		if ( copy[0]=='(' || copy[0]==',' )
			identadmit = 1;
	}
}			


int main( int argc, char *argv[ ], char *envp[ ] )
{
	KParser parser;

	if ( argc<2 )
	{
		printf("Skimmer: extracts function prototypes from Windows API header file\n");
		printf("Usage: skimmer <headerfilename>\n");
		return -1;
	}

	if ( !parser.Open(argv[1]) )
	{
		printf("Unable to open %s.\n", argv[1]);
		return -1;
	}

	parser.token[0] = '\n';
	parser.token[1] = 0;

	printf("[%s]\n", argv[1]);

	while ( ! parser.Eof() )
	{
		if ( parser.token[0] == '\n' ) // starting with a newline, check API
			parser.CheckAPI();
		else
			parser.NextToken();
	}
	
	parser.DumpTypes();

	parser.Close();

	return 0;
}