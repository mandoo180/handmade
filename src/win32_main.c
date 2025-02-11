#include <windows.h>
#include <winuser.h>

#define IDT_TIMER 1 // Define a timer ID
int animatedX = 50; // Initial x position of the animated element
int velocity = 5; // Speed at which the element moves

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
            // Set background color to light blue
            HBRUSH hBrush = CreateSolidBrush(RGB(173, 216, 230));
            FillRect(hdc, &ps.rcPaint, hBrush);
            DeleteObject(hBrush);

            // Set fill color for the animated element (red)
            HBRUSH hAnimBrush = CreateSolidBrush(RGB(255, 0, 0));
            SelectObject(hdc, hAnimBrush);
            // Draw the animated rectangle at position (animatedX, 100)
            Rectangle(hdc, animatedX, 100, animatedX + 50, 150);
            DeleteObject(hAnimBrush);

            // End painting
            EndPaint(window, &ps);
            return 0;
        }

        case WM_TIMER: {
            // Update the position of the animated element
            animatedX += velocity;

            // Reverse direction if hitting window bounds
            if (animatedX > 800 || animatedX < 0) { // 800 is the width of the window
                velocity = -velocity; // Reverse direction
            }

            // Invalidate the client area to trigger a repaint
            InvalidateRect(window, NULL, TRUE);
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
        "Windows API Animation Example",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        0, 0, instance, 0
    );

    if (WindowHandle) {
        // Set a timer to trigger the WM_TIMER message every 30 milliseconds
        SetTimer(WindowHandle, IDT_TIMER, 30, NULL);

        MSG message;
        while (GetMessage(&message, NULL, 0, 0)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
    return 0;
}
