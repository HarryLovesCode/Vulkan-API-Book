# Windows

In order to create a window, we're going to be using platform specific code. Please note that this tutorial is for Windows **only**. None of this will apply to Linux, Android, GLFW, etc.

# Including Headers

Because we're going to be writing this from scratch, we're going to have to interact with Windows directly. Before we can do anything, we need to include the Windows header. If you're only targeting Windows, you can write:

```cpp
#include <windows.h>
```

If you're targeting multiple platforms including Windows, you can write:

```cpp
#if defined(_WIN32)
#include <Windows.h>
#endif
```

# Creating a Window

As mentioned before, because we're writing this without any windowing libraries, we'll have to use the Win32 API. I won't go too much into detail because our focus is Vulkan and not Windows. Let's go ahead and list the variables we'll be using:

```cpp
const int windowWidth = 1280;
const int windowHeight = 720;

HINSTANCE windowInstance;
HWND window;
```

In this section we'll be writing the body this method:

```cpp
void VulkanExample::createWindow(HINSTANCE hInstance, WNDPROC wndProc)
{

}
```

Don't worry about `hInstance` and `wndProc` for now. Those are simply passed from the `WinMain` method we'll write later on. To setup our window, we'll need to register it with Windows. You can find the documentation for the `RegisterClassEx` method [here](https://goo.gl/m3WViB). The definition looks like this:

```cpp
ATOM WINAPI RegisterClassEx(
    _In_ const WNDCLASSEX *lpwcx
);
```

As you'll see, we'll need to call it with a `WNDCLASSEX` object. You can find the documentation [here](https://goo.gl/1M92FX) and the definition looks like this:

```cpp
typedef struct tagWNDCLASSEX {
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
```

Here's the code we'll be using to set it up:

```cpp
WNDCLASSEX wcex;
wcex.cbSize = sizeof(WNDCLASSEX);
wcex.style = CS_HREDRAW | CS_VREDRAW;
wcex.lpfnWndProc = wndProc;
wcex.cbClsExtra = 0;
wcex.cbWndExtra = 0;
wcex.hInstance = hInstance;
wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
wcex.lpszMenuName = NULL;
wcex.lpszClassName = engineName.c_str();
wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
```

As I said, I'm not going to go too much into detail on what each field means so we'll move on to registering the window. Calling `RegisterClassEx` returns `NULL` upon failure so we should make sure we check for that.

```cpp
if (!RegisterClassEx(&wcex)) {
    fprintf(stderr, "Call to RegisterClassEx failed\n");
    exit(EXIT_FAILURE);
}
```

Assuming all has gone well, we can set our `windowInstance` variable.

```cpp
windowInstance = hInstance;
```

While it's not required, I'd recommend centering the window on the screen upon showing it. We'll need to use the `GetSystemMetrics` method to get two values:

- Screen width (`SM_CXSCREEN`)
- Screen height (`SM_CYSCREEN`)

Once we have these, we can do that math required to center the window:

$$W_{left} = S_{width} / 2 - W_{width} / 2$$

$$W_{top} = S_{height} / 2 - W_{height} / 2$$

To get all of this information and compute the window's left and top positions, we can write the following code:

```cpp
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
int windowLeft = screenWidth / 2 - windowWidth / 2;
int windowTop = screenHeight / 2 - windowHeight / 2;
```

Finally, we can call Window's `CreateWindow` method. You can find documentation [here](https://goo.gl/dmHFfS) and the definition looks like this:

```cpp
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
```

Valid usage following the documentation looks like this:

```cpp
window = CreateWindow(
    engineName.c_str(),
    applicationName.c_str(),
    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
    windowLeft, windowTop,
    windowWidth, windowHeight,
    NULL,
    NULL,
    hInstance,
    NULL);
```

The `CreateWindow` method also returns `NULL` upon failure. Let's deal with that possibility before we move on:

```cpp
if (!window) {
    fprintf(stderr, "Failed to create Win32 window\n");
    exit(EXIT_FAILURE); 
}
```

Last, but not least, we should show the window, set it in the foreground, and focus it. Windows provides three methods that do exactly that:

```cpp
ShowWindow(window, SW_SHOW);
SetForegroundWindow(window);
SetFocus(window);
```

# Updating the Window

For this section, we'll be writing the body of this method:

```cpp
void VulkanExample::updateWindow(UINT message, WPARAM wParam, LPARAM lParam)
{

}
```

Essentially what we need to do is check if the window was closed. If it was, we should destroy it and tell Windows we're exiting. You can do this like so:

```cpp
switch (message) {
case WM_CLOSE:
    DestroyWindow(window);
    PostQuitMessage(0);
    break;
}
```

# `WndProc`

This method is called every time the application receives any input. This includes closing, key presses, mouse presses, and etc. All we're going to do is call out `updateWindow` method from here and pass some arguments. You can find documentation [here](https://goo.gl/aKPbCp) and this is the definition:

```cpp
LRESULT CALLBACK WndProc(
    HWND hWnd, 
    UINT uMsg, 
    WPARAM wParam, 
    LPARAM lParam);
```

Here is what our implementation looks like:

```cpp
vulkanExample->updateWindow(uMsg, wParam, lParam);
return DefWindowProc(hWnd, uMsg, wParam, lParam);
```

# `WinMain`

This is our application's new entry-point. We will **not** be using your typical `int main(void)` entry-point. This is because Windows requires GUI applications to enter at `WinMain`. We need to:

- Create an instance of our class
- Call our `createWindow` method
- Create an empty message
- Loop while propagating the message
  - Call `TranslateMessage` (required)
  - Call `DispatchMessage` (required)

You can find documentation on the `WinMain` entry-point [here](https://goo.gl/uToSOo) and the definition is:

```cpp
int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
);
```

All we have to do now is implement it:

```cpp
VulkanExample * vulkanExample;

void VulkanExample::createWindow(HINSTANCE hInstance, WNDPROC wndProc)
{
    vulkanExample = new VulkanExample();
    vulkanExample->createWindow(hInstance, WndProc);

    MSG message;

    while (GetMessage(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    delete vulkanExample;
}
```

# Resources

- [Creating a window walkthrough](https://msdn.microsoft.com/en-us/library/bb384843.aspx)