//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : logfont.h					                                         //
//  Description: Logical font wrapper                                                //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KLogFont
{
public:

	LOGFONTA		m_LogFont;

	KLogFont(int height, const TCHAR * FaceName )
	{
		m_LogFont.lfHeight	       = height;
		m_LogFont.lfWidth	       = 0;
		m_LogFont.lfEscapement     = 0;
		m_LogFont.lfOrientation    = 0;
		m_LogFont.lfWeight         = FW_NORMAL;
		m_LogFont.lfItalic         = FALSE;
		m_LogFont.lfUnderline      = FALSE;
		m_LogFont.lfStrikeOut      = FALSE;
		m_LogFont.lfCharSet        = DEFAULT_CHARSET;
		m_LogFont.lfOutPrecision   = OUT_TT_PRECIS;
		m_LogFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
		m_LogFont.lfQuality        = ANTIALIASED_QUALITY;
		m_LogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		_tcscpy(m_LogFont.lfFaceName, FaceName);
	}

	void SetHeight(int height)   { m_LogFont.lfHeight = height; }
	void SetWidth (int width )   { m_LogFont.lfWidth  = width;  }
	VOID SetCharSet(BYTE cs  )   { m_LogFont.lfCharSet = cs;    }

	HFONT Create(void)
	{
		return ::CreateFontIndirect(& m_LogFont);
	}

};

