
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch ( uMsg )
    {
        case WM_PAINT:
            {
                PAINTSTRUCT ps;

                HDC hDC = BeginPaint(hWnd, & ps);
                OnDraw(hDC);
                EndPaint(hWnd, & ps);

                return 0;
            }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, int nCmdShow)
{
    WNDCLASSEX wc;

    memset(& wc, 0, sizeof(wc));
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
    wc.lpszClassName = "EMFDC";

    if ( ! RegisterClassEx(& wc) )
        return -1;

    hModule = hInst;

    HWND hWnd = CreateWindowEx(0, "EMFDC", "Decompiled EMF", WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                    NULL, NULL, hInst, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;

    while ( GetMessage(&msg, NULL, 0, 0) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
