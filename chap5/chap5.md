# Creating a Surface

This section is divided up by platform. You must begin with the sections below before clicking on platform. The code below is platform independent and must be included in order to create a surface

## Extensions

You may remember a while back that when we created our instance, we enabled a few extensions. These were platform specific extensions for surfaces. Because we did it already, there's no need to write that code now.

## Procedures

Vulkan doesn't directly expose functions for all platforms. Thus, we'll have to query Vulkan for them at run-time. We'll be getting the function pointers at the instance level and device level using the `vkGetInstanceProcAddr` and `vkGetDeviceProcAddr` methods. You can find more information in the documentation [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#vkGetInstanceProcAddr). The definitions look like:

```cpp
PFN_vkVoidFunction vkGetInstanceProcAddr(
    VkInstance  instance,
    const char* pName);
PFN_vkVoidFunction vkGetDeviceProcAddr(
    VkDevice    device,
    const char* pName);
```

We can use two handy macros to do the heavy lifting for us:

```cpp
#define GET_INSTANCE_PROC_ADDR(inst, entry)                              \
  {                                                                      \
    fp##entry = (PFN_vk##entry)vkGetInstanceProcAddr(inst, "vk" #entry); \
    if (!fp##entry)                                                      \
      exitOnError("vkGetInstanceProcAddr failed to find vk" #entry);     \
  }

#define GET_DEVICE_PROC_ADDR(dev, entry)                              \
  {                                                                   \
    fp##entry = (PFN_vk##entry)vkGetDeviceProcAddr(dev, "vk" #entry); \
    if (!fp##entry)                                                   \
      exitOnError("vkGetDeviceProcAddr failed to find vk" #entry);    \
  }
```

These are based on the macros from the GLFW Vulkan example [here](https://github.com/glfw/glfw/blob/master/tests/vulkan.c). The reason we're using macros is because it would be tedious to same process for every function pointer we want.

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

We'll insert that right after we call `initSurface` in our constructor. We also need to add a few members of our `VulkanExample` class:

```cpp
PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
PFN_vkQueuePresentKHR fpQueuePresentKHR;
```

These variables will be written to by our macros so we have the function pointers for future use. We'll make use of the function pointers later in this chapter.

## Platform Specific Code

Now, please visit your platform (or all platforms) to get the specific code. You'll need this to create a surface.

- [Windows](./chap5-windows.md)
- [Linux](./chap5-linux.md)

## Checking Graphics / Present Support

Next, we need to look for a device queue that allows for drawing images **and** presenting images. You can use two different queues, but we'll discuss that in a later chapter. We can use the `fpGetPhysicalDeviceSurfaceSupportKHR` function pointer from earlier. The definition is the same as `vkGetPhysicalDeviceSurfaceSupportKHR` which looks like this:

```cpp
VkResult vkGetPhysicalDeviceSurfaceSupportKHR(
  VkPhysicalDevice physicalDevice, 
  uint32_t queueFamilyIndex, 
  VkSurfaceKHR surface, 
  VkBool32* pSupported);
```

The usage would therefore look like this:

```cpp
std::vector<VkBool32> supportsPresenting(queueCount);

for (uint32_t i = 0; i < queueCount; i++) {
  fpGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportsPresenting[i]);
}
```

However, if we don't check for graphics and presenting support, this is a little pointless. To check if our queue supports graphics operations, we can check if the queue flag contains `VK_QUEUE_GRAPHICS_BIT`. You may or may not be aware that this can be done using the `&` operator. We can use the following syntax:

```cpp
if ((QUEUE_FLAG & VK_QUEUE_GRAPHICS_BIT) != 0)
  // We support graphics operations!
```

We'll also want to keep the index of the queue on hand for later. Make sure you have `uint32_t queueIndex;` defined somewhere in the class. Now, let's finish the body of the loop:

```cpp
if ((queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
  if (supportsPresenting[i] == VK_TRUE) {
    queueIndex = i;
    break;
  }
}
```

## Color Formats and Color Spaces

For rendering purposes, we'll need some information on the surface formats our device supports. Specifically, we're going to check right now for color support. Vulkan breaks up this into two categories: color formats and color spaces. Color formats can describe the number of components, size of components, compression types, etc. In contrast, color spaces tells the Vulkan implementation how to interpret that data. For example, if we are telling Vulkan we have an RGBA image with 8 bits for each component (`VK_FORMAT_B8G8R8A8_*`), it would need to understand what `0.0` and `1.0` mean. A color space describes the range of colors or gamut if you prefer.

A common and well supported color format in Vulkan is `VK_FORMAT_B8G8R8A8_UNORM`. You can find documentation on available formats [here](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkFormat.html). Note, that page is a stub and lacks full documentation right now. We can use `fpGetPhysicalDeviceSurfaceFormatsKHR` to **directly** ask the device what formats it supports. You can find the documentation [here](https://www.khronos.org/registry/vulkan/specs/1.0/refguide/Vulkan-1.0-web.pdf) on page 9. The definition is the same as `vkGetPhysicalDeviceSurfaceFormatsKHR` which looks like this:

```cpp
VkResult vkGetPhysicalDeviceSurfaceFormatsKHR(
  VkPhysicalDevice     physicalDevice, 
  VkSurfaceKHR         surface, 
  uint32_t*            pSurfaceFormatCount, 
  VkSurfaceFormatKHR*  pSurfaceFormats);
```

We will provide `NULL` as the last argument to get the number of supported formats. Valid usage would look like this:

```cpp
uint32_t formatCount = 0;
result = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
                                              &formatCount, NULL);
```

We should verify that we were successful and that the device supports at least one format:

```cpp
if (result != VK_SUCCESS || formatCount < 1)
  exitOnError("Failed to get device surface formats.");
```

Now we can get the actual formats:

```cpp
std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
result = fpGetPhysicalDeviceSurfaceFormatsKHR(
    physicalDevice, surface, &formatCount, surfaceFormats.data());
```

As you've seen in previous chapters, I prefer to verify success each time we call a method that returns a `VkResult`. We can make the same check as before:

```cpp
if (result != VK_SUCCESS || formatCount < 1)
  exitOnError("Failed to get device surface formats.");
```

Now that we've found the formats our device supports, we can go ahead and set them. Let's add two variables to our `VulkanExample` class:

```cpp
VkFormat colorFormat;
VkColorSpaceKHR colorSpace;
```

Here is the process we should follow when choosing a color format and color space:

- Check if we support **only** one format and that format is `VK_FORMAT_UNDEFINED`
  - Yes: We should set our own format because the device does not default to any
  - No: Take the first color format we support.
- Take the first color space that's available

The code would look like this:

```cpp
if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
  colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
else
  colorFormat = surfaceFormats[0].format;

colorSpace = surfaceFormats[0].colorSpace;
```