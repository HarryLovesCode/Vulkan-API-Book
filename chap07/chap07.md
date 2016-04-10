# Image Layouts

In this chapter, we'll be writing a `setImageLayout` method. We'll make sure to come back to `initSwapchain` later.

```cpp
void setImageLayout(VkCommandBuffer cmdBuffer, VkImage image,
                    VkImageAspectFlags aspects,
                    VkImageLayout oldLayout,
                    VkImageLayout newLayout) {}
```

This will take a `VkCommandBuffer` and a `VkImage` whose image layout we want to set. While it's not necessary to build out this method, it will be useful later on. We'll also take in two `VkImageLayout`s.

## `VkImageMemoryBarrier`

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

## `vkCmdPipelineBarrier`

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

The only arguments we're not sure about are `srcFlags` and `dstFlags`. However, we know we want our execution / memory dependencies to be staged at the top of the command buffer. So, we'll use `VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT` to notify Vulkan of our intentions. You can find more information on pipeline state flags like `VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT` [here](https://www.khronos.org/registry/vulkan/specs/1.0/xhtml/vkspec.html#synchronization-pipeline-stage-flags). Our usage would look like:

```cpp
VkPipelineStageFlagBits srcFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
VkPipelineStageFlagBits dstFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
vkCmdPipelineBarrier(cmdBuffer, srcFlags, dstFlags, 0, 0, NULL, 0, NULL, 1,
                     &imageBarrier);
```

That's all for image layouts. Onto image views!
