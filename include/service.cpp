//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : service.cpp					                                         //
//  Description: Service control manager wrapper, Chapter 3                          //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winioctl.h>
#include <tchar.h>
#include <assert.h>

#include "device.h"


unsigned KDevice::InstallDriver(const TCHAR * DriverExe)
{
	m_schService = CreateService(m_schSCManager, 
		m_DeviceName, m_DeviceName, 
		SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, 
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
        DriverExe, NULL, NULL, NULL, NULL, NULL);

	if ( m_schService )
		return 0;
	else
		return GetLastError();
}


unsigned KDevice::RemoveDriver(void)
{
	if ( m_schService == NULL )
	{
		m_schService = OpenService(m_schSCManager, 
			m_DeviceName, SERVICE_ALL_ACCESS);

		if (m_schService == NULL)
			return GetLastError();
	}

	if ( DeleteService (m_schService) )
	{
		m_schService = NULL;
		return 0;
	}
	else
		return GetLastError();
}


unsigned KDevice::StartDriver(void)
{
	assert(m_schService!=NULL);
    
	if ( StartService (m_schService, 0, NULL) )
		return 0;
	else
	{
		DWORD ret = GetLastError();

		if ( ret == ERROR_SERVICE_ALREADY_RUNNING )
			ret = 0;

		return ret;
   }
}


unsigned KDevice::StopDriver(void)
{
   SERVICE_STATUS  serviceStatus;

	assert(m_schService!=NULL);
    
	if ( ControlService(m_schService, SERVICE_CONTROL_STOP,
		&serviceStatus) )
		return 0;
   else
		return GetLastError();
}


unsigned KDevice::OpenDevice(void)
{
    TCHAR completeDeviceName[MAX_PATH];

    // Create a \\.\XXX device name that CreateFile can use
    _tcscpy(completeDeviceName, _T("\\\\.\\"));
    _tcscat(completeDeviceName, m_DeviceName);

    m_hDevice = CreateFile(completeDeviceName,
		GENERIC_READ | GENERIC_WRITE, 0, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if ( m_hDevice == INVALID_HANDLE_VALUE )
		return GetLastError();
	else
		return 0;
}


unsigned KDevice::CloseDevice(void)
{
	assert( m_hDevice);

	if ( CloseHandle(m_hDevice) )
	{
		m_hDevice = NULL;
		return 0;
	}
	else
	{
		m_hDevice = NULL;
		return GetLastError();
	}
}


unsigned KDevice::Load(const TCHAR * DriverExe)
{
	RemoveDriver();
	
	InstallDriver(DriverExe);
	StartDriver();
	
	return OpenDevice();
}


unsigned KDevice::Close(void)
{
	CloseDevice();
	StopDriver();
	
	return RemoveDriver();
}
