# Acquiring, Presenting, Cleaning Up

For this chapter, we'll be focusing on:

- Acquiring the next image in the swapchain
- Presenting images
- Cleaning up when we're done with the swapchain

## `fpAcquireNextImageKHR`

For this section, we'll be writing a small helper method. The definition looks like this:

```cpp
void getSwapchainNext(VkSemaphore presentCompleteSemaphore, uint32_t buffer) {}
```

We already have the function pointer from earlier called `fpAcquireNextImageKHR`. This has the same definition as `vkAcquireNextImageKHR`.

**Definition for `vkAcquireNextImageKHR`**:

```cpp
VkResult vkAcquireNextImageKHR(
  VkDevice       device,
  VkSwapchainKHR swapchain,
  uint64_t       timeout,
  VkSemaphore    semaphore,
  VkFence        fence,
  uint32_t*      pImageIndex);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#vkAcquireNextImageKHR) for `vkAcquireNextImageKHR`**:

- `device` is the device assocated with swapchain.
- `swapchain` is the swapchain from which an image is being acquired.
- `timeout` indicates how long the function waits, in nanoseconds, if no image is available.
- `semaphore` is `VK_NULL_HANDLE` or a semaphore to signal.
- `fence` is `VK_NULL_HANDLE` or a fence to signal.
- `pImageIndex` is a pointer to a `uint32_t` that is set to the index of the next image to use (i.e. an index into the array of images returned by `vkGetSwapchainImagesKHR`).

**Usage for `vkAcquireNextImageKHR`**:

```cpp
VkResult result =
    fpAcquireNextImageKHR(device, swapchain, UINT64_MAX,
                          presentCompleteSemaphore, (VkFence)0, &buffer);
assert(result == VK_SUCCESS);
```

## `swapchainPresent`

For this section and the next, we'll be writing the body of this method:

```cpp
void swapchainPresent(VkCommandBuffer cmdBuffer, VkQueue queue,
                      uint32_t buffer) {}
```

In order for the swapchain to present images, we'll have to inform Vulkan of some things. We can use `VkPresentInfoKHR` to do this.

**Definition for `VkPresentInfoKHR`**:

```cpp
typedef struct VkPresentInfoKHR {
  VkStructureType          sType;
  const void*              pNext;
  uint32_t                 waitSemaphoreCount;
  const VkSemaphore*       pWaitSemaphores;
  uint32_t                 swapchainCount;
  const VkSwapchainKHR*    pSwapchains;
  const uint32_t*          pImageIndices;
  VkResult*                pResults;
} VkPresentInfoKHR;
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#VkPresentInfoKHR) for `VkPresentInfoKHR`**:

- `sType` is the type of this structure and must be `VK_STRUCTURE_TYPE_PRESENT_INFO_KHR`.
- `pNext` is `NULL` or a pointer to an extension-specific structure.
- `waitSemaphoreCount` is the number of semaphores to wait for before issuing the present request. The number may be zero.
- `pWaitSemaphores`, if non-`NUL`L, is an array of `VkSemaphore` objects with `waitSemaphoreCount` entries, and specifies the semaphores to wait for before issuing the present request.
- `swapchainCount` is the number of swapchains being presented to by this command.
- `pSwapchains` is an array of `VkSwapchainKHR` objects with `swapchainCount` entries. A given swapchain must not appear in this list more than once.
- `pImageIndices` is an array of indices into the array of each swapchain’s presentable images, with `swapchainCount` entries. Each entry in this array identifies the image to present on the corresponding entry in the `pSwapchains` array.
- `pResults` is an array of `VkResult` typed elements with `swapchainCount` entries. Applications that don’t need per-swapchain results can use `NULL` for pResults. If non-`NULL`, each entry in `pResults` will be set to the `VkResult` for presenting the swapchain corresponding to the same index in `pSwapchains`.

**Usage for `VkPresentInfoKHR`**:

```cpp
VkPresentInfoKHR presentInfo = {};
presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
presentInfo.pNext = NULL;
presentInfo.swapchainCount = 1;
presentInfo.pSwapchains = &swapchain;
presentInfo.pImageIndices = &buffer;
```

## `fpQueuePresentKHR`

As the last part of the `swapchainPresent` method, we actually get to present! We'll be using the function pointer from earlier called `fpQueuePresentKHR` which has the definition is the same as `vkQueuePresentKHR`.

**Definition for `vkQueuePresentKHR`**:

```cpp
VkResult vkQueuePresentKHR(
    VkQueue                 queue,
    const VkPresentInfoKHR* pPresentInfo);
```

**[Documentation](https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/xhtml/vkspec.html#vkQueuePresentKHR) for `vkQueuePresentKHR`**:

- `queue `is a queue that is capable of presentation to the target surface’s platform on the same device as the image’s swapchain.
- `pPresentInfo` is a pointer to an instance of the `VkPresentInfoKHR` structure specifying the parameters of the presentation.

## Cleaning Up

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
