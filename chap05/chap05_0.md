# Working With Surfaces

This section is divided up by platform. You must begin with the sections below before clicking on platform. The code below is platform independent and must be included in order to create a surface.

## Extensions

You may remember a while back that when we created our instance, we enabled a few extensions. These were platform specific extensions for surfaces. Because we did it already, there's no need to write that code now.

## Procedures

Vulkan doesn't directly expose functions for all platforms. Thus, we'll have to query Vulkan for them at run-time. We'll be getting the function pointers at the instance level and device level using the `vkGetInstanceProcAddr` and `vkGetDeviceProcAddr` methods.

**Definition for `vkGetInstanceProcAddr`**:

```cpp
PFN_vkVoidFunction vkGetInstanceProcAddr(
  VkInstance  instance,
  const char* pName);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#vkGetInstanceProcAddr) for `vkGetInstanceProcAddr`**:

- `instance` is the instance that the function pointer will be compatible with.
- `pName` is the name of the command to obtain.

**Usage for `vkGetInstanceProcAddr`**:

```cpp
#define GET_INSTANCE_PROC_ADDR(inst, entry)                              \
  {                                                                      \
    fp##entry = (PFN_vk##entry)vkGetInstanceProcAddr(inst, "vk" #entry); \
    if (!fp##entry)                                                      \
      exitOnError("vkGetInstanceProcAddr failed to find vk" #entry);     \
  }
```

**Definition for `vkGetDeviceProcAddr`**:

```cpp
PFN_vkVoidFunction vkGetDeviceProcAddr(
  VkDevice    device,
  const char* pName);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#vkGetDeviceProcAddr) for `vkGetDeviceProcAddr`**:

- `device` is the logical device that provides the function pointer.
- `pName` is the name of any Vulkan command whose first parameter is one of
  - `VkDevice`
  - `VkQueue`
  - `VkCommandBuffer`

**Usage for `vkGetDeviceProcAddr`**:

```cpp
#define GET_DEVICE_PROC_ADDR(dev, entry)                              \
  {                                                                   \
    fp##entry = (PFN_vk##entry)vkGetDeviceProcAddr(dev, "vk" #entry); \
    if (!fp##entry)                                                   \
      exitOnError("vkGetDeviceProcAddr failed to find vk" #entry);    \
  }
```

These are based on the macros from the GLFW Vulkan example [here](https://github.com/glfw/glfw/blob/master/tests/vulkan.c). The reason we're using macros is because it would be tedious to repeat this for every time we want a function pointer.

## Using the Macros

The reason we wrote the macros was to get function pointers, but I feel the need to explain what these function pointers are for. Basically, Vulkan has a concept like EGL. Vulkan provides us an interface between itself (the API) and our platform's windowing system. This is not visible to us. We instead work with abstractions over the systems Vulkan targets. The GPU, like with EGL, will be able to tell us extension support and capabilities of the system.

We'll be using the macros to verify the windowing system has support for surfaces. We'll also check for capabilities, supported formats, and presentation modes. You can learn more about the **Window System Integration / Interface (WSI)** [here](https://www.khronos.org/files/vulkan10-reference-guide.pdf) on page 9. We'll be asking for four different function pointers. Here's the piece of code we'll be using:

```cpp
GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceSupportKHR);
GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfacePresentModesKHR);
GET_DEVICE_PROC_ADDR(device, CreateSwapchainKHR);
GET_DEVICE_PROC_ADDR(device, DestroySwapchainKHR);
GET_DEVICE_PROC_ADDR(device, GetSwapchainImagesKHR);
GET_DEVICE_PROC_ADDR(device, AcquireNextImageKHR);
GET_DEVICE_PROC_ADDR(device, QueuePresentKHR);
```

We also need to add a few members of our `VulkanSwapchain` class:

```cpp
PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
    fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR
    fpGetPhysicalDeviceSurfacePresentModesKHR;
PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
PFN_vkQueuePresentKHR fpQueuePresentKHR;
```

These variables will be written to by our macros so we have the function pointers for future use. We'll make use of the function pointers later in this chapter.

## Platform Specific Code

Now, please visit your platform (or all platforms) to get the specific code. You'll need this to create a surface.
