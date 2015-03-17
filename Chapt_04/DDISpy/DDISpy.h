//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : ddispy.h				                                             //
//  Description: Interface to DDI Spy driver                                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

// instance information associated with device
class KDeviceExtension
{
    ULONG  StateVariable;
};


// Define the various device type values.  Range 0-32767 reserved by
// MS, and range 32768-65535 are reserved for use by customers.
#define FILE_DEVICE_DDISPY  0x00008302

// Macro definition for defining IOCTL and FSCTL function control codes. 
// Rabge 0-2047  Note range0-2047 reserved by MS, and range 2048-4095 
// are reserved for customers.

#define DDISPY_IOCTL_START  0x8301
#define DDISPY_IOCTL_END    0x8302
#define DDISPY_IOCTL_READ   0x8303
#define DDISPY_IOCTL_REPORT 0x8304

#define DDISPY_(i)	 CTL_CODE(FILE_DEVICE_DDISPY, i, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define DDISPY_START  DDISPY_(DDISPY_IOCTL_START )
#define DDISPY_END    DDISPY_(DDISPY_IOCTL_END   )
#define DDISPY_READ   DDISPY_(DDISPY_IOCTL_READ  )
#define DDISPY_REPORT DDISPY_(DDISPY_IOCTL_REPORT)

#define BUFFER_SIZE  8192

extern unsigned char Buffer[];

void DDISpy_Start(unsigned fntable, int count);
void DDISpy_Stop(unsigned fntable, int count);