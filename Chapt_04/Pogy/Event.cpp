//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : event.cpp				                                             //
//  Description: KEventPage class                                                    //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define NOCRYPT

#include <windows.h>
#include <commctrl.h>
#include <assert.h>
#include <stdio.h>

#include "..\..\include\win.h"
#include "..\..\include\listview.h"
#include "..\..\include\property.h"

#include "..\Diver\Diver.h"
#include "..\Diver\Report.h"

#include "Resource.h"

#include "ApiTable.h"
#include "ApiPage.h"
#include "Setup.h"
#include "Event.h"

LRESULT __declspec(dllimport) h_Reply;


LRESULT KEventPage::MonitorWindow(HWND hWnd, int oper, const char *owner)
{
    if ( strchr(owner, ':') )
        owner = strchr(owner, ':') + 1;

    while ( strchr(owner, '\\') )
        owner = strchr(owner, '\\') + 1;

    if ( ApiTable.MainTarget() )
    if ( stricmp(owner, ApiTable.MainTarget())==0 )
    {
         
        if (StartTime==0)
            StartTime = GetTickCount();

        if ( oper==M_CREATE )
        {
            h_Reply = REPLY_DIVERSTART;
            return REPLY_DIVERSTART;
        }
        else
        {
            h_Reply = REPLY_DIVERSTOP;
            return REPLY_DIVERSTOP;
        }
    }

    return 0;
}


BOOL KEventPage::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char temp[256];

	switch (uMsg)
    {
		case WM_INITDIALOG:
            hMailBox  = NULL;

            StartTime = 0;
			m_hWnd    = hWnd;
			SetupDiver(Diver_SetController, m_hWnd);

            // the propertysheet have two buttons OK and Cancel, (Apply removed )
            // hide the first OK button, change Cancel to Exit
            {
                // propertysheet is the parent window
                HWND h = GetDlgItem(GetParent(hWnd), IDOK);

                if (h)
                    SetWindowPos(h, HWND_BOTTOM, 0, 0, 0, 0,
                        SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);

                SetDlgItemText(GetParent(hWnd), IDCANCEL, "&Exit");
            }

			return TRUE;

		
        case WM_COPYDATA:
            {
                char t[256];

                h_Reply = 0;
                LRESULT lr = 0;

                COPYDATASTRUCT *pcds = (COPYDATASTRUCT *) lParam;
                assert(pcds);

                ReportMessage  *msg  = (ReportMessage *)(pcds->lpData);
                assert(msg);

                switch (pcds->dwData)
                {
                    case M_CREATE:
                    case M_DESTROY:
                        t[0] = '\'';
		                GetWindowText((HWND) msg->m_para, t+1, sizeof(t)-2);
				            
                        if ( pcds->dwData==M_CREATE )
                            strcat(t, "\' created");
                        else
                            strcat(t, "\' destroyed");

                        if ( strlen(msg->m_text) )
                            strcat(t, " by ");

                        lr = MonitorWindow((HWND) msg->m_para, pcds->dwData, msg->m_text);
                        break;

                   case M_ENTER: 
                        strcpy(t, "=> "); 

                        if (hMailBox)
                            SendMessage(hMailBox, WM_USER, 0x12345678, msg->m_para);
                        break;

                   case M_LEAVE: 
                        wsprintf(t, "<= %d us ", msg->m_time); 
                        break;

                   case M_ENTERSUMMARY:
                        wsprintf(t, "=> %d th ", msg->m_para);
                        break;

                   case M_LEAVESUMMARY:
                        wsprintf(t, "<= %d us %d th ", msg->m_time, msg->m_para);
                        break;

                   case M_MAILBOX:
                        hMailBox = (HWND) msg->m_para;
                            
                        assert(IsWindow(hMailBox));
                        wsprintf(t, "Mailbox %d", hMailBox);

                        pSetupPage->ReadButtons();
                        ApiTable.InterceptApi(m_hWnd, hMailBox, pSetupPage->bLogCall, pSetupPage->bDispCall);
                        break;

				   case M_TEXT:
						strcpy(t, "Diver: ");
						break;

                   default: 
                        strcpy(t, "Unknown "); break;
                }

                unsigned time = msg->m_tick - StartTime;

                wsprintf(temp, "(%d)  %d:%02d.%03d ", lr, time / 60000, 
                                                time / 1000 % 60, 
                                                time % 1000);
                strcat(temp, t);
                strcat(temp, msg->m_text);
                SendDlgItemMessage(hWnd, IDC_EVENT, LB_ADDSTRING, 0, (LPARAM) temp);

                return lr;
            }

    }
        
	return FALSE;
}
