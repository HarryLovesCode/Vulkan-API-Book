# Working with Swap Chains (Not ready!)

You might have noticed that in a previous section, we made used this value: `VK_KHR_SWAPCHAIN_EXTENSION_NAME`. So, what *is* a swap chain? It is essentially an **array of images** ready to be presented. One use is frame rate control. Using two buffers is called **double buffering**. The GPU renders completely to a single frame and then displays it. Once it has finished drawing the first frame, it begins drawing the second frame. This occurs even if we're rendering above the rate we're supposed to. Let's assume we're rendering faster than the physical display can display our images. We would then wait and **flip** the second buffer onto the screen. By flipping the image onto the screen during the **vertical blanking interval**, we can write our data while the display is blank. When it refreshes, our image appears on the screen. Other techniques such as **triple buffering** exist.

# `initSwapchain`

For the next few sections, we're going to be focusing on writing this method in our `VulkanExample` class. It is defined simply as:

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

And then we can verify we were successful:

```cpp
assert(result == VK_SUCCESS);
```

# `fpGetSwapchainImagesKHR`

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

# `setImageLayout`

In the next few sections, we'll be writing a `setImageLayout` method.

```cpp
void VulkanExample::setImageLayout(VkCommandBuffer cmdBuffer, VkImage image,
                                   VkImageAspectFlags aspects,
                                   VkImageLayout oldLayout,
                                   VkImageLayout newLayout) {}
```

This will take a `VkCommandBuffer` and a `VkImage` whose image layout we want to set. While it's not necessary to build out this method, it will be useful later on. We'll also take in two `VkImageLayout`s.

# `VkImageMemoryBarrier`

In Vulkan, we have a new concept called barriers. They make sure our operations done on the GPU occur in a particular order which assure we get the expected result. A barrier separates two operations in a queue: before the barrier and after the barrier. Work done before the barrier will always finish before it can be used again. You can find more information [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#synchronization-image-memory-barrier). The definition for `VkImageMemoryBarrier` looks like:

```cpp
typedef struct VkImageMemoryBarrier {
  VkStructureType            sType;
  const void*                pNext;
  VkAccessFlags              srcAccessMask;
  VkAccessFlags              dstAccessMask;
  VkImageLayout              oldLayout;
  VkImageLayout              newLayout;
  uint32_t                   srcQueueFamilyIndex;
  uint32_t                   dstQueueFamilyIndex;
  VkImage                    image;
  VkImageSubresourceRange    subresourceRange;
} VkImageMemoryBarrier;
```

Valid usage in our `setImageLayout` would look like:

```cpp
VkImageMemoryBarrier imageBarrier = {};
imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
imageBarrier.pNext = NULL;
imageBarrier.oldLayout = oldLayout;
imageBarrier.newLayout = newLayout;
imageBarrier.image = image;
imageBarrier.subresourceRange.aspectMask = aspects;
imageBarrier.subresourceRange.baseMipLevel = 0;
imageBarrier.subresourceRange.levelCount = 1;
imageBarrier.subresourceRange.layerCount = 1;
```

Notice we left our two parts: `srcAccessMask` and `dstAccessMask`. Depending on the values `oldLayout` and `newLayout` take, we'll change how we set up our `VkImageMemoryBarrier`. Here is the way I handle the two layouts:

```cpp
switch (oldLayout) {
  case VK_IMAGE_LAYOUT_PREINITIALIZED:
    imageBarrier.srcAccessMask =
        VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
    break;
  case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
    imageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    break;
  case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    imageBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    break;
  case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    break;
  case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    imageBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    break;
}

switch (newLayout) {
  case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
    imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    break;
  case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    imageBarrier.srcAccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    break;
  case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
    imageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    break;
  case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
    imageBarrier.dstAccessMask |=
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    break;
  case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
    imageBarrier.srcAccessMask =
        VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    break;
}
```

While there is a `VK_IMAGE_LAYOUT_GENERAL` that will work in all cases, it's not always optimal. We have different layouts for:

- Color attachments (framebuffer)
- Depth stencils attachments (framebuffer)
- Shader reading via sampling

Images will start as `VK_IMAGE_LAYOUT_UNDEFINED` or `VK_IMAGE_LAYOUT_PREINITIALIZED` depending on which you pick. Note that moving from `VK_IMAGE_LAYOUT_UNDEFINED` to another layout may not preserve the existing data. However, moving from `VK_IMAGE_LAYOUT_PREINITIALIZED` to another gurantees the data is preserved. The documentation says that any layout can be used for `oldLayout` while `newLayout` cannot use `VK_IMAGE_LAYOUT_UNDEFINED` or `VK_IMAGE_LAYOUT_PREINITIALIZED`. You can find the documentation I'm reading from [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#synchronization-image-memory-barrier).

# `vkCmdPipelineBarrier`

Before we can finish our `setImageLayout` method, we need to call `vkCmdPipelineBarrier`. This will record the command and insert our execution dependencies and memory dependencies between two sets of commands. You can find the documentation [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#vkCmdPipelineBarrier). The definition looks like:

```cpp
void vkCmdPipelineBarrier(
  // The command buffer into which the command is recorded.
  VkCommandBuffer               commandBuffer,
  // A bitmask of VkPipelineStageFlagBits specifying a set 
  // of source pipeline stages
  VkPipelineStageFlags          srcStageMask,
  // A bitmask specifying a set of destination pipeline stages.
  // Specifies an execution dependency such that all work 
  // performed by the set of pipeline stages included in 
  // srcStageMask of the first set of commands completes before
  // any work performed by the set of pipeline stages included
  // in dstStageMask of the second set of commands begins.
  VkPipelineStageFlags          dstStageMask,
  // A bitmask of VkDependencyFlagBits. The execution dependency 
  // is by-region if the mask includes VK_DEPENDENCY_BY_REGION_BIT.
  VkDependencyFlags             dependencyFlags,
  // The length of the pMemoryBarriers array.
  uint32_t                      memoryBarrierCount,
  // A pointer to an array of VkMemoryBarrier structures.
  const VkMemoryBarrier*        pMemoryBarriers,
  // The length of the pBufferMemoryBarriers array.
  uint32_t                      bufferMemoryBarrierCount,
  // A pointer to an array of VkBufferMemoryBarrier structures.
  const VkBufferMemoryBarrier*  pBufferMemoryBarriers,
  // The length of the pImageMemoryBarriers array.
  uint32_t                      imageMemoryBarrierCount,
  // A pointer to an array of VkImageMemoryBarrier structures.
  const VkImageMemoryBarrier*   pImageMemoryBarriers);
```

The only arguments we're not sure about are `srcFlags` and `dstFlags`. We want our execution / memory dependencies to be staged at the top of the command buffer. Thus, we'll use `VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT`. You can find more information on pipeline state flags like `VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT` [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#synchronization-pipeline-stage-flags). Our usage would look like:

```cpp
VkPipelineStageFlagBits srcFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
VkPipelineStageFlagBits dstFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
vkCmdPipelineBarrier(cmdBuffer, srcFlags, dstFlags, 0, 0, NULL, 0, NULL, 1,
                     &imageBarrier);
```

# `VkImageViewCreateInfo`

For the next few sections, we'll be back in our `initSwapchain` method. We'll start with our `for` loop where we will be setting image layouts and creating image views. It will look like this:

```cpp
for (uint32_t i = 0; i < imageCount; i++) {
  // We'll be filling this out
}
```

Now, in Vulkan, we don't directly access images via shaders for reading and writing. We make use of image views which represent subresources of the images and their metadata to do the same. You can find documentation [here](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkImageViewCreateInfo.html) and the definition is included below:

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

Notice I did not finish the `components` line. You can see this [section](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#VkComponentMapping) for more information on component mapping. But, what we're doing is telling Vulkan how to map image components to vector components output by our shaders. We'll simply tell Vulkan we want our `(R, G, B, A)` images to map to a vector in the form `<R, G, B, A>`. We can do that like so:

```cpp
imageCreateInfo.components = {
    VK_COMPONENT_SWIZZLE_R, 
    VK_COMPONENT_SWIZZLE_G, 
    VK_COMPONENT_SWIZZLE_B,
    VK_COMPONENT_SWIZZLE_A};
```

Unless you know what you're doing, just use `RGBA` values. Before we create the image, let's make sure we store it and call the `setImageLayout` method with it. We'll also finish filling out the `imageCreateInfo`:

```cpp
buffers[i].image = images[i];
setImageLayout(cmdBuffer, buffers[i].image, VK_IMAGE_ASPECT_COLOR_BIT,
               VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
imageCreateInfo.image = buffers[i].image;
```

# `vkCreateImageView`

To create our image view, we simply call `vkCreateImageView`. You can find documentation [here](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCreateImageView.html) and the definition is included below:

```cpp
VkResult vkCreateImageView(
  // Logical device which owns the image.
  VkDevice                     device,
  // Specifies properties of the new view.
  const VkImageViewCreateInfo* pCreateInfo,
  // A pointer to our VkImageViewCreateInfo structure.
  const VkAllocationCallbacks* pAllocator,
  // Returns the requested object.
  VkImageView*                 pView);
  device
```

Here is what it looks like in use with error checking:

```cpp
result =
    vkCreateImageView(device, &imageCreateInfo, NULL, &buffers[i].view);
assert(result == VK_SUCCESS);
```

# `VkFramebufferCreateInfo`

We're going to be creating framebuffers for every image in the swapchain. Thus, we'll continue writing the body of our `for` loop. Like most types in Vulkan, we'll need to create an info structure before we can create a framebuffer. This will be `VkFramebufferCreateInfo`. You can find the documentation [here](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkFramebufferCreateInfo.html) and the definition below:

```cpp
typedef struct VkFramebufferCreateInfo {
  VkStructureType             sType;
  const void*                 pNext;
  VkFramebufferCreateFlags    flags;
  VkRenderPass                renderPass;
  uint32_t                    attachmentCount;
  const VkImageView*          pAttachments;
  uint32_t                    width;
  uint32_t                    height;
  uint32_t                    layers;
} VkFramebufferCreateInfo;
```

1. We'll fill out `sType` normally.
2. We'll specify we only need `1` attachment which is our `VkImageView`.
3. We'll set the width and height to that of the `swapchainExtent` from earlier:

```cpp
VkFramebufferCreateInfo fbCreateInfo = {};
fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
fbCreateInfo.attachmentCount = 1;
fbCreateInfo.pAttachments = &buffers[i].view;
fbCreateInfo.width = swapchainExtent.width;
fbCreateInfo.height = swapchainExtent.height;
fbCreateInfo.layers = 1;
```

# `vkCreateFramebuffer`

Now we can create each framebuffer. We'll call `vkCreateFramebuffer`. You can find more information [here](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/vkCreateFramebuffer.html) and the definition is included below:

```cpp
VkResult vkCreateFramebuffer(
  // The device with which to create the framebuffer object.
  VkDevice                       device,
  // A pointer to a structure containing information about 
  // how to create the object.
  const VkFramebufferCreateInfo* pCreateInfo,
  // A pointer to our VkFramebufferCreateInfo structure.
  const VkAllocationCallbacks*   pAllocator,
  // A pointer to a variable which will receive the handle to the new object.
  VkFramebuffer*                 pFramebuffer);
```

Let's look at valid usage and error checking:

```cpp
result = vkCreateFramebuffer(device, &fbCreateInfo, NULL,
                             &buffers[i].frameBuffer);
assert(result == VK_SUCCESS);
```

That's all we'll need for our `initSwapchain` method!

# `fpAcquireNextImageKHR`

For this section, we'll be writing a small helper method. The definition looks like this:

```cpp
void getSwapchainNext(VkSemaphore presentCompleteSemaphore, uint32_t buffer) {}
```

We already have the function pointer from earlier called `fpAcquireNextImageKHR`. You can find the documentation [here](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#vkAcquireNextImageKHR) and this has the same definition as:

```cpp
VkResult vkAcquireNextImageKHR(
  // The device assocated with swapchain.
  VkDevice       device,
  // The swapchain from which an image is being acquired.
  VkSwapchainKHR swapchain,
  // Indicates how long the function waits, in nanoseconds, 
  // if no image is available.
  uint64_t       timeout,
  // VK_NULL_HANDLE or a semaphore to signal.
  VkSemaphore    semaphore,
  // VK_NULL_HANDLE or a fence to signal.
  VkFence        fence,
  // A pointer to a uint32_t that is set to the index of the 
  // next image to use (i.e. an index into the array of images 
  // returned by vkGetSwapchainImagesKHR).
  uint32_t*      pImageIndex);
```

Now that we know that, all we need to do is call the function and make sure we were successful:

```cpp
VkResult result =
    fpAcquireNextImageKHR(device, swapchain, UINT64_MAX,
                          presentCompleteSemaphore, (VkFence)0, &buffer);
assert(result == VK_SUCCESS);
```

# `swapchainPresent`

For the next two sections, we'll be writing the body of this method:

```cpp
void swapchainPresent(VkCommandBuffer cmdBuffer, VkQueue queue,
                      uint32_t buffer) {}
```

# Presenting Images

In order for the swapchain to present images, we'll have to inform Vulkan of some things. We can use `VkPresentInfoKHR` to do this. You can find documentation [here](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#VkPresentInfoKHR) and the definition is below:

```cpp
typedef struct VkPresentInfoKHR {
  // The type of this structure and must be 
  // VK_STRUCTURE_TYPE_PRESENT_INFO_KHR.
  VkStructureType          sType;
  // NULL or a pointer to an extension-specific structure.
  const void*              pNext;
  // The number of semaphores to wait for before issuing the 
  // present request. The number may be zero.
  uint32_t                 waitSemaphoreCount;
  // If non-NULL, is an array of VkSemaphore objects with 
  // waitSemaphoreCount entries, and specifies the semaphores to wait 
  // for before issuing the present request.
  const VkSemaphore*       pWaitSemaphores;
  // The number of swapchains being presented to by this command.
  uint32_t                 swapchainCount;
  // An array of VkSwapchainKHR objects with swapchainCount entries. 
  // A given swapchain must not appear in this list more than once.
  const VkSwapchainKHR*    pSwapchains;
  // An array of indices into the array of each swapchain’s 
  // presentable images, with swapchainCount entries. Each entry in 
  // this array identifies the image to present on the corresponding 
  // entry in the pSwapchains array.
  const uint32_t*          pImageIndices;
  // An array of VkResult typed elements with swapchainCount entries. 
  // Applications that don’t need per-swapchain results can use NULL 
  // for pResults. If non-NULL, each entry in pResults will be set to 
  // the VkResult for presenting the swapchain corresponding to the same 
  // index in pSwapchains.
  VkResult*                pResults;
} VkPresentInfoKHR;
```

Our usage will simply look like:

```cpp
VkPresentInfoKHR presentInfo = {};
presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
presentInfo.pNext = NULL;
presentInfo.swapchainCount = 1;
presentInfo.pSwapchains = &swapchain;
presentInfo.pImageIndices = &buffer;
```

# `fpQueuePresentKHR`

As the last part of the `swapchainPresent` method, we actually get to present! We'll be using the function pointer from earlier called `fpQueuePresentKHR`. The documentation is [here](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#vkQueuePresentKHR) and the definition is the same as:

```cpp
VkResult vkQueuePresentKHR(
  // A queue that is capable of presentation to the target 
  // surface’s platform on the same device as the image’s swapchain.
  VkQueue                 queue,
  // A pointer to an instance of the VkPresentInfoKHR structure 
  // specifying the parameters of the presentation.
  const VkPresentInfoKHR* pPresentInfo);
```

# Cleaning Up

For our new destructor, we will:

- Destroy all image views we stored
- Destroy the swapchain
- Destroy the surface
- Destroy the instance

That can be done easily with the following lines of code:

```cpp
for (SwapChainBuffer buffer : buffers)
  vkDestroyImageView(device, buffer.view, NULL);

fpDestroySwapchainKHR(device, swapchain, NULL);
vkDestroySurfaceKHR(instance, surface, NULL);
vkDestroyInstance(instance, NULL);
```
