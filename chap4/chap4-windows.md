# Windows

In order to create a window, we're going to be using platform specific code. Please note that this tutorial is for Windows **only**. None of this will apply to Linux, Android, GLFW, etc.

## Including Headers

Because we're going to be writing this from scratch, we're going to have to interact with Windows directly. Before we can do anything, we need to include the Windows header. If you're only targeting Windows, you can write:

```cpp
#include <windows.h>
```

If you're targeting Linux as well, you should surround both by the `#if defined(_WIN32)` directives.

## Allocating a Console

Because we're now switching from a **Windows Console Application** to a **Windows Application**, we'll need to make sure we have a console to view the output of `stdout` and `stderr`. In addition, because we're exiting right after we encounter an error, we should show a message box, wait for input, then close after the user has acknowledged the error. So, before we call any functions in our constructor, let's add the following code:

```cpp
AllocConsole();
AttachConsole(GetCurrentProcessId());
freopen("CON", "w", stdout);
freopen("CON", "w", stderr);
SetConsoleTitle(TEXT(applicationName));
```

Now, let's modify our `exitOnError` method to show a error message box:

```cpp
MessageBox(NULL, msg, applicationName, MB_ICONERROR);
```

## Creating a Window

As mentioned before, because we're writing this without any windowing libraries, we'll have to use the Win32 API. I won't go too much into detail because **our focus is Vulkan** and not Windows. Let's go ahead and list the variables we'll be using:

```cpp
const int windowWidth = 1280;
const int windowHeight = 720;

HINSTANCE windowInstance;
HWND window;
```

In this section we'll be writing the body this method:

```cpp
void VulkanExample::createWindow(HINSTANCE hInstance) {}
```

Don't worry about `hInstance` for now. It is simply passed from the `WinMain` method we'll write later on. To setup our window, we'll need to register it with Windows. You can find the documentation for the `RegisterClassEx` method [here](https://goo.gl/m3WViB). The definition looks like this:

```cpp
ATOM WINAPI RegisterClassEx(
  _In_ const WNDCLASSEX *lpwcx
);
```

As you see, we'll need to call it with a `WNDCLASSEX` object. You can find the documentation [here](https://goo.gl/1M92FX). The definition and valid usage look like this:

```cpp
// Definition
typedef struct WNDCLASSEX {
  UINT      cbSize;
  UINT      style;
  WNDPROC   lpfnWndProc;
  int       cbClsExtra;
  int       cbWndExtra;
  HINSTANCE hInstance;
  HICON     hIcon;
  HCURSOR   hCursor;
  HBRUSH    hbrBackground;
  LPCTSTR   lpszMenuName;
  LPCTSTR   lpszClassName;
  HICON     hIconSm;
} WNDCLASSEX, *PWNDCLASSEX;

// Usage
WNDCLASSEX wcex;

wcex.cbSize = sizeof(WNDCLASSEX);
wcex.style = CS_HREDRAW | CS_VREDRAW;
wcex.lpfnWndProc = WndProc;
wcex.cbClsExtra = 0;
wcex.cbWndExtra = 0;
wcex.hInstance = hInstance;
wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
wcex.lpszMenuName = NULL;
wcex.lpszClassName = applicationName;
wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
```

As I said, I'm not going to go too much into detail on what each field means so we'll move on to registering the window. Calling `RegisterClassEx` returns `NULL` upon failure so we should make sure we check for that.

```cpp
if (!RegisterClassEx(&wcex))
  exitOnError("Failed to register window");
```

Assuming all has gone well, we can set our `windowInstance` variable.

```cpp
windowInstance = hInstance;
```

While it's not required, I'd recommend centering the window on the screen upon showing it. We'll need to use the `GetSystemMetrics` method to get two values:

- Screen width (`SM_CXSCREEN`)
- Screen height (`SM_CYSCREEN`)

Once we have these, we can do the math required to center the window:

$$W_{left} = S_{width} / 2 - W_{width} / 2$$

$$W_{top} = S_{height} / 2 - W_{height} / 2$$

To get all of this information and compute the window's left and top positions, we can write the following code:

```cpp
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
int windowLeft = screenWidth / 2 - windowWidth / 2;
int windowTop = screenHeight / 2 - windowHeight / 2;
```

Finally, we can call Window's `CreateWindow` method. You can find documentation [here](https://goo.gl/dmHFfS). The definition and valid usage look like this:

```cpp
// Definition
HWND WINAPI CreateWindow(
    _In_opt_ LPCTSTR   lpClassName,
    _In_opt_ LPCTSTR   lpWindowName,
    _In_     DWORD     dwStyle,
    _In_     int       x,
    _In_     int       y,
    _In_     int       nWidth,
    _In_     int       nHeight,
    _In_opt_ HWND      hWndParent,
    _In_opt_ HMENU     hMenu,
    _In_opt_ HINSTANCE hInstance,
    _In_opt_ LPVOID    lpParam
);

// Usage
window = CreateWindow(
  applicationName,
  applicationName,
  WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
  windowX,
  windowY,
  windowWidth,
  windowHeight,
  NULL,
  NULL,
  windowInstance,
  NULL);
```

The `CreateWindow` method also returns `NULL` upon failure. Let's deal with that possibility before we move on:

```cpp
if (!window) exitOnError("Failed to create window");
```

Last, but not least, we should show the window, set it in the foreground, and focus it. Windows provides three methods that do exactly that:

```cpp
ShowWindow(window, SW_SHOW);
SetForegroundWindow(window);
SetFocus(window);
```

## `WndProc`

For this section, we'll be writing the body of this method:

```cpp
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {}
```

We need to destroy the window and tell Windows we quit if the user attempted to close the window. If we're told we need to paint, we'll simply update the window. If neither of those cases we're met, we'll simply call the window's default procedure to handle events we didn't process. You can do this like so:

```cpp
switch (message) {
  case WM_DESTROY:
    DestroyWindow(hWnd);
    PostQuitMessage(0);
    break;
  case WM_PAINT:
    ValidateRect(hWnd, NULL);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
    break;
}
```

## Our Update Loop

For this section, we'll write the body of this method:

```cpp
void VulkanExample::renderLoop() {}
```

We're calling it `renderLoop` because later we'll make calls to rendering functions within it. For now, however, we're going to create a message, loop while we have Windows set it, Windows translate it into a character message then add it to the thread queue, and dispatch the message to the windows procedure. While that sounds complicated, it can be done with just a few lines of code:

```cpp
MSG message;

while (GetMessage(&message, NULL, 0, 0)) {
  TranslateMessage(&message);
  DispatchMessage(&message);
}
```

## `WinMain`

This is our application's new entry-point. We will **not** be using your typical `int main(int argc, char * argv[])` entry-point. This is because Windows requires GUI applications to enter at `WinMain`. We need to:

- Create an instance of our class
- Call our `initWindow` method
- Call our `renderLoop`

You can find documentation on the `WinMain` entry-point [here](https://goo.gl/uToSOo). The definition and our usage are:

```cpp
// Definition
int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
);

// Our implementation
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  VulkanExample ve = VulkanExample();
  ve.initWindow(hInstance);
  ve.renderLoop();
}
```
