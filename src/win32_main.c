#include <windows.h>
#include <winuser.h>

LRESULT CALLBACK MainWindowCallback(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    HDC hdc;
    PAINTSTRUCT ps;

    switch (message) {
        case WM_SIZE:
            OutputDebugStringA("WM_SIZE\n");
            return 0;

        case WM_DESTROY:
            OutputDebugStringA("WM_DESTROY\n");
            PostQuitMessage(0);
            return 0;

        case WM_CLOSE:
            OutputDebugStringA("WM_CLOSE\n");
            DestroyWindow(window);
            return 0;

        case WM_PAINT: {
            // Begin painting
            hdc = BeginPaint(window, &ps);

            // Set the background color (e.g., light blue)
            HBRUSH hBrush = CreateSolidBrush(RGB(173, 216, 230)); // Light Blue
            FillRect(hdc, &ps.rcPaint, hBrush);
            DeleteObject(hBrush);

            // Set text color (e.g., dark blue)
            SetTextColor(hdc, RGB(0, 0, 139)); // Dark Blue
            // Draw some text
            TEXTMETRIC tm;
            GetTextMetrics(hdc, &tm);
            int x = (ps.rcPaint.right - ps.rcPaint.left) / 2;
            int y = (ps.rcPaint.bottom - ps.rcPaint.top) / 2 - tm.tmHeight / 2;
            TextOut(hdc, x, y, "Hello, Windows!", 15);

            // End painting
            EndPaint(window, &ps);
            return 0;
        }

        default:
            return DefWindowProc(window, message, wparam, lparam);
    }
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previnstance, LPSTR cmdline, int cmdshow) {
    WNDCLASSA WindowClass = {};
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = instance;
    WindowClass.lpszClassName = "ExampleWindowClass";

    RegisterClassA(&WindowClass);

    HWND WindowHandle = CreateWindowExA(
        0,
        WindowClass.lpszClassName,
        "Windows API Example",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        0, 0, instance, 0
    );

    if (WindowHandle) {
        MSG message;
        while (GetMessage(&message, NULL, 0, 0)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
    return 0;
}
