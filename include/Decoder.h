//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : decoder.h				                                             //
//  Description: IAtomTable, IDecoder abstract base classes                          //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#ifndef DECODER_INC

#define DECODER_INC

struct IAtomTable
{
	virtual ATOM         AddAtom(const char * name) = 0;
	virtual const char * GetAtomName(ATOM atom) = 0;
};
                                         

struct IDecoder
{
	IDecoder * pNextDecoder;

	virtual bool Initialize(IAtomTable * pAtomTable) = 0;
	
	virtual int Decode(ATOM typ, const void * pValue, char * szBuffer, int nBufferSize) = 0;
};

typedef IDecoder * (WINAPI * DecoderCreator)(void);

#endif