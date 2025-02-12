#include <windows.h>
#include <winuser.h>
#include <stdint.h>
#include <xinput.h>

#define internal static
#define local_persist static
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

struct win32_offscreen_buffer {
  BITMAPINFO info;
  void *memory;
  int width;
  int height;
  int pitch;
  int bpp;                      /* bytes per pixel */
};

global_variable BOOL running;
global_variable struct win32_offscreen_buffer GlobalBackBuffer;

struct win32_window_dimension {
  int width;
  int height;
};

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwuserindex, XINPUT_STATE *pstate)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub) {
  return 0;
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwuserindex, XINPUT_VIBRATION *pvibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub) {
  return 0;
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void Win32LoadXInput() {
  // TODO: implement
  HMODULE xinputlib = LoadLibrary("xinput1_3.dll");
  if (xinputlib) {
    XInputGetState = (x_input_get_state *) GetProcAddress(xinputlib, "XInputGetState");
    XInputSetState = (x_input_set_state *) GetProcAddress(xinputlib, "XInputSetState");
  }
}

struct win32_window_dimension Win32GetWindowDimension(HWND window) {
  struct win32_window_dimension result;
  RECT clientrect;
  GetClientRect(window, &clientrect);
  result.width = clientrect.right - clientrect.left;
  result.height = clientrect.bottom - clientrect.top;
  return result;
}

void RenderWeirdGradient(struct win32_offscreen_buffer buffer, int boffset, int goffset) {
  uint8 *row = (uint8 *)buffer.memory;
  for (int y = 0; y < buffer.height; ++y) {
    uint32 *pixel = (uint32 *)row;
    for (int x = 0; x < buffer.width; ++x) {
      uint8 blue = (x + boffset);
      uint8 green = (y + goffset);
      /* TODO: find out what it does */
      *pixel++ = ((green << 8) | blue);
    }
    row += buffer.pitch;
  }
}

void Win32ResizeDIBSection(struct win32_offscreen_buffer *buffer, int width, int height) {
  if (buffer->memory) {
    VirtualFree(buffer->memory, 0, MEM_RELEASE);
  }
  buffer->width = width;
  buffer->height = height;
  buffer->bpp = 4;

  buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
  buffer->info.bmiHeader.biWidth = buffer->width;
  buffer->info.bmiHeader.biHeight = -buffer->height;
  buffer->info.bmiHeader.biPlanes = 1;
  buffer->info.bmiHeader.biBitCount = 32;
  buffer->info.bmiHeader.biCompression = BI_RGB;

  int BitmapMemorySize = (buffer->width * buffer->height) * buffer->bpp;
  buffer->memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
  buffer->pitch = width * buffer->bpp;
}

void Win32DisplayBufferInWindow(HDC devicectx,
                                int winwidth, int winheight,
                                struct win32_offscreen_buffer buffer,
                                int x, int y, int width, int height)
{
  // TODO: aspect ratio conrrection
  StretchDIBits(devicectx,
                0, 0, winwidth, winheight,
                0, 0, buffer.width, buffer.height,
                buffer.memory,
                &buffer.info,
                DIB_RGB_COLORS, SRCCOPY);
}


#define IDT_TIMER 1 // Define a timer ID
int animatedX = 50; // Initial x position of the animated element
int velocity = 5; // Speed at which the element moves

LRESULT CALLBACK MainWindowCallback(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
  /* HDC hdc; */
  /* PAINTSTRUCT ps; */

  LRESULT result = 0;

  switch (message) {
  case WM_SIZE: {
    struct win32_window_dimension dimension = Win32GetWindowDimension(window);
    Win32ResizeDIBSection(&GlobalBackBuffer, dimension.width, dimension.height);
    /* Win32ResizeDIBSection() */
  } break;
  case WM_CLOSE: {
    running = FALSE;
    /* OutputDebugStringA("WM_CLOSE\n"); */
    /* DestroyWindow(window); */
  } break;
  case WM_DESTROY: {
    running = FALSE;
    /* OutputDebugStringA("WM_DESTROY\n"); */
    /* PostQuitMessage(0); */
  } break;
  case WM_ACTIVATEAPP: {
    OutputDebugStringA("WM_ACTIVATEAPP");
  } break;
  case WM_PAINT: {
    PAINTSTRUCT paint;
    HDC devicectx = BeginPaint(window, &paint);
    int x = paint.rcPaint.left;
    int y = paint.rcPaint.top;
    int w = paint.rcPaint.right - paint.rcPaint.left;
    int h = paint.rcPaint.bottom - paint.rcPaint.top;

    struct win32_window_dimension dimension = Win32GetWindowDimension(window);
    Win32DisplayBufferInWindow(devicectx, dimension.width, dimension.height,
                               GlobalBackBuffer, x, y, w, h);
    EndPaint(window, &paint);
          
    // Begin painting
    /* hdc = BeginPaint(window, &ps); */
    /* // Set background color to light blue */
    /* HBRUSH hBrush = CreateSolidBrush(RGB(173, 216, 230)); */
    /* FillRect(hdc, &ps.rcPaint, hBrush); */
    /* DeleteObject(hBrush); */

    /* // Set fill color for the animated element (red) */
    /* HBRUSH hAnimBrush = CreateSolidBrush(RGB(255, 0, 0)); */
    /* SelectObject(hdc, hAnimBrush); */
    /* // Draw the animated rectangle at position (animatedX, 100) */
    /* Rectangle(hdc, animatedX, 100, animatedX + 50, 150); */
    /* DeleteObject(hAnimBrush); */

    /* // End painting */
    /* EndPaint(window, &ps); */
  } break;

    /* case WM_TIMER: { */
    /*     // Update the position of the animated element */
    /*     animatedX += velocity; */

    /*     // Reverse direction if hitting window bounds */
    /*     if (animatedX > 800 || animatedX < 0) { // 800 is the width of the window */
    /*         velocity = -velocity; // Reverse direction */
    /*     } */

    /*     // Invalidate the client area to trigger a repaint */
    /*     InvalidateRect(window, NULL, TRUE); */
    /*     return 0; */
    /* } */

  default:
    return DefWindowProc(window, message, wparam, lparam);
  }
  return result;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previnstance, LPSTR cmdline, int showcode) {
  Win32LoadXInput();
  WNDCLASSA winclass = {};
  winclass.lpfnWndProc = MainWindowCallback;
  winclass.hInstance = instance;
  winclass.lpszClassName = "window";

  RegisterClassA(&winclass);

  HWND window = CreateWindowExA(0,
                                winclass.lpszClassName,
                                "Windows API Animation Example",
                                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                800, 600,
                                0, 0, instance, 0);

  if (window) {
    int xoffset = 0;
    int yoffset = 0;
    running = TRUE;
      
    while (running) {
      MSG message;
      while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
        if (message.message == WM_QUIT) {
          running = FALSE;
        }
        TranslateMessage(&message);
        DispatchMessageA(&message);
      }

      for (DWORD cindex = 0; cindex < XUSER_MAX_COUNT; ++cindex) {
        XINPUT_STATE cstate;
        if (XInputGetState(cindex, &cstate) == ERROR_SUCCESS) {
          XINPUT_GAMEPAD *pad = &cstate.Gamepad;
          BOOL up = (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
          BOOL down = (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
          BOOL left = (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
          BOOL right = (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
          BOOL back = (pad->wButtons & XINPUT_GAMEPAD_BACK);
          BOOL leftsh = (pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
          BOOL rightsh = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
          BOOL a = (pad->wButtons & XINPUT_GAMEPAD_A);
          BOOL b = (pad->wButtons & XINPUT_GAMEPAD_B);
          BOOL x = (pad->wButtons & XINPUT_GAMEPAD_X);
          BOOL y = (pad->wButtons & XINPUT_GAMEPAD_Y);

          int16 stickx = pad->sThumbLX;
          int16 sticky = pad->sThumbLY;
          
        } else {
          
        }
      }

        
      RenderWeirdGradient(GlobalBackBuffer, xoffset, yoffset);
      HDC devicectx = GetDC(window);
      struct win32_window_dimension dimension = Win32GetWindowDimension(window);
      Win32DisplayBufferInWindow(devicectx, dimension.width, dimension.height,
                                 GlobalBackBuffer,
                                 0, 0, dimension.width, dimension.height);
      ReleaseDC(window, devicectx);
      ++xoffset;
      yoffset += 2;
    }
  }
  return 0;
}
