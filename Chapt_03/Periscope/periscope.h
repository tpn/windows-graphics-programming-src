//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : periscope.h			                                             //
//  Description: Periscope interface                                                 //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

// instance information associated with device
class KDeviceExtension
{
    unsigned long  StateVariable;
};


// Define the various device type values.  Range 0-32767 reserved by
// MS, and range 32768-65535 are reserved for use by customers.
#define FILE_DEVICE_PERISCOPE  0x00008301

// Macro definition for defining IOCTL and FSCTL function control codes. 
// Rabge 0-2047  Note range0-2047 reserved by MS, and range 2048-4095 
// are reserved for customers.

#define PERISCOPE_IOCTL_INDEX  0x8301

#define IOCTL_PERISCOPE CTL_CODE(FILE_DEVICE_PERISCOPE, PERISCOPE_IOCTL_INDEX, METHOD_BUFFERED, FILE_ANY_ACCESS)

