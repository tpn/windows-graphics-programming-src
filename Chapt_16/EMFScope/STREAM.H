//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : stream.h    						                                 //
//  Description: KInputFile, KOutputFile		                                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define c_bufsize 2048

#define c_EOF 0x1A

class KInputFile
{
private:
	unsigned long	m_bufpos;                 // next read position
    unsigned long	m_buflen;                 // no of bytes in buffer
	unsigned long   m_fileoffset;			  // offset to read from

    BOOL			m_eof;        

public:
    BYTE			m_buffer[c_bufsize];
    HANDLE			m_handle;

    KInputFile(void)
    {
        m_handle = INVALID_HANDLE_VALUE;
    }

    BOOL Eof(void) const
	{
		return m_eof;
	}

    unsigned GetFilesize(void)
    {
        return GetFileSize(m_handle, NULL);
    }

	void Reset(void);
    BOOL Open(const TCHAR *filename);
    void Close(void);
        
    BYTE  readbyte(void);
	DWORD readdword(void);

    int   readint(void)
    {
        return (int) readdword();
    }

	int   read(char *buffer, int size);		
};


class KOutputFile
{
public:
    BYTE			m_buffer[c_bufsize];
    unsigned long	m_bufpos;                 // next read position
    HANDLE			m_handle;

    KOutputFile(void)
    {
        m_handle = INVALID_HANDLE_VALUE;
    }
        
    BOOL Eof(void);
    BOOL Open(const char far *filename);
    void Close(void);
    void Flush(void);
        
    void write(BYTE b);
    void write(const char far *str);
        
    void writehex(BYTE b);
        
    void writehex(WORD w)
    {
        writehex(HIBYTE(w));
        writehex(LOBYTE(w));
    }
        
    void writehex(DWORD d)
    {
        writehex(HIWORD(d));
        writehex(LOWORD(d));
    }
        
    void writehex(LPVOID p)
    {
        writehex(HIWORD(p));
        write(':');
        writehex(LOWORD(p));
    }
};

