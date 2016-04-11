## Linux

We're going to be writing the Linux specific code for getting a surface in this section. While this code may work on another operating system that uses the XCB library, I cannot guarantee it will.

### `VkXcbSurfaceCreateInfoKHR`

Before we create a surface, we must specify information ahead of time like most Vulkan objects. We'll be using `VkXcbSurfaceCreateInfoKHR`.

**Definition for `VkXcbSurfaceCreateInfoKHR`**:

```cpp
typedef struct VkXcbSurfaceCreateInfoKHR {
  VkStructureType             sType;
  const void*                 pNext;
  VkXcbSurfaceCreateFlagsKHR  flags;
  xcb_connection_t*           connection;
  xcb_window_t                window;
} VkXcbSurfaceCreateInfoKHR;
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#VkXcbSurfaceCreateInfoKHR) for `VkXcbSurfaceCreateInfoKHR`**:

- `sType` is the type of this structure and must be `VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR`.
- `pNext` is `NULL` or a pointer to an extension-specific structure.
- `flags` is reserved for future use.
- `connection` is a pointer to an `xcb_connection_t` to the X server.
- `window` is the `xcb_window_t` for the X11 window to associate the surface with.

**Usage for `VkXcbSurfaceCreateInfoKHR`**:

```cpp
VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
surfaceCreateInfo.pNext = NULL;
surfaceCreateInfo.flags = 0;
surfaceCreateInfo.connection = connection;
surfaceCreateInfo.window = window;
```

### `vkCreateXcbSurfaceKHR`

Now we can create the surface. We'll be calling the `vkCreateXcbSurfaceKHR` method to do so.

**Definition for `vkCreateXcbSurfaceKHR`**:

```cpp
VkResult vkCreateXcbSurfaceKHR(
  VkInstance                        instance,
  const VkXcbSurfaceCreateInfoKHR*  pCreateInfo,
  const VkAllocationCallbacks*      pAllocator,
  VkSurfaceKHR*                     pSurface);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#vkCreateXcbSurfaceKHR) for `vkCreateXcbSurfaceKHR`**:

- `instance` is the instance to associate the surface with.
- `pCreateInfo` is a pointer to an instance of the `VkXcbSurfaceCreateInfoKHR` structure containing parameters affecting the creation of the surface object.
- `pAllocator` is the allocator used for host memory allocated for the surface object when there is no more specific allocator available.
- `pSurface` points to a `VkSurfaceKHR` handle in which the created surface object is returned.

**Usage for `vkCreateXcbSurfaceKHR**:

```cpp
VkResult result =
    vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
assert(result == VK_SUCCESS);
```

Please go back to the [Chapter 5 page](./chap05.md) to read about determining the color formats and color spaces for the surface.
