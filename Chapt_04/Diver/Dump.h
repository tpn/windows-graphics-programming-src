//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dump.h  				                                             //
//  Description: Simple data dump class                                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KDump
{
	HFILE      m_handle;
        
    char       m_buffer[4096];
    unsigned   m_bufpos;
    int		   m_field;	    
        
public:        

    KDump()
    {
        m_handle = HFILE_ERROR;
        m_bufpos = 0;
		m_field  = 0;
    }
    
    void Newline(void);

    void Flush(void);

	void Write(char ch);
	void Write(const void * pData, int len);

	void Seperator(void);
	
	void WriteField(const char * pStr);
	void WriteField(const char * pStr, int nMaxSize);
	void WriteField(unsigned value, ATOM typ);

    int  BeginDump(const char *name);
    void EndDump(void);
};
