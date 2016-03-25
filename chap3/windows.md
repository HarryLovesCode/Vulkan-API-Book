# Windows

In order to show something on the screen, we have to deal with **Window System Integration (WSI)**. The documentation I'm going to be using can be found [here](https://www.khronos.org/registry/vulkan/specs/1.0/refguide/Vulkan-1.0-web.pdf). To follow along, you can go to section **29.2.4**.

# Preparing for Window Creation

Because we're going to be writing this from scratch, we're going to have to interact with Windows directly. The first thing we need to know is what headers we should include. The information I linked tells us that we should include these headers:

```cpp
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
```

The next thing we need to know is whether or not the entry point is different. It is in fact something other than `main` similar to a few GUI tool-kits I've seen. The definition for the Win32 application main method looks like this:

```cpp
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow);
```

Before we can begin creating the window and adding functionality, we need another function. Here's the definition:

```cpp
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
```

We'll come back to implement the bodies for these two methods later in this section.

# Creating the Window

Window creation isn't anything crazy. We're just going to be creating a basic window.

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
wcex.lpszClassName = windowClassName.c_str();
wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

if (!RegisterClassEx(&wcex)) {
    fprintf(stderr, "Call to RegisterClassEx failed\n");
    exit(EXIT_FAILURE);
}

windowInstance = hInstance;
```

We'll talk about full-screen at a later time. For now, we're just going to create a window and center it on the screen. First, we need to ask the system for the width and height of the screen. We can use the `GetSystemMetrics` method to do this.

```cpp
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
```

You probably know how to center something, but you may not know how to do it on a display. The way we do it is...

$$X_{window} = W_{screen} / 2 - W_{window} / 2 $$

$$Y_{window} = H_{screen} / 2 - H_{window} / 2 $$

The implementation in code looks like this:

```cpp
int windowX = screenWidth / 2 - windowWidth / 2;
int windowY = screenHeight / 2 - windowHeight / 2;
```

Now that we've prepared everything for our window, we can go ahead and create it. This is the definition for `CreateWindow` if you're curious:

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

Now for the usage. I use the following to create my window:

```cpp
std::string windowTitle = "Vulkan Example";
std::string windowClassName = "vulkanEngine";
int windowWidth = 1280;
int windowHeight = 720;

window = CreateWindow(
    windowClassName.c_str(), 
    windowTitle.c_str(), 
    WS_OVERLAPPEDWINDOW,
    windowX, 
    windowY, 
    windowWidth,
    windowHeight, 
    NULL, 
    NULL, 
    hInstance,
    NULL);
```

Next we need to verify our window was created. We can simply check if `window == NULL`.

```cpp
if (!window) {
    fprintf(stderr, "Failed to create window\n");
    exit(EXIT_FAILURE);
}
```

If the window was created, we should:

- Show it on the screen
- Set it to the foreground
- Set it to the active (focused) window

Luckily, Windows provides functions for those three operations.

```cpp
ShowWindow(window, SW_SHOW);
SetForegroundWindow(window);
SetFocus(window);
```

# Updating the Window

This section is super simple. We just need to check if the user tried to close the window.

```cpp
void VulkanExample::updateWindow(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CLOSE:
		DestroyWindow(window);
		PostQuitMessage(0);
		break;
	}
}
```

# Coming Back to `WndProc`

We'll need to write the `WndProc` method I mentioned earlier. This will pass some of the arguments onto our `updateWindow` method.

```cpp
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	vExample.updateWindow(uMsg, wParam, lParam);
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
```

# Coming Back to `WinMain`

Here's the entry point to our program. It will replace the normal main method. We're going to be using the setup outlined [here](https://msdn.microsoft.com/en-us/library/windows/desktop/ms644936(v=vs.85.aspx) to act as the main application loop. The code looks like this:

```cpp
MSG message;

while (GetMessage(&message, NULL, 0, 0)) {
    TranslateMessage(&message);
    DispatchMessage(&message);
}
```