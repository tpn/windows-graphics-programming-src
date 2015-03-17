//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : dcdtable.cpp					                                     //
//  Description: Decode GDI handle table, Chapter 3                                  //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

#define STRICT
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <tchar.h>
#include <winioctl.h>

#include "dialog.h"
#include "gditable.h"
#include "fxstring.h"

#include "resource.h"
#include "handles.h"
#include "creator.h"
#include "dcdtable.h"
#include "device.h"
#include "MemDump.h"
#include "..\\Periscope\Periscope.h"

#define _WIN32_WINNT 0x0500

#include "dcattr.h"

KDecodeTablePage::KDecodeTablePage()
{
	m_hInst = NULL;
}


class KTemp
{
public:
	KString<512> mess;

	HGDIOBJ Select(HDC hDC, HGDIOBJ hHandle);
};


HGDIOBJ KTemp::Select(HDC hDC, HGDIOBJ hHandle)
{
	HGDIOBJ hRslt;

	if ( GetObjectType(hHandle) == OBJ_PAL )
	{
		hRslt = SelectPalette(hDC, (HPALETTE) hHandle, FALSE);
		mess.Append("SelectPalette() returns 0x%x", (unsigned) hRslt);
	}
	else
	{
		hRslt = SelectObject(hDC, hHandle);
		mess.Append("SelectObject() returns 0x%x", (unsigned) hRslt);
	}
	
//	assert(hRslt);
	return hRslt;
}


void KDecodeTablePage::TestnCount(int obj, HDC hDC1, HDC hDC2, bool testdelete)
{
	KTemp temp;
	BOOL rslt;

	// create an object
	HGDIOBJ hHandle = CreateObject(obj, hDC1);

	assert(hHandle);

	temp.mess.Append("Create...() returns 0x%x", (unsigned) hHandle);
	temp.mess.Append(", nCount=%d\r\n", m_GDITable.GetCount(hHandle));

	// select into a DC1
	HGDIOBJ hOld1 = temp.Select(hDC1, hHandle); 
	temp.mess.Append(" nCount=%d\r\n", m_GDITable.GetCount(hHandle));

	// select into a DC2
	HGDIOBJ hOld2 = temp.Select(hDC2, hHandle); 
	temp.mess.Append(" nCount=%d\r\n", m_GDITable.GetCount(hHandle));

	if ( testdelete )
	{
		rslt = DeleteObject(hHandle);
		temp.mess.Append("DeleteObject() returns %d\n", rslt);
	}
	
	// deselect from a DC2
	temp.mess.Append("(De)");
	temp.Select(hDC2, hOld2);
	temp.mess.Append(", nCount=%d\r\n", m_GDITable.GetCount(hHandle));

	// deselect from a DC1
	temp.mess.Append("(De)");
	temp.Select(hDC1, hOld1);
	temp.mess.Append(", nCount=%d\r\n", m_GDITable.GetCount(hHandle));

	rslt = DeleteObject(hHandle);
	temp.mess.Append("DeleteObject() returns %d", rslt);
	
	MessageBox(NULL, temp.mess, Commands[obj], MB_OK);
}


void KDecodeTablePage::TestnCount(bool testdelete)
{
	HDC hDC = GetDC(m_hWnd);
	HDC hMemDC1 = CreateCompatibleDC(hDC);
	HDC hMemDC2 = CreateCompatibleDC(hDC);

	TestnCount(obj_CreateSolidBrush,   hMemDC1, hMemDC2, testdelete);
	TestnCount(obj_CreatePen,		   hMemDC1, hMemDC2, testdelete);
	TestnCount(obj_CreateBitmap,	   hMemDC1, hMemDC2, testdelete);
	TestnCount(obj_CreateFont,		   hMemDC1, hMemDC2, testdelete);
	TestnCount(obj_CreatePalette,      hMemDC1, hMemDC2, testdelete);

	DeleteDC(hMemDC1);
	DeleteDC(hMemDC2);

	ReleaseDC(m_hWnd, hDC);
}


// check reuse of rectangle region
void KDecodeTablePage::TestpUserRectRegion(void)
{
	KString<1024> mess;

	for (int i=0; i<8; i++)
	{
		HRGN hRgn = CreateRectRgn(i, i, i+10, i+10);

		mess.Append("%d  ", i+1);
		mess.Append("%08lx\t",     (unsigned) hRgn);
		mess.Append("pKernel=%x\t", (unsigned) m_GDITable[hRgn].pKernel);
		mess.Append("pUser=%x  ",    (unsigned) m_GDITable[hRgn].pUser);

		RECT * pRect = (RECT *) ((unsigned char *)m_GDITable[hRgn].pUser+8);
		mess.Append("[%d, ", pRect->left);
        mess.Append("%d, ", pRect->top);
        mess.Append("%d, ", pRect->right);
        mess.Append("%d]\r\n", pRect->bottom);

		KString<128> t;
		t.Append("%x, ", (unsigned) m_GDITable[hRgn].pKernel);
		t.Append("%x, ",            m_GDITable[hRgn].nUpper);
		t.Append("%x ",  (unsigned) m_GDITable[hRgn].pUser);

		DeleteObject(hRgn);

		t.Append(" -> delete -> ");
		t.Append("%x, ", (unsigned) m_GDITable[hRgn].pKernel);
		t.Append("%x, ",            m_GDITable[hRgn].nUpper);
		t.Append("%x ",  (unsigned) m_GDITable[hRgn].pUser);

		OutputDebugString(t);
		OutputDebugString("\n");
	}

	MessageBox(NULL, mess, "CreateRectRgn", MB_OK);
}


void KDecodeTablePage::TestpUserSolidBrush(void)
{
	KString<1024> mess;

	for (int i=0; i<8; i++)
	{
		HGDIOBJ hBrush = CreateSolidBrush(RGB(i*32, i*32, i*32));

		mess.Append("%d  ", i+1);
		mess.Append("%08lx\t",     (unsigned) hBrush);
		mess.Append("pKernel=%x\t", (unsigned) m_GDITable[hBrush].pKernel);
		mess.Append("pUser=%x  ",    (unsigned) m_GDITable[hBrush].pUser);

		LOGBRUSH * pLogBrush = (LOGBRUSH *) m_GDITable[hBrush].pUser;
		mess.Append("color=%x\r\n", pLogBrush->lbColor);

	    OutputDebugString(mess);

        KString<128> t;
		t.Append("%x, ", (unsigned) m_GDITable[hBrush].pKernel);
		t.Append("%x, ",            m_GDITable[hBrush].nUpper);
		t.Append("%x ",  (unsigned) m_GDITable[hBrush].pUser);

		DeleteObject(hBrush);

		t.Append(" -> delete -> ");
		t.Append("%x, ", (unsigned) m_GDITable[hBrush].pKernel);
		t.Append("%x, ",            m_GDITable[hBrush].nUpper);
		t.Append("%x ",  (unsigned) m_GDITable[hBrush].pUser);

	//	OutputDebugString(t);
	//	OutputDebugString("\n");
	}

	MessageBox(NULL, mess, "CreateSolidBrush", MB_OK);
}


BOOL KDecodeTablePage::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hExp = GetDlgItem(hWnd, IDC_EXPERIMENT);

    switch (uMsg)
    {
	    case WM_INITDIALOG:
			m_hWnd = hWnd;
            
			m_Objects.FromDlgItem(hWnd, IDC_GDITABLE);
			
			m_Objects.AddColumn(0, 50, "Index");
			m_Objects.AddColumn(1, 90, "pKernel");
		
			m_Objects.AddColumn(2, 60, "nCount");
        	
			m_Objects.AddColumn(3, 60, "nProc");
		    
			m_Objects.AddColumn(4, 60, "nUpper");
			m_Objects.AddColumn(5, 60, "nType");

            m_Objects.AddColumn(6, 90, "pUser");

			SendMessage(hExp, CB_ADDSTRING, 0, (LPARAM) _T("Query GDI Table"));
			SendMessage(hExp, CB_ADDSTRING, 0, (LPARAM) _T("nCount: SelectObject"));
			SendMessage(hExp, CB_ADDSTRING, 0, (LPARAM) _T("nCount: DeleteObject"));
			SendMessage(hExp, CB_ADDSTRING, 0, (LPARAM) _T("pUser: CreateSolidBrush"));
            SendMessage(hExp, CB_ADDSTRING, 0, (LPARAM) _T("pUser: CreateRectRgn"));
            SendMessage(hExp, CB_ADDSTRING, 0, (LPARAM) _T("Dump GDI Table"));
			SendMessage(hExp, CB_SETCURSEL, 0, 0);
			return TRUE;
		
		case WM_COMMAND:
			if ( LOWORD(wParam) == IDC_GO )
			switch ( SendMessage(hExp, CB_GETCURSEL, 0, 0) )
			{
				case 0: Query();		   return TRUE;
				case 1: TestnCount(false); return TRUE;
				case 2: TestnCount(true);  return TRUE;
				case 3: TestpUserSolidBrush(); return TRUE;
                case 4: TestpUserRectRegion(); return TRUE;
                case 5: DumpGDITable(); return TRUE;
			}
			break;

		case WM_NOTIFY:
			if (wParam == IDC_GDITABLE)
			{
				NM_LISTVIEW * pInfo = (NM_LISTVIEW *) lParam;
				
				if ( (pInfo->hdr.code == NM_CLICK) && (pInfo->iItem != -1) )  
				{
					TCHAR sIndex[16];
					unsigned nIndex;

					m_Objects.GetItemText(pInfo->iItem, 0, sIndex, sizeof(sIndex));
					sscanf(sIndex, "%x", & nIndex);

					ShowDetails(nIndex);
					return TRUE;
				}
				else if ( (pInfo->hdr.code == NM_DBLCLK) && (pInfo->iItem != -1) )  
				{
					TCHAR sIndex[16];
					unsigned nIndex;

					m_Objects.GetItemText(pInfo->iItem, 0, sIndex, sizeof(sIndex));
					sscanf(sIndex, "%x", & nIndex);

				//	HDC hDC = GetDC(NULL);
				//	Modify((unsigned) hDC & 0x3FFF);
				//	ReleaseDC(NULL, hDC);

					Modify(nIndex);
					return TRUE;
				}
	
			}

			break;

		case WM_DESTROY:
			m_Objects.DeleteAll();
			break;
    }
    
    return FALSE;
}


void KDecodeTablePage::Query(void)
{
	m_Objects.DeleteAll();

	DWORD nProcId;

	// determine if user is interested in single process or all processes
	if ( SendDlgItemMessage(m_hWnd, IDC_PROCESSONLY, BM_GETCHECK, 0, 0) == BST_CHECKED ) 
		nProcId = GetCurrentProcessId();
	else
		nProcId = 0;

	for (int i=0; i<=0x3FFF; i++)
	{
		GDITableCell cell = m_GDITable[i];

		if ( (nProcId == 0) || (nProcId==m_GDITable.GetProcess(i)) ) // process matches
		if ( (unsigned) cell.pKernel >= 0x80000000 )	  // valid pointer to kernel address space
		{
			KString<64> s;

			m_Objects.AddItem(0, s.Format("%4x",  i));
			m_Objects.AddItem(1, s.Format("%8x",  (unsigned) cell.pKernel));
			m_Objects.AddItem(2, s.Format("%x",   m_GDITable.GetCount(i)));
			m_Objects.AddItem(3, s.Format("%x",   m_GDITable.GetProcess(i)));
			m_Objects.AddItem(4, s.Format("%04x", cell.nUpper));
			m_Objects.AddItem(5, s.Format("%04x", cell.nType));
			m_Objects.AddItem(6, s.Format("%x",   (unsigned) cell.pUser));
		}
	}
}


const char * GDIObjectTypeName(HGDIOBJ obj)
{
    static char rslt[32];

    switch ( HIWORD(obj) & 0x7F )
    {
        case 0x01: return "dc";
        case 0x04: return "region";
        case 0x05: return "bitmap";
        case 0x08: return "palette";
        case 0x0a: return "font";
        case 0x10: return "brush";
        case 0x21: return "enhmetafile";
        case 0x30: return "pen";
        case 0x50: return "extpen";
    
        default:   
            wsprintfA(rslt, "unknown(%xh)", HIWORD(obj) & 0x7F);
            return rslt;
    }
}


class KPeriscopeClient : public KDevice
{
public:
    KPeriscopeClient(const TCHAR * DeviceName) : KDevice(DeviceName)
    {
    }

    bool Read(void * dst, const void * src, unsigned len);
};


bool KPeriscopeClient::Read(void * dst, const void * src, unsigned len)
{
    unsigned      cmd[2] = { (unsigned) src, len };
    unsigned long dwRead;
   
    return IoControl(IOCTL_PERISCOPE, cmd, sizeof(cmd), dst, len, &dwRead) && (dwRead==len);
}


void KDecodeTablePage::DumpGDITable(void)
{
    bool pagetouched[16*256]; // 16 Mb in pages
    char temp[MAX_PATH];
    KMemDump  memdump;
        
    memset(pagetouched, 0, sizeof(pagetouched));

    memdump.OpenDump();
        
	for (int i=0; i<=0x3FFF; i++)
	{
		GDITableCell cell = m_GDITable[i];

		if ( (unsigned) cell.pKernel >= 0x80000000 )
		{
            wsprintf(temp, "%4x %8x %8x %s", i, cell.pKernel, cell.pUser, 
                GDIObjectTypeName((HGDIOBJ) MAKELONG(i, cell.nUpper)));
            memdump.Writeln(temp);

            unsigned addr = (unsigned) cell.pKernel;

            if ( (addr>>24)==0xE1 )
                pagetouched[(addr & 0xFFFFFF)>>12] = true;
		}
	}
    memdump.Newline();

    KPeriscopeClient scope("PeriScope");
    
    if ( scope.Load("c:\\periscope.sys")==ERROR_SUCCESS )
    {
        unsigned char buf[4096];

        for (i=0; i<4096; i++)
            if ( pagetouched[i] )
            {
                unsigned addr = 0xE1000000 + i * 4096;
                scope.Read(buf, (void *) addr, sizeof(buf));

                memdump.Dump(buf, addr - (unsigned)buf, 4096, sizeof(unsigned long));
                memdump.Newline();
            }
        
        memdump.CloseDump();
        
        scope.Close();
    }
    else
        MessageBox(NULL, "Unable to load periscope", NULL, MB_OK);
}


const LPCTSTR DCProperty[] =
{
	TEXT("GetBkColor/SetBkColor"),
	TEXT("GetBkMode/SetBkMode"),
	TEXT("GetPolyFillMode/SetPolyFillMode"),
	TEXT("GetRop2/SetRop2"),
	TEXT("GetStretchBltMode/SetStretchBltMode"),
	TEXT("GetTextColor/SetTextColor"),
	TEXT("GetMapMode/SetMapMode"),
	TEXT("GetViewportOrg/SetViewportOrg"),
	TEXT("GetViewportExt/SetViewportExt"),
	TEXT("GetWindowOrg/SetWindowOrg"),
	TEXT("GetWindowExt/SetWindowExt"),
	TEXT("GetClipBox/SetClipBox"),
	TEXT("GetTextAlign/SetTxtAlign"),
	TEXT("GetTextJustification/SetTextJustification"),
	TEXT("GetTextCharacterExtra/SetTextCharacterExtra"),
	TEXT("GetAspectRatioFilterEx/SetMapperFlags")
};
	

#define Cap(name) DeviceCap(#name, name)

class KDecodeDC
{
	unsigned * m_addr;
	unsigned buffer[sizeof(DCAttr)/sizeof(unsigned)];
	HDC		   m_hDC;
public:

	KDecodeDC(HDC hDC, unsigned * addr)
	{
		m_hDC = hDC;
		m_addr = addr;
	}

	void Save(void)
	{
		memcpy(buffer, m_addr, sizeof(buffer));
	}

	void Comp(TCHAR * mess);
	void DeviceCap(const TCHAR * name, int id);
};


void KDecodeDC::DeviceCap(const TCHAR *name, int id)
{
	unsigned rslt = GetDeviceCaps(m_hDC, id);

	OutputDebugString("GetDeviceCaps(");
	OutputDebugString(name);

	TCHAR temp[64];
	wsprintf(temp, "%x", rslt);

	OutputDebugString(" returns ");
	OutputDebugString(temp);
	
	for (int i=0; i<sizeof(DCAttr)/sizeof(unsigned); i++)
		if (m_addr[i] == rslt )
		{
			wsprintf(temp, "offset %d", i);
			OutputDebugString(temp);
			break;
		}

	OutputDebugString("\n");
}


void KDecodeDC::Comp(TCHAR *mess)
{
	int changes = 0;
	TCHAR temp[128];

	for (int i=0; i<sizeof(DCAttr)/sizeof(unsigned); i++)
		if (buffer[i] != m_addr[i])
		{
			wsprintf(temp, "%s changes offse 0x%x (%08x -> %08x) \n", mess, i*4,
				buffer[i], m_addr[i]);
			OutputDebugString(temp);
			changes ++;
		}

	if (changes == 0)
	{
		wsprintf(temp, "%s changes nothing\n", mess);
		OutputDebugString(temp);
	}
}



#include <math.h>

// convert FLOATOBJ back to double
double FLOATOBJ2Double(const FLOATOBJ & f)
{
	double rslt = (double) f.ul1 * pow(2, (double)f.ul2-32);

	char temp[48];
	sprintf(temp, "%08lx %08lx %10.8f", f.ul2, f.ul1, rslt);
	OutputDebugString(temp);

	return rslt;
}


void DecodeDC(HDC hDC, unsigned * addr)
{
	KDecodeDC dc(hDC, addr);
	DCAttr * p = (DCAttr *) addr;

	int j = sizeof(DCAttr);

#if (_WIN32_WINNT >= 0x0500)
	assert (j==0x1c8);
#else
	assert (j==0x190);
#endif

	POINT Org; GetDCOrgEx(hDC, & Org);

	dc.Save();
	HBITMAP hDDB = CreateBitmap(15, 15, 1, 1, NULL);
	SelectObject(hDC, hDDB);
	dc.Comp("SelectBitmap");

	HRGN hRgn = CreateRectRgn(1, 2, 3, 4);
	SelectObject(hDC, hRgn);
	for (int i=0; i<sizeof(DCAttr)/sizeof(unsigned); i++)
		if ( GetObjectType((HGDIOBJ) addr[i]) )
		{
			TCHAR Temp[48];
			wsprintf(Temp, "offset %x objtype %d\n", i*4, GetObjectType((HGDIOBJ) addr[i]));
			OutputDebugString(Temp);
		}

	dc.Save();
	BeginPath(hDC); dc.Comp("BeginPath");
	Rectangle(hDC, 1, 2, 3, 4);
	dc.Save();
	EndPath(hDC);
	dc.Comp("EndPath");

	int s = SaveDC(hDC); dc.Save();
	RestoreDC(hDC, s);   dc.Comp("SaveDC");

	SetBrushOrgEx(hDC, 1, 2, NULL); dc.Save();
	SetBrushOrgEx(hDC, 3, 4, NULL); dc.Comp("SetBrushOrgEx");

	SetGraphicsMode(hDC, GM_COMPATIBLE); dc.Save();
	SetGraphicsMode(hDC, GM_ADVANCED);   dc.Comp("SetGraphicsMode");

	XFORM xform = { 1, 0, 0, 1, 5, 5.5 };
	SetWorldTransform(hDC, & xform);	 dc.Save();

	XFORM xform1= { 1, 0, 0, 2, 5, (float)5.49 };  
	SetWorldTransform(hDC, & xform1);	 dc.Comp("SetWorldTransform");
			
//	FLOATOBJ2Double(p->XFORM_eDx);
	
										 dc.Save();
	XFORM xform2= { 1.1111f, 0xFF02, 0xFF03, 0xFF04, 0xFF05, 0xFF06 };  
	
	SetWorldTransform(hDC, & xform2);	 dc.Comp("SetWorldTransform 1");
	
//	FLOATOBJ2Double(p->XFORM_eM11);
	
										dc.Save();
	XFORM xform3= { 1.1111f, 2.2222f, 0xFF03, 0xFF04, 0xFF05, 0xFF06 };  
	SetWorldTransform(hDC, & xform3);	 dc.Comp("SetWorldTransform 2");

	dc.Save();
	XFORM xform4= { 1.1111f, 2.2222f, 3.333f, 0xFF04, 0xFF05, 0xFF06 };  
	SetWorldTransform(hDC, & xform4);	 dc.Comp("SetWorldTransform 3");

	dc.Save();
	XFORM xform5= { 1.1111f, -2.2222f, 3.333f, 4.4444f, 0xFF05, 0xFF06 };  
	SetWorldTransform(hDC, & xform5);	 dc.Comp("SetWorldTransform 4");

	dc.Save();
	XFORM xform6= { 1.1111f, 2.2222f, 3.333f, 4.4444f, 5.5555f, 0xFF06 };  
	SetWorldTransform(hDC, & xform6);	 dc.Comp("SetWorldTransform 5");

	dc.Save();
	XFORM xform7= { 1.1111f, 2.2222f, 3.333f, 4.4444f, 5.5555f, 6.6666f };  
	SetWorldTransform(hDC, & xform7);	 dc.Comp("SetWorldTransform 6");


	SetMiterLimit(hDC, 0.5, NULL); dc.Save();
	SetMiterLimit(hDC, 1.1f, NULL); dc.Comp("SetMiterLimit");

	SetArcDirection(hDC, AD_COUNTERCLOCKWISE ); dc.Save();
	SetArcDirection(hDC, AD_CLOCKWISE);         dc.Comp("SetArcDirection");

	RECT rect = { 1961, 1962, 1963, 1964 }; dc.Save();
	SetBoundsRect(hDC, & rect, DCB_ENABLE); dc.Comp("SetBoundsRect");

	SelectObject(hDC, GetStockObject(BLACK_BRUSH)); dc.Save();
	SelectObject(hDC, GetStockObject(WHITE_BRUSH)); dc.Comp("SelectBrush");

	SelectObject(hDC, GetStockObject(BLACK_PEN));   dc.Save();
	SelectObject(hDC, GetStockObject(WHITE_PEN));   dc.Comp("SelectPen");

	SelectObject(hDC, GetStockObject(ANSI_FIXED_FONT)); dc.Save();
	SelectObject(hDC, GetStockObject(ANSI_VAR_FONT));   dc.Comp("SelectFont");

	SelectObject(hDC, GetStockObject(DEFAULT_PALETTE)); dc.Save();
	SelectObject(hDC, CreateHalftonePalette(hDC)); dc.Comp("SelectPalette");

	MoveToEx(hDC, 1, 2, NULL);	   dc.Save();
	MoveToEx(hDC, 100, 200, NULL); dc.Comp("MoveToEx");

	SetBkColor(hDC, RGB(1, 0, 0)); dc.Save(); 
	SetBkColor(hDC, RGB(2, 0, 0)); dc.Comp("SetBkColor");

	SetBkMode(hDC, OPAQUE);		 dc.Save();
	SetBkMode(hDC, TRANSPARENT); dc.Comp("SetBkMode");

	SetPolyFillMode(hDC, ALTERNATE); dc.Save();
	SetPolyFillMode(hDC, WINDING);   dc.Comp("SetPolyFillMode");

	SetROP2(hDC, R2_BLACK); dc.Save();
	SetROP2(hDC, R2_WHITE); dc.Comp("SetRop2");

	SetStretchBltMode(hDC, BLACKONWHITE); dc.Save();
	SetStretchBltMode(hDC, COLORONCOLOR); dc.Comp("SetStretchBltMode");

	SetTextColor(hDC, RGB(3, 0, 0)); dc.Save();
	SetTextColor(hDC, RGB(4, 0, 0)); dc.Comp("SetTextColor");

	SetMapMode(hDC, MM_TEXT); dc.Save();
	SetMapMode(hDC, MM_ANISOTROPIC); dc.Comp("SetMapMode");

	SetViewportOrgEx(hDC, 1, 2, NULL); dc.Save();
	SetViewportOrgEx(hDC, 3, 4, NULL); dc.Comp("SetViewportOrgEx");

	SetViewportExtEx(hDC, 100, 200, NULL); dc.Save();
	SetViewportExtEx(hDC, 300, 400, NULL); dc.Comp("SetViewportExtEx");

	SetWindowOrgEx(hDC, 1, 3, NULL); dc.Save();
	SetWindowOrgEx(hDC, 2, 4, NULL); dc.Comp("SetWindowOrgEx");

	SetWindowExtEx(hDC, 100, 50, NULL); dc.Save();
	SetWindowExtEx(hDC, 50, 100, NULL); dc.Comp("SetWindowExtEx");

	SetTextAlign(hDC, TA_BASELINE); dc.Save();
	SetTextAlign(hDC, TA_BOTTOM);   dc.Comp("SetTextAlign");

	SetTextJustification(hDC, 1, 2); dc.Save();
	SetTextJustification(hDC, 3, 4); dc.Comp("SetTextJustification");

	SetTextCharacterExtra(hDC, 5); dc.Save();
	SetTextCharacterExtra(hDC, 0); dc.Comp("SetTextCharacterExtra");

	SetMapperFlags(hDC, 1); dc.Save();
	SetMapperFlags(hDC, 0); dc.Comp("SetMapperFlags");

    TEXTMETRIC tm;
    GetTextMetrics(hDC, &tm);

    GetClipBox(hDC, & rect);

    i=1;
	dc.Cap(DRIVERVERSION);
	dc.Cap(TECHNOLOGY);
	dc.Cap(HORZSIZE);
	dc.Cap(VERTSIZE);
	dc.Cap(HORZRES);
	dc.Cap(VERTRES);
	dc.Cap(BITSPIXEL);
	dc.Cap(PLANES);
	dc.Cap(NUMBRUSHES);
	dc.Cap(NUMPENS);
	dc.Cap(NUMMARKERS);
	dc.Cap(NUMFONTS);
	dc.Cap(NUMCOLORS);
	dc.Cap(PDEVICESIZE);
	dc.Cap(CURVECAPS);
	dc.Cap(LINECAPS);
	dc.Cap(POLYGONALCAPS);
	dc.Cap(TEXTCAPS);
	dc.Cap(CLIPCAPS);
	dc.Cap(RASTERCAPS);
	dc.Cap(ASPECTX);
	dc.Cap(ASPECTY);
	dc.Cap(ASPECTXY);
//	dc.Cap(SHADEBLENDCAPS);
	dc.Cap(LOGPIXELSX);
	dc.Cap(LOGPIXELSY);
	dc.Cap(SIZEPALETTE);
	dc.Cap(NUMRESERVED);
	dc.Cap(COLORRES);
	dc.Cap(PHYSICALWIDTH);
	dc.Cap(PHYSICALHEIGHT);
	dc.Cap(PHYSICALOFFSETX);
	dc.Cap(PHYSICALOFFSETY);
	dc.Cap(SCALINGFACTORX);
	dc.Cap(SCALINGFACTORY);

	dc.Cap(VREFRESH);
    dc.Cap(DESKTOPVERTRES);
    dc.Cap(DESKTOPHORZRES);
	dc.Cap(BLTALIGNMENT);
}


class KEditDC : public KDialog
{
public:
	unsigned * addr;

	BOOL DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		int i;

		switch (uMsg)
		{
			case WM_INITDIALOG:
				m_hWnd = hWnd;
				
				for (i=0; i<sizeof(DCProperty)/sizeof(DCProperty[0]); i++)
				{
					SendDlgItemMessage(hWnd, IDC_OPERATIONS, LB_ADDSTRING, 0,
						(LPARAM) DCProperty[i]);
				}

				for (i=0; i<(sizeof(DCAttr)/4); i++)
				{
					TCHAR temp[64];
					wsprintf(temp, "%03x = %08x", i*4, addr[i]);
					SendDlgItemMessage(hWnd, IDC_CONTEXT, LB_ADDSTRING, 0,
						(LPARAM) temp);
				}
				return TRUE;

			case WM_COMMAND:
				switch ( LOWORD(wParam) )
				{
					case IDOK:
						EndDialog(hWnd, TRUE);
						return TRUE;
				}
		}

		return FALSE;
	}

};


void KDecodeTablePage::Modify(unsigned nIndex)
{
	GDITableCell Obj  = m_GDITable[nIndex];
	HGDIOBJ      hObj = (HGDIOBJ) (Obj.nUpper << 16 | nIndex);

	switch ( GetObjectType(hObj) )
	{
		case OBJ_MEMDC:
		case OBJ_DC:
			{
				KEditDC editdc;
				
				DecodeDC((HDC) hObj, (unsigned *) m_GDITable[nIndex].pUser);

				editdc.addr = (unsigned *) m_GDITable[nIndex].pUser;
				editdc.Dialogbox(m_hInst, MAKEINTRESOURCE(IDD_EDITDC));
			}
			break;
	}
}


void KDecodeTablePage::ShowDetails(unsigned nIndex)
{ 
	KString<1024> message;
	GDITableCell Obj = m_GDITable[nIndex];
	int * pInt;

	HGDIOBJ hObj = (HGDIOBJ) (Obj.nUpper << 16 | nIndex);
	
	message.Append("Index: %x, ", nIndex);
	message.Append("Handle: %x, ", (unsigned) hObj);
	message.Append("Type: "); message.Append(GetObjectTypeName(hObj));
	message.Append("\r\n");

	for (int o = obj_BLACK_BRUSH; o<=obj_DEFAULT_PALETTE; o++)
		if ( hObj == CreateObject(o) )
		{
			message.Append(Commands[o]);
		    message.Append("\r\n");
			break;
		}

	switch ( GetObjectType(hObj) )
	{
		case OBJ_BRUSH:
			if ( Obj.pUser )
			{
				assert (! IsBadReadPtr( Obj.pUser, 0x18 ) );
				pInt = (int *) Obj.pUser;

				LOGBRUSH * pBrush = (LOGBRUSH *) Obj.pUser;

				message.Append("LOGBRUSH { %d, ", pBrush->lbStyle);
				message.Append("0x%06x, ",        pBrush->lbColor);
				message.Append("%d }",            pBrush->lbHatch);
                message.Append("%d, ",            pInt[3]);
                message.Append("%d, ",            pInt[4]);
                message.Append("%d ",             pInt[5]);
			}
			break;

		case OBJ_REGION:
			if ( Obj.pUser )
			{
				assert (! IsBadReadPtr( Obj.pUser, 0x18 ) );
				pInt = (int *) Obj.pUser;

				message.Append("{ %d, ", pInt[0]);
				message.Append("%d, ", pInt[1]);
				message.Append("%d, ", pInt[2]);
				message.Append("%d, ", pInt[3]);
				message.Append("%d, ", pInt[4]);
				message.Append("%d }", pInt[5]);
			}
			break;

		case OBJ_FONT:
			if ( Obj.pUser )
			{
				assert (Obj.pUser );
				assert (! IsBadReadPtr( & Obj, 8 ) );
				pInt = (int *) Obj.pUser;

				message.Append("{ 0x%x, ", pInt[0]);
				message.Append("0x%x }", pInt[1]);

				LOGFONT logfont; 
				GetObject(hObj, sizeof(LOGFONT), &logfont);
				message.Append(logfont.lfFaceName);

				// Figuring out pInt[1]
				/*
				if ( pInt[1] )
				{
					HDC hDC = GetDC(NULL);

					HGDIOBJ hOld = SelectObject(hDC, hObj);
					
					TEXTMETRIC tm;
					GetTextMetrics(hDC, & tm);

					int width[256];
					GetCharWidth(hDC, tm.tmFirstChar, tm.tmLastChar, width);
					SelectObject(hDC, hOld);
					ReleaseDC(NULL, hDC);
				}
				*/
			}
			break;

		case OBJ_DC:
			assert (Obj.pUser);
			assert (! IsBadReadPtr(Obj.pUser, 0x190) );
			break;
	}

	// big enough to get the whole object
	int buffer[200];

	int size = GetObjectW(hObj, sizeof(buffer), buffer);

	if ( size )
	{
		message.Append("\r\n%d bytes: ", size);

		size = (size+3)/4;
		for (int i=0; i<size; i++)
			message.Append("%x ", buffer[i]);
	}
	SetDlgItemText(m_hWnd, IDC_DETAIL, message);
}

