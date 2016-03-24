# Windows

In order to show something on the screen, we have to deal with **Window System Integration (WSI)**. The documentation I'm going to be using can be found [here](https://www.khronos.org/registry/vulkan/specs/1.0/refguide/Vulkan-1.0-web.pdf). To follow along, you can go to section **29.2.4**.

# `HINSTANCE`

# `HWND`

# `VkAndroidSurfaceCreateInfoKHR`

Because we're getting into platform specific code, documentation can be a little hard to find. If you clicked on the documentation I linked, you'll see it's a quick reference guide. Looking at the documentation, we'll see in order to use `vkCreateWin32SurfaceKHR`, we'll need to create a `VkWin32SurfaceCreateInfoKHR` object. The definition looks like this...

```cpp
typedef struct VkWin32SurfaceCreateInfoKHR {
    VkStructureType sType;
    const void* pNext;
    VkWin32SurfaceCreateFlagsKHR flags;
    HINSTANCE hinstance;
    HWND hwnd;
} VkWin32SurfaceCreateInfoKHR;
```

Following the valid usage from other sections, we can say that creating a `VkAndroidSurfaceCreateInfoKHR` object looks like...

```cpp
VkWin32SurfaceCreateInfoKHR createInfo{};
createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
createInfo.pNext = NULL;
createInfo.flags = 0;
createInfo.hinstance = (HINSTANCE)platformHandle;
createInfo.hwnd = (HWND)platformWindow;
```