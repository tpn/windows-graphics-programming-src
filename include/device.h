#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : device.h				                                             //
//  Description: Service Control Manager wrapper                                     //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

// Service Control Manager
class KSCManager
{
public:

	SC_HANDLE m_schSCManager;

	KSCManager()
	{
		m_schSCManager = NULL;
	}

	bool OpenSCM(void)
	{
		m_schSCManager = OpenSCManager (NULL, NULL, 
			SC_MANAGER_ALL_ACCESS);

		return m_schSCManager != NULL; 
	}

	bool CloseSCM(void)
	{
		if ( m_schSCManager )
			return CloseServiceHandle(m_schSCManager) != 0;
		else
			return false;
	}
};


// Device
class KDevice : public KSCManager
{
	const TCHAR * m_DeviceName;
	SC_HANDLE     m_schService;
	HANDLE        m_hDevice;

	unsigned RemoveDriver(void);
	
	unsigned InstallDriver(const TCHAR * DriverExe);
	unsigned StartDriver(void);
	unsigned StopDriver(void);

	unsigned OpenDevice(void);
	unsigned CloseDevice(void);

public:
	
	KDevice(LPCTSTR DeviceName)
	{
		m_DeviceName = DeviceName;
		m_schService = NULL;
		m_hDevice    = NULL;

		OpenSCM();
	}

	~KDevice()
	{
		if ( m_schService )
			CloseServiceHandle( m_schService );

		CloseSCM();
	}

	unsigned Load(const TCHAR * DriverExe);
	unsigned Close(void);

	BOOL  IoControl(DWORD   dwIoControlCode, 
					LPVOID  lpInBuffer, 
					DWORD   nInBufferSize, 
					LPVOID  lpOutBuffer, 
					DWORD   nOutBufferSize, 
					LPDWORD lpBytesReturned) 
	{
		return DeviceIoControl(m_hDevice, dwIoControlCode,
			lpInBuffer, nInBufferSize, lpOutBuffer, 
			nOutBufferSize, lpBytesReturned, NULL);
	}
};

