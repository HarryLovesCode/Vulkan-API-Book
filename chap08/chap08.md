# Image Views

For this chapter, we'll be back in our `initSwapchain` method. We'll start with a `for` loop where we will be setting image layouts and creating image views. It will look like this:

```cpp
for (uint32_t i = 0; i < imageCount; i++) {
  // We'll be filling this out
}
```

## `VkImageViewCreateInfo`

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

## `vkCreateImageView`

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

## `VkFramebufferCreateInfo`

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

## `vkCreateFramebuffer`

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

That's all we'll need for our `initSwapchain` method! Now it's time for acquiring the next image in the swapchain and presenting images!
