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
if (result != VK_SUCCESS)
  exitOnError("Failed to get physical device surface capabilities");
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
if (result != VK_SUCCESS || presentModeCount < 1)
  exitOnError("Failed to get physical device present modes");
```

Now let's go ahead and call the function again with our `std::vector<VkPresentModeKHR>`:

```cpp
std::vector<VkPresentModeKHR> presentModes(presentModeCount);
result = fpGetPhysicalDeviceSurfacePresentModesKHR(
    physicalDevice, surface, &presentModeCount, presentModes.data());

if (result != VK_SUCCESS || presentModeCount < 1)
  exitOnError("Failed to get physical device present modes");
```

Now let's look at the available present modes. There are a few different types:

- `VK_PRESENT_MODE_IMMEDIATE_KHR` - Our engine **will not ever** wait for the vertical blanking interval. This *may* result in visible tearing if our frame misses the interval and is presented too late.
- `VK_PRESENT_MODE_MAILBOX_KHR` - Our engine waits for the next vertical blanking interval to update the image. If we render another image, the image waiting to be displayed is overwritten.
  - You can think of the underlying data structure as an array of images of length `1`.
- `VK_PRESENT_MODE_FIFO_KHR` - Our engine waits for the next vertical blanking interval to update the image. If we've missed an interval, we wait until the next one. We will append already rendered images to the pending presentation queue. We follow the "first in first out" (FIFO) philosophy as the name suggests. There **will not** be any visible tearing.
  - You can think of the underlying data structure as something similar to a [heap](https://en.wikipedia.org/wiki/Heap_(data_structure)).
- `VK_PRESENT_MODE_FIFO_RELAXED_KHR` - Our engine waits for the next vertical blanking interval to update the image. If we've missed the interval, we **do not** wait. We will append already rendered images to the pending presentation queue. We present as soon as possible. We follow the "first in first out" (FIFO) philosophy as the name suggests. This *may* result in tearing.
  - You can think of the underlying data structure as something similar to a [heap](https://en.wikipedia.org/wiki/Heap_(data_structure)).

If you do not care about tearing, you might want `VK_PRESENT_MODE_IMMEDIATE_KHR`. However, if you want a low-latency tear-less presentation mode, you would choose `VK_PRESENT_MODE_MAILBOX_KHR`.

# `VkSwapchainCreateInfoKHR`



# `vkCreateSwapchainKHR`