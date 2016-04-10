# Getting Started With Swapchains

You might have noticed that in a previous section, we made used this value: `VK_KHR_SWAPCHAIN_EXTENSION_NAME`. So, what *is* a swap chain? It is essentially an **array of images** ready to be presented. One use is frame rate control. Using two buffers is called **double buffering**. The GPU renders completely to a single frame and then displays it. Once it has finished drawing the first frame, it begins drawing the second frame. This occurs even if we're rendering above the rate we're supposed to. Let's assume we're rendering faster than the physical display can display our images. We would then wait and **flip** the second buffer onto the screen. By flipping the image onto the screen during the **vertical blanking interval**, we can write our data while the display is blank. When it refreshes, our image appears on the screen. Other techniques such as **triple buffering** exist.

## `initSwapchain`

For the next few sections, we're going to be focusing on writing this method in our `VulkanExample` class. It is defined simply as:

```cpp
void VulkanExample::initSwapchain() {}
```

## `fpGetPhysicalDeviceSurfaceCapabilitiesKHR`

We created a surface in the last chapter. Now we need to check the surface resolution so we can later inform our swapchain. To get the resolution of the surface, we'll have to ask it for its capabilities. You can find documentation [here](https://www.khronos.org/files/vulkan10-reference-guide.pdf) in section **29.5**. We'll be using `fpGetPhysicalDeviceSurfaceCapabilitiesKHR` which has a definition like:

```cpp
VkResult vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
  VkPhysicalDevice           physicalDevice,
  VkSurfaceKHR               surface,
  VkSurfaceCapabilitiesKHR*  pSurfaceCapabilities);
```

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

In Vulkan, there are multiple ways images can be presented. We'll talk about the options later in this section, but for now, we need to figure out which are supported. We can use `fpGetPhysicalDeviceSurfacePresentModesKHR` to get the present modes as the name suggests. You can find documentation [here](https://www.khronos.org/files/vulkan10-reference-guide.pdf) in section **29.5**. The definition is the same as:

```cpp
VkResult vkGetPhysicalDeviceSurfacePresentModesKHR(
  VkPhysicalDevice   physicalDevice,
  VkSurfaceKHR       surface,
  uint32_t*          pPresentModeCount,
  VkPresentModeKHR*  pPresentModes);
```

Like with other `vkGetX` methods, we should pass in `NULL` as the last argument to get `pPresentModeCount` before we allocate memory for `pPresentModes`. Let's look at how this is used:

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

Now let's go ahead and call the function again with our `std::vector<VkPresentModeKHR>`:

```cpp
std::vector<VkPresentModeKHR> presentModes(presentModeCount);
result = fpGetPhysicalDeviceSurfacePresentModesKHR(
    physicalDevice, surface, &presentModeCount, presentModes.data());

assert(result == VK_SUCCESS);
```

Now let's look at the available present modes. There are a few different types:

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

Next up, we're going to prepare the information needed to create our `VkSwapchainKHR`. You can find more information [here](https://www.khronos.org/files/vulkan10-reference-guide.pdf) in section **29.6**. The definition looks like this:

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

That's quite a definition. Before we can fill in the values, we'll need to prepare a number for `minImageCount`. First let's check verify our surface supports images:

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

Let's go ahead and fill in `VkSwapchainCreateInfoKHR`:

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

You can find documentation on this function [here](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCreateSwapchainKHR.html). The definition looks like this:

```cpp
VkResult vkCreateSwapchainKHR(
  VkDevice                                    device,
  const VkSwapchainCreateInfoKHR*             pCreateInfo,
  const VkAllocationCallbacks*                pAllocator,
  VkSwapchainKHR*                             pSwapchain);
```

Let's call the method and then we can verify we were successful::

```cpp
result = fpCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapchain);
assert(result == VK_SUCCESS);
```

## `fpGetSwapchainImagesKHR`

We will need to get the available images from the swapchain. In a later section of this chapter, we'll actually get them ready for use, but right now, let's focus on this part. We'll be using a function pointer we got earlier called ``. You can find documentation [here](https://www.khronos.org/files/vulkan10-reference-guide.pdf) in **29.6** and the definition is the same as:

```cpp
VkResult vkGetSwapchainImagesKHR(
  // The VkDevice associated with swapchain.
  VkDevice device,
  // The swapchain.
  VkSwapchainKHR swapchain,
  // The number of elements in the array pointed by pSwapchainImages.
  uint32_t* pSwapchainImageCount,
  // The returned array of images.
  VkImage* pSwapchainImages);
```

Let's go ahead and make use of the function and check if we were successful:

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
