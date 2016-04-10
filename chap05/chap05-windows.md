# Windows

We're going to be writing the Windows specific code for getting a surface in this section. Now, the code here does not exist in the specification. It is up to platforms like Windows to expose surface features on their own. For documentation, I recommend following the [Vulkan Quick Reference](https://www.khronos.org/files/vulkan10-reference-guide.pdf). You can find the platform specific WSI features on page 9.

## `VkWin32SurfaceCreateInfoKHR`

Before we create a surface, we must specify information ahead of time like most Vulkan objects. You'll see in section **29.2.4** that the structure we'll be using is `VkWin32SurfaceCreateInfoKHR`. Here is the definition:

```cpp
typedef struct VkWin32SurfaceCreateInfoKHR {
    VkStructureType              sType;
    const void*                  pNext;
    VkWin32SurfaceCreateFlagsKHR flags;
    HINSTANCE                    hinstance;
    HWND                         hwnd;
} VkWin32SurfaceCreateInfoKHR
```

We should follow the valid usage for `sType`, `pNext`, and `flags` in other structures. That means `pNext = NULL` and `flags = 0`. We'll also grab our `windowInstance` and `window` from earlier. Here's what the code looks like:

```cpp
VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
surfaceCreateInfo.pNext = NULL;
surfaceCreateInfo.flags = 0;
surfaceCreateInfo.hinstance = windowInstance;
surfaceCreateInfo.hwnd = window;
```

## `vkCreateWin32SurfaceKHR`

Now we can create the surface. Also in section **29.2.4** is the definition for `vkCreateWin32SurfaceKHR`. It looks like:

```cpp
VkResult vkCreateWin32SurfaceKHR(
    VkInstance                         instance,
    const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
    const VkAllocationCallbacks*       pAllocator,
    VkSurfaceKHR*                      pSurface
)
```

Again, we should follow the usage we've seen before for `pAllocator`. This would mean we pass in `NULL` to the function as the third argument. Let's look at the code:

```cpp
VkResult result =
    vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
```

Of course, we'll also need to verify our surface creation was successful using:

```cpp
if (result != VK_SUCCESS) exitOnError("Failed to create VkSurfaceKHR.");
```

## Color Formats and Color Spaces

Please go back to the [Chapter 5 landing page](./chap05.md) to read about determining the color formats and color spaces for the surface.
