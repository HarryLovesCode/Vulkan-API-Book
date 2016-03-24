# Windows

In order to show something on the screen, we have to deal with **Window System Integration (WSI)**. The documentation I'm going to be using can be found [here](https://www.khronos.org/registry/vulkan/specs/1.0/refguide/Vulkan-1.0-web.pdf). To follow along, you can go to section **29.2.4**.

# Creating a Window

Creating a window for display on Windows is more work than you'd expect. You can find information on how I've created my implementation [here](https://msdn.microsoft.com/en-us/library/bb384843.aspx). So the first thing we need to know is what headers we should include. The information I linked tells us that we should include...

```cpp
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
```

The next thing we need to know is whether or not the entry point is different. It is in fact something other than `main` similar to a few GUI tool-kits I've seen. The definition for the Win32 application main method looks like this...

```cpp
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow);
```

Finally, before we can begin creating the window and adding functionality, we need another function. We'll follow the information I linked and call it `WndProc`. Here's the definition...

```cpp
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
```