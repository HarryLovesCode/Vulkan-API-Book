## Windows

We're going to be writing the Windows specific code for getting a surface in this section.

### `VkWin32SurfaceCreateInfoKHR`

Before we create a surface, we must specify information ahead of time like most Vulkan objects. We'll be using `VkWin32SurfaceCreateInfoKHR` which you can find documentation on [here](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#VkWin32SurfaceCreateInfoKHR) and the definition is below:

```cpp
typedef struct VkWin32SurfaceCreateInfoKHR {
  VkStructureType              sType;
  const void*                  pNext;
  VkWin32SurfaceCreateFlagsKHR flags;
  HINSTANCE                    hinstance;
  HWND                         hwnd;
} VkWin32SurfaceCreateInfoKHR
```

- `sType` is the type of this structure and must be `VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR`.
- `pNext` is `NULL` or a pointer to an extension-specific structure.
- `flags` is reserved for future use.
- `hinstance` and `hwnd` are the `WIN32 HINSTANCE` and `HWND` for the window to associate the surface with.

Here's what the code looks like:

```cpp
VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
surfaceCreateInfo.pNext = NULL;
surfaceCreateInfo.flags = 0;
surfaceCreateInfo.hinstance = windowInstance;
surfaceCreateInfo.hwnd = window;
```

### `vkCreateWin32SurfaceKHR`

Now we can create the surface. We can make a call to the `vkCreateWin32SurfaceKHR` method. You can find documentation on it [here](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#vkCreateWin32SurfaceKHR) and the definition is below:

```cpp
VkResult vkCreateWin32SurfaceKHR(
  VkInstance                         instance,
  const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
  const VkAllocationCallbacks*       pAllocator,
  VkSurfaceKHR*                      pSurface
)
```

- `instance` is the instance to associate the surface with.
- `pCreateInfo` is a pointer to an instance of the `VkWin32SurfaceCreateInfoKHR` structure containing parameters affecting the creation of the surface object.
- `pAllocator` is the allocator used for host memory allocated for the surface object when there is no more specific allocator available.
- `pSurface` points to a `VkSurfaceKHR` handle in which the created surface object is returned.

Let's look at the code:

```cpp
VkResult result =
    vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
assert(result == VK_SUCCESS);
```

Please go back to the [Chapter 5 page](./chap05.md) to read about determining the color formats and color spaces for the surface.
