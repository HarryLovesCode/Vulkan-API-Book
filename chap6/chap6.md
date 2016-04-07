# Working with Swap Chains (Not ready!)

You might have noticed that in a previous section, we made used this value: `VK_KHR_SWAPCHAIN_EXTENSION_NAME`. So, what *is* a swap chain? It is essentially an **array of images** ready to be presented. One use is frame rate control. Using two buffers is called **double buffering**. The GPU renders completely to a single frame and then displays it. Once it has finished drawing the first frame, it begins drawing the second frame. This occurs even if we're rendering above the rate we're supposed to. Let's assume we're rendering faster than the physical display can display our images. We would then wait and **flip** the second buffer onto the screen. By flipping the image onto the screen during the **vertical blanking interval**, we can write our data while the display is blank. When it refreshes, our image appears on the screen. Other techniques such as **triple buffering** exist.

# `initSwapchain`

For this section, we're going to be focusing on writing this method in our `VulkanExample` class. It is defined simply as:

```cpp
void VulkanExample::initSwapchain() {}
```

# `fpGetPhysicalDeviceSurfaceCapabilitiesKHR`

We created a surface in the last chapter. Now we need to check the surface resolution so we can later inform our swapchain. To get the resolution of the surface, we'll have to ask it for its capabilities. You can find documentation [here](https://www.khronos.org/files/vulkan10-reference-guide.pdf) in section **29.5**. We'll be using `fpGetPhysicalDeviceSurfaceCapabilitiesKHR` which has a definition like:

```cpp
VkResult vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
  VkPhysicalDevice           physicalDevice,
  VkSurfaceKHR               surface,
  VkSurfaceCapabilitiesKHR*  pSurfaceCapabilities);
```

Note that it takes a pointer to a `VkSurfaceCapabilitiesKHR` object. We'll have to create that to pass it in to be written:

```cpp
VkSurfaceCapabilitiesKHR caps = {};
```

Now let's get the information we need:

```cpp
VkSurfaceCapabilitiesKHR caps = {};
VkResult result = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(
    physicalDevice, surface, &caps);
```

Per usual, let's verify we were successful:

```cpp
assert(result == VK_SUCCESS);
```

# `VkExtent2D`

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

# `fpGetPhysicalDeviceSurfacePresentModesKHR`

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
assert(result == VK_SUCCESS && presentModeCount >= 1);
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

# `VkSwapchainCreateInfoKHR`

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

If we asked for more images that are supported, we should just go with the maximum:

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

# `vkCreateSwapchainKHR`

You can find documentation on this function [here](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCreateSwapchainKHR.html). The definition looks like this:

```cpp
VkResult vkCreateSwapchainKHR(
  VkDevice                                    device,
  const VkSwapchainCreateInfoKHR*             pCreateInfo,
  const VkAllocationCallbacks*                pAllocator,
  VkSwapchainKHR*                             pSwapchain);
```

Let's call the method:

```cpp
result = fpCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapchain);
```

and verify we were successful:

```cpp
assert(result == VK_SUCCESS);
```

# `fpGetSwapchainImagesKHR`

We will need to get the available images from the swapchain. In a later section of this chapter, we'll actually get them ready for use, but right now, let's focus on this part. We'll be using a function pointer we got earlier called ``. You can find documentation [here](https://www.khronos.org/files/vulkan10-reference-guide.pdf) in **29.6** and the definition is the same as:

```cpp
VkResult vkGetSwapchainImagesKHR(
  VkDevice device, 
  VkSwapchainKHR swapchain, 
  uint32_t* pSwapchainImageCount, 
  VkImage* pSwapchainImages);
```

Let's go ahead and make use of the function and check if we were successful:

```cpp
result = fpGetSwapchainImagesKHR(device, swapchain, &imageCount, NULL);
assert(result == VK_SUCCESS && imageCount > 0);
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

And of course, we'll call the function again:

```cpp
result =
    fpGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());
assert(result == VK_SUCCESS);
```

# `VkImageViewCreateInfo`

In Vulkan, we don't directly access images via shaders for reading and writing. We make use of image views which represent subresources of the images and their metadata to do the same. You can find documentation [here](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkImageViewCreateInfo.html) and the definition is included below:

```cpp
typedef struct VkImageViewCreateInfo {
  VkStructureType            sType;
  const void*                pNext;
  VkImageViewCreateFlags     flags;
  VkImage                    image;
  VkImageViewType            viewType;
  VkFormat                   format;
  VkComponentMapping         components;
  VkImageSubresourceRange    subresourceRange;
} VkImageViewCreateInfo;
```

There is a pretty typical way of setting up your `VkImageViewCreateInfo`. Let's look at my usage:

```cpp
VkImageViewCreateInfo imageCreateInfo = {};
imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
imageCreateInfo.pNext = NULL;
imageCreateInfo.format = colorFormat;
imageCreateInfo.components = {}; // We'll get to this in a second!
imageCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
imageCreateInfo.subresourceRange.baseMipLevel = 0;
imageCreateInfo.subresourceRange.levelCount = 1;
imageCreateInfo.subresourceRange.baseArrayLayer = 0;
imageCreateInfo.subresourceRange.layerCount = 1;
imageCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
imageCreateInfo.flags = 0;
```

Notice I did not finish the `components` line. You can see this [section](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkComponentMapping) for more information on component mapping. But, essentially what we're doing is telling Vulkan how to map components of the image to components of the vector output by our shaders. We'll simply tell Vulkan we want our `(R, G, B, A)` images to map to a vector in the form `<R, G, B, A>`. We can do that like so:

```cpp
imageCreateInfo.components = {
    VK_COMPONENT_SWIZZLE_R, 
    VK_COMPONENT_SWIZZLE_G, 
    VK_COMPONENT_SWIZZLE_B,
    VK_COMPONENT_SWIZZLE_A};
```

Unless you know what you're doing, just use `RGBA` values.

# `VkImageMemoryBarrier`



# `setImageLayout`

For this section, we're going to be writing the contents of this method:

```cpp
void VulkanExample::setImageLayout(VkCommandBuffer cmdBuffer, VkImage image,
                                   VkImageAspectFlags aspects,
                                   VkImageLayout oldLayout,
                                   VkImageLayout newLayout) {}
```

This will take a `VkCommandBuffer` and a `VkImage` whose image layout we want to set. While it's not necessary to build out a method, it may be useful later on. We'll also take in two `VkImageLayout`s. Depending on the values `oldLayout` and `newLayout` take, we'll change how we set up our `VkImageMemoryBarrier`.

# `vkCreateImageView`

You can find documentation [here](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCreateImageView.html).

# Acquiring the Next Image

# Presenting Images

# Cleaning Up
