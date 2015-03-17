#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "resource.h"

HINSTANCE hModule;

int GetDIBColorCount(const BITMAPINFOHEADER & bmih)
{
    if ( bmih.biBitCount <= 8 )
        if ( bmih.biClrUsed )
            return bmih.biClrUsed;
        else
            return 1 << bmih.biBitCount;
    else if ( bmih.biCompression==BI_BITFIELDS )
        return 3 + bmih.biClrUsed;
    else
        return bmih.biClrUsed;
}

class KDIB
{
    const BITMAPINFO * m_pDIB;
    const void       * m_pBits;

public:
    KDIB()
    {
        m_pDIB = NULL;
    }

    void Load(int resid)
    {
        if ( m_pDIB )
            return;

        HRSRC hRsc = FindResource(hModule, MAKEINTRESOURCE(resid), RT_BITMAP);
        
        if ( hRsc )
        {
            m_pDIB  = (const BITMAPINFO *) LockResource(LoadResource(hModule, hRsc));
            m_pBits =  & m_pDIB->bmiColors[GetDIBColorCount(m_pDIB->bmiHeader)];
        }
    }

    int StretchDIBits(HDC hDC, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh, int coloruse, DWORD rop)
    {
        return ::StretchDIBits(hDC, dx, dy, dw, dh, sx, sy, sw, sh, 
                    m_pBits, m_pDIB, coloruse, rop);
    }
   
};


void OnDraw(HDC hDC)
{
    HRGN hRegion = NULL;
