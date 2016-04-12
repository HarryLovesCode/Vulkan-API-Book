## Creating a Window on Microsoft Windows

In order to create a window, we're going to be using platform specific code. Please note that this tutorial is for Windows **only**. None of this will apply to Linux, Android, GLFW, etc. I'll make the warning ahead of time: this chapter makes use of a lot of functions. Their definitions are not super relevant to you as a Vulkan developer because this will be written once.

### Including Headers

Because we're going to be writing this from scratch, we're going to have to interact with Windows directly. Before we can do anything, we need to include the Windows header. If you're only targeting Windows, you can write:

```cpp
#include <windows.h>
```

If you're targeting Linux as well, you should surround both by the `#if defined(_WIN32)` directives.

### Setting Up a Console Window

Because we're now switching from a **Windows Console Application** to a **Windows Application**, we'll need to make sure we have a console to view the output of `stdout` and `stderr`. Also, because we're exiting right after we encounter an error, we should:

- Show a message box
- Wait for user input (keypress)
- Close after the user has acknowledged the error

We'll be using four methods to do this work.

**Definition for `AllocConsole`**:

```cpp
BOOL WINAPI AllocConsole(void);
```

**[Documentation](https://goo.gl/8k36tq) for `AllocConsole`**:

- This function takes no arguments

**Usage for `AllocConsole`**:

```cpp
AllocConsole();
```

**Definition for `AttachConsole`**:

```cpp
BOOL WINAPI AttachConsole(
  DWORD dwProcessId
);
```

**[Documentation](https://goo.gl/EeSrhh) for `AttachConsole`**:

- `dwProcessId` is the identifier of the process whose console is to be used.

**Usage for `AttachConsole`**:

```cpp
AttachConsole(GetCurrentProcessId());
```

**Definition for `freopen`**:

```cpp
FILE * freopen (
  const char * filename,
  const char * mode,
  FILE * stream );
```

**[Documentation](http://www.cplusplus.com/reference/cstdio/freopen/) for `freopen`**:

- `fileName` is a C string containing the name of the file to be opened.
- `mode` is a C string containing a file access mode. It can be:
  - `"r"`
  - `"w"`
  - `"a"`
  - etc.
- `stream` is a pointer to a `FILE` object that identifies the stream to be reopened.

**Usage for `freopen`**:

```cpp
freopen("CON", "w", stdout);
freopen("CON", "w", stderr);
```

**Definition for `SetConsoleTitle`**:

```cpp
BOOL WINAPI SetConsoleTitle(
  LPCTSTR lpConsoleTitle
);
```

**[Documentation](https://goo.gl/HAIfMd) for `SetConsoleTitle`**:

- `lpConsoleTitle` is the string to be displayed in the title bar of the console window. The total size must be less than 64K.

**Usage for `SetConsoleTitle`**:

```cpp
SetConsoleTitle(TEXT(applicationName));
```

If you put these methods together you can:

- Allocate a console
- Attach the console to the current process
- Redirect `stdout` and `stderr` to said console
- Set the title of the console window

Now, let's modify our `exitOnError` method to show a error message box. We'll need to use the `MessageBox` method.

**Definition for `MessageBox`**:

```cpp
int WINAPI MessageBox(
  HWND    hWnd,
  LPCTSTR lpText,
  LPCTSTR lpCaption,
  UINT    uType
);
```

**[Documentation](https://goo.gl/7tAVnv) for `MessageBox`**:

- `hWnd` is a handle to the owner window of the message box to be created. If this parameter is `NULL`, the message box has no owner window.
- `lpText` is the message to be displayed. If the string consists of more than one line, you can separate the lines using a carriage return and/or linefeed character between each line.
- `lpCaption` is the dialog box title. If this parameter is `NULL`, the default title is "Error".
- `uType` is the contents and behavior of the dialog box. This parameter can be a combination of flags.

**Usage for `MessageBox`**:

```cpp
MessageBox(NULL, msg, applicationName, MB_ICONERROR);
```

### Creating a Window

As mentioned before, because we're writing this without any windowing libraries, we'll have to use the Win32 API. I won't go too much into detail because **our focus is Vulkan** and not Windows. Let's go ahead and list the variables we'll be using:

```cpp
const uint32_t windowWidth = 1280;
const uint32_t windowHeight = 720;

HINSTANCE windowInstance;
HWND window;
```

In this section we'll be writing the body this method:

```cpp
void initWindow(HINSTANCE hInstance) {}
```

Don't worry about `hInstance` for now. It is passed from the `WinMain` method we'll write later on. To setup our window, we'll need to register it with Windows, but first, we need to create a `WNDCLASSEX` object to pass during registration.

```cpp
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
```

**[Documentation](https://goo.gl/1M92FX) for `WNDCLASSEX`**:

- `cbSize` is the size, in bytes, of this structure. Set this member to `sizeof(WNDCLASSEX)`. Be sure to set this member before calling the `GetClassInfoEx` function.
- `style` is the class style(s). This member can be any combination of the Class Styles.
- `lpfnWndProc` is a pointer to the window procedure. You must use the `CallWindowProc` function to call the window procedure.
- `cbClsExtra` is the number of extra bytes to allocate following the window-class structure.
- `cbWndExtra` is the number of extra bytes to allocate following the window instance.
- `hInstance` is a handle to the instance that contains the window procedure for the class.
- `hIcon` is a handle to the class icon. This member must be a handle to an icon resource. If this member is `NULL`, the system provides a default icon.
- `hCursor` is a handle to the class cursor.
- `hbrBackground` A handle to the class background brush.
- `lpszMenuName` is a pointer to a null-terminated character string that specifies the resource name of the class menu, as the name appears in the resource file. If you use an integer to identify the menu, use the `MAKEINTRESOURCE` macro.
- `lpszClassName` is a pointer to a null-terminated string or is an atom.
- `hIconSm` is a handle to a small icon that is associated with the window class.

**Usage for `WNDCLASSEX`**:

```cpp
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

Now, we can make a call to `RegisterClassEx` to get Windowss to register the Window class.

**Definition for `RegisterClassEx`**:

```cpp
ATOM WINAPI RegisterClassEx(
  const WNDCLASSEX *lpwcx
);
```

**[Documentation](https://goo.gl/m3WViB) for `RegisterClassEx`**:

- `lpwcx` is a pointer to a `WNDCLASSEX` structure. You must fill the structure with the appropriate class attributes before passing it to the function.

**Usage for `RegisterClassEx`**:

Calling `RegisterClassEx` returns `NULL` upon failure so we should make sure we check for that.

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

To get this information and compute the window's left and top positions, we can write the following code:

```cpp
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
int windowLeft = screenWidth / 2 - windowWidth / 2;
int windowTop = screenHeight / 2 - windowHeight / 2;
```

Finally, we can call Window's `CreateWindow` method. This will, as the name suggests, create the window like we want. We'll specify dimensions, location, and other parameters.

**Definition for `CreateWindow`**:

```cpp
HWND WINAPI CreateWindow(
  LPCTSTR  lpClassName,
  LPCTSTR   lpWindowName,
  DWORD     dwStyle,
  int       x,
  int       y,
  int       nWidth,
  int       nHeight,
  HWND      hWndParent,
  HMENU     hMenu,
  HINSTANCE hInstance,
  LPVOID    lpParam
);
```

**[Documentation](https://goo.gl/dmHFfS) for `CreateWindow`**:

- `lpClassName` is a null-terminated string or a class atom created by a previous call to the `RegisterClass` or `RegisterClassEx` function. The atom must be in the low-order word of `lpClassName`; the high-order word must be zero. If `lpClassName` is a string, it specifies the window class name.
- `lpWindowName` is the window name. If the window style specifies a title bar, the window title pointed to by lpWindowName is displayed in the title bar.
- `dwStyle` is the style of the window being created. This parameter can be a combination of the window style values.
- `x` is the initial horizontal position of the window.
- `y` is the initial vertical position of the window.
- `nWidth` is the width, in device units, of the window.
- `nHeight` is the height, in device units, of the window.
- `hWndParent` is a handle to the parent or owner window of the window being created or `NULL` in our case.
- `hMenu` is a handle to a menu, or specifies a child-window identifier depending on the window style or `NULL` in our case.
- `hInstance` is a handle to the instance of the module to be associated with the window.
- `lpParam` is a pointer to a value to be passed to the window through the `CREATESTRUCT` structure (`lpCreateParams` member) pointed to by the `lParam` param of the `WM_CREATE` message or `NULL` in our case.

**Usage for `CreateWindow`**:

```cpp
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

The `CreateWindow` method returns `NULL` upon failure. Let's deal with that possibility before we move on:

```cpp
if (!window)
  exitOnError("Failed to create window");
```

Last, but not least, we should show the window, set it in the foreground, and focus it. Windows provides three methods that do exactly that. These are very self explanatory:

```cpp
ShowWindow(window, SW_SHOW);
SetForegroundWindow(window);
SetFocus(window);
```

### Window Process

For this section, we'll be writing the body of a method called `WndProc`. This is required by Windows to process window events.

```cpp
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {}
```

We need to destroy the window and tell Windows we quit if the user attempted to close the window. If we're told we need to paint, we'll simply update the window. If neither of those cases we're met, we'll use the default procedure to handle events we didn't process. You can do this like so:

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

### Our Update Loop

For this section, we'll write the body of this method:

```cpp
void VulkanSwapchain::renderLoop() {}
```

We're calling it `renderLoop` because later we'll make calls to rendering functions within it. For now, however, we're going to:

- Create a message, loop while we have Windows set it
- Windows translate it into a character message then add it to the thread queue
- Dispatch the message to the windows procedure.

While that sounds complicated, it can be done with just a few lines of code:

```cpp
MSG message;

while (GetMessage(&message, NULL, 0, 0)) {
  TranslateMessage(&message);
  DispatchMessage(&message);
}
```

### A New Entry-Point

This is our application's new entry-point. We will **not** be using your typical `int main()` entry-point. This is because Windows requires GUI applications to enter at `WinMain`. We need to:

- Create an instance of our class
- Call our `initWindow` method
- Call our `renderLoop`

**Definition for `WinMain`**:

```cpp
int CALLBACK WinMain(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nCmdShow
);
```

**[Documentation](https://goo.gl/uToSOo) for `WinMain`**:

- `hInstance` is a handle to the current instance of the application.
- `hPrevInstance` is a handle to the previous instance of the application. This parameter is always `NULL`.
- `lpCmdLine` is the command line for the application, excluding the program name.
- `nCmdShow` controls how the window is to be shown.

**Usage for `WinMain`**:

```cpp
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  VulkanSwapchain ve = VulkanSwapchain();
  ve.initWindow(hInstance);
  ve.renderLoop();
}
```
