//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : periscope.cpp			                                             //
//  Description: Periscope kernel mode driver implementation                         //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#include "kernelopt.h"
#include "periscope.h"

const WCHAR DeviceName[] = L"\\Device\\Periscope";
const WCHAR DeviceLink[] = L"\\DosDevices\\PERISCOPE";


// Process CreateFile, CloseHandle
NTSTATUS DrvCreateClose(IN PDEVICE_OBJECT DeviceObject,   
                        IN PIRP           Irp)
{
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status      = STATUS_SUCCESS;
    
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}


// Process DeviceIoControl
NTSTATUS DrvDeviceControl(IN PDEVICE_OBJECT DeviceObject,   
                          IN PIRP           Irp)
{
	NTSTATUS nStatus = STATUS_INVALID_PARAMETER;

	Irp->IoStatus.Information = 0;

	// Get a pointer to the current location in the Irp,
	// where the function codes and parameters are located.
	PIO_STACK_LOCATION irpStack = 
		IoGetCurrentIrpStackLocation (Irp);
   
	unsigned * ioBuffer = (unsigned *) Irp->AssociatedIrp.SystemBuffer;
            
	if ( (irpStack->Parameters.DeviceIoControl.IoControlCode
         == IOCTL_PERISCOPE) && (ioBuffer!=NULL) && 
        (irpStack->Parameters.DeviceIoControl.
          InputBufferLength >= 8) )
	{
		unsigned leng = ioBuffer[1];
 
		if ( irpStack->Parameters.DeviceIoControl.OutputBufferLength >= leng )
		{
			Irp->IoStatus.Information = leng;
			nStatus = STATUS_SUCCESS;

			__try
			{
				memcpy(ioBuffer, (void *) ioBuffer[0], leng);
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
				Irp->IoStatus.Information = 0;
				nStatus = STATUS_INVALID_PARAMETER;
			}
		}
	}
    
	Irp->IoStatus.Status = nStatus;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
   
	return nStatus;
}


// Process driver unloading
void DrvUnload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING deviceLinkUnicodeString;

	RtlInitUnicodeString(&deviceLinkUnicodeString, DeviceLink);

	IoDeleteSymbolicLink(&deviceLinkUnicodeString);
	IoDeleteDevice(DriverObject->DeviceObject);
}


// Installable driver initialization entry point.
NTSTATUS DriverEntry(IN PDRIVER_OBJECT  Driver, IN PUNICODE_STRING RegistryPath)
{
	UNICODE_STRING  deviceNameUnicodeString;
    
	RtlInitUnicodeString( &deviceNameUnicodeString, DeviceName );

	// Create a device
	PDEVICE_OBJECT  deviceObject = NULL;
    
	NTSTATUS ntStatus = IoCreateDevice (Driver,
		sizeof(KDeviceExtension), & deviceNameUnicodeString,
		FILE_DEVICE_PERISCOPE, 0, TRUE, & deviceObject);

	if ( NT_SUCCESS(ntStatus) )
	{
		// Create a symbolic link that Win32 apps can specify
		// to gain access to this driver/device
		UNICODE_STRING  deviceLinkUnicodeString;

		RtlInitUnicodeString (&deviceLinkUnicodeString, DeviceLink);

		ntStatus = IoCreateSymbolicLink(
			&deviceLinkUnicodeString,
			&deviceNameUnicodeString);

		// Create driver dispatch table
		if ( NT_SUCCESS(ntStatus) )
		{
			Driver->DriverUnload                 = DrvUnload;
			Driver->MajorFunction[IRP_MJ_CREATE] = DrvCreateClose;
			Driver->MajorFunction[IRP_MJ_CLOSE]  = DrvCreateClose;
			Driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DrvDeviceControl;
		}
	}

	if ( !NT_SUCCESS(ntStatus) && deviceObject!=NULL )
		IoDeleteDevice(deviceObject);

	return ntStatus;
}
