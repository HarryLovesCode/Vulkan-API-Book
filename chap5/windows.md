# Windows

We're going to be writing the Windows specific code for getting a surface in this section.

# Extensions

You may remember a while back that when we created our instance, we enabled a few extensions. These were platform specific extensions for getting a surface. Because we did it already, there's no need to write that code now.

# Procedures

Vulkan doesn't directly expose functions for all platforms. Thus, we'll have to query Vulkan for them at run-time. We'll be getting the function pointers at the instance level and we'll be using the `vkGetInstanceProcAddr` method. You can find it in the documentation [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#vkGetInstanceProcAddr) and the definition below:

```cpp
PFN_vkVoidFunction vkGetInstanceProcAddr(
    VkInstance     instance,
    const char*    pName);
```

We can use two handy macros to do the heavy lifting for us:

```cpp
#define EXIT_ON_ERR(msg)                                                    \
do {                                                                        \
    fprintf(stderr, msg);                                                   \
    fflush(stdout);                                                         \
    exit(EXIT_FAILURE);                                                     \
} while (0)

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                            \
{                                                                           \
    this->fp##entrypoint =                                                  \
        (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint);  \
    if (!this->fp##entrypoint)                                              \
        EXIT_ON_ERR("vkGetInstanceProcAddr failed to find vk" #entrypoint); \
}
```

These are based on the macros from the GLFW Vulkan example [here](https://github.com/glfw/glfw/blob/master/tests/vulkan.c). The reason we're using macros is because it would be tedious to same process for every function pointer we want.

# Using the Macros

The reason we wrote the macros was to get function pointers, but I feel the need to explain what these function pointers are for. Basically, Vulkan has a concept like EGL. Vulkan provides us an interface between itself (the API) and our platform's windowing system. This, however, is not visible to us. We instead work with abstractions over the various systems Vulkan targets. The GPU, like with EGL, will be able to tell us extension support and capabilities of the system.

We'll be using the macros to verify the windowing system has support for surfaces. We'll also check for capabilities, supported formats, and presentation modes. You can learn more about the **Window System Integration / Interface (WSI)** [here](https://www.khronos.org/files/vulkan10-reference-guide.pdf) on page 9. We'll be asking for four different function pointers. Here's the piece of code we'll be using:

```cpp
GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceSupportKHR);
GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfacePresentModesKHR);
```

We'll insert that right after we call `initDevices` in our constructor. We also need to add a few members of our `VulkanExample` class:

```cpp
PFN_vkGetPhysicalDeviceSurfaceSupportKHR
    fpGetPhysicalDeviceSurfaceSupportKHR;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR 
    fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR 
    fpGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR 
    fpGetPhysicalDeviceSurfacePresentModesKHR;
```

These variables will be written to by our macros so we have the function pointers for future use.