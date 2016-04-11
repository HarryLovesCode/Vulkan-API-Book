# Getting Started With Swapchains

You might have noticed that in a previous section, we made used this value: `VK_KHR_SWAPCHAIN_EXTENSION_NAME`. So, what *is* a swap chain? It is essentially an **array of images** ready to be presented. One use is frame rate control. Using two buffers is called **double buffering**. The GPU renders completely to a single frame and then displays it. Once it has finished drawing the first frame, it begins drawing the second frame. This occurs even if we're rendering above the rate we're supposed to. Let's assume we're rendering faster than the physical display can display our images. We would then wait and **flip** the second buffer onto the screen. By flipping the image onto the screen during the **vertical blanking interval**, we can write our data while the display is blank. When it refreshes, our image appears on the screen. Other techniques such as **triple buffering** exist.

## `initSwapchain`

For the next few sections, we're going to be focusing on writing this method in our `VulkanExample` class. It is defined simply as:

```cpp
void VulkanExample::initSwapchain() {}
```

## `fpGetPhysicalDeviceSurfaceCapabilitiesKHR`

We created a surface in the last chapter. Now we need to check the surface resolution so we can later inform our swapchain. To get the resolution of the surface, we'll have to ask it for its capabilities. We'll be using `vkGetPhysicalDeviceSurfaceCapabilitiesKHR` which has the same definition as `fpGetPhysicalDeviceSurfaceCapabilitiesKHR`.

**Usage for `vkGetPhysicalDeviceSurfaceCapabilitiesKHR`**:

```cpp
VkResult vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
  VkPhysicalDevice           physicalDevice,
  VkSurfaceKHR               surface,
  VkSurfaceCapabilitiesKHR*  pSurfaceCapabilities);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#vkGetPhysicalDeviceSurfaceCapabilitiesKHR) for `vkGetPhysicalDeviceSurfaceCapabilitiesKHR**:

- `physicalDevice` is the physical device that will be associated with the swapchain to be created, as described for `vkCreateSwapchainKHR`.
- `surface` is the surface that will be associated with the swapchain.
- `pSurfaceCapabilities` is a pointer to an instance of the `VkSurfaceCapabilitiesKHR` structure that will be filled with information.

**Usage for `vkGetPhysicalDeviceSurfaceCapabilitiesKHR`**:

Note that it takes a pointer to a `VkSurfaceCapabilitiesKHR` object. We'll have to create that to pass it in. Let's do that and verify we were successful:

```cpp
VkSurfaceCapabilitiesKHR caps = {};
VkResult result = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(
    physicalDevice, surface, &caps);
assert(result == VK_SUCCESS);
```

## `VkExtent2D`

Windows uses `RECT` to define properties of rectangles. Similarly, Vulkan uses `VkExtent2D` for the same purpose. The `VkExtent2D` object will be used later, but for now, let's check something. The `caps` variable has a `currentExtent` field. This informs us about the surface's size. In the case that either the `width` or `height` are `-1`, we'll need to set the extent size ourselves. Otherwise, we can just use `caps.currentExtent` for the swapchain. Let's see how this looks in code:

```cpp
VkExtent2D swapchainExtent = {};

if (caps.currentExtent.width == -1 || caps.currentExtent.height == -1) {
  swapchainExtent.width = windowWidth;
  swapchainExtent.height = windowHeight;
} else {
  swapchainExtent = caps.currentExtent;
}
```

## `fpGetPhysicalDeviceSurfacePresentModesKHR`

In Vulkan, there are multiple ways images can be presented. We'll talk about the options later in this section, but for now, we need to figure out which are supported. We can use `fpGetPhysicalDeviceSurfacePresentModesKHR` to get the present modes as the name suggests. The definition is the same as `vkGetPhysicalDeviceSurfacePresentModesKHR`.

**Definition for `vkGetPhysicalDeviceSurfacePresentModesKHR`**:

```cpp
VkResult vkGetPhysicalDeviceSurfacePresentModesKHR(
  VkPhysicalDevice   physicalDevice,
  VkSurfaceKHR       surface,
  uint32_t*          pPresentModeCount,
  VkPresentModeKHR*  pPresentModes);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#vkGetPhysicalDeviceSurfacePresentModesKHR) for `vkGetPhysicalDeviceSurfacePresentModesKHR`**:

- `physicalDevice` is the physical device that will be associated with the swapchain to be created, as described for `vkCreateSwapchainKHR`.
- `surface` is the surface that will be associated with the swapchain.
- `pPresentModeCount` is a pointer to an integer related to the number of format pairs available or queried, as described below.
- `pPresentModes` is either `NULL` or a pointer to an array of `VkPresentModeKHR` structures.

**Usage for `vkGetPhysicalDeviceSurfacePresentModesKHR`**:

```cpp
uint32_t presentModeCount = 0;
result = fpGetPhysicalDeviceSurfacePresentModesKHR(
    physicalDevice, surface, &presentModeCount, NULL);
```

Before we move on, let's verify success. We should check the `result` and check make sure one or more present modes are available.

```cpp
assert(result == VK_SUCCESS);
assert(presentModeCount >= 1);
```

Now let's go ahead and call the function again with our `vector`:

```cpp
std::vector<VkPresentModeKHR> presentModes(presentModeCount);
result = fpGetPhysicalDeviceSurfacePresentModesKHR(
    physicalDevice, surface, &presentModeCount, presentModes.data());

assert(result == VK_SUCCESS);
```

Now we should take a look at the available present modes. There are a few different types:

- `VK_PRESENT_MODE_IMMEDIATE_KHR` - Our engine **will not ever** wait for the vertical blanking interval. This *may* result in visible tearing if our frame misses the interval and is presented too late.
- `VK_PRESENT_MODE_MAILBOX_KHR` - Our engine waits for the next vertical blanking interval to update the image. If we render another image, the image waiting to be displayed is overwritten.
  - You can think of the underlying data structure as an array of images of length `1`.
- `VK_PRESENT_MODE_FIFO_KHR` - Our engine waits for the next vertical blanking interval to update the image. If we've missed an interval, we wait until the next one. We will append already rendered images to the pending presentation queue. We follow the "first in first out" (FIFO) philosophy as the name suggests. There **will not** be any visible tearing.
  - You can think of the underlying data structure as something similar to a [heap](https://en.wikipedia.org/wiki/Heap_(data_structure)).
- `VK_PRESENT_MODE_FIFO_RELAXED_KHR` - Our engine waits for the next vertical blanking interval to update the image. If we've missed the interval, we **do not** wait. We will append already rendered images to the pending presentation queue. We present as soon as possible. We follow the "first in first out" (FIFO) philosophy as the name suggests. This *may* result in tearing.
  - You can think of the underlying data structure as something similar to a [heap](https://en.wikipedia.org/wiki/Heap_(data_structure)).

If you do not care about tearing, you might want `VK_PRESENT_MODE_IMMEDIATE_KHR`. However, if you want a low-latency tear-less presentation mode, you would choose `VK_PRESENT_MODE_MAILBOX_KHR`. Now let's look through our present modes and see if we can find `VK_PRESENT_MODE_MAILBOX_KHR`. If we find it, stop looking. Otherwise, if we see our next preference `VK_PRESENT_MODE_IMMEDIATE_KHR`, keep that. Finally, if we didn't find anything, we should continue with our default `VK_PRESENT_MODE_FIFO_KHR`. The code would look like this:

```cpp
VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

for (uint32_t i = 0; i < presentModeCount; i++) {
  if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
    presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    break;
  }

  if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
    presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
}
```

## `VkSwapchainCreateInfoKHR`

Next up, we're going to prepare the information needed to create our `VkSwapchainKHR`.

**Definition for `VkSwapchainCreateInfoKHR`**:

```cpp
typedef struct VkSwapchainCreateInfoKHR {
  VkStructureType sType;
  const void* pNext;
  VkSwapchainCreateFlagsKHR flags;
  VkSurfaceKHR surface;
  uint32_t minImageCount;
  VkFormat imageFormat;
  VkColorSpaceKHR imageColorSpace;
  VkExtent2D imageExtent;
  uint32_t imageArrayLayers;
  VkImageUsageFlags imageUsage;
  VkSharingMode imageSharingMode;
  uint32_t queueFamilyIndexCount;
  const uint32_t* pQueueFamilyIndices;
  VkSurfaceTransformFlagBitsKHR preTransform;
  VkCompositeAlphaFlagBitsKHR compositeAlpha;
  VkPresentModeKHR presentMode;
  VkBool32 clipped;
  VkSwapchainKHR oldSwapchain;
} VkSwapchainCreateInfoKHR
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#VkSwapchainCreateInfoKHR) for `VkSwapchainCreateInfoKHR`**:

- `sType` is the type of this structure and must be `VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR`.
- `pNext` is `NULL` or a pointer to an extension-specific structure.
- `flags` is reserved for future use, and must be zero.
- `surface` is the surface that the swapchain will present images to.
- `minImageCount` is the minimum number of presentable images that the application needs. The platform will either create the swapchain with at least that many images, or will fail to create the swapchain.
- `imageFormat` is a `VkFormat` that is valid for swapchains on the specified surface.
- `imageColorSpace` is a `VkColorSpaceKHR` that is valid for swapchains on the specified surface.
- `imageExtent` is the size (in pixels) of the swapchain. Behavior is platform-dependent when the image extent does not match the surface’s `currentExtent` as returned by `vkGetPhysicalDeviceSurfaceCapabilitiesKHR`.
- `imageArrayLayers` is the number of views in a multiview/stereo surface. For non-stereoscopic-3D applications, this value is `1`.
- `imageUsage` is a bitfield of `VkImageUsageFlagBits`, indicating how the application will use the swapchain’s presentable images.
- `imageSharingMode` is the sharing mode used for the images of the swapchain.
- `queueFamilyIndexCount` is the number of queue families having access to the images of the swapchain in case `imageSharingMode` is `VK_SHARING_MODE_CONCURRENT`.
- `pQueueFamilyIndices` is an array of queue family indices having access to the images of the swapchain in case `imageSharingMode` is `VK_SHARING_MODE_CONCURRENT`.
- `preTransform` is a bitfield of `VkSurfaceTransformFlagBitsKHR`, describing the transform, relative to the presentation engine’s natural orientation, applied to the image content prior to presentation. If it does not match the `currentTransform` value returned by `vkGetPhysicalDeviceSurfaceCapabilitiesKHR`, the presentation engine will transform the image content as part of the presentation operation.
- `compositeAlpha` is a bitfield of `VkCompositeAlphaFlagBitsKHR`, indicating the alpha compositing mode to use when this surface is composited together with other surfaces on certain window systems.
- `presentMode` is the presentation mode the swapchain will use. A swapchain’s present mode determines how incoming present requests will be processed and queued internally.
- `clipped` indicates whether the Vulkan implementation is allowed to discard rendering operations that affect regions of the surface which aren’t visible.

That's quite a definition. Before we can fill in the values, we'll need to figure out `minImageCount`. First let's check verify our surface supports images:

```cpp
assert(caps.maxImageCount >= 1);
```

Now let's say we want at least one image:

```cpp
uint32_t imageCount = caps.minImageCount + 1;
```

If we asked for more images than the implementation supports, we'll have to settle for less:

```cpp
if (imageCount > caps.maxImageCount)
  imageCount = caps.maxImageCount;
```

**Usage for `VkSwapchainCreateInfoKHR`**:

```cpp
VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
swapchainCreateInfo.surface = surface;
swapchainCreateInfo.minImageCount = imageCount;
swapchainCreateInfo.imageFormat = colorFormat;
swapchainCreateInfo.imageColorSpace = colorSpace;
swapchainCreateInfo.imageExtent = { swapchainExtent.width, swapchainExtent.height };
swapchainCreateInfo.imageArrayLayers = 1;
swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
swapchainCreateInfo.queueFamilyIndexCount = 1;
swapchainCreateInfo.pQueueFamilyIndices = { 0 };
swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
swapchainCreateInfo.presentMode = presentMode;
```

## `vkCreateSwapchainKHR`

This function will simply take in our `swapchainCreateInfo` and create the swapchain with that configuration.

**Definition for `vkCreateSwapchainKHR`**:

```cpp
VkResult vkCreateSwapchainKHR(
  VkDevice                                    device,
  const VkSwapchainCreateInfoKHR*             pCreateInfo,
  const VkAllocationCallbacks*                pAllocator,
  VkSwapchainKHR*                             pSwapchain);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCreateSwapchainKHR.html) for `vkCreateSwapchainKHR`**:

- `device` is the device to create the swapchain for.
- `pCreateInfo` is a pointer to an instance of the `VkSwapchainCreateInfoKHR` structure specifying the parameters of the created swapchain.
- `pAllocator` is the allocator used for host memory allocated for the swapchain object when there is no more specific allocator available.
- `pSwapchain` is a pointer to a `VkSwapchainKHR` handle in which the created swapchain object will be returned.

**Usage for `vkCreateSwapchainKHR`**:

```cpp
result = fpCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapchain);
assert(result == VK_SUCCESS);
```

## `fpGetSwapchainImagesKHR`

We will need to get the available images from the swapchain. In a later section of this chapter, we'll actually get them ready for use, but right now, let's focus on this part. We'll be using a function pointer we got earlier called `fpGetSwapchainImagesKHR`. The definition is the same as `vkGetSwapchainImagesKHR`.

**Definition for `vkGetSwapchainImagesKHR`**:

```cpp
VkResult vkGetSwapchainImagesKHR(
  VkDevice device,
  VkSwapchainKHR swapchain,
  uint32_t* pSwapchainImageCount,
  VkImage* pSwapchainImages);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#vkGetSwapchainImagesKHR) for `vkGetSwapchainImagesKHR`**:

- `device` is the device associated with swapchain.
- `swapchain` is the swapchain to query.
- `pSwapchainImageCount` is a pointer to an integer related to the number of format pairs available or queried, as described below.
- `pSwapchainImages` is either `NULL` or a pointer to an array of `VkSwapchainImageKHR` structures.

**Usage for `vkGetSwapchainImagesKHR`**:

```cpp
result = fpGetSwapchainImagesKHR(device, swapchain, &imageCount, NULL);
assert(result == VK_SUCCESS);
assert(imageCount > 0);
```

We'll need to make a new `struct` to contain a few things. Let's look at it:

```cpp
struct SwapChainBuffer {
  VkImage image;
  VkImageView view;
  VkFramebuffer frameBuffer;
};
```

This will become more relevant later on when we get closer to rendering. For now, we'll create two variables in the `VulkanExample` class. These will be:

```cpp
std::vector<VkImage> images;
std::vector<SwapChainBuffer> buffers;
```

Let's make sure we `resize` these to fit the number of images we'll get back:

```cpp
images.resize(imageCount);
buffers.resize(imageCount);
```

And of course, we'll call the function again and check for errors:

```cpp
result =
    fpGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());
assert(result == VK_SUCCESS);
```
